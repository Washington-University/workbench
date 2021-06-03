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

#include "inc_libCZI.h"

enum class SaveDataFormat
{
	Invalid,
	PNG
};

#if defined(WIN32ENV)
#include <Wincodec.h>

class CSaveData
{
private:
	std::wstring	fileName;
	SaveDataFormat	format;
public:
	CSaveData(const wchar_t* fileName, SaveDataFormat dataFormat);
	CSaveData(const std::wstring fileName, SaveDataFormat dataFormat) : CSaveData(fileName.c_str(), dataFormat) {};

	void	Save(libCZI::IBitmapData* bitmap);
private:
	void SaveWithWIC(const wchar_t* filename, const GUID encoder, const WICPixelFormatGUID& wicPixelFmt, libCZI::IBitmapData* bitmap);
	void SaveWithWIC(IWICImagingFactory* pFactory, IWICStream* destStream, const GUID encoder, const WICPixelFormatGUID& wicPixelFmt, libCZI::IBitmapData* bitmap);
};

#endif

#if defined(LINUXENV)
//#if !defined(EMSCRIPTEN)
	class CSaveData
	{
	private:
		std::wstring	fileName;
		SaveDataFormat	format;
	public:
		CSaveData(const wchar_t* fileName, SaveDataFormat dataFormat);
		CSaveData(const std::wstring fileName, SaveDataFormat dataFormat) : CSaveData(fileName.c_str(), dataFormat) {};

		void	Save(libCZI::IBitmapData* bitmap);
	private:
		void 	SaveBgr24(libCZI::IBitmapData* bitmap);
		void 	SaveBgr48(libCZI::IBitmapData* bitmap);
		void 	SaveGray16(libCZI::IBitmapData* bitmap);
		void 	SaveGray8(libCZI::IBitmapData* bitmap);
		void 	SaveBgra32(libCZI::IBitmapData* bitmap);
		void 	SavePng(libCZI::IBitmapData* bitmap, int bit_depth, int color_type);
		void 	SavePngTweakLineBeforeWritng(libCZI::IBitmapData* bitmap, int bit_depth, int color_type,
			std::function<void(std::uint32_t, void*)> tweakLine);
		FILE*	OpenDestForWrite();
		void 	ThrowIfNull(const void* p,const char* info);
	};
//#else
//class CSaveData
//{
//public:
//	CSaveData(const wchar_t* fileName, SaveDataFormat dataFormat){};
//	CSaveData(const std::wstring fileName, SaveDataFormat dataFormat) : CSaveData(fileName.c_str(), dataFormat) {};
//
//	void	Save(libCZI::IBitmapData* bitmap){};
//};
//#endif
#endif
