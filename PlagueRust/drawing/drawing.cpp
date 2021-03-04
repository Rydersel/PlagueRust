

#include "../include.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"
#include <tchar.h>
#include "drawing.hpp"
#include "../menu/menu.h"
#include "../globals/globals.hpp"
#include <future>
#include "../features/visuals/visuals.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include <io.h>

using namespace ImGui;
static bool isDown = true;
static bool isClicked = false;
bool Drawing::CreateDeviceD3D()
{
    DXGI_SWAP_CHAIN_DESC sdr;
    ZeroMemory(&sdr, sizeof(sdr));
    sdr.BufferCount = 2;
    sdr.BufferDesc.Width = 0;
    sdr.BufferDesc.Height = 0;
    sdr.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sdr.BufferDesc.RefreshRate.Numerator = 60;
    sdr.BufferDesc.RefreshRate.Denominator = 1;
    sdr.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sdr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sdr.OutputWindow = h_hWnd;
    sdr.SampleDesc.Count = 1;
    sdr.SampleDesc.Quality = 0;
    sdr.Windowed = TRUE;
    sdr.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sdr, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}


bool Drawing::Initialize()
{
    WINDOWPLACEMENT g_wpPrev;
    DWORD dwStyle = GetWindowLong(h_Game, GWL_STYLE);
    MONITORINFO mi = { sizeof(mi) };
    if (GetWindowPlacement(h_Game, &g_wpPrev) &&
        GetMonitorInfo(MonitorFromWindow(h_Game,
            MONITOR_DEFAULTTOPRIMARY), &mi)) {
        SetWindowLong(h_Game, GWL_STYLE,
            dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(h_Game, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    //Possible Detect Vector
    wc = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Internet Explorer"), NULL };
    RegisterClassEx(&wc);
    h_hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, _T(""), WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(h_hWnd, &margins);

    SetLayeredWindowAttributes(h_hWnd, 0, 1.0f, LWA_ALPHA);
    SetLayeredWindowAttributes(h_hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);


    if (!CreateDeviceD3D())
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(h_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(h_hWnd);

    SetForegroundWindow(h_hWnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplWin32_Init(h_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (b_Shutdown)
            break;

        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        menu->Watermark();


        if (menu->isOpen )
        {
            if (fade < 0.5)
                fade += 0.01f;


            menu->Render();
        }
        else {
            if (fade > 0.f)
                fade -= 0.01f;
        }

        if (GetAsyncKeyState(VK_INSERT)) {
            isClicked = false;
            isDown = true;
        }
        else if (!GetAsyncKeyState(VK_INSERT) && isDown) {
            isClicked = true;
            isDown = false;
        }
        else {
            isClicked = false;
            isDown = false;
        }

        if (isClicked) { 
            menu->isOpen = !menu->isOpen;
            long style = GetWindowLongPtr(h_hWnd, GWL_EXSTYLE);
            if (menu->isOpen) {
                style &= ~WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
                SetForegroundWindow(h_hWnd);
            }
            else {
                style |= WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
                SetForegroundWindow(h_Game);
            }
        }

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        if(settings->globalvars.isNightMode)
            clear_color = ImVec4(0.0667f, 0.067f, 0.0667f, fade);
        else if(!settings->globalvars.isNightMode)
            clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(0, 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    Shutdown();
    return 0;
}

LRESULT WINAPI Drawing::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) 
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    default: break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


void Drawing::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(h_hWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    std::exit(0);
}


void Drawing::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}


auto Drawing::CleanupRenderTarget() -> void
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}


auto Drawing::CleanupDeviceD3D() -> void
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

auto Drawing::text(ImVec2 v2_pos, std::string str_text, ImVec4 v4_col, bool b_outline, float font_size) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const char* text_end = str_text.c_str() + strlen(str_text.c_str());
    const char* text_display_end = FindRenderedTextEnd(str_text.c_str(), text_end);

    if (font_size == 0.f) font_size = g.FontSize;

    if (b_outline) {
        window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(1, 0), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(0, 1), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(1, 0), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(0, 1), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
    }
    window->DrawList->AddText(g.Font, font_size, v2_pos, GetColorU32(v4_col), str_text.c_str(), NULL, 0.0f, NULL);
}

auto Drawing::ToImVec(float* color) -> ImVec4
{
    return ImVec4(color[0], color[1], color[2], color[3]);
}

auto Drawing::line(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness) -> void
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    window->DrawList->AddLine(v2_pos, v2_size, ImGui::GetColorU32(v4_col), f_thickness);
}

auto Drawing::line(RECT rect, ImVec4 v4_col, float f_thickness) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect line_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    window->DrawList->AddLine(ImVec2(line_bb.Min.x, line_bb.Min.y), ImVec2(line_bb.Max.x, line_bb.Max.y), GetColorU32(v4_col), f_thickness);
}

auto Drawing::rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(v2_pos, v2_pos + v2_size);

    if (b_outline) {
        window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f, 15, f_thickness);
}

auto Drawing::rect(RECT rect, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    if (b_outline) {
        window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f, 15, f_thickness);
}

auto Drawing::filled_rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, bool b_outline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(v2_pos, v2_pos + v2_size);

    if (b_outline) {
        window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f);
    }
    window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f);
}

auto Drawing::filled_rect(RECT rect, ImVec4 v4_col, bool b_outline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    if (b_outline) {
        window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f);
    }
    window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f);
}

auto Drawing::circle(ImVec2 v2_pos, float f_radius, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (b_outline) {
        window->DrawList->AddCircle(v2_pos, f_radius + 1.0f, GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddCircle(v2_pos, f_radius - 1.0f, GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
    }
    window->DrawList->AddCircle(v2_pos, f_radius, GetColorU32(v4_col), 50, f_thickness);
}

auto Drawing::crosshair(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    int CenterX = GetSystemMetrics(0) / 2 - 1;
    int CenterY = GetSystemMetrics(1) / 2 - 1;

    rect(ImVec2(CenterX - (len * 2) + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);
    rect(ImVec2(CenterX + len + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);
    circle(ImVec2(CenterX, CenterY), 1, ImVec4(0.f, 0.f, 0.f, 1.f), 0.5, true, true);
}
auto Drawing::crosshair2(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    int CenterX = GetSystemMetrics(0) / 2 - 1;
    int CenterY = GetSystemMetrics(1) / 2 - 1;

    circle(ImVec2(CenterX, CenterY), 3, ImVec4(255.f, 255.f, 255.f, 1.f), 0.0, false, false);
}
auto Drawing::crosshair3(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    int CenterX = GetSystemMetrics(0) / 2 - 1;
    int CenterY = GetSystemMetrics(1) / 2 - 1;

    line(ImVec2(CenterX - 10, CenterY), ImVec2(CenterX + 10, CenterY), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
    line(ImVec2(CenterX, CenterY - 10), ImVec2(CenterX, CenterY + 10), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
}
auto Drawing::crosshair4(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    int CenterX = GetSystemMetrics(0) / 2 - 1;
    int CenterY = GetSystemMetrics(1) / 2 - 1;

    circle(ImVec2(CenterX - 10, CenterY - 10), 10, ImVec4(255.f, 255.f, 255.f, 1.f), 0.0, false, false);
    circle(ImVec2(CenterX + 10, CenterY - 10), 10, ImVec4(255.f, 255.f, 255.f, 1.f), 0.0, false, false);
    line(ImVec2(CenterX - 10, CenterY - 20), ImVec2(CenterX - 10, CenterY - 100), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
    line(ImVec2(CenterX + 10, CenterY - 20), ImVec2(CenterX + 10, CenterY - 100), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
    line(ImVec2(CenterX - 10, CenterY - 80), ImVec2(CenterX + 10, CenterY - 80), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
    line(ImVec2(CenterX - 10, CenterY - 100), ImVec2(CenterX + 10, CenterY - 100), ImVec4(255.f, 255.f, 255.f, 1.f), 2);\
    line(ImVec2(CenterX, CenterY - 100), ImVec2(CenterX, CenterY - 90), ImVec4(255.f, 255.f, 255.f, 1.f), 2);
}

Drawing* drawing = new Drawing();