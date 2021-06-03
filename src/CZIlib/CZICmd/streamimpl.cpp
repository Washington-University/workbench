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
#include "streamimpl.h"
#include <cerrno>
#include "utils.h"

CStreamImpl::CStreamImpl(const wchar_t* filename)
{
    #if defined(WIN32ENV)
	errno_t err = _wfopen_s(&this->fp, filename, L"rb");
    #endif
    #if defined(LINUXENV)
	int/*error_t*/ err = 0;
	std::wstring wstrFname(filename);
	std::string utf8fname = convertToUtf8(wstrFname);
    this->fp = fopen(utf8fname.c_str(),"rb");
    if (!this->fp)
    {
        err = errno;
    }
    #endif
	if (err != 0)
	{
		throw std::logic_error("couldn't open file");
	}
}

CStreamImpl::CStreamImpl(const std::wstring& filename) : CStreamImpl(filename.c_str())
{
}

CStreamImpl::~CStreamImpl()
{
	if (this->fp != nullptr)
	{
		fclose(this->fp);
	}
}

/*virtual*/void CStreamImpl::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	std::uint64_t bytesRead;
    #if defined(WIN32ENV)
	int r = _fseeki64(this->fp, offset, SEEK_SET);
    #endif
    #if defined(LINUXENV)
	int r = fseeko(this->fp, offset, SEEK_SET);
    #endif
	if (r != 0)
	{
		throw std::logic_error("error seeking file");
	}

	bytesRead = fread(pv, 1, (size_t)size, this->fp);
	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}