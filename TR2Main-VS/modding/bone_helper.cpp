#include "precompiled.h"
#include "bone_helper.h"

void BONE_AddRotateFlags(short objectID, int boneIndex, BoneRotateType flags)
{
	OBJECT_INFO* obj = &Objects[objectID];
	AnimBones[obj->boneIndex + boneIndex * 4] |= static_cast<short>(flags);
}

void BONE_AddRotateFlags(OBJECT_INFO* obj, int boneIndex, BoneRotateType flags)
{
	if (obj == nullptr) return;
	AnimBones[obj->boneIndex + boneIndex * 4] |= static_cast<short>(flags);
}

void BONE_RemoveRotateFlags(short objectID, int boneIndex, BoneRotateType flags)
{
	OBJECT_INFO* obj = &Objects[objectID];
	AnimBones[obj->boneIndex + boneIndex * 4] &= ~static_cast<short>(flags);
}

void BONE_RemoveRotateFlags(OBJECT_INFO* obj, int boneIndex, BoneRotateType flags)
{
	if (obj == nullptr) return;
	AnimBones[obj->boneIndex + boneIndex * 4] &= ~static_cast<short>(flags);
}
