#pragma once
#include "global/types.h"

extern bool IsSecret(int objNumber);
extern BOOL AddSecret(int secretType);
extern int GetSecretType(ITEM_INFO* item);
extern int GetSecretMesh(int objNumber, int secretType);
