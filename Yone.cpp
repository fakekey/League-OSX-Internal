#include "Yone.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

void Yone::Load() {
    LaneClearKey = kVK_ANSI_C;
    LastHitKey = kVK_ANSI_X;
    ComboKey = kVK_Space;
    Me = LocalPlayer;
    Q = Me->SpellBook.GetSpellSlotByID(0);
    W = Me->SpellBook.GetSpellSlotByID(1);
}

void Yone::Run() {
    if (Utils::IsPressed(ComboKey) && !ChatClient->IsChatOpen)
        Combo();
    else if (Utils::IsPressed(LaneClearKey) && !ChatClient->IsChatOpen)
        LaneClear();
    else if (Utils::IsPressed(LastHitKey) && !ChatClient->IsChatOpen)
        LastHit();
}

bool Yone::CanQLastHit(GameObject *Target) {
    float TotalAD = Me->BaseAttack + Me->BonusAttack;
    float QPhysDmg = QLevelDmg[Q->Level - 1] + (1.05f * TotalAD) + Control->GetOnHitPhysicFromItems(Me, Target);
    float QMagDmg = Control->GetOnHitMagicFromItems(Me, Target);
    float TotalDmg = Control->DmgOnArmor(Me, Target, QPhysDmg) + Control->DmgOnMagicRes(Me, Target, QMagDmg);

    if (Target->Health - TotalDmg < 0.f)
        return true;

    return false;
}

bool Yone::IsQReady() {
    return Me->SpellBook.IsReady(0);
}

bool Yone::IsWReady() {
    return Me->SpellBook.IsReady(1);
}

void Yone::CastQ(Vector3 pos) {
    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)Q;
    auto a3 = 0;
    auto a4 = (__int64)Q->CastData;
    auto a5 = 0;

    Q->CastData->SourceIndex = Me->Index;
    Q->CastData->StartPos = Me->Position;
    Q->CastData->EndPos = pos;
    Q->CastData->BoundPos = pos;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
}

void Yone::CastW(Vector3 pos) {
    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)W;
    auto a3 = 1;
    auto a4 = (__int64)W->CastData;
    auto a5 = 0;

    W->CastData->SourceIndex = Me->Index;
    W->CastData->StartPos = Me->Position;
    W->CastData->EndPos = pos;
    W->CastData->BoundPos = pos;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
}

void Yone::LaneClear() {
    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YoneQ") == 0) {
            GameObject *Mob = MobSelector(450.f);
            if (Mob != nullptr)
                CastQ(Mob->Position);
        }

        if (strcmp(Q->Info->Data->SpellName, "YoneQ3") == 0) {
            GameObject *Mob = MobSelector(1050.f);
            if (Mob != nullptr)
                CastQ(Mob->Position);
        }
    }
}

void Yone::LastHit() {
    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YoneQ") == 0) {
            GameObject *Mob = LastHitSelector(450.f);
            if (Mob != nullptr)
                CastQ(Mob->Position);
        }

        if (strcmp(Q->Info->Data->SpellName, "YoneQ3") == 0) {
            GameObject *Mob = LastHitSelector(1050.f);
            if (Mob != nullptr)
                CastQ(Mob->Position);
        }
    }
}

void Yone::Combo() {
    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YoneQ") == 0) {
            GameObject *Enemy = HumanSelector(450.f);
            if (Enemy != nullptr) {
                Vector3 predict = Control->PredictPos(Enemy, 450.f / 1500.f);
                if (Me->Position.distance_squared(predict) <= pow(450.f, 2))
                    CastQ(predict);
            }
        }

        if (strcmp(Q->Info->Data->SpellName, "YoneQ3") == 0) {
            GameObject *Enemy = HumanSelector(1050.f);
            if (Enemy != nullptr) {
                Vector3 predict = Control->PredictPos(Enemy, 1050.f / 1500.f);
                if (Me->Position.distance_squared(predict) <= pow(1050.f, 2))
                    CastQ(predict);
            }
        }
    }

    if (Me->IsAlive() && IsWReady()) {
        GameObject *Enemy = HumanSelector(650.f);
        if (Enemy != nullptr) {
            Vector3 predict = Control->PredictPos(Enemy, 600.f / 1800.f);
            if (Me->Position.distance_squared(predict) <= pow(650.f, 2))
                CastW(predict);
        }
    }
}

GameObject *Yone::HumanSelector(float range) {
    float minHP = 999999999.f;
    GameObject *Target = nullptr;

    for (int i = 0; i < HeroList->Size; i++) {
        GameObject *Champ = HeroList->Item[i];
        if (Champ->IsAllyTo(Me))
            continue;
        if (Champ->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Champ->IsAttackable())
            continue;
        if (Champ->Position.distance_squared(Me->Position) >= pow(range + Champ->Info()->gameplayRadius / 2, 2))
            continue;
        if (minHP >= Champ->Health) {
            minHP = Champ->Health;
            Target = Champ;
        }
        if (CanQLastHit(Champ))
            return Champ;
    }

    return Target;
}

GameObject *Yone::MobSelector(float range) {
    float minHP = 999999999.f;
    GameObject *Target = nullptr;

    for (int i = 0; i < MinionList->Size; i++) {
        GameObject *Mob = MinionList->Item[i];
        if (Mob->IsAllyTo(Me))
            continue;
        if (Mob->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (Mob->HasUnitTag("Unit_Plant"))
            continue;
        if (!Mob->IsAttackable() || !Mob->Visible)
            continue;
        if (Controller::blacklistedObjectNames.find(Character::ToLower(Mob->GetName())) != Controller::blacklistedObjectNames.end())
            continue;
        if (Mob->Position.distance_squared(Me->Position) >= pow(range + Mob->Info()->gameplayRadius / 2, 2))
            continue;
        if (minHP >= Mob->Health) {
            minHP = Mob->Health;
            Target = Mob;
        }
        if (CanQLastHit(Mob))
            return Mob;
    }

    return Target;
}

GameObject *Yone::LastHitSelector(float range) {
    for (int i = 0; i < MinionList->Size; i++) {
        GameObject *Mob = MinionList->Item[i];
        if (Mob->IsAllyTo(Me))
            continue;
        if (Mob->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Mob->IsAttackable() || !Mob->Visible)
            continue;
        if (Controller::blacklistedObjectNames.find(Character::ToLower(Mob->GetName())) != Controller::blacklistedObjectNames.end())
            continue;
        if (Mob->Position.distance_squared(Me->Position) >= pow(range + Mob->Info()->gameplayRadius / 2, 2))
            continue;
        if (CanQLastHit(Mob))
            return Mob;
    }

    return nullptr;
}
