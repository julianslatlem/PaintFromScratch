#pragma once

#include <windows.h>

void* bufferMemory;
int bufferWidth;
int bufferHeight;

BITMAPINFO bufferBitmapInfo;

void* pixelBufferMemory;
int pixelBufferWidth;
int pixelBufferHeight;

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