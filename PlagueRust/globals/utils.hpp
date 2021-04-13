#ifndef _UTILS_HPP
#define _UTILS_HPP

class Settings
{
public:
    /* visuals */
    struct visuals {
        struct crosshair {
            int crossnum = 1;
            int size = 5;
            int x_offset;
            int y_offset;
        } crosshair;

        struct watermark {
            bool enabled = true;
            float color[4] = { 1, 1, 1, 1 };

        } watermark;    

        struct Watermarksolid {
            bool enabled;
            float color[4] = { 1, 1, 1, 1 };
        } Watermarksolid;

        struct CustomizeMenu {
            bool enabled;
            bool rainbowmode;
        } CustomizeMenu;


        struct AntiAFK {
            bool enabled;
            bool rainbowmode;
        } AntiAFK;


        struct players {
            struct name {
                bool enabled;
                float color[4] = { 0, 0, 0, 1 };
            } name;

            struct box {
                bool enabled;
                bool health_bar;
                float health_bar_color[4] = { 0, 0, 0, 1 };
                float outline_color[4] = { 0, 0, 0, 1 };
                float inline_color[4] = { 0, 0, 0, 1 };
            } box;

            struct skeleton {
                bool enabled;

                int thickness = 1;
                float color[4] = { 0, 0, 0, 1 };
            } skeleton;
        } players;

        int font_size = 8;
        bool font_outline = true;
        bool font_fading = true;
    } visuals;


    /* features */
    struct features {
        bool xAK = false;
        bool xLR = false;
        bool xMP5 = false;
        bool xSMG = false;
        bool xThompson = false;
        char str2;
        bool xSemi = false;
        bool xP250 = false;
        bool xM249 = false;
        bool x8x = false;
        bool xHolo = false;
        bool xSimple = false;
        bool xSuppressor = false;
        bool xMuzzleBoost = false;
        bool xPython = false;
        bool xRevolver = false;
        bool fake_admin;
        bool uSound = true;
        int xRandom;
        int yRandom;
        int oRandom;
        bool isHipfire = false;
    } features;

    struct keybinds {
        int Pause = 112;
        int akKeybind = 113;
        int lrKeybind = 114;
        int mp5Keybind = 115;
        int smgKeybind = 116;
        int thompsonKeybind = 117;
        int semiKeybind = 119;
        int m249Keybind = 118;
        int pythonKeybind = 120;
        int revolverKeybind = 121;
        int p250Keybind = 122;
        int holoKeybind = 97;
        int simpleKeybind = 98;
        int x8Keybind = 99;
        int muzzleKeybind = 101;
        int silenceKeybind = 102;
        int furnaceKeybind = 89;
        int lfurnaceKeybind = 88;
        int upgradeKeybind = 70;
        int codelockkeybind = 66;
        int MenKey = 45;
        bool close = false;
    } keybinds;
    struct globalvars {
        double hipMult = 0.6;
        bool isCrouched = false;
        bool isReload;
        int weaponAmmo;
        bool isNightMode = false;
    } globalvars;
};

class Utils
{
public:
    
    std::pair<float, float> getWindowSize(HWND window);

    std::string wstring_to_string(const std::wstring& wstring);


    float* color_cycle();
    float* color_cycle2();
    float* color_cycle3();
    float* color_cycle4();
    float color_hue[3] = { 255, 0, 0 };
    int color_iteration = 0;

};


extern Settings* settings;

extern Utils* utils;

#endif // _UTILS_HPP