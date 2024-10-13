#pragma once
#ifndef Irelia_h
#define Irelia_h

#include "Script.hpp"

class Irelia : public Script {
  public:
    Irelia() {}

  private:
    _WORD LaneClearKey;
    _WORD LastHitKey;
    _WORD ComboKey;
    int QLevelDmg[5] = {5, 25, 45, 65, 85};
    float QMinionDmg = 0.f;
    float PassiveDmg = 0.f;
    GameObject *Me;
    SpellSlot *Q;
    SpellSlot *E;

  public:
    void Load() override;
    void Run() override;

  private:
    std::tuple<float, float> QDmg(GameObject *Target);
    bool CanQ(GameObject *Target);
    bool IsQReady();
    bool IsEReady();
    GameObject *GetWeakestTarget(float range = 0.f);
    GameObject *GetClosestMobToEnemyForGap(GameObject *Enemy);
    GameObject *MobSelector(float range = 0.f);
    GameObject *HumanSelector(float range = 0.f);
    void LaneClear();
    void Combo();
    void CastQ(GameObject *Target);
    void CastE(Vector3 pos);
    void CastE1(Vector3 pos);
    void CastE2(GameObject *Target);
};

#endif /* Irelia_h */
