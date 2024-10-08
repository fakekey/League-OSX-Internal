#pragma once
#ifndef Offsets_hpp
#define Offsets_hpp

namespace Offset
{
    enum Chat
    {
        ChatInstance = 0x208D448,
        ChatClient = 0x208DD70,
        PrintChat = 0xB40730,
    };

    enum LocalPlayer
    {
        PlayerInstance = 0x212ED90,
        GetAttackDelay = 0x406200,
        GetAttackCastDelay = 0x406670,
    };

    enum World
    {
        GameTime = 0x20B0708,
        HeroList = 0x208D260,
        MinionList = 0x208D3A0,
        HudInstance = 0x2096E90,
        NetClient = 0x208DC10,
        GetPing = 0x5AE7C0,
        IsBuffActive = 0x8D0650,
    };

    enum Control
    {
        Check = 0x212A530,
        IssueOrder = 0xF9E40,
        CastSpell = 0x939C00,
        GetSpellState = 0x93B1C0,
    };

    enum SpellState
    {
        Ready = 0,
        NotAvailable = 4,
        Surpressed = 8,
        NotLearned = 12,
        Processing = 24,
        Cooldown = 32,
        NoMana = 64,
        Unknown
    };

    enum Order
    {
        Move = 2,
        Attack = 3,
        Stop = 10,
    };
}

#endif /* Offsets_hpp */
