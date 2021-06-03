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
#include "libCZI_Pixels.h"

class CBitmapOperations
{
public:
	static int CalcMd5Sum(libCZI::IBitmapData* bm, std::uint8_t* ptrHash, int hashSize);

	static void NNResize(libCZI::IBitmapData* bmSrc, libCZI::IBitmapData* bmDest);

	static void NNResize(libCZI::IBitmapData* bmSrc, libCZI::IBitmapData* bmDest,const libCZI::DblRect& roiSrc,const libCZI::DblRect& roiDst);

	template <typename tFlt>
	struct NNResizeInfo2
	{
		const void* srcPtr;
		int srcStride;
		int srcWidth, srcHeight;
		tFlt srcRoiX, srcRoiY, srcRoiW, srcRoiH;
		void* dstPtr;
		int dstStride;
		int dstWidth, dstHeight;
		tFlt dstRoiX, dstRoiY, dstRoiW, dstRoiH;
	};

	typedef NNResizeInfo2<float> NNResizeInfo2Flt;
	typedef NNResizeInfo2<double> NNResizeInfo2Dbl;

	template <typename tFlt>
	static void NNSCale2(libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, const NNResizeInfo2<tFlt>& resizeInfo);

	struct CopyOffsetedInfo
	{
		int xOffset;
		int yOffset;
		libCZI::PixelType srcPixelType;
		const void* srcPtr;
		int srcStride;
		int srcWidth;
		int srcHeight;
		libCZI::PixelType dstPixelType;
		void* dstPtr;
		int dstStride;
		int dstWidth;
		int dstHeight;

		bool drawTileBorder;
	};

	static void CopyOffseted(const CopyOffsetedInfo& info);
	static void Copy(libCZI::PixelType srcPixelType, const void* srcPtr, int srcStride, libCZI::PixelType dstPixelType, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder);

	template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType>
	static void Copy(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder);

	template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, typename tPixelConverter>
	static void Copy(const tPixelConverter& conv, const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder);

	template <libCZI::PixelType tSrcDstPixelType>
	static void CopySamePixelType(const void* srcPtr, int srcStride, void* dstPtr, int dstStride, int width, int height, bool drawTileBorder);

	static void Fill(libCZI::IBitmapData* bm, const libCZI::RgbFloatColor& floatColor);

	static void Fill_Gray8(int w, int h, void* ptr, int stride, std::uint8_t val);
	static void Fill_Gray16(int w, int h, void* ptr, int stride, std::uint16_t val);
	static void Fill_Bgr24(int w, int h, void* ptr, int stride, std::uint8_t b, std::uint8_t g, std::uint8_t r);
	static void Fill_Bgr48(int w, int h, void* ptr, int stride, std::uint16_t b, std::uint16_t g, std::uint16_t r);
	static void Fill_GrayFloat(int w, int h, void* ptr, int stride, float v);
	static void RGB48ToBGR48(int w, int h, std::uint16_t* ptr, int stride);
private:
	
	template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, typename tPixelConverter, typename tFlt>
	static void InternalNNScale2(const tPixelConverter& conv, const NNResizeInfo2<tFlt>& resizeInfo);

	template <libCZI::PixelType tSrcPixelType, libCZI::PixelType tDstPixelType, typename tPixelConverter, typename tFlt>
	static void InternalNNScale2(const NNResizeInfo2<tFlt>& resizeInfo)
	{
		tPixelConverter conv;
		InternalNNScale2<tSrcPixelType, tDstPixelType, tPixelConverter>(conv, resizeInfo);
	}

	static void ThrowUnsupportedConversion(libCZI::PixelType srcPixelType, libCZI::PixelType dstPixelType);
};

#include "BitmapOperations.hpp"
