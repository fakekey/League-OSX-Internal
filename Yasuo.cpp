#include "Yasuo.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

void Yasuo::Load() {
    LaneClearKey = kVK_ANSI_C;
    LastHitKey = kVK_ANSI_X;
    ComboKey = kVK_Space;
    Me = LocalPlayer;
    Q = Me->SpellBook.GetSpellSlotByID(0);
    E = Me->SpellBook.GetSpellSlotByID(2);
    LastE = 0;
}

void Yasuo::Run() {
    if (Utils::IsPressed(kVK_ANSI_E) && !ChatClient->IsChatOpen)
        LastE = mach_absolute_time();
    else if (Utils::IsPressed(ComboKey) && !ChatClient->IsChatOpen)
        Combo();
    else if (Utils::IsPressed(LaneClearKey) && !ChatClient->IsChatOpen)
        LaneClear();
    else if (Utils::IsPressed(LastHitKey) && !ChatClient->IsChatOpen)
        LastHit();
}

bool Yasuo::CanQLastHit(GameObject *Target) {
    float TotalAD = Me->BaseAttack + Me->BonusAttack;
    float QVatLy = QLevelDmg[Q->Level - 1] + (1.05f * TotalAD) + Control->GetOnHitPhysicFromItems(Me, Target);
    float QPhep = Control->GetOnHitMagicFromItems(Me, Target);
    float Dmg = Control->DmgOnArmor(Me, Target, QVatLy) + Control->DmgOnMagicRes(Me, Target, QPhep);
    return Target->Health - Dmg < 0.f ? true : false;
}

bool Yasuo::CanELastHit(GameObject *Target) {
    float bonusAD = Me->BonusAttack;
    float AP = Me->AbilityPower;
    float EDmg = ELevelDmg[E->Level - 1] + (0.2f * bonusAD) + (0.6f * AP);

    BuffInstance *EPassive = nullptr;
    if (Me->BuffManager.HasBuff("YasuoDashScalar", EPassive))
        if (EPassive != nullptr) {
            _DWORD stacks = EPassive->Count;
            EDmg = EDmg + (EStackLevelDmg[E->Level - 1] + (0.05f * bonusAD) + (0.15f * AP)) * stacks;
        }
    float Dmg = Control->DmgOnMagicRes(Me, Target, EDmg);
    return Target->Health - Dmg < 0.f ? true : false;
}

bool Yasuo::IsQReady() {
    return Me->SpellBook.IsReady(0);
}

bool Yasuo::IsEReady() {
    return Me->SpellBook.IsReady(2);
}

void Yasuo::CastQ(Vector3 pos) {
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

bool Yasuo::CastE(GameObject *Target) {
    if (Target->BuffManager.HasBuff("YasuoE"))
        return false;

    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)E;
    auto a3 = 2;
    auto a4 = (__int64)E->CastData;
    auto a5 = Target->NetworkId;

    E->CastData->SourceIndex = Me->Index;
    E->CastData->TargetIndex = Target->Index;
    E->CastData->StartPos = Me->Position;
    E->CastData->EndPos = Target->Position;
    E->CastData->BoundPos = Target->Position;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
    return true;
}

void Yasuo::LaneClear() {
    if (Me->IsAlive() && IsEReady()) {
        GameObject *Mob = MobESelector(475.f);
        if (Mob != nullptr) {
            if (CastE(Mob))
                LastE = mach_absolute_time();
        }
    }

    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YasuoQ1Wrapper") == 0 || strcmp(Q->Info->Data->SpellName, "YasuoQ2Wrapper") == 0) {
            GameObject *Mob = MobQSelector(450.f);
            if (Mob != nullptr)
                if (mach_absolute_time() - LastE > 420000000)
                    CastQ(Mob->Position);
        }

        if (strcmp(Q->Info->Data->SpellName, "YasuoQ3Wrapper") == 0) {
            GameObject *Mob = MobQSelector(1150.f);
            if (Mob != nullptr)
                if (mach_absolute_time() - LastE > 420000000)
                    CastQ(Mob->Position);
        }
    }
}

void Yasuo::LastHit() {
    if (Me->IsAlive() && IsEReady()) {
        GameObject *Mob = LastHitESelector(475.f);
        if (Mob != nullptr) {
            if (CastE(Mob))
                LastE = mach_absolute_time();
        }
    }

    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YasuoQ1Wrapper") == 0 || strcmp(Q->Info->Data->SpellName, "YasuoQ2Wrapper") == 0) {
            GameObject *Mob = LastHitQSelector(450.f);
            if (Mob != nullptr)
                if (mach_absolute_time() - LastE > 420000000)
                    CastQ(Mob->Position);
        }

        if (strcmp(Q->Info->Data->SpellName, "YasuoQ3Wrapper") == 0) {
            GameObject *Mob = LastHitQSelector(1150.f);
            if (Mob != nullptr)
                if (mach_absolute_time() - LastE > 420000000)
                    CastQ(Mob->Position);
        }
    }
}

void Yasuo::Combo() {
    if (Me->IsAlive() && IsEReady()) {
        GameObject *Enemy = HumanESelector(475.f);
        if (Enemy != nullptr) {
            if (CastE(Enemy))
                LastE = mach_absolute_time();
        }
    }

    if (Me->IsAlive() && IsQReady()) {
        if (strcmp(Q->Info->Data->SpellName, "YasuoQ1Wrapper") == 0 || strcmp(Q->Info->Data->SpellName, "YasuoQ2Wrapper") == 0) {
            GameObject *Enemy = HumanQSelector(450.f);
            if (Enemy != nullptr) {
                Vector3 predict = Control->PredictPos(Enemy, 450.f / 1500.f);
                if (Me->Position.distance_squared(predict) <= pow(450.f, 2))
                    if (mach_absolute_time() - LastE > 420000000)
                        CastQ(predict);
            }
        }

        if (strcmp(Q->Info->Data->SpellName, "YasuoQ3Wrapper") == 0) {
            GameObject *Enemy = HumanQSelector(1150.f);
            if (Enemy != nullptr) {
                Vector3 predict = Control->PredictPos(Enemy, 1150.f / 1500.f);
                if (Me->Position.distance_squared(predict) <= pow(1150.f, 2))
                    if (mach_absolute_time() - LastE > 420000000)
                        CastQ(predict);
            }
        }
    }
}

GameObject *Yasuo::HumanQSelector(float range) {
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

GameObject *Yasuo::HumanESelector(float range) {
    for (int i = 0; i < HeroList->Size; i++) {
        GameObject *Champ = HeroList->Item[i];
        if (Champ->IsAllyTo(Me))
            continue;
        if (Champ->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (!Champ->IsAttackable() || !Champ->Visible)
            continue;
        if (Champ->Position.distance_squared(Me->Position) >= pow(range + Champ->Info()->gameplayRadius / 2, 2))
            continue;
        if (CanELastHit(Champ))
            return Champ;
    }

    return nullptr;
}

GameObject *Yasuo::MobQSelector(float range) {
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

GameObject *Yasuo::MobESelector(float range) {
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
        if (CanELastHit(Mob))
            return Mob;
    }

    return nullptr;
}

GameObject *Yasuo::LastHitQSelector(float range) {
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

GameObject *Yasuo::LastHitESelector(float range) {
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
        if (CanELastHit(Mob))
            return Mob;
    }

    return nullptr;
}
