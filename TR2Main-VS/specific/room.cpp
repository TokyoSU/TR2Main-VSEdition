#include "precompiled.h"
#include "room.h"
#include "3dinsert.h"
#include "hwr.h"
#include "vars.h"

#if defined(FEATURE_VIDEOFX_IMPROVED)
extern DWORD ShadowMode;
extern DWORD AlphaBlendMode;
#endif

void(*ins_roomGT3)(FACE3*, int, SORTTYPE);
void(*ins_roomGT4)(FACE4*, int, SORTTYPE);

void InsertRoomGT3(FACE3* ptrObj, int number, SORTTYPE sortType)
{
	char clipOR, clipAND;
	PHD_VBUF* vtx0, *vtx1, *vtx2;
	int i, j, nPoints;
	float zv;
	short textureIdx;
	FACE3* face;
	PHD_TEXTURE* texture;
	PHD_UV* uv;
	POINT_INFO points[3] = {};

	for (i = 0; i < number; ++i) {
		face = &ptrObj[i];
		vtx0 = &PhdVBuf[face->vertices[0]];
		vtx1 = &PhdVBuf[face->vertices[1]];
		vtx2 = &PhdVBuf[face->vertices[2]];
		textureIdx = face->texture;
		texture = &PhdTextureInfo[textureIdx];
		uv = texture->uv;
		nPoints = 3;
		clipOR = LOBYTE(vtx0->clip | vtx1->clip | vtx2->clip);
		clipAND = LOBYTE(vtx0->clip & vtx1->clip & vtx2->clip);
		if (clipAND != 0)
			continue;

		if (clipOR >= 0) {
			if (!CheckVisible(vtx0, vtx1, vtx2))
				continue;

			if (clipOR == 0) {
				zv = CalculatePolyZ(sortType, vtx0->zv, vtx1->zv, vtx2->zv);
				Sort3dPtr->_0 = (DWORD)Info3dPtr;
				Sort3dPtr->_1 = MAKE_ZSORT(zv);
				++Sort3dPtr;

				if (zv >= (float)PerspectiveDistance) {
					*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap : POLY_WGTmap;
					*Info3dPtr++ = texture->tpage;
					*Info3dPtr++ = 3;

					*Info3dPtr++ = (int)vtx0->xs;
					*Info3dPtr++ = (int)vtx0->ys;
					*Info3dPtr++ = (int)vtx0->g;
					*Info3dPtr++ = uv[0].u;
					*Info3dPtr++ = uv[0].v;

					*Info3dPtr++ = (int)vtx1->xs;
					*Info3dPtr++ = (int)vtx1->ys;
					*Info3dPtr++ = (int)vtx1->g;
					*Info3dPtr++ = uv[1].u;
					*Info3dPtr++ = uv[1].v;

					*Info3dPtr++ = (int)vtx2->xs;
					*Info3dPtr++ = (int)vtx2->ys;
					*Info3dPtr++ = (int)vtx2->g;
					*Info3dPtr++ = uv[2].u;
					*Info3dPtr++ = uv[2].v;
				}
				else {
					*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap_persp : POLY_WGTmap_persp;
					*Info3dPtr++ = texture->tpage;
					*Info3dPtr++ = 3;

					*Info3dPtr++ = (int)vtx0->xs;
					*Info3dPtr++ = (int)vtx0->ys;
					*Info3dPtr++ = (int)vtx0->g;
					*(float*)Info3dPtr = vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[0].u * vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[0].v * vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);

					*Info3dPtr++ = (int)vtx1->xs;
					*Info3dPtr++ = (int)vtx1->ys;
					*Info3dPtr++ = (int)vtx1->g;
					*(float*)Info3dPtr = vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[1].u * vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[1].v * vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);

					*Info3dPtr++ = (int)vtx2->xs;
					*Info3dPtr++ = (int)vtx2->ys;
					*Info3dPtr++ = (int)vtx2->g;
					*(float*)Info3dPtr = vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[2].u * vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[2].v * vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
				}
				++SurfaceCount;
				continue;
			}

			VBuffer[0].x = vtx0->xs;
			VBuffer[0].y = vtx0->ys;
			VBuffer[0].rhw = vtx0->rhw;
			VBuffer[0].g = (float)vtx0->g;
			VBuffer[0].u = (float)uv[0].u * vtx0->rhw;
			VBuffer[0].v = (float)uv[0].v * vtx0->rhw;

			VBuffer[1].x = vtx1->xs;
			VBuffer[1].y = vtx1->ys;
			VBuffer[1].rhw = vtx1->rhw;
			VBuffer[1].g = (float)vtx1->g;
			VBuffer[1].u = (float)uv[1].u * vtx1->rhw;
			VBuffer[1].v = (float)uv[1].v * vtx1->rhw;

			VBuffer[2].x = vtx2->xs;
			VBuffer[2].y = vtx2->ys;
			VBuffer[2].rhw = vtx2->rhw;
			VBuffer[2].g = (float)vtx2->g;
			VBuffer[2].u = (float)uv[2].u * vtx2->rhw;
			VBuffer[2].v = (float)uv[2].v * vtx2->rhw;
		}
		else {
			if (!visible_zclip(vtx0, vtx1, vtx2))
				continue;

			points[0].xv = vtx0->xv;
			points[0].yv = vtx0->yv;
			points[0].zv = vtx0->zv;
			points[0].rhw = vtx0->rhw;
			points[0].xs = vtx0->xs;
			points[0].ys = vtx0->ys;
			points[0].u = (float)uv[0].u;
			points[0].v = (float)uv[0].v;
			points[0].g = (float)vtx0->g;

			points[1].yv = vtx1->yv;
			points[1].xv = vtx1->xv;
			points[1].zv = vtx1->zv;
			points[1].rhw = vtx1->rhw;
			points[1].xs = vtx1->xs;
			points[1].ys = vtx1->ys;
			points[1].u = (float)uv[1].u;
			points[1].v = (float)uv[1].v;
			points[1].g = (float)vtx1->g;

			points[2].xv = vtx2->xv;
			points[2].yv = vtx2->yv;
			points[2].zv = vtx2->zv;
			points[2].rhw = vtx2->rhw;
			points[2].xs = vtx2->xs;
			points[2].ys = vtx2->ys;
			points[2].u = (float)uv[2].u;
			points[2].v = (float)uv[2].v;
			points[2].g = (float)vtx2->g;

			nPoints = ZedClipper(nPoints, points, VBuffer);
			if (nPoints == 0) continue;
		}

		nPoints = XYGUVClipper(nPoints, VBuffer);
		if (nPoints == 0) continue;

		zv = CalculatePolyZ(sortType, vtx0->zv, vtx1->zv, vtx2->zv);
		Sort3dPtr->_0 = (DWORD)Info3dPtr;
		Sort3dPtr->_1 = MAKE_ZSORT(zv);
		++Sort3dPtr;

		if (zv >= (float)PerspectiveDistance) {
			*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap : POLY_WGTmap;
			*Info3dPtr++ = texture->tpage;
			*Info3dPtr++ = nPoints;

			for (j = 0; j < nPoints; ++j) {
				*Info3dPtr++ = (int)VBuffer[j].x;
				*Info3dPtr++ = (int)VBuffer[j].y;
				*Info3dPtr++ = (int)VBuffer[j].g;
				*Info3dPtr++ = (int)(VBuffer[j].u / VBuffer[j].rhw);
				*Info3dPtr++ = (int)(VBuffer[j].v / VBuffer[j].rhw);
			}
		}
		else {
			*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap_persp : POLY_WGTmap_persp;
			*Info3dPtr++ = texture->tpage;
			*Info3dPtr++ = nPoints;

			for (j = 0; j < nPoints; ++j) {
				*Info3dPtr++ = (int)VBuffer[j].x;
				*Info3dPtr++ = (int)VBuffer[j].y;
				*Info3dPtr++ = (int)VBuffer[j].g;
				*(float*)Info3dPtr = VBuffer[j].rhw;
				Info3dPtr += sizeof(float) / sizeof(short);
				*(float*)Info3dPtr = VBuffer[j].u;
				Info3dPtr += sizeof(float) / sizeof(short);
				*(float*)Info3dPtr = VBuffer[j].v;
				Info3dPtr += sizeof(float) / sizeof(short);
			}
		}
		++SurfaceCount;
	}
}

void InsertRoomGT4(FACE4* ptrObj, int number, SORTTYPE sortType)
{
	char clipOR, clipAND;
	PHD_VBUF* vtx0, *vtx1, *vtx2, *vtx3;
	int i, j, nPoints;
	float zv;
	short textureIdx;
	FACE4* face;
	PHD_TEXTURE* texture;
	PHD_UV* uv;
	POINT_INFO points[4] = {};

	for (i = 0; i < number; ++i) {
		face = &ptrObj[i];
		vtx0 = &PhdVBuf[face->vertices[0]];
		vtx1 = &PhdVBuf[face->vertices[1]];
		vtx2 = &PhdVBuf[face->vertices[2]];
		vtx3 = &PhdVBuf[face->vertices[3]];
		textureIdx = face->texture;
		texture = &PhdTextureInfo[textureIdx];
		uv = texture->uv;
		nPoints = 4;
		clipOR = LOBYTE(vtx0->clip | vtx1->clip | vtx2->clip | vtx3->clip);
		clipAND = LOBYTE(vtx0->clip & vtx1->clip & vtx2->clip & vtx3->clip);
		if (clipAND != 0)
			continue;

		if (clipOR >= 0) {
			if (!CheckVisible(vtx0, vtx1, vtx2))
				continue;

			if (clipOR == 0) {
				zv = CalculatePolyZ(sortType, vtx0->zv, vtx1->zv, vtx2->zv, vtx3->zv);
				Sort3dPtr->_0 = (DWORD)Info3dPtr;
				Sort3dPtr->_1 = MAKE_ZSORT(zv);
				++Sort3dPtr;

				if (zv >= (double)PerspectiveDistance) {
					*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap : POLY_WGTmap;
					*Info3dPtr++ = texture->tpage;
					*Info3dPtr++ = 4;

					*Info3dPtr++ = (int)vtx0->xs;
					*Info3dPtr++ = (int)vtx0->ys;
					*Info3dPtr++ = (int)vtx0->g;
					*Info3dPtr++ = uv[0].u;
					*Info3dPtr++ = uv[0].v;

					*Info3dPtr++ = (int)vtx1->xs;
					*Info3dPtr++ = (int)vtx1->ys;
					*Info3dPtr++ = (int)vtx1->g;
					*Info3dPtr++ = uv[1].u;
					*Info3dPtr++ = uv[1].v;

					*Info3dPtr++ = (int)vtx2->xs;
					*Info3dPtr++ = (int)vtx2->ys;
					*Info3dPtr++ = (int)vtx2->g;
					*Info3dPtr++ = uv[2].u;
					*Info3dPtr++ = uv[2].v;

					*Info3dPtr++ = (int)vtx3->xs;
					*Info3dPtr++ = (int)vtx3->ys;
					*Info3dPtr++ = (int)vtx3->g;
					*Info3dPtr++ = uv[3].u;
					*Info3dPtr++ = uv[3].v;
				}
				else {
					*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap_persp : POLY_WGTmap_persp;
					*Info3dPtr++ = texture->tpage;
					*Info3dPtr++ = 4;

					*Info3dPtr++ = (int)vtx0->xs;
					*Info3dPtr++ = (int)vtx0->ys;
					*Info3dPtr++ = (int)vtx0->g;
					*(float*)Info3dPtr = vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[0].u * vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[0].v * vtx0->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);

					*Info3dPtr++ = (int)vtx1->xs;
					*Info3dPtr++ = (int)vtx1->ys;
					*Info3dPtr++ = (int)vtx1->g;
					*(float*)Info3dPtr = vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[1].u * vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[1].v * vtx1->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);

					*Info3dPtr++ = (int)vtx2->xs;
					*Info3dPtr++ = (int)vtx2->ys;
					*Info3dPtr++ = (int)vtx2->g;
					*(float*)Info3dPtr = vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[2].u * vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[2].v * vtx2->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);

					*Info3dPtr++ = (int)vtx3->xs;
					*Info3dPtr++ = (int)vtx3->ys;
					*Info3dPtr++ = (int)vtx3->g;
					*(float*)Info3dPtr = vtx3->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[3].u * vtx3->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
					*(float*)Info3dPtr = (float)uv[3].v * vtx3->rhw;
					Info3dPtr += sizeof(float) / sizeof(short);
				}
				++SurfaceCount;
				continue;
			}

			VBuffer[0].x = vtx0->xs;
			VBuffer[0].y = vtx0->ys;
			VBuffer[0].rhw = vtx0->rhw;
			VBuffer[0].g = (float)vtx0->g;
			VBuffer[0].u = (float)uv[0].u * vtx0->rhw;
			VBuffer[0].v = (float)uv[0].v * vtx0->rhw;

			VBuffer[1].x = vtx1->xs;
			VBuffer[1].y = vtx1->ys;
			VBuffer[1].rhw = vtx1->rhw;
			VBuffer[1].g = (float)vtx1->g;
			VBuffer[1].u = (float)uv[1].u * vtx1->rhw;
			VBuffer[1].v = (float)uv[1].v * vtx1->rhw;

			VBuffer[2].x = vtx2->xs;
			VBuffer[2].y = vtx2->ys;
			VBuffer[2].rhw = vtx2->rhw;
			VBuffer[2].g = (float)vtx2->g;
			VBuffer[2].u = (float)uv[2].u * vtx2->rhw;
			VBuffer[2].v = (float)uv[2].v * vtx2->rhw;

			VBuffer[3].x = vtx3->xs;
			VBuffer[3].y = vtx3->ys;
			VBuffer[3].rhw = vtx3->rhw;
			VBuffer[3].g = (float)vtx3->g;
			VBuffer[3].u = (float)uv[3].u * vtx3->rhw;
			VBuffer[3].v = (float)uv[3].v * vtx3->rhw;
		}
		else {
			if (!visible_zclip(vtx0, vtx1, vtx2))
				continue;

			points[0].xv = vtx0->xv;
			points[0].yv = vtx0->yv;
			points[0].zv = vtx0->zv;
			points[0].rhw = vtx0->rhw;
			points[0].xs = vtx0->xs;
			points[0].ys = vtx0->ys;
			points[0].u = (float)uv[0].u;
			points[0].v = (float)uv[0].v;
			points[0].g = (float)vtx0->g;

			points[1].yv = vtx1->yv;
			points[1].xv = vtx1->xv;
			points[1].zv = vtx1->zv;
			points[1].rhw = vtx1->rhw;
			points[1].xs = vtx1->xs;
			points[1].ys = vtx1->ys;
			points[1].u = (float)uv[1].u;
			points[1].v = (float)uv[1].v;
			points[1].g = (float)vtx1->g;

			points[2].xv = vtx2->xv;
			points[2].yv = vtx2->yv;
			points[2].zv = vtx2->zv;
			points[2].rhw = vtx2->rhw;
			points[2].xs = vtx2->xs;
			points[2].ys = vtx2->ys;
			points[2].u = (float)uv[2].u;
			points[2].v = (float)uv[2].v;
			points[2].g = (float)vtx2->g;

			points[3].xv = vtx3->xv;
			points[3].yv = vtx3->yv;
			points[3].zv = vtx3->zv;
			points[3].rhw = vtx3->rhw;
			points[3].xs = vtx3->xs;
			points[3].ys = vtx3->ys;
			points[3].u = (float)uv[3].u;
			points[3].v = (float)uv[3].v;
			points[3].g = (float)vtx3->g;

			nPoints = ZedClipper(nPoints, points, VBuffer);
			if (nPoints == 0) continue;
		}

		nPoints = XYGUVClipper(nPoints, VBuffer);
		if (nPoints == 0) continue;

		zv = CalculatePolyZ(sortType, vtx0->zv, vtx1->zv, vtx2->zv, vtx3->zv);
		Sort3dPtr->_0 = (DWORD)Info3dPtr;
		Sort3dPtr->_1 = MAKE_ZSORT(zv);
		++Sort3dPtr;

		if (zv >= (double)PerspectiveDistance) {
			*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap : POLY_WGTmap;
			*Info3dPtr++ = texture->tpage;
			*Info3dPtr++ = nPoints;

			for (j = 0; j < nPoints; ++j) {
				*Info3dPtr++ = (int)VBuffer[j].x;
				*Info3dPtr++ = (int)VBuffer[j].y;
				*Info3dPtr++ = (int)VBuffer[j].g;
				*Info3dPtr++ = (int)(VBuffer[j].u / VBuffer[j].rhw);
				*Info3dPtr++ = (int)(VBuffer[j].v / VBuffer[j].rhw);
			}
		}
		else {
			*Info3dPtr++ = (texture->drawtype == DRAW_Opaque) ? POLY_GTmap_persp : POLY_WGTmap_persp;
			*Info3dPtr++ = texture->tpage;
			*Info3dPtr++ = nPoints;

			for (j = 0; j < nPoints; ++j) {
				*Info3dPtr++ = (int)VBuffer[j].x;
				*Info3dPtr++ = (int)VBuffer[j].y;
				*Info3dPtr++ = (int)VBuffer[j].g;
				*(float*)Info3dPtr = VBuffer[j].rhw;
				Info3dPtr += sizeof(float) / sizeof(short);
				*(float*)Info3dPtr = VBuffer[j].u;
				Info3dPtr += sizeof(float) / sizeof(short);
				*(float*)Info3dPtr = VBuffer[j].v;
				Info3dPtr += sizeof(float) / sizeof(short);
			}
		}
		++SurfaceCount;
	}
}

void InsertRoomGT4_ZBuffered(FACE4* ptrObj, int number, SORTTYPE sortType)
{
	for (int i = 0; i < number; ++i) {
		if (HWR_VertexBufferFull()) break;
		FACE4* face = &ptrObj[i];
		PHD_VBUF* vtx0 = &PhdVBuf[face->vertices[0]];
		PHD_VBUF* vtx1 = &PhdVBuf[face->vertices[1]];
		PHD_VBUF* vtx2 = &PhdVBuf[face->vertices[2]];
		PHD_VBUF* vtx3 = &PhdVBuf[face->vertices[3]];
		PHD_TEXTURE* texture = &PhdTextureInfo[face->texture];
		if (texture->drawtype != DRAW_Opaque)
			InsertGT4_Sorted(vtx0, vtx1, vtx2, vtx3, texture, sortType);
		else
			InsertGT4_ZBuffered(vtx0, vtx1, vtx2, vtx3, texture);
	}
}

void InsertRoomGT3_ZBuffered(FACE3* ptrObj, int number, SORTTYPE sortType)
{
	for (int i = 0; i < number; ++i) {
		if (HWR_VertexBufferFull()) break;
		FACE3* face = &ptrObj[i];
		PHD_VBUF* vtx0 = &PhdVBuf[face->vertices[0]];
		PHD_VBUF* vtx1 = &PhdVBuf[face->vertices[1]];
		PHD_VBUF* vtx2 = &PhdVBuf[face->vertices[2]];
		PHD_TEXTURE* texture = &PhdTextureInfo[face->texture];
		PHD_UV* uv = texture->uv;
		if (texture->drawtype != DRAW_Opaque)
			InsertGT3_Sorted(vtx0, vtx1, vtx2, texture, &uv[0], &uv[1], &uv[2], sortType);
		else
			InsertGT3_ZBuffered(vtx0, vtx1, vtx2, texture, &uv[0], &uv[1], &uv[2]);
	}
}

void InsertRoomGT4_Sorted(FACE4* ptrObj, int number, SORTTYPE sortType)
{
	for (int i = 0; i < number; ++i)
	{
		if (HWR_VertexBufferFull()) break;
		FACE4* face = &ptrObj[i];
		PHD_VBUF* vtx0 = &PhdVBuf[face->vertices[0]];
		PHD_VBUF* vtx1 = &PhdVBuf[face->vertices[1]];
		PHD_VBUF* vtx2 = &PhdVBuf[face->vertices[2]];
		PHD_VBUF* vtx3 = &PhdVBuf[face->vertices[3]];
		PHD_TEXTURE* texture = &PhdTextureInfo[face->texture];
		InsertGT4_Sorted(vtx0, vtx1, vtx2, vtx3, texture, sortType);
	}
}

void InsertRoomGT3_Sorted(FACE3* ptrObj, int number, SORTTYPE sortType)
{
	for (int i = 0; i < number; ++i)
	{
		if (HWR_VertexBufferFull()) continue;
		FACE3* face = &ptrObj[i];
		PHD_VBUF* vtx0 = &PhdVBuf[face->vertices[0]];
		PHD_VBUF* vtx1 = &PhdVBuf[face->vertices[1]];
		PHD_VBUF* vtx2 = &PhdVBuf[face->vertices[2]];
		PHD_TEXTURE* texture = &PhdTextureInfo[face->texture];
		PHD_UV* uv = texture->uv;
		InsertGT3_Sorted(vtx0, vtx1, vtx2, texture, &uv[0], &uv[1], &uv[2], sortType);
	}
}
