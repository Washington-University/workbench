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
#include "CziMetadataSegment.h"
#include "CziUtils.h"

using namespace libCZI;

CCziMetadataSegment::CCziMetadataSegment(const CCZIParse::MetadataSegmentData& data, std::function<void(void*)> deleter)
	:
	spXmlData(std::shared_ptr<const void>(data.ptrXmlData, deleter)),
	spAttachment(std::shared_ptr<const void>(data.ptrAttachment, deleter)),
	xmlDataSize(data.xmlDataSize),
	attachmentSize(data.attachmentSize)
{
}

CCziMetadataSegment::~CCziMetadataSegment()
{}

// interface ISubBlock
/*virtual*/void CCziMetadataSegment::DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const
{
	switch (type)
	{
	case XmlMetadata:
		ptr = this->spXmlData.get();
		size = (size_t)this->xmlDataSize;
		break;
	case Attachment:
		ptr = this->spAttachment.get();
		size = this->attachmentSize;
		break;
	default:
		throw std::logic_error("illegal value for type");
	}
}

/*virtual*/std::shared_ptr<const void> CCziMetadataSegment::GetRawData(MemBlkType type, size_t* ptrSize)
{
	switch (type)
	{
	case XmlMetadata:
		if (ptrSize != nullptr)
		{
			*ptrSize = (size_t)this->xmlDataSize;
		}

		return this->spXmlData;
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
