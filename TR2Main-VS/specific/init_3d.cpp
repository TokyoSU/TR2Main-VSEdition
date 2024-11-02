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
#include "specific/init_3d.h"
#include "global/vars.h"

LPDIRECT3DVERTEXBUFFER9 D3DVtx = NULL;

bool D3DCreate() {
	D3D = Direct3DCreate9(D3D_SDK_VERSION);
	return (D3D != NULL);
}

void D3DRelease() {
	if (D3D) {
		D3D->Release();
		D3D = NULL;
	}
}

void D3DDeviceCreate(LPDDS lpBackBuffer) {
	if (D3D == NULL && !D3DCreate())
		throw ERR_D3D_Create;

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = HGameWindow;
	d3dpp.EnableAutoDepthStencil = TRUE;

	D3DCAPS9* caps = &SavedAppSettings.PreferredDisplayAdapter->body.caps;
	if SUCCEEDED(D3D->CheckDepthStencilMatch(caps->AdapterOrdinal, caps->DeviceType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D16)) {
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else if SUCCEEDED(D3D->CheckDepthStencilMatch(caps->AdapterOrdinal, caps->DeviceType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_D24X8)) {
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	}
	else {
		throw ERR_CreateDevice;
	}

	d3dpp.Windowed = !SavedAppSettings.FullScreen;
	if (SavedAppSettings.FullScreen) {
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferWidth = FullScreenWidth;
		d3dpp.BackBufferHeight = FullScreenHeight;
	}

	if (D3DDev) {
		if (D3DVtx != NULL) {
			D3DVtx->Release();
			D3DVtx = NULL;
		}
		HRESULT res = D3D_OK;
		do {
			res = D3DDev->TestCooperativeLevel();
		} while (res == D3DERR_DEVICELOST);
		if ((res != D3D_OK && res != D3DERR_DEVICENOTRESET) || FAILED(D3DDev->Reset(&d3dpp))) {
			throw ERR_CreateDevice;
		}
	}
	else {
		DWORD flags = CHK_ANY(caps->DevCaps, D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		if FAILED(D3D->CreateDevice(CurrentDisplayAdapter.index, D3DDEVTYPE_HAL, HGameWindow, flags, &d3dpp, &D3DDev)) {
			throw ERR_CreateDevice;
		}
	}

	if (!D3DVtx && FAILED(D3DDev->CreateVertexBuffer(sizeof(D3DTLVERTEX) * VTXBUF_LEN, D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC, D3DFVF_TLVERTEX, D3DPOOL_DEFAULT, &D3DVtx, NULL)))
		throw ERR_CreateDevice;

	D3DDev->SetStreamSource(0, D3DVtx, 0, sizeof(D3DTLVERTEX));
	D3DDev->SetFVF(D3DFVF_TLVERTEX);
}

void Direct3DRelease() {
	if (D3DVtx != NULL) {
		D3DVtx->Release();
		D3DVtx = NULL;
	}
	if (D3DDev != NULL) {
		D3DDev->Release();
		D3DDev = NULL;
	}
	D3DRelease();
}

bool Direct3DInit() {
	return true;
}

/*
 * Inject function
 */
void Inject_Init3d() {
	INJECT(0x00444620, D3DCreate);
	INJECT(0x00444640, D3DRelease);
	INJECT(0x00444820, D3DDeviceCreate);
	INJECT(0x004449E0, Direct3DRelease);
	INJECT(0x00444A30, Direct3DInit);
}