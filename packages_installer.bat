@echo off
echo ========================================
echo Installing X86 Windows Library
echo ========================================
vcpkg install spdlog:x86-windows-static
vcpkg install rapidjson:x86-windows-static
vcpkg install stb:x86-windows-static
pause