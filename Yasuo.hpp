#pragma once
#ifndef Yasuo_h
#define Yasuo_h

#include "Script.hpp"
#include <mach/mach_time.h>

class Yasuo : public Script {
  public:
    Yasuo() {}

  private:
    _WORD LaneClearKey;
    _WORD LastHitKey;
    _WORD ComboKey;
    int QLevelDmg[5] = {20, 45, 70, 95, 120};
    int ELevelDmg[5] = {70, 85, 100, 115, 130};
    float EStackLevelDmg[5] = {17.5, 21.25, 25, 28.75, 32.5};
    GameObject *Me;
    SpellSlot *Q;
    SpellSlot *E;
    uint64_t LastE;

  public:
    void Load() override;
    void Run() override;

  private:
    bool CanQLastHit(GameObject *Target);
    bool CanELastHit(GameObject *Target);
    bool IsQReady();
    bool IsEReady();
    void CastQ(Vector3 pos);
    bool CastE(GameObject *Target);
    void LaneClear();
    void LastHit();
    void Combo();
    GameObject *HumanQSelector(float range = 0.f);
    GameObject *HumanESelector(float range = 0.f);
    GameObject *MobQSelector(float range = 0.f);
    GameObject *MobESelector(float range = 0.f);
    GameObject *LastHitQSelector(float range = 0.f);
    GameObject *LastHitESelector(float range = 0.f);
};

#endif