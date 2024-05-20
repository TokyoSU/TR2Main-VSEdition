/*
 * Copyright (c) 2017-2024 Michael Chaban. All rights reserved.
 * Original game is created by Core Design Ltd. in 1997.
 * Lara Croft and Tomb Raider are trademarks of Embracer Group AB.
 *
 * This file is part of TR2Main.
 *
 * TR2Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TR2Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TR2Main.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "precompiled.h"
#include "modding/json_utils.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#define MOD_CONFIG_NAME "TR2Main.json"

ModConfig Mod;
void ModConfig::Initialize() {
    ZeroMemory(&Mod, sizeof(Mod));
}

void ModConfig::Release() {
    if (semitrans.animtex != NULL) {
        free(semitrans.animtex);
        semitrans.animtex = NULL;
    }
    FreePolyfilterNodes(&semitrans.rooms);
    FreePolyfilterNodes(&semitrans.statics);
    FreePolyfilterNodes(&reflect.statics);
    for (DWORD i = 0; i < ARRAY_SIZE(semitrans.objects); ++i) {
        FreePolyfilterNodes(&semitrans.objects[i]);
        FreePolyfilterNodes(&reflect.objects[i]);
    }
    ZeroMemory(&Mod, sizeof(Mod));
}

bool ModConfig::LoadJson(LPCSTR filePath) {
    if (!PathFileExists(filePath)) {
        LogWarn("Failed to load json: %s, does not exist !", filePath);
        return false;
    }

    std::ifstream file(MOD_CONFIG_NAME);
    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    StringStream ss(json.c_str());
    CursorStreamWrapper<StringStream> csw(ss);
    Document doc;
    doc.ParseStream(csw);

    if (doc.HasParseError()) {
        size_t lineID = csw.GetLine();
        if (lineID > 0)
            lineID--;
        ParseJsonError(MOD_CONFIG_NAME, lineID, doc.GetParseError());
        return false;
    }

    char levelName[256] = { 0 };
    strncpy(levelName, PathFindFileName(filePath), sizeof(levelName) - 1);
    char* ext = PathFindExtension(levelName);
    if (ext != NULL) *ext = 0;

    LogDebug("Level configuration loaded !");
    LogDebug("LevelFilename: %s", levelName);

    for (size_t i = 0; i < strlen(levelName); i++)
        levelName[i] = std::toupper(levelName[i]);

    if (doc.HasMember("default"))
        ParseDefaultConfiguration(doc["default"]);
    if (doc.HasMember("levels"))
        ParseLevelConfiguration(doc["levels"], levelName);
    return isLoaded;
}

void ModConfig::LoadHealthBarConfig(Value& data, BAR_CONFIG* result) {
    result->isCentered = GetValueByNameBool(data, "isCentered", false);
    result->basedOnEnemyHealth = GetValueByNameBool(data, "basedOnEnemyHealth", true);
    result->PC_xpos = GetValueByNameInt<int>(data, "PC_x", 8);
    result->PC_ypos = GetValueByNameInt<int>(data, "PC_y", 8);
    result->PC_color[0] = (INV_COLOURS)GetValueByNameInt<int>(data, "PC_color0", 3);
    result->PC_color[1] = (INV_COLOURS)GetValueByNameInt<int>(data, "PC_color1", 4);
    result->PSX_xpos = GetValueByNameInt<int>(data, "PSX_x", 20);
    result->PSX_ypos = GetValueByNameInt<int>(data, "PSX_y", 18);
    result->CENTER_xpos = GetValueByNameInt<int>(data, "CENTER_x", 0);
    result->CENTER_ypos = GetValueByNameInt<int>(data, "CENTER_y", 0);
    result->PSX_leftcolor[0] = GetColorByName(data, "PSX_leftcolor0", RGB_MAKE(0x68, 0, 0));
    result->PSX_leftcolor[1] = GetColorByName(data, "PSX_leftcolor1", RGB_MAKE(0x70, 0, 0));
    result->PSX_leftcolor[2] = GetColorByName(data, "PSX_leftcolor2", RGB_MAKE(0x98, 0, 0));
    result->PSX_leftcolor[3] = GetColorByName(data, "PSX_leftcolor3", RGB_MAKE(0xD8, 0, 0));
    result->PSX_leftcolor[4] = GetColorByName(data, "PSX_leftcolor4", RGB_MAKE(0xE4, 0, 0));
    result->PSX_leftcolor[5] = GetColorByName(data, "PSX_leftcolor5", RGB_MAKE(0xF0, 0, 0));
    result->PSX_rightcolor[0] = GetColorByName(data, "PSX_rightcolor0", RGB_MAKE(0, 0x44, 0));
    result->PSX_rightcolor[1] = GetColorByName(data, "PSX_rightcolor1", RGB_MAKE(0, 0x74, 0));
    result->PSX_rightcolor[2] = GetColorByName(data, "PSX_rightcolor2", RGB_MAKE(0, 0x9C, 0));
    result->PSX_rightcolor[3] = GetColorByName(data, "PSX_rightcolor3", RGB_MAKE(0, 0xD4, 0));
    result->PSX_rightcolor[4] = GetColorByName(data, "PSX_rightcolor4", RGB_MAKE(0, 0xE8, 0));
    result->PSX_rightcolor[5] = GetColorByName(data, "PSX_rightcolor5", RGB_MAKE(0, 0xFC, 0));
    result->PSX_framecolor[0] = GetColorByName(data, "PSX_framecolor0", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[1] = GetColorByName(data, "PSX_framecolor1", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[2] = GetColorByName(data, "PSX_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
    result->PSX_framecolor[3] = GetColorByName(data, "PSX_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
    result->PSX_framecolor[4] = GetColorByName(data, "PSX_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
    result->PSX_framecolor[5] = GetColorByName(data, "PSX_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
}

void ModConfig::LoadAirBarConfig(Value& data, BAR_CONFIG* result) {
    result->isCentered = GetValueByNameBool(data, "is_centered", false);
    result->basedOnEnemyHealth = false;
    result->PC_xpos = GetValueByNameInt<int>(data, "PC_x", 8);
    result->PC_ypos = GetValueByNameInt<int>(data, "PC_y", 8);
    result->PC_color[0] = (INV_COLOURS)GetValueByNameInt<int>(data, "PC_color0", 3);
    result->PC_color[1] = (INV_COLOURS)GetValueByNameInt<int>(data, "PC_color1", 4);
    result->PSX_xpos = GetValueByNameInt<int>(data, "PSX_x", 20);
    result->PSX_ypos = GetValueByNameInt<int>(data, "PSX_y", 18);
    result->CENTER_xpos = GetValueByNameInt<int>(data, "CENTER_x", 0);
    result->CENTER_ypos = GetValueByNameInt<int>(data, "CENTER_y", 0);
    result->PSX_leftcolor[0] = GetColorByName(data, "PSX_leftcolor0", RGB_MAKE(0, 0x40, 0x54));
    result->PSX_leftcolor[1] = GetColorByName(data, "PSX_leftcolor1", RGB_MAKE(0, 0x50, 0x64));
    result->PSX_leftcolor[2] = GetColorByName(data, "PSX_leftcolor2", RGB_MAKE(0, 0x68, 0x74));
    result->PSX_leftcolor[3] = GetColorByName(data, "PSX_leftcolor3", RGB_MAKE(0, 0x78, 0x84));
    result->PSX_leftcolor[4] = GetColorByName(data, "PSX_leftcolor4", RGB_MAKE(0, 0x84, 0x8E));
    result->PSX_leftcolor[5] = GetColorByName(data, "PSX_leftcolor5", RGB_MAKE(0, 0x90, 0x98));
    result->PSX_rightcolor[0] = GetColorByName(data, "PSX_rightcolor0", RGB_MAKE(0, 0x40, 0));
    result->PSX_rightcolor[1] = GetColorByName(data, "PSX_rightcolor1", RGB_MAKE(0, 0x50, 0));
    result->PSX_rightcolor[2] = GetColorByName(data, "PSX_rightcolor2", RGB_MAKE(0, 0x68, 0));
    result->PSX_rightcolor[3] = GetColorByName(data, "PSX_rightcolor3", RGB_MAKE(0, 0x78, 0));
    result->PSX_rightcolor[4] = GetColorByName(data, "PSX_rightcolor4", RGB_MAKE(0, 0x84, 0));
    result->PSX_rightcolor[5] = GetColorByName(data, "PSX_rightcolor5", RGB_MAKE(0, 0x90, 0));
    result->PSX_framecolor[0] = GetColorByName(data, "PSX_framecolor0", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[1] = GetColorByName(data, "PSX_framecolor1", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[2] = GetColorByName(data, "PSX_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
    result->PSX_framecolor[3] = GetColorByName(data, "PSX_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
    result->PSX_framecolor[4] = GetColorByName(data, "PSX_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
    result->PSX_framecolor[5] = GetColorByName(data, "PSX_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
}

void ModConfig::LoadLevelConfig(Value& data) {
    SizeType pictureStrSize = 0;
    const char* pictureName = GetValueByNameString(data, "picture", &pictureStrSize, NULL);
    if (pictureStrSize > 0)
    {
        snprintf(loadingPix, sizeof(loadingPix), "data\\%.*s.pcx", pictureStrSize, pictureName);
        loadingPixFound = true;
    }

    waterColor = GetColorByName(data, "watercolor", RGB_MAKE(255, 255, 255));
    isBarefoot = GetValueByNameBool(data, "barefoot", false);

    // Reset the enemyHealth structure to 1 hp to avoid them dying when triggered !
    memset(&enemyHealth, 1, sizeof(enemyHealth));
    enemyHealth.dog = GetValueByNameInt<short>(data, "dogHealth", 10);
    enemyHealth.mouse = GetValueByNameInt<short>(data, "mouseHealth", 4);
    enemyHealth.cult1 = GetValueByNameInt<short>(data, "cult1Health", 25);
    enemyHealth.cult1A = GetValueByNameInt<short>(data, "cult1AHealth", 25);
    enemyHealth.cult1B = GetValueByNameInt<short>(data, "cult1BHealth", 25);
    enemyHealth.cult2 = GetValueByNameInt<short>(data, "cult2Health", 60);
    enemyHealth.shark = GetValueByNameInt<short>(data, "sharkHealth", 30);
    enemyHealth.tiger = GetValueByNameInt<short>(data, "tigerHealth", 20);
    enemyHealth.barracuda = GetValueByNameInt<short>(data, "barracudaHealth", 12);
    enemyHealth.smallSpider = GetValueByNameInt<short>(data, "smallSpiderHealth", 5);
    enemyHealth.wolf = GetValueByNameInt<short>(data, "wolfHealth", 10);
    enemyHealth.bigSpider = GetValueByNameInt<short>(data, "bigSpiderHealth", 40);
    enemyHealth.bear = GetValueByNameInt<short>(data, "bearHealth", 30);
    enemyHealth.yeti = GetValueByNameInt<short>(data, "yetiHealth", 30);
    enemyHealth.jelly = GetValueByNameInt<short>(data, "jellyHealth", 10);
    enemyHealth.diver = GetValueByNameInt<short>(data, "diverHealth", 20);
    enemyHealth.worker1 = GetValueByNameInt<short>(data, "worker1Health", 25);
    enemyHealth.worker2 = GetValueByNameInt<short>(data, "worker2Health", 20);
    enemyHealth.worker3 = GetValueByNameInt<short>(data, "worker3Health", 27);
    enemyHealth.worker4 = GetValueByNameInt<short>(data, "worker4Health", 27);
    enemyHealth.worker5 = GetValueByNameInt<short>(data, "worker5Health", 20);
    enemyHealth.cult3 = GetValueByNameInt<short>(data, "cult3Health", 150);
    enemyHealth.monk1 = GetValueByNameInt<short>(data, "monk1Health", 30);
    enemyHealth.monk2 = GetValueByNameInt<short>(data, "monk2Health", 30);
    enemyHealth.eagle = GetValueByNameInt<short>(data, "eagleHealth", 20);
    enemyHealth.crow = GetValueByNameInt<short>(data, "crowHealth", 15);
    enemyHealth.bigEel = GetValueByNameInt<short>(data, "bigEelHealth", 20);
    enemyHealth.eel = GetValueByNameInt<short>(data, "eelHealth", 5);
    enemyHealth.bandit1 = GetValueByNameInt<short>(data, "bandit1Health", 45);
    enemyHealth.bandit2 = GetValueByNameInt<short>(data, "bandit2Health", 50);
    enemyHealth.bandit2B = GetValueByNameInt<short>(data, "bandit2BHealth", 50);
    enemyHealth.skidman = GetValueByNameInt<short>(data, "skidmanHealth", 100);
    enemyHealth.xianLord = GetValueByNameInt<short>(data, "xianLordHealth", 100);
    enemyHealth.warrior = GetValueByNameInt<short>(data, "warriorHealth", 80);
    enemyHealth.dragon = GetValueByNameInt<short>(data, "dragonHealth", 300);
    enemyHealth.giantYeti = GetValueByNameInt<short>(data, "giantYetiHealth", 200);
    enemyHealth.dino = GetValueByNameInt<short>(data, "dinoHealth", 100);

    disableGiantYetiNextLevelOnDeath = GetValueByNameBool(data, "disableGiantYetiNextLevelOnDeath", false);
    laraIgnoreMonkIfNotAngry = GetValueByNameBool(data, "laraignoremonkifnotangry", true);
    makeMercenaryAttackLaraFirst = GetValueByNameBool(data, "mercenaryattacklaradirectly", false);
    makeMonkAttackLaraFirst = GetValueByNameBool(data, "monksattacklaradirectly", false);
    enemyBarEnabled = GetValueByNameBool(data, "enableenemybar", true);
    makeYetiExplodeOnDeath = GetValueByNameBool(data, "makeyetiexplodeondeath", false);
}

void ModConfig::LoadSemitransConfig(Value& data, SEMITRANS_CONFIG* semitrans) {
    if (data.HasMember("animtex"))
    {
        LPCSTR animTexStr = data["animtex"].GetString();
        if (strcasecmp(animTexStr, "auto"))
        {
            semitrans->animtex = (POLYINDEX*)malloc(sizeof(POLYINDEX) * POLYFILTER_SIZE);
            if (semitrans->animtex)
                ParsePolyValue(animTexStr, semitrans->animtex, POLYFILTER_SIZE);
        }
    }
    if (data.HasMember("objects"))
    {
        Value& objectList = data["objects"].GetArray();
        for (SizeType i = 0; i < objectList.Size(); i++)
        {
            Value& obj = objectList[i];
            int objID = GetValueByNameInt(obj, "object", -1);
            if (objID < 0 || objID >= ID_NUMBER_OBJECTS)
                continue;
            if (obj.HasMember("objects"))
                LoadPolyfilterConfig(obj["meshes"], "mesh", &semitrans->objects[objID]);
        }
    }
    if (data.HasMember("statics"))
        LoadPolyfilterConfig(data["statics"], "static", &semitrans->statics);
    if (data.HasMember("rooms"))
        LoadPolyfilterConfig(data["rooms"], "room", &semitrans->rooms);
    semitrans->isLoaded = true;
}

void ModConfig::LoadPolyfilterConfig(Value& data, LPCSTR name, POLYFILTER_NODE** filterNodes)
{
    FreePolyfilterNodes(filterNodes);

    Value& polyList = data.GetArray();
    if (!polyList.IsArray())
    {
        LogWarn("Failed to load polyfilter node for type name: %s, type is not an array !", name);
        return;
    }

    for (SizeType i = 0; i < polyList.Size(); i++)
    {
        Value& poly = polyList[i];

        int meshID = GetValueByNameInt(poly, name, -1);
        if (meshID < 0) continue;

        POLYFILTER* meshFilter = CreatePolyfilterNode(filterNodes, meshID);
        if (meshFilter == NULL) continue;

        if (poly.HasMember("filter"))
        {
            Value& filterData = poly["filter"];
            meshFilter->n_vtx = GetValueByNameInt(filterData, "v", 0);
            meshFilter->n_gt4 = GetValueByNameInt(filterData, "t4", 0);
            meshFilter->n_gt3 = GetValueByNameInt(filterData, "t3", 0);
            meshFilter->n_g4 = GetValueByNameInt(filterData, "c4", 0);
            meshFilter->n_g3 = GetValueByNameInt(filterData, "c3", 0);
        }

        SizeType size = 0;
        LPCSTR t4list = GetValueByNameString(poly, "t4list", &size, NULL);
        LPCSTR t3list = GetValueByNameString(poly, "t3list", &size, NULL);
        LPCSTR c4list = GetValueByNameString(poly, "c4list", &size, NULL);
        LPCSTR c3list = GetValueByNameString(poly, "c3list", &size, NULL);
        if (t4list || t3list || c4list || c3list)
        {
            ParsePolyValue(t4list, meshFilter->gt4, ARRAY_SIZE(meshFilter->gt4));
            ParsePolyValue(t3list, meshFilter->gt3, ARRAY_SIZE(meshFilter->gt3));
            ParsePolyValue(c4list, meshFilter->g4, ARRAY_SIZE(meshFilter->g4));
            ParsePolyValue(c3list, meshFilter->g3, ARRAY_SIZE(meshFilter->g3));
        }
    }
}

void ModConfig::LoadReflectConfig(Value& data, REFLECT_CONFIG* reflect) {
    if (data.HasMember("objects") && data["objects"].IsArray())
    {
        Value& objectList = data["objects"].GetArray();
        for (SizeType i = 0; i < objectList.Size(); i++)
        {
            Value& obj = objectList[i];
            int objID = GetValueByNameInt(obj, "object", -1);
            if (objID < 0 || objID >= ID_NUMBER_OBJECTS)
                continue;
            LoadPolyfilterConfig(obj["meshes"], "mesh", &reflect->objects[objID]);
        }
    }
    if (data.HasMember("statics"))
        LoadPolyfilterConfig(data["statics"], "static", &reflect->statics);
    reflect->isLoaded = true;
}

void ModConfig::ParseDefaultConfiguration(Value& data) {
    pistolAtStart = GetValueByNameBool(data, "pistolsatstart", true);
    shotgunAtStart = GetValueByNameBool(data, "shotgunatstart", true);
    uzisAtStart = GetValueByNameBool(data, "uzisatstart", false);
    autoPistolAtStart = GetValueByNameBool(data, "autopistolsatstart", false);
    m16AtStart = GetValueByNameBool(data, "m16atstart", false);
    grenadeAtStart = GetValueByNameBool(data, "grenadeatstart", false);
    harpoonAtStart = GetValueByNameBool(data, "harpoonatstart", false);

    shotgunAmmoAtStart = GetValueByNameInt<short>(data, "shotgunammoatstart", 2);
    uzisAmmoAtStart = GetValueByNameInt<short>(data, "uzisammoatstart", 0);
    autoPistolAmmoAtStart = GetValueByNameInt<short>(data, "autopistolsammoatstart", 0);
    m16AmmoAtStart = GetValueByNameInt<short>(data, "m16ammoatstart", 0);
    grenadeAmmoAtStart = GetValueByNameInt<short>(data, "grenadeammoatstart", 0);
    harpoonAmmoAtStart = GetValueByNameInt<short>(data, "harpoonammoatstart", 0);

    flareAtStart = GetValueByNameInt<short>(data, "flaresatstart", 2);
    smallMediAtStart = GetValueByNameInt<short>(data, "smallmedikitatstart", 1);
    bigMediAtStart = GetValueByNameInt<short>(data, "bigmedikitatstart", 1);

    LoadHealthBarConfig(data["larahealthbar"], &laraBar.health);
    laraBar.health.basedOnEnemyHealth = false;
    LoadAirBarConfig(data["laraairbar"], &laraBar.air);
    LoadHealthBarConfig(data["enemyhealthbar"], &enemyBar);

    LoadSemitransConfig(data["semitransparent"], &semitrans);
    LoadReflectConfig(data["reflective"], &reflect);
}

void ModConfig::ParseLevelConfiguration(Value& data, LPCSTR currentLevel) {
    if (!data.IsArray()) {
        LogWarn("Failed to load level configuration (json), 'levels' is not an array !");
        return;
    }

    Value& levelData = data.GetArray();
    SizeType levelCount = levelData.Size();
    if (levelCount <= 0) {
        LogWarn("Failed to load level configuration (json), no level in the array !");
        return;
    }

    for (SizeType i = 0; i < levelCount; i++)
    {
        Value& level = levelData[i];
        // Filename member required for detecting current level !
        if (level.HasMember("filename")) {
            // If the filename is equal then load it !
            if (strcasecmp(level["filename"].GetString(), currentLevel) == 0) {
                LoadLevelConfig(level);
                if (level.HasMember("semitransparent")) LoadSemitransConfig(level["semitransparent"], &semitrans);
                if (level.HasMember("reflective")) LoadReflectConfig(level["reflective"], &reflect);
                break;
            }
        }
        else
        {
            LogWarn("Failed to load level configuration (json), current level: %s not have a 'filename' entry !", currentLevel);
            break;
        }
    }
}

POLYFILTER* ModConfig::CreatePolyfilterNode(POLYFILTER_NODE** data, int id)
{
    if (data == NULL) return NULL;
    POLYFILTER_NODE* node = (POLYFILTER_NODE*)malloc(sizeof(POLYFILTER_NODE));
    if (node == NULL) return NULL;
    node->id = id;
    node->next = *data;
    memset(&node->filter, 0, sizeof(node->filter));
    *data = node;
    return &node->filter;
}

void ModConfig::FreePolyfilterNodes(POLYFILTER_NODE** data)
{
    if (data == NULL) return;
    POLYFILTER_NODE* node = *data;
    while (node) {
        POLYFILTER_NODE* next = node->next;
        free(node);
        node = next;
    }
    *data = NULL;
}

bool ModConfig::IsCompatibleFilterObjects(short* ptrObj, POLYFILTER* filter)
{
    if (!ptrObj || !filter || !filter->n_vtx) return true;
    ptrObj += 5; // skip x, y, z, radius, flags
    short num = *(ptrObj++); // get vertex counter
    if (num != filter->n_vtx) return false;
    ptrObj += num * 3; // skip vertices
    num = *(ptrObj++); // get normal counter
    ptrObj += (num > 0) ? num * 3 : ABS(num); // skip normals/shades
    num = *(ptrObj++); // get gt4 number
    if (num != filter->n_gt4) return false;
    ptrObj += num * 5; // skip gt4 polys
    num = *(ptrObj++); // get gt3 number
    if (num != filter->n_gt3) return false;
    ptrObj += num * 4; // skip gt3 polys
    num = *(ptrObj++); // get g4 number
    if (num != filter->n_g4) return false;
    ptrObj += num * 5; // skip g4 polys
    num = *(ptrObj++); // get g3 number
    if (num != filter->n_g3) return false;
    return true;
}

short* ModConfig::EnumeratePolysSpecificObjects(short* ptrObj, int vtxCount, bool colored, ENUM_POLYS_OBJECTS_CB callback, POLYINDEX* filter, LPVOID param)
{
    int polyNumber = *ptrObj++;
    if (filter == NULL || (!filter[0].idx && !filter[0].num)) {
        for (int i = 0; i < polyNumber; ++i) {
            if (!callback(ptrObj, vtxCount, colored, param)) return NULL;
            ptrObj += vtxCount + 1;
        }
    }
    else {
        int polyIndex = 0;
        for (int i = 0; i < POLYFILTER_SIZE; i++) {
            if (filter[i].idx < polyIndex || filter[i].idx >= polyNumber) {
                break;
            }
            int skip = filter[i].idx - polyIndex;
            if (skip > 0) {
                ptrObj += skip * (vtxCount + 1);
                polyIndex += skip;
            }
            int number = MIN(filter[i].num, polyNumber - polyIndex);
            for (int j = 0; j < number; ++j) {
                if (!callback(ptrObj, vtxCount, colored, param)) return NULL;
                ptrObj += vtxCount + 1;
            }
            polyIndex += number;
        }
        ptrObj += (polyNumber - polyIndex) * (vtxCount + 1);
    }
    return ptrObj;
}

void ModConfig::EnumeratePolysSpecificRoomFace4(FACE4* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE4_CB callback, POLYINDEX* filter, LPVOID param)
{
    if (filter == NULL || (!filter[0].idx && !filter[0].num)) {
        for (int i = 0; i < faceCount; ++i) {
            if (!callback(&ptrObj[i], 4, colored, param))
                return;
        }
    }
    else
    {
        for (int i = 0; i < POLYFILTER_SIZE; ++i)
        {
            if (!callback(&ptrObj[filter[i].idx], filter[i].num - 1, colored, param))
                LogWarn("Failed to setup the room face4 index: %d, at number: %d", filter[i].idx, filter[i].num - 1);
        }
    }
}

void ModConfig::EnumeratePolysSpecificRoomFace3(FACE3* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE3_CB callback, POLYINDEX* filter, LPVOID param)
{
    if (filter == NULL || (!filter[0].idx && !filter[0].num)) {
        for (int i = 0; i < faceCount; ++i) {
            if (!callback(&ptrObj[i], 3, colored, param))
                return;
        }
    }
    else
    {
        for (int i = 0; i < POLYFILTER_SIZE; ++i)
        {
            if (!callback(&ptrObj[filter[i].idx], filter[i].num - 1, colored, param))
                LogWarn("Failed to setup the room face3 index: %d, at number: %d", filter[i].idx, filter[i].num - 1);
        }
    }
}

bool ModConfig::EnumeratePolysObjects(short* ptrObj, ENUM_POLYS_OBJECTS_CB callback, POLYFILTER* filter, LPVOID param)
{
    if (ptrObj == NULL || callback == NULL) return false; // wrong parameters
    if (!IsCompatibleFilterObjects(ptrObj, filter)) return false; // filter is not compatible

    ptrObj += 5; // skip x, y, z, radius, flags
    short num = *(ptrObj++); // get vertex counter
    ptrObj += num * 3; // skip vertices
    num = *(ptrObj++); // get normal counter
    ptrObj += (num > 0) ? num * 3 : ABS(num); // skip normals/shades
    ptrObj = EnumeratePolysSpecificObjects(ptrObj, 4, false, callback, filter ? filter->gt4 : NULL, param); // enumerate textured quads
    if (ptrObj == NULL) return true;
    ptrObj = EnumeratePolysSpecificObjects(ptrObj, 3, false, callback, filter ? filter->gt3 : NULL, param); // enumerate textured triangles
    if (ptrObj == NULL) return true;
    ptrObj = EnumeratePolysSpecificObjects(ptrObj, 4, true, callback, filter ? filter->g4 : NULL, param); // enumerate colored quads
    if (ptrObj == NULL) return true;
    ptrObj = EnumeratePolysSpecificObjects(ptrObj, 3, true, callback, filter ? filter->g3 : NULL, param); // enumerate colored triangles
    return true;
}

bool ModConfig::EnumeratePolysRoomFace3(FACE3* ptrObj, int faceCount, ENUM_POLYS_FACE3_CB callback, POLYFILTER* filter, LPVOID param)
{
    if (ptrObj == NULL) return false; // wrong parameters
    if (faceCount != filter->n_gt3) return false;
    EnumeratePolysSpecificRoomFace3(ptrObj, faceCount, false, callback, filter ? filter->gt3 : NULL, param); // enumerate textured triangles
    return false;
}

bool ModConfig::EnumeratePolysRoomFace4(FACE4* ptrObj, int faceCount, ENUM_POLYS_FACE4_CB callback, POLYFILTER* filter, LPVOID param)
{
    if (ptrObj == NULL) return false; // wrong parameters
    if (faceCount != filter->n_gt4) return false;
    EnumeratePolysSpecificRoomFace4(ptrObj, faceCount, false, callback, filter ? filter->gt4 : NULL, param); // enumerate textured triangles
    return false;
}

int ModConfig::ParsePolyString(LPCSTR str, POLYINDEX* lst, DWORD lstLen) {
    if (!lst || !lstLen) {
        return -1;
    }

    lst[0].idx = ~0;
    lst[0].num = ~0;

    POLYINDEX* lstBuf = (POLYINDEX*)malloc(lstLen * sizeof(POLYINDEX));
    if (lstBuf == NULL) {
        return -2;
    }

    char* strBuf = _strdup(str);
    if (strBuf == NULL) {
        free(lstBuf);
        return -2;
    }

    DWORD bufLen = 0;
    char* token = strtok(strBuf, ",");
    while (token != NULL) {
        char* range = strchr(token, '-');
        if (range) {
            int from = atoi(token);
            int to = atoi(range + 1);
            lstBuf[bufLen].idx = MIN(to, from);
            lstBuf[bufLen].num = ABS(to - from) + 1;
        }
        else {
            lstBuf[bufLen].idx = atoi(token);
            lstBuf[bufLen].num = 1;
        }
        if (++bufLen >= lstLen) {
            break;
        }
        token = strtok(NULL, ",");
    }

    free(strBuf);
    if (!bufLen) {
        free(lstBuf);
        return 0;
    }

    for (DWORD i = 0; i < bufLen - 1; ++i) {
        for (DWORD j = i + 1; j < bufLen; ++j) {
            if (lstBuf[i].idx > lstBuf[j].idx) {
                POLYINDEX t;
                SWAP(lstBuf[i], lstBuf[j], t);
            }
        }
    }

    lst[0] = lstBuf[0];
    DWORD resLen = 1;

    for (DWORD i = 1; i < bufLen; ++i) {
        int bound = lst[resLen - 1].idx + lst[resLen - 1].num;
        if (lstBuf[i].idx > bound) {
            lst[resLen] = lstBuf[i];
            ++resLen;
        }
        else {
            int ext = lstBuf[i].idx + lstBuf[i].num;
            if (ext > bound) {
                lst[resLen - 1].num += ext - bound;
            }
        }
    }
    if (resLen < lstLen) {
        lst[resLen].idx = 0;
        lst[resLen].num = 0;
    }

    free(lstBuf);
    return resLen;
}

int ModConfig::ParsePolyValue(LPCSTR value, POLYINDEX* lst, DWORD lstLen) {
    if (!lst || !lstLen) {
        return -1;
    }

    lst[0].idx = ~0;
    lst[0].num = ~0;
    if (value == NULL) {
        return 0;
    }

    const char* str = value;
    if (!str || !*str || !strcasecmp(str, "none")) {
        return 0;
    }
    if (!strcasecmp(str, "all")) {
        lst[0].idx = 0;
        lst[0].num = 0;
        return 1;
    }
    return ParsePolyString(str, lst, lstLen);
}
#endif
