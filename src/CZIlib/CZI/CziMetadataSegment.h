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

class CCziMetadataSegment : public  libCZI::IMetadataSegment
{
private:
	std::shared_ptr<const void> spXmlData, spAttachment;
	std::uint64_t	xmlDataSize;
	std::uint32_t	attachmentSize;
public:
	CCziMetadataSegment(const CCZIParse::MetadataSegmentData& data, std::function<void(void*)> deleter);
	~CCziMetadataSegment() override;

	// interface ISubBlock
	void DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const override;
	std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize) override;
};