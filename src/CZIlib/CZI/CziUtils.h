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

#include "libCZI_Pixels.h"
#include "libCZI_DimCoordinate.h"
#include <functional>
#include <stdint.h>

enum class CompareResult
{
	Equal,
	NotEqual,
	Ambigious
};

class CziUtils
{
public:
	static libCZI::PixelType PixelTypeFromInt(int i);
	static libCZI::CompressionMode CompressionModeFromInt(int i);
	static std::uint8_t GetBytesPerPel(libCZI::PixelType pixelType);
	static bool CompareCoordinate(const libCZI::IDimCoordinate* coord1, const libCZI::IDimCoordinate* coord2);
	static void EnumAllCoordinateDimensions(std::function<bool(libCZI::DimensionIndex)> func);

	static double CalculateMinificationFactor(int logicalSizeWidth, int logicalSizeHeight, int physicalSizeWidth, int physicalSizeHeight);


	template <libCZI::PixelType tPixelType>
	static constexpr std::uint8_t BytesPerPel();// { /*throw std::logic_error("invalid pixeltype");*/return 0; }
};

template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray8>() { return 1; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray16>() { return 2; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Bgr24>() { return 3; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Bgr48>() { return 6; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray32Float>() { return 4; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray64Float>() { return 8; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Bgra32>() { return 4; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray32>() { return 4; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Bgr96Float>() { return 3 * 4; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Gray64ComplexFloat>() { return 2 * 8; }
template <>	constexpr std::uint8_t CziUtils::BytesPerPel<libCZI::PixelType::Bgr192ComplexFloat>() { return 48; }
