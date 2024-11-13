#pragma once

template<typename T>
static T GetValueByNameInt(Value& data, LPCSTR name, T defaultValue) {
	if (data.HasMember(name))
		return static_cast<T>(data[name].GetInt());
	return defaultValue;
}
double GetValueByNameDouble(Value& data, LPCSTR name, double defaultValue);
extern D3DCOLOR GetColorRGBByName(Value& data, LPCSTR name, D3DCOLOR defaultValue);
extern bool GetValueByNameBool(Value& data, LPCSTR name, bool defaultValue);
extern std::string GetValueByNameString(Value& data, LPCSTR name, SizeType* destStringSize, std::string defaultValue);
extern void ParseJsonError(LPCSTR filePath, size_t line, int parseCode);
