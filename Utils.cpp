#include "Utils.hpp"

std::string Character::ToLower(std::string str) {
    std::string result;
    result.resize(str.size());
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);

    return result;
}

Boolean Utils::IsPressed(unsigned short inKeyCode) {
    unsigned char keyMap[16];
    GetKeys((BigEndianUInt32 *)&keyMap);
    return (0 != ((keyMap[inKeyCode >> 3] >> (inKeyCode & 7)) & 1));
}