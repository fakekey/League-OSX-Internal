//
//  GameData.cpp
//  Shit
//
//  Created by Nghi La on 08/09/2024.
//

#include "GameData.hpp"

std::map<std::string, UnitInfo *> GameData::Units = {};
std::map<std::string, SpellDBInfo *> GameData::Spells = {};
std::map<std::string, SpellDBInfo *> GameData::SpellAlts = {};

void UnitInfo::SetTag(std::string tag)
{
    tags.push_back(tag);
}

void GameData::Initialize(std::string &DataPath)
{
    std::string unitData = DataPath + "/UnitData.json";
    std::string spellData = DataPath + "/SpellData.json";
    LoadUnitData(unitData);
    LoadSpellData(spellData);
}

SpellDBInfo *GameData::GetSpellInfoByName(std::string name)
{
    auto it = Spells.find(name);
    if (it != Spells.end())
        return it->second;

    it = SpellAlts.find(name);
    if (it != SpellAlts.end())
        return it->second;

    return nullptr;
}

UnitInfo *GameData::GetUnitInfoByName(std::string name)
{
    auto it = Units.find(name);
    if (it != Units.end())
        return it->second;

    return nullptr;
}

void GameData::LoadUnitData(std::string &path)
{
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto &unit : data)
    {
        UnitInfo *info = new UnitInfo();

        info->name = Character::ToLower(unit["name"]);
        info->healthBarHeight = unit["healthBarHeight"];
        info->baseMoveSpeed = unit["baseMoveSpeed"];
        info->attackRange = unit["attackRange"];
        info->attackSpeed = unit["attackSpeed"];
        info->attackSpeedRatio = unit["attackSpeedRatio"];
        info->acquisitionRange = unit["acquisitionRange"];
        info->selectionRadius = unit["selectionRadius"];
        info->pathingRadius = unit["pathingRadius"];
        info->gameplayRadius = unit["gameplayRadius"];
        info->basicAtkMissileSpeed = unit["basicAtkMissileSpeed"];
        info->basicAtkWindup = unit["basicAtkWindup"];
        info->basicAtkWindupModify = unit["basicAtkWindupModify"];

        for (auto &tag : unit["tags"])
            info->SetTag(tag);

        Units[info->name] = info;
    }
}

void GameData::LoadSpellData(std::string &path)
{
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto &spell : data)
    {
        SpellDBInfo *info = new SpellDBInfo();

        info->name = Character::ToLower(spell["name"]);
        info->nameAlt = Character::ToLower(spell["nameAlt"]);
        info->icon = Character::ToLower(spell["icon"]);
        info->flags = spell["flags"];
        info->delay = spell["delay"];
        info->castRange = spell["castRange"];
        info->castRadius = spell["castRadius"];
        info->width = spell["width"];
        info->height = spell["height"];
        info->speed = spell["speed"];
        info->travelTime = spell["travelTime"];
        info->type = spell["type"];
        info->projectDestination = spell["projectDestination"];

        Spells[info->name] = info;
        SpellAlts[info->nameAlt] = info;
    }
}
