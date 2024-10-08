#pragma once
#ifndef FuncExport_h
#define FuncExport_h

#include "Memory.hpp"

typedef char (*IssueOrderFunc)(unsigned int *, int, float *, unsigned int *, char, int, char);
typedef __int64 (*GetSpellStateFunc)(__int64, __int64, _BYTE *);
typedef void (*PrintChatFunc)(uint64_t *, const char *, unsigned int);
typedef void (*CastSpellFunc)(__int64, __int64, unsigned int, __int64 *, int);

#endif /* FuncExport_h */
