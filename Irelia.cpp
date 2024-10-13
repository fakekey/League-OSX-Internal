#include "Irelia.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

void Irelia::Load() {
    LaneClearKey = kVK_ANSI_C;
    LastHitKey = kVK_ANSI_X;
    ComboKey = kVK_Space;
    Me = LocalPlayer;
    Q = Me->SpellBook.GetSpellSlotByID(0);
    E = Me->SpellBook.GetSpellSlotByID(2);
}

void Irelia::Run() {
    if (Utils::IsPressed(ComboKey) && !ChatClient->IsChatOpen)
        Combo();
    else if (Utils::IsPressed(LaneClearKey) && !ChatClient->IsChatOpen)
        LaneClear();
    else if (Utils::IsPressed(LastHitKey) && !ChatClient->IsChatOpen)
        LaneClear();
}

std::tuple<float, float> Irelia::QDmg(GameObject *Target) {
    float TotalAtk = Me->BaseAttack + Me->BonusAttack;
    QMinionDmg = 43.f + 12.f * Me->Level;

    if (Me->BuffManager.HasBuff("ireliapassivestacksmax"))
        PassiveDmg = (7.f + 3.f * Me->Level) + (0.2f * Me->BonusAttack);
    else
        PassiveDmg = 0.f;

    float QPhysDmg = QLevelDmg[Q->Level - 1] + (0.6f * TotalAtk) + Control->GetOnHitPhysicFromItems(Me, Target);
    float QMagDmg = PassiveDmg + Control->GetOnHitMagicFromItems(Me, Target);

    return {QPhysDmg, QMagDmg};
}

bool Irelia::CanQ(GameObject *Target) {
    float TotalDmg = 0.f;
    std::tuple<float, float> QCalcDmg = QDmg(Target);
    float DmgQAd = std::get<0>(QCalcDmg);
    float DmgQAp = std::get<1>(QCalcDmg);

    if (Target->HasUnitTag("Unit_Minion_Lane"))
        TotalDmg = Control->DmgOnArmor(Me, Target, DmgQAd + QMinionDmg) + Control->DmgOnMagicRes(Me, Target, DmgQAp);
    else
        TotalDmg = Control->DmgOnArmor(Me, Target, DmgQAd) + Control->DmgOnMagicRes(Me, Target, DmgQAp);

    if (Target->Health - TotalDmg < 0.f)
        return true;

    if (Target->HasUnitTag("Unit_Champion") || Target->HasUnitTag("Unit_Monster")) {
        if (Target->BuffManager.HasBuff("ireliamark"))
            return true;
    }

    return false;
}

bool Irelia::IsQReady() {
    return Me->SpellBook.IsReady(0);
}

bool Irelia::IsEReady() {
    return Me->SpellBook.IsReady(2);
}

GameObject *Irelia::GetWeakestTarget(float range) {
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
        if (CanQ(Champ))
            return Champ;
    }

    return Target;
}

GameObject *Irelia::GetClosestMobToEnemyForGap(GameObject *Enemy) {
    for (int i = 0; i < MinionList->Size; i++) {
        GameObject *Mob = MinionList->Item[i];
        if (Mob->IsAllyTo(Me))
            continue;
        if (Controller::blacklistedObjectNames.find(Character::ToLower(Mob->GetName())) != Controller::blacklistedObjectNames.end())
            continue;
        if (Mob->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (Mob->HasUnitTag("Unit_Plant"))
            continue;
        if (!Mob->IsAttackable() || !Mob->Visible)
            continue;
        if (Mob->Position.distance_squared(Me->Position) >= pow(600 + Mob->Info()->gameplayRadius / 2, 2))
            continue;
        float MeEnemyDist = Me->Position.distance_squared(Enemy->Position);
        float EnemyMobDist = Enemy->Position.distance_squared(Mob->Position);
        if (EnemyMobDist < MeEnemyDist && CanQ(Mob))
            return Mob;
    }

    return nullptr;
}

GameObject *Irelia::MobSelector(float range) {
    for (int i = 0; i < MinionList->Size; i++) {
        GameObject *Mob = MinionList->Item[i];
        if (Mob->IsAllyTo(Me))
            continue;
        if (Controller::blacklistedObjectNames.find(Character::ToLower(Mob->GetName())) != Controller::blacklistedObjectNames.end())
            continue;
        if (Mob->HasUnitTag("Unit_Champion_Clone"))
            continue;
        if (Mob->HasUnitTag("Unit_Plant"))
            continue;
        if (!Mob->IsAttackable() || !Mob->Visible)
            continue;
        if (Mob->Position.distance_squared(Me->Position) >= pow(range + Mob->Info()->gameplayRadius / 2, 2))
            continue;
        if (CanQ(Mob))
            return Mob;
    }

    return nullptr;
}

GameObject *Irelia::HumanSelector(float range) {
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
        if (CanQ(Champ))
            return Champ;
    }

    return nullptr;
}

void Irelia::LaneClear() {
    if (Me->IsAlive() && IsQReady()) {
        GameObject *Mob = MobSelector(600.f);
        if (Mob != nullptr)
            CastQ(Mob);
    }
}

void Irelia::Combo() {
    if (Me->IsAlive() && IsQReady()) {
        GameObject *Target = HumanSelector(600.f);
        if (Target != nullptr)
            CastQ(Target);
    }
    if (Me->IsAlive() && IsQReady()) {
        GameObject *Target = GetWeakestTarget(3000.f);
        if (Target != nullptr) {
            if (IsEReady() && Me->Position.distance_squared(Target->Position) <= pow(775.f, 2))
                CastE2(Target);
            GameObject *Mob = GetClosestMobToEnemyForGap(Target);
            if (Mob != nullptr) {
                Vector3 tmpPos = Mob->Position;
                CastQ(Mob);
                if (IsEReady() && tmpPos.distance_squared(Target->Position) <= pow(775.f, 2))
                    CastE1(tmpPos);
            }
        }
    }
}

void Irelia::CastQ(GameObject *Target) {
    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)Q;
    auto a3 = 0;
    auto a4 = (__int64)Q->CastData;
    auto a5 = Target->NetworkId;

    Q->CastData->SourceIndex = Me->Index;
    Q->CastData->TargetIndex = Target->Index;
    Q->CastData->StartPos = Me->Position;
    Q->CastData->EndPos = Target->Position;
    Q->CastData->BoundPos = Target->Position;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
}

void Irelia::CastE(Vector3 pos) {
    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)E;
    auto a3 = 2;
    auto a4 = (__int64)E->CastData;
    auto a5 = 0;

    E->CastData->SourceIndex = Me->Index;
    E->CastData->StartPos = Me->Position;
    E->CastData->EndPos = pos;
    E->CastData->BoundPos = pos;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
}

void Irelia::CastE1(Vector3 pos) {
    if (strcmp(E->Info->Data->SpellName, "IreliaE") == 0)
        CastE(pos);
}

void Irelia::CastE2(GameObject *Target) {
    if (strcmp(E->Info->Data->SpellName, "IreliaE2") == 0) {
        Vector3 predict = Control->PredictPos(Target, 0.1f);
        if (Me->Position.distance_squared(predict) <= pow(775.f, 2))
            CastE(predict.add((predict.sub(Me->Position)).normalize().scale(775.f)));
    }
}
