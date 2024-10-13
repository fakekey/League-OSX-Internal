#pragma once
#ifndef FuncExport_h
#define FuncExport_h

#include "Vector.h"
#include "XRayDef.hpp"

typedef char (*IssueOrderFunc)(unsigned int *, int, float *, unsigned int *, char, int, char);
typedef __int64 (*GetSpellStateFunc)(__int64, __int64, _BYTE *);
typedef void (*PrintChatFunc)(uint64_t *, const char *, unsigned int);
typedef void (*CastSpellFunc)(__int64, __int64, unsigned int, __int64 *, int);
typedef __int64 (*OnProcessSpellFunc)(__int64, __int64);
typedef char (*LolDrawFunc)(__int64, __int64, _DWORD *, _DWORD *, unsigned __int8);
typedef bool (*WorldToScreenFunc)(_DWORD *, Vector3 *, Vector3 *);
typedef __int64 (*GetHealthBarScreenPositionFunc)(__int64, __int64, __int64, float, float);

#endif /* FuncExport_h */
