import json
import os

unit_data_folder = 'unit_data'
items = []
jdata = None

with open(os.path.join(unit_data_folder, 'item.json')) as file:
    jdata = json.loads(file.read())

jitems = jdata["data"]

for id, jitem in jitems.items():

    jstats = jitem["stats"]
    items.append({
        "id":                   int(id),
        "name":                 jitem["name"],
        "cost":                 jitem["gold"]["total"],
        "movementSpeed":        jstats.get("FlatMovementSpeedMod", 0.0),
        "health":               jstats.get("FlatHPPoolMod", 0.0),
        "crit":                 jstats.get("FlatCritChanceMod", 0.0),
        "abilityPower":         jstats.get("FlatMagicDamageMod", 0.0),
        "mana":                 jstats.get("FlatMPPoolMod", 0.0),
        "armour":               jstats.get("FlatArmorMod", 0.0),
        "magicResist":          jstats.get("FlatSpellBlockMod", 0.0),
        "physicalDamage":       jstats.get("FlatPhysicalDamageMod", 0.0),
        "attackSpeed":          jstats.get("PercentAttackSpeedMod", 0.0),
        "lifeSteal":            jstats.get("PercentLifeStealMod", 0.0),
        "hpRegen":              jstats.get("FlatHPRegenMod", 0.0),
        "movementSpeedPercent": jstats.get("PercentMovementSpeedMod", 0.0)
    })

with open("item_data.json", 'w') as f:
    f.write(json.dumps(items, indent=4))