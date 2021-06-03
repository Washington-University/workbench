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
#include "CziUtils.h"

using namespace libCZI;

/*static*/libCZI::PixelType CziUtils::PixelTypeFromInt(int i)
{
	switch (i)
	{
	case 0:  return PixelType::Gray8;
	case 1:  return PixelType::Gray16;
	case 2:  return PixelType::Gray32Float;
	case 3:  return PixelType::Bgr24;
	case 4:  return PixelType::Bgr48;
	case 8:  return PixelType::Bgr96Float;
	case 9:  return PixelType::Bgra32;
	case 10: return PixelType::Gray64ComplexFloat;
	case 11: return PixelType::Bgr192ComplexFloat;
	case 12: return PixelType::Gray32;
	case 13: return PixelType::Gray64Float;
	}

	return PixelType::Invalid;
}

/*static*/libCZI::CompressionMode CziUtils::CompressionModeFromInt(int i)
{
	switch (i)
	{
	case 0: return CompressionMode::UnCompressed;
	case 1: return CompressionMode::Jpg;
	case 4: return CompressionMode::JpgXr;
	}

	return CompressionMode::Invalid;
}

/*static*/std::uint8_t CziUtils::GetBytesPerPel(libCZI::PixelType pixelType)
{
	switch (pixelType)
	{
	case PixelType::Gray8: return 1;
	case PixelType::Gray16:return 2;
	case PixelType::Gray32Float:return 4;
	case PixelType::Bgr24:return 3;
	case PixelType::Bgr48: return 6;
	case PixelType::Bgr96Float: return 12;
	case PixelType::Bgra32:return 4;
	case PixelType::Gray64ComplexFloat: return 16;
	case PixelType::Bgr192ComplexFloat: return 48;
	case PixelType::Gray32: return 4;
	case PixelType::Gray64Float:return 8;
	default:throw std::invalid_argument("illegal pixeltype");
	}
}

/// <summary>	Compare coordinate in the following way:
/// 			All dimensions that are present in the first coordinate MUST be present
/// 			in the second and have the same value, otherwise we return false.
/// 			We DO NOT check if the second coordinate contains dimension that are
/// 			not present in the first.
/// </summary>
/// <param name="coord1">	[in] The first coordinate. </param>
/// <param name="coord2">	[in] The second coordinate. </param>
///
/// <returns>	A boolean indicating whether the coordinates are equal or not (according to the above definition of equality). </returns>
/*static*/bool CziUtils::CompareCoordinate(const libCZI::IDimCoordinate* coord1, const libCZI::IDimCoordinate* coord2)
{
	bool areEqual = true;
	CziUtils::EnumAllCoordinateDimensions(
		[&](libCZI::DimensionIndex dim)->bool
	{
		int coordinate1, coordinate2;
		if (coord1->TryGetPosition(dim, &coordinate1) == true)
		{
			if (coord2->TryGetPosition(dim, &coordinate2) != true)
			{
				areEqual= false;
				return false;
			}

			if (coordinate1 != coordinate2)
			{
				areEqual = false;
				return false;
			}
		}

		return true;
	});

	return areEqual;
}

/*static*/void CziUtils::EnumAllCoordinateDimensions(std::function<bool(libCZI::DimensionIndex)> func)
{
	static const DimensionIndex dims[] = { DimensionIndex::Z,DimensionIndex::C,DimensionIndex::T,DimensionIndex::R,DimensionIndex::S,
											DimensionIndex::I,DimensionIndex::H,DimensionIndex::V,DimensionIndex::B };

	for (int i = 0; i < sizeof(dims) / sizeof(dims[0]); ++i)
	{
		if (func(dims[i]) == false)
			break;
	}
}

/*static*/double CziUtils::CalculateMinificationFactor(int logicalSizeWidth, int logicalSizeHeight, int physicalSizeWidth, int physicalSizeHeight)
{
	// We try to reduce the error (introduced by the fact that the width/height are given in integers)
	// by using the largest value.
	// - Note that we are limiting ourselves to isotrophic scales (which is not a problem because that is all that works)
	// - This is only mitigating the issue...
	double minFactor;
	if (physicalSizeWidth > physicalSizeHeight)
	{
		minFactor = double(logicalSizeWidth) / physicalSizeWidth;
	}
	else
	{
		minFactor = double(logicalSizeHeight) / physicalSizeHeight;
	}

	return minFactor;
}
