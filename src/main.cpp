#include <windows.h>
#include <iostream>

#include "renderer.h"

bool running = true;

static HWND window;

LRESULT CALLBACK WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (msg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			PostQuitMessage(0);
			running = false;
		} break;
		
		case WM_SIZE: {
			RECT rect;
			GetClientRect(window, &rect);
			bufferWidth = rect.right - rect.left;
			bufferHeight = rect.bottom - rect.top;

			int bufferSize = bufferWidth * bufferHeight * sizeof(unsigned int);

			if (bufferMemory) VirtualFree(bufferMemory, 0, MEM_RELEASE);
			bufferMemory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			bufferBitmapInfo.bmiHeader.biSize = sizeof(bufferBitmapInfo.bmiHeader);
			bufferBitmapInfo.bmiHeader.biWidth = bufferWidth;
			bufferBitmapInfo.bmiHeader.biHeight = bufferHeight;
			bufferBitmapInfo.bmiHeader.biPlanes = 1;
			bufferBitmapInfo.bmiHeader.biBitCount = 32;
			bufferBitmapInfo.bmiHeader.biCompression = BI_RGB;
		} break;

		default: {
			result = DefWindowProcA(window, msg, wParam, lParam);
		}
	}

	return result;
}

bool CreateWindowContext(int width, int height, const char* title) {
	HINSTANCE instance = GetModuleHandleA(0);

	WNDCLASSA windowClass = {};
	windowClass.hInstance = instance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = title;
	windowClass.lpfnWndProc = WindowCallback;

	if (!RegisterClassA(&windowClass)) return false;

	int dwStyle = WS_OVERLAPPEDWINDOW;

	RECT windowRect = { 0, 0, width, height };
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	int adjustedWidth = windowRect.right - windowRect.left;
	int adjustedHeight = windowRect.bottom - windowRect.top;

	window = CreateWindowExA(NULL, title, title, dwStyle, 100, 100, adjustedWidth, adjustedHeight, NULL, NULL, instance, NULL);

	if (!window) return false;

	ShowWindow(window, SW_SHOW);

	return true;
}

void UpdateWindow() {
	MSG msg;

	while (PeekMessage(&msg, window, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	HDC hdc = GetDC(window);

	StretchDIBits(hdc, 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, bufferMemory, &bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(window, hdc);
}

int GetWindowWidth() {
	RECT windowRect;
	/*AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	GetWindowRect(window, &windowRect);*/
	GetClientRect(window, &windowRect);
	return windowRect.right - windowRect.left;
}

int GetWindowHeight() {
	RECT windowRect;
	/*AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	GetWindowRect(window, &windowRect);*/
	GetClientRect(window, &windowRect);
	return windowRect.bottom - windowRect.top;
}

bool GetCursorPixelCoordinates(int& x, int& y) {
	POINT cursorPos;
	if (GetCursorPos(&cursorPos)) {
		HDC hDC = GetDC(window);
		int dpiX = GetDeviceCaps(hDC, LOGPIXELSX);
		int dpiY = GetDeviceCaps(hDC, LOGPIXELSY);
		ReleaseDC(window, hDC);

		cursorPos.x = MulDiv(cursorPos.x, dpiX, 96);
		cursorPos.y = MulDiv(cursorPos.y, dpiY, 96);

		if (ScreenToClient(window, &cursorPos)) {
			x = cursorPos.x;
			y = GetWindowHeight() - cursorPos.y;
			return true;
		}
	}

	return false;
}

template <class T> void swap(T& a, T& b)
{
	T c(a); a = b; b = c;
}

void line(int x0, int y0, int x1, int y1, unsigned int color) {
	bool steep = false;
	if (abs(x0 - x1) < abs(y0 - y1)) {
		swap(x0, y0);
		swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			SetPixel(y, x, color);
		}
		else {
			SetPixel(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

unsigned int BlendColors(unsigned int sourceColor, unsigned int destColor, float alpha) {
	// Extract the individual color channels (RGBA) from the source and destination colors.
	unsigned int sourceR = (sourceColor >> 16) & 0xFF;
	unsigned int sourceG = (sourceColor >> 8) & 0xFF;
	unsigned int sourceB = sourceColor & 0xFF;
	unsigned int sourceA = (sourceColor >> 24) & 0xFF;

	unsigned int destR = (destColor >> 16) & 0xFF;
	unsigned int destG = (destColor >> 8) & 0xFF;
	unsigned int destB = destColor & 0xFF;
	unsigned int destA = (destColor >> 24) & 0xFF;

	// Calculate the blended color channels using alpha blending.
	unsigned int blendedR = static_cast<unsigned int>((sourceR * alpha) + (destR * (1.0f - alpha)));
	unsigned int blendedG = static_cast<unsigned int>((sourceG * alpha) + (destG * (1.0f - alpha)));
	unsigned int blendedB = static_cast<unsigned int>((sourceB * alpha) + (destB * (1.0f - alpha)));
	unsigned int blendedA = static_cast<unsigned int>((sourceA * alpha) + (destA * (1.0f - alpha)));

	// Combine the blended color channels into a single color value.
	unsigned int blendedColor =
		(blendedA << 24) | (blendedR << 16) | (blendedG << 8) | blendedB;

	return blendedColor;
}

void WuLine(int x0, int y0, int x1, int y1, unsigned int color) {
	// Determine if the line is steep, i.e., slope > 45 degrees or not.
	bool steep = abs(y1 - y0) > abs(x1 - x0);

	// If the line is steep, swap x and y coordinates.
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	// Ensure that the line is always drawn from left to right.
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	// Calculate the delta in x and y, and the initial fractional part of y.
	int dx = x1 - x0;
	int dy = y1 - y0;
	float gradient = static_cast<float>(dy) / dx;

	// Initial coordinates and fractional part of y.
	int xpxl1 = x0;
	int xpxl2 = x1;
	float intery = y0;

	// Calculate the alpha component of the color for blending.
	float alpha = 1.0f;

	// Main loop for drawing the line.
	for (int x = xpxl1; x <= xpxl2; x++) {
		if (steep) {
			int y = static_cast<int>(intery);
			// Calculate the alpha component based on the fractional part.
			alpha = intery - static_cast<int>(intery);
			SetPixel(y, x, BlendColors(GetPixel(y, x), color, alpha));
			SetPixel(y + 1, x, BlendColors(GetPixel(y + 1, x), color, 1.0f - alpha));
		}
		else {
			int y = static_cast<int>(intery);
			// Calculate the alpha component based on the fractional part.
			alpha = intery - static_cast<int>(intery);
			SetPixel(x, y, BlendColors(GetPixel(x, y), color, alpha));
			SetPixel(x, y + 1, BlendColors(GetPixel(x, y + 1), color, 1.0f - alpha));
		}
		intery += gradient;
	}
}

int lastX;
int lastY;

void Draw(int x, int y, unsigned int color) {
	WuLine(x, y, lastX, lastY, color);
	//line(x, y, lastX, lastY, color);

	lastX = x;
	lastY = y;
}

int main() {
	CreateWindowContext(1280, 720, "Window Title");

	int mouseX = 0;
	int mouseY = 0;

	GetCursorPixelCoordinates(mouseX, mouseY);

	lastX = mouseX;
	lastY = mouseY;

	while (running) {
		GetCursorPixelCoordinates(mouseX, mouseY);

		Draw(mouseX, mouseY, 0xffffff);

		UpdateWindow();
	}

	return 0;
}