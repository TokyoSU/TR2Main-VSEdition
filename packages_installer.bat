@echo off
echo ========================================
echo Installing X86 Windows Library
echo ========================================
vcpkg install mimalloc[core,override,secure]:x86-windows
vcpkg install dxsdk-d3dx:x86-windows
vcpkg install spdlog:x86-windows
vcpkg install rapidjson:x86-windows
echo ========================================
echo Installing X86 Windows Library
echo ========================================
vcpkg install mimalloc[core,override,secure]:x64-windows
vcpkg install dxsdk-d3dx:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install rapidjson:x64-windows
pause