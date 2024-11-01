@echo off
echo ========================================
echo Installing X86 Windows Library
echo ========================================
vcpkg install spdlog:x86-windows-static
vcpkg install rapidjson:x86-windows-static
vcpkg install stb:x86-windows-static
echo ========================================
echo Installing X64 Windows Library
echo ========================================
vcpkg install spdlog:x64-windows-static
vcpkg install rapidjson:x64-windows-static
vcpkg install stb:x64-windows-static
pause