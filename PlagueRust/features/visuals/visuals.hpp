/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#ifndef _VISUALS_HPP
#define _VISUALS_HPP

class Visuals
{
public:
    void DrawCornerBox(int X, int Y, int W, int H, ImVec4 inline_color, ImVec4 outline_color);
    auto DrawPlayers() -> void;
    auto Draw() -> void;

};

extern Visuals* visuals;

#endif // _VISUALS_HPP