

#ifndef _DRAWING_HPP
#define _DRAWING_HPP
#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#define DIRECTINPUT_VERSION 0x0800
#include "imgui/imgui.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class Drawing
{
private:
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
public:
	bool Initialize();
	void Shutdown();

	auto text(ImVec2 v2_pos, std::string str_text, ImVec4 v4_col, bool b_outline, float font_size = 0.f) -> void;
	auto ToImVec(float* color)->ImVec4;
	auto line(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness) -> void;
	auto line(RECT rect, ImVec4 v4_col, float f_thickness) -> void;
	auto rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void;
	auto rect(RECT rect, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void;
	auto crosshair(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void;
	auto filled_rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, bool b_outline) -> void;
	auto filled_rect(RECT rect, ImVec4 v4_col, bool b_outline) -> void;
	auto circle(ImVec2 v2_pos, float f_radius, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void;
	auto crosshair3(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void;
	auto crosshair2(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void;
	auto crosshair4(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void;
private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	HWND h_Game;
	HWND h_hWnd;
	bool b_Shutdown = false;

	ImFont* font1;
	ImFont* font2;
	ImFont* font3;
private:
	WNDCLASSEX wc;

	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	float fade = 0.f;
};
extern Drawing* drawing;

#endif // _DRAWING_HPP