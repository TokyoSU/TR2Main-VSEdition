#pragma once
#include "../global/types.h"

extern BYTE GetValueByNameByte(Value& data, const std::string& name, BYTE defaultValue);
extern short GetValueByNameShort(Value& data, const std::string& name, short defaultValue);
extern int GetValueByNameInt(Value& data, const std::string& name, int defaultValue);
extern double GetValueByNameDouble(Value& data, const std::string& name, double defaultValue);
extern D3DCOLOR GetColorRGBByName(Value& data, const std::string& name, D3DCOLOR defaultValue);
extern bool GetValueByNameBool(Value& data, const std::string& name, bool defaultValue);
extern std::string GetValueByNameString(Value& data, const std::string& name, SizeType* destStringSize, std::string defaultValue);
extern void ParseJsonError(LPCSTR filePath, size_t line, int parseCode);
