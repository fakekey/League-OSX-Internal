#pragma once
#ifndef GameData_hpp
#define GameData_hpp

#include "Utils.hpp"
#include "JSON.hpp"
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>

using json = nlohmann::json;

struct SpellDBInfo
{
public:
    std::string name;
    std::string nameAlt;
    std::string icon;
    int flags;
    float delay;
    float castRange;
    float castRadius;
    float width;
    float height;
    float speed;
    float travelTime;
    std::string type;
    bool projectDestination;
};

struct UnitInfo
{
public:
    std::string name;
    float healthBarHeight;
    float baseMoveSpeed;
    float attackRange;
    double attackSpeed;
    double attackSpeedRatio;
    float acquisitionRange;
    float selectionRadius;
    float pathingRadius;
    float gameplayRadius;
    float basicAtkMissileSpeed;
    double basicAtkWindup;
    float basicAtkWindupModify;
    std::vector<std::string> tags;

public:
    void SetTag(std::string tag);
};

class GameData
{
public:
    static std::map<std::string, UnitInfo *> Units;
    static std::map<std::string, SpellDBInfo *> Spells;
    static std::map<std::string, SpellDBInfo *> SpellAlts;

public:
    static void Initialize(std::string &DataPath);
    static SpellDBInfo *GetSpellInfoByName(std::string name);
    static UnitInfo *GetUnitInfoByName(std::string name);

private:
    static void LoadUnitData(std::string &path);
    static void LoadSpellData(std::string &path);
};

#endif /* GameData_hpp */
