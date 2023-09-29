#pragma once

#include <windows.h>

void* bufferMemory;
int bufferWidth;
int bufferHeight;

BITMAPINFO bufferBitmapInfo;

void* pixelBufferMemory;
int pixelBufferWidth;
int pixelBufferHeight;

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


void DrawPixel(int x, int y, unsigned int color) {
	unsigned int* pixel = (unsigned int*)bufferMemory + x + y * bufferWidth;
	if (x >= 0 && x < bufferWidth && y >= 0 && y < bufferHeight)
		*pixel++ = color;
}

void SetPixel(int x, int y, unsigned int color) {
	unsigned int* pixel = (unsigned int*)pixelBufferMemory + x + y * pixelBufferWidth;
	if (x >= 0 && x < pixelBufferWidth && y >= 0 && y < pixelBufferHeight)
		*pixel++ = color;
}

unsigned int GetPixel(int x, int y) {
	if (x >= 0 && x < bufferWidth && y >= 0 && y < bufferHeight) {
		unsigned int* pixel = (unsigned int*)bufferMemory + x + y * bufferWidth;
		return *pixel;
	}
	else return 0x000000;
}

unsigned int PixelGetPixel(int x, int y) {
	if (x >= 0 && x < pixelBufferWidth && y >= 0 && y < pixelBufferHeight) {
		unsigned int* pixel = (unsigned int*)pixelBufferMemory + x + y * pixelBufferWidth;
		return *pixel;
	}
	else return 0x000000;
}

void ClearWindow() {
	unsigned int* pixel = (unsigned int*)bufferMemory;
	for (int y = 0; y < bufferHeight; y++) {
		for (int x = 0; x < bufferWidth; x++) {
			*pixel++ = 0x000000;
		}
	}
}

void ClearWindowColor(unsigned int color) {
	unsigned int* pixel = (unsigned int*)bufferMemory;
	for (int y = 0; y < bufferHeight; y++) {
		for (int x = 0; x < bufferWidth; x++) {
			*pixel++ = color;
		}
	}
}

void DrawRect(int x0, int y0, int x1, int y1, unsigned int color) {
	int xx0 = min(x0, x1);
	int xx1 = max(x0, x1);
	int yy0 = min(y0, y1);
	int yy1 = max(y0, y1);

	for (int y = yy0; y <= yy1; y++) {
		for (int x = xx0; x <= xx1; x++) {
			DrawPixel(x, y, color);
		}
	}
}

void FillCircle(int centerX, int centerY, int radius, unsigned int color) {
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		for (int i = centerX - x; i <= centerX + x; i++) {
			DrawPixel(i, centerY + y, color);
			DrawPixel(i, centerY - y, color);
		}
		for (int i = centerX - y; i <= centerX + y; i++) {
			DrawPixel(i, centerY + x, color);
			DrawPixel(i, centerY - x, color);
		}

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

void FillCircleBlend(int centerX, int centerY, int radius, unsigned int color) {
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		for (int i = centerX - x; i <= centerX + x; i++) {
			DrawPixel(i, centerY + y, BlendColors(color, GetPixel(i, centerY + y), 0.5f));
			DrawPixel(i, centerY - y, BlendColors(color, GetPixel(i, centerY - y), 0.5f));
		}
		for (int i = centerX - y; i <= centerX + y; i++) {
			DrawPixel(i, centerY + x, BlendColors(color, GetPixel(i, centerY + x), 0.5f));
			DrawPixel(i, centerY - x, BlendColors(color, GetPixel(i, centerY - x), 0.5f));
		}

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

void FillCircleW(int centerX, int centerY, int radius, unsigned int color) {
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		for (int i = centerX - x; i <= centerX + x; i++) {
			SetPixel(i, centerY + y, color);
			SetPixel(i, centerY - y, color);
		}
		for (int i = centerX - y; i <= centerX + y; i++) {
			SetPixel(i, centerY + x, color);
			SetPixel(i, centerY - x, color);
		}

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

void DrawCircle(int centerX, int centerY, int radius, unsigned int color) {
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		DrawPixel(centerX + x, centerY + y, color);
		DrawPixel(centerX - x, centerY + y, color);
		DrawPixel(centerX + x, centerY - y, color);
		DrawPixel(centerX - x, centerY - y, color);
		DrawPixel(centerX + y, centerY + x, color);
		DrawPixel(centerX - y, centerY + x, color);
		DrawPixel(centerX + y, centerY - x, color);
		DrawPixel(centerX - y, centerY - x, color);

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}