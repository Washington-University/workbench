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

#include <functional>
#include "libCZI.h"
#include "CziSubBlockDirectory.h"
#include "CziAttachmentsDirectory.h"
#include "CziDataStructs.h"

class CCZIReader : public libCZI::ICZIReader, public std::enable_shared_from_this<CCZIReader>
{
private:
	std::shared_ptr<libCZI::IStream> stream;
	CFileHeaderSegmentData hdrSegmentData;
	CCziSubBlockDirectory subBlkDir;
	CCziAttachmentsDirectory attachmentDir;
	bool	isOperational;	///<	If true, then stream, hdrSegmentData and subBlkDir can be considered valid and operational
public:
	CCZIReader();
	~CCZIReader() override;

	// interface ISubBlockRepository
	void EnumerateSubBlocks(std::function<bool(int index, const libCZI::SubBlockInfo& info)> funcEnum) override;
	void EnumSubset(const libCZI::IDimCoordinate* planeCoordinate, const libCZI::IntRect* roi, bool onlyLayer0, std::function<bool(int index, const libCZI::SubBlockInfo& info)> funcEnum) override;
	std::shared_ptr<libCZI::ISubBlock> ReadSubBlock(int index) override;
	bool TryGetSubBlockInfoOfArbitrarySubBlockInChannel(int channelIndex, libCZI::SubBlockInfo& info) override;
	libCZI::SubBlockStatistics GetStatistics() override;
	libCZI::PyramidStatistics GetPyramidStatistics() override;
	
	// interface ICZIReader
	void Open(std::shared_ptr<libCZI::IStream> stream) override;
	std::shared_ptr<libCZI::IMetadataSegment> ReadMetadataSegment() override;
	std::shared_ptr<libCZI::IAccessor> CreateAccessor(libCZI::AccessorType accessorType) override;
	void Close() override;

	// interface IAttachmentRepository
	void EnumerateAttachments(std::function<bool(int index, const libCZI::AttachmentInfo& infi)> funcEnum) override;
	void EnumerateSubset(const char* contentFileType, const char* name, std::function<bool(int index, const libCZI::AttachmentInfo& infi)> funcEnum) override;
	std::shared_ptr<libCZI::IAttachment> ReadAttachment(int index) override;

private:
	std::shared_ptr<libCZI::ISubBlock> ReadSubBlock(const CCziSubBlockDirectory::SubBlkEntry& entry);
	std::shared_ptr<libCZI::IAttachment> ReadAttachment(const CCziAttachmentsDirectory::AttachmentEntry& entry);
	std::shared_ptr<libCZI::IMetadataSegment> ReadMetadataSegment(std::uint64_t position);

	void ThrowIfNotOperational();
	void SetOperationalState(bool operational);
};
