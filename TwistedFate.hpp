#pragma once
#ifndef TwistedFate_h
#define TwistedFate_h

#include "Script.hpp"
#include <mach/mach_time.h>

class TwistedFate : public Script {
  public:
    TwistedFate() {}

  private:
    _WORD BlueKey;
    _WORD RedKey;
    _WORD YellowKey;
    _WORD ComboKey;
    uint64_t LastCastW;
    GameObject *Me;
    SpellSlot *Q;
    SpellSlot *W;

  public:
    void Load() override;
    void Run() override;

  private:
    bool IsQReady();
    bool IsWReady();
    void CastQ(Vector3 pos);
    void CastW();
    void Combo();
    void PickBlue();
    void PickRed();
    void PickYellow();
    GameObject *HumanSelector(float range = 0.f);
};

#endif /* TwistedFate_h */