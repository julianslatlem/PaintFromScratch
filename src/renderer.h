#pragma once
#include "engine.h"

namespace Renderer {
	void* bufferMemory;
	int bufferWidth;
	int bufferHeight;

	BITMAPINFO bufferBitmapInfo;

	/*Fills the screen with a specified color. [default = 0x000000]*/
	void ClearWindow(unsigned int color = 0x000000) {
		unsigned int* pixel = (unsigned int*)bufferMemory;
		for (int y = 0; y < bufferHeight; y++) {
			for (int x = 0; x < bufferWidth; x++) {
				*pixel++ = color;
			}
		}
	}

	void SetPixel(int x, int y, unsigned int color) {
		unsigned int* pixel = (unsigned int*)bufferMemory + x + y * bufferWidth;
		if (x >= 0 && x < bufferWidth && y >= 0 && y < bufferHeight)
			*pixel++ = color;
	}

	unsigned int GetPixel(int x, int y) {
		if (x >= 0 && x < bufferWidth && y >= 0 && y < bufferHeight) {
			unsigned int* pixel = (unsigned int*)bufferMemory + x + y * bufferWidth;
			return *pixel;
		}
		else return 0x000000;
	}

	void DrawLine(int x0, int y0, int x1, int y1, unsigned int color) {
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

	void DrawRect(int x0, int y0, int x1, int y1, unsigned int color) {
		int xx0 = min(x0, x1);
		int xx1 = max(x0, x1);
		int yy0 = min(y0, y1);
		int yy1 = max(y0, y1);

		for (int y = yy0; y <= yy1; y++) {
			for (int x = xx0; x <= xx1; x++) {
				SetPixel(x, y, color);
			}
		}
	}

	void DrawHueSlider(int x0, int y0, int x1, int y1) {
		int xx0 = min(x0, x1);
		int xx1 = max(x0, x1);
		int yy0 = min(y0, y1);
		int yy1 = max(y0, y1);

		for (int x = xx0; x <= xx1; x++) {
			// Calculate the hue smoothly transitioning from 0 to 360
			int hue = static_cast<int>((x - xx0) / static_cast<double>(xx1 - xx0) * 360);

			for (int y = yy0; y <= yy1; y++) {
				// Calculate RGB color based on hue
				unsigned int color = HSLToRGB2(hue, 100, 50); // Full saturation and 50% lightness

				SetPixel(x, y, color);
			}
		}
	}

	void DrawHueSliderr(int x0, int y0, int x1, int y1) {
		int xx0 = min(x0, x1);
		int xx1 = max(x0, x1);
		int yy0 = min(y0, y1);
		int yy1 = max(y0, y1);

		// Calculate hue increment per pixel to cover the entire hue spectrum
		double hueIncrement = 360.0 / (xx1 - xx0);

		for (int x = xx0; x <= xx1; x++) {
			// Calculate hue based on x position
			int hue = static_cast<int>((x - xx0) * hueIncrement);

			for (int y = yy0; y <= yy1; y++) {
				// Set saturation and lightness to constant values
				int saturation = 100;
				int lightness = 50;

				// Convert hue, saturation, and lightness to an RGB color
				unsigned int color = HSLToRGB(hue, saturation, lightness);

				SetPixel(x, y, color);
			}
		}
	}

	void DrawColorPicker(int x0, int y0, int x1, int y1, int selectedHue) {
		int xx0 = min(x0, x1);
		int xx1 = max(x0, x1);
		int yy0 = min(y0, y1);
		int yy1 = max(y0, y1);

		// Calculate hue increment per pixel
		double hueIncrement = 360.0 / (xx1 - xx0);
		int hue = selectedHue % 360; // Keep the selected hue within [0, 360]

		for (int x = xx0; x <= xx1; x++) {
			for (int y = yy0; y <= yy1; y++) {
				// Calculate saturation based on x position
				int saturation = static_cast<int>((x - xx0) / static_cast<double>(xx1 - xx0) * 100);

				// Calculate lightness based on y position
				int lightness = 100 - static_cast<int>((y - yy0) / static_cast<double>(yy1 - yy0) * 100);

				// Convert hue, saturation, and lightness to an RGB color
				unsigned int color = HSLToRGB2(hue, saturation, lightness);

				SetPixel(x, y, color);
			}
		}
	}

	void DrawColorPickerrr(int x0, int y0, int x1, int y1, int selectedHue) {
		int xx0 = min(x0, x1);
		int xx1 = max(x0, x1);
		int yy0 = min(y0, y1);
		int yy1 = max(y0, y1);

		unsigned int color = HSLToRGB2(selectedHue, 100, 10);

		DrawRect(xx0, yy0, xx1, yy1, color);
	}




	void DrawCircle(int centerX, int centerY, int radius, unsigned int color) {
		int x = radius;
		int y = 0;
		int err = 0;

		while (x >= y) {
			SetPixel(centerX + x, centerY + y, color);
			SetPixel(centerX - x, centerY + y, color);
			SetPixel(centerX + x, centerY - y, color);
			SetPixel(centerX - x, centerY - y, color);
			SetPixel(centerX + y, centerY + x, color);
			SetPixel(centerX - y, centerY + x, color);
			SetPixel(centerX + y, centerY - x, color);
			SetPixel(centerX - y, centerY - x, color);

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

	void FillCircleAA(int centerX, int centerY, int radius, unsigned int color) {
		for (int y = -radius; y <= radius; y++) {
			for (int x = -radius; x <= radius; x++) {
				float distance = sqrt(x * x + y * y);

				// Calculate the blending factor based on the distance from the circle's center to the current pixel.
				float alpha = 1.0f - (distance / radius);

				// Ensure alpha is in the range [0, 1].
				alpha = max(0.0f, min(1.0f, alpha));

				// Calculate the pixel color by blending the circle color with the background color.
				unsigned int blendedColor = BlendColors(color, GetPixel(centerX + x, centerY + y), alpha);

				// Set the pixel with the blended color.
				SetPixel(centerX + x, centerY + y, blendedColor);
			}
		}
	}
}