#pragma once
#ifndef Spell_h
#define Spell_h

#include "XRayDef.hpp"
#include "Vector.h"
#include "FuncExport.hpp"
#include "Offsets.hpp"

class SpellCastData
{
public:
    __int64 Skip[2];
    _DWORD SourceIndex;
    _DWORD TargetIndex;
    Vector3 StartPos;
    Vector3 EndPos;
    Vector3 BoundPos;
    Vector3 MousePos;
};

class SpellData
{
public:
    union
    {
        DEFINE_MEMBER_N(char *SpellName, 0x80);
    };
};

class SpellInfo
{
public:
    union
    {
        DEFINE_MEMBER_N(char *Id, 0x30);
        DEFINE_MEMBER_N(SpellData *Data, 0x58);
    };
};

class SpellSlot
{
public:
    union
    {
        DEFINE_MEMBER_N(int Level, 0x28);
        DEFINE_MEMBER_N(float ReadyAt, 0x30);
        DEFINE_MEMBER_N(SpellCastData *CastData, 0x120);
        DEFINE_MEMBER_N(SpellInfo *Info, 0x128);
    };
};

class OActiveSpell
{
public:
    union
    {
        DEFINE_MEMBER_N(SpellInfo *Info, 0x8);
    };
};

class OSpellBook
{
public:
    SpellSlot *GetSpellSlotByID(int id);
    bool IsReady(int id);

public:
    static Memory::Module *Lol;
};

class BuffInfo
{
public:
    union
    {
        DEFINE_MEMBER_N(char *Name, 0x8);
    };
};

class BuffInstance
{
public:
    union
    {
        DEFINE_MEMBER_N(_BYTE BuffType, 0xC);
        DEFINE_MEMBER_N(BuffInfo *Info, 0x10);
        DEFINE_MEMBER_N(_DWORD Count, 0x38);
    };

public:
    bool IsActive();
};

class OBuffManager
{
public:
    bool HasBuff(std::string name);
    bool HasBuff(std::string name, BuffInstance *out);
    BuffInstance *GetBuffInstance(int id);
};

#endif /* Spell_h */
