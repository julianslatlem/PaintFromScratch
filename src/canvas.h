#pragma once
#include "engine.h"

class Canvas {
private:
	int m_width;
	int m_height;

	void* m_bufferMemory;
	int m_bufferSize;
public:
	void Init(int width, int height) {
		this->m_width = width;
		this->m_height = height;

		this->m_bufferSize = m_width * m_height * sizeof(unsigned int);

		m_bufferMemory = VirtualAlloc(0, this->m_bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	}

	int GetWidth() {
		return this->m_width;
	}

	int GetHeight() {
		return this->m_height;
	}

	unsigned int GetPixel(int x, int y) {
		if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
			unsigned int* pixel = (unsigned int*)m_bufferMemory + x + y * m_width;
			return *pixel;
		}
		else return 0x000000;
	}

	void SetPixel(int x, int y, unsigned int color) {
		unsigned int* pixel = (unsigned int*)m_bufferMemory + x + y * m_width;
		if (x >= 0 && x < m_width && y >= 0 && y < m_height)
			*pixel++ = color;
	}

	void Clear(unsigned int color = 0xffffff) {
		unsigned int* pixel = (unsigned int*)m_bufferMemory;
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				*pixel++ = color;
			}
		}
	}

	void Brush(Canvas& canvas, int centerX, int centerY, int radius, unsigned int color) {
		for (int y = -radius; y <= radius; y++) {
			for (int x = -radius; x <= radius; x++) {
				double distance = sqrt(x * x + y * y);

				// Calculate the blending factor based on the distance from the circle's center to the current pixel.
				double alpha = 1.0f - (distance / radius);

				// Ensure alpha is in the range [0, 1].
				alpha = max(0.0f, min(1.0f, alpha));

				// Calculate the pixel color by blending the circle color with the background color.
				unsigned int blendedColor = BlendColors(color, GetPixel(centerX + x, centerY + y), alpha);

				// Set the pixel with the blended color.
				canvas.SetPixel(centerX + x, centerY + y, blendedColor);
			}
		}
	}

	void Line(int x0, int y0, int x1, int y1, unsigned int color, int lineWidth, Canvas& canvas) {
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
				Brush(canvas, y, x, lineWidth, color);
			}
			else {
				Brush(canvas, x, y, lineWidth, color);

			}
			error2 += derror2;
			if (error2 > dx) {
				y += (y1 > y0 ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}

	void Draw(int lastX, int lastY, int x, int y, Canvas& canvas, unsigned int color, int size) {
		Line(lastX, lastY, x, y, color, size, canvas);
	}

	void Display(int offsetX, int offsetY) {
		for (int y = 0; y < this->m_height; y++) {
			for (int x = 0; x < this->m_width; x++) {
				Renderer::SetPixel(x + offsetX, y + offsetY, GetPixel(x, y));
			}
		}
	}
};