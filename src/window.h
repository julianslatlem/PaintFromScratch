#pragma once
#include <Windows.h>
#include <iostream>
#include "renderer.h"

namespace WindowsWindow {
	bool* running;
	LRESULT CALLBACK WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
		LRESULT result = 0;

		switch (msg) {
			case WM_CLOSE:
			case WM_DESTROY: {
				PostQuitMessage(0);
				*running = false;
			} break;

			case WM_SIZE: {
				RECT rect;
				GetClientRect(window, &rect);
				Renderer::bufferWidth = rect.right - rect.left;
				Renderer::bufferHeight = rect.bottom - rect.top;

				int bufferSize = Renderer::bufferWidth * Renderer::bufferHeight * sizeof(unsigned int);

				if (Renderer::bufferMemory) VirtualFree(Renderer::bufferMemory, 0, MEM_RELEASE);
				Renderer::bufferMemory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

				Renderer::bufferBitmapInfo.bmiHeader.biSize = sizeof(Renderer::bufferBitmapInfo.bmiHeader);
				Renderer::bufferBitmapInfo.bmiHeader.biWidth = Renderer::bufferWidth;
				Renderer::bufferBitmapInfo.bmiHeader.biHeight = Renderer::bufferHeight;
				Renderer::bufferBitmapInfo.bmiHeader.biPlanes = 1;
				Renderer::bufferBitmapInfo.bmiHeader.biBitCount = 32;
				Renderer::bufferBitmapInfo.bmiHeader.biCompression = BI_RGB;
			} break;

			default: {
				result = DefWindowProcA(window, msg, wParam, lParam);
			}
		}

		return result;
	}

	HWND InitializeWindow(int width, int height, const char* title, bool* m_running) {
		HINSTANCE instance = GetModuleHandleA(0);

		running = m_running;

		WNDCLASSA windowClass = {};
		windowClass.hInstance = instance;
		//windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = title;
		windowClass.lpfnWndProc = WindowCallback;

		if (!RegisterClassA(&windowClass)) return NULL;

		int dwStyle = WS_OVERLAPPEDWINDOW;

		RECT windowRect = { 0, 0, width, height };
		AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, false, 0);

		int adjustedWidth = windowRect.right - windowRect.left;
		int adjustedHeight = windowRect.bottom - windowRect.top;

		HWND window = CreateWindowExA(NULL, title, title, dwStyle, 100, 100,
			adjustedWidth, adjustedHeight, NULL, NULL, instance, NULL);

		if (!window) return NULL;

		ShowWindow(window, SW_SHOW);

		return window;
	}

	void UpdateWindow(HWND window) {
		MSG msg;

		while (PeekMessage(&msg, window, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		HDC hdc = GetDC(window);

		StretchDIBits(hdc, 0, 0, Renderer::bufferWidth, Renderer::bufferHeight, 0, 0, 
			Renderer::bufferWidth, Renderer::bufferHeight, Renderer::bufferMemory, &Renderer::bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		ReleaseDC(window, hdc);
	}
}

class Window {
private:
	int m_width;
	int m_height;
	const char* m_title;

	bool* m_running;

	HWND window;
public:
	/*Initiates the window.*/
	void Init(int width, int height, const char* title, bool* running) {
		this->m_width = width;
		this->m_height = height;
		this->m_title = title;

		this->m_running = running;

		this->window = WindowsWindow::InitializeWindow(this->m_width, this->m_height, this->m_title, this->m_running);
	}

	/*Checks for messages and renders the current pixel buffer to the window.*/
	void Update() {
		WindowsWindow::UpdateWindow(this->window);
	}

	/*Gets the current width of the window in pixels.*/
	int GetWidth() {
		RECT windowRect;

		GetClientRect(this->window, &windowRect);
		return windowRect.right - windowRect.left;
	}

	/*Gets the current height of the window in pixels.*/
	int GetHeight() {
		RECT windowRect;

		GetClientRect(this->window, &windowRect);
		return windowRect.bottom - windowRect.top;
	}

	/*Sets the title of the window.*/
	void SetTitle(const char* title) {
		this->m_title = title;

		SetWindowTextA(this->window, title);
	}

	void SetCur(HCURSOR cursor) {
		SetCursor(cursor);
	}

	/*Returns the position of the mouse relative to the window in pixels.*/
	bool GetMousePos(int& x, int& y) {
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
				y = this->GetHeight() - cursorPos.y;
				return true;
			}
		}

		return false;
	}
};