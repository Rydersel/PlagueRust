

#include "include.hpp"
#include "globals/globals.hpp"
#include "features/aimbot/Aimbot.hpp"
#include "features/features.hpp"
#include "menu/menu.h"
bool test = true;
#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <stdlib.h>
#include "menu/menu.h"
#include "xor.h"
#include "lw_http.hpp"
#include <ostream>
#include <regex>
#include "print.h"
#include "hwid.h"
#include "md5wrapper.h"
#include "crypto.h"
#include "authgg.h"
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <fstream>|
#include <iostream>
std::string username;
std::string password;
std::string email;
std::string key;
std::string choice;
bool x = false;
/* main function */
void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}
void autoDetection2()
{
    while (true) {
        Sleep(10000000);
        authgg::Login2(username, password);
    }
}
int main()
{
    extern c_crypto crypto;
    ///////////////////////////////////////////////////////////////////////////////////////
    //                              Application Settings
    crypto.aid = "295521";
    crypto.secret = "mb6bgDAU5dL2alSMjjsEWRcxFEi39y8gEq9";
    crypto.apikey = "94474811782731843521956429331157219";
    crypto.version = "5.0";
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    //                                Initialization
    auto md5 = new md5wrapper();
    authgg::GenerateSeed();
    authgg::Initialize();
    std::string choice;
    SetConsoleTitle("Plague Rust");
    //////////////////////////////////////////////////////////////////////////////////////

  //  FILE* file;
 //   if (file = fopen("Login.inv", "r")) {
   //     std::cout << "Auto Log-In Authorizing. . . \n";
  //      fclose(file);
  //      char takenstring[100];
   //    GetPrivateProfileString(TEXT("Bindings"), TEXT("username"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
 //       username = takenstring;
  //      GetPrivateProfileString(TEXT("Bindings"), TEXT("password"), TEXT("fail while retrieving"), takenstring, 100, TEXT(".\\Config.inv"));
   //     password = takenstring;
   //     std::cout << username, password;
   //     authgg::Login(username, password);


     std::cout << R"( /$$$$$$$  /$$                                                                                    
| $$__  $$| $$                                                                                    
| $$  \ $$| $$  /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$  /$$  /$$  /$$  /$$$$$$   /$$$$$$   /$$$$$$ 
| $$$$$$$/| $$ |____  $$ /$$__  $$| $$  | $$ /$$__  $$| $$ | $$ | $$ |____  $$ /$$__  $$ /$$__  $$
| $$____/ | $$  /$$$$$$$| $$  \ $$| $$  | $$| $$$$$$$$| $$ | $$ | $$  /$$$$$$$| $$  \__/| $$$$$$$$
| $$      | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$ | $$ | $$ /$$__  $$| $$      | $$_____/
| $$      | $$|  $$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$/$$$$/|  $$$$$$$| $$      |  $$$$$$$
|__/      |__/ \_______/ \____  $$ \______/  \_______/ \_____/\___/  \_______/|__/       \_______/
                         /$$  \ $$                                                                
                        |  $$$$$$/                                                                
                         \______/       
                                                          )" << std::endl;
        while (x == false) {
            std::cout << "\n" << std::endl;
            std::cout << "Plagueware Auth \n" << std::endl;
            std::cout << "Login [1] or Info [2]" << std::endl;
       
            std::cin >> choice;
            if (choice == "1") {
                x = true;
                std::cout << "Enter Username: ";
                std::cin >> username;
                std::cout << "Enter Password: ";
                std::cin >> password;
                std::cout << " " << std::endl;
                authgg::Login(username, password);
            }
            else if (choice == "2") {
                std::cout << "sign in with your forum account";
            }
            else {
                std::cout << "Wrong Login" << std::endl;
                exit(0);
            }
        }
        std::string config;
        config = "[Bindings]\n username = " + username + "\npassword = " + password;
        std::ofstream file_;
        file_.open("Login.inv");
        file_ << config;
        file_.close();



    {
		AllocConsole();
		SetConsoleTitleW(L"PlagueWare - Rust Debug");
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
    }

    /* Find Rust Window */
    drawing->h_Game = FindWindowA("UnityWndClass", "Rust");
    if (!drawing->h_Game)
    {
        print(4, "\n[:(] ", "Rust Not Found! Please Open Rust Before Opening The Loader \n");
        SleepEx(3000, false);
        exit(0);
    }

        globals->ScreenSize = { utils->getWindowSize(drawing->h_Game).first, utils->getWindowSize(drawing->h_Game).second };

        mymenu::autoDetection();
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)autoDetection2, 0, 0, 0);

        Sleep(100);
        HideConsole();
        SetForegroundWindow(drawing->h_Game);

        std::thread(&Drawing::Initialize, drawing).detach();

        bool b_ov;
        while (1)
        {
            SleepEx(1, false);
            if (!IsWindow(drawing->h_Game))
                drawing->b_Shutdown = true;

            HWND h_Foreground = GetForegroundWindow();
            if (h_Foreground != drawing->h_Game && h_Foreground != drawing->h_hWnd) {
                if (!b_ov) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
                SetWindowPos(drawing->h_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                b_ov = false;
            }
            else {
                if (b_ov) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
                    continue;
                }

                SetWindowPos(drawing->h_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

                if (menu->isOpen)
                    SetForegroundWindow(drawing->h_hWnd);

                b_ov = true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            if (settings->keybinds.close == true) {
                exit(0);
            }
        }
    }
