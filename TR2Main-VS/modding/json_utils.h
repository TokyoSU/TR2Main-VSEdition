#pragma once

template<typename T>
static T GetValueByNameInt(Value& data, LPCSTR name, T defaultValue) {
	if (data.HasMember(name))
		return static_cast<T>(data[name].GetInt());
	return defaultValue;
}
double GetValueByNameDouble(Value& data, LPCSTR name, double defaultValue);
extern D3DCOLOR GetColorByName(Value& data, LPCSTR name, D3DCOLOR defaultValue);
extern bool GetValueByNameBool(Value& data, LPCSTR name, bool defaultValue);
extern LPCSTR GetValueByNameString(Value& data, LPCSTR name, SizeType* destStringSize, LPCSTR defaultValue);
extern void ParseJsonError(LPCSTR filePath, size_t line, int parseCode);
