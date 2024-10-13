#include "GameObject.hpp"
#include "FuncExport.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

Memory::Module *GameObject::Lol = nullptr;
std::set<std::string> Controller::blacklistedObjectNames = {
    "testcube",
    "testcuberender",
    "testcuberender10vision",
    "s5test_wardcorpse",
    "sru_camprespawnmarker",
    "sru_plantrespawnmarker",
    "preseason_turret_shield",
    "shaco",
    "leblanc",
    "monkeyking",
    "neeko",
    "fiddlesticks",
};

const char *GameObject::GetName() {
    if ((*(_BYTE *)((_QWORD)this + 0x3920) & 1) != 0)
        return *(const char **)((_QWORD)this + 0x3930);
    else
        return (const char *)((_QWORD)this + 0x3921);
}

bool GameObject::IsDead() {
    int64_t v20 = (int64_t)this;
    return (*(int64_t (**)(int64_t))(*(_QWORD *)v20 + 0x280LL))(v20);
}

bool GameObject::IsAlive() {
    return !IsDead() && Health > 0.f;
}

bool GameObject::IsAttackable() {
    return IsAlive() && Targetable;
}

bool GameObject::IsRanged() {
    return Info()->attackRange >= 300.f;
}

UnitInfo *GameObject::Info() {
    return GameData::GetUnitInfoByName(Character::ToLower(this->GetName()));
}

MTL::Texture *GameObject::Icon() {
    return GameData::GetUnitIconByName(Character::ToLower(this->GetName()));
}

bool GameObject::IsAllyTo(GameObject *other) {
    return this->Team == other->Team;
}

bool GameObject::IsEnemyTo(GameObject *other) {
    return this->Team != other->Team;
}

bool GameObject::HasUnitTag(std::string tag) {
    auto info = this->Info();
    if (info == nullptr)
        return false;

    return std::find(info->tags.begin(), info->tags.end(), tag) != info->tags.end();
}

double GameObject::GetAttackDelay() {
    return ((float (*)(__int64))(Lol->Address + Offset::LocalPlayer::GetAttackDelay))((__int64)this);
}

double GameObject::GetAttackCastDelay() {
    return ((float (*)(__int64))(Lol->Address + Offset::LocalPlayer::GetAttackCastDelay))((__int64)this);
}

AIManager *GameObject::GetAIManager() {
    auto a1 = *(_QWORD *)((_QWORD)this + 0x3818);
    auto a2 = *(_QWORD *)(a1 + 0x10) + *(_QWORD *)(**(_QWORD **)(a1 + 0x10) - 0x18LL);
    return (AIManager *)a2;
}

Controller::Controller(Memory::Module *Lol) {
    Module = Lol;
}

void Controller::Move(GameObject *Obj, Vector3 *position) {
    auto RitoCheck = Module->GetType<__int128 *>(Offset::Control::Check);
    auto IssueOrder = Module->GetType<IssueOrderFunc>(Offset::Control::IssueOrder);

    *((_DWORD *)RitoCheck + 0x5A) = 0x13;
    IssueOrder((unsigned int *)Obj, Offset::Order::Move, (float *)position, 0, 0, 0, 0);
    *((_DWORD *)RitoCheck + 0x5A) = 0x13;
}

void Controller::Attack(GameObject *Obj, GameObject *Target) {
    auto RitoCheck = Module->GetType<__int128 *>(Offset::Control::Check);
    auto IssueOrder = Module->GetType<IssueOrderFunc>(Offset::Control::IssueOrder);

    *((_DWORD *)RitoCheck + 0x5A) = 0x14;
    IssueOrder((unsigned int *)Obj, Offset::Order::Attack, (float *)&Target->Position, (unsigned int *)Target, 0, 0, 0);
    *((_DWORD *)RitoCheck + 0x5A) = 0x14;
}

GameObject *Controller::GetBestChampInRange(GameObject *Obj, ObjectList *Champs, float range) {
    float minHP = 999999999.f;
    GameObject *Target = nullptr;

    if (range == 0.f)
        range = Obj->AttackRange + Obj->Info()->gameplayRadius;

    for (int i = 0; i < Champs->Size; i++) {
        GameObject *Champ = Champs->Item[i];
        if (Champ->IsAllyTo(Obj))
            continue;
        if (Champ->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Champ->IsAttackable() || !Champ->Visible)
            continue;
        if (Champ->Position.distance_squared(Obj->Position) >= pow(range + Champ->Info()->gameplayRadius / 2, 2))
            continue;
        if (minHP >= Champ->Health) {
            minHP = Champ->Health;
            Target = Champ;
        }
        if (IsLasthitable(Obj, Champ))
            return Champ;
    }

    return Target;
}

GameObject *Controller::GetBestMinionInRange(GameObject *Obj, ObjectList *Minions, float range) {
    float minHP = 999999999.f;
    GameObject *Target = nullptr;

    if (range == 0.f)
        range = Obj->AttackRange + Obj->Info()->gameplayRadius;

    for (int i = 0; i < Minions->Size; i++) {
        GameObject *Minion = Minions->Item[i];
        if (Minion->IsAllyTo(Obj))
            continue;
        if (Minion->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Minion->IsAttackable() || !Minion->Visible)
            continue;
        if (blacklistedObjectNames.find(Character::ToLower(Minion->GetName())) != blacklistedObjectNames.end())
            continue;
        if (Minion->Position.distance_squared(Obj->Position) >= pow(range + Minion->Info()->gameplayRadius / 2, 2))
            continue;
        if (minHP >= Minion->Health) {
            minHP = Minion->Health;
            Target = Minion;
        }
        if (IsLasthitable(Obj, Minion))
            return Minion;
    }

    return Target;
}

GameObject *Controller::GetMinionLastHitInRange(GameObject *Obj, ObjectList *Minions, float range) {
    if (range == 0.f)
        range = Obj->AttackRange + Obj->Info()->gameplayRadius;

    for (int i = 0; i < Minions->Size; i++) {
        GameObject *Minion = Minions->Item[i];
        if (Minion->IsAllyTo(Obj))
            continue;
        if (Minion->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Minion->IsAttackable() || !Minion->Visible)
            continue;
        if (blacklistedObjectNames.find(Character::ToLower(Minion->GetName())) != blacklistedObjectNames.end())
            continue;
        if (Minion->Position.distance_squared(Obj->Position) >= pow(range + Minion->Info()->gameplayRadius / 2, 2))
            continue;
        if (IsLasthitable(Obj, Minion))
            return Minion;
    }

    return nullptr;
}

Vector3 *Controller::GetCursorPos() {
    _QWORD *HudInstance = Module->GetType<_QWORD *>(Offset::World::HudInstance);
    _QWORD *MousePtr = (_QWORD *)(*HudInstance + 0x28);
    Vector3 *Pos = (Vector3 *)(*MousePtr + 0x2C);
    return Pos;
}

float Controller::GetPing() {
    __int64 *NetClient = Module->GetType<__int64 *>(Offset::World::NetClient);
    return ((__int64 (*)(__int64))(Module->Address + Offset::World::GetPing))(*NetClient) / 1000.0;
}

bool Controller::IsLasthitable(GameObject *Me, GameObject *Target) {
    float hitDmg = DmgOnArmor(Me, Target, Me->BaseAttack + Me->BonusAttack + GetOnHitPhysicFromItems(Me, Target)) + DmgOnMagicRes(Me, Target, GetOnHitMagicFromItems(Me, Target));
    return Target->Health - hitDmg < 0.f;
}

float Controller::DmgOnArmor(GameObject *Me, GameObject *Target, float Dmg) {
    float calcArmor = Target->Armor * Me->ArmorPen - Me->Lethality;
    calcArmor = calcArmor < 0.f ? 0.f : calcArmor;
    return (Dmg * 100.f) / (calcArmor + 100.f);
}

float Controller::DmgOnMagicRes(GameObject *Me, GameObject *Target, float Dmg) {
    float calcMagicRes = Target->MagicRes * Me->MagicResPen - Me->MagicResPenFlat;
    calcMagicRes = calcMagicRes < 0.f ? 0.f : calcMagicRes;
    return (Dmg * 100.f) / (calcMagicRes + 100.f);
}

float Controller::OnHitBeginItem(GameObject *Me, GameObject *Target) {
    return Target->HasUnitTag("Unit_Minion_Lane") ? 5.f : 0.f;
}

float Controller::OnHitRecurveBow(GameObject *Me, GameObject *Target) {
    return 15.f;
}

float Controller::OnHitWitsEnd(GameObject *Me, GameObject *Target) {
    return 45.f;
}

float Controller::OnHitNashorsTooth(GameObject *Me, GameObject *Target) {
    return 15.f + Me->AbilityPower * 15.f / 100.f;
}

float Controller::OnHitGuinsoosRageblade(GameObject *Me, GameObject *Target) {
    return 30.f;
}

float Controller::OnHitTerminus(GameObject *Me, GameObject *Target) {
    return 30.f;
}

float Controller::OnHitBladeOfTheRuinedKing(GameObject *Me, GameObject *Target) {
    float Dmg = Me->IsRanged() ? Target->Health * 6.f / 100.f : Target->Health * 10.f / 100.f;
    if (Dmg > 100.f && !Target->HasUnitTag("Unit_Champion"))
        return 100.f;
    if (Dmg < 15.f)
        return 15.f;
    return Dmg;
}

float Controller::GetOnHitPhysical(GameObject *Me, GameObject *Target, __int64 ItemId) {
    switch (ItemId) {
    case 3070:
    case 1056:
    case 1054:
        return OnHitBeginItem(Me, Target);

    case 1043:
        return OnHitRecurveBow(Me, Target);

    case 3153:
        return OnHitBladeOfTheRuinedKing(Me, Target);

    default:
        return 0.f;
    }
}

float Controller::GetOnHitMagical(GameObject *Me, GameObject *Target, __int64 ItemId) {
    switch (ItemId) {
    case 3091:
        return OnHitWitsEnd(Me, Target);

    case 3115:
        return OnHitNashorsTooth(Me, Target);

    case 3124:
        return OnHitGuinsoosRageblade(Me, Target);

    case 3302:
        return OnHitTerminus(Me, Target);

    default:
        return 0.f;
    }
}

float Controller::GetOnHitPhysicFromItems(GameObject *Me, GameObject *Target) {
    float Dmg = 0.f;
    for (int i = 0; i < 7; i++) {
        ItemSlot *itemSlot = Me->Inventory->GetItemSlotByID(i);
        if (itemSlot == nullptr || itemSlot->Item == nullptr || itemSlot->Item->Info == nullptr)
            continue;
        Dmg += GetOnHitPhysical(Me, Target, itemSlot->Item->Info->ItemID);
    }

    return Dmg;
}

float Controller::GetOnHitMagicFromItems(GameObject *Me, GameObject *Target) {
    float Dmg = 0.f;
    for (int i = 0; i < 7; i++) {
        ItemSlot *itemSlot = Me->Inventory->GetItemSlotByID(i);
        if (itemSlot == nullptr || itemSlot->Item == nullptr || itemSlot->Item->Info == nullptr)
            continue;
        Dmg += GetOnHitMagical(Me, Target, itemSlot->Item->Info->ItemID);
    }

    return Dmg;
}

Vector3 Controller::PredictPos(GameObject *Target, float PredictTime) {
    AIManager *AIBase = Target->GetAIManager();
    if (!AIBase->IsMoving) {
        return Target->Position;
    }
    if (AIBase->IsDashing()) {
        return AIBase->EndPos;
    }

    // Vector3 Direct = AIBase->EndPos.sub(AIBase->StartPos).normalize();
    Vector3 Direct = AIBase->Velocity.normalize();
    float Distance = AIBase->MoveSpeed * (PredictTime + GetPing());

    return Target->Position.add(Direct.scale(Distance));
}

void Controller::PrintChat(const char *Msg) {
    ((void (*)(uint64_t *, const char *, unsigned int))(Module->Address + Offset::Chat::PrintChat))((uint64_t *)(Module->Address + Offset::Chat::ChatInstance), Msg, 1);
}

bool AIManager::IsDashing() {
    return IsMoving && Dash;
}
