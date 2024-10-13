'''
	Utility script that generates two JSONs. One with unit data and one with spell data for unit spells. It extracts this data from the unit data downloaded with DownloadUnitData.py.
'''

import os
import json


def find_key_ending_with(dictionary, partial_key):
    for key, val in dictionary.items():
        if key.endswith(partial_key):
            return val
    return None


units, spells = {}, {}
infos = ''
unit_tags = set()
unit_data_folder = 'unit_data'

for fname in os.listdir(unit_data_folder):
    print("Processing: " + fname)
    if fname.startswith(('brush_', 'nexusblitz_', 'slime_', 'tft4_', 'tft_')):
        print('Object blacklisted. Skipping...')
        continue

    props = {}
    with open(os.path.join(unit_data_folder, fname)) as file:
        props = json.loads(file.read())

    # Find character property node
    root = find_key_ending_with(props, '/Root')
    if not root:
        print('[Fail] No root found for: ' + fname)
        continue

    # Get character name
    name = root.get('mCharacterName', '')
    if len(name) == 0:
        print('[Fail] No character name found for: ' + fname)
        continue

    # Get basic attack info
    missile_speed = 0.0
    windup = 0.0
    basicAtkWindupModify = 0.0
    basic_attack = find_key_ending_with(props, name + "BasicAttack")
    if basic_attack != None:
        spell = basic_attack.get('mSpell', None)
        if spell:
            missile_speed = spell.get("missileSpeed", 0.0)
    if 'basicAttack' in root:
        basic_attack = root['basicAttack']
        if 'mAttackTotalTime' in basic_attack and 'mAttackCastTime' in basic_attack:
            windup = basic_attack['mAttackCastTime']/basic_attack['mAttackTotalTime']
        else:
            windup = 0.3 + basic_attack.get('mAttackDelayCastOffsetPercent', 0.0)
        if 'mAttackDelayCastOffsetPercentAttackSpeedRatio' in basic_attack:
            basicAtkWindupModify = basic_attack['mAttackDelayCastOffsetPercentAttackSpeedRatio']

    tags = set(['Unit_' + x.strip().replace('=', '_') for x in root.get("unitTagsString", "").split('|')])
    unit = {
        "name":             name.lower(),
        "healthBarHeight":  root.get("healthBarHeight", 100.0),
        "baseMoveSpeed":    root.get("baseMoveSpeed", 0.0),
        "attackRange":      root.get("attackRange", 0.0),
        "attackSpeed":      root.get("attackSpeed", 0.0),
        "attackSpeedRatio": root.get("attackSpeedRatio", 0.0),
        "acquisitionRange": root.get("acquisitionRange", 0.0),
        "selectionRadius":  root.get("selectionRadius", 0.0),
        "pathingRadius":    root.get("pathfindingCollisionRadius", 0.0),
        "gameplayRadius":   root.get("overrideGameplayCollisionRadius", 65.0),
        "basicAtkMissileSpeed": missile_speed,
        "basicAtkWindup": windup,
        "basicAtkWindupModify": basicAtkWindupModify,
        "tags": list(tags)
    }

    if 'tft' in unit['name'] or 'strawberry' in unit['name']:
        continue

    units[unit["name"]] = unit

    # Read spells
    for key, val in props.items():
        if "mSpell" not in val:
            continue

        s = val["mSpell"]
        mScriptName = val["mScriptName"]

        if s or mScriptName:
            icon_name = os.path.basename(s.get("mImgIconName", [""])[0]).replace(".dds", "")
            target_type = s.get("mTargetingTypeData")
            if target_type is not None:
                target_type = target_type.get('__type')
            spell = {
                "name":               os.path.basename(key),
                "nameAlt":            mScriptName,
                "icon":               icon_name,
                "flags":              s.get("mAffectsTypeFlags", 0),
                "delay":              s.get("mCastTime", 0.5 + 0.5 * s.get("delayCastOffsetPercent", 0.0)),
                "castRange":          s.get("castRangeDisplayOverride", s.get("castRange", [s.get("castConeDistance", 0.0)]))[0],
                "castRadius":         s.get("castRadiusSecondary", s.get("castRadius", [0.0]))[0],
                "width":              s.get("mLineWidth", 0.0),
                "height":             0.0,
                "speed":              s.get("missileSpeed", 0.0),
                "travelTime":         0.0,
                "type":               target_type if target_type is not None else 'Unknown',
                "projectDestination": False,
                "skillShotType":      0,
            }

            if 'attack' in spell['name'].lower() or 'attack' in spell['nameAlt'].lower():
                continue;

            if spell['type'] == 'Location' or spell['type'] == 'LocationClamped' or spell['type'] == 'Direction':
                spell['skillShotType'] = 3
            elif spell['type'] == 'Area' or spell['type'] == 'AreaClamped' or spell['type'] == 'SelfAoe':
                spell['skillShotType'] = 1
            elif spell['type'] == 'Cone':
                spell['skillShotType'] = 2

            if 'mCastRangeGrowthMax' in s:
                spell['castRange'] = s['mCastRangeGrowthMax'][4]

            missile = s.get("mMissileSpec", None)
            if missile:
                movcomp = missile.get("movementComponent", None)
                if movcomp:
                    if spell["speed"] == 0:
                        spell["speed"] = movcomp.get("mSpeed", 0.0)
                    spell["height"] = movcomp.get("mOffsetInitialTargetHeight", 100.0)
                    spell["projectDestination"] = movcomp.get("mProjectTargetToCastRange", False)
                    spell["travelTime"] = movcomp.get("mTravelTime", 0.0)

            spells[spell["name"]] = spell

print(f'Found {len(units)} units and {len(spells)} spells')
with open("unit_data.json", 'w') as f:
    f.write(json.dumps(list(units.values()), indent=4))

with open("spell_data.json", 'w') as f:
    f.write(json.dumps(list(spells.values()), indent=4))