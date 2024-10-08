#include <mach/mach_time.h>
#include "GameObject.hpp"
#include "Irelia.hpp"
#include "Yasuo.hpp"
#include "Yone.hpp"

static Memory::Module *Lol = nullptr;
static GameObject *LocalPlayer = nullptr;
static uint64 LastLoop = 0;
static float LastAttack = 0.f;
static float LastMove = 0.f;
static float LastScript = 0.f;
static uint64_t *ChatInstance = nullptr;
static Chat *ChatClient = nullptr;
static float *GameTime = nullptr;
static ObjectList *HeroList = nullptr;
static ObjectList *MinionList = nullptr;
static Controller *Control = nullptr;
static bool Running = true;

void *Initialize(void *args)
{
    Lol = new Memory::Module(xorstr("LeagueofLegends"));
    OSpellBook::Lol = Lol;
    GameObject::Lol = Lol;
    while (true)
    {
        if (mach_absolute_time() - LastLoop > 1000000000)
        {
            LocalPlayer = Lol->GetStruct<GameObject *>(Offset::LocalPlayer::PlayerInstance);
            if (LocalPlayer != nullptr)
                break;

            LastLoop = mach_absolute_time();
        }
    }

    // Hooks

    // Stuffs
    ChatInstance = Lol->GetType<uint64 *>(Offset::Chat::ChatInstance);
    ChatClient = Lol->GetStruct<Chat *>(Offset::Chat::ChatClient);
    GameTime = Lol->GetType<float *>(Offset::World::GameTime);
    HeroList = Lol->GetStruct<ObjectList *>(Offset::World::HeroList);
    MinionList = Lol->GetStruct<ObjectList *>(Offset::World::MinionList);
    Control = new Controller(Lol);

    // Load DB
    Memory::Module *libShit = new Memory::Module(xorstr("libShit.dylib"));
    Dl_info libShitInfo;
    if (dladdr(reinterpret_cast<void *>(libShit->Address), &libShitInfo))
    {
        std::string fullPath(libShitInfo.dli_fname);
        std::string dataPath = fullPath.substr(0, fullPath.find_last_of('/')) + xorstr("/Data");
        GameData::Initialize(dataPath);
    }

    // Init Script
    Script::Lol = Lol;
    Script::Control = Control;
    Script::LocalPlayer = LocalPlayer;
    Script::HeroList = HeroList;
    Script::MinionList = MinionList;
    Script::ChatInstance = ChatInstance;
    Script::ChatClient = ChatClient;

    Script *Combo = nullptr;
    if (std::string(xorstr("Irelia")).compare(LocalPlayer->GetName()) == 0)
    {
        Combo = new Irelia();
    }
    else if (std::string(xorstr("Yasuo")).compare(LocalPlayer->GetName()) == 0)
    {
        Combo = new Yasuo();
    }
    else if (std::string(xorstr("Yone")).compare(LocalPlayer->GetName()) == 0)
    {
        Combo = new Yone();
    }
    if (Combo != nullptr)
    {
        Combo->Load();
    }

    for (int i = 0; i < Memory::Module::ListModl.size(); i++)
    {
        ((void (*)(uint64_t *, const char *, unsigned int))(Lol->Address + Offset::Chat::PrintChat))(ChatInstance, Memory::Module::ListModl[i].c_str(), 1);
    }

    // Orbwalker
    while (Running)
    {
        if (mach_absolute_time() - LastLoop > 5000000)
        {
            // Script
            if (Combo != nullptr && *GameTime > LastScript + 0.03f)
            {
                Combo->Run();
                LastScript = *GameTime;
            }

            // Orbwalk
            if (Utils::IsPressed(kVK_Space) && !ChatClient->IsChatOpen)
            {
                GameObject *Target = Control->GetBestChampInRange(LocalPlayer, HeroList);
                if (Target != nullptr && *GameTime >= LastAttack + LocalPlayer->GetAttackDelay() + Control->GetPing() + 0.01f)
                {
                    Control->Attack(LocalPlayer, Target);
                    LastAttack = *GameTime;
                }
                else if (*GameTime > LastMove + 0.03f && *GameTime >= LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() + 0.01f)
                {
                    Vector3 *CurPos = Control->GetCursorPos();
                    Control->Move(LocalPlayer, CurPos);
                    LastMove = *GameTime;
                }
            }
            else if (Utils::IsPressed(kVK_ANSI_C) && !ChatClient->IsChatOpen)
            {
                GameObject *Target = Control->GetBestMinionInRange(LocalPlayer, MinionList);
                if (Target != nullptr && *GameTime >= LastAttack + LocalPlayer->GetAttackDelay() + Control->GetPing() + 0.01f)
                {
                    Control->Attack(LocalPlayer, Target);
                    LastAttack = *GameTime;
                }
                else if (*GameTime > LastMove + 0.03f && *GameTime >= LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() + 0.01f)
                {
                    Vector3 *CurPos = Control->GetCursorPos();
                    Control->Move(LocalPlayer, CurPos);
                    LastMove = *GameTime;
                }
            }
            else if (Utils::IsPressed(kVK_ANSI_X) && !ChatClient->IsChatOpen)
            {
                GameObject *Target = Control->GetMinionLastHitInRange(LocalPlayer, MinionList);
                if (Target != nullptr && *GameTime >= LastAttack + LocalPlayer->GetAttackDelay() + Control->GetPing() + 0.01f)
                {
                    Control->Attack(LocalPlayer, Target);
                    LastAttack = *GameTime;
                }
                else if (*GameTime > LastMove + 0.03f && *GameTime >= LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() + 0.01f)
                {
                    Vector3 *CurPos = Control->GetCursorPos();
                    Control->Move(LocalPlayer, CurPos);
                    LastMove = *GameTime;
                }
            }

            LastLoop = mach_absolute_time();
        }
    }

    return NULL;
}

void Deinitialize()
{
    Running = false;
    delete Control;
}

__attribute__((constructor)) void Load()
{
    pthread_t thrd;
    pthread_create(&thrd, NULL, Initialize, NULL);
}

__attribute__((destructor)) void Unload()
{
    Deinitialize();
}
