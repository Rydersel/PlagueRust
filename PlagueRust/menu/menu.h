

#pragma once
namespace mymenu {
	void autoDetection();
}
class Menu {

public:
	void Watermark();
	void WatermarkSolid();
	void Customize();
	void Render();
	void Shutdown();
	void ColorPicker(const char* name, float* color, bool alpha);
	ImFont* futura_large;
	ImFont* gotham;
	void Guns();
	void Config();
	void Keybinds();
	void Misc();
	void Crosshair();
	
	bool isOpen = false;
};

extern Menu* menu;