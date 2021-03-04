/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#ifndef _INCLUDE_HPP
#define _INCLUDE_HPP

// include the Direct3D Library file
#pragma comment (lib, "d3d10.lib")

#pragma warning(disable : 4005)
#pragma warning(disable : 4244)
#pragma warning(disable : 6001)
#pragma warning(disable : 6031)
#pragma warning(disable : 6387)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <WinUser.h>
#include <thread>
#include <chrono>
#include <string>
#include <functional>
#include <vector>
#include <array>
#include <iterator>
#include <TlHelp32.h>
#include <tchar.h>
#include <dinput.h>
#include <CommCtrl.h>
#include <random>
#include <algorithm>
#include <cctype>
#include <atomic>
#include <mutex>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <cmath>
/* memory */

/* drawing */
#include <d3d11.h>
#include "drawing/drawing.hpp"

#define print(c, b, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); printf(b); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  printf(__VA_ARGS__)

#endif // _INCLUDE_HPP