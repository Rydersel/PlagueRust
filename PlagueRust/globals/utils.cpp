#include "../include.hpp"
#include "../classes/math/math.hpp"
#include "globals.hpp"

std::pair<float, float> Utils::getWindowSize(HWND window)
{
	RECT rect;
	if (GetWindowRect(window, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		return { width, height };
	}
	return { 0, 0 };
}

float* Utils::color_cycle()
{
    switch (color_iteration)
    {
    case 0:
        color_hue[1]++; /* 255, ++, 0 */

        if (color_hue[1] == 255)
            color_iteration++;
        break;
    case 1:
        color_hue[0]--; 

        if (color_hue[0] == 0)
            color_iteration++;
        break;
    case 2:
        color_hue[2]++; 

        if (color_hue[2] == 255)
            color_iteration++;
        break;
    case 3:
        color_hue[1]--; 

        if (color_hue[1] == 0)
            color_iteration++;
        break;
    case 4:
        color_hue[0]++; 

        if (color_hue[0] == 255)
            color_iteration++;
        break;
    case 5:
        color_hue[2]--; 

        if (color_hue[2] == 0)
            color_iteration++;
        break;
    case 6:
        color_iteration = 0;
    }

    float f_color[4] = { color_hue[0] / 255.f, color_hue[1] / 255.f, color_hue[2] / 255.f, 1.f };


    return f_color;
}

std::string Utils::wstring_to_string(const std::wstring& wstring)
{
	int slength = (int)wstring.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, &r[0], len, 0, 0);
	return r;
}



Utils* utils = new Utils();
Settings* settings = new Settings();