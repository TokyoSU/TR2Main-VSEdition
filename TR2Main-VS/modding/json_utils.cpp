/*
 * Copyright (c) 2017-2024 Michael Chaban. All rights reserved.
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
#include "precompiled.h"
#include "json_utils.h"

double GetValueByNameDouble(Value& data, LPCSTR name, double defaultValue)
{
    if (data.HasMember(name))
        return data[name].GetDouble();
    return defaultValue;
}

D3DCOLOR GetColorRGBByName(Value& data, LPCSTR name, D3DCOLOR defaultValue)
{
    if (data.HasMember(name) && data[name].GetStringLength() == 6)
        return strtol(data[name].GetString(), NULL, 16);
    return defaultValue;
}

bool GetValueByNameBool(Value& data, LPCSTR name, bool defaultValue)
{
    if (data.HasMember(name))
        return data[name].GetBool();
    return defaultValue;
}

std::string GetValueByNameString(Value& data, LPCSTR name, SizeType* destStringSize, std::string defaultValue)
{
    if (data.HasMember(name))
    {
        *destStringSize = data[name].GetStringLength();
        return data[name].GetString();
    }
    *destStringSize = 0;
    return defaultValue;
}

void ParseJsonError(LPCSTR filePath, size_t line, int parseCode) {
    std::string str = "Failed to load json";

    str.append(", Filename: "); str.append(filePath);
    str.append(", Line: "); str.append(std::to_string(line));

    str.append(", Error: ");
    switch (parseCode)
    {
    case kParseErrorDocumentEmpty:
        str.append("The document is empty.");
        break;
    case kParseErrorDocumentRootNotSingular:
        str.append("The document data must not follow by other values.");
        break;
    case kParseErrorValueInvalid:
        str.append("Invalid value.");
        break;
    case kParseErrorObjectMissName:
        str.append("Missing a name for object member. (Or a ',' is not followed by any object)");
        break;
    case kParseErrorObjectMissColon:
        str.append("Missing a colon after a name of object member.");
        break;
    case kParseErrorObjectMissCommaOrCurlyBracket:
        str.append("Missing a comma or '}' after an object member.");
        break;
    case kParseErrorArrayMissCommaOrSquareBracket:
        str.append("Missing a comma or ']' after an array element.");
        break;
    case kParseErrorStringUnicodeEscapeInvalidHex:
        str.append("Incorrect hex digit after \\u escape in string.");
        break;
    case kParseErrorStringUnicodeSurrogateInvalid:
        str.append("The surrogate pair in string is invalid.");
        break;
    case kParseErrorStringEscapeInvalid:
        str.append("Invalid escape character in string.");
        break;
    case kParseErrorStringMissQuotationMark:
        str.append("Missing a closing quotation mark in string.");
        break;
    case kParseErrorStringInvalidEncoding:
        str.append("Invalid encoding in string.");
        break;
    case kParseErrorNumberTooBig:
        str.append("Number too big to be stored in double.");
        break;
    case kParseErrorNumberMissFraction:
        str.append("Miss fraction part in number.");
        break;
    case kParseErrorNumberMissExponent:
        str.append("Miss exponent in number.");
        break;
    case kParseErrorTermination:
        str.append("Parsing was terminated.");
        break;
    case kParseErrorUnspecificSyntaxError:
        str.append("Unspecific syntax error.");
        break;
    default:
        str.append("Unknown error !");
        break;
    }

    LogWarn(str.c_str());
}
