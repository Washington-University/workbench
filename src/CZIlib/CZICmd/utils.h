//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#pragma once

#include <string>
#include <vector>
#include "inc_libCZI.h"

std::string convertToUtf8(const std::wstring& str);
std::wstring convertUtf8ToUCS2(const std::string& str);

std::string trim(const std::string& str, const std::string& whitespace = " \t");
std::wstring trim(const std::wstring& str, const std::wstring& whitespace = L" \t");
bool icasecmp(const std::string& l, const std::string& r);
bool icasecmp(const std::wstring& l, const std::wstring& r);
bool __wcasecmp(const wchar_t* l, const wchar_t* r);
std::uint8_t HexCharToInt(char c);
bool ConvertHexStringToInteger(const char* cp, std::uint32_t* value);
char LowerNibbleToHexChar(std::uint8_t v);
char UpperNibbleToHexChar(std::uint8_t v);

std::string BytesToHexString(const std::uint8_t* ptr, size_t size);
std::wstring BytesToHexWString(const std::uint8_t* ptr, size_t size);

std::vector<std::wstring> wrap(const wchar_t* text, size_t line_length/* = 72*/);

const wchar_t* skipWhiteSpaceAndOneOfThese(const wchar_t* s, const wchar_t* charToSkipOnce);

std::ostream& operator<<(std::ostream& os, const GUID& guid); 