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
#include "game/invfunc.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#define MOD_CONFIG_NAME "TR2Main.json"

ModConfig Mod;
void ModConfig::Initialize() {
    ZeroMemory(&Mod, sizeof(Mod));
}

void ModConfig::Release() {
    if (Mod.semitrans.animtex != NULL) {
        free(Mod.semitrans.animtex);
        Mod.semitrans.animtex = NULL;
    }
    FreePolyfilterNodes(&Mod.semitrans.rooms);
    FreePolyfilterNodes(&Mod.semitrans.statics);
    FreePolyfilterNodes(&Mod.reflect.statics);
    for (DWORD i = 0; i < ARRAY_SIZE(Mod.semitrans.objects); ++i) {
        FreePolyfilterNodes(&Mod.semitrans.objects[i]);
        FreePolyfilterNodes(&Mod.reflect.objects[i]);
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
    //LogDebug("LevelFilename: %s", levelName);

    for (size_t i = 0; i < strlen(levelName); i++)
        levelName[i] = std::toupper(levelName[i]);

    if (doc.HasMember("default"))
        ParseDefaultConfiguration(doc["default"]);
    if (doc.HasMember("levels"))
        ParseLevelConfiguration(doc["levels"], levelName);
    return Mod.isLoaded;
}

bool GetCustomItemFromObjectID(int objectID, CUST_INVENTORY_ITEM& invItem)
{
    for (int i = 0; i < ARRAY_SIZE(Mod.invItemList); i++)
    {
        auto& foundItem = Mod.invItemList[i];
        if (foundItem.objectID != -1 && foundItem.objectID == objectID)
        {
            invItem = foundItem;
            return true;
        }
    }
    return false;
}

void LoadHealthBarConfig(Value& data, BAR_CONFIG* result) {
    result->isCentered = GetValueByNameBool(data, "is_centered", false);
    result->basedOnEnemyHealth = GetValueByNameBool(data, "based_on_enemy_health", true);
    result->PC_xpos = GetValueByNameInt(data, "pc_x", 8);
    result->PC_ypos = GetValueByNameInt(data, "pc_y", 8);
    result->PC_color[0] = (INV_COLOURS)GetValueByNameInt(data, "pc_color0", 3);
    result->PC_color[1] = (INV_COLOURS)GetValueByNameInt(data, "pc_color1", 4);
    result->PSX_xpos = GetValueByNameInt(data, "psx_x", 20);
    result->PSX_ypos = GetValueByNameInt(data, "psx_y", 18);
    result->CENTER_xpos = GetValueByNameInt(data, "center_x", 0);
    result->CENTER_ypos = GetValueByNameInt(data, "center_y", 0);
    result->PSX_leftcolor[0] = GetColorRGBByName(data, "psx_leftcolor0", RGB_MAKE(0x68, 0, 0));
    result->PSX_leftcolor[1] = GetColorRGBByName(data, "psx_leftcolor1", RGB_MAKE(0x70, 0, 0));
    result->PSX_leftcolor[2] = GetColorRGBByName(data, "psx_leftcolor2", RGB_MAKE(0x98, 0, 0));
    result->PSX_leftcolor[3] = GetColorRGBByName(data, "psx_leftcolor3", RGB_MAKE(0xD8, 0, 0));
    result->PSX_leftcolor[4] = GetColorRGBByName(data, "psx_leftcolor4", RGB_MAKE(0xE4, 0, 0));
    result->PSX_leftcolor[5] = GetColorRGBByName(data, "psx_leftcolor5", RGB_MAKE(0xF0, 0, 0));
    result->PSX_rightcolor[0] = GetColorRGBByName(data, "psx_rightcolor0", RGB_MAKE(0, 0x44, 0));
    result->PSX_rightcolor[1] = GetColorRGBByName(data, "psx_rightcolor1", RGB_MAKE(0, 0x74, 0));
    result->PSX_rightcolor[2] = GetColorRGBByName(data, "psx_rightcolor2", RGB_MAKE(0, 0x9C, 0));
    result->PSX_rightcolor[3] = GetColorRGBByName(data, "psx_rightcolor3", RGB_MAKE(0, 0xD4, 0));
    result->PSX_rightcolor[4] = GetColorRGBByName(data, "psx_rightcolor4", RGB_MAKE(0, 0xE8, 0));
    result->PSX_rightcolor[5] = GetColorRGBByName(data, "psx_rightcolor5", RGB_MAKE(0, 0xFC, 0));
    result->PSX_framecolor[0] = GetColorRGBByName(data, "psx_framecolor0", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[1] = GetColorRGBByName(data, "psx_framecolor1", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[2] = GetColorRGBByName(data, "psx_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
    result->PSX_framecolor[3] = GetColorRGBByName(data, "psx_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
    result->PSX_framecolor[4] = GetColorRGBByName(data, "psx_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
    result->PSX_framecolor[5] = GetColorRGBByName(data, "psx_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
}

void LoadAirBarConfig(Value& data, BAR_CONFIG* result) {
    result->isCentered = GetValueByNameBool(data, "is_centered", false);
    result->basedOnEnemyHealth = false;
    result->PC_xpos = GetValueByNameInt(data, "pc_x", 8);
    result->PC_ypos = GetValueByNameInt(data, "pc_y", 8);
    result->PC_color[0] = (INV_COLOURS)GetValueByNameInt(data, "pc_color0", 3);
    result->PC_color[1] = (INV_COLOURS)GetValueByNameInt(data, "pc_color1", 4);
    result->PSX_xpos = GetValueByNameInt(data, "psx_x", 20);
    result->PSX_ypos = GetValueByNameInt(data, "psx_y", 18);
    result->CENTER_xpos = GetValueByNameInt(data, "center_x", 0);
    result->CENTER_ypos = GetValueByNameInt(data, "center_y", 0);
    result->PSX_leftcolor[0] = GetColorRGBByName(data, "psx_leftcolor0", RGB_MAKE(0, 0x40, 0x54));
    result->PSX_leftcolor[1] = GetColorRGBByName(data, "psx_leftcolor1", RGB_MAKE(0, 0x50, 0x64));
    result->PSX_leftcolor[2] = GetColorRGBByName(data, "psx_leftcolor2", RGB_MAKE(0, 0x68, 0x74));
    result->PSX_leftcolor[3] = GetColorRGBByName(data, "psx_leftcolor3", RGB_MAKE(0, 0x78, 0x84));
    result->PSX_leftcolor[4] = GetColorRGBByName(data, "psx_leftcolor4", RGB_MAKE(0, 0x84, 0x8E));
    result->PSX_leftcolor[5] = GetColorRGBByName(data, "psx_leftcolor5", RGB_MAKE(0, 0x90, 0x98));
    result->PSX_rightcolor[0] = GetColorRGBByName(data, "psx_rightcolor0", RGB_MAKE(0, 0x40, 0));
    result->PSX_rightcolor[1] = GetColorRGBByName(data, "psx_rightcolor1", RGB_MAKE(0, 0x50, 0));
    result->PSX_rightcolor[2] = GetColorRGBByName(data, "psx_rightcolor2", RGB_MAKE(0, 0x68, 0));
    result->PSX_rightcolor[3] = GetColorRGBByName(data, "psx_rightcolor3", RGB_MAKE(0, 0x78, 0));
    result->PSX_rightcolor[4] = GetColorRGBByName(data, "psx_rightcolor4", RGB_MAKE(0, 0x84, 0));
    result->PSX_rightcolor[5] = GetColorRGBByName(data, "psx_rightcolor5", RGB_MAKE(0, 0x90, 0));
    result->PSX_framecolor[0] = GetColorRGBByName(data, "psx_framecolor0", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[1] = GetColorRGBByName(data, "psx_framecolor1", RGB_MAKE(0, 0, 0));
    result->PSX_framecolor[2] = GetColorRGBByName(data, "psx_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
    result->PSX_framecolor[3] = GetColorRGBByName(data, "psx_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
    result->PSX_framecolor[4] = GetColorRGBByName(data, "psx_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
    result->PSX_framecolor[5] = GetColorRGBByName(data, "psx_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
}

void LoadCustomInventoryItems(Value& data)
{
    Value& inventoryItemList = data.GetArray();
    if (!inventoryItemList.IsArray())
    {
        LogWarn("Failed to load inventory_item_list, type is not an array !");
        return;
    }

    if (inventoryItemList.Size() <= 0) {
        LogWarn("Failed to load inventory_item_list, there no value in the array !");
        return;
    } else if (inventoryItemList.Size() > MAX_ITEM_IN_INVENTORY) {
        LogWarn("Failed to load inventory_item_list, array size out of bounds, you can't have more than 23 objects in inventory !");
        return;
    }

    for (SizeType i = 0; i < inventoryItemList.Size(); i++)
    {
        SizeType destStringSize = 0;
        auto& invItem = inventoryItemList[i];
        auto& newItemCust = Mod.invItemList[i];
        int objectID = GetValueByNameInt(invItem, "object_id", -1);
        if (objectID < 0)
        {
            LogWarn("Failed to load inventory_item_list, index number: %d have object_id not set or is negative !", i);
            continue;
        }
        newItemCust.objectID = objectID;
        newItemCust.message = GetValueByNameString(invItem, "text", &destStringSize, "");
        LogDebug("Message: %s", newItemCust.message);
        newItemCust.message_pos_x = GetValueByNameInt(invItem, "text_x", 0);
        newItemCust.message_pos_y = GetValueByNameInt(invItem, "text_y", 0);
        if (invItem.HasMember("position"))
        {
            auto& offset = invItem["position"];
            newItemCust.xRotPtSel = GetValueByNameInt(offset, "xRotPtSel", 0);
            newItemCust.xRotSel = GetValueByNameInt(offset, "xRotSel", 0);
            newItemCust.yRotSel = GetValueByNameInt(offset, "yRotSel", 0);
            newItemCust.zTransSel = GetValueByNameInt(offset, "zTransSel", 0);
            newItemCust.yTransSel = GetValueByNameInt(offset, "yTransSel", 0);
        }
        newItemCust.canExamine = GetValueByNameBool(invItem, "canExamine", false);
        newItemCust.canRotateManually = GetValueByNameBool(invItem, "canRotateManually", false);
    }

    static bool wasItemInitialized = false;
    if (!wasItemInitialized)
    {
        CUST_INVENTORY_ITEM invItem;
        for (int index = 0; index < ARRAY_SIZE(Mod.invItemList); index++)
        {
            auto* item = Inv_GetItemFromIndex(index);
            if (item == nullptr)
                continue;
            if (GetCustomItemFromObjectID((int)Inv_GetItemOption((GAME_OBJECT_ID)item->objectID), invItem))
            {
                item->xRotSel = invItem.xRotSel;
                item->xRotPtSel = invItem.xRotPtSel;
                item->yRotSel = invItem.yRotSel;
                item->yTransSel = invItem.yTransSel;
                item->zTransSel = invItem.zTransSel;
            }
        }
        wasItemInitialized = true;
    }
}

void LoadLevelConfig(Value& data) {
    SizeType size = 0;

    Mod.levelLoadingPix = GetValueByNameString(data, "picture", &size, "");
    Mod.waterColor = GetColorRGBByName(data, "watercolor", RGB_MAKE(255, 255, 255));
    Mod.isBarefoot = GetValueByNameBool(data, "barefoot", false);

    // Reset the enemyHealth structure to 1 hp to avoid them dying when triggered !
    memset(&Mod.enemyHealth, 1, sizeof(Mod.enemyHealth));
    Mod.enemyHealth.dog = GetValueByNameShort(data, "dog_health", 10);
    Mod.enemyHealth.mouse = GetValueByNameShort(data, "mouse_health", 4);
    Mod.enemyHealth.cult1 = GetValueByNameShort(data, "cult1_health", 25);
    Mod.enemyHealth.cult1A = GetValueByNameShort(data, "cult1a_health", 25);
    Mod.enemyHealth.cult1B = GetValueByNameShort(data, "cult1b_health", 25);
    Mod.enemyHealth.cult2 = GetValueByNameShort(data, "cult2_health", 60);
    Mod.enemyHealth.shark = GetValueByNameShort(data, "shark_health", 30);
    Mod.enemyHealth.tiger = GetValueByNameShort(data, "tiger_health", 20);
    Mod.enemyHealth.barracuda = GetValueByNameShort(data, "barracuda_health", 12);
    Mod.enemyHealth.smallSpider = GetValueByNameShort(data, "small_spider_health", 5);
    Mod.enemyHealth.wolf = GetValueByNameShort(data, "wolf_health", 10);
    Mod.enemyHealth.bigSpider = GetValueByNameShort(data, "big_spider_health", 40);
    Mod.enemyHealth.bear = GetValueByNameShort(data, "bear_health", 30);
    Mod.enemyHealth.yeti = GetValueByNameShort(data, "yeti_health", 30);
    Mod.enemyHealth.jelly = GetValueByNameShort(data, "jelly_health", 10);
    Mod.enemyHealth.diver = GetValueByNameShort(data, "diver_health", 20);
    Mod.enemyHealth.worker1 = GetValueByNameShort(data, "worker1_health", 25);
    Mod.enemyHealth.worker2 = GetValueByNameShort(data, "worker2_health", 20);
    Mod.enemyHealth.worker3 = GetValueByNameShort(data, "worker3_health", 27);
    Mod.enemyHealth.worker4 = GetValueByNameShort(data, "worker4_health", 27);
    Mod.enemyHealth.worker5 = GetValueByNameShort(data, "worker5_health", 20);
    Mod.enemyHealth.cult3 = GetValueByNameShort(data, "cult3_health", 150);
    Mod.enemyHealth.monk1 = GetValueByNameShort(data, "monk1_health", 30);
    Mod.enemyHealth.monk2 = GetValueByNameShort(data, "monk2_health", 30);
    Mod.enemyHealth.eagle = GetValueByNameShort(data, "eagle_health", 20);
    Mod.enemyHealth.crow = GetValueByNameShort(data, "crow_health", 15);
    Mod.enemyHealth.bigEel = GetValueByNameShort(data, "big_eel_health", 20);
    Mod.enemyHealth.eel = GetValueByNameShort(data, "eel_health", 5);
    Mod.enemyHealth.bandit1 = GetValueByNameShort(data, "bandit1_health", 45);
    Mod.enemyHealth.bandit2 = GetValueByNameShort(data, "bandit2_health", 50);
    Mod.enemyHealth.bandit2B = GetValueByNameShort(data, "bandit2b_health", 50);
    Mod.enemyHealth.skidman = GetValueByNameShort(data, "skidman_health", 100);
    Mod.enemyHealth.xianLord = GetValueByNameShort(data, "xian_lord_health", 100);
    Mod.enemyHealth.warrior = GetValueByNameShort(data, "warrior_health", 80);
    Mod.enemyHealth.dragon = GetValueByNameShort(data, "dragon_health", 300);
    Mod.enemyHealth.giantYeti = GetValueByNameShort(data, "giant_yeti_health", 200);
    Mod.enemyHealth.dino = GetValueByNameShort(data, "dino_health", 100);

    Mod.underwaterInfo.maxAir = GetValueByNameShort(data, "max_lara_air", LARA_AIR_MAX);
    Mod.underwaterInfo.noAirDamagePerTick = GetValueByNameShort(data, "no_air_damage_per_tick", LARA_NO_AIR_DAMAGE_PER_TICK);
    Mod.underwaterInfo.restoreAirPerTick = GetValueByNameShort(data, "restore_air_per_tick", LARA_RESTORE_AIR_PER_TICK);
    Mod.underwaterInfo.unlimitedAir = GetValueByNameBool(data, "unlimited_air", false);

    Mod.disableGiantYetiNextLevelOnDeath = GetValueByNameBool(data, "disable_giant_yeti_next_level_on_death", false);
    Mod.laraIgnoreMonkIfNotAngry = GetValueByNameBool(data, "lara_ignore_monk_if_not_angry", true);
    Mod.makeMercenaryAttackLaraFirst = GetValueByNameBool(data, "mercenary_attack_lara_directly", false);
    Mod.makeMonkAttackLaraFirst = GetValueByNameBool(data, "monks_attack_lara_directly", false);
    Mod.enemyBarEnabled = GetValueByNameBool(data, "enable_enemy_bar", true);
    Mod.makeYetiExplodeOnDeath = GetValueByNameBool(data, "make_yeti_explode_on_death", false);

    Mod.isDartEffectOpaque = GetValueByNameBool(data, "is_dart_effect_opaque", false);
    Mod.isFlameOpaque = GetValueByNameBool(data, "is_flame_opaque", false);
    Mod.isLavaFountainOpaque = GetValueByNameBool(data, "is_lava_fountain_opaque", false);
    Mod.isDragonFlameOpaque = GetValueByNameBool(data, "is_dragon_flame_opaque", false);
    Mod.isSphereOfDoom1Opaque = GetValueByNameBool(data, "is_sphere_of_doom1_opaque", false);
    Mod.isSphereOfDoom2Opaque = GetValueByNameBool(data, "is_sphere_of_doom2_opaque", false);
    Mod.isBloodOpaque = GetValueByNameBool(data, "is_blood_opaque", false);
    Mod.isExplosionOpaque = GetValueByNameBool(data, "is_explosion_opaque", false);
    Mod.isTwinkleOpaque = GetValueByNameBool(data, "is_twinkle_opaque", false);
    Mod.isSplashOpaque = GetValueByNameBool(data, "is_splash_opaque", false);
    Mod.isWaterSpriteOpaque = GetValueByNameBool(data, "is_water_sprite_opaque", false);
    Mod.isHotLiquidOpaque = GetValueByNameBool(data, "is_hot_liquid_opaque", false);
}

void LoadSemitransConfig(Value& data, SEMITRANS_CONFIG* semitrans) {
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

void LoadPolyfilterConfig(Value& data, LPCSTR name, POLYFILTER_NODE** filterNodes)
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
        std::string t4list = GetValueByNameString(poly, "t4list", &size, "");
        std::string t3list = GetValueByNameString(poly, "t3list", &size, "");
        std::string c4list = GetValueByNameString(poly, "c4list", &size, "");
        std::string c3list = GetValueByNameString(poly, "c3list", &size, "");
        if (t4list.size() > 0 || t3list.size() > 0 || c4list.size() > 0 || c3list.size() > 0)
        {
            ParsePolyValue(t4list, meshFilter->gt4, ARRAY_SIZE(meshFilter->gt4));
            ParsePolyValue(t3list, meshFilter->gt3, ARRAY_SIZE(meshFilter->gt3));
            ParsePolyValue(c4list, meshFilter->g4, ARRAY_SIZE(meshFilter->g4));
            ParsePolyValue(c3list, meshFilter->g3, ARRAY_SIZE(meshFilter->g3));
        }
    }
}

void LoadReflectConfig(Value& data, REFLECT_CONFIG* reflect) {
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

/// <param name="type">0 Bgnd, 1 Main, 2 Seq</param>
static void LoadGouraudFill(Value& data, int type, GOURAUD_FILL* fill)
{
    switch (type)
    {
    case 0:
        fill->clr[0][0] = GetColorRGBByName(data, "0", RGB_MAKE(0, 32, 0));
        fill->clr[0][1] = GetColorRGBByName(data, "1", RGB_MAKE(0, 32, 0));
        fill->clr[0][2] = GetColorRGBByName(data, "2", RGB_MAKE(0, 96, 0));
        fill->clr[0][3] = GetColorRGBByName(data, "3", RGB_MAKE(0, 32, 0));
        fill->clr[1][0] = GetColorRGBByName(data, "4", RGB_MAKE(0, 32, 0));
        fill->clr[1][1] = GetColorRGBByName(data, "5", RGB_MAKE(0, 32, 0));
        fill->clr[1][2] = GetColorRGBByName(data, "6", RGB_MAKE(0, 32, 0));
        fill->clr[1][3] = GetColorRGBByName(data, "7", RGB_MAKE(0, 96, 0));
        fill->clr[2][0] = GetColorRGBByName(data, "8", RGB_MAKE(0, 96, 0));
        fill->clr[2][1] = GetColorRGBByName(data, "9", RGB_MAKE(0, 32, 0));
        fill->clr[2][2] = GetColorRGBByName(data, "10", RGB_MAKE(0, 32, 0));
        fill->clr[2][3] = GetColorRGBByName(data, "11", RGB_MAKE(0, 32, 0));
        fill->clr[3][0] = GetColorRGBByName(data, "12", RGB_MAKE(0, 32, 0));
        fill->clr[3][1] = GetColorRGBByName(data, "13", RGB_MAKE(0, 96, 0));
        fill->clr[3][2] = GetColorRGBByName(data, "14", RGB_MAKE(0, 32, 0));
        fill->clr[3][3] = GetColorRGBByName(data, "15", RGB_MAKE(0, 32, 0));
        break;
    case 1:
        fill->clr[0][0] = GetColorRGBByName(data, "0", RGB_MAKE(0, 0, 0));
        fill->clr[0][1] = GetColorRGBByName(data, "1", RGB_MAKE(0, 0, 0));
        fill->clr[0][2] = GetColorRGBByName(data, "2", RGB_MAKE(16, 128, 56));
        fill->clr[0][3] = GetColorRGBByName(data, "3", RGB_MAKE(0, 0, 0));
        fill->clr[1][0] = GetColorRGBByName(data, "4", RGB_MAKE(0, 0, 0));
        fill->clr[1][1] = GetColorRGBByName(data, "5", RGB_MAKE(0, 0, 0));
        fill->clr[1][2] = GetColorRGBByName(data, "6", RGB_MAKE(0, 0, 0));
        fill->clr[1][3] = GetColorRGBByName(data, "7", RGB_MAKE(16, 128, 56));
        fill->clr[2][0] = GetColorRGBByName(data, "8", RGB_MAKE(16, 128, 56));
        fill->clr[2][1] = GetColorRGBByName(data, "9", RGB_MAKE(0, 0, 0));
        fill->clr[2][2] = GetColorRGBByName(data, "10", RGB_MAKE(0, 0, 0));
        fill->clr[2][3] = GetColorRGBByName(data, "11", RGB_MAKE(0, 0, 0));
        fill->clr[3][0] = GetColorRGBByName(data, "12", RGB_MAKE(0, 0, 0));
        fill->clr[3][1] = GetColorRGBByName(data, "13", RGB_MAKE(16, 128, 56));
        fill->clr[3][2] = GetColorRGBByName(data, "14", RGB_MAKE(0, 0, 0));
        fill->clr[3][3] = GetColorRGBByName(data, "15", RGB_MAKE(0, 0, 0));
        break;
    case 2:
        fill->clr[0][0] = GetColorRGBByName(data, "0", RGB_MAKE(0, 0, 0));
        fill->clr[0][1] = GetColorRGBByName(data, "1", RGB_MAKE(0, 0, 0));
        fill->clr[0][2] = GetColorRGBByName(data, "2", RGB_MAKE(56, 240, 128));
        fill->clr[0][3] = GetColorRGBByName(data, "3", RGB_MAKE(0, 0, 0));
        fill->clr[1][0] = GetColorRGBByName(data, "4", RGB_MAKE(0, 0, 0));
        fill->clr[1][1] = GetColorRGBByName(data, "5", RGB_MAKE(0, 0, 0));
        fill->clr[1][2] = GetColorRGBByName(data, "6", RGB_MAKE(0, 0, 0));
        fill->clr[1][3] = GetColorRGBByName(data, "7", RGB_MAKE(56, 240, 128));
        fill->clr[2][0] = GetColorRGBByName(data, "8", RGB_MAKE(56, 240, 128));
        fill->clr[2][1] = GetColorRGBByName(data, "9", RGB_MAKE(0, 0, 0));
        fill->clr[2][2] = GetColorRGBByName(data, "10", RGB_MAKE(0, 0, 0));
        fill->clr[2][3] = GetColorRGBByName(data, "11", RGB_MAKE(0, 0, 0));
        fill->clr[3][0] = GetColorRGBByName(data, "12", RGB_MAKE(0, 0, 0));
        fill->clr[3][1] = GetColorRGBByName(data, "13", RGB_MAKE(56, 240, 128));
        fill->clr[3][2] = GetColorRGBByName(data, "14", RGB_MAKE(0, 0, 0));
        fill->clr[3][3] = GetColorRGBByName(data, "15", RGB_MAKE(0, 0, 0));
        break;
    }
}

/// <param name="type">0 Bgnd, 1 Main, 2 Seq</param>
static void LoadGouraudFillDefault(int type, GOURAUD_FILL* fill)
{
    switch (type)
    {
    case 0:
        fill->clr[0][0] = RGBA_MAKE(0, 32, 0, 128); // 0
        fill->clr[0][1] = RGBA_MAKE(0, 32, 0, 128); // 1
        fill->clr[0][2] = RGBA_MAKE(0, 96, 0, 128); // 2
        fill->clr[0][3] = RGBA_MAKE(0, 32, 0, 128); // 3
        fill->clr[1][0] = RGBA_MAKE(0, 32, 0, 128); // 4
        fill->clr[1][1] = RGBA_MAKE(0, 32, 0, 128); // 5
        fill->clr[1][2] = RGBA_MAKE(0, 32, 0, 128); // 6
        fill->clr[1][3] = RGBA_MAKE(0, 96, 0, 128); // 7
        fill->clr[2][0] = RGBA_MAKE(0, 96, 0, 128); // 8
        fill->clr[2][1] = RGBA_MAKE(0, 32, 0, 128); // 9
        fill->clr[2][2] = RGBA_MAKE(0, 32, 0, 128); // 10
        fill->clr[2][3] = RGBA_MAKE(0, 32, 0, 128); // 11
        fill->clr[3][0] = RGBA_MAKE(0, 32, 0, 128); // 12
        fill->clr[3][1] = RGBA_MAKE(0, 96, 0, 128); // 13
        fill->clr[3][2] = RGBA_MAKE(0, 32, 0, 128); // 14
        fill->clr[3][3] = RGBA_MAKE(0, 32, 0, 128); // 15
        break;
    case 1:
        fill->clr[0][0] = RGBA_MAKE(0, 0, 0, 128); // 0
        fill->clr[0][1] = RGBA_MAKE(0, 0, 0, 128); // 1
        fill->clr[0][2] = RGBA_MAKE(16, 128, 56, 128); // 2
        fill->clr[0][3] = RGBA_MAKE(0, 0, 0, 128); // 3
        fill->clr[1][0] = RGBA_MAKE(0, 0, 0, 128); // 4
        fill->clr[1][1] = RGBA_MAKE(0, 0, 0, 128); // 5
        fill->clr[1][2] = RGBA_MAKE(0, 0, 0, 128); // 6
        fill->clr[1][3] = RGBA_MAKE(16, 128, 56, 128); // 7
        fill->clr[2][0] = RGBA_MAKE(16, 128, 56, 128); // 8
        fill->clr[2][1] = RGBA_MAKE(0, 0, 0, 128); // 9
        fill->clr[2][2] = RGBA_MAKE(0, 0, 0, 128); // 10
        fill->clr[2][3] = RGBA_MAKE(0, 0, 0, 128); // 11
        fill->clr[3][0] = RGBA_MAKE(0, 0, 0, 128); // 12
        fill->clr[3][1] = RGBA_MAKE(16, 128, 56, 128); // 13
        fill->clr[3][2] = RGBA_MAKE(0, 0, 0, 128); // 14
        fill->clr[3][3] = RGBA_MAKE(0, 0, 0, 128); // 15
        break;
    case 2:
        fill->clr[0][0] = RGBA_MAKE(0, 0, 0, 128); // 0
        fill->clr[0][1] = RGBA_MAKE(0, 0, 0, 128); // 1
        fill->clr[0][2] = RGBA_MAKE(56, 240, 128, 128); // 2
        fill->clr[0][3] = RGBA_MAKE(0, 0, 0, 128); // 3
        fill->clr[1][0] = RGBA_MAKE(0, 0, 0, 128); // 4
        fill->clr[1][1] = RGBA_MAKE(0, 0, 0, 128); // 5
        fill->clr[1][2] = RGBA_MAKE(0, 0, 0, 128); // 6
        fill->clr[1][3] = RGBA_MAKE(56, 240, 128, 128); // 7
        fill->clr[2][0] = RGBA_MAKE(56, 240, 128, 128); // 8
        fill->clr[2][1] = RGBA_MAKE(0, 0, 0, 128); // 9
        fill->clr[2][2] = RGBA_MAKE(0, 0, 0, 128); // 10
        fill->clr[2][3] = RGBA_MAKE(0, 0, 0, 128); // 11
        fill->clr[3][0] = RGBA_MAKE(0, 0, 0, 128); // 12
        fill->clr[3][1] = RGBA_MAKE(56, 240, 128, 128); // 13
        fill->clr[3][2] = RGBA_MAKE(0, 0, 0, 128); // 14
        fill->clr[3][3] = RGBA_MAKE(0, 0, 0, 128); // 15
        break;
    }
}

/// <param name="type">0 Bgnd, 1 Main, 2 Seq</param>
static void LoadGouraudOutlineDefault(int type, GOURAUD_OUTLINE* outline)
{
    switch (type)
    {
    case 0:
        outline->clr[0] = RGBA_MAKE(96, 96, 96, 128);
        outline->clr[1] = RGBA_MAKE(128, 128, 128, 128);
        outline->clr[2] = RGBA_MAKE(32, 32, 32, 128);
        outline->clr[3] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[4] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[5] = RGBA_MAKE(32, 32, 32, 128);
        outline->clr[6] = RGBA_MAKE(64, 64, 64, 128);
        outline->clr[7] = RGBA_MAKE(64, 64, 64, 128);
        outline->clr[8] = RGBA_MAKE(96, 96, 96, 128);
        break;
    case 1:
        for (int i = 0; i < 9; i++)
            outline->clr[i] = RGBA_MAKE(0, 0, 0, 128);
        break;
    case 2:
        outline->clr[0] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[1] = RGBA_MAKE(255, 255, 255, 128);
        outline->clr[2] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[3] = RGBA_MAKE(56, 240, 128, 128);
        outline->clr[4] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[5] = RGBA_MAKE(255, 255, 255, 128);
        outline->clr[6] = RGBA_MAKE(0, 0, 0, 128);
        outline->clr[7] = RGBA_MAKE(56, 240, 128, 128);
        outline->clr[8] = RGBA_MAKE(0, 0, 0, 128);
        break;
    }
}

/// <param name="type">0 Bgnd, 1 Main, 2 Seq</param>
static void LoadGouraudOutline(Value& data, int type, GOURAUD_OUTLINE* outline)
{
    switch (type)
    {
    case 0:
        outline->clr[0] = GetColorRGBByName(data, "0", RGBA_MAKE(96, 96, 96, 128));
        outline->clr[1] = GetColorRGBByName(data, "1", RGBA_MAKE(128, 128, 128, 128));
        outline->clr[2] = GetColorRGBByName(data, "2", RGBA_MAKE(32, 32, 32, 128));
        outline->clr[3] = GetColorRGBByName(data, "3", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[4] = GetColorRGBByName(data, "4", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[5] = GetColorRGBByName(data, "5", RGBA_MAKE(32, 32, 32, 128));
        outline->clr[6] = GetColorRGBByName(data, "6", RGBA_MAKE(64, 64, 64, 128));
        outline->clr[7] = GetColorRGBByName(data, "7", RGBA_MAKE(64, 64, 64, 128));
        outline->clr[8] = GetColorRGBByName(data, "8", RGBA_MAKE(96, 96, 96, 128));
        break;
    case 1:
        outline->clr[0] = GetColorRGBByName(data, "0", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[1] = GetColorRGBByName(data, "1", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[2] = GetColorRGBByName(data, "2", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[3] = GetColorRGBByName(data, "3", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[4] = GetColorRGBByName(data, "4", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[5] = GetColorRGBByName(data, "5", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[6] = GetColorRGBByName(data, "6", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[7] = GetColorRGBByName(data, "7", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[8] = GetColorRGBByName(data, "8", RGBA_MAKE(0, 0, 0, 128));
        break;
    case 2:
        outline->clr[0] = GetColorRGBByName(data, "0", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[1] = GetColorRGBByName(data, "1", RGBA_MAKE(255, 255, 255, 128));
        outline->clr[2] = GetColorRGBByName(data, "2", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[3] = GetColorRGBByName(data, "3", RGBA_MAKE(56, 240, 128, 128));
        outline->clr[4] = GetColorRGBByName(data, "4", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[5] = GetColorRGBByName(data, "5", RGBA_MAKE(255, 255, 255, 128));
        outline->clr[6] = GetColorRGBByName(data, "6", RGBA_MAKE(0, 0, 0, 128));
        outline->clr[7] = GetColorRGBByName(data, "7", RGBA_MAKE(56, 240, 128, 128));
        outline->clr[8] = GetColorRGBByName(data, "8", RGBA_MAKE(0, 0, 0, 128));
        break;
    }
}

static void LoadGouraudAlpha(Value& data, GOURAUD_FILL* fill, GOURAUD_OUTLINE* outline, unsigned char defaultValue = 128)
{
    unsigned char alpha = data.GetInt() & 255;
    for (int i = 0; i < 4; i++)
    {
        for (int y = 0; y < 4; y++)
        {
            fill->clr[i][y] = RGBA_SETALPHA(fill->clr[i][y], alpha);
        }
    }
    for (int i = 0; i < 9; i++)
    {
        outline->clr[i] = RGBA_SETALPHA(outline->clr[i], alpha);
    }
}

static void LoadGouraudAlphaDefault(GOURAUD_FILL* fill, GOURAUD_OUTLINE* outline, unsigned char defaultValue = 128)
{
    for (int i = 0; i < 4; i++)
    {
        for (int y = 0; y < 4; y++)
        {
            fill->clr[i][y] = RGBA_SETALPHA(fill->clr[i][y], defaultValue);
        }
    }
    for (int i = 0; i < 9; i++)
    {
        outline->clr[i] = RGBA_SETALPHA(outline->clr[i], defaultValue);
    }
}

extern GOURAUD_FILL ReqBgndGour1;
extern GOURAUD_OUTLINE ReqBgndGour2;
extern GOURAUD_FILL ReqMainGour1;
extern GOURAUD_OUTLINE ReqMainGour2;
extern GOURAUD_FILL ReqSelGour1;
extern GOURAUD_OUTLINE ReqSelGour2;

void LoadUIConfig(Value& data, LPCSTR name)
{
    LoadGouraudFill(data["req_bgnd"], 0, &ReqBgndGour1);
    LoadGouraudOutline(data["req_bgnd_outline"], 0, &ReqBgndGour2);
    LoadGouraudAlpha(data["req_bgnd_alpha"], &ReqBgndGour1, &ReqBgndGour2, 128);
    LoadGouraudFill(data["req_main"], 1, &ReqMainGour1);
    LoadGouraudOutline(data["req_main_outline"], 1, &ReqMainGour2);
    LoadGouraudAlpha(data["req_main_alpha"], &ReqMainGour1, &ReqMainGour2, 128);
    LoadGouraudFill(data["req_sel"], 2, &ReqSelGour1);
    LoadGouraudOutline(data["req_sel_outline"], 2, &ReqSelGour2);
    LoadGouraudAlpha(data["req_sel_alpha"], &ReqSelGour1, &ReqSelGour2, 128);
    Mod.isUIColorLoaded = true;
}

void LoadUIConfigDefault()
{
    LoadGouraudFillDefault(0, &ReqBgndGour1);
    LoadGouraudOutlineDefault(0, &ReqBgndGour2);
    LoadGouraudAlphaDefault(&ReqBgndGour1, &ReqBgndGour2, 128);
    LoadGouraudFillDefault(1, &ReqMainGour1);
    LoadGouraudOutlineDefault(1, &ReqMainGour2);
    LoadGouraudAlphaDefault(&ReqMainGour1, &ReqMainGour2, 128);
    LoadGouraudFillDefault(2, &ReqSelGour1);
    LoadGouraudOutlineDefault(2, &ReqSelGour2);
    LoadGouraudAlphaDefault(&ReqSelGour1, &ReqSelGour2, 128);
    Mod.isUIColorLoaded = false;
}

void ParseDefaultConfiguration(Value& data) {
    SizeType a, b, c;

    Mod.pistolAtStart = GetValueByNameBool(data, "pistols_at_start", true);
    Mod.shotgunAtStart = GetValueByNameBool(data, "shotgun_at_start", true);
    Mod.uzisAtStart = GetValueByNameBool(data, "uzis_at_start", false);
    Mod.autoPistolAtStart = GetValueByNameBool(data, "autopistols_at_start", false);
    Mod.m16AtStart = GetValueByNameBool(data, "m16_at_start", false);
    Mod.grenadeAtStart = GetValueByNameBool(data, "grenade_at_start", false);
    Mod.harpoonAtStart = GetValueByNameBool(data, "harpoon_at_start", false);

    Mod.shotgunAmmoAtStart = GetValueByNameShort(data, "shotgun_ammo_at_start", 2);
    Mod.uzisAmmoAtStart = GetValueByNameShort(data, "uzis_ammo_at_start", 0);
    Mod.autoPistolAmmoAtStart = GetValueByNameShort(data, "autopistols_ammo_at_start", 0);
    Mod.m16AmmoAtStart = GetValueByNameShort(data, "m16_ammo_at_start", 0);
    Mod.grenadeAmmoAtStart = GetValueByNameShort(data, "grenade_ammo_at_start", 0);
    Mod.harpoonAmmoAtStart = GetValueByNameShort(data, "harpoon_ammo_at_start", 0);

    Mod.flareAtStart = GetValueByNameShort(data, "flares_at_start", 2);
    Mod.smallMediAtStart = GetValueByNameShort(data, "smallmedikit_at_start", 1);
    Mod.bigMediAtStart = GetValueByNameShort(data, "bigmedikit_at_start", 1);

    Mod.titleLoadingPixLanguage = GetValueByNameString(data, "inv_loading_pix_language", &a, "data/title%s.pcx");
    Mod.titleLoadingPix = GetValueByNameString(data, "inv_loading_pix", &b, "data/title.pcx");
    Mod.titleLoadingPixGold = GetValueByNameString(data, "inv_loading_pix_gold", &c, "data/titleg.pcx");

    LoadHealthBarConfig(data["lara_health_bar"], &Mod.laraBar.health);
    Mod.laraBar.health.basedOnEnemyHealth = false;
    LoadAirBarConfig(data["lara_air_bar"], &Mod.laraBar.air);
    LoadHealthBarConfig(data["enemy_health_bar"], &Mod.enemyBar);

    LoadSemitransConfig(data["semi_transparent"], &Mod.semitrans);
    LoadReflectConfig(data["reflective"], &Mod.reflect);

    if (data.HasMember("ui_config")) LoadUIConfig(data["ui_config"], "ui_config");
    else if (!Mod.isUIColorLoaded) LoadUIConfigDefault();
}

void ParseLevelConfiguration(Value& data, LPCSTR currentLevel) {
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
                if (level.HasMember("semitransparent")) LoadSemitransConfig(level["semitransparent"], &Mod.semitrans);
                if (level.HasMember("reflective")) LoadReflectConfig(level["reflective"], &Mod.reflect);
                if (level.HasMember("inventory_item_list")) LoadCustomInventoryItems(level["inventory_item_list"]);
                if (level.HasMember("ui_config")) LoadUIConfig(level["ui_config"], "ui_config");
                else if (!Mod.isUIColorLoaded) LoadUIConfigDefault();
                break;
            }
            else
            {
                LogWarn("Failed to load level configuration, filename entry: %s not found, instead found: %s", currentLevel, level["filename"].GetString());
            }
        }
        else
        {
            LogWarn("Failed to load level configuration (json), current level: %s not have a 'filename' entry !", currentLevel);
            break;
        }
    }
}

POLYFILTER* CreatePolyfilterNode(POLYFILTER_NODE** data, int id)
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

void FreePolyfilterNodes(POLYFILTER_NODE** data)
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

bool IsCompatibleFilterObjects(short* ptrObj, POLYFILTER* filter)
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

short* EnumeratePolysSpecificObjects(short* ptrObj, int vtxCount, bool colored, ENUM_POLYS_OBJECTS_CB callback, POLYINDEX* filter, LPVOID param)
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

void EnumeratePolysSpecificRoomFace4(FACE4* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE4_CB callback, POLYINDEX* filter, LPVOID param)
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

void EnumeratePolysSpecificRoomFace3(FACE3* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE3_CB callback, POLYINDEX* filter, LPVOID param)
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

bool EnumeratePolysObjects(short* ptrObj, ENUM_POLYS_OBJECTS_CB callback, POLYFILTER* filter, LPVOID param)
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

bool EnumeratePolysRoomFace3(FACE3* ptrObj, int faceCount, ENUM_POLYS_FACE3_CB callback, POLYFILTER* filter, LPVOID param)
{
    if (ptrObj == NULL) return false; // wrong parameters
    if (faceCount != filter->n_gt3) return false;
    EnumeratePolysSpecificRoomFace3(ptrObj, faceCount, false, callback, filter ? filter->gt3 : NULL, param); // enumerate textured triangles
    return false;
}

bool EnumeratePolysRoomFace4(FACE4* ptrObj, int faceCount, ENUM_POLYS_FACE4_CB callback, POLYFILTER* filter, LPVOID param)
{
    if (ptrObj == NULL) return false; // wrong parameters
    if (faceCount != filter->n_gt4) return false;
    EnumeratePolysSpecificRoomFace4(ptrObj, faceCount, false, callback, filter ? filter->gt4 : NULL, param); // enumerate textured triangles
    return false;
}

int ParsePolyString(LPCSTR str, POLYINDEX* lst, DWORD lstLen) {
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

int ParsePolyValue(std::string value, POLYINDEX* lst, DWORD lstLen) {
    if (!lst || !lstLen) {
        return -1;
    }

    lst[0].idx = ~0;
    lst[0].num = ~0;
    if (value.empty()) {
        return 0;
    }

    const char* str = value.c_str();
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
