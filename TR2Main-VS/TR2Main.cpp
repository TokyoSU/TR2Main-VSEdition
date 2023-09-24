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
#include <windows.h>

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
	Log("Injecting new functions from DLL to EXE.");
	Log("===================================================");
	// 3d system
	Log("Starting 3DSystem Class.");
	Inject_3Dgen(); Log("- Injected 3DGen");
	Inject_3Dout(); Log("- Injected 3DOut");
	Inject_3Dinsert(); Log("- Injected 3DInsert");
	Inject_PhdMath(); Log("- Injected PhdMath");
	Inject_ScaleSpr(); Log("- Injected ScaleSpr");
	// game
	Log("Starting Game Class.");
	Inject_Bird(); Log("- Injected Bird");
	Inject_Boat(); Log("- Injected Boat");
	Inject_Box(); Log("- Injected Box");
	Inject_Camera(); Log("- Injected Camera");
	Inject_Cinema(); Log("- Injected Cinema");
	Inject_Collide(); Log("- Injected Collide");
	Inject_Control(); Log("- Injected Control");
	Inject_Demo(); Log("- Injected Demo");
	Inject_Diver(); Log("- Injected Diver");
	Inject_Dog(); Log("- Injected Dog");
	Inject_Dragon(); Log("- Injected Dragon");
	Inject_Draw(); Log("- Injected Draw");
	Inject_Eel(); Log("- Injected Eel");
	Inject_Effects(); Log("- Injected Effects");
	Inject_Enemies(); Log("- Injected Enemies");
	Inject_Gameflow(); Log("- Injected Gameflow");
	Inject_Hair(); Log("- Injected Hair");
	Inject_Health(); Log("- Injected Health");
	Inject_Inventory(); Log("- Injected Inventory");
	Inject_InvFunc(); Log("- Injected InvFunc");
	Inject_InvText(); Log("- Injected InvText");
	Inject_Items(); Log("- Injected Items");
	Inject_Lara(); Log("- Injected Lara");
	Inject_Lara1Gun(); Log("- Injected Lara1Gun");
	Inject_Lara2Gun(); Log("- Injected Lara2Gun");
	Inject_LaraClimb(); Log("- Injected LaraClimb");
	Inject_LaraFire(); Log("- Injected LaraFire");
	Inject_LaraFlare(); Log("- Injected LaraFlare");
	Inject_LaraMisc(); Log("- Injected LaraMisc");
	Inject_LaraSurf(); Log("- Injected LaraSurf");
	Inject_LaraSwim(); Log("- Injected LaraSwim");
	Inject_Lot(); Log("- Injected Lot");
	Inject_Missile(); Log("- Injected Missile");
	Inject_MoveBlock(); Log("- Injected MoveBlock");
	Inject_Objects(); Log("- Injected Objects");
	Inject_People(); Log("- Injected People");
	Inject_Pickup(); Log("- Injected Pickup");
	Inject_Rat(); Log("- Injected Rat");
	Inject_SaveGame(); Log("- Injected SaveGame");
	Inject_Setup(); Log("- Injected Setup");
	Inject_Shark(); Log("- Injected Shark");
	Inject_Skidoo(); Log("- Injected Skidoo");
	Inject_Sound(); Log("- Injected Sound");
	Inject_Sphere(); Log("- Injected Sphere");
	Inject_Spider(); Log("- Injected Spider");
	Inject_Text(); Log("- Injected Text");
	Inject_Traps(); Log("- Injected Traps");
	Inject_Yeti(); Log("- Injected Yeti");
	// specific
	Log("Starting Specific Class.");
	Inject_Background(); Log("- Injected Background");
	Inject_Display(); Log("- Injected Display");
	Inject_File(); Log("- Injected File");
	Inject_Fmv(); Log("- Injected Fmv");
	Inject_Frontend(); Log("- Injected Frontend");
	Inject_Game(); Log("- Injected Game");
	Inject_HWR(); Log("- Injected Hwr");
	Inject_Init(); Log("- Injected Init");
	Inject_Init3d(); Log("- Injected Init3D");
	Inject_InitDisplay(); Log("- Injected InitDisplay");
	Inject_InitInput(); Log("- Injected InitInput");
	Inject_InitSound(); Log("- Injected InitSound");
	Inject_Input(); Log("- Injected Input");
	Inject_Option(); Log("- Injected Option");
	Inject_Output(); Log("- Injected Output");
	Inject_Registry(); Log("- Injected Registry");
	Inject_Screenshot(); Log("- Injected ScreenShot");
	Inject_SetupDlg(); Log("- Injected SetupDlg");
	Inject_SetupWnd(); Log("- Injected SetupWnd");
	Inject_SMain(); Log("- Injected SMain");
	Inject_SndPC(); Log("- Injected SndPC");
	Inject_Texture(); Log("- Injected Texture");
	Inject_Utils(); Log("- Injected Utils");
	Inject_WinMain(); Log("- Injected WinMain");
	Inject_WinVid(); Log("- Injected WinVid");
	Log("===================================================");
	Log("Finished injecting.");
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
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		Log("Process was attached.");
		// attach to process
		hInstance = hinstDLL;
		Inject();
		break;

	case DLL_PROCESS_DETACH:
		// detach from process
		Log("Process was detached.");
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