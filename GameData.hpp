#pragma once
#ifndef GameData_hpp
#define GameData_hpp

#include "JSON.hpp"
#include "Metal/MTLDevice.hpp"
#include "Metal/MTLTexture.hpp"
#include <map>
#include <vector>

using json = nlohmann::json;

struct SpellDBInfo {
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
    uint8_t skillShotType;
};

struct UnitInfo {
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

class GameData {
  public:
    static std::map<std::string, UnitInfo *> Units;
    static std::map<std::string, SpellDBInfo *> Spells;
    static std::map<std::string, SpellDBInfo *> SpellAlts;
    static std::map<std::string, MTL::Texture *> SpellIcons;
    static std::map<std::string, MTL::Texture *> UnitIcons;

  public:
    static void Initialize(std::string &DataPath, MTL::Device *pDevice);
    static SpellDBInfo *GetSpellInfoByName(std::string name);
    static UnitInfo *GetUnitInfoByName(std::string name);
    static MTL::Texture *GetUnitIconByName(std::string name);
    static MTL::Texture *GetSpellIconByName(std::string name);
    static void Deallocate();

  private:
    static void LoadUnitData(std::string &path, std::string &iconPath, MTL::Device *pDevice);
    static void LoadSpellData(std::string &path, std::string &iconPath, MTL::Device *pDevice);
};

#endif /* GameData_hpp */
