/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#include "../../include.hpp"
#include "../../drawing/imgui/imgui.h"
#include "visuals.hpp"
#include "../../globals/globals.hpp"
#include "../../drawing/drawing.hpp"
#include "../../drawing/imgui/imgui_internal.h"
ImGuiContext& g = *GImGui;

int iterations = 1;

/* call this in the ImGui::Draw Loop <3 */
auto Visuals::DrawPlayers() -> void
{

}

void Visuals::DrawCornerBox(int X, int Y, int W, int H, ImVec4 inline_color, ImVec4 outline_color)
{
	float lineW = (W / 5);
	float lineH = (H / 6);
	float lineT = 1;

	/* outline */
	{
		/* bottom right */
		drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X + lineW, Y - lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X - lineT, Y + lineH }, outline_color, 1.5f);

		/* top right */
		drawing->line(ImVec2{ X - lineT, Y + H - lineH }, ImVec2{ X - lineT, Y + H + lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X - lineT, Y + H + lineT }, ImVec2{ X + lineW, Y + H + lineT }, outline_color, 1.5f);

		/* bottom left */
		drawing->line(ImVec2{ X + W - lineW, Y - lineT }, ImVec2{ X + W + lineT, Y - lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X + W + lineT, Y - lineT }, ImVec2{ X + W + lineT, Y + lineH }, outline_color, 1.5f);

		/* top left */
		drawing->line(ImVec2{ X + W + lineT, Y + H - lineH }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X + W - lineW, Y + H + lineT }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f);
	}

	/* inline */
	{
		/* top left*/
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X + lineW), static_cast<LONG>(Y) }, inline_color, 1.f);

		/* top right*/
		drawing->line({ static_cast<LONG>(X + W - lineW), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X + W), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);

		/* bottom left */
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H), static_cast<LONG>(X + lineW), static_cast<LONG>(Y + H) }, inline_color, 1.f);

		/* bottom right */
		drawing->line({ static_cast<LONG>(X + W - lineW),  static_cast<LONG>(Y + H),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X + W),  static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
	}

}

auto Visuals::Draw() -> void
{
	/* Set ImGui Window Size */
	ImGui::SetNextWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));

	/* Set ImGui Window Pos */
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	/* Begin Drawing */
	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_None | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		DrawPlayers();
	}
	ImGui::End();
}

Visuals* visuals = new Visuals();