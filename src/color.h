#pragma once
#include <iostream>

unsigned int GetBrightness(unsigned int color) {
    // Extract the RGB components from the color
    int red = (color >> 16) & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = color & 0xFF;

    // Calculate brightness using the perceived brightness formula
    double brightness = 0.299 * red + 0.587 * green + 0.114 * blue;

    return static_cast<unsigned int>(brightness);
}

unsigned int InvertColor(unsigned int color) {
    // Extract the RGB components from the color
    int red = (color >> 16) & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = color & 0xFF;

    // Invert each component using bitwise NOT
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;

    // Combine the inverted components to form the new color
    return (red << 16) | (green << 8) | blue;
}

unsigned int BlendColors(unsigned int sourceColor, unsigned int destColor, double alpha) {
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

unsigned int HSLToRGB(int hue, int saturation, int lightness) {
    int r, g, b;

    if (saturation == 0) {
        // Achromatic (gray)
        r = g = b = (lightness * 255) / 100;
    }
    else {
        int C = (100 - abs(2 * lightness - 100)) * saturation / 100;
        int X = C * (1 - abs((hue / 60) % 2 - 1));
        int m = lightness - C / 2;

        switch (hue / 60) {
        case 0:
            r = C;
            g = X;
            b = 0;
            break;
        case 1:
            r = X;
            g = C;
            b = 0;
            break;
        case 2:
            r = 0;
            g = C;
            b = X;
            break;
        case 3:
            r = 0;
            g = X;
            b = C;
            break;
        case 4:
            r = X;
            g = 0;
            b = C;
            break;
        case 5:
            r = C;
            g = 0;
            b = X;
            break;
        }

        r = (r + m) * 255 / 100;
        g = (g + m) * 255 / 100;
        b = (b + m) * 255 / 100;
    }

    return (r << 16) | (g << 8) | b;
}

unsigned int HSLToRGB2(int hue, int saturation, int lightness) {
    // Ensure hue is within the valid range [0, 360]
    hue = (hue % 360 + 360) % 360;

    // Ensure saturation and lightness are within the valid range [0, 100]
    saturation = max(0, min(100, saturation));
    lightness = max(0, min(100, lightness));

    double h = static_cast<double>(hue) / 60.0;
    double s = static_cast<double>(saturation) / 100.0;
    double l = static_cast<double>(lightness) / 100.0;

    double c = (1.0 - std::abs(2.0 * l - 1.0)) * s;
    double x = c * (1.0 - std::abs(std::fmod(h, 2.0) - 1.0));
    double m = l - c / 2.0;

    double r, g, b;

    if (h >= 0.0 && h < 1.0) {
        r = c;
        g = x;
        b = 0.0;
    }
    else if (h >= 1.0 && h < 2.0) {
        r = x;
        g = c;
        b = 0.0;
    }
    else if (h >= 2.0 && h < 3.0) {
        r = 0.0;
        g = c;
        b = x;
    }
    else if (h >= 3.0 && h < 4.0) {
        r = 0.0;
        g = x;
        b = c;
    }
    else if (h >= 4.0 && h < 5.0) {
        r = x;
        g = 0.0;
        b = c;
    }
    else {
        r = c;
        g = 0.0;
        b = x;
    }

    unsigned int red = static_cast<unsigned int>((r + m) * 255.0);
    unsigned int green = static_cast<unsigned int>((g + m) * 255.0);
    unsigned int blue = static_cast<unsigned int>((b + m) * 255.0);

    return (red << 16) | (green << 8) | blue;
}

int RGBToHuee(unsigned int color) {
    int red = (color >> 16) & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = color & 0xFF;

    double r = static_cast<double>(red) / 255.0;
    double g = static_cast<double>(green) / 255.0;
    double b = static_cast<double>(blue) / 255.0;

    // Custom min and max functions
    double maxVal = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    double minVal = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);

    double hue = 0.0;

    if (maxVal == minVal) {
        return 0; // Hue is undefined for grayscale colors
    }
    else if (maxVal == r) {
        hue = 60.0 * (0.0 + (g - b) / (maxVal - minVal));
    }
    else if (maxVal == g) {
        hue = 60.0 * (2.0 + (b - r) / (maxVal - minVal));
    }
    else if (maxVal == b) {
        hue = 60.0 * (4.0 + (r - g) / (maxVal - minVal));
    }

    if (hue < 0.0) {
        hue += 360.0;
    }

    return static_cast<int>(hue);
}

int RGBToHue(unsigned int color) {
    int red = (color >> 16) & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = color & 0xFF;

    double r = static_cast<double>(red) / 255.0;
    double g = static_cast<double>(green) / 255.0;
    double b = static_cast<double>(blue) / 255.0;

    double maxVal = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    double minVal = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
    double hue = 0.0;

    if (maxVal == r) {
        hue = std::fmod(60.0 * ((g - b) / (maxVal - minVal)) + 360.0, 360.0);
    }
    else if (maxVal == g) {
        hue = std::fmod(60.0 * ((b - r) / (maxVal - minVal)) + 120.0, 360.0);
    }
    else if (maxVal == b) {
        hue = std::fmod(60.0 * ((r - g) / (maxVal - minVal)) + 240.0, 360.0);
    }

    return static_cast<int>(hue);
}