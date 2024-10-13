#pragma once
#ifndef Utils_hpp
#define Utils_hpp

#include <Carbon/Carbon.h>
#include <string>

namespace Character {
    std::string ToLower(std::string str);
}

namespace Utils {
    Boolean IsPressed(unsigned short inKeyCode);
}

#endif
