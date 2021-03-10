
#include "../include.hpp"
#include "Menu.h"
#include "MenuControls.h"
#include <iostream>
#include "../globals/globals.hpp"
#include <sstream>
#include "../drawing/imgui/imgui.h"
#include "../drawing/imgui/imgui_impl_dx11.h"
#include "../drawing/imgui/imgui_impl_win32.h"
#include "../drawing/imgui/imgui_internal.h"
#include <inttypes.h>
#include "../drawing/drawing.hpp"
#include "../Plague.hpp"
#include <iostream>
#include <Lmcons.h>
#include <time.h>
#include "menu.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
std::string config;
extern ImFont* menuFont{};
extern ImFont* keybindsFont{};
extern ImFont* tabFont{};
int corner;
extern ImFont* tabFont2;
extern ImFont* tabFont3;
extern ImFont* controlFont{};

extern ID3D11Texture1D* menuBg{};
int xControlP = 100;
int yControlP = 100;
static int tab = 0;



extern bool unload;
int isCountingAmmo = 1;
void loadConfig() {
	char takenstring[100];
	GetPrivateProfileString(TEXT("Bindings"), TEXT("akbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.akKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("lrbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.lrKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("mp5bind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.mp5Keybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("smgbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.smgKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("thompsonbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.thompsonKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("semibind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.semiKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("m249bind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.m249Keybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("pythonbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.pythonKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("revolverbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.revolverKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("p250bind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.p250Keybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("holobind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.holoKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("simplebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.simpleKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("8xbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.x8Keybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("muzzlebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.muzzleKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("silencebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.silenceKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("furnacebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.furnaceKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("lfurnacebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.lfurnaceKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("upgradebind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.upgradeKeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("codelockbind"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->keybinds.codelockkeybind = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("sens"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	uSens = std::stod(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("rando"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	oRandom = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("xrando"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	xControlP = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("yrando"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	yControlP = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("cross"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	settings->visuals.crosshair.crossnum = atoi(takenstring);
	GetPrivateProfileString(TEXT("Bindings"), TEXT("ammocount"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
	isCountingAmmo = atoi(takenstring);
}
void saveConfig() {
	config = "[Bindings]\n akbind = " + std::to_string(settings->keybinds.akKeybind) + "\nlrbind = " + std::to_string(settings->keybinds.lrKeybind) + "\nmp5bind = " + std::to_string(settings->keybinds.mp5Keybind) + "\nsmgbind = " + std::to_string(settings->keybinds.smgKeybind) + "\nthompsonbind = " + std::to_string(settings->keybinds.thompsonKeybind) + "\nsemibind = " + std::to_string(settings->keybinds.semiKeybind) + "\nm249bind = " + std::to_string(settings->keybinds.m249Keybind) + "\npythonbind = " + std::to_string(settings->keybinds.pythonKeybind) + "\nrevolverbind = "
		+ std::to_string(settings->keybinds.revolverKeybind) + "\np250bind = " + std::to_string(settings->keybinds.p250Keybind) + "\nholobind = " + std::to_string(settings->keybinds.holoKeybind) + "\nsimplebind = " + std::to_string(settings->keybinds.simpleKeybind) + "\n8xbind = " + std::to_string(settings->keybinds.x8Keybind) + "\nmuzzlebind = " + std::to_string(settings->keybinds.muzzleKeybind) + "\nsilencebind = " + std::to_string(settings->keybinds.silenceKeybind) + "\nfurnacebind = " + std::to_string(settings->keybinds.furnaceKeybind) + "\nlfurnacebind = "
		+ std::to_string(settings->keybinds.lfurnaceKeybind) + "\nupgradebind = " + std::to_string(settings->keybinds.upgradeKeybind) + "\ncodelockbind = " + std::to_string(settings->keybinds.codelockkeybind) + "\nsens = " + std::to_string(uSens) + "\nrando = " + std::to_string(oRandom) + "\nxrando = " + std::to_string(xControlP) + "\nyrando = " + std::to_string(yControlP) + "\ncross = " + std::to_string(settings->visuals.crosshair.crossnum) + "\nammocount = " +std::to_string(isCountingAmmo);
	std::ofstream file_;
	file_.open("Config.inv");
	file_ << config;
	file_.close();
}
const char* mykeys[] = {
	"[-]",
	"[M1]",
	"[M2]",
	"[CN]",
	"[M3]",
	"[M4]",
	"[M5]",
	"[-]",
	"[BAC]",
	"[TAB]",
	"[-]",
	"[-]",
	"[CLR]",
	"[RET]",
	"[-]",
	"[-]",
	"[SHI]",
	"[CTL]",
	"[MEN]",
	"[PAU]",
	"[CAP]",
	"[KAN]",
	"[-]",
	"[JUN]",
	"[FIN]",
	"[KAN]",
	"[-]",
	"[ESC]",
	"[CON]",
	"[NCO]",
	"[ACC]",
	"[MAD]",
	"[SPA]",
	"[PGU]",
	"[PGD]",
	"[END]",
	"[HOM]",
	"[LEF]",
	"[UP]",
	"[RIG]",
	"[DOW]",
	"[SEL]",
	"[PRI]",
	"[EXE]",
	"[PRI]",
	"[INS]",
	"[DEL]",
	"[HEL]",
	"[0]",
	"[1]",
	"[2]",
	"[3]",
	"[4]",
	"[5]",
	"[6]",
	"[7]",
	"[8]",
	"[9]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[A]",
	"[B]",
	"[C]",
	"[D]",
	"[E]",
	"[F]",
	"[G]",
	"[H]",
	"[I]",
	"[J]",
	"[K]",
	"[L]",
	"[M]",
	"[N]",
	"[O]",
	"[P]",
	"[Q]",
	"[R]",
	"[S]",
	"[T]",
	"[U]",
	"[V]",
	"[W]",
	"[X]",
	"[Y]",
	"[Z]",
	"[WIN]",
	"[WIN]",
	"[APP]",
	"[-]",
	"[SLE]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[NUM]",
	"[MUL]",
	"[ADD]",
	"[SEP]",
	"[MIN]",
	"[DEC]",
	"[DIV]",
	"[F1]",
	"[F2]",
	"[F3]",
	"[F4]",
	"[F5]",
	"[F6]",
	"[F7]",
	"[F8]",
	"[F9]",
	"[F10]",
	"[F11]",
	"[F12]",
	"[F13]",
	"[F14]",
	"[F15]",
	"[F16]",
	"[F17]",
	"[F18]",
	"[F19]",
	"[F20]",
	"[F21]",
	"[F22]",
	"[F23]",
	"[F24]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[NUM]",
	"[SCR]",
	"[EQU]",
	"[MAS]",
	"[TOY]",
	"[OYA]",
	"[OYA]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[-]",
	"[SHI]",
	"[SHI]",
	"[CTR]",
	"[CTR]",
	"[ALT]",
	"[ALT]"
};


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}



void mouse_move() {
	if (isFurnace) {
		SetCursorPos(826, 500);
		Sleep(100);
		// first slot
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1380, 700);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		// first slot done
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1450, 700);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(200);
		SetCursorPos(1150, 500);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(200);
		SetCursorPos(1200, 500);
		Sleep(200);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1615, 700);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}
bool ammoCount() {
	settings->globalvars.weaponAmmo -= 1;
	if (isCountingAmmo == 0)
		return true;
	if (settings->globalvars.weaponAmmo <= 0)
		return false;
	return true;
}
void autoCode() {
	int multiple = 0;
	int leftover = 0;
	if (!settings->features.uSound) {
		settings->features.uSound = false;
		multiple = 1;
	}
	leftover = CodeLockNum / 1000;
	keybd_event((96 + leftover), 0, 0, 0);
	keybd_event((96 + leftover), KEYEVENTF_KEYUP, 0, 0);
	Sleep(100);
	leftover = ((CodeLockNum % 1000) / 100);
	keybd_event((96 + leftover), 0, 0, 0);
	keybd_event((96 + leftover), KEYEVENTF_KEYUP, 0, 0);
	Sleep(100);
	leftover = ((CodeLockNum % 100) / 10);
	keybd_event((96 + leftover), 0, 0, 0);
	keybd_event((96 + leftover), KEYEVENTF_KEYUP, 0, 0);
	Sleep(100);
	leftover = (CodeLockNum % 10);
	keybd_event((96 + leftover), 0, 0, 0);
	keybd_event((96 + leftover), KEYEVENTF_KEYUP, 0, 0);
	Sleep(100);
	if (multiple == 1) {
		settings->features.uSound = true;
	}
}
void largeFurnace() {
	if (islfurnace) {
		SetCursorPos(695, 500);
		Sleep(100);
		// first slot
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1390, 540);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(698, 500);
		Sleep(100);
		// first slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1485, 540);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(701, 500);
		Sleep(100);
		//second slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1575, 540);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(704, 500);
		Sleep(100);
		//third slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1670, 540);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(708, 500);
		Sleep(100);
		//fourth slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1765, 540);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(712, 500);
		Sleep(100);
		//Fifth slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1300, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(718, 500);
		Sleep(100);
		//Sixth slot
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1390, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(725, 500);
		Sleep(100);
		//Seventh Slot
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1485, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(734, 500);
		Sleep(100);
		//second slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1575, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(746, 500);
		Sleep(100);
		//third slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1670, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(762, 500);
		Sleep(100);
		//fourth slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1765, 630);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(786, 500);
		Sleep(100);
		//Fifth slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1300, 720);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(829, 500);
		Sleep(100);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1485, 720);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(912, 500);
		Sleep(100);
		//second slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1575, 720);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(707, 616);
		Sleep(100);
		//third slot done
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(50);
		SetCursorPos(1200, 500);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(30);
		SetCursorPos(1670, 720);
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}
void autoUpgrade() {
	if (bgradeNum == 1) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(1030, 245);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
	if (bgradeNum == 2) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(1250, 510);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
	if (bgradeNum == 3) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(1100, 800);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
	if (bgradeNum == 4) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		Sleep(20);
		SetCursorPos(650, 560);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(20);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
}
void ReplacementFor_Sound(bool type) {
	if (settings->features.uSound) {
		if (type == true) {
			switch (xSound) {
			case(true):
				Beep(1000, 200);
			}
		}
		else {
			switch (xSound) {
			case(true):
				Beep(1200, 200);
			}
		}
	}
		Sleep(120);
	
}
//Opens The Menu Form
void SleepFunc(int ms)
{
	LONGLONG ReplacementFor_timerResolution;
	LONGLONG ReplacementFor_wantedTime;
	LONGLONG ReplacementFor_currentTime;

	QueryPerformanceFrequency((LARGE_INTEGER*)&ReplacementFor_timerResolution);
	ReplacementFor_timerResolution /= 1000;

	QueryPerformanceCounter((LARGE_INTEGER*)&ReplacementFor_currentTime);
	ReplacementFor_wantedTime = ReplacementFor_currentTime / ReplacementFor_timerResolution + ms;
	ReplacementFor_currentTime = 0;
	while (ReplacementFor_currentTime < ReplacementFor_wantedTime)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&ReplacementFor_currentTime);
		ReplacementFor_currentTime /= ReplacementFor_timerResolution;
	}
}
//Randomization function
void xRandomization(double ReplacementFor_delay, double ReplacementFor_control_time, int x, int y) {
	double randomized = 0;
	double xrandomized = 0;
	double yrandomized = 0;
	if (oRandom > 0 && oRandom <= 100) {
		double randomized1 = (rand() % oRandom) / 5;
		double randomized2 = (rand() % oRandom) / 5;
		randomized = (randomized1 - randomized2);
	}
	else {
		randomized = 0;
	}
	if (xRandom > 0 && xRandom <= 100) {
		double randomized1 = (rand() % oRandom) / 5;
		double randomized2 = (rand() % oRandom) / 5;
		xrandomized = (randomized1 - randomized2);
	}
	else {
		xrandomized = 0;
	}
	if (xRandom > 0 && xRandom <= 100) {
		double randomized1 = (rand() % oRandom) / 5;
		double randomized2 = (rand() % oRandom) / 5;
		xrandomized = (randomized1 - randomized2);
	}
	else {
		xrandomized = 0;
	}
	int ReplacementFor_x_ = (randomized + xrandomized), ReplacementFor_y_ = (randomized + yrandomized), ReplacementFor_t_ = randomized;
	for (int i = 1; i <= (int)ReplacementFor_control_time; ++i) {
		int ReplacementFor_xI = i * x / (int)ReplacementFor_control_time;
		int ReplacementFor_yI = i * y / (int)ReplacementFor_control_time;
		int ReplacementFor_tI = i * (int)ReplacementFor_control_time / (int)ReplacementFor_control_time;
		mouse_event(1, (int)ReplacementFor_xI - (int)ReplacementFor_x_, (int)ReplacementFor_yI - (int)ReplacementFor_y_, 0, 0);
		SleepFunc((int)ReplacementFor_tI - (int)ReplacementFor_t_);
		ReplacementFor_x_ = ReplacementFor_xI; ReplacementFor_y_ = ReplacementFor_yI; ReplacementFor_t_ = ReplacementFor_tI;
	}
	SleepFunc((int)ReplacementFor_delay - (int)ReplacementFor_control_time);
}
// Muzzle Boost
int ReplacementFor_xMuzzleBoost(float Delay) {
	if (settings->features.xMuzzleBoost == true) {
		float ReplacementFor_difference = Delay * 0.1f;
		Delay = Delay - ReplacementFor_difference;
		return (int)Delay + 2;
	}
	else
		return (int)Delay;
}
//Attachments
double ReplacementFor_BarrelAttachment() {
	if (settings->features.xSuppressor == true) {
		if (settings->features.xM249 == true)
			Multiplication::userBarrel.ReplacementFor_Suppressor = 0.75;
		if (settings->features.xSMG == true)
			Multiplication::userBarrel.ReplacementFor_Suppressor = 0.85;
		if (settings->features.xSimple == true || settings->features.xHolo == true)
			return 0.75;
		if (settings->features.x8x == true && settings->features.xSuppressor == true)
			return 1.46;
		else
			return Multiplication::userBarrel.ReplacementFor_Suppressor;
	}
	else if (settings->features.xMuzzleBoost == true) {
		return Multiplication::userBarrel.ReplacementFor_None;
	}
	else
		return Multiplication::userBarrel.ReplacementFor_None;
}
double ReplacementFor_ScopeAttachment() {
	if (settings->features.x8x == true) {
		if (settings->features.xSMG == true || settings->features.xThompson == true) {
			if (settings->features.x8x == true)
				return Multiplication::userScope.ReplacementFor_x8 + 0.75;
		}
		// James Charles has a Giant Cock
		return Multiplication::userScope.ReplacementFor_x8;
	}
	else if (settings->features.xSimple == true) {
		if (settings->features.xSMG == true || settings->features.xThompson == true) {
			if (settings->features.xSimple == true)
				return Multiplication::userScope.Simple + 0.1;
		}
		return Multiplication::userScope.Simple;
	}
	else if (settings->features.xHolo == true) {
		if (settings->features.xSMG == true || settings->features.xThompson == true) {
			if (settings->features.xHolo == true)
				return Multiplication::userScope.ReplacementFor_Holo + 0.3;
		}
		return Multiplication::userScope.ReplacementFor_Holo;
	}
	else
		return Multiplication::userScope.ReplacementFor_None;
}

namespace ReplacementFor_CurrentWeapon {
	double x(int Velocity) {
			if (settings->features.xAK == true)
				return (((AKRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xLR == true)
				return (((LRRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xMP5 == true)
				return (((MP5Recoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xThompson == true)
				return (((ThompsonRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xSMG == true)
				return (((CustomRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xM249 == true)
				return (((M249Recoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xSemi == true)
				return (((SemiRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xPython == true)
				return (((PythonRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xRevolver == true)
				return (((RevolverRecoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else if (settings->features.xP250 == true)
				return (((P250Recoil[Velocity].x * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)xControlP / 100)) / uSens;
			else
				return 0;
	}
	double y(int Velocity) {
			if (settings->features.xAK == true)
				return (((AKRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xLR == true)
				return (((LRRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xMP5 == true)
				return (((MP5Recoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xThompson == true)
				return (((ThompsonRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xSMG == true)
				return (((CustomRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xM249 == true)
				return (((M249Recoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xSemi == true)
				return (((SemiRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xPython == true)
				return (((PythonRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xRevolver == true)
				return (((RevolverRecoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else if (settings->features.xP250 == true)
				return (((P250Recoil[Velocity].y * ReplacementFor_ScopeAttachment()) * ReplacementFor_BarrelAttachment()) / 4 * ((double)yControlP / 100)) / uSens;
			else
				return 0;

	}
	//Incorporates Delay from HPP file
	double Delay() {
		if (settings->features.xAK == true)
			return userWeaponData::ReplacementFor_AK47.Delay;
		else if (settings->features.xLR == true)
			return userWeaponData::ReplacementFor_LR300.Delay;
		else if (settings->features.xMP5 == true)
			return userWeaponData::ReplacementFor_MP5.Delay;
		else if (settings->features.xThompson == true)
			return userWeaponData::ReplacementFor_THOMPSON.Delay;
		else if (settings->features.xSMG == true)
			return userWeaponData::ReplacementFor_CUSTOM_SMG.Delay;
		else if (settings->features.xM249 == true)
			return userWeaponData::ReplacementFor_M249.Delay;
		else if (settings->features.xSemi == true)
			return userWeaponData::ReplacementFor_SEMI.Delay;
		else if (settings->features.xPython == true)
			return userWeaponData::ReplacementFor_Python.Delay;
		else if (settings->features.xRevolver == true)
			return userWeaponData::ReplacementFor_Revolver.Delay;
		else if (settings->features.xP250 == true)
			return userWeaponData::ReplacementFor_P250.Delay;
		else
			return 0;
	}

	double Control(int Velocity) {
		if (settings->features.xAK == true)
			return ReplacementFor_ControlTime_Ak47[Velocity];
		else if (settings->features.xLR == true)
			return userWeaponData::ReplacementFor_LR300.Delay;
		else if (settings->features.xMP5 == true)
			return userWeaponData::ReplacementFor_MP5.Delay;
		else if (settings->features.xThompson == true)
			return userWeaponData::ReplacementFor_THOMPSON.Delay;
		else if (settings->features.xSMG == true)
			return userWeaponData::ReplacementFor_CUSTOM_SMG.Delay;
		else if (settings->features.xM249 == true)
			return userWeaponData::ReplacementFor_M249.Delay;
		else if (settings->features.xSemi == true)
			return userWeaponData::ReplacementFor_SEMI.Delay;
		else if (settings->features.xPython == true)
			return userWeaponData::ReplacementFor_Python.Delay;
		else if (settings->features.xRevolver == true)
			return userWeaponData::ReplacementFor_Revolver.Delay;
		else if (settings->features.xP250 == true)
			return userWeaponData::ReplacementFor_P250.Delay;
		else
			return 0;
	}
	int Velocity() {
		if (settings->features.xAK == true)
			return AKRecoil.size() - 1;
		else if (settings->features.xLR == true)
			return LRRecoil.size() - 1;
		else if (settings->features.xMP5 == true)
			return MP5Recoil.size() - 1;
		else if (settings->features.xThompson == true)
			return ThompsonRecoil.size() - 1;
		else if (settings->features.xSMG == true)
			return CustomRecoil.size() - 1;
		else if (settings->features.xM249 == true)
			return M249Recoil.size() - 1;
		else if (settings->features.xSemi == true)
			return SemiRecoil.size() - 1;
		else if (settings->features.xPython == true) 
			return PythonRecoil.size() - 1;
		else if (settings->features.xRevolver == true)
			return RevolverRecoil.size() - 1;
		else if (settings->features.xP250 == true)
			return P250Recoil.size() - 1;
		else 
			return 0;
	}
	bool xActive() {
		if (settings->features.xAK == true || settings->features.xLR == true || settings->features.xMP5 == true || settings->features.xThompson == true || settings->features.xSMG == true || settings->features.xSemi == true || settings->features.xM249 == true || settings->features.xPython == true || settings->features.xRevolver == true || settings->features.xP250 == true)
			return true;
		else
			return false;
	}
	void ReplacementFor_RecoilBreak(int count) {
		if (settings->features.xSemi == true) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		else if (settings->features.xPython == true) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		else if (settings->features.xRevolver == true) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		else if (settings->features.xP250 == true) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		else if (settings->features.xSemi == true || settings->features.xPython == true || settings->features.xP250 == true || settings->features.xRevolver == true) {
			if (count == ReplacementFor_CurrentWeapon::Velocity()) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
		}
	}
};
//Where recoil actually takes place
bool ShootGun() {
	for (;;) {
		if (ReplacementFor_CurrentWeapon::xActive() == true)
		{
			if (settings->features.xM249 || settings->features.xSemi || settings->features.xPython) {
				if (settings->globalvars.isCrouched) {
					if (settings->features.isHipfire) {
						for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000; i++)
						{
							xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)(ReplacementFor_CurrentWeapon::x(i) * settings->globalvars.hipMult), (int)(ReplacementFor_CurrentWeapon::y(i) * settings->globalvars.hipMult));
							ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);
							

						}
						Sleep(0);
					}

					else {
						for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000; i++)
						{
							xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)ReplacementFor_CurrentWeapon::x(i), (int)ReplacementFor_CurrentWeapon::y(i));
							ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);
						}
						Sleep(0);
					}
				}
				else {
					if (settings->features.isHipfire) {
						for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000; i++)
						{
							xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)(ReplacementFor_CurrentWeapon::x(i) * settings->globalvars.hipMult * 2), (int)(ReplacementFor_CurrentWeapon::y(i) * settings->globalvars.hipMult * 2));
							ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);

						}
						Sleep(0);
					}

					else {
						for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000; i++)
						{
							xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)(ReplacementFor_CurrentWeapon::x(i) * 2), (int)(ReplacementFor_CurrentWeapon::y(i) * 2));
							ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);
						}
						Sleep(0);
					}
				}
			}
			else {
				if (settings->features.isHipfire) {
					for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000 && ammoCount(); i++)
					{
						xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)(ReplacementFor_CurrentWeapon::x(i) * settings->globalvars.hipMult), (int)(ReplacementFor_CurrentWeapon::y(i) * settings->globalvars.hipMult));
						ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);

					}
					Sleep(0);
				}

				else {
					for (int i = 0; i <= ReplacementFor_CurrentWeapon::Velocity() && GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000 && ammoCount(); i++)
					{
						xRandomization((int)ReplacementFor_xMuzzleBoost((float)ReplacementFor_CurrentWeapon::Delay()), (int)ReplacementFor_xMuzzleBoost(ReplacementFor_CurrentWeapon::Control(i)), (int)ReplacementFor_CurrentWeapon::x(i), (int)ReplacementFor_CurrentWeapon::y(i));
						ReplacementFor_CurrentWeapon::ReplacementFor_RecoilBreak(i);
					}
					Sleep(0);
				}
			}
		}
		Sleep(0);
	}
	return false;
}
void Keybinds() {

	for (;;) {
		if (GetAsyncKeyState(82) != 0) {
			settings->globalvars.isReload = true;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
		}
		else {
			settings->globalvars.isReload = false;
		}
		if (GetAsyncKeyState(settings->keybinds.akKeybind) != 0) {
			settings->features.xAK = !settings->features.xAK;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xAK == true) {
				userWeapon = "AK47";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 0;

			}
			else if (settings->features.xAK == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xAK);
		}
		if (GetAsyncKeyState(settings->keybinds.lrKeybind) != 0) {
			settings->features.xLR = !settings->features.xLR;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xLR == true) {
				userWeapon = "LR300";
				settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 1;
			}
			else if (settings->features.xLR == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xLR);
		}
		if (GetAsyncKeyState(settings->keybinds.mp5Keybind) != 0) {
			settings->features.xMP5 = !settings->features.xMP5;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xMP5 == true) {
				userWeapon = "MP5A4";
				settings->features.xLR = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 2;
			}
			else if (settings->features.xMP5 == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xMP5);
		}
		if (GetAsyncKeyState(settings->keybinds.smgKeybind) != 0) {
			settings->features.xSMG = !settings->features.xSMG;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xSMG == true) {
				userWeapon = "CUSTOM_SMG";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 3;
			}
			else if (settings->features.xSMG == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
			}
			ReplacementFor_Sound(settings->features.xSMG);
		}
		if (GetAsyncKeyState(settings->keybinds.thompsonKeybind) != 0) {
			settings->features.xThompson = !settings->features.xThompson;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xThompson == true) {
				userWeapon = "THOMPSON";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 4;
			}
			else if (settings->features.xThompson == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xThompson);
		}
		if (GetAsyncKeyState(settings->keybinds.semiKeybind) != 0) {
			settings->features.xSemi = !settings->features.xSemi;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xSemi == true) {
				userWeapon = "SEMI-AUTO-RIFLE";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xM249 = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 5;
			}
			else if (settings->features.xSemi == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xSemi);
		}
		if (GetAsyncKeyState(settings->keybinds.m249Keybind) != 0) {
			settings->features.xM249 = !settings->features.xM249;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xM249 == true) {
				userWeapon = "M249";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xAK = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 6;
			}
			else if (settings->features.xM249 == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xM249);
		}
		if (GetAsyncKeyState(settings->keybinds.pythonKeybind) != 0) {
			settings->features.xPython = !settings->features.xPython;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xPython == true) {
				userWeapon = "Python";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xAK = false; settings->features.xM249 = false; settings->features.xRevolver = false; settings->features.xP250 = false;
				selectedItem = 7;
			}
			else if (settings->features.xPython == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xPython);
		}
		if (GetAsyncKeyState(settings->keybinds.revolverKeybind) != 0) {
			settings->features.xRevolver = !settings->features.xRevolver;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xRevolver == true) {
				userWeapon = "Revolver";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xAK = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xP250 = false;
				selectedItem = 8;
			}
			else if (settings->features.xRevolver == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xRevolver);
		}
		if (GetAsyncKeyState(settings->keybinds.p250Keybind) != 0) {
			settings->features.xP250 = !settings->features.xP250;
			settings->globalvars.weaponAmmo = ReplacementFor_CurrentWeapon::Velocity() + 2;
			if (settings->features.xP250 == true) {
				selectedItem = 9;
				userWeapon = "P250";
				settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xAK = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false;
			}
			else if (settings->features.xP250 == false) {
				userWeapon = "NONE"; userScope = "NONE"; userBarrel = "NONE";
				settings->features.x8x = false; settings->features.xHolo = false; settings->features.xSimple = false; settings->features.xSuppressor = false;
				selectedItem = 10;
				selectedScope = 3;
				selectedBarrel = 2;
			}
			ReplacementFor_Sound(settings->features.xP250);
		}
		while (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			settings->globalvars.hipMult = 1;
		}
		settings->globalvars.hipMult = 0.6;
		if (var) {
			if (GetAsyncKeyState(settings->keybinds.x8Keybind) != 0) {
				settings->features.x8x = !settings->features.x8x;
				switch (settings->features.x8x) {
				case(true):
					userScope = "8x"; settings->features.xHolo = false; settings->features.xSimple = false;
					selectedScope = 2;
					break;
				case(false):
					userScope = "NONE";
					selectedScope = 3;
					break;
				}
				ReplacementFor_Sound(settings->features.x8x);
			}
			if (GetAsyncKeyState(settings->keybinds.simpleKeybind) != 0) {
				settings->features.xSimple = !settings->features.xSimple;
				if (settings->features.xSimple == true) {
					selectedScope = 1;
					userScope = "SIMPLE"; settings->features.x8x = false; settings->features.xHolo = false;
				}
				else if (settings->features.xSimple == false) {
					userScope = "NONE";
					selectedScope = 3;
				}
				ReplacementFor_Sound(settings->features.xSimple);
			}
			if (GetAsyncKeyState(settings->keybinds.holoKeybind) != 0) {
				settings->features.xHolo = !settings->features.xHolo;
				if (settings->features.xHolo == true) {
					selectedScope = 0;
					userScope = "HOLO"; settings->features.x8x = false; settings->features.xSimple = false;
				}
				else if (settings->features.xHolo == false) {
					userScope = "NONE";
					selectedScope = 3;
				}
				ReplacementFor_Sound(settings->features.xHolo);
			}
			if (GetAsyncKeyState(settings->keybinds.silenceKeybind) != 0) {
				settings->features.xSuppressor = !settings->features.xSuppressor;
				if (settings->features.xSuppressor == true) {
					selectedBarrel = 1;
					userBarrel = "SILENCER"; settings->features.xMuzzleBoost = false;
				}
				else if (settings->features.xSuppressor == false)
					selectedBarrel = 2;
				userBarrel = "NONE";
				ReplacementFor_Sound(settings->features.xSuppressor);
			}
			if (GetAsyncKeyState(settings->keybinds.muzzleKeybind) != 0) {
				settings->features.xMuzzleBoost = !settings->features.xMuzzleBoost;
				if (settings->features.xMuzzleBoost == true) {
					userBarrel = "Boost"; settings->features.xSuppressor = false;
					selectedBarrel = 0;
				}
				else if (settings->features.xMuzzleBoost == false)
					selectedBarrel = 2;
				userBarrel = "NONE";
				ReplacementFor_Sound(settings->features.xMuzzleBoost);
			}
						if (scopes[selectedScope] == "Holo") {
				settings->features.xHolo = true; settings->features.x8x = false; settings->features.xSimple = false;
			}
			if (scopes[selectedScope] == "Simple") {
				settings->features.xHolo = false; settings->features.x8x = false; settings->features.xSimple = true;
			}
			if (scopes[selectedScope] == "8x") {
				settings->features.xHolo = false; settings->features.x8x = true; settings->features.xSimple = false;
			}
			if (scopes[selectedScope] == "None") {
				settings->features.xHolo = false; settings->features.x8x = false; settings->features.xSimple = false;
			}
			//Barrel
			if (barrel[selectedBarrel] == "Boost") {
				settings->features.xMuzzleBoost = true; settings->features.xSuppressor = false;
			}
			if (barrel[selectedBarrel] == "Suppresor") {
				settings->features.xMuzzleBoost = false; settings->features.xSuppressor = true;
			}
			if (barrel[selectedBarrel] == "None") {
				settings->features.xMuzzleBoost = false; settings->features.xSuppressor = false;
			}
			//Guns
			if (items[selectedItem] == "AK47") {
				settings->features.xAK = true;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "LR-300") {
				settings->features.xAK = false;  settings->features.xLR = true; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "MP5") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = true; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "SMG") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = true; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "Thompson") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = true; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "Semi") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = true; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "M249") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = true; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "Python") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = true; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "Revolver") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = true; settings->features.xP250 = false;
			}
			else if (items[selectedItem] == "P250") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = true;
			}
			else if (items[selectedItem] == "None") {
				settings->features.xAK = false;  settings->features.xLR = false; settings->features.xMP5 = false; settings->features.xSMG = false; settings->features.xThompson = false; settings->features.xSemi = false; settings->features.xM249 = false; settings->features.xPython = false; settings->features.xRevolver = false; settings->features.xP250 = false;
			}
		}
		if (GetAsyncKeyState(settings->keybinds.furnaceKeybind) != 0) {
			mouse_move();
		}
		if (GetAsyncKeyState(settings->keybinds.upgradeKeybind) & 0x8000) {
			autoUpgrade();
		}
		if (GetAsyncKeyState(settings->keybinds.lfurnaceKeybind) & 0x8000)
			largeFurnace();
		if (GetAsyncKeyState(VK_ADD) != 0) {
			xSound = !xSound;
			if (xSound == true) {
				userSound = "ON";
			}
			else if (xSound == false) {
				userSound = "OFF";
			}
			ReplacementFor_Sound(xSound);
		}
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			if (isAutofire == true)
			{
				if (settings->features.xSemi == true) {
					for (int i = 0; i < 16; i++) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(20);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						Sleep(154);
					}
				}
				if (settings->features.xP250 == true) {
					for (int i = 0; i < 10; i++) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(20);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						Sleep(130);
					}
				}
				if (settings->features.xRevolver == true) {
					for (int i = 0; i < 8; i++) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(20);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						Sleep(154);
					}
				}
				if (settings->features.xPython == true) {
					for (int i = 0; i < 6; i++) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(20);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						Sleep(130);
					}
				}
			}
		}
		Sleep(1);
	}
}

void Menu::ColorPicker(const char* name, float* color, bool alpha) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiStyle* style = &ImGui::GetStyle();

	auto alphaSliderFlag = alpha ? ImGuiColorEditFlags_AlphaBar : ImGuiColorEditFlags_NoAlpha;

	ImGui::SameLine(219.f);
	ImGui::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
}
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
static bool isDown = true;

void Menu::Customize()
	
{
	ImGuiStyle* style = &ImGui::GetStyle();
	static ImVec4 textcolor = ImColor(213, 213, 213, 255);
	static ImVec4 buttoncolor = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	static ImVec4 WindowColor = ImColor(40, 40, 40, 255);
	static ImVec4 TabTextColor = ImColor(100, 100, 100, 255);
	static ImVec4 FrameBg = ImColor(32, 32, 38, 255);
	if (settings->visuals.CustomizeMenu.enabled) {

		ImGui::PushFont(menuFont);

		ImGui::SetNextWindowSize(ImVec2(200.f, 330.f));
		ImGui::Begin("Customize", &isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiConfigFlags_NavEnableKeyboard | ImGuiWindowFlags_NoScrollWithMouse); {
			{
				InsertCheckbox("Rainbow Mode", settings->visuals.CustomizeMenu.rainbowmode); style->ItemSpacing = ImVec2(8, 2);
				ImGui::ColorEdit3("Text Color##2", (float*)&textcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar); style->WindowPadding = ImVec2(8, 2);
				ImGui::GetStyle().Colors[ImGuiCol_Text] = textcolor; style->WindowPadding = ImVec2(8, 2);
				ImGui::ColorEdit3("Button Color##2", (float*)&buttoncolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar); style->WindowPadding = ImVec2(8, 2);
				ImGui::GetStyle().Colors[ImGuiCol_Button] = buttoncolor; style->WindowPadding = ImVec2(8, 2);
				ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = buttoncolor; style->WindowPadding = ImVec2(8, 2);
				ImGui::ColorEdit3("Window##2", (float*)&WindowColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar); style->WindowPadding = ImVec2(8, 2);
				ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = WindowColor; style->WindowPadding = ImVec2(8, 2);
				ImGui::ColorEdit3("Tab Text##2", (float*)&TabTextColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar); style->WindowPadding = ImVec2(8, 2);
				ImGui::GetStyle().Colors[ImGuiCol_TabText] = TabTextColor, style->WindowPadding = ImVec2(8, 2);


				if (ImGui::Button("Close")) {
					settings->visuals.CustomizeMenu.enabled = false;

				}
				if (ImGui::Button("Reset")) {
					settings->visuals.CustomizeMenu.enabled = false;
					ImGui::GetStyle().Colors[ImGuiCol_Text] = ImColor(213, 213, 213, 255); style->WindowPadding = ImVec2(8, 2);
					ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f); style->WindowPadding = ImVec2(8, 2);
					ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor(40, 40, 40, 255); style->WindowPadding = ImVec2(8, 2);
					ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(32, 32, 38, 255); style->WindowPadding = ImVec2(8, 2);
					ImGui::GetStyle().Colors[ImGuiCol_TabText] = ImColor(100, 100, 100, 255); style->WindowPadding = ImVec2(8, 2);
					ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImColor(32, 32, 38, 255); style->WindowPadding = ImVec2(8, 2);
					static ImVec4 textcolor = ImColor(213, 213, 213, 255);
					static ImVec4 buttoncolor = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
					static ImVec4 WindowColor = ImColor(40, 40, 40, 255);
					static ImVec4 TabTextColor = ImColor(100, 100, 100, 255);
					static ImVec4 FrameBg = ImColor(32, 32, 38, 255);
				}



			}
			ImGui::End();
		}
	}

}

void Menu::Watermark()
{
	ImGui::SetNextWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_None | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		
		ImGuiStyle* style = &ImGui::GetStyle();
		if (settings->visuals.crosshair.crossnum == 1) {
			drawing->crosshair(ImVec4(1.f, 1.f, 1.f, 1.f), 1.f, settings->visuals.crosshair.size, settings->visuals.crosshair.x_offset, settings->visuals.crosshair.y_offset);
		}
		else if (settings->visuals.crosshair.crossnum == 2) {
			drawing->crosshair2(ImVec4(1.f, 1.f, 1.f, 1.f), 1.f, settings->visuals.crosshair.size, settings->visuals.crosshair.x_offset, settings->visuals.crosshair.y_offset);
		}
		else if (settings->visuals.crosshair.crossnum == 3) {
			drawing->crosshair3(ImVec4(1.f, 1.f, 1.f, 1.f), 1.f, settings->visuals.crosshair.size, settings->visuals.crosshair.x_offset, settings->visuals.crosshair.y_offset);
		}
		else if (settings->visuals.crosshair.crossnum == 4) {
			drawing->crosshair4(ImVec4(1.f,1.f,1.f,1.f), 1.f, settings->visuals.crosshair.size, settings->visuals.crosshair.x_offset, settings->visuals.crosshair.y_offset);
		}

		static ImVec4 textcolor = ImColor(213, 213, 213, 255);
		static ImVec4 buttoncolor = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		static ImVec4 WindowColor = ImColor(40, 40, 40, 255);
		static ImVec4 TabTextColor = ImColor(100, 100, 100, 255);
		static ImVec4 FrameBg = ImColor(32, 32, 38, 255);

	}
	if (settings->visuals.watermark.enabled) {
		drawing->text({ 5, 0 }, "Plague Rust | V5.1.0", drawing->ToImVec(utils->color_cycle()), true);
		ImGui::Spacing();
		ImGui::PushFont(menuFont);
		ImGui::Text(" ");
		ImGui::Text(items[selectedItem]);
		ImGui::Text(scopes[selectedScope]);
		ImGui::Text(barrel[selectedBarrel]);
		if (settings->globalvars.weaponAmmo < 0) {
			settings->globalvars.weaponAmmo = 0;
		}
		std::string s = std::to_string(settings->globalvars.weaponAmmo);
		char const* pchar = s.c_str();
		if (isCountingAmmo && !settings->features.xM249 && !settings->features.xSemi && !settings->features.xPython && !settings->features.xRevolver && !settings->features.xP250) {
			ImGui::Text("Ammo Count: "); ImGui::SameLine(); ImGui::Text(pchar);
		}

		if (settings->globalvars.isCrouched) {
			ImGui::Text("Crouched");
		}
		if (settings->globalvars.isReload)
			ImGui::Text("Reloading");
		ImGui::PopFont();

	}


}
	
	

void myFlags() {
	for (;;) {
		while (GetAsyncKeyState(VK_CONTROL) != 0) {
			settings->globalvars.isCrouched = true;
			Sleep(1);
		}
		settings->globalvars.isCrouched = false;
	}
}
constexpr auto rainbowColor(float time, float speed) noexcept
{
	return std::make_tuple(std::sin(speed * time) * 0.5f + 0.5f,
		std::sin(speed * time + static_cast<float>(2 * 3.14159265358979323846f / 3)) * 0.5f + 0.5f,
		std::sin(speed * time + static_cast<float>(4 * 3.14159265358979323846f / 3)) * 0.5f + 0.5f);
}
void mymenu::autoDetection() {
	uSens = 1.0;
	FILE* file;
	if (file = fopen("Config.inv", "r")) {
		fclose(file);
		loadConfig();
	}
	else {
		saveConfig();
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ShootGun, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Keybinds, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)myFlags, 0, 0, 0);
}

void Menu::WatermarkSolid()
{
	ImGuiIO& io = ImGui::GetIO();
	if (settings->keybinds.close == true)
		exit(0);
	ImGuiStyle* style = &ImGui::GetStyle();
	ImGui::PushFont(tabFont);
	const float DISTANCE = 10.0f;

	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		
		
		
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);


		style->ItemSpacing = ImVec2(25.f, 4.5f), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav  | ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoScrollWithMouse;
		{
			ImGui::PushFont(menuFont);
				ImGui::BeginChild("WatermarkContents", ImVec2(150.f, 50.f), false); {
					ImGui::Text("Gun:    ");
					ImGui::SameLine();
					ImGui::Text(items[selectedItem]);
					ImGui::EndChild();
				}
				ImGui::Spacing();
				ImGui::BeginChild("WatermarkContents", ImVec2(150.f, 50.f), false); {
					ImGui::Text("Scope:  ");
							   
							   
					ImGui::SameLine();
					ImGui::Text(scopes[selectedScope]);
					ImGui::EndChild();
				}
				ImGui::BeginChild("WatermarkContents", ImVec2(150.f, 50.f), false); {
				
					ImGui::Text("Barrel: ");
					ImGui::SameLine();
					ImGui::Text(barrel[selectedBarrel]);
					if (settings->globalvars.weaponAmmo < 0) {
						settings->globalvars.weaponAmmo = 0;
					}
					ImGui::EndChild();
				}

				std::string s = std::to_string(settings->globalvars.weaponAmmo);
				char const* pchar = s.c_str();
			}
		}
		ImGui::End();
	}

void Menu::Render() {

	ImGuiIO& io = ImGui::GetIO();

	if (settings->keybinds.close == true)
		exit(0);


	
	ImGuiStyle* style = &ImGui::GetStyle();



	ImGui::PushFont(menuFont);

	ImGui::SetNextWindowSize(ImVec2(400.f, 330.f));
	ImGui::Begin("PlagueWare Contents", &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiConfigFlags_NavEnableKeyboard | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar); {


		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::SameLine(6.f);
		style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);

		ImGui::BeginChild("Menu Contents", ImVec2(400.f, 548.f), false); {

			style->ItemSpacing = ImVec2(0.f, -1.f);

			ImGui::BeginTabs("Tabs", ImVec2(75.f, 320.f), false); {

				style->ItemSpacing = ImVec2(0.f, 0.f);

				style->ButtonTextAlign = ImVec2(0.5f, 0.47f);

				ImGui::PopFont();
				ImGui::PushFont(tabFont);

				switch (tab) {

				case 0:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::SelectedTab("Guns", ImVec2(75.f, 75.f))) tab = 0;
					if (ImGui::Tab("Config", ImVec2(75.f, 75.f))) tab = 1;
					if (ImGui::Tab("Keybinds", ImVec2(75.f, 75.f))) tab = 2;
					if (ImGui::Tab("Misc", ImVec2(75.f, 75.f))) tab = 3;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 1:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("Guns", ImVec2(75.f, 75.f))) tab = 0;
					if (ImGui::SelectedTab("Config", ImVec2(75.f, 75.f))) tab = 1;
					if (ImGui::Tab("Keybinds", ImVec2(75.f, 75.f))) tab = 2;
					if (ImGui::Tab("Misc", ImVec2(75.f, 75.f))) tab = 3;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 2:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("Guns", ImVec2(75.f, 75.f))) tab = 0;
					if (ImGui::Tab("Config", ImVec2(75.f, 75.f))) tab = 1;
					if (ImGui::SelectedTab("Keybinds", ImVec2(75.f, 75.f))) tab = 2;
					if (ImGui::Tab("Misc", ImVec2(75.f, 75.f))) tab = 3;
					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 3:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));
					if (ImGui::Tab("Guns", ImVec2(75.f, 75.f))) tab = 0;
					if (ImGui::Tab("Config", ImVec2(75.f, 75.f))) tab = 1;
					if (ImGui::Tab("Keybind", ImVec2(75.f, 75.f))) tab = 2;
					if (ImGui::SelectedTab("Misc", ImVec2(75.f, 75.f))) tab = 3;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				}

				ImGui::PopFont();
				ImGui::PushFont(menuFont);

				style->ButtonTextAlign = ImVec2(0.5f, 0.5f);

			} ImGui::EndTabs();

			ImGui::SameLine(75.f);

			ImGui::BeginChild("Tab Contents", ImVec2(300.f, 322.f), false); {
				//inner border
				style->Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);

				switch (tab) {

				case 0:
					Guns();
					break;
				case 1:
					Keybinds();
					break;
				case 2:
					Config();
					break;
				case 3:
					Misc();
					break;
				}

				style->Colors[ImGuiCol_Border] = ImColor(10, 10, 10, 255);

			} ImGui::EndChild();

			style->ItemSpacing = ImVec2(4.f, 4.f);
			//outer border
			if (settings->visuals.CustomizeMenu.rainbowmode)
			{
				style->Colors[ImGuiCol_Border] = drawing->ToImVec(utils->color_cycle2());
				style->WindowBorderSize = 5.0f;
			}
			else
			{ 
				style->Colors[ImGuiCol_Border] = ImColor(125, 45, 204, 125);
				style->WindowBorderSize = 1.0f;
			}
		} ImGui::EndChild();

		ImGui::PopFont();

	} ImGui::End();
}


void Menu::Shutdown() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void Menu::Guns() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

		InsertGroupBoxLeft("Guns", 286.f); {
			style->WindowPadding = ImVec2(8, 8);
			ImGui::Text("Guns:"); style->WindowPadding = ImVec2(8, 2);
			ImGui::Combo(" ", &selectedItem, items, IM_ARRAYSIZE(items)); style->ItemSpacing = ImVec2(8, 2);
			ImGui::Text("Scopes:"); style->WindowPadding = ImVec2(8, 2);
			ImGui::Combo("  ", &selectedScope, scopes, IM_ARRAYSIZE(scopes)); style->ItemSpacing = ImVec2(8, 2);
			ImGui::Text("Barrels:"); style->WindowPadding = ImVec2(8, 2);
			ImGui::Combo("    ", &selectedBarrel, barrel, IM_ARRAYSIZE(barrel)); style->WindowPadding = ImVec2(8, 2);
			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Aimbot Cover", "Guns");

	}

void Menu::Config() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	InsertGroupBoxLeft("Guns", 286.f); {
		style->WindowPadding = ImVec2(8, 8);
		ImGui::Text("AK47 Keybind: "); ImGui::SameLine(); ImGui::Keybind("  @", &settings->keybinds.akKeybind, NULL);;
		ImGui::Text("LR3000 Keybind: "); ImGui::SameLine(); ImGui::Keybind(" ", &settings->keybinds.lrKeybind, NULL);;
		ImGui::Text("MP5 Keybind: "); ImGui::SameLine(); ImGui::Keybind("  ", &settings->keybinds.mp5Keybind, NULL);;
		ImGui::Text("Custom SMG Keybind: "); ImGui::SameLine(); ImGui::Keybind("   ", &settings->keybinds.smgKeybind, NULL);;
		ImGui::Text("Thompson Keybind: "); ImGui::SameLine(); ImGui::Keybind("    ", &settings->keybinds.thompsonKeybind, NULL);;
		ImGui::Text("Semi Keybind: "); ImGui::SameLine(); ImGui::Keybind("     ", &settings->keybinds.semiKeybind, NULL);;
		ImGui::Text("M249 Keybind: "); ImGui::SameLine(); ImGui::Keybind("      ", &settings->keybinds.m249Keybind, NULL);;
		ImGui::Text("Python Keybind: "); ImGui::SameLine(); ImGui::Keybind("       ", &settings->keybinds.pythonKeybind, NULL);;
		ImGui::Text("Revolver Keybind: "); ImGui::SameLine(); ImGui::Keybind(".", &settings->keybinds.revolverKeybind, NULL);;
		ImGui::Text("P250 Keybind: "); ImGui::SameLine(); ImGui::Keybind("..", &settings->keybinds.p250Keybind, NULL);;
		ImGui::Text("Holo Keybind: "); ImGui::SameLine(); ImGui::Keybind("...", &settings->keybinds.holoKeybind, NULL);;
		ImGui::Text("Simple Keybind: "); ImGui::SameLine(); ImGui::Keybind("....", &settings->keybinds.simpleKeybind, NULL);;
		ImGui::Text("8x Keybind: "); ImGui::SameLine(); ImGui::Keybind(".......", &settings->keybinds.x8Keybind, NULL);;
		ImGui::Text("Silencer Keybind: "); ImGui::SameLine(); ImGui::Keybind("........", &settings->keybinds.silenceKeybind, NULL);;
		ImGui::Text("Muzzle Boost Keybind: "); ImGui::SameLine(); ImGui::Keybind("", &settings->keybinds.muzzleKeybind, NULL);;
		ImGui::Text("Small Furnace Splitter: "); ImGui::SameLine(); ImGui::Keybind("@", &settings->keybinds.furnaceKeybind, NULL);;
		ImGui::Text("Large Furnace Splitter: "); ImGui::SameLine(); ImGui::Keybind("@@", &settings->keybinds.lfurnaceKeybind, NULL);;
		ImGui::Text("Auto Upgrade Keybind: "); ImGui::SameLine(); ImGui::Keybind("@@@", &settings->keybinds.upgradeKeybind, NULL);;
		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);

	} InsertEndGroupBoxLeft("Aimbot Cover", "Keybinds");

}


void Menu::Keybinds() {





	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	InsertGroupBoxLeft("Guns", 286.f); {
		style->WindowPadding = ImVec2(8, 8);

		ImGui::InputDouble("Sensitivity: ", &uSens, 0.1, 100, 0); style->ItemSpacing = ImVec2(8, 2);
		ImGui::Text("Randomization:"); style->ItemSpacing = ImVec2(8, 2);
		InsertIntSlider("Overall Random", oRandom, 0, 100, "%d"); style->ItemSpacing = ImVec2(8, 2);
		InsertIntSlider("X Control %", xControlP, 0, 100, "%d"); style->ItemSpacing = ImVec2(8, 2);
		InsertIntSlider("Crosshair Type", settings->visuals.crosshair.crossnum, 0, 4, "%d"); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Sound", settings->features.uSound); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Overlay", settings->visuals.watermark.enabled); style->ItemSpacing = ImVec2(8, 2);
		ImGui::Combo("  ", &WMenuPos, wpos, IM_ARRAYSIZE(wpos)); style->ItemSpacing = ImVec2(8, 2);
		{
			if (wpos[WMenuPos] == "Top -left") {
				corner = -1;
			}
			if (wpos[WMenuPos] == "Top-right") {
				corner = 1;
			}
			if (wpos[WMenuPos] == "Bottom-left") {
				corner = 2;
			}
			if (wpos[WMenuPos] == "Bottom-right") {
				corner = 3;
			}
		}
		
		if (ImGui::Button("Save Config")) {
		
			saveConfig();
		}
		if (ImGui::Button("Load Config")) {
			loadConfig();
		}
	
		InsertCheckbox("Toggle Menu", settings->visuals.CustomizeMenu.enabled); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Toggle Solid Overlay", settings->visuals.Watermarksolid.enabled); style->ItemSpacing = ImVec2(8, 2);
		if (settings->visuals.Watermarksolid.enabled)
			settings->visuals.watermark.enabled = false;
		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);

	} InsertEndGroupBoxLeft("Aimbot Cover", "Config");
}

void Menu::Misc() {


	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	InsertGroupBoxLeft("Guns", 286.f); {
		style->WindowPadding = ImVec2(8, 8);
		InsertCheckbox("Large Furnace Splitter", islfurnace); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Small Furnace Splitter", isFurnace); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Auto Fire", isAutofire); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Anti Afk", isAntiAfk); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Night Mode Overlay", settings->globalvars.isNightMode); style->ItemSpacing = ImVec2(8, 2);
		InsertCheckbox("Hipfire", settings->features.isHipfire); style->ItemSpacing = ImVec2(8, 2);

		ImGui::Text("Auto Upgrade: 0 = None, 1 = Wood,");
		ImGui::Text(" 2 = Stone, 3 = Metal, 4 = HQM");
		InsertIntSlider(" ", bgradeNum, 0, 4, "%d"); style->ItemSpacing = ImVec2(8, 2);
		InsertIntSlider("Count Ammo", isCountingAmmo, 0, 1, "%d"); style->ItemSpacing = ImVec2(8, 2);
		if (ImGui::Button("Exit Script")) {
			exit(0);
		}
		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);
	} InsertEndGroupBoxLeft("Aimbot Cover", "Misc");
}
Menu* menu = new Menu();