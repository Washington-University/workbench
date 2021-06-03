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

#include "CziSubBlockDirectory.h"
#include "CziAttachmentsDirectory.h"
#include "CziDataStructs.h"
#include "libCZI.h"

class CCZIParse
{
private:
	static const std::uint8_t FILEHDRMAGIC[16];
	static const std::uint8_t SUBBLKDIRMAGIC[16];
	static const std::uint8_t SUBBLKMAGIC[16];
	static const std::uint8_t METADATASEGMENTMAGIC[16];
	static const std::uint8_t ATTACHMENTSDIRMAGC[16];
	static const std::uint8_t ATTACHMENTBLKMAGIC[16];
public:
	static CFileHeaderSegmentData ReadFileHeaderSegment(libCZI::IStream* str);

	static CCziSubBlockDirectory ReadSubBlockDirectory(libCZI::IStream* str, std::uint64_t offset);
	static CCziAttachmentsDirectory ReadAttachmentsDirectory(libCZI::IStream* str, std::uint64_t offset);

	static void ReadSubBlockDirectory(libCZI::IStream* str, std::uint64_t offset, CCziSubBlockDirectory& subBlkDir);

	struct SubBlockStorageAllocate
	{
		std::function<void*(size_t size)> alloc;
		std::function<void(void*)> free;
	};

	struct SubBlockData
	{
		void*			ptrData;
		std::uint64_t	dataSize;
		void*			ptrAttachment;
		std::uint32_t	attachmentSize;
		void*			ptrMetadata;
		std::uint32_t	metaDataSize;

		int						compression;
		int						pixelType;
		libCZI::CDimCoordinate	coordinate;
		libCZI::IntRect			logicalRect;
		libCZI::IntSize			physicalSize;
		int						mIndex;
		double					zoom;
	};

	static SubBlockData ReadSubBlock(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo);

	struct MetadataSegmentData
	{
		void*			ptrXmlData;
		std::uint64_t	xmlDataSize;
		void*			ptrAttachment;
		std::uint32_t	attachmentSize;
	};

	static MetadataSegmentData ReadMetadataSegment(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo);

	struct AttachmentData
	{
		void*			ptrData;
		std::uint64_t	dataSize;
	};

	static AttachmentData ReadAttachment(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo);
private:
	static void ParseThroughDirectoryEntries(int count, std::function<void(int, void*)> funcRead, std::function<void(const SubBlockDirectoryEntryDE*, const SubBlockDirectoryEntryDV*)> funcAddEntry);

	static void AddEntryToSubBlockDirectory(const SubBlockDirectoryEntryDE* subBlkDirDE, CCziSubBlockDirectory& subBlkDir);
	static void AddEntryToSubBlockDirectory(const SubBlockDirectoryEntryDV* subBlkDirDE, CCziSubBlockDirectory& subBlkDir);

	static libCZI::DimensionIndex DimensionCharToDimensionIndex(const char* ptr, size_t size);
	static bool IsMDimension(const char* ptr, size_t size);
	static bool IsXDimension(const char* ptr, size_t size);
	static bool IsYDimension(const char* ptr, size_t size);
	static char ToUpperCase(char c);

	static void ThrowNotEnoughDataRead(std::uint64_t offset, std::uint64_t bytesRequested, std::uint64_t bytesActuallyRead);
	static void ThrowIllegalData(std::uint64_t offset, const char* sz);
	static void ThrowIllegalData(const char* sz);

	static bool CheckAttachmentSchemaType(const char* p, size_t cnt);
};