# TR2Main
Original repositories: https://github.com/Arsunt/TR2Main

This is an open source implementation of the classic Tomb Raider II game (1997), made by reverse engineering of the original game for PC and PS1. Right now it's a dynamic library injecting the original game EXE to allow step-by-step decompilation, fixing, and improvement, but someday it will become single EXE file.

The goal of the project is to make a clone of the original PC game (Not 1:1), to fix all its bugs, and to add more features from non-PC official ports (for example from PS1) and through port of other tomb raider games. So TR2Main is some sort of unofficial modern PC port of Tomb Raider II.

## Getting Started

You may download patched Tomb2.exe, TR2Main.dll from [releases](https://github.com/TokyoSU/TR2Main-VSEdition/releases), put them in your game folder and just launch. Also you may build TR2Main.dll yourself and use it with patched Tomb2.exe from the [releases](https://github.com/TokyoSU/TR2Main-VSEdition/releases).

The Golden Mask game is included into a [release pack](https://github.com/TokyoSU/TR2Main-VSEdition/releases), since it's freeware. The original game level files must be officially obtained via Steam/GOG/CD/whatever.

## Changelog

Learn about [the latest improvements](CHANGELOG.md).

## Built With

* [Visual studio 2022](https://visualstudio.microsoft.com/)
* [vcpkg](https://github.com/microsoft/vcpkg) (Required to use this project)
1) `vcpkg integrate install` (required for using vcpkg with visual studio)
2) use packages_installer.bat to install all packages !

* Next you need to download [DX7_SDK](https://archive.org/details/dx7sdk-7001) (if your planning to use the DirectX5)
1) then extract it to somewhere you want. (you can rename the folder to DX7_SDK after extraction)
2) go to visual studio (open the project if it's not already opened).
3) go to the project properties, linker -> general then look for "Additional library directories".
4) change the configuration to "Debug" then copy the path from DX7_SDK\lib to "Additional library directories" box on the right (replacing the path i set).
5) do the same for "Release".
6) done.

* If you want to debug the game:
1) go to visual studio (open the project if it's not already opened).
2) go to your project properties.
3) go to "Debugging".
4) change the "Working directory" to your game path (don't forget the \ at the end of the path if it not exist).
5) done.

## Authors

* **Michael Chaban** \([Arsunt](https://github.com/Arsunt)\). Author of the project. E-mail: <arsunt@gmail.com>
* [**ChocolateFan**](https://github.com/asasas9500). Decompilation.
* [**TokyoSU**](https://github.com/TokyoSU). Decompilation and features development and also the Visual studio edition.
* **PaulD**. Author of [CD audio solution](modding/cd_pauld.cpp) used now by Steam.
* [**Lito Perezito**](https://litoperezito.com). The artist who created loading screens for The Golden Mask.

## License

This project is licensed under the GNU General Public License - see the [COPYING.md](COPYING.md) file for details

## Acknowledgments

If you are inspired by my project and decided to borrow some of the ideas found in the code, do not forget provide a link to this project. It took me years to decompile and understand the details required for this project.

## Copyright
(c) 2017-2023 Michael Chaban. All rights reserved.
Original game is created by Core Design Ltd. in 1997.
Lara Croft and Tomb Raider are trademarks of Embracer Group AB.
