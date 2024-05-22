@echo off
echo ========================================
echo Installing X86 Windows Library
echo ========================================
vcpkg install mimalloc[core,override,secure]:x86-windows
vcpkg install dxsdk-d3dx:x86-windows
vcpkg install sdl2[core]:x86-windows
vcpkg install sdl2-mixer-ext[core,libflac,mpg123,wavpack]:x86-windows
vcpkg install spdlog:x86-windows
vcpkg install rapidjson:x86-windows
echo ========================================
echo Installing X64 Windows Library
echo ========================================
vcpkg install mimalloc[core,override,secure]:x64-windows
vcpkg install dxsdk-d3dx:x64-windows
vcpkg install sdl2[core]:x64-windows
vcpkg install sdl2-mixer-ext[core,libflac,mpg123,wavpack]:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install rapidjson:x64-windows
pause