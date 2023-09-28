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

	StretchDIBits(GetDC(window), 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, bufferMemory, &bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
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

int main() {
	CreateWindowContext(1280, 720, "Window Title");

	int mouseX = 0;
	int mouseY = 0;

	while (running) {
		ClearWindow();

		SetPixel(200, 200, 0xff0000);
		SetPixel(200, 201, 0xff0000);
		//GetCursorPixelCoordinates(mouseX, mouseY);

		std::cout << mouseX << " " << mouseY << std::endl;

		UpdateWindow();
	}

	return 0;
}