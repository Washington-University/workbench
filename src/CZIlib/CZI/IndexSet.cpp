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
#include "IndexSet.h"
#include "utilities.h"

using namespace std;
using namespace libCZI;

CIndexSet::CIndexSet(const std::wstring& str)
{
	this->ParseString(str);
}

/*virtual*/bool CIndexSet::IsContained(int index) const
{
	for (const auto i : this->intervals)
	{
		if (i.IsContained(index))
		{
			return true;
		}
	}

	return false;
}

void CIndexSet::ParseString(const std::wstring& str)
{
	std::vector<wstring> parts;
	Utilities::Split(str, L',', [&](const std::wstring p)->bool {parts.push_back(Utilities::Trim(p)); return true; });

	for (const auto& part : parts)
	{
		auto t = ParsePart(part);
		if (get<0>(t) > get<1>(t))
		{
			throw LibCZIStringParseException("The from-value is bigger than the to-value", -1, LibCZIStringParseException::ErrorType::FromGreaterThanTo);
		}

		this->AddInterval(get<0>(t), get<1>(t));
	}
}

void CIndexSet::AddInterval(int start, int end)
{
	// TODO: we could check if we can "simplify" the intervals, e. g. if we are given {0,2} and {2,4} we could turn it in
	//        one interval (->{0,4}) 
	this->intervals.emplace_back(interval{ start,end });
}

/*static*/std::tuple<int, int> CIndexSet::ParsePart(const std::wstring& str)
{
	// the regex is something like (for the case where a range is given, e.g. "3-7" or "-inf-3") -> ([+-]*\s*(?:\d+|inf))\s*-\s*([+-]*\s*(?:\d+|inf))
	// we use a quickly-put-together hand-crafted parser because regex seems broken before GCC4.9.
	// TODO: we do not maintain (and report) the "number of characters successfully parsed" when bailing out with an exception
	wstring num1, num2;

	int i = 0;
	wchar_t c = str[i];
	if (c == L'-' || c == L'+' || iswdigit(c))
	{
		num1 += c;
		++i;
		if (c == L'-' || c == L'+')
		{
			for (; str[i] != L'\0' && iswspace(str[i]); ++i);
			if (str[i] == L'i' && str[i + 1] == L'n' && str[i + 2] == L'f')
			{
				num1 += L"inf";
				i += 3;
				goto first_part_done;
			}
		}

		for (; iswdigit(str[i]); ++i) { num1 += str[i]; }
	}
	else if (c == L'i' && str[i + 1] == L'n' && str[i + 2] == L'f')
	{
		num1 = L"inf";
		i += 3;
	}
	else
	{
		throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
	}

first_part_done:
	// now, the next (non whitespace) character must either be '-' or end-of-string
	for (; str[i] != L'\0' && iswspace(str[i]); ++i);

	c = str[i];
	if (c == L'\0')
	{
		int v = ValueFromNumString(num1);
		return make_tuple(v, v);
	}
	else if (c != L'-')
	{
		throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
	}

	c = str[++i];
	if (c == L'-' || c == L'+' || iswdigit(c))
	{
		num2 += c;
		++i;
		if (c == L'-' || c == L'+')
		{
			for (; str[i] != L'\0'&&iswspace(str[i]); ++i);
			if (str[i] == L'i' && str[i + 1] == L'n' && str[i + 2] == 'f')
			{
				num2 += L"inf";
				i += 3;
				goto second_part_done;
			}
		}

		for (; iswdigit(str[i]); ++i) { num2 += str[i]; }
	}
	else if (c == L'i' && str[i + 1] == L'n' && str[i + 2] == L'f')
	{
		num2 = L"inf";
		i += 3;
	}
	else
	{
		throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
	}

second_part_done:
	// now we _must_ have reached end-of-string
	if (str[i] != L'\0')
	{
		throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
	}

	int v1 = ValueFromNumString(num1);
	int v2 = ValueFromNumString(num2);
	return make_tuple(v1, v2);
}

/*static*/int CIndexSet::ValueFromNumString(const std::wstring& num)
{
	int v;
	if (num.compare(L"-inf") == 0)
	{
		v = (numeric_limits<int>::min)();
	}
	else if (num.compare(L"+inf") == 0 || num.compare(L"inf") == 0)
	{
		v = (numeric_limits<int>::max)();
	}
	else
	{
		long int liValue = wcstol(num.c_str(), nullptr, 10);
		if (liValue == (std::numeric_limits<long int>::max)() || liValue == (std::numeric_limits<long int>::min)())
		{
			if (errno == ERANGE)
			{
				throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
			}
		}

		if (liValue > (numeric_limits<int>::max)() || liValue < (numeric_limits<int>::min)())
		{
			throw LibCZIStringParseException("Syntax error", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
		}

		v = static_cast<int>(liValue);
	}

	return v;
}