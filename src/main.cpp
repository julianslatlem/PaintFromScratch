#include <windows.h>
#include <iostream>

#include "renderer.h"

bool running = true;

bool rightMouse = false;

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

		case WM_KEYDOWN: {
			rightMouse = true;
		} break;

		case WM_KEYUP: {
			rightMouse = false;
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
			SetPixel(y, x, BlendColors(PixelGetPixel(y, x), color, alpha));
			SetPixel(y + 1, x, BlendColors(PixelGetPixel(y + 1, x), color, 1.0f - alpha));
		}
		else {
			int y = static_cast<int>(intery);
			// Calculate the alpha component based on the fractional part.
			alpha = intery - static_cast<int>(intery);
			SetPixel(x, y, BlendColors(PixelGetPixel(x, y), color, alpha));
			SetPixel(x, y + 1, BlendColors(PixelGetPixel(x, y + 1), color, 1.0f - alpha));
		}
		intery += gradient;
	}
}

void WuLineW(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth) {
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

	// Main loop for drawing the line.
	for (int x = x0; x <= x1; x++) {
		for (int w = -lineWidth / 2; w <= lineWidth / 2; w++) {
			int y = static_cast<int>(y0 + gradient * (x - x0)) + w;
			if (steep) {
				SetPixel(y, x, color);
				FillCircleW(y, x, lineWidth, color);
			}
			else {
				SetPixel(x, y, color);
				FillCircleW(x, y, lineWidth, color);
			}
		}
	}
}

void lineS(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth) {
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
			FillCircleW(y, x, lineWidth / 2, color);
		}
		else {
			FillCircleW(x, y, lineWidth / 2, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void WuLineWMain(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth) {
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

	// Main loop for drawing the line.
	for (int x = x0; x <= x1; x++) {
		for (int w = -lineWidth / 2; w <= lineWidth / 2; w++) {
			int y = static_cast<int>(y0 + gradient * (x - x0)) + w;
			if (steep) {
				DrawPixel(y, x, color);
			}
			else {
				DrawPixel(x, y, color);
			}
		}
	}
}

int lastX;
int lastY;

void Draw(int x, int y, unsigned int color, int lineWidth) {
	//WuLineW(x, y, lastX, lastY, color, lineWidth);
	lineS(x, y, lastX, lastY, color, lineWidth);
	//FillCircleW(x, y, lineWidth / 2, color);
	//line(x, y, lastX, lastY, color);

	lastX = x;
	lastY = y;
}

void DrawAA(int x, int y, unsigned int color) {
	WuLine(x, y, lastX, lastY, color);

	lastX = x;
	lastY = y;
}

void DrawPixelBuffer() {
	for (int y = 0; y < pixelBufferHeight; y++) {
		for (int x = 0; x < pixelBufferWidth; x++) {
			DrawPixel(x, y, PixelGetPixel(x, y));
		}
	}
}

int main() {
	CreateWindowContext(1280, 720, "Window Title");

	pixelBufferWidth = 2560;
	pixelBufferHeight = 1440;

	int pixelBufferSize = pixelBufferWidth * pixelBufferHeight * sizeof(unsigned int);

	if (pixelBufferMemory) VirtualFree(pixelBufferMemory, 0, MEM_RELEASE);
	pixelBufferMemory = VirtualAlloc(0, pixelBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	int mouseX = 0;
	int mouseY = 0;

	GetCursorPixelCoordinates(mouseX, mouseY);

	lastX = mouseX;
	lastY = mouseY;

	bool leftMouseButtonDown = false;
	bool tabDown = false;
	bool eDown = false;
	bool upDown = false;
	bool downDown = false;

	unsigned int activeColor = 0;

	unsigned int colors[11] = {0xffffff, 0xf72585, 0xb5179e, 0x7209b7, 0x560bad, 0x480ca8, 0x3a0ca3, 0x3f37c9, 0x4361ee, 0x4895ef, 0x4cc9f0 };

	bool eraser = false;

	int lineWidth = 30;

	while (running) {
		ClearWindowColor(0x000000);
		GetCursorPixelCoordinates(mouseX, mouseY);

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8001) {
			if (!leftMouseButtonDown) {
				lastX = mouseX; lastY = mouseY;
				//FillCircleW(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
			}
			leftMouseButtonDown = true;
		}
		else {
			if (leftMouseButtonDown) {
				//FillCircleW(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
			}
			leftMouseButtonDown = false;
		}

		if (GetAsyncKeyState(VK_TAB) & 0x8001) {
			if (!tabDown) {
				activeColor++;
				if (activeColor > 10) activeColor = 0;
			}
			tabDown = true;
		}
		else {
			tabDown = false;
		}

		if (GetAsyncKeyState(VK_UP) & 0x8001) {
			if (!upDown) {
				if (lineWidth < 150) lineWidth+=2;
			}
			upDown = true;
		}
		else {
			upDown = false;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8001) {
			if (!downDown) {
				if (lineWidth > 1) lineWidth-=2;
			}
			downDown = true;
		}
		else {
			downDown = false;
		}

		if (GetAsyncKeyState(0x45) & 0x8001) {
			if (!eDown) {
				eraser = !eraser;
			}
			eDown = true;
		}
		else {
			eDown = false;
		}

		if (leftMouseButtonDown) {
			if (!eraser) {
				if (lineWidth > 1) Draw(mouseX, mouseY, colors[activeColor], lineWidth);
				else DrawAA(mouseX, mouseY, colors[activeColor]);
			}
			else Draw(mouseX, mouseY, 0x000000, lineWidth * 2);
		}

		DrawPixelBuffer();

		//DrawRect(mouseX - (lineWidth / 2), mouseY - (lineWidth / 2), mouseX + (lineWidth / 2), mouseY + (lineWidth / 2), colors[activeColor]);

		if (!eraser) {
			FillCircleBlend(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
			DrawCircle(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
		}
		else DrawCircle(mouseX, mouseY, lineWidth, 0xff0000);

		if (eraser) {
			WuLineWMain(0, 0, 50, 50, 0xff0000, 2);
			WuLineWMain(0, 50, 50, 0, 0xff0000, 2);
		}
		else DrawRect(0, 0, 50, 50, colors[activeColor]);

		UpdateWindow();

	}

	return 0;
}