#pragma once
#ifndef Utils_h
#define Utils_h

#include "Metal/MTLDevice.hpp"
#include "Metal/MTLTexture.hpp"

void ShowDialog(const char *message, const char *info);
MTL::Texture *LoadTextureFromImagePath(MTL::Device *pDevice, const char *filePath);

#endif