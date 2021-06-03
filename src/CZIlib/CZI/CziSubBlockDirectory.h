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

#include <functional>
#include "libCZI.h"

class CCziSubBlockDirectory
{
public:
	struct SubBlkEntry
	{
		libCZI::CDimCoordinate coordinate;
		int mIndex;
		int x;
		int y;
		int width;
		int height;
		int storedWidth;
		int storedHeight;
		int PixelType;
		std::uint64_t FilePosition;
		int Compression;

		bool IsMIndexValid() const
		{
			return this->mIndex != (std::numeric_limits<int>::min)() ? true : false;
		}

		bool IsStoredSizeEqualLogicalSize() const
		{
			return this->width == this->storedWidth && this->height == this->storedHeight;
		}

		void Invalidate()
		{
			this->mIndex = this->x = this->y = this->width = this->height = this->storedWidth = this->storedHeight = (std::numeric_limits<int>::min)();
		}
	};

private:
	std::vector<SubBlkEntry> subBlks;
	libCZI::SubBlockStatistics statistics;
	libCZI::PyramidStatistics pyramidStatistics;
	enum class State
	{
		AddingAllowed,
		AddingFinished
	};
	State state;
public:
	CCziSubBlockDirectory();

	const libCZI::SubBlockStatistics& GetStatistics() const;
	const libCZI::PyramidStatistics& GetPyramidStatistics() const;

	void AddSubBlock(const SubBlkEntry& entry);
	void AddingFinished();

	void EnumSubBlocks(std::function<bool(int index, const SubBlkEntry&)> func);
	bool TryGetSubBlock(int index, SubBlkEntry& entry);

private:
	void UpdateStatistics(const SubBlkEntry& entry);
	void SortPyramidStatistics();
	static void UpdateBoundingBox(libCZI::IntRect& rect, const SubBlkEntry& entry);
	static bool TryToDeterminePyramidLayerInfo(const SubBlkEntry& entry, std::uint8_t* minificationFactor, std::uint8_t* pyramidLayerNo);
	static void UpdatePyramidLayerStatistics(std::vector<libCZI::PyramidStatistics::PyramidLayerStatistics>& vec, const libCZI::PyramidStatistics::PyramidLayerInfo& pli);
};
