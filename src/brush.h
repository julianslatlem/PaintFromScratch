#pragma once

class BaseBrush {
private:
	int m_size;
	unsigned int m_color;
public:
	void SetSize(int size) {
		this->m_size = size;
	}

	void SetColor(unsigned int color) {
		this->m_color = color;
	}

	virtual void Paint() {

	}
};

class BasicBrush : BaseBrush {
private:
	int m_size = 30;
	unsigned int m_color = 0x00a00f;

	int m_lastX = 0;
	int m_lastY = 0;
	
	double m_hardness = 1.0;
public:
	void Paint(int lastX, int lastY, int x, int y, Canvas& canvas) {
		//std::cout << "Paint" << std::endl;
		this->m_lastX = lastX;
		this->m_lastY = lastY;
		canvas.Draw(this->m_lastX, this->m_lastY, x, y, canvas, this->m_color, this->m_size, this->m_hardness);
	}

	void SetSize(int size) {
		this->m_size = size;
	}

	void SetHardness(double hardness) {
		this->m_hardness = hardness;
	}

	void SetColor(unsigned int color) {
		this->m_color = color;
	}
};