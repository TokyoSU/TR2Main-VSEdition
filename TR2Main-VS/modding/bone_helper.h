#pragma once
#include "global/types.h"
#include "global/vars.h"

enum class BoneRotateType
{
	None = 0x00,
	X = 0x04,
	Y = 0x08,
	Z = 0x10,
	XY = X | Y,
	XZ = X | Z,
	YZ = Y | Z,
	XYZ = X | Y | Z
};

extern void BONE_AddRotateFlags(short objectID, int boneIndex, BoneRotateType flags);
extern void BONE_AddRotateFlags(OBJECT_INFO* obj, int boneIndex, BoneRotateType flags);
extern void BONE_RemoveRotateFlags(short objectID, int boneIndex, BoneRotateType flags);
extern void BONE_RemoveRotateFlags(OBJECT_INFO* obj, int boneIndex, BoneRotateType flags);
