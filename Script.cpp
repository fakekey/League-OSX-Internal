#include "Script.hpp"

Script::~Script() {}

Memory::Module *Script::Lol = nullptr;
Controller *Script::Control = nullptr;
GameObject *Script::LocalPlayer = nullptr;
ObjectList *Script::HeroList = nullptr;
ObjectList *Script::MinionList = nullptr;
uint64_t *Script::ChatInstance = nullptr;
Chat *Script::ChatClient = nullptr;