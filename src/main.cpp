#include "engine.h"
#include <iostream>

bool running = true;

int main() {
	Window window;

	window.Init(1920, 1080, "Paint 3.0", &running);

	Canvas canvas;

	canvas.Init(1280, 720);
	canvas.Clear();

	int canvasOffsetX = 0;
	int canvasOffsetY = 0;

	int previousX = 0;
	int previousY = 0;

	int mouseX = 0;
	int mouseY = 0;

	BasicBrush brush;

	while (running) {
		Renderer::ClearWindow(0x282828);

		window.GetMousePos(mouseX, mouseY);

		if (Input::GetKeyPressed(KeyCode::MIDDLE_MOUSE_BUTTON)) {
			previousX = mouseX;
			previousY = mouseY;
		}

		if (Input::GetKeyDown(KeyCode::MIDDLE_MOUSE_BUTTON)) {
			int deltaX = mouseX - previousX;
			int deltaY = mouseY - previousY;
			canvasOffsetX += deltaX;
			canvasOffsetY += deltaY;

			previousX = mouseX;
			previousY = mouseY;
		}

		if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
			//canvas.Brush(canvas, mouseX - canvasOffsetX, mouseY - canvasOffsetY, 30, 0x000000);
			brush.Paint(mouseX - canvasOffsetX, mouseY - canvasOffsetY, canvas);
		}

		canvas.Display(canvasOffsetX, canvasOffsetY);

		Renderer::DrawCircle(mouseX, mouseY, 30, 0x000000);
		Renderer::DrawCircle(mouseX, mouseY, 31, 0xffffff);
		Renderer::DrawCircle(mouseX, mouseY, 32, 0x000000);

		Renderer::DrawRect(0, 0, 40, window.GetHeight(), 0x535353);
		Renderer::DrawLine(41, 0, 41, window.GetHeight(), 0x303030);
		Renderer::DrawLine(42, 0, 42, window.GetHeight(), 0x363636);

		window.Update();
	}

	return 0;
}