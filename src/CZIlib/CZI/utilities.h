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

#include <algorithm>
#include <string>
#include <functional>
#include "libCZI_Pixels.h"

class Utilities
{
public:
	static inline void Split(const std::wstring &text, wchar_t sep, std::function<bool(const std::wstring)> funcToken)
	{
		std::size_t start = 0, end = 0;
		while (static_cast<std::size_t>(end = text.find(sep, start)) != static_cast<std::size_t>(std::wstring::npos))
		{
			std::wstring temp = text.substr(start, end - start);
			if (!temp.empty())
			{
				if (funcToken(temp) == false)
					return;
			}

			start = end + 1;
		}

		std::wstring temp = text.substr(start);
		if (!temp.empty())
		{
			funcToken(temp);
		}
	}

	static inline  libCZI::IntRect Intersect(const libCZI::IntRect&a, const libCZI::IntRect& b)
	{
		int x1 = (std::max)(a.x, b.x);
		int x2 = (std::min)(a.x + a.w, b.x + b.w);
		int y1 = (std::max)(a.y, b.y);
		int y2 = (std::min)(a.y + a.h, b.y + b.h);

		if (x2 >= x1 && y2 >= y1)
		{
			return libCZI::IntRect{ x1, y1, x2 - x1, y2 - y1 };
		}

		return libCZI::IntRect{ 0,0,0,0 };
	}

	static inline bool DoIntersect(const libCZI::IntRect&a, const libCZI::IntRect& b)
	{
		auto r = Intersect(a, b);
		return (r.w <= 0 || r.h <= 0) ? false : true;
	}

	static inline std::uint8_t clampToByte(float f)
	{
		if (f <= 0)
		{
			return 0;
		}
		else if (f >= 255)
		{
			return 255;
		}

		return (std::uint8_t)(f + .5f);
	}

	static inline std::uint16_t clampToUShort(float f)
	{
		if (f <= 0)
		{
			return 0;
		}
		else if (f >= 65535)
		{
			return 65535;
		}

		return (std::uint16_t)(f + .5f);
	}

	static std::uint8_t HexCharToInt(char c);

	static std::string Trim(const std::string& str, const std::string& whitespace = " \t");
	static std::wstring Trim(const std::wstring& str, const std::wstring& whitespace = L" \t");

	static bool icasecmp(const std::string& l, const std::string& r);
	static bool icasecmp(const std::wstring& l, const std::wstring& r);

	template<typename t>
	inline static t clamp(t v, t min, t max)
	{
		if (v < min)
		{
			return min;
		}
		else if (v > max)
		{
			return max;
		}

		return v;
	}
};

