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
#include "decoder.h"
#include "../CZIJxrDecode/JxrDecode.h"
#include "bitmapData.h"
#include "stdAllocator.h"
#include "BitmapOperations.h"
#include "Site.h"

using namespace libCZI;
using namespace std;

/*static*/std::shared_ptr<CJxrLibDecoder> CJxrLibDecoder::Create()
{
	return make_shared<CJxrLibDecoder>(JxrDecode::Initialize());
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CJxrLibDecoder::Decode(const void* ptrData, size_t size)
{
	std::shared_ptr<IBitmapData> bm;

	JxrDecode::WMPDECAPPARGS args; args.Clear();
	args.uAlphaMode = 0;	// we don't need any alpha, never

	try
	{
		if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
		{
			stringstream ss; ss << "Begin JxrDecode with " << size << " bytes";
			GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
		}

		JxrDecode::Decode(this->handle, &args, ptrData, size,
			[](JxrDecode::PixelFormat decPixFmt)->JxrDecode::PixelFormat
		{
			// We get the "original pixelformat" of the compressed data, and we need to respond
			// which pixelformat we want to get from the decoder.
			// ...one problem is that not all format-conversions are possible - we choose the "closest"
			JxrDecode::PixelFormat destFmt;
			switch (decPixFmt)
			{
			case JxrDecode::PixelFormat::_24bppBGR:
			case JxrDecode::PixelFormat::_32bppRGBE:
			case JxrDecode::PixelFormat::_32bppCMYK:
			case JxrDecode::PixelFormat::_32bppBGR:
			case JxrDecode::PixelFormat::_16bppBGR555:
			case JxrDecode::PixelFormat::_16bppBGR565:
			case JxrDecode::PixelFormat::_32bppBGR101010:
			case JxrDecode::PixelFormat::_24bppRGB:
			case JxrDecode::PixelFormat::_32bppBGRA:
				destFmt=JxrDecode::PixelFormat::_24bppBGR;
				break;
			case JxrDecode::PixelFormat::_1bppBlackWhite:
			case JxrDecode::PixelFormat::_8bppGray:
				destFmt=JxrDecode::PixelFormat::_8bppGray;
				break;
			case JxrDecode::PixelFormat::_16bppGray:
			case JxrDecode::PixelFormat::_16bppGrayFixedPoint:
			case JxrDecode::PixelFormat::_16bppGrayHalf:
				destFmt = JxrDecode::PixelFormat::_16bppGray;
				break;
			case JxrDecode::PixelFormat::_32bppGrayFixedPoint:
			case JxrDecode::PixelFormat::_32bppGrayFloat:
				destFmt = JxrDecode::PixelFormat::_32bppGrayFloat;
				break;
			case JxrDecode::PixelFormat::_48bppRGB:
				destFmt = JxrDecode::PixelFormat::_48bppRGB;
				break;
			case JxrDecode::PixelFormat::_48bppRGBFixedPoint:
			case JxrDecode::PixelFormat::_48bppRGBHalf:
			case JxrDecode::PixelFormat::_96bppRGBFixedPoint:
			case JxrDecode::PixelFormat::_128bppRGBFloat:
			case JxrDecode::PixelFormat::_64bppCMYK:
			case JxrDecode::PixelFormat::_64bppRGBA:
			case JxrDecode::PixelFormat::_64bppRGBAFixedPoint:
			case JxrDecode::PixelFormat::_64bppRGBAHalf:
			case JxrDecode::PixelFormat::_128bppRGBAFixedPoint:
			case JxrDecode::PixelFormat::_128bppRGBAFloat:
			case JxrDecode::PixelFormat::_40bppCMYKA:
			case JxrDecode::PixelFormat::_80bppCMYKA:
			default:
				destFmt = JxrDecode::PixelFormat::invalid;
				throw std::logic_error("need to look into these formats...");
			}

			if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
			{
				stringstream ss; ss << "JxrDecode: original pixelfmt: " << JxrDecode::PixelFormatAsInformalString(decPixFmt) << ", requested pixelfmt: " << JxrDecode::PixelFormatAsInformalString(destFmt);
				GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
			}

			if (destFmt==JxrDecode::PixelFormat::invalid)
			{
				throw std::logic_error("need to look into these formats...");
			}

			return destFmt;
		},
			[&](JxrDecode::PixelFormat pixFmt, std::uint32_t  width, std::uint32_t  height, std::uint32_t linesCount, const void* ptrData, std::uint32_t stride)->void
		{
			if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
			{
				stringstream ss; ss << "JxrDecode: decode done - pixelfmt="<<JxrDecode::PixelFormatAsInformalString(pixFmt)<<" width=" << width << " height=" << height << " linesCount=" << linesCount << " stride=" << stride;
				GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
			}

			// TODO: it seems feasible to directly decode to the buffer (saving us the copy)
			PixelType px_type;
			switch (pixFmt)
			{
			case JxrDecode::PixelFormat::_24bppBGR: px_type = PixelType::Bgr24; break;
			case JxrDecode::PixelFormat::_8bppGray: px_type = PixelType::Gray8; break;
			case JxrDecode::PixelFormat::_48bppRGB: px_type = PixelType::Bgr48; break;
			case JxrDecode::PixelFormat::_16bppGray: px_type = PixelType::Gray16; break;
			case JxrDecode::PixelFormat::_32bppGrayFloat: px_type = PixelType::Gray32Float; break;
			default: throw std::logic_error("need to look into these formats...");
			}

			bm = GetSite()->CreateBitmap(px_type, width, height);
			auto bmLckInfo = ScopedBitmapLockerSP(bm);
			if (bmLckInfo.stride != stride)
			{
				for (uint32_t i = 0; i < linesCount; ++i)
				{
					memcpy(static_cast<char*>(bmLckInfo.ptrDataRoi) + i*bmLckInfo.stride, static_cast<const char*>(ptrData) + i*stride, stride);
				}
			}
			else
			{
				memcpy(bmLckInfo.ptrDataRoi, ptrData, stride*linesCount);
			}

			// since BGR48 is not available as output, we need to convert (#36)
			if (px_type == PixelType::Bgr48)
			{
				CBitmapOperations::RGB48ToBGR48(width, height, (uint16_t*)bmLckInfo.ptrDataRoi, bmLckInfo.stride);
			}
		});
	}
	catch (std::runtime_error& err)
	{
		// TODO: now what...?
		if (GetSite()->IsEnabled(LOGLEVEL_ERROR))
		{
			stringstream ss;
			ss << "Exception 'runtime_error' caught from JXR-decoder-invocation -> \"" << err.what() << "\".";
			GetSite()->Log(LOGLEVEL_ERROR, ss);
		}

		throw;
	}
	catch (std::exception& excp)
	{
		// TODO: now what...?
		if (GetSite()->IsEnabled(LOGLEVEL_ERROR))
		{
			stringstream ss;
			ss << "Exception caught from JXR-decoder-invocation -> \"" << excp.what() << "\".";
			GetSite()->Log(LOGLEVEL_ERROR, ss);
		}

		throw;
	}

	return bm;
}
