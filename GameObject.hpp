#pragma once
#ifndef GameObject_hpp
#define GameObject_hpp

#include "GameData.hpp"
#include "Item.hpp"
#include "Spell.hpp"
#include <set>

class Chat {
  public:
    union {
        DEFINE_MEMBER_N(bool IsChatOpen, 0x4A8);
    };
};

class AIManager {
  public:
    union {
        DEFINE_MEMBER_N(float MoveSpeed, 0x238);
        DEFINE_MEMBER_N(bool IsMoving, 0x23C);
        DEFINE_MEMBER_N(Vector3 StartPos, 0x250);
        DEFINE_MEMBER_N(Vector3 EndPos, 0x25C);
        DEFINE_MEMBER_N(Vector3 *TargetPaths, 0x268);
        DEFINE_MEMBER_N(_DWORD SegmentCount, 0x270);
        DEFINE_MEMBER_N(bool DashSpeed, 0x280);
        DEFINE_MEMBER_N(bool Dash, 0x2A4);
        DEFINE_MEMBER_N(Vector3 Velocity, 0x3A0);
    };

  public:
    bool IsDashing();
};

class GameObject {
  public:
    union {
        DEFINE_MEMBER_N(_DWORD Index, 0x10);
        DEFINE_MEMBER_N(uint16_t Team, 0x1C);
        DEFINE_MEMBER_N(uint32_t NetworkId, 0x68);
        DEFINE_MEMBER_N(Vector3 Position, 0x128);
        DEFINE_MEMBER_N(bool Visible, 0x1D1);
        DEFINE_MEMBER_N(bool Targetable, 0x748);
        DEFINE_MEMBER_N(float Health, 0x8F0);
        DEFINE_MEMBER_N(float MaxHealth, 0x918);
        DEFINE_MEMBER_N(float MagicResPenFlat, 0xE24);
        DEFINE_MEMBER_N(float ArmorPen, 0xE28);
        DEFINE_MEMBER_N(float MagicResPen, 0xE2C);
        DEFINE_MEMBER_N(float Lethality, 0xE40);
        DEFINE_MEMBER_N(float BonusAttack, 0xE80);
        DEFINE_MEMBER_N(float AbilityPower, 0xE90);
        DEFINE_MEMBER_N(float BonusAttackSpeed, 0xEF4);
        DEFINE_MEMBER_N(float BaseAttack, 0xF24);
        DEFINE_MEMBER_N(float Armor, 0xF4C);
        DEFINE_MEMBER_N(float MagicRes, 0xF54);
        DEFINE_MEMBER_N(float MoveSpeed, 0xF64);
        DEFINE_MEMBER_N(float AttackRange, 0xF6C);
        DEFINE_MEMBER_N(OInventory *Inventory, 0x2370);
        DEFINE_MEMBER_N(OBuffManager BuffManager, 0x2510);
        DEFINE_MEMBER_N(OSpellBook SpellBook, 0x2768);
        DEFINE_MEMBER_N(OActiveSpell *ActiveSpell, 0x27A0);
        DEFINE_MEMBER_N(uint8_t Level, 0x4038);
    };

  public:
    const char *GetName();
    bool IsDead();
    bool IsAlive();
    bool IsAttackable();
    bool IsRanged();
    UnitInfo *Info();
    MTL::Texture *Icon();
    bool IsAllyTo(GameObject *other);
    bool IsEnemyTo(GameObject *other);
    bool HasUnitTag(std::string tag);
    double GetAttackDelay();
    double GetAttackCastDelay();
    AIManager *GetAIManager();

  public:
    static Memory::Module *Lol;
};

class ObjectList {
  public:
    union {
        DEFINE_MEMBER_N(GameObject **Item, 0x8);
        DEFINE_MEMBER_N(uint32_t Size, 0x10);
    };
};

class Controller {
  public:
    Memory::Module *Module;

  public:
    Controller(Memory::Module *Lol);
    void Move(GameObject *Obj, Vector3 *position);
    void Attack(GameObject *Obj, GameObject *Target);
    GameObject *GetBestChampInRange(GameObject *Obj, ObjectList *Champs, float range = 0.f);
    GameObject *GetBestMinionInRange(GameObject *Obj, ObjectList *Minions, float range = 0.f);
    GameObject *GetMinionLastHitInRange(GameObject *Obj, ObjectList *Minions, float range = 0.f);
    Vector3 *GetCursorPos();
    float GetPing();
    bool IsLasthitable(GameObject *Me, GameObject *Target);
    float DmgOnArmor(GameObject *Me, GameObject *Target, float Dmg = 0.f);
    float DmgOnMagicRes(GameObject *Me, GameObject *Target, float Dmg = 0.f);
    float OnHitBeginItem(GameObject *Me, GameObject *Target);
    float OnHitRecurveBow(GameObject *Me, GameObject *Target);
    float OnHitWitsEnd(GameObject *Me, GameObject *Target);
    float OnHitNashorsTooth(GameObject *Me, GameObject *Target);
    float OnHitGuinsoosRageblade(GameObject *Me, GameObject *Target);
    float OnHitTerminus(GameObject *Me, GameObject *Target);
    float OnHitBladeOfTheRuinedKing(GameObject *Me, GameObject *Target);
    float GetOnHitPhysical(GameObject *Me, GameObject *Target, __int64 ItemId);
    float GetOnHitMagical(GameObject *Me, GameObject *Target, __int64 ItemId);
    float GetOnHitPhysicFromItems(GameObject *Me, GameObject *Target);
    float GetOnHitMagicFromItems(GameObject *Me, GameObject *Target);
    Vector3 PredictPos(GameObject *Target, float PredictTime = 0.f);
    void PrintChat(const char *Msg);

  public:
    static std::set<std::string> blacklistedObjectNames;
};

#endif /* GameObject_hpp */
