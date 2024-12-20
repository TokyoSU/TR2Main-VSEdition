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
#include "3dsystem/3d_gen.h"
#include "3dsystem/3d_out.h"
#include "3dsystem/3dinsert.h"
#include "3dsystem/phd_math.h"
#include "3dsystem/scalespr.h"
#include "specific/hwr.h"
#include "specific/room.h"
#include "global/vars.h"

PHD_VECTOR CamPos;

 // related to POLYTYPE enum
static void(__cdecl* PolyDrawRoutines[])(short*) = {
	draw_poly_gtmap,		// gouraud shaded poly (texture)
	draw_poly_wgtmap,		// gouraud shaded poly (texture + colorkey)
	draw_poly_gtmap_persp,	// gouraud shaded poly (texture + perspective)
	draw_poly_wgtmap_persp,	// gouraud shaded poly (texture + colorkey + perspective)
	draw_poly_line,			// line (color)
	draw_poly_flat,			// flat shaded poly (color)
	draw_poly_gouraud,		// gouraud shaded poly (color)
	draw_poly_trans,		// shadow poly (color + semitransparent)
	draw_scaled_spriteC		// scaled sprite (texture + colorkey)
};

#if defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_VIEW_IMPROVED)
SORT_ITEM SortBuffer[16000];
short Info3dBuffer[480000];
#endif // defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_VIEW_IMPROVED)

#ifdef FEATURE_EXTENDED_LIMITS
PHD_SPRITE PhdSpriteInfo[2048];
D3DTLVERTEX HWR_VertexBuffer[32768];
#endif // FEATURE_EXTENDED_LIMITS

int PhdFov;

#ifdef FEATURE_VIEW_IMPROVED
bool PsxFovEnabled;

// view distance
double ViewDistanceFactor = 6.0;

// regular fog
double FogBeginFactor = 1.0;
double FogEndFactor = 6.0;
int FogBeginDepth = DEPTHQ_START;
int FogEndDepth = DEPTHQ_END;

// underwater fog
double WaterFogBeginFactor = 0.6;
double WaterFogEndFactor = 1.0;
int WaterFogBeginDepth = DEPTHQ_START;
int WaterFogEndDepth = DEPTHQ_END;

// fog formula
int CalculateFogShade(int depth) {
	int fogBegin, fogEnd;

	if (IsWaterEffect) {
		fogBegin = WaterFogBeginDepth;
		fogEnd = WaterFogEndDepth;
	}
	else {
		fogBegin = FogBeginDepth;
		fogEnd = FogEndDepth;
	}

	if (depth < fogBegin)
		return 0;
	if (depth >= fogEnd)
		return 0x1FFF;

	return (depth - fogBegin) * 0x1FFF / (fogEnd - fogBegin);
}
#endif // FEATURE_VIEW_IMPROVED

#ifdef FEATURE_VIDEOFX_IMPROVED
#include "modding/mod_utils.h"

extern DWORD ReflectionMode;
static POLYFILTER ReflectFilter;
static D3DCOLOR ReflectTint = 0;
static bool IsReflect = false;

void ClearMeshReflectState() {
	memset(&ReflectFilter, 0, sizeof(ReflectFilter));
	ReflectTint = RGBA_MAKE(0xFF, 0xFF, 0xFF, 0x80);
	IsReflect = false;
}

void SetMeshReflectState(int objID, int meshIdx) {
	// Clear poly filters and disable reflection by default
	ClearMeshReflectState();
	if (TextureFormat.bpp < 16 || !ReflectionMode) return;

	if (objID == ID_NONE) {
		// Reflect all meshes with custom tint instead of mesh index
		ReflectTint = meshIdx;
		IsReflect = true;
		return;
	}

#ifdef FEATURE_MOD_CONFIG
	// Check if config is presented
	if (Mod.reflect.isLoaded) {
		POLYFILTER_NODE* node = NULL;
		if (meshIdx < 0) {
			for (node = Mod.reflect.statics; node != NULL; node = node->next) {
				if (node->id == objID) {
					ReflectFilter = node->filter;
					IsReflect = true;
					break;
				}
			}
		}
		else if (objID >= 0 && objID < ID_NUMBER_OBJECTS) {
			POLYFILTER_NODE** obj = Mod.reflect.objects;
			for (node = obj[objID]; node != NULL; node = node->next) {
				if (node->id == meshIdx) {
					ReflectFilter = node->filter;
					IsReflect = true;
					break;
				}
			}
		}
		return;
	}
#endif // FEATURE_MOD_CONFIG

	// If config is absent or disabled, use hardcoded params
	if (objID >= 0 && meshIdx < 0) {
		// This is static object mesh
		return;
	}

	// This is animated object mesh
	switch (objID) {
	case ID_SKIDOO_FAST:
		// This one is a fast showmobile from the Golden Mask
		// Reflect the windshield only (skidoo body is mesh #0)
		if (meshIdx == 0) {
			// Set filter conditions
			ReflectFilter.n_vtx = 59;
			ReflectFilter.n_gt4 = 14;
			ReflectFilter.n_gt3 = 73;
			ReflectFilter.n_g4 = 0;
			ReflectFilter.n_g3 = 17;
			// All colored triangles are reflective
			// The only reflective textured triangle is 48
			ReflectFilter.gt3[0].idx = 48;
			ReflectFilter.gt3[0].num = 1;
			// Quads are not reflective
			ReflectFilter.gt4[0].idx = ~0;
			ReflectFilter.g4[0].idx = ~0;
			IsReflect = true;
		}
		break;
	case ID_SKIDOO_ARMED:
		// This one is an armed showmobile
		// Reflect the windshield only (skidoo body is mesh #0)
		if (meshIdx == 0) {
			// Set filter conditions
			ReflectFilter.n_vtx = 88;
			ReflectFilter.n_gt4 = 45;
			ReflectFilter.n_gt3 = 60;
			ReflectFilter.n_g4 = 0;
			ReflectFilter.n_g3 = 0;
			// The reflective textured quads are 21..22, 34..47
			ReflectFilter.gt4[0].idx = 21;
			ReflectFilter.gt4[0].num = 2;
			ReflectFilter.gt3[0].idx = 34;
			ReflectFilter.gt3[0].num = 14;
			// Other polys are not reflective
			ReflectFilter.g4[0].idx = ~0;
			ReflectFilter.g3[0].idx = ~0;
			IsReflect = true;
		}
		break;
	case ID_WORKER5:
		// Reflect the black glass mask of flamethrower buddy (his head is mesh #15)
		if (meshIdx == 15) {
			// Set filter conditions
			ReflectFilter.n_vtx = 38;
			ReflectFilter.n_gt4 = 30;
			ReflectFilter.n_gt3 = 12;
			ReflectFilter.n_g4 = 0;
			ReflectFilter.n_g3 = 0;
			// The reflective textured quads are 22..26
			ReflectFilter.gt4[0].idx = 22;
			ReflectFilter.gt4[0].num = 5;
			// Other polys are not reflective
			ReflectFilter.gt3[0].idx = ~0;
			ReflectFilter.g4[0].idx = ~0;
			ReflectFilter.g3[0].idx = ~0;
			IsReflect = true;
		}
		break;
	case ID_SPINNING_BLADE:
		if (meshIdx == 0) {
			// Reflect only quads, not triangles
			ReflectFilter.gt3[0].idx = ~0;
			ReflectFilter.g3[0].idx = ~0;
			IsReflect = true;
		}
		break;
	case ID_BLADE:
		// Reflect the blade only (mesh #1)
		if (meshIdx == 1) {
			IsReflect = true;
		}
		break;
	case ID_KILLER_STATUE:
		// Reflect the sword only (mesh #7)
		if (meshIdx == 7) {
			IsReflect = true;
		}
		break;
	}
}

static bool InsertEnvmap(short* ptrObj, int vtxCount, bool colored, LPVOID param) {
	return InsertObjectEM(ptrObj, vtxCount, ReflectTint, (PHD_UV*)param);
}

static void phd_PutEnvmapPolygons(short* ptrEnv) {
	if (ptrEnv == NULL || !IsReflect || SavedAppSettings.RenderMode != RM_Hardware) return;
	short* ptrObj = ptrEnv;

	ptrObj += 5; // skip x, y, z, radius, flags
	short num = *(ptrObj++); // get vertex counter
	ptrObj += num * 3; // skip vertices

	int vtxCount = *ptrObj++;
	if (vtxCount <= 0) return;

	PHD_UV* uv = new PHD_UV[vtxCount];
	for (int i = 0; i < vtxCount; ++i) {
		// make sure that reflection will be drawn after normal poly
		PhdVBuf[i].zv -= (double)(W2V_SCALE / 2);
		// set lighting that depends only from fog distance
		PhdVBuf[i].g = 0x1000;
		int depth = PhdMatrixPtr->_23 >> W2V_SHIFT;
#ifdef FEATURE_VIEW_IMPROVED
		PhdVBuf[i].g += CalculateFogShade(depth);
#else // !FEATURE_VIEW_IMPROVED
		if (depth > DEPTHQ_START) // fog begin
			PhdVBuf[i].g += depth - DEPTHQ_START;
#endif // FEATURE_VIEW_IMPROVED
		CLAMP(PhdVBuf[i].g, 0x1000, 0x1FFF); // reflection can be darker but not brighter

		// rotate normal vectors for X/Y, no translation
		int x = (PhdMatrixPtr->_00 * ptrObj[0] +
			     PhdMatrixPtr->_01 * ptrObj[1] +
			     PhdMatrixPtr->_02 * ptrObj[2]) >> W2V_SHIFT;

		int y = (PhdMatrixPtr->_10 * ptrObj[0] +
			     PhdMatrixPtr->_11 * ptrObj[1] +
			     PhdMatrixPtr->_12 * ptrObj[2]) >> W2V_SHIFT;

		CLAMP(x, -PHD_IONE, PHD_IONE);
		CLAMP(y, -PHD_IONE, PHD_IONE);
		uv[i].u = PHD_ONE / PHD_IONE * (x + PHD_IONE) / 2;
		uv[i].v = PHD_ONE / PHD_IONE * (y + PHD_IONE) / 2;
		ptrObj += 3;
	}
	EnumeratePolysObjects(ptrEnv, InsertEnvmap, &ReflectFilter, (LPVOID)uv);
	delete[] uv;
}
#endif // FEATURE_VIDEOFX_IMPROVED

void phd_GenerateW2V(PHD_3DPOS* viewPos) {
	int sx = phd_sin(viewPos->rotX);
	int cx = phd_cos(viewPos->rotX);
	int sy = phd_sin(viewPos->rotY);
	int cy = phd_cos(viewPos->rotY);
	int sz = phd_sin(viewPos->rotZ);
	int cz = phd_cos(viewPos->rotZ);

	PhdMatrixPtr = &MatrixStack[0]; // set matrix stack pointer to W2V

	MatrixW2V._00 = PhdMatrixPtr->_00 = TRIGMULT3(sx, sy, sz) + TRIGMULT2(cy, cz);
	MatrixW2V._01 = PhdMatrixPtr->_01 = TRIGMULT2(cx, sz);
	MatrixW2V._02 = PhdMatrixPtr->_02 = TRIGMULT3(sx, cy, sz) - TRIGMULT2(sy, cz);

	MatrixW2V._10 = PhdMatrixPtr->_10 = (int)(FltViewAspect * (double)(TRIGMULT3(sx, sy, cz) - TRIGMULT2(cy, sz)));
	MatrixW2V._11 = PhdMatrixPtr->_11 = (int)(FltViewAspect * (double)(TRIGMULT2(cx, cz)));
	MatrixW2V._12 = PhdMatrixPtr->_12 = (int)(FltViewAspect * (double)(TRIGMULT3(sx, cy, cz) + TRIGMULT2(sy, sz)));

	MatrixW2V._20 = PhdMatrixPtr->_20 = TRIGMULT2(cx, sy);
	MatrixW2V._21 = PhdMatrixPtr->_21 = -(sx);
	MatrixW2V._22 = PhdMatrixPtr->_22 = TRIGMULT2(cx, cy);

	MatrixW2V._03 = PhdMatrixPtr->_03 = viewPos->x;
	MatrixW2V._13 = PhdMatrixPtr->_13 = viewPos->y;
	MatrixW2V._23 = PhdMatrixPtr->_23 = viewPos->z;
}

void phd_LookAt(int xsrc, int ysrc, int zsrc, int xtar, int ytar, int ztar, short roll) {
	PHD_3DPOS viewPos = {};
	VECTOR_ANGLES angles;
	phd_GetVectorAngles(xtar - xsrc, ytar - ysrc, ztar - zsrc, &angles);
	viewPos.x = xsrc;
	viewPos.y = ysrc;
	viewPos.z = zsrc;
	viewPos.rotX = angles.rotX;
	viewPos.rotY = angles.rotY;
	viewPos.rotZ = roll;
	phd_GenerateW2V(&viewPos);
	CamPos.x = xsrc;
	CamPos.y = ysrc;
	CamPos.z = zsrc;
}

void phd_GetVectorAngles(int x, int y, int z, VECTOR_ANGLES* angles) {
	short xRot;

	angles->rotY = phd_atan(z, x);
	while ((short)x != x || (short)y != y || (short)z != z) {
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}
	xRot = phd_atan(phd_sqrt(SQR(x) + SQR(z)), y);
	if ((y > 0 && xRot > 0) || (y < 0 && xRot < 0))
		xRot = -xRot;
	angles->rotX = xRot;
}

void phd_RotX(short angle) {
	if (angle != 0) {
		int m0, m1;
		int sx = phd_sin(angle);
		int cx = phd_cos(angle);

		m0 = PhdMatrixPtr->_01 * cx + PhdMatrixPtr->_02 * sx;
		m1 = PhdMatrixPtr->_02 * cx - PhdMatrixPtr->_01 * sx;
		PhdMatrixPtr->_01 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_02 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_11 * cx + PhdMatrixPtr->_12 * sx;
		m1 = PhdMatrixPtr->_12 * cx - PhdMatrixPtr->_11 * sx;
		PhdMatrixPtr->_11 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_12 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_21 * cx + PhdMatrixPtr->_22 * sx;
		m1 = PhdMatrixPtr->_22 * cx - PhdMatrixPtr->_21 * sx;
		PhdMatrixPtr->_21 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_22 = m1 >> W2V_SHIFT;
	}
}

void phd_RotY(short angle) {
	if (angle != 0) {
		int m0, m1;
		int sy = phd_sin(angle);
		int cy = phd_cos(angle);

		m0 = PhdMatrixPtr->_00 * cy - PhdMatrixPtr->_02 * sy;
		m1 = PhdMatrixPtr->_02 * cy + PhdMatrixPtr->_00 * sy;
		PhdMatrixPtr->_00 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_02 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_10 * cy - PhdMatrixPtr->_12 * sy;
		m1 = PhdMatrixPtr->_12 * cy + PhdMatrixPtr->_10 * sy;
		PhdMatrixPtr->_10 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_12 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_20 * cy - PhdMatrixPtr->_22 * sy;
		m1 = PhdMatrixPtr->_22 * cy + PhdMatrixPtr->_20 * sy;
		PhdMatrixPtr->_20 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_22 = m1 >> W2V_SHIFT;
	}
}

void phd_RotZ(short angle) {
	if (angle != 0) {
		int m0, m1;
		int sz = phd_sin(angle);
		int cz = phd_cos(angle);

		m0 = PhdMatrixPtr->_00 * cz + PhdMatrixPtr->_01 * sz;
		m1 = PhdMatrixPtr->_01 * cz - PhdMatrixPtr->_00 * sz;
		PhdMatrixPtr->_00 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_01 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_10 * cz + PhdMatrixPtr->_11 * sz;
		m1 = PhdMatrixPtr->_11 * cz - PhdMatrixPtr->_10 * sz;
		PhdMatrixPtr->_10 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_11 = m1 >> W2V_SHIFT;
		m0 = PhdMatrixPtr->_20 * cz + PhdMatrixPtr->_21 * sz;
		m1 = PhdMatrixPtr->_21 * cz - PhdMatrixPtr->_20 * sz;
		PhdMatrixPtr->_20 = m0 >> W2V_SHIFT;
		PhdMatrixPtr->_21 = m1 >> W2V_SHIFT;
	}
}

void phd_RotYXZ(short ry, short rx, short rz) {
	if (ry != 0) phd_RotY(ry);
	if (rx != 0) phd_RotX(rx);
	if (rz != 0) phd_RotZ(rz);
}

void phd_RotYXZpack(DWORD rpack) {
	short rx = ((rpack >> 20) & 0x3FF) << 6;
	short ry = ((rpack >> 10) & 0x3FF) << 6;
	short rz = ((rpack >> 00) & 0x3FF) << 6;
	phd_RotYXZ(ry, rx, rz);
}

BOOL phd_TranslateRel(int x, int y, int z) {
	PhdMatrixPtr->_03 += PhdMatrixPtr->_00 * x + PhdMatrixPtr->_01 * y + PhdMatrixPtr->_02 * z;
	PhdMatrixPtr->_13 += PhdMatrixPtr->_10 * x + PhdMatrixPtr->_11 * y + PhdMatrixPtr->_12 * z;
	PhdMatrixPtr->_23 += PhdMatrixPtr->_20 * x + PhdMatrixPtr->_21 * y + PhdMatrixPtr->_22 * z;

	if (ABS(PhdMatrixPtr->_03) > PhdFarZ ||
		ABS(PhdMatrixPtr->_13) > PhdFarZ ||
		ABS(PhdMatrixPtr->_23) > PhdFarZ)
	{
		return FALSE;
	}
	return TRUE;
}

void phd_TranslateAbs(int x, int y, int z) {
	x -= MatrixW2V._03;
	y -= MatrixW2V._13;
	z -= MatrixW2V._23;

	PhdMatrixPtr->_03 = x * PhdMatrixPtr->_00 + y * PhdMatrixPtr->_01 + z * PhdMatrixPtr->_02;
	PhdMatrixPtr->_13 = x * PhdMatrixPtr->_10 + y * PhdMatrixPtr->_11 + z * PhdMatrixPtr->_12;
	PhdMatrixPtr->_23 = x * PhdMatrixPtr->_20 + y * PhdMatrixPtr->_21 + z * PhdMatrixPtr->_22;
}

void phd_PutPolygons(short* ptrObj, int clip) {
	FltWinLeft = (float)PhdWinMinX;
	FltWinTop = (float)PhdWinMinY;
	FltWinRight = (float)(PhdWinMinX + PhdWinMaxX + 1);
	FltWinBottom = (float)(PhdWinMinY + PhdWinMaxY + 1);
	FltWinCenterX = (float)(PhdWinMinX + PhdWinCenterX);
	FltWinCenterY = (float)(PhdWinMinY + PhdWinCenterY);
#ifdef FEATURE_VIDEOFX_IMPROVED
	short* ptrEnv = ptrObj;
#endif // FEATURE_VIDEOFX_IMPROVED
	ptrObj += 4; // skip x, y, z, radius
	ptrObj = calc_object_vertices(ptrObj);
	if (ptrObj != NULL) {
		ptrObj = calc_vertice_light(ptrObj);
		ptrObj = ins_objectGT4(ptrObj + 1, *ptrObj, ST_AvgZ);
		ptrObj = ins_objectGT3(ptrObj + 1, *ptrObj, ST_AvgZ);
		ptrObj = ins_objectG4(ptrObj + 1, *ptrObj, ST_AvgZ);
		ptrObj = ins_objectG3(ptrObj + 1, *ptrObj, ST_AvgZ);
#ifdef FEATURE_VIDEOFX_IMPROVED
		phd_PutEnvmapPolygons(ptrEnv);
#endif // FEATURE_VIDEOFX_IMPROVED
	}
}

void S_InsertRoom(ROOM_DATA* ptrObj, BOOL isOutside) {
	FltWinLeft = (float)(PhdWinMinX + PhdWinLeft);
	FltWinTop = (float)(PhdWinMinY + PhdWinTop);
	FltWinRight = (float)(PhdWinMinX + PhdWinRight + 1);
	FltWinBottom = (float)(PhdWinMinY + PhdWinBottom + 1);
	FltWinCenterX = (float)(PhdWinMinX + PhdWinCenterX);
	FltWinCenterY = (float)(PhdWinMinY + PhdWinCenterY);
	calc_room_vertices(ptrObj, isOutside ? 0 : 16);
	ins_roomGT4(ptrObj->gt4, ptrObj->gt4Size, ST_MaxZ);
	ins_roomGT3(ptrObj->gt3, ptrObj->gt3Size, ST_MaxZ);
	ins_room_sprite(ptrObj->sprites, ptrObj->spriteSize);
}

short* calc_background_light(short* ptrObj) {
	int vtxCount = *ptrObj++;

	if (vtxCount > 0) {
		ptrObj += 3 * vtxCount;
	}
	else if (vtxCount < 0) {
		vtxCount = -vtxCount;
		ptrObj += vtxCount;
	}

	// Skybox has normal brightness
	int shade = 0xFFF;

	// NOTE: Sunset did not change the skybox brightness in the original game
	if (GF_SunsetEnabled)
		shade += 0x400 * SunsetTimer / SUNSET_TIMEOUT;

	for (int i = 0; i < vtxCount; ++i)
		PhdVBuf[i].g = shade;

	return ptrObj;
}

void S_InsertBackground(short* ptrObj) {
	FltWinLeft = (float)(PhdWinMinX + PhdWinLeft);
	FltWinTop = (float)(PhdWinMinY + PhdWinTop);
	FltWinRight = (float)(PhdWinMinX + PhdWinRight + 1);
	FltWinBottom = (float)(PhdWinMinY + PhdWinBottom + 1);
	FltWinCenterX = (float)(PhdWinMinX + PhdWinCenterX);
	FltWinCenterY = (float)(PhdWinMinY + PhdWinCenterY);

	ptrObj += 4; // skip x, y, z, radius
	ptrObj = calc_object_vertices(ptrObj);
	if (ptrObj == NULL) {
		return;
	}
	ptrObj = calc_background_light(ptrObj);

#ifdef FEATURE_VIEW_IMPROVED
	MidSort = 0xFFFF;
#endif // FEATURE_VIEW_IMPROVED
	if (SavedAppSettings.RenderMode == RM_Hardware) {
		HWR_EnableZBuffer(false, false);
	}
	ptrObj = ins_objectGT4(ptrObj + 1, *ptrObj, ST_FarZ);
	ptrObj = ins_objectGT3(ptrObj + 1, *ptrObj, ST_FarZ);
	ptrObj = ins_objectG4(ptrObj + 1, *ptrObj, ST_FarZ);
	ptrObj = ins_objectG3(ptrObj + 1, *ptrObj, ST_FarZ);
	if (SavedAppSettings.RenderMode == RM_Hardware) {
		HWR_EnableZBuffer(true, true);
	}
#ifdef FEATURE_VIEW_IMPROVED
	MidSort = 0;
#endif // FEATURE_VIEW_IMPROVED
}

void S_InsertInvBgnd(short* ptrObj) {
	// NOTE: Null function in the PC version.
	// But there is waving inventory function in the PlayStation version.
	// Main S_InsertInvBgnd() logic is similar to S_InsertBackground();
}

short* calc_object_vertices(short* ptrObj) {
	float xv, yv, zv, persp, baseZ;
	int vtxCount;
	BYTE totalClip, clipFlags;

	baseZ = 0.0;
#ifndef FEATURE_VIEW_IMPROVED
	if (SavedAppSettings.RenderMode == RM_Software || !SavedAppSettings.ZBuffer) {
		baseZ = (double)(MidSort << (W2V_SHIFT + 8));
	}
#endif // !FEATURE_VIEW_IMPROVED

	totalClip = 0xFF;

	ptrObj++; // skip poly counter
	vtxCount = *(ptrObj++); // get vertex counter

	for (int i = 0; i < vtxCount; ++i) {
		xv = (float)(PhdMatrixPtr->_00 * ptrObj[0] +
			PhdMatrixPtr->_01 * ptrObj[1] +
			PhdMatrixPtr->_02 * ptrObj[2] +
			PhdMatrixPtr->_03);

		yv = (float)(PhdMatrixPtr->_10 * ptrObj[0] +
			PhdMatrixPtr->_11 * ptrObj[1] +
			PhdMatrixPtr->_12 * ptrObj[2] +
			PhdMatrixPtr->_13);

		zv = (float)(PhdMatrixPtr->_20 * ptrObj[0] +
			PhdMatrixPtr->_21 * ptrObj[1] +
			PhdMatrixPtr->_22 * ptrObj[2] +
			PhdMatrixPtr->_23);

		PhdVBuf[i].xv = xv;
		PhdVBuf[i].yv = yv;

		if (IsWaterEffect)
			PhdVBuf[i].g += ShadesTable[(WibbleOffset + (BYTE)RandomTable[(vtxCount - i) % WIBBLE_SIZE]) % WIBBLE_SIZE] >> 2;

		if (zv < FltNearZ) {
			clipFlags = 0x80;
			PhdVBuf[i].zv = zv;
		}
		else {
			clipFlags = 0;

			if (zv >= FltFarZ) {
				zv = FltFarZ;
				PhdVBuf[i].zv = zv;
			}
			else {
				PhdVBuf[i].zv = zv + baseZ;
			}

			persp = FltPersp / zv;

			PhdVBuf[i].xs = persp * xv + FltWinCenterX;
			PhdVBuf[i].ys = persp * yv + FltWinCenterY;
			PhdVBuf[i].rhw = persp * FltRhwOPersp;

			if (PhdVBuf[i].xs < FltWinLeft)
				clipFlags |= 0x01;
			else if (PhdVBuf[i].xs > FltWinRight)
				clipFlags |= 0x02;

			if (PhdVBuf[i].ys < FltWinTop)
				clipFlags |= 0x04;
			else if (PhdVBuf[i].ys > FltWinBottom)
				clipFlags |= 0x08;
		}

		PhdVBuf[i].clip = clipFlags;
		totalClip &= clipFlags;
		ptrObj += 3;
	}
	return (totalClip == 0) ? ptrObj : NULL;
}

short* calc_vertice_light(short* ptrObj) {
	int i, xv, yv, zv;
	short shade;
	int vtxCount = *ptrObj++;

	if (vtxCount > 0) {
		if (LsDivider != 0) {
			xv = (PhdMatrixPtr->_00 * LsVectorView.x +
				  PhdMatrixPtr->_10 * LsVectorView.y +
				  PhdMatrixPtr->_20 * LsVectorView.z) / LsDivider;
			yv = (PhdMatrixPtr->_01 * LsVectorView.x +
				  PhdMatrixPtr->_11 * LsVectorView.y +
				  PhdMatrixPtr->_21 * LsVectorView.z) / LsDivider;
			zv = (PhdMatrixPtr->_02 * LsVectorView.x +
				  PhdMatrixPtr->_12 * LsVectorView.y +
				  PhdMatrixPtr->_22 * LsVectorView.z) / LsDivider;

			for (i = 0; i < vtxCount; ++i) {
				shade = LsAdder + ((ptrObj[0] * xv + ptrObj[1] * yv + ptrObj[2] * zv) >> 16);
				CLAMP(shade, 0, 0x1FFF);
				PhdVBuf[i].g = shade;
				ptrObj += 3;
			}
		}
		else {
			shade = LsAdder;
			CLAMP(shade, 0, 0x1FFF);
			for (i = 0; i < vtxCount; ++i) {
				PhdVBuf[i].g = shade;
			}
			ptrObj += 3 * vtxCount;
		}
	}
	else {
		for (i = 0; i < -vtxCount; ++i) {
			shade = LsAdder + *ptrObj;
			CLAMP(shade, 0, 0x1FFF);
			PhdVBuf[i].g = shade;
			++ptrObj;
		}
	}
	return ptrObj;
}

void calc_room_vertices(ROOM_DATA* ptrObj, BYTE farClip) {
	PHD_VBUF* vbuf;
	ROOM_VERTEX* vtx;
	float xv, yv, zv, persp, depth;
	float baseZ = 0.0;
	int zv_int;

#if !defined(FEATURE_VIEW_IMPROVED)
	if (SavedAppSettings.RenderMode == RM_Software || !SavedAppSettings.ZBuffer) {
		baseZ = (double)(MidSort << (W2V_SHIFT + 8));
	}
#endif // !FEATURE_VIEW_IMPROVED

	for (int i = 0; i < ptrObj->vtxSize; ++i)
	{
		vtx = &ptrObj->vertices[i];
		xv = (float)(PhdMatrixPtr->_00 * vtx->x + PhdMatrixPtr->_01 * vtx->y + PhdMatrixPtr->_02 * vtx->z + PhdMatrixPtr->_03);
		yv = (float)(PhdMatrixPtr->_10 * vtx->x + PhdMatrixPtr->_11 * vtx->y + PhdMatrixPtr->_12 * vtx->z + PhdMatrixPtr->_13);
		zv_int =    (PhdMatrixPtr->_20 * vtx->x + PhdMatrixPtr->_21 * vtx->y + PhdMatrixPtr->_22 * vtx->z + PhdMatrixPtr->_23);
		zv = (float)zv_int;

		vbuf = &PhdVBuf[i];
		vbuf->xv = xv;
		vbuf->yv = yv;
		vbuf->g = vtx->lightAdder;

		if (IsWaterEffect != 0)
			vbuf->g += ShadesTable[(WibbleOffset + (BYTE)RandomTable[(ptrObj->vtxSize - i) % WIBBLE_SIZE]) % WIBBLE_SIZE];

		if (zv < FltNearZ) {
			vbuf->clip = 0xFF80;
			vbuf->zv = zv;
		}
		else {
			persp = FltPersp / zv;
			depth = (float)(zv_int >> W2V_SHIFT);

#if defined(FEATURE_VIEW_IMPROVED)
			if (depth >= PhdViewDistance) {
				vbuf->rhw = persp * FltRhwOPersp;
				vbuf->zv = zv + baseZ;
#else // !FEATURE_VIEW_IMPROVED
			if (depth >= DEPTHQ_END) { // fog end
				vbuf->rhw = 0.0; // NOTE: zero RHW is an invalid value, but the original game sets it.
				vbuf->zv = FltFarZ;
#endif // FEATURE_VIEW_IMPROVED
				vbuf->g = 0x1FFF;
				vbuf->clip = farClip;
			}
			else {
#if defined(FEATURE_VIEW_IMPROVED)
				vbuf->g += (short)CalculateFogShade((int)depth);
#else // !FEATURE_VIEW_IMPROVED
				if (depth > DEPTHQ_START) { // fog begin
					vbuf->g += depth - DEPTHQ_START;
				}
#endif // FEATURE_VIEW_IMPROVED
				vbuf->rhw = persp * FltRhwOPersp;
				vbuf->clip = 0;
				vbuf->zv = zv + baseZ;
			}

			vbuf->xs = persp * xv + FltWinCenterX;
			vbuf->ys = persp * yv + FltWinCenterY;

			if (IsWibbleEffect && vtx->flags >= 0) {
				vbuf->xs += WibbleTable[(WibbleOffset + (BYTE)vbuf->ys) % WIBBLE_SIZE];
				vbuf->ys += WibbleTable[(WibbleOffset + (BYTE)vbuf->xs) % WIBBLE_SIZE];
			}

			if (vbuf->xs < FltWinLeft)
				vbuf->clip |= 0x01;
			else if (vbuf->xs > FltWinRight)
				vbuf->clip |= 0x02;
			if (vbuf->ys < FltWinTop)
				vbuf->clip |= 0x04;
			else if (vbuf->ys > FltWinBottom)
				vbuf->clip |= 0x08;

			vbuf->clip |= ~(BYTE)(vbuf->zv / 0x155555.p0) << 8;
		}

		CLAMP(vbuf->g, 0, 8191);
	}
}

void phd_RotateLight(short yRot, short xRot) {
	int xcos, ysin, wcos, wsin;
	int ls_x, ls_y, ls_z;

	PhdLsXRot = xRot;
	PhdLsYRot = yRot;

	xcos = phd_cos(yRot);
	ysin = phd_sin(yRot);
	wcos = phd_cos(xRot);
	wsin = phd_sin(xRot);

	ls_x = TRIGMULT2(xcos, wsin);
	ls_y = -ysin;
	ls_z = TRIGMULT2(xcos, wcos);

	LsVectorView.x = (MatrixW2V._00 * ls_x + MatrixW2V._01 * ls_y + MatrixW2V._02 * ls_z) >> W2V_SHIFT;
	LsVectorView.y = (MatrixW2V._10 * ls_x + MatrixW2V._11 * ls_y + MatrixW2V._12 * ls_z) >> W2V_SHIFT;
	LsVectorView.z = (MatrixW2V._20 * ls_x + MatrixW2V._21 * ls_y + MatrixW2V._22 * ls_z) >> W2V_SHIFT;
}

void phd_InitPolyList() {
	SurfaceCount = 0;
	Sort3dPtr = SortBuffer;
	Info3dPtr = Info3dBuffer;
	if (SavedAppSettings.RenderMode == RM_Hardware)
		HWR_VertexPtr = HWR_VertexBuffer;
}

void phd_SortPolyList() {
	if (SurfaceCount) {
		for (DWORD i = 0; i < SurfaceCount; ++i) {
#ifdef FEATURE_VIEW_IMPROVED
			SortBuffer[i]._1 <<= 16;
#endif // FEATURE_VIEW_IMPROVED
			SortBuffer[i]._1 += i;
		}
		do_quickysorty(0, SurfaceCount - 1);
	}
}

void do_quickysorty(int left, int right) {
#ifdef FEATURE_VIEW_IMPROVED
	UINT64 swapBuf;
	UINT64 compare = SortBuffer[(left + right) / 2]._1;
#else // FEATURE_VIEW_IMPROVED
	DWORD swapBuf;
	DWORD compare = SortBuffer[(left + right) / 2]._1;
#endif // FEATURE_VIEW_IMPROVED
	int i = left;
	int j = right;

	do {
		while ((i < right) && (SortBuffer[i]._1 > compare)) ++i;
		while ((left < j) && (compare > SortBuffer[j]._1)) --j;
		if (i > j) break;
		SWAP(SortBuffer[i]._0, SortBuffer[j]._0, swapBuf);
		SWAP(SortBuffer[i]._1, SortBuffer[j]._1, swapBuf);
	} while (++i <= --j);

	if (left < j)
		do_quickysorty(left, j);
	if (i < right)
		do_quickysorty(i, right);
}

void phd_PrintPolyList(BYTE* surfacePtr) {
	short polyType, * bufPtr;
	PrintSurfacePtr = surfacePtr;

	for (DWORD i = 0; i < SurfaceCount; ++i) {
		bufPtr = (short*)SortBuffer[i]._0;
		polyType = *(bufPtr++); // poly has type as routine index in first word
		PolyDrawRoutines[polyType](bufPtr); // send poly data as parameter to routine
	}
}

void AlterFOV(short fov) {
	fov /= 2; // half fov angle

#ifdef FEATURE_VIEW_IMPROVED
	int fovWidth = PhdWinHeight * 320 / (PsxFovEnabled ? 200 : 240);
	FltViewAspect = 1.0; // must always be 1.0 for unstretched view
	PhdPersp = (fovWidth / 2) * phd_cos(fov) / phd_sin(fov);
#else // !FEATURE_VIEW_IMPROVED
	PhdPersp = (PhdWinWidth / 2) * phd_cos(fov) / phd_sin(fov);
#endif // FEATURE_VIEW_IMPROVED

	FltPersp = (float)PhdPersp;
	FltRhwOPersp = RhwFactor / FltPersp;
	FltPerspONearZ = FltPersp / FltNearZ;

#ifndef FEATURE_VIEW_IMPROVED
	double windowAspect = 4.0 / 3.0;
	if (!SavedAppSettings.FullScreen && SavedAppSettings.AspectMode == AM_16_9) {
		windowAspect = 16.0 / 9.0;
	}
	FltViewAspect = windowAspect / ((double)PhdWinWidth / (double)PhdWinHeight);
#endif // !FEATURE_VIEW_IMPROVED
}

void phd_SetNearZ(int nearZ) {
	PhdNearZ = nearZ;
	FltNearZ = (float)nearZ;
	FltRhwONearZ = RhwFactor / FltNearZ;
	FltPerspONearZ = FltPersp / FltNearZ;
	float resZ = 0.99f * FltFarZ * FltNearZ / (FltFarZ - FltNearZ);
	FltResZ = resZ;
	FltResZORhw = resZ / RhwFactor;
	FltResZBuf = 0.005f + resZ / FltNearZ;
}

void phd_SetFarZ(int farZ) {
	PhdFarZ = farZ;
	FltFarZ = (float)farZ;
	float resZ = 0.99f * FltFarZ * FltNearZ / (FltFarZ - FltNearZ);
	FltResZ = resZ;
	FltResZORhw = resZ / RhwFactor;
	FltResZBuf = 0.005f + resZ / FltNearZ;
}

void phd_InitWindow(short x, short y, int width, int height, int nearZ, int farZ, short viewAngle, int screenWidth, int screenHeight) {
	PhdWinMinX = x;
	PhdWinMinY = y;
	PhdWinMaxX = width - 1;
	PhdWinMaxY = height - 1;

	PhdWinWidth = width;
	PhdWinHeight = height;

	PhdWinCenterX = width / 2;
	PhdWinCenterY = height / 2;
	FltWinCenterX = (float)(PhdWinCenterX);
	FltWinCenterY = (float)(PhdWinCenterY);

	PhdWinLeft = 0;
	PhdWinTop = 0;
	PhdWinRight = PhdWinMaxX;
	PhdWinBottom = PhdWinMaxY;

	PhdWinRect.left = PhdWinMinX;
	PhdWinRect.bottom = PhdWinMinY + PhdWinHeight;
	PhdWinRect.top = PhdWinMinY;
	PhdWinRect.right = PhdWinMinX + PhdWinWidth;

	PhdScreenWidth = screenWidth;
	PhdScreenHeight = screenHeight;

#ifdef FEATURE_VIEW_IMPROVED
	double baseDistance = (double)farZ;
	farZ = (int)(baseDistance * ViewDistanceFactor);
	FogBeginDepth = (int)(baseDistance * FogBeginFactor);
	FogEndDepth = (int)(baseDistance * FogEndFactor);
	WaterFogBeginDepth = (int)(baseDistance * WaterFogBeginFactor);
	WaterFogEndDepth = (int)(baseDistance * WaterFogEndFactor);
#endif // FEATURE_VIEW_IMPROVED

	PhdFov = ANGLE(viewAngle);
	PhdNearZ = nearZ << W2V_SHIFT;
	PhdFarZ = farZ << W2V_SHIFT;
	PhdViewDistance = farZ;

	AlterFOV(PhdFov); // convert degrees to PHD angle
	phd_SetNearZ(PhdNearZ);
	phd_SetFarZ(PhdFarZ);

	PhdMatrixPtr = MatrixStack; // reset matrix stack pointer

	switch (SavedAppSettings.RenderMode)
	{
	case RM_Software:
		PerspectiveDistance = SavedAppSettings.PerspectiveCorrect ? SW_DETAIL_HIGH : SW_DETAIL_MEDIUM;
		ins_objectGT3 = InsertObjectGT3;
		ins_objectGT4 = InsertObjectGT4;
		ins_objectG3 = InsertObjectG3;
		ins_objectG4 = InsertObjectG4;
		ins_roomGT3 = InsertRoomGT3;
		ins_roomGT4 = InsertRoomGT4;
		ins_flat_rect = InsertFlatRect;
		ins_line = InsertLine;
		ins_sprite = InsertSprite;
		ins_poly_trans8 = InsertTrans8;
		ins_trans_quad = InsertTransQuad;
		break;
	case RM_Hardware:
		if (SavedAppSettings.ZBuffer) {
			ins_objectGT3 = InsertObjectGT3_ZBuffered;
			ins_objectGT4 = InsertObjectGT4_ZBuffered;
			ins_objectG3 = InsertObjectG3_ZBuffered;
			ins_objectG4 = InsertObjectG4_ZBuffered;
			ins_roomGT3 = InsertRoomGT3_ZBuffered;
			ins_roomGT4 = InsertRoomGT4_ZBuffered;
			ins_flat_rect = InsertFlatRect_ZBuffered;
			ins_line = InsertLine_ZBuffered;
		}
		else {
			ins_objectGT3 = InsertObjectGT3_Sorted;
			ins_objectGT4 = InsertObjectGT4_Sorted;
			ins_objectG3 = InsertObjectG3_Sorted;
			ins_objectG4 = InsertObjectG4_Sorted;
			ins_roomGT3 = InsertRoomGT3_Sorted;
			ins_roomGT4 = InsertRoomGT4_Sorted;
			ins_flat_rect = InsertFlatRect_Sorted;
			ins_line = InsertLine_Sorted;
		}
		ins_sprite = InsertSprite_Sorted;
		ins_poly_trans8 = InsertTrans8_Sorted;
		ins_trans_quad = InsertTransQuad_Sorted;
		break;
	case RM_Unknown:
		LogWarn("Failed to setup the renderer, render mode is unknown !");
		break;
	}
}

void phd_PopMatrix() {
	--PhdMatrixPtr;
}

void phd_PushMatrix() {
	PhdMatrixPtr[1] = PhdMatrixPtr[0]; // copy the last matrix
	++PhdMatrixPtr;
}

void phd_PushUnitMatrix() {
	++PhdMatrixPtr;
	memset(PhdMatrixPtr, 0, sizeof(PHD_MATRIX));
	PhdMatrixPtr->_00 = W2V_SCALE;
	PhdMatrixPtr->_11 = W2V_SCALE;
	PhdMatrixPtr->_22 = W2V_SCALE;
}

/// <summary>
/// Decompose the matrix to position, rotation and scale.
/// Useful for making something follow a bone rotation like effects.
/// </summary>
/// <param name="matrix">A valid matrix (Usually PhdMatrixPtr).</param>
/// <param name="position">Position will be in local space, use the item position to be in absolute space.</param>
/// <param name="rotation">In TR angle (-16384 to 16384)</param>
/// <param name="scale">In TR scale (16384 is 1)</param>
void phd_DecomposeMatrix(PHD_MATRIX* matrix, PHD_VECTOR* position, POS_3D* rotation, PHD_VECTOR* scale, bool changeToOriginalScale)
{
	// Extract position
	position->x = matrix->_03 >> W2V_SHIFT;
	position->y = matrix->_13 >> W2V_SHIFT;
	position->z = matrix->_23 >> W2V_SHIFT;

	// Shift down the matrix components to adjust for W2V_SHIFT
	int adjustedMatrix[3][3] = {
		{ matrix->_00 >> W2V_SHIFT, matrix->_01 >> W2V_SHIFT, matrix->_02 >> W2V_SHIFT },
		{ matrix->_10 >> W2V_SHIFT, matrix->_11 >> W2V_SHIFT, matrix->_12 >> W2V_SHIFT },
		{ matrix->_20 >> W2V_SHIFT, matrix->_21 >> W2V_SHIFT, matrix->_22 >> W2V_SHIFT }
	};

	// Extract scale
	scale->x = (int)phd_sqrt(adjustedMatrix[0][0] * adjustedMatrix[0][0] +
		                     adjustedMatrix[0][1] * adjustedMatrix[0][1] +
		                     adjustedMatrix[0][2] * adjustedMatrix[0][2]);

	scale->y = (int)phd_sqrt(adjustedMatrix[1][0] * adjustedMatrix[1][0] +
		                     adjustedMatrix[1][1] * adjustedMatrix[1][1] +
		                     adjustedMatrix[1][2] * adjustedMatrix[1][2]);

	scale->z = (int)phd_sqrt(adjustedMatrix[2][0] * adjustedMatrix[2][0] +
		                     adjustedMatrix[2][1] * adjustedMatrix[2][1] +
		                     adjustedMatrix[2][2] * adjustedMatrix[2][2]);

	// Handle potential zero scales
	scale->x = scale->x == 0 ? 1 : scale->x;
	scale->y = scale->y == 0 ? 1 : scale->y;
	scale->z = scale->z == 0 ? 1 : scale->z;

	// Normalize rotation matrix
	int rotMatrix[3][3] = {
		{ adjustedMatrix[0][0] / scale->x, adjustedMatrix[0][1] / scale->x, adjustedMatrix[0][2] / scale->x },
		{ adjustedMatrix[1][0] / scale->y, adjustedMatrix[1][1] / scale->y, adjustedMatrix[1][2] / scale->y },
		{ adjustedMatrix[2][0] / scale->z, adjustedMatrix[2][1] / scale->z, adjustedMatrix[2][2] / scale->z }
	};

	// Restore original scale (Not 1 but 16384 which is original TR scale).
	if (changeToOriginalScale)
	{
		scale->x <<= W2V_SHIFT;
		scale->y <<= W2V_SHIFT;
		scale->z <<= W2V_SHIFT;
	}

	// Extract Euler angles from normalized rotation matrix
	rotation->y = (short)phd_atan(-rotMatrix[2][0], phd_sqrt(rotMatrix[2][1] * rotMatrix[2][1] + rotMatrix[2][2] * rotMatrix[2][2]));

	if (rotMatrix[2][0] != 1 && rotMatrix[2][0] != -1) {
		rotation->x = (short)phd_atan(rotMatrix[2][1], rotMatrix[2][2]);
		rotation->z = (short)phd_atan(rotMatrix[1][0], rotMatrix[0][0]);
	}
	else {
		rotation->z = 0; // Handle gimbal lock case
		rotation->x = (short)phd_atan(rotMatrix[1][2], rotMatrix[1][1]);
	}
}

/*
 * Inject function
 */
void Inject_3Dgen() {
	INJECT(0x00401000, phd_GenerateW2V);
	INJECT(0x004011D0, phd_LookAt);
	INJECT(0x00401250, phd_GetVectorAngles);
	INJECT(0x004012D0, phd_RotX);
	INJECT(0x00401380, phd_RotY);
	INJECT(0x00401430, phd_RotZ);
	INJECT(0x004014E0, phd_RotYXZ);
	INJECT(0x004016C0, phd_RotYXZpack);
	INJECT(0x004018B0, phd_TranslateRel);
	INJECT(0x00401960, phd_TranslateAbs);
	INJECT(0x004019E0, phd_PutPolygons);
	INJECT(0x00401AE0, S_InsertRoom);
	INJECT(0x00401BD0, calc_background_light);
	INJECT(0x00401C10, S_InsertBackground);
	//INJECT(----------, S_InsertInvBgnd); // NOTE: this is null in the original code
	INJECT(0x00401D50, calc_object_vertices);
	INJECT(0x00401F30, calc_vertice_light);
	INJECT(0x004020A0, calc_room_vertices);
	INJECT(0x00402320, phd_RotateLight);
	INJECT(0x004023F0, phd_InitPolyList);
	INJECT(0x00402420, phd_SortPolyList);
	INJECT(0x00402460, do_quickysorty);
	INJECT(0x00402530, phd_PrintPolyList);
	INJECT(0x00402570, AlterFOV);
	INJECT(0x00402680, phd_SetNearZ);
	INJECT(0x004026D0, phd_SetFarZ);
	INJECT(0x004026F0, phd_InitWindow);
	//INJECT(----------, phd_PopMatrix); // NOTE: this is inline or macro in the original code
	INJECT(0x00457510, phd_PushMatrix);
	INJECT(0x0045752E, phd_PushUnitMatrix);
}