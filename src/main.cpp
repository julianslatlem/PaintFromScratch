#include <windows.h>
#include <iostream>

#include "renderer.h"

bool running = true;

bool rightMouse = false;

static HWND window;

// Initialize variables
LARGE_INTEGER frequency, lastTime, currentTime;
double deltaTime, fps;
int frameCount = 0;

int lineWidth = 30;

bool antialiasing = true;


LRESULT CALLBACK WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (msg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			PostQuitMessage(0);
			running = false;
		} break;

		case WM_MOUSEWHEEL: {
			// Get the delta value to determine the direction and amount of scrolling.
			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			// Process the scroll event based on the delta value.
			if (delta > 0) {
				// Scroll up (positive delta)
				// Handle upward scrolling here

				if (lineWidth < 150) lineWidth += 2 * delta / 120;
			}
			else if (delta < 0) {
				// Scroll down (negative delta)
				// Handle downward scrolling here

				if (lineWidth > 1) lineWidth += 2 * delta / 120;

				std::cout << delta << std::endl;

			}

			return 0;
		}
		
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
			if (antialiasing) FillAntiAliasedCircle(y, x, lineWidth / 2, color);
			else FillCircleW(y, x, lineWidth / 2, color);
		}
		else {
			if (antialiasing) FillAntiAliasedCircle(x, y, lineWidth / 2, color);
			else FillCircleW(x, y, lineWidth / 2, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void lineSTaper(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth) {
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
			float lLineWidth = lineWidth;
			if (lLineWidth > 4) lLineWidth -= 0.1f;

			lineWidth = (int)lLineWidth;
			if (antialiasing) FillAntiAliasedCircle(y, x, lineWidth / 2, color);
			else FillCircleW(y, x, lineWidth / 2, color);
		}
		else {
			float lLineWidth = lineWidth;
			if (lLineWidth > 4) lLineWidth -= 0.1f;

			lineWidth = (int)lLineWidth;
			if (antialiasing) FillAntiAliasedCircle(x, y, lineWidth / 2, color);
			else FillCircleW(x, y, lineWidth / 2, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void lineSMain(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth) {
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
			FillCircleWMain(y, x, lineWidth / 2, color);
		}
		else {
			FillCircleWMain(x, y, lineWidth / 2, color);
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

int canvasOffsetX = 50;
int canvasOffsetY = 100;

void Draw(int x, int y, unsigned int color, int lineWidth) {
	//WuLineW(x, y, lastX, lastY, color, lineWidth);
	lineS(x - canvasOffsetX, y - canvasOffsetY, lastX - canvasOffsetX, lastY - canvasOffsetY, color, lineWidth);
	//FillCircleW(x, y, lineWidth / 2, color);
	//line(x, y, lastX, lastY, color);

	lastX = x;
	lastY = y;
}

void DrawTaper(int x, int y, unsigned int color, int lineWidth) {
	//WuLineW(x, y, lastX, lastY, color, lineWidth);
	lineSTaper(x - canvasOffsetX, y - canvasOffsetY, lastX - canvasOffsetX, lastY - canvasOffsetY, color, lineWidth);
	//FillCircleW(x, y, lineWidth / 2, color);
	//line(x, y, lastX, lastY, color);

	lastX = x;
	lastY = y;
}

void DrawAA(int x, int y, unsigned int color) {
	WuLine(x - canvasOffsetX, y - canvasOffsetY, lastX - canvasOffsetX, lastY - canvasOffsetY, color);

	lastX = x;
	lastY = y;
}

void DrawPixelBuffer() {
	for (int y = 0; y < pixelBufferHeight; y++) {
		for (int x = 0; x < pixelBufferWidth; x++) {
			DrawPixel(x + canvasOffsetX, y + canvasOffsetY, PixelGetPixel(x, y));
		}
	}
}






int canvasLastX = 0;
int canvasLastY = 0;




void Pan(int currentX, int currentY, int previousX, int previousY) {
	int deltaX = currentX - previousX;
	int deltaY = currentY - previousY;
	canvasOffsetX += deltaX;
	canvasOffsetY += deltaY;
}












int main() {
	CreateWindowContext(1920, 1080, "Paint 2.0");

	/*pixelBufferWidth = 2560;
	pixelBufferHeight = 1440;*/

	pixelBufferWidth = 1920 - 100;
	pixelBufferHeight = 1080 - 150;

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
	bool cDown = false;
	bool aDown = false;
	bool iDown = false;
	bool upDown = false;
	bool downDown = false;
	bool scrollUp = false;
	bool scrollDown = false;
	bool middleMouseDown = false;

	bool pan = false;

	unsigned int activeColor = 0;

	unsigned int clickColor;
	int clickColorTimer = 0;

	unsigned int colors[11] = {0x000000, 0xf72585, 0xb5179e, 0x7209b7, 0x560bad, 0x480ca8, 0x3a0ca3, 0x3f37c9, 0x4361ee, 0x4895ef, 0x4cc9f0 };

	bool eraser = false;

	bool dropTool = false;

	// Get the frequency of the performance counter
	QueryPerformanceFrequency(&frequency);

	// Get the initial time
	QueryPerformanceCounter(&lastTime);

	ClearCanvasColor(0xffffff);


	while (running) {

		// Calculate delta time (time elapsed since the last frame)
		QueryPerformanceCounter(&currentTime);
		deltaTime = static_cast<double>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

		// Calculate FPS
		fps = 1.0 / deltaTime;

		// Update frame count
		frameCount++;

		// Output FPS every second
		if (deltaTime >= 1.0) {
			std::cout << "FPS: " << frameCount << std::endl;

			// Reset frame count and last time
			frameCount = 0;
			lastTime = currentTime;
		}





		ClearWindowColor(0x232323);
		GetCursorPixelCoordinates(mouseX, mouseY);

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8001) {
			if (!leftMouseButtonDown) {
				lastX = mouseX; lastY = mouseY;
				//FillCircleW(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
				if (PixelGetPixel(mouseX, mouseY) != 0xffffff) clickColor = GetPixel(mouseX, mouseY);
				else clickColor = colors[activeColor];

				if (dropTool) {
					colors[activeColor] = GetPixel(mouseX, mouseY);
				}
			}
			leftMouseButtonDown = true;
		}
		else {
			if (leftMouseButtonDown) {
				//FillCircleW(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
				//DrawTaper(mouseX, mouseY, colors[activeColor], lineWidth);
				clickColorTimer = 0;
			}
			leftMouseButtonDown = false;
		}

		if (GetAsyncKeyState(0x04) & 0x8001) {
			if (!middleMouseDown) {
				lastX = mouseX;
				lastY = mouseY;
			}
			middleMouseDown = true;
		}
		else {
			middleMouseDown = false;
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

		if (GetAsyncKeyState(0x49) & 0x8001) {
			if (!iDown) {
				//dropTool = !dropTool;
			}
			iDown = true;
		}
		else {
			iDown = false;
		}

		if (GetAsyncKeyState(0x41) & 0x8001) {
			if (!aDown) {
				antialiasing = !antialiasing;
			}
			aDown = true;
		}
		else {
			aDown = false;
		}

		if (GetAsyncKeyState(0x43) & 0x8001) {
			if (!cDown) {
				ClearCanvasColor(0xffffff);
			}
			cDown = true;
		}
		else {
			cDown = false;
		}

		if (GetAsyncKeyState(VK_SCROLL) & 1) {
			// Scroll wheel was released (scroll-up)
			if (!scrollUp) {
				lineWidth += 2;
			}
			scrollUp = true;
		}
		else {
			scrollUp = false;
		}

		if (leftMouseButtonDown) {
			clickColorTimer++;
			if (clickColorTimer > 3000) {
				clickColor = PixelGetPixel(mouseX, mouseY);
			}

			if (!eraser) {
				//if (lineWidth > 1) Draw(mouseX, mouseY, BlendColors(colors[activeColor], clickColor, 0.2f), lineWidth);
				if (lineWidth > 1) Draw(mouseX, mouseY, colors[activeColor], lineWidth);
				else DrawAA(mouseX, mouseY, colors[activeColor]);
			}
			else Draw(mouseX, mouseY, 0xffffff, lineWidth * 2);
		}






		if (middleMouseDown) {
			pan = true;
			Pan(mouseX, mouseY, lastX, lastY);
			lastX = mouseX;
			lastY = mouseY;
		}





		int w = 10;
		int w2 = 5;
		int w3 = 3;

		DrawRect(canvasOffsetX - w, canvasOffsetY - w, canvasOffsetX + pixelBufferWidth + w, canvasOffsetY + pixelBufferHeight + w, 0x212121);
		DrawRect(canvasOffsetX - w2, canvasOffsetY - w2, canvasOffsetX + pixelBufferWidth + w2, canvasOffsetY + pixelBufferHeight + w2, 0x202020);
		DrawRect(canvasOffsetX - w3, canvasOffsetY - w3, canvasOffsetX + pixelBufferWidth + w3, canvasOffsetY + pixelBufferHeight + w3, 0x191919);

		DrawPixelBuffer();

		//DrawRect(mouseX - (lineWidth / 2), mouseY - (lineWidth / 2), mouseX + (lineWidth / 2), mouseY + (lineWidth / 2), colors[activeColor]);

		if (!eraser && !dropTool) {
			FillCircleBlend(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
			DrawCircle(mouseX, mouseY, lineWidth / 2, colors[activeColor]);
		}
		else if (eraser && !dropTool) DrawCircle(mouseX, mouseY, lineWidth, 0xff0000);
		else if (dropTool) {
			DrawCircle(mouseX, mouseY, lineWidth / 2, GetPixel(mouseX, mouseY));
			DrawCircle(mouseX, mouseY, (lineWidth / 2) - 1, GetPixel(mouseX, mouseY));
			DrawCircle(mouseX, mouseY, (lineWidth / 2) + 1, GetPixel(mouseX, mouseY));
			DrawCircle(mouseX, mouseY, (lineWidth / 2) - 2, BlendColors(0x000000, GetPixel(mouseX, mouseY), 0.2f));
			DrawCircle(mouseX, mouseY, (lineWidth / 2) + 2, BlendColors(0x000000, GetPixel(mouseX, mouseY), 0.2f));
		}



		//FillAntiAliasedCircle(GetWindowWidth() / 2, GetWindowHeight() / 2, 40, 0xffffff);

		// UI

		/*DrawRect(0, 0, 200, GetWindowHeight(), 0x101010);
		WuLineWMain(200, 0, 200, GetWindowHeight(), 0x404040, 1);
		WuLineWMain(199, 0, 199, GetWindowHeight(), 0x000000, 1);
		WuLineWMain(198, 0, 198, GetWindowHeight(), 0x404040, 1);

		DrawRect(GetWindowWidth() - 100, 0, GetWindowWidth(), GetWindowHeight(), 0x101010);
		WuLineWMain(GetWindowWidth() - 100, 0, GetWindowWidth() - 100, GetWindowHeight(), 0x404040, 1);
		WuLineWMain(GetWindowWidth() - 99, 0, GetWindowWidth() - 99, GetWindowHeight(), 0x000000, 1);
		WuLineWMain(GetWindowWidth() - 98, 0, GetWindowWidth() - 98, GetWindowHeight(), 0x404040, 1);

		DrawRect(0, GetWindowHeight() - 40, GetWindowWidth(), GetWindowHeight(), 0x101010);
		WuLineWMain(0, GetWindowHeight() - 40, GetWindowWidth(), GetWindowHeight() - 40, 0x404040, 1);
		WuLineWMain(0, GetWindowHeight() - 39, GetWindowWidth(), GetWindowHeight() - 39, 0x000000, 1);
		WuLineWMain(0, GetWindowHeight() - 38, GetWindowWidth(), GetWindowHeight() - 38, 0x404040, 1);*/

		DrawRect(0, 0, GetWindowWidth(), 50, 0x101010);
		WuLineWMain(0, 50, GetWindowWidth(), 50, 0x404040, 1);
		//DrawRect((GetWindowWidth() / 2) - 20, 5, (GetWindowWidth() / 2) + 20, 45, 0xffffff);

		//lineSMain((GetWindowWidth() / 2) - 15, 10, (GetWindowWidth() / 2) + 15, 40, 0xffffff, 10);

		if (eraser) {
			WuLineWMain(5, 5, 45, 45, 0xff0000, 1);
			WuLineWMain(5, 45, 45, 5, 0xff0000, 1);

			WuLineWMain(5, 5, 45, 5, 0xff0000, 1);
			WuLineWMain(45, 5, 45, 45, 0xff0000, 1);
			WuLineWMain(45, 45, 5, 45, 0xff0000, 1);
			WuLineWMain(5, 45, 5, 5, 0xff0000, 1);
		}
		else DrawRect(5, 5, 45, 45, colors[activeColor]);

		if (antialiasing) {
			for (int i = 0; i < 10; i++) {
				FillAntiAliasedCircleMain(85, 25, 20, colors[activeColor]);
			}
		}
		else {
			FillCircle(85, 25, 20, colors[activeColor]);
		}

		//FillColorWheel(100, 150, 80);

		//DrawSquareColorPicker(25, 75, 11, 2000);
		//DrawCircle(120, 170, 8, 0x494949);
		//DrawCircle(120, 170, 9, 0x494949);


		UpdateWindow();

	}

	return 0;
}