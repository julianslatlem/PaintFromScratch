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

void ClearCanvas() {
	unsigned int* pixel = (unsigned int*)pixelBufferMemory;
	for (int y = 0; y < pixelBufferHeight; y++) {
		for (int x = 0; x < pixelBufferWidth; x++) {
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
			DrawPixel(i, centerY + y, BlendColors(color, GetPixel(i, centerY + y), 0.3f));
			DrawPixel(i, centerY - y, BlendColors(color, GetPixel(i, centerY - y), 0.3f));
		}
		for (int i = centerX - y; i <= centerX + y; i++) {
			DrawPixel(i, centerY + x, BlendColors(color, GetPixel(i, centerY + x), 0.3f));
			DrawPixel(i, centerY - x, BlendColors(color, GetPixel(i, centerY - x), 0.3f));
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

void FillCircleWMain(int centerX, int centerY, int radius, unsigned int color) {
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

unsigned int BlendColorss(unsigned int background, unsigned int foreground, int intensity) {
	int alpha = intensity;
	int invAlpha = 255 - intensity;

	int blendedRed = (GetRValue(background) * invAlpha + GetRValue(foreground) * alpha) / 255;
	int blendedGreen = (GetGValue(background) * invAlpha + GetGValue(foreground) * alpha) / 255;
	int blendedBlue = (GetBValue(background) * invAlpha + GetBValue(foreground) * alpha) / 255;

	return RGB(blendedRed, blendedGreen, blendedBlue);
}

void FillCircleAntiAliased(int centerX, int centerY, int radius, unsigned int color) {
	for (int x = centerX - radius; x <= centerX + radius; x++) {
		for (int y = centerY - radius; y <= centerY + radius; y++) {
			// Calculate the distance from the current pixel to the circle center
			double distance = sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));

			// Calculate the coverage based on the distance and radius
			double coverage = 1.0 - (distance - radius + 0.5);

			// Ensure the coverage is within the range [0, 1]
			coverage = max(0.0, min(1.0, coverage));

			// Calculate the alpha (transparency) of the new circle color
			int alpha = static_cast<int>(coverage * 255);

			// Blend the color with the existing pixel color based on alpha
			unsigned int existingColor = GetPixel(x, y);
			unsigned int blendedColor = BlendColorss(existingColor, color, alpha);

			// Set the pixel with the blended color
			SetPixel(x, y, blendedColor);
		}
	}
}

void FillAntiAliasedCircle(int centerX, int centerY, int radius, unsigned int color) {
	for (int y = -radius; y <= radius; y++) {
		for (int x = -radius; x <= radius; x++) {
			float distance = sqrt(x * x + y * y);

			// Calculate the blending factor based on the distance from the circle's center to the current pixel.
			float alpha = 1.0f - (distance / radius);

			// Ensure alpha is in the range [0, 1].
			alpha = max(0.0f, min(1.0f, alpha));

			// Calculate the pixel color by blending the circle color with the background color.
			unsigned int blendedColor = BlendColors(color, PixelGetPixel(centerX + x, centerY + y), alpha);

			// Set the pixel with the blended color.
			SetPixel(centerX + x, centerY + y, blendedColor);
		}
	}
}

void FillAntiAliasedCircleMain(int centerX, int centerY, int radius, unsigned int color) {
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
			DrawPixel(centerX + x, centerY + y, blendedColor);
		}
	}
}

void FillCircleAntiAliasedSSAA(int centerX, int centerY, int radius, unsigned int color, int scaleFactor) {
	int scaledRadius = radius * scaleFactor;
	int scaledWidth = scaledRadius * 2;
	int outputWidth = scaledWidth / scaleFactor;

	for (int x = centerX - scaledRadius; x <= centerX + scaledRadius; x++) {
		for (int y = centerY - scaledRadius; y <= centerY + scaledRadius; y++) {
			double coverage = 0.0;

			for (int subX = 0; subX < scaleFactor; subX++) {
				for (int subY = 0; subY < scaleFactor; subY++) {
					// Calculate the subpixel coordinates within the pixel
					double subXCoord = (x - centerX + subX / static_cast<double>(scaleFactor)) / scaledRadius;
					double subYCoord = (y - centerY + subY / static_cast<double>(scaleFactor)) / scaledRadius;

					// Calculate the distance from the subpixel to the circle center
					double subDistance = sqrt(subXCoord * subXCoord + subYCoord * subYCoord);

					// Calculate the subpixel coverage
					double subCoverage = 1.0 - (subDistance - 1.0);

					// Ensure the subpixel coverage is within the range [0, 1]
					subCoverage = max(0.0, min(1.0, subCoverage));

					// Accumulate the subpixel coverage
					coverage += subCoverage;
				}
			}

			// Calculate the average coverage for the pixel
			coverage /= (scaleFactor * scaleFactor);

			// Blend the color with the existing pixel color based on the coverage
			unsigned int existingColor = GetPixel(x / scaleFactor, y / scaleFactor);
			unsigned int blendedColor = BlendColorss(existingColor, color, static_cast<int>(coverage * 255));

			// Set the pixel with the blended color
			SetPixel(x / scaleFactor, y / scaleFactor, blendedColor);
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