#pragma once
#ifndef Yone_h
#define Yone_h

#include "Script.hpp"

class Yone : public Script {
  public:
    Yone() {}

  private:
    _WORD LaneClearKey;
    _WORD LastHitKey;
    _WORD ComboKey;
    int QLevelDmg[5] = {20, 45, 70, 95, 120};
    GameObject *Me;
    SpellSlot *Q;
    SpellSlot *W;

  public:
    void Load() override;
    void Run() override;

  private:
    bool CanQLastHit(GameObject *Target);
    bool IsQReady();
    bool IsWReady();
    void CastQ(Vector3 pos);
    void CastW(Vector3 pos);
    void LaneClear();
    void LastHit();
    void Combo();
    GameObject *HumanSelector(float range = 0.f);
    GameObject *MobSelector(float range = 0.f);
    GameObject *LastHitSelector(float range = 0.f);
};

#endif /* Yone_h */