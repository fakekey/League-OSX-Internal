#pragma once
#ifndef Utils_h
#define Utils_h

#include "FuncExport.hpp"
#include <Carbon/Carbon.h>

namespace Character
{
    std::string ToLower(std::string str);
}

namespace Utils
{
    Boolean IsPressed(unsigned short inKeyCode);
}

#endif /* Utils_h */
