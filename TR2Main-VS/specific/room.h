#pragma once
#include "types.h"

extern void InsertRoomGT3(FACE3* ptrObj, int number, SORTTYPE sortType);
extern void InsertRoomGT4(FACE4* ptrObj, int number, SORTTYPE sortType);
extern void InsertRoomGT4_ZBuffered(FACE4* ptrObj, int number, SORTTYPE sortType);
extern void InsertRoomGT3_ZBuffered(FACE3* ptrObj, int number, SORTTYPE sortType);
extern void InsertRoomGT4_Sorted(FACE4* ptrObj, int number, SORTTYPE sortType);
extern void InsertRoomGT3_Sorted(FACE3* ptrObj, int number, SORTTYPE sortType);
