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
#include "CZIReader.h"
#include "CziParse.h"
#include "CziSubBlock.h"
#include "CziMetadataSegment.h"
#include "CziUtils.h"
#include "utilities.h"
#include "CziAttachment.h"

using namespace std;
using namespace libCZI;

CCZIReader::CCZIReader() : isOperational(false)
{
}

CCZIReader::~CCZIReader()
{
}

/*virtual */void CCZIReader::Open(std::shared_ptr<IStream> stream)
{
	if (this->isOperational == true)
	{
		throw logic_error("CZIReader is already operational.");
	}

	this->hdrSegmentData = CCZIParse::ReadFileHeaderSegment(stream.get());
	this->subBlkDir = std::move(CCZIParse::ReadSubBlockDirectory(stream.get(), this->hdrSegmentData.GetSubBlockDirectoryPosition()));
	auto attachmentPos = this->hdrSegmentData.GetAttachmentDirectoryPosition();
	if (attachmentPos != 0)
	{
		// we should be operational without an attachment-directory as well I suppose.
		// TODO: how to determine whether there is "no attachment-directory" - is the check for 0 sufficient?
		this->attachmentDir = std::move(CCZIParse::ReadAttachmentsDirectory(stream.get(), attachmentPos));
	}

	this->stream = stream;
	this->SetOperationalState(true);
}

/*virtual*/std::shared_ptr<libCZI::IMetadataSegment> CCZIReader::ReadMetadataSegment()
{
	this->ThrowIfNotOperational();
	return this->ReadMetadataSegment(this->hdrSegmentData.GetMetadataPosition());
}

/*virtual*/SubBlockStatistics CCZIReader::GetStatistics()
{
	this->ThrowIfNotOperational();
	SubBlockStatistics s = this->subBlkDir.GetStatistics();
	return s;
}

/*virtual*/libCZI::PyramidStatistics CCZIReader::GetPyramidStatistics()
{
	this->ThrowIfNotOperational();
	return this->subBlkDir.GetPyramidStatistics();
}

/*virtual*/void CCZIReader::EnumerateSubBlocks(std::function<bool(int index, const SubBlockInfo& info)> funcEnum)
{
	this->ThrowIfNotOperational();
	this->subBlkDir.EnumSubBlocks(
		[&](int index, const CCziSubBlockDirectory::SubBlkEntry& entry)->bool
	{
		SubBlockInfo info;
		info.mode = CziUtils::CompressionModeFromInt(entry.Compression);
		info.pixelType = CziUtils::PixelTypeFromInt(entry.PixelType);
		info.coordinate = entry.coordinate;
		info.logicalRect = IntRect{ entry.x,entry.y,entry.width,entry.height };
		info.physicalSize = IntSize{ std::uint32_t(entry.storedWidth), std::uint32_t(entry.storedHeight) };
		info.mIndex = entry.mIndex;
		return funcEnum(index, info);
	});
}

/*virtual*/void CCZIReader::EnumSubset(const IDimCoordinate* planeCoordinate, const IntRect* roi, bool onlyLayer0, std::function<bool(int index, const SubBlockInfo& info)> funcEnum)
{
	this->ThrowIfNotOperational();

	// TODO:
	// Ok... for a first tentative, experimental and quick-n-dirty implementation, simply
	//      walk through all the subblocks. We surely want to have something more elaborated
	//      here.
	this->EnumerateSubBlocks(
		[&](int index, const SubBlockInfo& info)->bool
	{
		// TODO: we only deal with layer 0 currently... or, more precisely, we do not take "zoom" into account at all
		//        -> well... added that boolean "onlyLayer0" - is this sufficient...?
		if (onlyLayer0 == false || (info.physicalSize.w == info.logicalRect.w && info.physicalSize.h == info.logicalRect.h))
		{
			if (planeCoordinate == nullptr || CziUtils::CompareCoordinate(planeCoordinate, &info.coordinate) == true)
			{
				if (roi == nullptr || Utilities::DoIntersect(*roi, info.logicalRect))
				{
					bool b = funcEnum(index, info);
					return b;
				}
			}
		}

		return true;
	});
}

/*virtual*/std::shared_ptr<ISubBlock> CCZIReader::ReadSubBlock(int index)
{
	this->ThrowIfNotOperational();
	CCziSubBlockDirectory::SubBlkEntry entry;
	if (this->subBlkDir.TryGetSubBlock(index, entry) == false)
	{
		return std::shared_ptr<ISubBlock>();
	}

	return this->ReadSubBlock(entry);
}

/*virtual*/bool CCZIReader::TryGetSubBlockInfoOfArbitrarySubBlockInChannel(int channelIndex, SubBlockInfo& info)
{
	this->ThrowIfNotOperational();

	// TODO: we should be able to gather this information when constructing the subblock-list
	//  for the time being... just walk through the whole list
	//  
	bool foundASubBlock = false;
	SubBlockStatistics s = this->subBlkDir.GetStatistics();
	if (!s.dimBounds.IsValid(DimensionIndex::C))
	{
		// in this case -> just take the first subblock...
		this->EnumerateSubBlocks(
			[&](int index, const SubBlockInfo& sbinfo)->bool
		{
			info = sbinfo;
			foundASubBlock = true;
			return false;
		});
	}
	else
	{
		this->EnumerateSubBlocks(
			[&](int index, const SubBlockInfo& sbinfo)->bool
		{
			int c;
			if (sbinfo.coordinate.TryGetPosition(DimensionIndex::C, &c) == true && c == channelIndex)
			{
				info = sbinfo;
				foundASubBlock = true;
				return false;
			}

			return true;
		});
	}

	return foundASubBlock;
}

/*virtual*/std::shared_ptr<libCZI::IAccessor> CCZIReader::CreateAccessor(libCZI::AccessorType accessorType)
{
	this->ThrowIfNotOperational();
	return CreateAccesor(this->shared_from_this(), accessorType);
}

/*virtual*/void CCZIReader::Close()
{
	this->ThrowIfNotOperational();
	this->SetOperationalState(false);
	this->stream.reset();
}

/*virtual*/void CCZIReader::EnumerateAttachments(std::function<bool(int index, const libCZI::AttachmentInfo& info)> funcEnum)
{
	this->ThrowIfNotOperational();
	libCZI::AttachmentInfo ai;
	ai.contentFileType[sizeof(ai.contentFileType) - 1] = '\0';
	this->attachmentDir.EnumAttachments(
		[&](int index, const CCziAttachmentsDirectory::AttachmentEntry& ae)
	{
		ai.contentGuid = ae.ContentGuid;
		memcpy(ai.contentFileType, ae.ContentFileType, sizeof(ae.ContentFileType));
		ai.name = ae.Name;
		bool b = funcEnum(index, ai);
		return b;
	});
}
/*virtual*/void CCZIReader::EnumerateSubset(const char* contentFileType, const char* name, std::function<bool(int index, const libCZI::AttachmentInfo& infi)> funcEnum)
{
	this->ThrowIfNotOperational();
	libCZI::AttachmentInfo ai;
	ai.contentFileType[sizeof(ai.contentFileType) - 1] = '\0';
	this->attachmentDir.EnumAttachments(
		[&](int index, const CCziAttachmentsDirectory::AttachmentEntry& ae)
	{
		if (contentFileType == nullptr || strcmp(contentFileType, ae.ContentFileType) == 0)
		{
			if (name == nullptr || strcmp(name, ae.Name) == 0)
			{
				ai.contentGuid = ae.ContentGuid;
				memcpy(ai.contentFileType, ae.ContentFileType, sizeof(ae.ContentFileType));
				ai.name = ae.Name;
				bool b = funcEnum(index, ai);
				return b;
			}
		}

		return true;
	});
}

/*virtual*/std::shared_ptr<libCZI::IAttachment> CCZIReader::ReadAttachment(int index)
{
	this->ThrowIfNotOperational();
	CCziAttachmentsDirectory::AttachmentEntry entry;
	if (this->attachmentDir.TryGetAttachment(index, entry) == false)
	{
		return std::shared_ptr<IAttachment>();
	}

	return this->ReadAttachment(entry);
}


std::shared_ptr<ISubBlock> CCZIReader::ReadSubBlock(const CCziSubBlockDirectory::SubBlkEntry& entry)
{
	CCZIParse::SubBlockStorageAllocate allocateInfo{ malloc,free };

	auto subBlkData = CCZIParse::ReadSubBlock(this->stream.get(), entry.FilePosition, allocateInfo);

	libCZI::SubBlockInfo info;
	info.pixelType = CziUtils::PixelTypeFromInt(subBlkData.pixelType);
	info.mode = CziUtils::CompressionModeFromInt(subBlkData.compression);
	info.coordinate = subBlkData.coordinate;
	info.mIndex = subBlkData.mIndex;
	info.logicalRect = subBlkData.logicalRect;
	info.physicalSize = subBlkData.physicalSize;

	return std::make_shared<CCziSubBlock>(info, subBlkData, free);
}

std::shared_ptr<libCZI::IAttachment> CCZIReader::ReadAttachment(const CCziAttachmentsDirectory::AttachmentEntry& entry)
{
	CCZIParse::SubBlockStorageAllocate allocateInfo{ malloc,free };

	auto attchmnt = CCZIParse::ReadAttachment(this->stream.get(), entry.FilePosition, allocateInfo);
	libCZI::AttachmentInfo attchmentInfo;
	attchmentInfo.contentGuid = entry.ContentGuid;
	static_assert(sizeof(attchmentInfo.contentFileType) > sizeof(entry.ContentFileType), "sizeof(attchmentInfo.contentFileType) must be greater than sizeof(entry.ContentFileType)");
	memcpy(attchmentInfo.contentFileType, entry.ContentFileType, sizeof(entry.ContentFileType));
	attchmentInfo.contentFileType[sizeof(entry.ContentFileType)] = '\0';
	attchmentInfo.name = entry.Name;

	return std::make_shared<CCziAttachment>(attchmentInfo, attchmnt, allocateInfo.free);
}

std::shared_ptr<libCZI::IMetadataSegment> CCZIReader::ReadMetadataSegment(std::uint64_t position)
{
	CCZIParse::SubBlockStorageAllocate allocateInfo{ malloc,free };

	auto metaDataSegmentData = CCZIParse::ReadMetadataSegment(this->stream.get(), position, allocateInfo);
	return std::make_shared<CCziMetadataSegment>(metaDataSegmentData, free);
}

void CCZIReader::ThrowIfNotOperational()
{
	if (this->isOperational == false)
	{
		throw logic_error("CZReader is not operational (must call 'Open' first)");
	}
}

void CCZIReader::SetOperationalState(bool operational)
{
	this->isOperational = operational;
}