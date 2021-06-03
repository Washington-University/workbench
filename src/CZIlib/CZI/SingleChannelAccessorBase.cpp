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
#include "SingleChannelAccessorBase.h"
#include "BitmapOperations.h"

using namespace std;
using namespace libCZI;

bool CSingleChannelAccessorBase::TryGetPixelType(const libCZI::IDimCoordinate* planeCoordinate, libCZI::PixelType& pixeltype)
{
	int c = (numeric_limits<int>::min)();
	planeCoordinate->TryGetPosition(libCZI::DimensionIndex::C, &c);

	// the idea is: for the cornerstone-case where we do not have a C-index, the call to "TryGetSubBlockInfoOfArbitrarySubBlockInChannel"
	// will igonore the specified index _if_ there are no C-indices at all
	pixeltype = Utils::TryDeterminePixelTypeForChannel(this->sbBlkRepository.get(), c);
	return (pixeltype != PixelType::Invalid) ? true : false;
}

/*static*/void CSingleChannelAccessorBase::Clear(libCZI::IBitmapData* bm, const libCZI::RgbFloatColor& floatColor)
{
	if (!isnan(floatColor.r) && !isnan(floatColor.g) && !isnan(floatColor.b))
	{
		CBitmapOperations::Fill(bm, floatColor);
	}
}

void CSingleChannelAccessorBase::CheckPlaneCoordinates(const libCZI::IDimCoordinate* planeCoordinate) const
{
	// planeCoordinate must not contain S
	if (planeCoordinate->IsValid(DimensionIndex::S))
	{
		throw LibCZIInvalidPlaneCoordinateException("S-dimension is illegal for a plane.", LibCZIInvalidPlaneCoordinateException::ErrorCode::InvalidDimension);
	}

	static const DimensionIndex DimensionsToCheck[] =
	{ DimensionIndex::Z,DimensionIndex::C,DimensionIndex::T,DimensionIndex::R,DimensionIndex::I,DimensionIndex::H,DimensionIndex::V,DimensionIndex::B };

	SubBlockStatistics statistics = this->sbBlkRepository->GetStatistics();

	for (size_t i = 0; i < sizeof(DimensionsToCheck) / sizeof(DimensionsToCheck[0]); ++i)
	{
		auto d = DimensionsToCheck[i];
		int start, size;
		if (statistics.dimBounds.TryGetInterval(d, &start, &size))
		{
			// if the dimension is present in the dim-bounds, then it must be also given
			// in the plane-coordinate - with the sole exception that it can be absent if size is 1
			int co;
			if (planeCoordinate->TryGetPosition(d, &co) == false)
			{
				if (size > 1)
				{
					stringstream ss;
					ss << "Coordinate for dimension '" << Utils::DimensionToChar(d) << "' not given.";
					throw LibCZIInvalidPlaneCoordinateException(ss.str().c_str(), LibCZIInvalidPlaneCoordinateException::ErrorCode::MissingDimension);
				}
			}
			else
			{
				if (co < start || co >= start + size)
				{
					stringstream ss;
					ss << "Coordinate for dimension '" << Utils::DimensionToChar(d) << "' is out-of-range.";
					throw LibCZIInvalidPlaneCoordinateException(ss.str().c_str(), LibCZIInvalidPlaneCoordinateException::ErrorCode::CoordinateOutOfRange);
				}
			}
		}
		else
		{
			// if the dimension is not present in the dim-bounds, it must not be given in the plane-coordinate
			if (planeCoordinate->IsValid(d))
			{
				stringstream ss;
				ss << "Coordinate for dimension '" << Utils::DimensionToChar(d) << "' is not expected.";
				throw LibCZIInvalidPlaneCoordinateException(ss.str().c_str(), LibCZIInvalidPlaneCoordinateException::ErrorCode::SurplusDimension);
			}
		}
	}
}