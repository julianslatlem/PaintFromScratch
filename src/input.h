#pragma once

namespace KeyCode {
    enum KeyCode {
        LEFT_MOUSE_BUTTON = 1,
        RIGHT_MOUSE_BUTTON = 2,
        MIDDLE_MOUSE_BUTTON = 4,
        TAB = 9,
        SPACE = 32,
        LEFT = 37,
        UP = 38,
        RIGHT = 39,
        DOWN = 40,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        ALT = 164,
    };
}

namespace Input {
	bool GetKeyDown(int button) {
        for (int key = 0; key < 256; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (key == button) return true;
            }
        }

        return false;
	}

    bool GetKeyPressed(int button) {
        static bool keyStates[256] = { false }; // Initialize key states to false

        bool isPressed = (GetAsyncKeyState(button) & 0x8000) != 0;

        if (isPressed && !keyStates[button]) {
            // Key was not pressed in the previous iteration but is pressed now
            keyStates[button] = true;
            return true;
        }
        else if (!isPressed) {
            // Key is not pressed in the current iteration
            keyStates[button] = false;
        }

        return false;
    }

    bool GetKeyReleased(int button) {
        static bool keyStates[256] = { false }; // Initialize key states to false

        bool isPressed = (GetAsyncKeyState(button) & 0x8000) != 0;

        if (!isPressed && keyStates[button]) {
            // Key was pressed in the previous iteration but is not pressed now
            keyStates[button] = false;
            return true;
        }
        else if (isPressed) {
            // Key is pressed in the current iteration
            keyStates[button] = true;
        }

        return false;
    }
}