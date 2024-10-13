#include "Memory.hpp"

Memory::Module::Module(std::string ModuleName) {
    for (int i = 0; i < _dyld_image_count(); i++) {
        mach_header_64 *ImgHeader = (mach_header_64 *)(_dyld_get_image_header(i));

        std::string ImgFilePath(_dyld_get_image_name(i));
        if (ImgFilePath.empty()) {
            continue;
        }

        // EXTRACT FILE NAME FROM RELATIVE PATH
        std::string ImgName;
        size_t pos = ImgFilePath.rfind("/", ImgFilePath.length());
        if (pos != std::string::npos) {
            ImgName = ImgFilePath.substr(pos + 1, ImgFilePath.length() - pos);
        }
        if (ImgName != ModuleName) {
            continue;
        }

        HeaderPtr = ImgHeader;
        Name = ModuleName;
        FilePath = ImgFilePath;
        Handle = reinterpret_cast<uintptr_t>(dlopen(ImgFilePath.c_str(), RTLD_GLOBAL | RTLD_LAZY | RTLD_NOW));
        Address = reinterpret_cast<uintptr_t>(HeaderPtr);

        break;
    }
}

Memory::Module::~Module() {
    dlclose(reinterpret_cast<void *>(Handle));
    // free(HeaderPtr);
};
