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

#include "libCZI.h"

class CIndexSet : public libCZI::IIndexSet
{
private:
	struct interval
	{
		int start, end;

		bool IsContained(int index) const { return this->start <= index && index <= this->end ? true : false; }
	};

	std::vector<interval> intervals;
public:
	CIndexSet(const std::wstring& str);

	/// Query if 'index' is contained in the set.
	///
	/// \param index Index to query.
	///
	/// \return True if the specified index is contained, false if not.
	bool IsContained(int index) const override;
private:
	void ParseString(const std::wstring& str);
	void AddInterval(int start, int end);
	static std::tuple<int, int> ParsePart(const std::wstring& str);
	static int ValueFromNumString(const std::wstring& str);
};
