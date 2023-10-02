#include "engine.h"
#include <iostream>

bool running = true;

int main() {
	Window window;

	window.Init(1920, 1080, "Photoshop 2.0", &running);

	Canvas canvas;
	Canvas canvas2;

	canvas2.Init(100, 100);
	canvas2.Clear();

	canvas.Init(1920, 1080);
	canvas.Clear();

	int canvasOffsetX = (window.GetWidth() / 2) - (canvas.GetWidth() / 2) - 100;
	int canvasOffsetY = 50;

	int previousX = 0;
	int previousY = 0;

	int mouseX = 0;
	int mouseY = 0;

	BasicBrush brush;

	bool pan = false;

	unsigned int colors[4]{ 0x000000, 0xff0000, 0x00ff00, 0x0000ff};

	int colorIndex = 0;

	int brushSize = 30;
	double brushHardness = 0.2;

	int startPosX = 0;
	int startPosY = 0;

	int cursorPosX = 0;
	int cursorPosY = 0;

	int startBrushSize = 0;

	bool isChangingSize = false;

	bool eraser = false;

	int huePosX = 0;

	int colorPickerHue = 0;

	bool isChangingHue = false;

	int colorPosX = 0;
	int colorPosY = 160;

	bool isChangingColor = false;

	int triPosX = 80;

	bool isChangingHardness = false;

	//HCURSOR cursor;

	while (running) {
		Renderer::ClearWindow(0x282828);

		window.GetMousePos(mouseX, mouseY);

		

		if (Input::GetKeyPressed(KeyCode::SPACE)) {
			previousX = mouseX;
			previousY = mouseY;
		}

		if (Input::GetKeyDown(KeyCode::SPACE)) {
			pan = true;
		}
		else {
			pan = false;
		}

		if (Input::GetKeyPressed(KeyCode::TAB)) {
			colorIndex++;
			if (colorIndex > 3) {
				colorIndex = 0;
			}
		}

		cursorPosX = mouseX;
		cursorPosY = mouseY;

		if (Input::GetKeyDown(KeyCode::ALT)) {
			if (Input::GetKeyPressed(KeyCode::RIGHT_MOUSE_BUTTON)) {
				startPosX = mouseX;
				startPosY = mouseY;
				startBrushSize = brushSize;
			}

			if (Input::GetKeyDown(KeyCode::RIGHT_MOUSE_BUTTON)) {
				brushSize = startBrushSize + (mouseX - startPosX);
				if (brushSize < 3) brushSize = 3;
				isChangingSize = true;

				cursorPosX = startPosX;
				cursorPosY = startPosY;
			}
			else {
				isChangingSize = false;
			}
		}
		else {
			isChangingSize = false;
		}

		if (Input::GetKeyPressed(KeyCode::E)) {
			eraser = !eraser;
		}

		if (Input::GetKeyPressed(KeyCode::LEFT_MOUSE_BUTTON) || Input::GetKeyPressed(KeyCode::G)) {
			previousX = mouseX;
			previousY = mouseY;
			if (!eraser && !isChangingSize && !pan) {
				brush.SetColor(colors[colorIndex]);
				brush.SetSize(brushSize);
				brush.SetHardness(brushHardness);
				brush.Paint(previousX - canvasOffsetX, previousY - canvasOffsetY, mouseX - canvasOffsetX, mouseY - canvasOffsetY, canvas);
			}
			else if (!isChangingSize && !pan) {
				brush.SetColor(0xffffff);
				brush.SetSize(brushSize * 1.5);
				brush.SetHardness(brushHardness);
				brush.Paint(previousX - canvasOffsetX, previousY - canvasOffsetY, mouseX - canvasOffsetX, mouseY - canvasOffsetY, canvas);
			}
		}
		if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON) || Input::GetKeyDown(KeyCode::G)) {
			if (Input::GetKeyDown(KeyCode::SPACE)) {
				int deltaX = mouseX - previousX;
				int deltaY = mouseY - previousY;
				canvasOffsetX += deltaX;
				canvasOffsetY += deltaY;

				if (canvasOffsetX + window.GetWidth() / 2 > (canvas.GetWidth() * 1.5)) canvasOffsetX = (canvas.GetWidth() * 1.5) - window.GetWidth() / 2;
				if (canvasOffsetY + window.GetHeight() / 2 > (canvas.GetHeight() * 1.5)) canvasOffsetY = (canvas.GetHeight() * 1.5) - window.GetHeight() / 2;
				if (canvasOffsetX - window.GetWidth() / 2 < -(canvas.GetWidth() * 1.5)) canvasOffsetX = -(canvas.GetWidth() * 1.5) + window.GetWidth() / 2;
				if (canvasOffsetY - window.GetHeight() / 2 < -(canvas.GetHeight() * 1.5)) canvasOffsetY = -(canvas.GetHeight() * 1.5) + window.GetHeight() / 2;

				previousX = mouseX;
				previousY = mouseY;
			}
			else {
				if (mouseX != previousX || mouseY != previousY) {
					if (!eraser) {
						brush.SetColor(colors[colorIndex]);
						brush.SetSize(brushSize);
						brush.SetHardness(brushHardness);
						brush.Paint(previousX - canvasOffsetX, previousY - canvasOffsetY, mouseX - canvasOffsetX, mouseY - canvasOffsetY, canvas);
					}
					else {
						brush.SetColor(0xffffff);
						brush.SetSize(brushSize * 1.5);
						brush.SetHardness(brushHardness);
						brush.Paint(previousX - canvasOffsetX, previousY - canvasOffsetY, mouseX - canvasOffsetX, mouseY - canvasOffsetY, canvas);
					}
				}

				previousX = mouseX;
				previousY = mouseY;
			}
		}

		if (Input::GetKeyPressed(KeyCode::RIGHT)) {
			huePosX++;
		}

		if (isChangingSize) {
			window.SetCur(LoadCursorFromFile(L"./cursors/Normal1.cur"));
		}
		else if (mouseX > 40 && mouseX < window.GetWidth() - 200 && mouseY > 15 && mouseY < window.GetHeight() - 60 && !pan) {
			//cursor = LoadCursorFromFile(L"./cursors/Normal1.cur");
			window.SetCur(NULL);
		}
		else if (mouseX > 0 && mouseX < window.GetWidth() && mouseY > 0 && mouseY < window.GetHeight() && !pan) window.SetCur(LoadCursorFromFile(L"./cursors/Normal1.cur"));
		else if (pan) {
			window.SetCur(LoadCursorFromFile(L"./cursors/Slide.cur"));
		}

		Renderer::DrawRect(canvasOffsetX - 4, canvasOffsetY - 4, canvas.GetWidth() + 4 + canvasOffsetX, canvas.GetHeight() + 4 + canvasOffsetY, 0x272727);
		Renderer::DrawRect(canvasOffsetX - 3, canvasOffsetY - 3, canvas.GetWidth() + 3 + canvasOffsetX, canvas.GetHeight() + 3 + canvasOffsetY, 0x262626);
		Renderer::DrawRect(canvasOffsetX - 2, canvasOffsetY - 2, canvas.GetWidth() + 2 + canvasOffsetX, canvas.GetHeight() + 2 + canvasOffsetY, 0x252525);
		Renderer::DrawRect(canvasOffsetX - 1, canvasOffsetY - 1, canvas.GetWidth() + 1 + canvasOffsetX, canvas.GetHeight() + 1 + canvasOffsetY, 0x242424);

		canvas.Display(canvasOffsetX, canvasOffsetY);

		Renderer::DrawRect(0, window.GetHeight(), window.GetWidth(), window.GetHeight() - 60, 0x424242);

		Renderer::DrawRect(0, 0, 40, window.GetHeight(), 0x535353);
		Renderer::DrawLine(40, 0, 40, window.GetHeight(), 0x383838);
		Renderer::DrawLine(39, 0, 39, window.GetHeight(), 0x474747);
		Renderer::DrawLine(38, 0, 38, window.GetHeight(), 0x383838);

		Renderer::DrawRect(window.GetWidth() - 200, 0, window.GetWidth(), window.GetHeight(), 0x535353);

		Renderer::DrawRect(0, window.GetHeight(), window.GetWidth(), window.GetHeight() - 30, 0x535353);
		Renderer::DrawLine(0, window.GetHeight() - 30, window.GetWidth(), window.GetHeight() - 30, 0x383838);

		Renderer::DrawRect(40, 0, window.GetWidth() - 200, 15, 0x4a4a4a);
		Renderer::DrawRect(700 - (canvasOffsetX / 4), 2, window.GetWidth() - 860 - (canvasOffsetX / 4), 13, 0x696969);

		Renderer::DrawRect(window.GetWidth() - 215, 0, window.GetWidth() - 200, window.GetHeight() - 60, 0x4a4a4a);
		Renderer::DrawRect(window.GetWidth() - 213, 700 - (canvasOffsetY / 4), window.GetWidth() - 202, window.GetHeight() - 760 - (canvasOffsetY / 4), 0x696969);

		Renderer::DrawRect(window.GetWidth() - 215, 0, window.GetWidth() - 200, 15, 0x535353);

		Renderer::DrawLine(window.GetWidth() - 200, 0, window.GetWidth() - 200, window.GetHeight() - 30, 0x383838);
		Renderer::DrawLine(window.GetWidth() - 199, 0, window.GetWidth() - 199, window.GetHeight() - 30, 0x474747);
		Renderer::DrawLine(window.GetWidth() - 198, 0, window.GetWidth() - 198, window.GetHeight() - 30, 0x383838);

		if (!eraser && mouseX > 40 && mouseX < window.GetWidth() - 200 && mouseY > 15 && mouseY < window.GetHeight() - 60 && !pan) {
			if (GetBrightness(Renderer::GetPixel(mouseX, mouseY)) > 127) {
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize, 0xffffff);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 1, 0x505050);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 2, 0x505050);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 3, 0xffffff);
			}
			else {
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize, 0x505050);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 1, 0xffffff);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 2, 0xffffff);
				Renderer::DrawCircle(cursorPosX, cursorPosY, brushSize + 3, 0x505050);
			}
		}
		else if (eraser && mouseX > 40 && mouseX < window.GetWidth() - 200 && mouseY > 15 && mouseY < window.GetHeight() - 60 && !pan) {
			if (GetBrightness(Renderer::GetPixel(mouseX, mouseY)) > 127) {
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5), 0xffffff);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 1, 0xff0000);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 2, 0xff0000);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 3, 0xffffff);
			}
			else {
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5), 0x505050);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 1, 0xff0000);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 2, 0xff0000);
				Renderer::DrawCircle(cursorPosX, cursorPosY, (brushSize * 1.5) + 3, 0x505050);
			}
		}

		if (isChangingSize && !eraser) {
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
			Renderer::FillCircleAA(cursorPosX, cursorPosY, brushSize, colors[colorIndex]);
		}

		if (!isChangingColor && mouseX > window.GetWidth() - 180 && mouseX < window.GetWidth() - 20 && mouseY > window.GetHeight() - 250 && mouseY < window.GetHeight() - 230) {
			if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
				isChangingHue = true;
			}
		}

		if (!isChangingHue && mouseX > window.GetWidth() - 180 && mouseX < window.GetWidth() - 20 && mouseY > window.GetHeight() - 210 && mouseY < window.GetHeight() - 50) {
			if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
				isChangingColor = true;
			}
		}

		if (mouseX > window.GetWidth() - 180 && mouseX < window.GetWidth() - 20 && mouseY < window.GetHeight() - 272 && mouseY > window.GetHeight() - 296) {
			if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
				isChangingHardness = true;
			}
		}

		if (Input::GetKeyReleased(KeyCode::LEFT_MOUSE_BUTTON)) {
			isChangingColor = false;
			isChangingHue = false;
			isChangingHardness = false;
		}

		if (isChangingHue) {
			huePosX = mouseX - (window.GetWidth() - 185);
			if (huePosX < 0) huePosX = 0;
			if (huePosX > 160) huePosX = 160;
		}

		if (isChangingColor) {
			colorPosX = mouseX - (window.GetWidth() - 180);
			colorPosY = mouseY - (window.GetHeight() - 210);

			if (colorPosX < 0) colorPosX = 0;
			if (colorPosX > 160) colorPosX = 160;
			if (colorPosY < 0) colorPosY = 0;
			if (colorPosY > 160) colorPosY = 160;
		}

		if (isChangingHardness) {
			triPosX = mouseX - (window.GetWidth() - 184);

			if (triPosX < 0) triPosX = 0;
			if (triPosX > 160) triPosX = 160;
		}

		brushHardness = 0.02 + (((double)triPosX / 160)) * (((double)triPosX / 160));




		Renderer::DrawHueSlider(window.GetWidth() - 180, window.GetHeight() - 250, window.GetWidth() - 20, window.GetHeight() - 230);
		int colorPickerHue = RGBToHue(Renderer::GetPixel((window.GetWidth() - 180) + huePosX, window.GetHeight() - 240));

		Renderer::DrawColorPicker(window.GetWidth() - 180, window.GetHeight() - 210, window.GetWidth() - 20, window.GetHeight() - 50, colorPickerHue);

		Renderer::DrawCircle(window.GetWidth() - 180 + colorPosX, window.GetHeight() - 210 + colorPosY, 5, GetBrightness(colors[colorIndex]) > 127 ? 0x000000 : 0xffffff);
		Renderer::DrawCircle(window.GetWidth() - 180 + colorPosX, window.GetHeight() - 210 + colorPosY, 6, GetBrightness(colors[colorIndex]) > 127 ? 0x000000 : 0xffffff);


		Renderer::DrawCircle((window.GetWidth() - 180) + huePosX, window.GetHeight() - 240, 5, 0x000000);
		Renderer::DrawCircle((window.GetWidth() - 180) + huePosX, window.GetHeight() - 240, 6, 0x000000);

		colors[colorIndex] = Renderer::GetPixel(window.GetWidth() - 180 + colorPosX, window.GetHeight() - 210 + colorPosY);

		Renderer::DrawRect(window.GetWidth() - 180, window.GetHeight() - 279, window.GetWidth() - 20, window.GetHeight() - 281, 0x696969);

		int triOffsetX = window.GetWidth() - 184 + triPosX;
		int triOffsetY = window.GetHeight() - 293;

		Renderer::DrawTri(triOffsetX, triOffsetY, triOffsetX + 10, triOffsetY, triOffsetX + 5, triOffsetY + 10, 0xafafaf);




		//Renderer::DrawRect(window.GetWidth() - 180, window.GetHeight() - 280, window.GetWidth() - 20, window.GetHeight() - 278, 0x757575);
		//Renderer::DrawCircle(window.GetWidth() - 150, window.GetHeight() - 279, 6, 0xffffff);


		window.Update();
	}

	return 0;
}