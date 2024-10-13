//
//  GameData.cpp
//  Shit
//
//  Created by Nghi La on 08/09/2024.
//

#include "GameData.hpp"
#include "Utils.h"
#include "Utils.hpp"
#include <fstream>

std::map<std::string, UnitInfo *> GameData::Units = {};
std::map<std::string, SpellDBInfo *> GameData::Spells = {};
std::map<std::string, SpellDBInfo *> GameData::SpellAlts = {};
std::map<std::string, MTL::Texture *> GameData::SpellIcons = {};
std::map<std::string, MTL::Texture *> GameData::UnitIcons = {};

void UnitInfo::SetTag(std::string tag) {
    tags.push_back(tag);
}

void GameData::Initialize(std::string &DataPath, MTL::Device *pDevice) {
    std::string unitData = DataPath + "/UnitData.json";
    std::string spellData = DataPath + "/SpellData.json";
    std::string unitIcon = DataPath + "/square_icons/";
    std::string spellIcon = DataPath + "/icons_spells/";
    LoadUnitData(unitData, unitIcon, pDevice);
    LoadSpellData(spellData, spellIcon, pDevice);
}

SpellDBInfo *GameData::GetSpellInfoByName(std::string name) {
    auto it = Spells.find(name);
    if (it != Spells.end())
        return it->second;

    it = SpellAlts.find(name);
    if (it != SpellAlts.end())
        return it->second;

    return nullptr;
}

UnitInfo *GameData::GetUnitInfoByName(std::string name) {
    auto it = Units.find(name);
    if (it != Units.end())
        return it->second;

    return nullptr;
}

MTL::Texture *GameData::GetUnitIconByName(std::string name) {
    auto it = UnitIcons.find(name);
    if (it != UnitIcons.end())
        return it->second;

    return nullptr;
}

MTL::Texture *GameData::GetSpellIconByName(std::string name) {
    auto it = SpellIcons.find(name);
    if (it != SpellIcons.end())
        return it->second;

    return nullptr;
}

void GameData::Deallocate() {
    for (auto const &[key, val] : UnitIcons) {
        if (val != nullptr) {
            val->release();
        }
    }
    for (auto const &[key, val] : SpellIcons) {
        if (val != nullptr) {
            val->release();
        }
    }
}

void GameData::LoadUnitData(std::string &path, std::string &iconPath, MTL::Device *pDevice) {
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto &unit : data) {
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
        UnitIcons[info->name] = LoadTextureFromImagePath(pDevice, (iconPath + info->name + "_square.png").c_str());
    }

    UnitIcons["outofchakra"] = LoadTextureFromImagePath(pDevice, (iconPath + "outofchakra.png").c_str());
}

void GameData::LoadSpellData(std::string &path, std::string &iconPath, MTL::Device *pDevice) {
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto &spell : data) {
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
        info->skillShotType = spell["skillShotType"];

        Spells[info->name] = info;
        SpellAlts[info->nameAlt] = info;
        auto texture = LoadTextureFromImagePath(pDevice, (iconPath + info->icon + ".png").c_str());
        SpellIcons[info->name] = texture;
        SpellIcons[info->nameAlt] = texture;
    }

    SpellIcons["summonerbarrier"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerbarrier.png").c_str());
    SpellIcons["summonerboost"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerboost.png").c_str());
    SpellIcons["snowballfollowupcast"] = LoadTextureFromImagePath(pDevice, (iconPath + "snowballfollowupcast.png").c_str());
    SpellIcons["summonerexhaust"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerexhaust.png").c_str());
    SpellIcons["summonerflash"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerflash.png").c_str());
    SpellIcons["summonerhaste"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerhaste.png").c_str());
    SpellIcons["summonerheal"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerheal.png").c_str());
    SpellIcons["summonerdot"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerdot.png").c_str());
    SpellIcons["summonermana"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonermana.png").c_str());
    SpellIcons["summonersnowball"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonersnowball.png").c_str());
    SpellIcons["summonersmite"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonersmite.png").c_str());
    SpellIcons["summonerteleport"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerteleport.png").c_str());
    SpellIcons["s5_summonersmiteplayerganker"] = LoadTextureFromImagePath(pDevice, (iconPath + "s5_summonersmiteplayerganker.png").c_str());
    SpellIcons["summonersmiteavataroffensive"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonersmiteavataroffensive.png").c_str());
    SpellIcons["summonersmiteavatardefensive"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonersmiteavatardefensive.png").c_str());
    SpellIcons["summonersmiteavatarutility"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonersmiteavatarutility.png").c_str());
    SpellIcons["summonerflashperkshextechflashtraptionv2"] = LoadTextureFromImagePath(pDevice, (iconPath + "summonerflashperkshextechflashtraptionv2.png").c_str());
    SpellIcons["s12_summonerteleportupgrade"] = LoadTextureFromImagePath(pDevice, (iconPath + "s12_summonerteleportupgrade.png").c_str());
}
