#include "TwistedFate.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

void TwistedFate::Load() {
    BlueKey = kVK_ANSI_1;
    RedKey = kVK_ANSI_2;
    YellowKey = kVK_ANSI_3;
    ComboKey = kVK_Space;
    Me = LocalPlayer;
    Q = Me->SpellBook.GetSpellSlotByID(0);
    W = Me->SpellBook.GetSpellSlotByID(1);
    LastCastW = 0;
}

void TwistedFate::Run() {
    if (Utils::IsPressed(ComboKey) && !ChatClient->IsChatOpen)
        Combo();
    else if (Utils::IsPressed(BlueKey) && !ChatClient->IsChatOpen)
        PickBlue();
    else if (Utils::IsPressed(RedKey) && !ChatClient->IsChatOpen)
        PickRed();
    else if (Utils::IsPressed(YellowKey) && !ChatClient->IsChatOpen)
        PickYellow();
}

bool TwistedFate::IsQReady() {
    return Me->SpellBook.IsReady(0);
}

bool TwistedFate::IsWReady() {
    return Me->SpellBook.IsReady(1);
}

void TwistedFate::CastQ(Vector3 pos) {
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

void TwistedFate::CastW() {
    auto a1 = (__int64)&Me->SpellBook;
    auto a2 = (__int64)W;
    auto a3 = 1;
    auto a4 = (__int64)W->CastData;
    auto a5 = Me->NetworkId;

    W->CastData->SourceIndex = Me->Index;
    W->CastData->TargetIndex = Me->Index;
    W->CastData->StartPos = Me->Position;
    W->CastData->EndPos = Me->Position;
    W->CastData->BoundPos = Me->Position;

    ((void (*)(__int64, __int64, unsigned int, __int64 *, int))(Lol->Address + Offset::Control::CastSpell))(a1, a2, a3, (__int64 *)(a4 + 8), a5);
    LastCastW = mach_absolute_time();
}

void TwistedFate::Combo() {
    if (Me->IsAlive() && IsQReady()) {
        GameObject *Enemy = HumanSelector(1450);
        if (Enemy != nullptr) {
            Vector3 predict = Control->PredictPos(Enemy, 1450.0 / 10000);
            if (Me->Position.distance_squared(predict) <= pow(1300, 2))
                CastQ(predict);
        }
    }

    GameObject *Enemy = HumanSelector(Me->AttackRange + Me->Info()->gameplayRadius + 250);
    if (Enemy != nullptr) {
        PickYellow();
    }
}

void TwistedFate::PickBlue() {
    if (Me->IsAlive() && IsWReady() && mach_absolute_time() - LastCastW > 150000000) {
        if (strcmp(W->Info->Data->SpellName, "PickACard") == 0) {
            CastW();
            return;
        }
        if (strcmp(W->Info->Data->SpellName, "BlueCardLock") == 0) {
            CastW();
        }
    }
}

void TwistedFate::PickRed() {
    if (Me->IsAlive() && IsWReady() && mach_absolute_time() - LastCastW > 150000000) {
        if (strcmp(W->Info->Data->SpellName, "PickACard") == 0) {
            CastW();
            return;
        }
        if (strcmp(W->Info->Data->SpellName, "RedCardLock") == 0) {
            CastW();
        }
    }
}

void TwistedFate::PickYellow() {
    if (Me->IsAlive() && IsWReady() && mach_absolute_time() - LastCastW > 150000000) {
        if (strcmp(W->Info->Data->SpellName, "PickACard") == 0) {
            CastW();
            return;
        }
        if (strcmp(W->Info->Data->SpellName, "GoldCardLock") == 0) {
            CastW();
        }
    }
}

GameObject *TwistedFate::HumanSelector(float range) {
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
    }

    return Target;
}
