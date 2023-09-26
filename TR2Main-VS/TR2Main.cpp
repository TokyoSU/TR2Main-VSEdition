/*
 * Copyright (c) 2017-2018 Michael Chaban. All rights reserved.
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

 /**
  * @file
  * @brief TR2Main interface
  *
  * This file implements TR2Main.DLL exported functions and interfaces
  */

  /**
   * @defgroup TR2MAIN TR2Main
   * @brief TR2Main interface
   *
   * This module contains TR2Main.DLL exported functions and interfaces
   *
   * @{
   */

#include "precompiled.h"
#include "resource.h"

/** @cond Doxygen_Suppress */
#ifdef TR2MAINVS_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
/** @endcond */

HINSTANCE hInstance = NULL;

// 3d system
extern void Inject_3Dgen();
extern void Inject_3Dout();
extern void Inject_3Dinsert();
extern void Inject_PhdMath();
extern void Inject_ScaleSpr();
// game
extern void Inject_Bird();
extern void Inject_Boat();
extern void Inject_Box();
extern void Inject_Camera();
extern void Inject_Cinema();
extern void Inject_Collide();
extern void Inject_Control();
extern void Inject_Demo();
extern void Inject_Diver();
extern void Inject_Dog();
extern void Inject_Dragon();
extern void Inject_Draw();
extern void Inject_Eel();
extern void Inject_Effects();
extern void Inject_Enemies();
extern void Inject_Gameflow();
extern void Inject_Hair();
extern void Inject_Health();
extern void Inject_Inventory();
extern void Inject_InvFunc();
extern void Inject_InvText();
extern void Inject_Items();
extern void Inject_Lara();
extern void Inject_Lara1Gun();
extern void Inject_Lara2Gun();
extern void Inject_LaraClimb();
extern void Inject_LaraFire();
extern void Inject_LaraFlare();
extern void Inject_LaraMisc();
extern void Inject_LaraSurf();
extern void Inject_LaraSwim();
extern void Inject_Lot();
extern void Inject_Missile();
extern void Inject_MoveBlock();
extern void Inject_Objects();
extern void Inject_People();
extern void Inject_Pickup();
extern void Inject_Rat();
extern void Inject_SaveGame();
extern void Inject_Setup();
extern void Inject_Shark();
extern void Inject_Skidoo();
extern void Inject_Sound();
extern void Inject_Sphere();
extern void Inject_Spider();
extern void Inject_Text();
extern void Inject_Traps();
extern void Inject_Yeti();
// specific
extern void Inject_Background();
extern void Inject_Display();
extern void Inject_File();
extern void Inject_Fmv();
extern void Inject_Frontend();
extern void Inject_Game();
extern void Inject_HWR();
extern void Inject_Init();
extern void Inject_Init3d();
extern void Inject_InitDisplay();
extern void Inject_InitInput();
extern void Inject_InitSound();
extern void Inject_Input();
extern void Inject_Option();
extern void Inject_Output();
extern void Inject_Registry();
extern void Inject_Screenshot();
extern void Inject_SetupDlg();
extern void Inject_SetupWnd();
extern void Inject_SMain();
extern void Inject_SndPC();
extern void Inject_Texture();
extern void Inject_Utils();
extern void Inject_WinMain();
extern void Inject_WinVid();

static void Inject() {
	LogDebug("Injecting new functions from DLL to EXE.");
	LogDebug("===================================================");
	// 3d system
	LogDebug("Starting 3DSystem Class.");
	Inject_3Dgen(); LogDebug("- Injected 3DGen");
	Inject_3Dout(); LogDebug("- Injected 3DOut");
	Inject_3Dinsert(); LogDebug("- Injected 3DInsert");
	Inject_PhdMath(); LogDebug("- Injected PhdMath");
	Inject_ScaleSpr(); LogDebug("- Injected ScaleSpr");
	// game
	LogDebug("Starting Game Class.");
	Inject_Bird(); LogDebug("- Injected Bird");
	Inject_Boat(); LogDebug("- Injected Boat");
	Inject_Box(); LogDebug("- Injected Box");
	Inject_Camera(); LogDebug("- Injected Camera");
	Inject_Cinema(); LogDebug("- Injected Cinema");
	Inject_Collide(); LogDebug("- Injected Collide");
	Inject_Control(); LogDebug("- Injected Control");
	Inject_Demo(); LogDebug("- Injected Demo");
	Inject_Diver(); LogDebug("- Injected Diver");
	Inject_Dog(); LogDebug("- Injected Dog");
	Inject_Dragon(); LogDebug("- Injected Dragon");
	Inject_Draw(); LogDebug("- Injected Draw");
	Inject_Eel(); LogDebug("- Injected Eel");
	Inject_Effects(); LogDebug("- Injected Effects");
	Inject_Enemies(); LogDebug("- Injected Enemies");
	Inject_Gameflow(); LogDebug("- Injected Gameflow");
	Inject_Hair(); LogDebug("- Injected Hair");
	Inject_Health(); LogDebug("- Injected Health");
	Inject_Inventory(); LogDebug("- Injected Inventory");
	Inject_InvFunc(); LogDebug("- Injected InvFunc");
	Inject_InvText(); LogDebug("- Injected InvText");
	Inject_Items(); LogDebug("- Injected Items");
	Inject_Lara(); LogDebug("- Injected Lara");
	Inject_Lara1Gun(); LogDebug("- Injected Lara1Gun");
	Inject_Lara2Gun(); LogDebug("- Injected Lara2Gun");
	Inject_LaraClimb(); LogDebug("- Injected LaraClimb");
	Inject_LaraFire(); LogDebug("- Injected LaraFire");
	Inject_LaraFlare(); LogDebug("- Injected LaraFlare");
	Inject_LaraMisc(); LogDebug("- Injected LaraMisc");
	Inject_LaraSurf(); LogDebug("- Injected LaraSurf");
	Inject_LaraSwim(); LogDebug("- Injected LaraSwim");
	Inject_Lot(); LogDebug("- Injected Lot");
	Inject_Missile(); LogDebug("- Injected Missile");
	Inject_MoveBlock(); LogDebug("- Injected MoveBlock");
	Inject_Objects(); LogDebug("- Injected Objects");
	Inject_People(); LogDebug("- Injected People");
	Inject_Pickup(); LogDebug("- Injected Pickup");
	Inject_Rat(); LogDebug("- Injected Rat");
	Inject_SaveGame(); LogDebug("- Injected SaveGame");
	Inject_Setup(); LogDebug("- Injected Setup");
	Inject_Shark(); LogDebug("- Injected Shark");
	Inject_Skidoo(); LogDebug("- Injected Skidoo");
	Inject_Sound(); LogDebug("- Injected Sound");
	Inject_Sphere(); LogDebug("- Injected Sphere");
	Inject_Spider(); LogDebug("- Injected Spider");
	Inject_Text(); LogDebug("- Injected Text");
	Inject_Traps(); LogDebug("- Injected Traps");
	Inject_Yeti(); LogDebug("- Injected Yeti");
	// specific
	LogDebug("Starting Specific Class.");
	Inject_Background(); LogDebug("- Injected Background");
	Inject_Display(); LogDebug("- Injected Display");
	Inject_File(); LogDebug("- Injected File");
	Inject_Fmv(); LogDebug("- Injected Fmv");
	Inject_Frontend(); LogDebug("- Injected Frontend");
	Inject_Game(); LogDebug("- Injected Game");
	Inject_HWR(); LogDebug("- Injected Hwr");
	Inject_Init(); LogDebug("- Injected Init");
	Inject_Init3d(); LogDebug("- Injected Init3D");
	Inject_InitDisplay(); LogDebug("- Injected InitDisplay");
	Inject_InitInput(); LogDebug("- Injected InitInput");
	Inject_InitSound(); LogDebug("- Injected InitSound");
	Inject_Input(); LogDebug("- Injected Input");
	Inject_Option(); LogDebug("- Injected Option");
	Inject_Output(); LogDebug("- Injected Output");
	Inject_Registry(); LogDebug("- Injected Registry");
	Inject_Screenshot(); LogDebug("- Injected ScreenShot");
	Inject_SetupDlg(); LogDebug("- Injected SetupDlg");
	Inject_SetupWnd(); LogDebug("- Injected SetupWnd");
	Inject_SMain(); LogDebug("- Injected SMain");
	Inject_SndPC(); LogDebug("- Injected SndPC");
	Inject_Texture(); LogDebug("- Injected Texture");
	Inject_Utils(); LogDebug("- Injected Utils");
	Inject_WinMain(); LogDebug("- Injected WinMain");
	Inject_WinVid(); LogDebug("- Injected WinVid");
	LogDebug("===================================================");
	LogDebug("Finished injecting.");
}

extern "C" DLL_EXPORT int DummyFunction() {
	return 0;
}

/**
 * An optional entry point into a dynamic-link library (DLL)
 * @param[in] hinstDLL A handle to the DLL module
 * @param[in] fdwReason The reason code that indicates why the DLL
 * entry-point function is being called
 * @param[in] lpvReserved used if fdwReason is DLL_PROCESS_ATTACH or DLL_PROCESS_DETACH
 * @return TRUE if it succeeds or FALSE if it fails
 * @note See the MSDN for more information
 */
extern "C" BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	LogInit();
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		LogDebug("Process was attached.");
		LogDebug("DLL version: %s", VER_FULL);
		// attach to process
		hInstance = hinstDLL;
		Inject();
		break;

	case DLL_PROCESS_DETACH:
		// detach from process
		LogDebug("Process was detached.");
		break;

	case DLL_THREAD_ATTACH:
		// attach to thread
		break;

	case DLL_THREAD_DETACH:
		// detach from thread
		break;
	}
	return TRUE; // successful
}

/** @} */