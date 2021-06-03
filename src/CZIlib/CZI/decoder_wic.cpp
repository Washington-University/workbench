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
#if defined(_WIN32)
#include "decoder_wic.h"
#include "BitmapOperations.h"
#include <wincodec.h>

#include <atlbase.h>
#include "Site.h"

#pragma comment(lib, "Windowscodecs.lib")

using namespace std;
using namespace libCZI;

static void ThrowIfFailed(const char* function, HRESULT hr)
{
	if (FAILED(hr))
	{
		char errorMsg[255];
		_snprintf_s(errorMsg, _TRUNCATE, "COM-ERROR hr=0x%08X (%s)", hr, function);
		throw std::runtime_error(errorMsg);
	}
}

/*static*/std::shared_ptr<CWicJpgxrDecoder> CWicJpgxrDecoder::Create()
{
	CComPtr<IWICImagingFactory> cpWicImagingFactor;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&cpWicImagingFactor);
	ThrowIfFailed("Creating WICImageFactory", hr);

	return make_shared<CWicJpgxrDecoder>(cpWicImagingFactor);
}

CWicJpgxrDecoder::CWicJpgxrDecoder(IWICImagingFactory* pFactory)
{
	this->pFactory = pFactory;
	this->pFactory->AddRef();
}

CWicJpgxrDecoder::~CWicJpgxrDecoder()
{
	// DO NOT release the thing here because very likely we have already called CoUninitialize
	// before getting here, and then this call will crash
	//this->pFactory->Release();
}

static const char* GetInformativeString(const WICPixelFormatGUID& wicPxlFmt)
{
	static const struct
	{
		GUID wicPxlFmt;
		const char* szText;
	} WicPxlFmtAndName[] =
	{
		{ GUID_WICPixelFormatBlackWhite ,"WICPixelFormatBlackWhite" },
		{ GUID_WICPixelFormat8bppGray ,"WICPixelFormat8bppGray" },
		{ GUID_WICPixelFormat16bppBGR555 ,"WICPixelFormat16bppBGR555" },
		{ GUID_WICPixelFormat16bppGray ,"WICPixelFormat16bppGray" },
		{ GUID_WICPixelFormat24bppBGR ,"WICPixelFormat24bppBGR" },
		{ GUID_WICPixelFormat24bppRGB ,"WICPixelFormat24bppRGB" },
		{ GUID_WICPixelFormat32bppBGR ,"WICPixelFormat32bppBGR" },
		{ GUID_WICPixelFormat32bppBGRA ,"WICPixelFormat32bppBGRA" },
		{ GUID_WICPixelFormat48bppRGBFixedPoint ,"WICPixelFormat48bppRGBFixedPoint" },
		{ GUID_WICPixelFormat16bppGrayFixedPoint ,"WICPixelFormat16bppGrayFixedPoint" },
		{ GUID_WICPixelFormat32bppBGR101010  ,"WICPixelFormat32bppBGR101010" },
		{ GUID_WICPixelFormat48bppRGB,"WICPixelFormat48bppRGB" },
		{ GUID_WICPixelFormat64bppRGBA,"WICPixelFormat64bppRGBA" },
		{ GUID_WICPixelFormat96bppRGBFixedPoint,"WICPixelFormat96bppRGBFixedPoint" },
		{ GUID_WICPixelFormat96bppRGBFixedPoint,"WICPixelFormat96bppRGBFixedPoint" },
		{ GUID_WICPixelFormat128bppRGBFloat,"WICPixelFormat128bppRGBFloat" },
		{ GUID_WICPixelFormat32bppCMYK,"WICPixelFormat32bppCMYK" },
		{ GUID_WICPixelFormat64bppRGBAFixedPoint,"WICPixelFormat64bppRGBAFixedPoint" },
		{ GUID_WICPixelFormat128bppRGBAFixedPoint,"WICPixelFormat128bppRGBAFixedPoint" },
		{ GUID_WICPixelFormat64bppCMYK,"WICPixelFormat64bppCMYK" },
		{ GUID_WICPixelFormat24bpp3Channels,"WICPixelFormat24bpp3Channels" },
		{ GUID_WICPixelFormat32bpp4Channels,"WICPixelFormat32bpp4Channels" },
		{ GUID_WICPixelFormat40bpp5Channels,"WICPixelFormat40bpp5Channels" },
		{ GUID_WICPixelFormat48bpp6Channels,"WICPixelFormat48bpp6Channels" },
		{ GUID_WICPixelFormat56bpp7Channels,"WICPixelFormat56bpp7Channels" },
		{ GUID_WICPixelFormat64bpp8Channels,"WICPixelFormat64bpp8Channels" },
		{ GUID_WICPixelFormat48bpp3Channels,"WICPixelFormat48bpp3Channels" },
		{ GUID_WICPixelFormat64bpp4Channels,"WICPixelFormat64bpp4Channels" },
		{ GUID_WICPixelFormat80bpp5Channels,"WICPixelFormat80bpp5Channels" },
		{ GUID_WICPixelFormat96bpp6Channels,"WICPixelFormat96bpp6Channels" },
		{ GUID_WICPixelFormat112bpp7Channels,"WICPixelFormat112bpp7Channels" },
		{ GUID_WICPixelFormat128bpp8Channels,"WICPixelFormat128bpp8Channels" },
		{ GUID_WICPixelFormat40bppCMYKAlpha,"WICPixelFormat40bppCMYKAlpha" },
		{ GUID_WICPixelFormat80bppCMYKAlpha,"WICPixelFormat80bppCMYKAlpha" },
		{ GUID_WICPixelFormat32bpp3ChannelsAlpha,"WICPixelFormat32bpp3ChannelsAlpha" },
		{ GUID_WICPixelFormat64bpp7ChannelsAlpha,"WICPixelFormat64bpp7ChannelsAlpha" },
		{ GUID_WICPixelFormat72bpp8ChannelsAlpha,"WICPixelFormat72bpp8ChannelsAlpha" },
		{ GUID_WICPixelFormat64bpp3ChannelsAlpha,"WICPixelFormat64bpp3ChannelsAlpha" },
		{ GUID_WICPixelFormat80bpp4ChannelsAlpha,"WICPixelFormat80bpp4ChannelsAlpha" },
		{ GUID_WICPixelFormat96bpp5ChannelsAlpha,"WICPixelFormat96bpp5ChannelsAlpha" },
		{ GUID_WICPixelFormat112bpp6ChannelsAlpha,"WICPixelFormat112bpp6ChannelsAlpha" },
		{ GUID_WICPixelFormat128bpp7ChannelsAlpha,"WICPixelFormat128bpp7ChannelsAlpha" },
		{ GUID_WICPixelFormat144bpp8ChannelsAlpha,"WICPixelFormat144bpp8ChannelsAlpha" },
		{ GUID_WICPixelFormat64bppRGBAHalf,"WICPixelFormat64bppRGBAHalf" },
		{ GUID_WICPixelFormat48bppRGBHalf,"WICPixelFormat48bppRGBHalf" },
		{ GUID_WICPixelFormat32bppRGBE,"WICPixelFormat32bppRGBE" },
		{ GUID_WICPixelFormat16bppGrayHalf,"WICPixelFormat16bppGrayHalf" },
		{ GUID_WICPixelFormat32bppGrayFixedPoint,"WICPixelFormat32bppGrayFixedPoint" },
		{ GUID_WICPixelFormat64bppRGBFixedPoint,"WICPixelFormat64bppRGBFixedPoint" },
		{ GUID_WICPixelFormat128bppRGBFixedPoint,"xelFormat128bppRGBFixedPoint" },
		{ GUID_WICPixelFormat64bppRGBHalf,"GUID_WICPixelFormat64bppRGBHalf" }
	};

	for (size_t i = 0; i < sizeof(WicPxlFmtAndName) / sizeof(WicPxlFmtAndName[0]); ++i)
	{
		if (WicPxlFmtAndName[i].wicPxlFmt == wicPxlFmt)
		{
			return WicPxlFmtAndName[i].szText;
		}
	}

	return "Unknown";
}

static bool DeterminePixelType(const WICPixelFormatGUID& wicPxlFmt, GUID* destPixelFmt, PixelType* pxlType)
{
	static const struct
	{
		/// <summary>	The WIC-pixel-format as reported by the decoder. </summary>
		GUID wicPxlFmt;

		/// <summary>	The WIC-pixel-format  that we wish to get from the decoder (if necessary, utilizing a WIC-fomat converter). 
		/// 			If this has the value "GUID_WICPixelFormatUndefined" it means: I am not sure at this point, I have never 
		/// 			seen this and it is not obvious to me what to do. </summary>
		GUID wicDstPxlFmt;

		/// <summary>	The libCZI-pixelType which we finally want to end up with. </summary>
		PixelType pxlType;
	} WicPxlFmtAndPixelType[] =
	{
		{GUID_WICPixelFormatBlackWhite ,GUID_WICPixelFormat8bppGray ,PixelType::Gray8 },
		{GUID_WICPixelFormat8bppGray ,GUID_WICPixelFormat8bppGray ,PixelType::Gray8 },
		{GUID_WICPixelFormat16bppBGR555 ,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24},
		{GUID_WICPixelFormat16bppGray ,GUID_WICPixelFormat16bppGray,PixelType::Gray16},
		{GUID_WICPixelFormat24bppBGR ,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24},
		{GUID_WICPixelFormat24bppRGB ,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24 },
		{GUID_WICPixelFormat32bppBGR ,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24 },
		{GUID_WICPixelFormat32bppBGRA ,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24 },
		{GUID_WICPixelFormat48bppRGBFixedPoint ,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat16bppGrayFixedPoint ,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat32bppBGR101010  ,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat48bppRGB, GUID_WICPixelFormat48bppRGB, PixelType::Bgr48 },
		{GUID_WICPixelFormat64bppRGBA,GUID_WICPixelFormat48bppRGB, PixelType::Bgr48 },
		{GUID_WICPixelFormat96bppRGBFixedPoint,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat96bppRGBFixedPoint,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat128bppRGBFloat,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat32bppCMYK,GUID_WICPixelFormat24bppBGR,PixelType::Bgr24 },
		{GUID_WICPixelFormat64bppRGBAFixedPoint,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat128bppRGBAFixedPoint,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat64bppCMYK,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat24bpp3Channels,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat32bpp4Channels,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat40bpp5Channels,GUID_WICPixelFormatUndefined , PixelType::Invalid },
		{GUID_WICPixelFormat48bpp6Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat56bpp7Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bpp8Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat48bpp3Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bpp4Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat80bpp5Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat96bpp6Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat112bpp7Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat128bpp8Channels, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat40bppCMYKAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat80bppCMYKAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat32bpp3ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bpp7ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat72bpp8ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bpp3ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat80bpp4ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat96bpp5ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat112bpp6ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat128bpp7ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat144bpp8ChannelsAlpha, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bppRGBAHalf, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat48bppRGBHalf, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat32bppRGBE, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat16bppGrayHalf, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat32bppGrayFixedPoint, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bppRGBFixedPoint, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat128bppRGBFixedPoint, GUID_WICPixelFormatUndefined, PixelType::Invalid },
		{GUID_WICPixelFormat64bppRGBHalf,  GUID_WICPixelFormatUndefined, PixelType::Invalid }
	};

	for (size_t i = 0; i < sizeof(WicPxlFmtAndPixelType) / sizeof(WicPxlFmtAndPixelType[0]); ++i)
	{
		if (WicPxlFmtAndPixelType[i].wicPxlFmt == wicPxlFmt)
		{
			if (destPixelFmt != nullptr) { *destPixelFmt = WicPxlFmtAndPixelType[i].wicDstPxlFmt; }
			if (pxlType != nullptr) { *pxlType = WicPxlFmtAndPixelType[i].pxlType; }
			return true;
		}
	}

	return false;
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CWicJpgxrDecoder::Decode(const void* ptrData, size_t size)
{
	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss; ss << "Begin WIC-JpgXR-Decode with " << size << " bytes";
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
	}

	CComPtr<IWICStream> cpWicStream;
	HRESULT hr = this->pFactory->CreateStream(&cpWicStream);
	ThrowIfFailed("pFactory->CreateStream", hr);

	// Initialize the stream with the memory pointer and size.
	hr = cpWicStream->InitializeFromMemory((BYTE*)(ptrData), (DWORD)size);
	ThrowIfFailed("wicStream->InitializeFromMemory", hr);

	CComPtr<IWICBitmapDecoder> cpWicBitmapDecoder;
	hr = this->pFactory->CreateDecoderFromStream(
		cpWicStream,
		NULL,/*decoder vendor*/
		WICDecodeMetadataCacheOnDemand,
		&cpWicBitmapDecoder);
	ThrowIfFailed("pFactory->CreateDecoderFromStream", hr);

	CComPtr<IWICBitmapFrameDecode> cpWicBitmapFrameDecode;
	hr = cpWicBitmapDecoder->GetFrame(0, &cpWicBitmapFrameDecode);
	ThrowIfFailed("wicBitmapDecoder->GetFrame", hr);

	IntSize sizeBitmap;
	WICPixelFormatGUID wicPxlFmt;
	WICPixelFormatGUID wicDestPxlFmt;
	PixelType px_type;
	hr = cpWicBitmapFrameDecode->GetPixelFormat(&wicPxlFmt);
	ThrowIfFailed("wicBitmapFrameDecode->GetPixelFormat", hr);

	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss; ss << " Encoded PixelFormat:" << GetInformativeString(wicPxlFmt);
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
	}

	bool b = DeterminePixelType(wicPxlFmt, &wicDestPxlFmt, &px_type);
	if (b == false || wicDestPxlFmt == GUID_WICPixelFormatUndefined)
	{
		throw  std::logic_error("need to look into these formats...");
	}

	hr = cpWicBitmapFrameDecode->GetSize(&sizeBitmap.w, &sizeBitmap.h);
	ThrowIfFailed("wicBitmapFrameDecode->GetSize", hr);

	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		stringstream ss; ss << " Requested Decoded PixelFormat:" << GetInformativeString(wicDestPxlFmt) << " Width:" << sizeBitmap.w << " Height:" << sizeBitmap.h;
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, ss.str());
	}

	auto bm = GetSite()->CreateBitmap(px_type, sizeBitmap.w, sizeBitmap.h);
	auto bmLckInfo = ScopedBitmapLockerSP(bm);

	if (wicPxlFmt == wicDestPxlFmt)
	{
		// in this case we do not need to create a converter
		hr = cpWicBitmapFrameDecode->CopyPixels(NULL, bmLckInfo.stride, bmLckInfo.stride*sizeBitmap.h, (BYTE*)bmLckInfo.ptrDataRoi);
		ThrowIfFailed("wicBitmapFrameDecode->CopyPixels", hr);
	}
	else
	{
		CComPtr<IWICFormatConverter> pFormatConverter;
		hr = this->pFactory->CreateFormatConverter(&pFormatConverter);
		ThrowIfFailed("pFactory->CreateFormatConverter", hr);
		hr = pFormatConverter->Initialize(
			cpWicBitmapFrameDecode,          // Input bitmap to convert
			wicDestPxlFmt,				     // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			nullptr,                         // Specify a particular palette 
			0,                               // Alpha threshold
			WICBitmapPaletteTypeCustom);     // Palette translation type
		ThrowIfFailed("pFormatConverter->Initialize", hr);
		hr = pFormatConverter->CopyPixels(NULL, bmLckInfo.stride, bmLckInfo.stride*sizeBitmap.h, (BYTE*)bmLckInfo.ptrDataRoi);
		ThrowIfFailed("pFormatConverter->CopyPixels", hr);
	}

	// WIC-codec does not directly support "BGR48", so we need to convert (#36)
	if (px_type == PixelType::Bgr48)
	{
		CBitmapOperations::RGB48ToBGR48(sizeBitmap.w, sizeBitmap.h, (uint16_t*)bmLckInfo.ptrDataRoi, bmLckInfo.stride);
	}

	if (GetSite()->IsEnabled(LOGLEVEL_CHATTYINFORMATION))
	{
		GetSite()->Log(LOGLEVEL_CHATTYINFORMATION, "Finished WIC-JpgXR-Decode");
	}

	return bm;
}

#endif
