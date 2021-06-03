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
#include "CziAttachment.h"
#include "CziUtils.h"

using namespace libCZI;

CCziAttachment::CCziAttachment(const libCZI::AttachmentInfo& info, const CCZIParse::AttachmentData& data, std::function<void(void*)> deleter)
	:	
	spData(std::shared_ptr<const void>(data.ptrData, deleter)),
	dataSize(data.dataSize),
	info(info)
{
}

CCziAttachment::~CCziAttachment()
{
}

/*virtual*/const libCZI::AttachmentInfo& CCziAttachment::GetAttachmentInfo() const 
{
	return this->info;
}

/*virtual*/void CCziAttachment::DangerousGetRawData(const void*& ptr, size_t& size) const
{
	ptr = this->spData.get();
	size = (size_t)this->dataSize;	// TODO: check the cast
}

/*virtual*/std::shared_ptr<const void> CCziAttachment::GetRawData(size_t* ptrSize)
{
	if (ptrSize!=nullptr)
	{
		*ptrSize = (size_t)this->dataSize;
	}

	return this->spData;
}