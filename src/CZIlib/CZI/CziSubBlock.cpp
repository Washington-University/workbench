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
#include "CziSubBlock.h"
#include "CziUtils.h"

using namespace libCZI;

CCziSubBlock::CCziSubBlock(const libCZI::SubBlockInfo& info, const CCZIParse::SubBlockData& data, std::function<void(void*)> deleter)
	:
	spData(std::shared_ptr<const void>(data.ptrData, deleter)),
	spAttachment(std::shared_ptr<const void>(data.ptrAttachment, deleter)),
	spMetadata(std::shared_ptr<const void>(data.ptrMetadata, deleter)),
	dataSize(data.dataSize),
	attachmentSize(data.attachmentSize),
	metaDataSize(data.metaDataSize),
	info(info)
{
}

CCziSubBlock::~CCziSubBlock()
{
}

/*virtual*/const SubBlockInfo& CCziSubBlock::GetSubBlockInfo() const
{
	return this->info;
}

/*virtual*/void CCziSubBlock::DangerousGetRawData(ISubBlock::MemBlkType type, const void*& ptr, size_t& size) const
{
	switch (type)
	{
	case Metadata:
		ptr = this->spMetadata.get();
		size = (size_t)this->metaDataSize;	// TODO: check the cast
		break;
	case Data:
		ptr = this->spData.get();
		size = (size_t)this->dataSize;
		break;
	case Attachment:
		ptr = this->spAttachment.get();
		size = (size_t)this->attachmentSize;
		break;
	default:
		throw std::logic_error("illegal value for type");
	}
}

/*virtual*/std::shared_ptr<const void> CCziSubBlock::GetRawData(MemBlkType type, size_t* ptrSize)
{
	switch (type)
	{
	case Metadata:
		if (ptrSize != nullptr)
		{
			*ptrSize = this->metaDataSize;
		}

		return this->spMetadata;
	case Data:
		if (ptrSize != nullptr)
		{
			*ptrSize = (size_t)this->dataSize;// TODO: check the cast
		}

		return this->spData;
	case Attachment:
		if (ptrSize != nullptr)
		{
			*ptrSize = this->attachmentSize;
		}

		return this->spAttachment;
	default:
		throw std::logic_error("illegal value for type");
	}
}

/*virtual*/std::shared_ptr<IBitmapData> CCziSubBlock::CreateBitmap()
{
	return CreateBitmapFromSubBlock(this);
}
