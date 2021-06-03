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

#include <cstdint>
#include <functional>
#include <string.h>

namespace JxrDecode
{
	enum class PixelFormat
	{
		dontCare,
		_24bppBGR,
		_1bppBlackWhite,
		_8bppGray,
		_16bppGray,
		_16bppGrayFixedPoint,
		_16bppGrayHalf,
		_32bppGrayFixedPoint,
		_32bppGrayFloat,
		_24bppRGB,
		_48bppRGB,
		_48bppRGBFixedPoint,
		_48bppRGBHalf,
		_96bppRGBFixedPoint,
		_128bppRGBFloat,
		_32bppRGBE,
		_32bppCMYK,
		_64bppCMYK,
		_32bppBGRA,
		_64bppRGBA,
		_64bppRGBAFixedPoint,
		_64bppRGBAHalf,
		_128bppRGBAFixedPoint,
		_128bppRGBAFloat,
		_16bppBGR555,
		_16bppBGR565,
		_32bppBGR101010,
		_40bppCMYKA,
		_80bppCMYKA,
		_32bppBGR,

		invalid
	};

	const char* PixelFormatAsInformalString(PixelFormat pfmt);

	enum class Orientation :std::uint8_t
	{
		// CRW: Clock Wise 90% Rotation; FlipH: Flip Horizontally;  FlipV: Flip Vertically
		// Peform rotation FIRST!
		//                CRW FlipH FlipV
		O_NONE = 0,    // 0    0     0
		O_FLIPV,       // 0    0     1
		O_FLIPH,       // 0    1     0
		O_FLIPVH,      // 0    1     1
		O_RCW,         // 1    0     0
		O_RCW_FLIPV,   // 1    0     1
		O_RCW_FLIPH,   // 1    1     0
		O_RCW_FLIPVH,  // 1    1     1
		/* add new ORIENTATION here */ O_MAX
	};

	enum class Subband : std::uint8_t
	{
		SB_ALL = 0,         // keep all subbands
		SB_NO_FLEXBITS,     // skip flex bits
		SB_NO_HIGHPASS,     // skip highpass
		SB_DC_ONLY,         // skip lowpass and highpass, DC only
		SB_ISOLATED,        // not decodable
		/* add new SUBBAND here */ SB_MAX
	};

	struct WMPDECAPPARGS
	{
		JxrDecode::PixelFormat pixFormat;

		// region decode
		size_t rLeftX;
		size_t rTopY;
		size_t rWidth;
		size_t rHeight;

		std::uint8_t cPostProcStrength;
		JxrDecode::Orientation  oOrientation;
		JxrDecode::Subband sbSubband;
		/*	// thumbnail
			size_t tThumbnailFactor;

			// orientation
			ORIENTATION oOrientation;

			// post processing
			U8 cPostProcStrength;
			*/
		std::uint8_t uAlphaMode; // 0:no alpha 1: alpha only else: something + alpha 
		/*
		SUBBAND sbSubband;  // which subbands to keep (for transcoding)

		BITSTREAMFORMAT bfBitstreamFormat; // desired bitsream format (for transcoding)

		CWMIStrCodecParam wmiSCP;

		Bool bIgnoreOverlap;*/
		bool bIgnoreOverlap;

		void Clear()
		{
			memset(this, 0, sizeof(*this));
			this->pixFormat = JxrDecode::PixelFormat::dontCare;
			//args->bVerbose = FALSE;
			//args->tThumbnailFactor = 0;
			this->oOrientation = Orientation::O_NONE;
			this->cPostProcStrength = 0;
			this->uAlphaMode = 255;
			this->sbSubband = JxrDecode::Subband::SB_ALL;
		}
	};


	typedef void* codecHandle;

	codecHandle Initialize();

	void Decode(
		codecHandle h,
		const WMPDECAPPARGS* decArgs,
		const void* ptrData,
		size_t size,
		std::function<PixelFormat(PixelFormat)> selectDestPixFmt,
		std::function<void(PixelFormat pixFmt, std::uint32_t  width, std::uint32_t  height, std::uint32_t linesCount, const void* ptrData, std::uint32_t stride)> deliverData);

	void Destroy(codecHandle h);

}