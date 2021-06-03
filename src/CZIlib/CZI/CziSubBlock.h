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

#include "libCZI.h"
#include "CziParse.h"

class CCziSubBlock : public  libCZI::ISubBlock
{
private:
	std::shared_ptr<const void> spData, spAttachment, spMetadata;
	std::uint64_t	dataSize;
	std::uint32_t	attachmentSize;
	std::uint32_t	metaDataSize;
	libCZI::SubBlockInfo	info;
public:
	CCziSubBlock(const libCZI::SubBlockInfo& info,const CCZIParse::SubBlockData& data, std::function<void(void*)> deleter);
	~CCziSubBlock() override;

	// interface ISubBlock
	const libCZI::SubBlockInfo& GetSubBlockInfo() const override;
	void DangerousGetRawData(libCZI::ISubBlock::MemBlkType type, const void*& ptr, size_t& size) const override;
	std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize) override;
	std::shared_ptr<libCZI::IBitmapData> CreateBitmap() override;
};
