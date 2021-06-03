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
#include "utilities.h"

/*static*/std::uint8_t Utilities::HexCharToInt(char c)
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

template <class tString>
tString trimImpl(const tString& str, const tString& whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == tString::npos)
		return tString(); // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

/*static*/std::string Utilities::Trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
	return trimImpl(str, whitespace);
}

/*static*/std::wstring Utilities::Trim(const std::wstring& str, const std::wstring& whitespace /*= L" \t"*/)
{
	return trimImpl(str, whitespace);
}


/*static*/bool Utilities::icasecmp(const std::string& l, const std::string& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::string::value_type l1, std::string::value_type r1)
	{ return toupper(l1) == toupper(r1); });
}

/*static*/bool Utilities::icasecmp(const std::wstring& l, const std::wstring& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::wstring::value_type l1, std::wstring::value_type r1)
	{ return towupper(l1) == towupper(r1); });
}
