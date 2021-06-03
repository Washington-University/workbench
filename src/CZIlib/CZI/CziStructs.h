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

// TODO: we need a platform-independent replacement for GUID

#if !defined(__GNUC__)
 #include <pshpack2.h>
#define PACKED 
#else
 #define PACKED __attribute__ ((__packed__))
#endif

/////////////////////////////////////////////////////////////////////////////////
// Enumerations
/////////////////////////////////////////////////////////////////////////////////

enum class PixelType : std::uint8_t
{
	Gray8 = 0,
	Gray16 = 1,
	Gray32Float = 2, Bgr24 = 3,
	Bgr48 = 4,
	Bgr96Float = 8,
	Bgra32 = 9,
	Gray64ComplexFloat = 10,
	Bgr192ComplexFloat = 11,
	Gray32 = 12,
	Gray64Float = 13,
};

////////////////////////////////////////////////////////////////////
// STRUCTURES
////////////////////////////////////////////////////////////////////


typedef struct PACKED DimensionEntry
{
	char Dimension[4];
	int	Start;
	int Size;
	float StartCoordinate;
	int StoredSize;
} DIMENSIONENTRY;

typedef struct PACKED AttachmentInfo
{
	std::int64_t AllocatedSize;
	std::int64_t DataSize;
	int FilePart;
	GUID ContentGuid;
	char ContentFileType[8];
	char Name[80];
	//HANDLE FileHandle;
	unsigned char spare[128];
} ATTACHMENTINFO;

typedef struct PACKED MetadataInfo
{
	std::int64_t AllocatedSize;
	int XmlSize;
	int BinarySize;
} METADATAINFO;

typedef struct PACKED AttachmentDirectoryInfo
{
	int EntryCount;
	//HANDLE* attachmentHandles;
} ATTACHMENTDIRECTORYINFO;

////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////

struct PACKED SegmentHeader
{
	unsigned char Id[16];
	std::int64_t AllocatedSize;
	std::int64_t UsedSize;
};

// defined segment algignments (never modify this constants!)
const int SEGMENT_ALIGN = 32;

// Sizes of segment parts (never modify this constants!)
const int SIZE_SEGMENTHEADER = 32;
const int SIZE_SEGMENTID = 16;
const int SIZE_SUBBLOCKDIRECTORYENTRY_DE = 128;
const int SIZE_ATTACHMENTENTRY = 128;
const int SIZE_SUBBLOCKDIRECTORYENTRY_DV_FIXEDPART = 32;

// Data section within segments (never modify this constants!)
const int SIZE_FILEHEADER_DATA = 512;
const int SIZE_METADATA_DATA = 256;
const int SIZE_SUBBLOCKDATA_MINIMUM = 256;
const int SIZE_SUBBLOCKDATA_FIXEDPART = 16;
const int SIZE_SUBBLOCKDIRECTORY_DATA = 128;
const int SIZE_ATTACHMENTDIRECTORY_DATA = 256;
const int SIZE_ATTACHMENT_DATA = 256;
const int SIZE_DIMENSIONENTRYDV = 20;

// internal implementation limits (internal use of pre-allocated structures)
// re-dimension if more items needed
const int MAXDIMENSIONS = 40;
//#define MAXFILE 50000
//
//#define ATTACHMENT_SPARE 2048

////////////////////////////////////////////////////////////////////
// SCHEMAS
////////////////////////////////////////////////////////////////////

// FileHeader

struct PACKED FileHeaderSegmentData
{
	int Major;
	int Minor;
	int _Reserved1;
	int _Reserved2;
	GUID PrimaryFileGuid;
	GUID FileGuid;
	int FilePart;
	std::int64_t SubBlockDirectoryPosition;
	std::int64_t MetadataPosition;
	int updatePending;
	std::int64_t AttachmentDirectoryPosition;
	unsigned char _spare[SIZE_FILEHEADER_DATA - 80];  // offset 80
};

// SubBlockDirectory - Entry: DE fixed size 256 bytes

struct PACKED SubBlockDirectoryEntryDE
{
	unsigned char SchemaType[2];
	int PixelType;
	int SizeXStored;
	int SizeYStored;
	unsigned char _pad[2];
	int StartX;		// offset 16
	int SizeX;
	int StartY;
	int SizeY;
	int StartC;
	int SizeC;
	int StartZ;
	int SizeZ;
	int StartT;
	int SizeT;
	int StartS;
	int StartR;
	int StartI;
	int StartB;
	int Compression;
	int StartM;
	std::int64_t FilePosition;
	int FilePart;
	unsigned char DimensionOrder[16];
	int StartH;
	int Start10;
	int Start11;
	int Start12;
	int Start13;
};

// SubBlockDirectory - Entry: DV variable length - mimimum of 256 bytes

// same structure for Dimension entries as used in public API
struct PACKED DimensionEntryDV : DIMENSIONENTRY
{
};

struct PACKED SubBlockDirectoryEntryDV
{
	unsigned char SchemaType[2];
	int PixelType;
	std::int64_t FilePosition;
	int FilePart;
	int Compression;
	unsigned char _spare[6];
	int DimensionCount;

	// max. allocation for ease of use (valid size = 32 + EntryCount * 20)
	struct DimensionEntryDV DimensionEntries[MAXDIMENSIONS]; // offset 32
};

struct PACKED SubBlockDirectorySegmentData
{
	int EntryCount;
	unsigned char _spare[SIZE_SUBBLOCKDIRECTORY_DATA - 4];
	// followed by any sequence of SubBlockDirectoryEntryDE or SubBlockDirectoryEntryDV records;
};

///////////////////////////////////////////////////////////////////////////////////
// Attachment

struct PACKED AttachmentEntryA1
{
	unsigned char SchemaType[2];
	unsigned char _spare[10];
	std::int64_t FilePosition;
	int FilePart;
	GUID ContentGuid;
	unsigned char ContentFileType[8];
	unsigned char Name[80];
};

struct PACKED AttachmentSegmentData
{
	std::int64_t DataSize;
	unsigned char _spare[8];
	union
	{
		std::uint8_t reserved[SIZE_ATTACHMENTENTRY];
		struct AttachmentEntryA1 entry;		// offset 16
	};
	unsigned char _spare2[SIZE_ATTACHMENT_DATA - SIZE_ATTACHMENTENTRY - 16];
};

struct PACKED AttachmentDirectorySegmentData
{
	int EntryCount;
	unsigned char _spare[SIZE_ATTACHMENTDIRECTORY_DATA - 4];
	// followed by => AttachmentEntry entries[EntryCount];
};


///////////////////////////////////////////////////////////////////////////////////
// SubBlock

struct PACKED SubBlockSegmentData
{
	int MetadataSize;
	int AttachmentSize;
	std::int64_t DataSize;
	union PACKED
	{
		unsigned char _spare[SIZE_SUBBLOCKDATA_MINIMUM - SIZE_SUBBLOCKDATA_FIXEDPART];  // offset 16
		unsigned char entrySchema[2];
		SubBlockDirectoryEntryDV entryDV;
		SubBlockDirectoryEntryDE entryDE;
	};
};

///////////////////////////////////////////////////////////////////////////////////
// Metadata

struct PACKED MetadataSegmentData
{
	int XmlSize;
	int AttachmentSize;
	unsigned char _spare[SIZE_METADATA_DATA - 8];
};


////////////////////////////////////////////////////////////////////
// SEGMENTS
////////////////////////////////////////////////////////////////////

// SubBlockDirectorySegment: size = [128 bytes fixed (or variable if DV)] + MetadataSize + AttachmentSize + DataSize
struct PACKED SubBlockSegment
{
	struct SegmentHeader header;
	struct SubBlockSegmentData data;
};

// SubBlockDirectorySegment: size = 128(fixed) + EntryCount * [128 bytes fixed (or variable if DV)]
struct PACKED SubBlockDirectorySegment
{
	struct SegmentHeader header;
	struct SubBlockDirectorySegmentData data;
};

// MetdataSegment: size = 128(fixed) + dataLength
struct PACKED MetadataSegment
{
	struct SegmentHeader header;
	struct MetadataSegmentData data;
};

// AttachmentDirectorySegment: size = 256(fixed) + EntryCount * 128(fixed)
struct PACKED AttachmentDirectorySegment
{
	struct SegmentHeader header;
	struct AttachmentDirectorySegmentData data;
};

// AttachmentSegment: size = 256(fixed)
struct PACKED AttachmentSegment
{
	struct SegmentHeader header;
	struct AttachmentSegmentData data;
};

// FileHeaderSegment: size = 512(fixed)
struct PACKED FileHeaderSegment
{
	struct SegmentHeader header;
	struct FileHeaderSegmentData data;
};

#if !defined(__GNUC__)
 #include <poppack.h>
#else
 #define PACK
#endif