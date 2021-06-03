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

#include "stdafx.h"
#include "utils.h"
#include <cwctype>
#include <iomanip>

#if defined(WIN32ENV)
#define HAS_CODECVT
#endif

#if defined(HAS_CODECVT)
#include <locale>
#include <codecvt>
#include <stdlib.h>
#endif

using namespace std;

std::string convertToUtf8(const std::wstring& str)
{
#if defined(HAS_CODECVT)
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	std::string conv = utf8_conv.to_bytes(str);
	return conv;
#else
	size_t requiredSize = std::wcstombs(nullptr, str.c_str(), 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], str.c_str(), requiredSize));
	return conv;
#endif
}

std::wstring convertUtf8ToUCS2(const std::string& str)
{
#if defined(HAS_CODECVT)
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
	std::wstring conv = utf8conv.from_bytes(str);
	return conv;
#else
	std::wstring conv(str.size(), 0);
	size_t size = std::mbstowcs(&conv[0], str.c_str(), str.size());
	conv.resize(size);
	/*printf(str.c_str());
	printf("\n");
	wprintf(L"ÄÜÖ");
	printf(convertToUtf8(conv).c_str());
	printf("\n");*/
	return conv;
#endif
}

std::string trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::wstring trim(const std::wstring& str, const std::wstring& whitespace /*= " \t"*/)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::wstring::npos)
		return L""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

bool icasecmp(const std::string& l, const std::string& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::string::value_type l1, std::string::value_type r1)
	{ return toupper(l1) == toupper(r1); });
}

bool icasecmp(const std::wstring& l, const std::wstring& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::wstring::value_type l1, std::wstring::value_type r1)
	{ return towupper(l1) == towupper(r1); });
}

bool __wcasecmp(const wchar_t* l, const wchar_t* r)
{
#if defined(_WIN32)
	return _wcsicmp(l, r) == 0 ? true : false;
#else
	return wcscasecmp(l, r) == 0 ? true : false;
#endif
}

std::uint8_t HexCharToInt(char c)
{
	switch (c)
	{
	case '0':return 0;
	case '1':return 1;
	case '2':return 2;
	case '3':return 3;
	case '4':return 4;
	case '5':return 5;
	case '6':return 6;
	case '7':return 7;
	case '8':return 8;
	case '9':return 9;
	case 'A':case 'a':return 10;
	case 'B':case 'b':return 11;
	case 'C':case 'c':return 12;
	case 'D':case 'd':return 13;
	case 'E':case 'e':return 14;
	case 'F':case 'f':return 15;
	}

	return 0xff;
}

bool ConvertHexStringToInteger(const char* cp, std::uint32_t* value)
{
	if (*cp=='\0')
	{
		return false;
	}

	std::uint32_t v = 0;
	int cntOfSignificantDigits = 0;
	for (; *cp != '\0'; ++cp)
	{
		std::uint8_t x = HexCharToInt(*cp);
		if (x == 0xff)
		{
			return false;
		}

		if (v > 0)
		{
			if (++cntOfSignificantDigits > 7)
			{
				return false;
			}
		}

		v = v * 16 + x;
	}

	if (value != nullptr)
	{
		*value = v;
	}

	return true;
}

char LowerNibbleToHexChar(std::uint8_t v)
{
	static char Hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	return Hex[v & 0xf];
}
char UpperNibbleToHexChar(std::uint8_t v)
{
	return LowerNibbleToHexChar(v >> 4);
}

std::string BytesToHexString(const std::uint8_t* ptr, size_t size)
{
	std::string s;
	s.reserve(size * 2);
	for (size_t i = 0; i < size; ++i)
	{
		s.push_back(UpperNibbleToHexChar(ptr[i]));
		s.push_back(LowerNibbleToHexChar(ptr[i]));
	}

	return s;
}

std::wstring BytesToHexWString(const std::uint8_t* ptr, size_t size)
{
	std::wstring s;
	s.reserve(size * 2);
	for (size_t i = 0; i < size; ++i)
	{
		s.push_back(UpperNibbleToHexChar(ptr[i]));
		s.push_back(LowerNibbleToHexChar(ptr[i]));
	}

	return s;
}

std::vector<std::wstring> wrap(const wchar_t* text, size_t line_length/* = 72*/)
{
	wistringstream iss(text);
	std::wstring word, line;
	std::vector<std::wstring> vec;

	for (; ;)
	{
		iss >> word;
		if (!iss)
		{
			break;
		}

		if (word.size() > 2 && word[0] == L'\\'&&word[1] == L'n')
		{
			line.erase(line.size() - 1);	// remove trailing space
			vec.push_back(line);
			line.clear();
			word.erase(0, 2);
		}

		if (line.length() + word.length() > line_length)
		{
			line.erase(line.size() - 1);
			vec.push_back(line);
			line.clear();
		}

		line += word + L' ';
	}

	if (!line.empty())
	{
		line.erase(line.size() - 1);
		vec.push_back(line);
	}

	return vec;
}

const wchar_t* skipWhiteSpaceAndOneOfThese(const wchar_t* s, const wchar_t* charsToSkipOnce)
{
	bool delimiteralreadyskipped = false;
	for (; *s != L'\0'; ++s)
	{
		if (iswspace(*s))
		{
			continue;
		}

		if (delimiteralreadyskipped == false && charsToSkipOnce != nullptr && std::wcschr(charsToSkipOnce, *s) != nullptr)
		{
			delimiteralreadyskipped = true;
			continue;
		}

		return s;
	}

	return s;
}

std::ostream& operator<<(std::ostream& os, const GUID& guid)
{
	os << std::uppercase;
	os.width(8);
	os.fill('0');
	os << std::hex << guid.Data1 << '-';

	os.width(4);
	os << std::hex << guid.Data2 << '-';

	os.width(4);
	os << std::hex << guid.Data3 << '-';

	os << std::hex
		<< std::setw(2) << static_cast<short>(guid.Data4[0])
		<< std::setw(2) << static_cast<short>(guid.Data4[1])
		<< '-'
		<< std::setw(2) << static_cast<short>(guid.Data4[2])
		<< std::setw(2) << static_cast<short>(guid.Data4[3])
		<< std::setw(2) << static_cast<short>(guid.Data4[4])
		<< std::setw(2) << static_cast<short>(guid.Data4[5])
		<< std::setw(2) << static_cast<short>(guid.Data4[6])
		<< std::setw(2) << static_cast<short>(guid.Data4[7]);
	os << std::nouppercase;
	return os;
}