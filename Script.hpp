#pragma once
#ifndef Script_h
#define Script_h

#include "GameObject.hpp"

class Script {
  public:
    Script() {}
    virtual ~Script() = 0;
    virtual void Load() = 0;
    virtual void Run() = 0;

  public:
    static Memory::Module *Lol;
    static Controller *Control;
    static GameObject *LocalPlayer;
    static ObjectList *HeroList;
    static ObjectList *MinionList;
    static Chat *ChatClient;
};

#endif /* Script_h */
