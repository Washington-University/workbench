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
#include "CziSubBlockDirectory.h"
#include "CziUtils.h"

using namespace libCZI;

CCziSubBlockDirectory::CCziSubBlockDirectory() : state(State::AddingAllowed)
{
	this->statistics.Invalidate();
	this->statistics.subBlockCount = 0;
}

void CCziSubBlockDirectory::AddSubBlock(const SubBlkEntry& entry)
{
	if (this->state != State::AddingAllowed)
	{
		throw std::logic_error("The object is not allowing to add subblocks any more.");
	}

	this->subBlks.push_back(entry);
	this->UpdateStatistics(entry);
}

void CCziSubBlockDirectory::AddingFinished()
{
	this->state = State::AddingFinished;
	this->SortPyramidStatistics();
}

const libCZI::SubBlockStatistics& CCziSubBlockDirectory::GetStatistics() const
{
	return this->statistics;
}

const libCZI::PyramidStatistics& CCziSubBlockDirectory::GetPyramidStatistics() const
{
	return this->pyramidStatistics;
}

void CCziSubBlockDirectory::EnumSubBlocks(std::function<bool(int index, const SubBlkEntry&)> func)
{
	int i = 0;
	for (auto it = this->subBlks.cbegin(); it != this->subBlks.cend(); ++it)
	{
		bool b = func(i++, *it);
		if (b == false)
		{
			break;
		}
	}
}

bool CCziSubBlockDirectory::TryGetSubBlock(int index, SubBlkEntry& entry)
{
	if (index < (int)this->subBlks.size())
	{
		entry = this->subBlks.at(index);
		return true;
	}

	return false;
}

void CCziSubBlockDirectory::UpdateStatistics(const SubBlkEntry& entry)
{
	// TODO: check validity of x,y etc.
	CCziSubBlockDirectory::UpdateBoundingBox(this->statistics.boundingBox, entry);
	if (entry.IsStoredSizeEqualLogicalSize())
	{
		CCziSubBlockDirectory::UpdateBoundingBox(this->statistics.boundingBoxLayer0Only, entry);
	}

	entry.coordinate.EnumValidDimensions(
		[&](libCZI::DimensionIndex dim, int value)->bool
	{
		int start, size;
		if (this->statistics.dimBounds.TryGetInterval(dim, &start, &size) == false)
		{
			this->statistics.dimBounds.Set(dim, value, 1);
		}
		else
		{
			bool changed = false;
			if (value < start)
			{
				size += (start - value);
				start = value;
				changed = true;
			}
			else if (value >= start + size)
			{
				size = 1 + value - start;
				changed = true;
			}

			if (changed)
			{
				this->statistics.dimBounds.Set(dim, start, size);
			}
		}

		if (entry.IsMIndexValid())
		{
			if (entry.mIndex < this->statistics.minMindex)
			{
				this->statistics.minMindex = entry.mIndex;
			}

			if (entry.mIndex > this->statistics.maxMindex)
			{
				this->statistics.maxMindex = entry.mIndex;
			}
		}

		return true;
	});

	// now deal with "enclosing Rect" for the scenes...
	int sceneIndex;
	if (entry.coordinate.TryGetPosition(libCZI::DimensionIndex::S, &sceneIndex))
	{
		auto it = this->statistics.sceneBoundingBoxes.find(sceneIndex);
		if (it != this->statistics.sceneBoundingBoxes.end())
		{
			CCziSubBlockDirectory::UpdateBoundingBox(it->second.boundingBox, entry);
			if (entry.IsStoredSizeEqualLogicalSize() == true)
			{
				CCziSubBlockDirectory::UpdateBoundingBox(it->second.boundingBoxLayer0, entry);
			}
		}
		else
		{
			BoundingBoxes boundingBoxes;
			boundingBoxes.boundingBox.x = entry.x;
			boundingBoxes.boundingBox.y = entry.y;
			boundingBoxes.boundingBox.w = entry.width;
			boundingBoxes.boundingBox.h = entry.height;
			if (entry.IsStoredSizeEqualLogicalSize() == true)
			{
				boundingBoxes.boundingBoxLayer0 = boundingBoxes.boundingBox;
			}
			else
			{
				boundingBoxes.boundingBoxLayer0.Invalidate();
			}

			this->statistics.sceneBoundingBoxes.insert(std::pair<int, BoundingBoxes>(sceneIndex, boundingBoxes));
		}
	}

	// now deal with the pyramid-layer info
	if (!entry.coordinate.TryGetPosition(libCZI::DimensionIndex::S, &sceneIndex))
	{
		// in case that the scene-index is not valid, then we use int::max in order to
		// represent this fact
		sceneIndex = (std::numeric_limits<int>::max)();
	}

	PyramidStatistics::PyramidLayerInfo pli;
	bool b = CCziSubBlockDirectory::TryToDeterminePyramidLayerInfo(entry, &pli.minificationFactor, &pli.pyramidLayerNo);
	if (b == false)
	{
		pli.minificationFactor = pli.pyramidLayerNo = 0xff;
	}

	auto it = this->pyramidStatistics.scenePyramidStatistics.find(sceneIndex);
	if (it != this->pyramidStatistics.scenePyramidStatistics.end())
	{
		this->UpdatePyramidLayerStatistics(it->second, pli);
	}
	else
	{
		std::vector<PyramidStatistics::PyramidLayerStatistics> vecPs;
		this->UpdatePyramidLayerStatistics(vecPs, pli);
		this->pyramidStatistics.scenePyramidStatistics.insert(std::pair<int, std::vector<PyramidStatistics::PyramidLayerStatistics>>(sceneIndex, vecPs));
	}

	++this->statistics.subBlockCount;
}

/*static*/void CCziSubBlockDirectory::UpdateBoundingBox(libCZI::IntRect& rect, const SubBlkEntry& entry)
{
	if (rect.IsValid() == true)
	{
		if (rect.x > entry.x)
		{
			int diff = rect.x - entry.x;
			rect.x = entry.x;
			rect.w += diff;
		}

		if (rect.y > entry.y)
		{
			int diff = rect.y - entry.y;
			rect.y = entry.y;
			rect.h += diff;
		}

		if (rect.x + rect.w < entry.x + entry.width)
		{
			rect.w = (entry.x + entry.width) - rect.x;
		}

		if (rect.y + rect.h < entry.y + entry.height)
		{
			rect.h = (entry.y + entry.height) - rect.y;
		}
	}
	else
	{
		rect.x = entry.x;
		rect.y = entry.y;
		rect.w = entry.width;
		rect.h = entry.height;
	}
}

/// Attempts to to determine pyramid layer information from the given data.
/// If we have a layer-0 subblock, minificationFactor and pyramdidLayerNo are set to 0.
///
/// \param entry					   The subblock-entry.
/// \param [out] ptrMinificationFactor If non-null, the minification factor will be stored here.
/// \param [out] ptrPyramidLayerNo	   If non-null, the pyramid layer no will be stored here.
///
/// \return True if it succeeds, false if it fails.
/*static*/bool CCziSubBlockDirectory::TryToDeterminePyramidLayerInfo(const SubBlkEntry& entry, std::uint8_t* ptrMinificationFactor, std::uint8_t* ptrPyramidLayerNo)
{
	if (entry.width == entry.storedWidth && entry.height == entry.storedHeight)
	{
		if (ptrMinificationFactor != nullptr) { *ptrMinificationFactor = 0; }
		if (ptrPyramidLayerNo != nullptr) { *ptrPyramidLayerNo = 0; }
		return true;
	}

	double minificationFactor = CziUtils::CalculateMinificationFactor(entry.width, entry.height, entry.storedWidth, entry.storedHeight);

	struct MinificationFactorToPyramidLayerInfo
	{
		double value, delta_min, delta_max;
		int pyramidLayer;

		bool IsInRange(double v) const
		{
			if (v >= (this->value - this->delta_min) && v <= (this->value + this->delta_max))
			{
				return true;
			}

			return false;
		}
	};

	static const MinificationFactorToPyramidLayerInfo MinFacToPLI_Factor2[] =
	{
		{2,.1,.1,1},
		{4,.2,.2,2},
		{8,.4,.4,3},
		{16,.8,.8,4},
		{32, 1,1,5},
		{64, 1,1,6},
		{128,1,1,7},
		{256,2,2,8},
		{512,4,4,9},
		{1024,10,10,10}
	};

	static const MinificationFactorToPyramidLayerInfo MinFacToPLI_Factor3[] =
	{
		{ 3,.1,.1,1 },
		{ 9,.2,.2,2 },
		{ 27,.8,.8,3 },
		{ 81,1.5,1.5,4 },
		{ 243, 2,2,5 },
		{ 729, 5,5,6 },
		{ 2187,15,15,7 }
	};

	// check whether it is a factor of 2
	for (int i = 0; i < sizeof(MinFacToPLI_Factor2) / sizeof(MinFacToPLI_Factor2[0]); ++i)
	{
		if (MinFacToPLI_Factor2[i].IsInRange(minificationFactor))
		{
			if (ptrMinificationFactor != nullptr) { *ptrMinificationFactor = 2; }
			if (ptrPyramidLayerNo != nullptr) { *ptrPyramidLayerNo = MinFacToPLI_Factor2[i].pyramidLayer; }
			return true;
		}
	}

	// check whether it is a factor of 3
	for (int i = 0; i < sizeof(MinFacToPLI_Factor3) / sizeof(MinFacToPLI_Factor3[0]); ++i)
	{
		if (MinFacToPLI_Factor3[i].IsInRange(minificationFactor))
		{
			if (ptrMinificationFactor != nullptr) { *ptrMinificationFactor = 3; }
			if (ptrPyramidLayerNo != nullptr) { *ptrPyramidLayerNo = MinFacToPLI_Factor3[i].pyramidLayer; }
			return true;
		}
	}

	return false;
}

/*static*/void CCziSubBlockDirectory::UpdatePyramidLayerStatistics(std::vector<libCZI::PyramidStatistics::PyramidLayerStatistics>& vec, const PyramidStatistics::PyramidLayerInfo& pli)
{
	auto it = std::find_if(vec.begin(), vec.end(), [&](const PyramidStatistics::PyramidLayerStatistics& i) {return pli.minificationFactor == i.layerInfo.minificationFactor && pli.pyramidLayerNo == i.layerInfo.pyramidLayerNo; });
	if (it != vec.end())
	{
		++it->count;
	}
	else
	{
		PyramidStatistics::PyramidLayerStatistics pls;
		pls.layerInfo = pli;
		pls.count = 1;
		vec.emplace_back(pls);
	}
}

void CCziSubBlockDirectory::SortPyramidStatistics()
{
	for (auto& v : this->pyramidStatistics.scenePyramidStatistics)
	{
		std::sort(v.second.begin(), v.second.end(), [](const PyramidStatistics::PyramidLayerStatistics& a, const PyramidStatistics::PyramidLayerStatistics& b)->bool
		{
			// layer0 always "goes first"
			if (a.layerInfo.IsLayer0())
			{
				return true;
			}

			// ...and "not identified" always last
			if (a.layerInfo.IsNotIdentifiedAsPyramidLayer())
			{
				return false;
			}

			if (b.layerInfo.IsLayer0())
			{
				return false;
			}

			if (b.layerInfo.IsNotIdentifiedAsPyramidLayer())
			{
				return true;
			}

			int minificationFactorA = a.layerInfo.minificationFactor;
			for (int i = 0; i < a.layerInfo.pyramidLayerNo - 1; ++i)
			{
				minificationFactorA *= a.layerInfo.minificationFactor;
			}

			int minificationFactorB = b.layerInfo.minificationFactor;
			for (int i = 0; i < b.layerInfo.pyramidLayerNo - 1; ++i)
			{
				minificationFactorB *= b.layerInfo.minificationFactor;
			}

			return minificationFactorA < minificationFactorB;
		});
	}
}