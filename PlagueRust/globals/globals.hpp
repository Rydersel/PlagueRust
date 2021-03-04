#ifndef _GLOBALS_HPP
#define _GLOBALS_HPP

#include <list>
#include <cstdint>
#include <string>
#include <xmmintrin.h>
#include <emmintrin.h>
#include "../classes/Math/Math.hpp"

#include "../globals/utils.hpp"
#include <future>
class Globals
{
public:
    Vector2 ScreenSize{};

};

extern Globals* globals;

#endif 