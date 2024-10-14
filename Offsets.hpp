#pragma once
#ifndef Offsets_hpp
#define Offsets_hpp

namespace Offset {
    enum Chat {
        ChatInstance = 0x20C3B48, // Passed to PrintChat
        ChatClient = 0x20C53D0,   // Inside PrintChat
        PrintChat = 0xB65DE0,     // %s <font color=\"#ffa500\">%s</font>
    };

    enum LocalPlayer {
        PlayerInstance = 0x2168030,    // game_announcement_replace_you
        GetAttackDelay = 0x4125D0,     // attackSpeed
        GetAttackCastDelay = 0x4125F0, // above GetAttackDelay
    };

    enum World {
        GameTime = 0x20E7C38,   // {:5s}| {#channel!} Spawning completed
        HeroList = 0x20C3978,   // Champion_Kills
        MinionList = 0x20C3AA0, // Death // 8
        MissileList = 0x20C3AD8,
        TurretList = 0x20C3CB0,  // {:5s}| {#channel!} Spawning completed
        HudInstance = 0x20C4460, // {:5s}| {#channel!} Spawning completed
        NetClient = 0x20C3D80,   // above where you found AttackDelay and CastDelay together
        GetPing = 0x5D36C0,      // game_cornerdisplay_ping
        IsBuffActive = 0x8EE530,
        LolDraw = 0x169AF30,        // presentDrawable:
        LolWindow = 0x2173180,      // LolWindow
        ScreenInstance = 0x20C3A60, // {:5s}| {#channel!} Spawning completed
        WorldToScreen = 0x15E66D0,  // param ScreenInstance + 0x270
        GetHealthBarScreenPosition = 0xBD8800,
    };

    enum Control {
        Check = 0x2163760,
        IssueOrder = 0xE86E0,      // evtPlayerHoldPosition
        CastSpell = 0x952110,      // ERROR: Client Tried to cast a spell from an invalid slot: %i.\n // 2
        GetSpellState = 0x953760,  // Ctrl + L look for Vfx // Look above function under 3.4028235e37
        OnProcessSpell = 0x94F4D0, // under where you found AttackDelay and CastDelay together
    };

    enum SpellState {
        Ready = 0,
        PlayerReady = 8,
        ActiveReady = 520,
        ActiveCasting = 552,
        ActiveCooldown = 544,
        NotAvailable = 4,
        Surpressed = 8,
        NotLearned = 12,
        Processing = 24,
        Cooldown = 32,
        PlayerCooldown = 40,
        NoMana = 64,
        PlayerNoMana = 72,
    };

    enum Order {
        Move = 2,
        Attack = 3,
        Stop = 10,
    };

    enum SkillShotType {
        UNKNOWN = 0,
        CIRCLE = 1,
        CONE = 2,
        LINE = 3,
    };
} // namespace Offset

#endif /* Offsets_hpp */
