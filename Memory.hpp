#pragma once
#ifndef Memory_hpp
#define Memory_hpp

#include "XRayDef.hpp"
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <string>

namespace Memory {
    class Module {
      public:
        mach_header_64 *HeaderPtr = nullptr;
        std::string Name;
        std::string FilePath;
        uintptr_t Handle = NULL;
        uintptr_t Address = NULL;
        Module(std::string ModuleName);
        ~Module();

      public:
        template <typename T>
        T GetStruct(const int offset = 0) {
            return reinterpret_cast<T>(*reinterpret_cast<_QWORD *>(this->Address + offset));
        }

        template <typename T>
        T GetType(const int offset = 0) {
            return reinterpret_cast<T>(this->Address + offset);
        }
    };
} // namespace Memory

#endif /* Memory_hpp */
