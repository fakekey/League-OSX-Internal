//
//  Spell.cpp
//  Shit
//
//  Created by Nghi La on 10/09/2024.
//

#include "Spell.hpp"
#include "FuncExport.hpp"
#include "Offsets.hpp"
#include "Utils.hpp"

Memory::Module *OSpellBook::Lol = nullptr;

SpellSlot *OSpellBook::GetSpellSlotByID(int id) {
    return *(SpellSlot **)((_QWORD)this + 8 * id + 0xB00);
}

bool OSpellBook::IsReady(int id) {
    return GetSpellStateByID(id) == Offset::SpellState::Ready;
}

__int64 OSpellBook::GetSpellStateByID(int id) {
    _BYTE v36;
    auto GetSpellState = Lol->GetType<GetSpellStateFunc>(Offset::Control::GetSpellState);
    return GetSpellState((__int64)this, id, &v36);
}

MTL::Texture *SpellData::Icon() {
    return GameData::GetSpellIconByName(Character::ToLower(this->SpellName));
}

SpellDBInfo *SpellData::Info() {
    return GameData::GetSpellInfoByName(Character::ToLower(this->SpellName));
}

enum BuffType {
    Internal = 0,
    Aura = 1,
    CombatEnchancer = 2,
    CombatDehancer = 3,
    SpellShield = 4,
    Stun = 5,
    Invisibility = 6,
    Silence = 7,
    Taunt = 8,
    Berserk = 9,
    Polymorph = 10,
    Slow = 11,
    Snare = 12,
    Damage = 13,
    Heal = 14,
    Haste = 15,
    SpellImmunity = 16,
    PhysicalImmunity = 17,
    Invulnerability = 18,
    AttackSpeedSlow = 19,
    NearSight = 20,
    Fear = 22,
    Charm = 23,
    Poison = 24,
    Suppression = 25,
    Blind = 26,
    Counter = 27,
    Currency = 21,
    Shred = 28,
    Flee = 29,
    Knockup = 30,
    Knockback = 31,
    Disarm = 32,
    Grounded = 33,
    Drowsy = 34,
    Asleep = 35,
    Obscured = 36,
    ClickProofToEnemies = 37,
    Unkillable = 38
};

bool OBuffManager::HasBuff(std::string name) {
    __int64 v36 = (*(_QWORD *)((_QWORD)this + 0x20) - *(_QWORD *)((_QWORD)this + 0x18)) >> 4;

    for (unsigned int v6 = 0; v6 < (_DWORD)v36; v6++) {
        BuffInstance *buff = GetBuffInstance(v6);
        if (!buff || !buff->IsActive() || !buff->Info)
            continue;

        if (name.compare(buff->Info->Name) == 0)
            return true;
    }

    return false;
}

bool OBuffManager::HasBuff(std::string name, BuffInstance *out) {
    __int64 v36 = (*(_QWORD *)((_QWORD)this + 0x20) - *(_QWORD *)((_QWORD)this + 0x18)) >> 4;

    for (unsigned int v6 = 0; v6 < (_DWORD)v36; v6++) {
        BuffInstance *buff = GetBuffInstance(v6);
        if (!buff || !buff->IsActive() || !buff->Info)
            continue;

        if (name.compare(buff->Info->Name) == 0) {
            out = buff;
            return true;
        }
    }

    out = nullptr;
    return false;
}

BuffInstance *OBuffManager::GetBuffInstance(int id) {
    __int64 v3 = *(_QWORD *)((_QWORD)this + 0x18);
    if ((*(_QWORD *)((_QWORD)this + 0x20) - v3) >> 4 <= (unsigned __int64)id)
        return nullptr;
    else {
        __int64 v4 = 0x10LL * id;
        return *(BuffInstance **)(v3 + v4);
    }
}

bool BuffInstance::IsActive() {
    return ((bool (*)(__int64))(OSpellBook::Lol->Address + Offset::World::IsBuffActive))((_QWORD)this);
}
