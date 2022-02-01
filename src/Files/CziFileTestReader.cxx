
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __CZI_FILE_TEST_READER_DECLARE__
#include "CziFileTestReader.h"
#undef __CZI_FILE_TEST_READER_DECLARE__

#include <array>

#include <QFile>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"

using namespace caret;



/**
 * \class caret::CziFileTestReader
 * \brief Test reading of CZI file to understand the file structure
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziFileTestReader::CziFileTestReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
CziFileTestReader::~CziFileTestReader()
{
}

/**
 * Test reading a CZI file and print info to terminal.
 * @param filename
 *    Name of file.
 */
void
CziFileTestReader::testReading(const AString& filename)
{
    try {
        CziFileTestReader reader;
        reader.readFile(filename);
    }
    catch (const DataFileException& dfe) {
        std::cout << "Failed to read file " << dfe.whatString() << std::endl;
    }
}


/**
 * Read a CZI file.
 * @param filename
 *    Name of file.
 * @throws DataFileException
 *    If there is an error.
 */
void
CziFileTestReader::readFile(const AString& filename)
{
    if (filename.isEmpty()) {
        throw DataFileException("Filename is empty");
    }
    
    QFile file(filename);
    
    if ( ! file.exists()) {
        throw DataFileException(filename + " does not exist");
    }
    
    if ( ! file.open(QFile::ReadOnly)) {
        throw DataFileException(file.errorString());
    }
    
    /*
     * CZI file is little ending (spec 3.1)
     * Must also set precision of floating point numbers (default is Double, 64 bit)
     */
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    
    readAllSegments(file,
                    dataStream);
    
}

/**
 * Read all segments from the file.
 * @param file
 *    File to read from.
 */
void
CziFileTestReader::readAllSegments(QFile& file,
                               QDataStream& dataStream)
{
    int32_t segmentHeaderCount(0);
    
    AString errorMessages;
    AString warningMessages;
    
    while ( ! file.atEnd()) {
        
        AString segmentErrorMessage;
        AString segmentWarningMessage;
        const SegmentBase* segment(readNextSegment(file,
                                                   dataStream,
                                                   segmentErrorMessage,
                                                   segmentWarningMessage));

        if (segment == NULL) {
            throw DataFileException("Error reading segment header index="
                                    + AString::number(segmentHeaderCount)
                                    + ": "
                                    + segmentErrorMessage);
        }
        else if ( ! segmentErrorMessage.isEmpty()) {
            errorMessages.appendWithNewLine(segmentErrorMessage);
        }
        else if ( ! segmentWarningMessage.isEmpty()) {
            warningMessages.appendWithNewLine(segmentWarningMessage);
        }
        
//        switch (segment->m_segmentType) {
//            case SegmentBase::SegmentType::FILE_HEADER:
//                break;
//            case SegmentBase::SegmentType::ATTACHMENT:
//                break;
//            case SegmentBase::SegmentType::ATTACHMENT_DIRECTORY:
//                break;
//            case SegmentBase::SegmentType::DELETED:
//                break;
//            case SegmentBase::SegmentType::METADATA:
//                break;
//            case SegmentBase::SegmentType::SUB_BLOCK:
//                break;
//            case SegmentBase::SegmentType::SUB_BLOCK_DIRECTORY:
//                break;
//            case SegmentBase::SegmentType::UNKNOWN:
//                break;
//        }
        //        ": id=" << segmentInfo->m_id
        //        << ", allocated=" << segmentInfo->m_allocatedSize
        //        << ", used=" << segmentInfo->m_usedSize
        //        << ", fileOffset=" << segmentInfo->m_fileOffset << std::endl;
        
        segmentHeaderCount++;
        
        
        const int64_t nextSegmentOffset(segment->m_fileOffset
                                        + s_SEGMENT_HEADER_LENGTH
                                        + segment->m_allocatedSize);
        if ( ! file.seek(nextSegmentOffset)) {
            throw DataFileException("Failed to seek to offset="
                                    + AString::number(nextSegmentOffset));
        }
        //        break;
    }
    
    if (errorMessages.isEmpty()) {
        if ( ! m_fileHeaderSegment) {
            errorMessages.appendWithNewLine("File header segment is missing. ");
        }
    }
    if ( ! errorMessages.isEmpty()) {
        throw DataFileException(errorMessages);
    }
    
    if ( ! warningMessages.isEmpty()) {
        CaretLogWarning(warningMessages);
    }
    
    m_fileHeaderSegment->readContent(file,
                                         dataStream,
                                         errorMessages);
    
    if (m_metadataSegment) {
        m_metadataSegment->readContent(file,
                                           dataStream,
                                           errorMessages);
    }
    
    const int32_t numToPrint(std::min(100, (int32_t)m_subBlockSegments.size()));
    for (int32_t i = 0; i < numToPrint; i++) {
        std::cout << "Subblock: " << i << " of " << m_subBlockSegments.size() << std::endl;
        m_subBlockSegments[i]->readContent(file,
                                               dataStream,
                                               errorMessages);
    }
    
    if (m_subBlockDirectorySegment) {
        m_subBlockDirectorySegment->readContent(file,
                                                    dataStream,
                                                    errorMessages);
    }
    
    if (m_attachmentDirectorySegment) {
        m_attachmentDirectorySegment->readContent(file,
                                                  dataStream,
                                                  errorMessages);
    }
}

/**
 * Read the next segment.
 * @param file
 *    File to read from
 * @param dataStream
 *    Data stream attached to file
 *@param errorMessageOut
 *    Contains error information if failure to read segment
 *@return
 *    Pointer to segment read DO NOT DELETE
 */
const CziFileTestReader::SegmentBase*
CziFileTestReader::readNextSegment(QFile& file,
                               QDataStream& dataStream,
                               AString& errorMessageOut,
                               AString& warningMessageOut)
{
    errorMessageOut.clear();
    warningMessageOut.clear();
    
    /*
     * Offset of segment from beginning of the file
     */
    int64_t segmentOffset(file.pos());
    
    /*
     * Segment begins with 32 byte header
     * - 16 byte (character) ID
     * - 64-bit integer with size of segment data
     * - 64-bit ingeger with amount of segment data used (0 is all)
     */
    
    std::array<char, 17> idChars;
    const int32_t bytesRead(dataStream.readRawData(idChars.data(), 16));
    CaretAssert(bytesRead == 16);
    idChars[16] = '\0';
    const AString idText(idChars.data());
    
    qint64 allocatedSize;
    dataStream >> allocatedSize;
    
    qint64 usedSize;
    dataStream >> usedSize;
    
    std::shared_ptr<SegmentBase> segmentRead;
    
    if (idText == "DELETED") {
        std::shared_ptr<SegmentDeleted> s(new SegmentDeleted(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        m_deletedSegments.push_back(s);
        segmentRead = s;
    }
    else if (idText == "ZISRAWATTACH") { /* ATTACH (attachment) */
        std::shared_ptr<SegmentAttachment> s(new SegmentAttachment(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        m_attachmentSegments.push_back(s);
        segmentRead = s;
    }
    else if (idText == "ZISRAWATTDIR") { /* ATTDIR (attatchment directory) */
        std::shared_ptr<SegmentAttachmentDirectory> s( new SegmentAttachmentDirectory(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        segmentRead = s;
        if (m_attachmentDirectorySegment) {
            errorMessageOut = "File contains more than one attachment directory segment";
        }
        else {
            m_attachmentDirectorySegment = s;
        }
    }
    else if (idText == "ZISRAWDIRECTORY") { /* DIRECTORY */
        std::shared_ptr<SegmentSubBlockDirectory> s(new SegmentSubBlockDirectory(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        segmentRead = s;
        if (m_subBlockDirectorySegment) {
            errorMessageOut = "File contains more than one sub block directory segment";
        }
        else {
            m_subBlockDirectorySegment = s;
        }
    }
    else if (idText == "ZISRAWFILE") { /* FILE header */
        std::shared_ptr<SegmentFileHeader> s(new SegmentFileHeader(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        segmentRead = s;
        if (m_fileHeaderSegment) {
            errorMessageOut = "File contains more than one file header segment";
        }
        else {
            m_fileHeaderSegment = s;
        }
    }
    else if (idText == "ZISRAWMETADATA") { /* METADATA */
        std::shared_ptr<SegmentMetadata> s(new SegmentMetadata(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        segmentRead = s;
        if (m_metadataSegment) {
            errorMessageOut = "File contains more than one metadata segment";
        }
        else {
            m_metadataSegment = s;
        }
    }
    else if (idText == "ZISRAWSUBBLOCK") { /* SUBBLOCK */
        std::shared_ptr<SegmentSubBlock> s(new SegmentSubBlock(idText,
                                  segmentOffset,
                                  allocatedSize,
                                  usedSize));
        segmentRead = s;
        m_subBlockSegments.push_back(s);
    }
    else {
        warningMessageOut = ("File contains unknown semgent named \""
                             + idText
                             + "\".");
        std::shared_ptr<SegmentUnknown> s(new SegmentUnknown(idText,
                                                             segmentOffset,
                                                             allocatedSize,
                                                             usedSize));
        segmentRead = s;
        m_unknownSegments.push_back(s);
    }

    CaretAssert(segmentRead.get());
    std::cout << "Segment " << m_segmentReadingCounter << ": " << segmentRead->toString() << std::endl;

    m_segmentReadingCounter++;
    
    return segmentRead.get();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
CziFileTestReader::toString() const
{
    return "CziFileTestReader";
}

/* ================================================================ */
/**
 * Constructor.
 */
CziFileTestReader::SegmentBase::SegmentBase(const SegmentType segmentType,
                                        const AString& segmentIdName,
                                        const int64_t fileOffset,
                                        const int64_t allocatedSize,
                                        const int64_t usedSize)
:
m_segmentType(segmentType),
m_segmentIdName(segmentIdName),
m_fileOffset(fileOffset),
m_allocatedSize(allocatedSize),
m_usedSize(usedSize)
{
}

/**
 * Destructor.
 */
CziFileTestReader::SegmentBase::~SegmentBase()
{
    
}

AString
CziFileTestReader::SegmentBase::toString() const
{
    AString s("id name=" + m_segmentIdName
              + ", allocated=" + AString::number(m_allocatedSize)
              + ", used=" + AString::number(m_usedSize)
              + ", fileOffset=" + AString::number(m_fileOffset));
    return s;
}

/**
 * Move the file pointer to the start of the data segment for this segment.
 * @param file
 *    The QFile
 * @param errorMessageOut
 *    Contains error information if seek operation fails
 * @return True if successful, else false.
 */
bool
CziFileTestReader::SegmentBase::seekToStartOfSegmentData(QFile& file,
                                                     AString& errorMessageOut)
{
    const int64_t dataOffset(m_fileOffset +
                             s_SEGMENT_HEADER_LENGTH);

    if (file.seek(dataOffset)) {
        errorMessageOut.clear();
        return true;
    }
    
    errorMessageOut = ("Failed to seek to data location="
                       + AString::number(m_fileOffset)
                       + " of file with total length="
                       + AString::number(file.size()));
    return false;
}

/**
 * Move the file pointer to the start of the data segment for this segment plus an offset
 * @param file
 *    The QFile
 * @param offset
 *    Additional offset
 * @param errorMessageOut
 *    Contains error information if seek operation fails
 * @return True if successful, else false.
 */
bool
CziFileTestReader::SegmentBase::seekToStartOfSegmentDataWithOffset(QFile& file,
                                                               int64_t offset,
                                                               AString& errorMessageOut)
{
    const int64_t dataOffset(m_fileOffset
                             + offset
                             + s_SEGMENT_HEADER_LENGTH);
    
    if (file.seek(dataOffset)) {
        errorMessageOut.clear();
        return true;
    }
    
    errorMessageOut = ("Failed to seek to data location="
                       + AString::number(m_fileOffset)
                       + " of file with total length="
                       + AString::number(file.size()));
    return false;
}

/**
 * Read a DirectoryEntryDV
 * @param file
 *    The QFile
 * @param dataStream
 *    Datastream for reading from file
 * @param dimensionCountOut,
 *    Output with count of dimensions
 * @param errorMessageOut
 *    Contains error information if  operation fails
 * @return True if successful, else false.
 */
bool
CziFileTestReader::SegmentBase::readDirectoryEntryDV(QFile& file,
                                                 QDataStream& dataStream,
                                                 int32_t& dimensionCountOut,
                                                 AString& errorMessageOut)
{
    /*
     * Spec 4.4.6  DirectoryEntryDV (32 bytes + EntryCount * 20)
     *
     * SchemaType        Byte[2]            0   2   "DV"
     * PixelType         Int32              2   4
     * FilePosition      Int64              6   8
     * FilePart          Int32              14  4
     * Compression       Int32              18  4
     * PyramidType       Byte               22  1
     * spare             Byte               23  1
     * spare             Byte[4]            24  4
     * DimensionCount    Int32              28  4
     * DimensionEntries  DimensionEntryDV   32  DimensionCount*20
     *
     *
     * DimensionEntryDV
     * Dimension        Byte[4] 0   4
     * Start            Int32   4   4
     * Size             Int32   8   4
     * StartCoordinate  Float   12  4
     * StoredSize       Int32   16  4
     */
    char schemaType[3];
    dataStream.readRawData(&schemaType[0], 2);
    schemaType[2] = '\0';
    
    int32_t pixelType(readInt32(dataStream));
    int64_t filePosition(readInt64(dataStream));
    int32_t filePart(readInt32(dataStream));
    int32_t compression(readInt32(dataStream));
    int8_t  pyramidType(readInt8(dataStream));
    int8_t  spare1(readInt8(dataStream));
    char spare2[4];
    dataStream.readRawData(spare2, 4);
    dimensionCountOut = readInt32(dataStream);
    
    
    //    std::cout << "SchemaType: " << schemaType << std::endl;
    //    std::cout << "Pixel Type: " << pixelType << std::endl;
    //    std::cout << "Compression: " << compression << std::endl;
    //    std::cout << "DimensionCount: " << dimensionCount << std::endl;
    
    AString compressionString;
    switch (compression) {
        case 0:
            compressionString = "Uncompressed";
            break;
        case 1:
            compressionString = "JPEG";
            break;
        case 2:
            compressionString = "LZW";
            break;
        case 4:
            compressionString = "JpgXr";
            break;
        default:
            if ((compression >= 100)
                && (compression <= 999)) {
                compressionString = ("Camera specific RAW "
                                     + AString::number(compression));
            }
            else if (compression >= 1000) {
                compressionString = ("System specific RAW "
                                     + AString::number(compression));
            }
            else {
                compressionString = ("Unknown "
                                     + AString::number(compression));
            }
    }
    
    AString pixelTypeString;
    switch (pixelType) {
        case 0:
            pixelTypeString = "Gray8";
            break;
        case 1:
            pixelTypeString = "Gray16";
            break;
        case 2:
            pixelTypeString = "Gray32Float";
            break;
        case 3:
            pixelTypeString = "Bgr24";
            break;
        case 4:
            pixelTypeString = "Bgr48";
            break;
        case 8:
            pixelTypeString = "Bgr96Float";
            break;
        case 9:
            pixelTypeString = "Bgra32";
            break;
        case 10:
            pixelTypeString = "Gray64ComplexFloat";
            break;
        case 11:
            pixelTypeString = "Bgr192ComplessFloat";
            break;
        case 12:
            pixelTypeString = "Gray32";
            break;
        case 13:
            pixelTypeString = "Gray64";
            break;
        default:
            pixelTypeString = ("Unknown "
                               + AString::number(pixelType));
            break;
    }
    std::cout << "Pixel Type: " << pixelTypeString
    << " Compression: " << compressionString
    << std::endl;
    
    if ((dimensionCountOut > 0)
        && (dimensionCountOut < 10)) {
        //        std::vector<char> stuff;
        //        stuff.resize(dimensionCount * 20);
        //        dataStream.readRawData(&stuff[0], stuff.size());
        //        for (int32_t i = 0; i < stuff.size(); i++) {
        //            std::cout << i << ": " << (int32_t)stuff[i] << ", " << (char)stuff[i] << std::endl;
        //        }
        //
        //        return true;
        
        
        for (int32_t i = 0; i < dimensionCountOut; i++) {
            char dimension[5];
            dataStream.readRawData(&dimension[0], 4);
            dimension[4] = '\0';
            
            int32_t startIndex(readInt32(dataStream));
            int32_t size(readInt32(dataStream));
            float startCoordinate(readFloat(dataStream));
            int32_t storedSize(readInt32(dataStream));
            
            //            if ((dimension[0] == 'X')
            //                || (dimension[0] == 'Y')
            //                || (dimension[0] == 'S')) {
            std::cout
            << "  Dim=" << dimension[0]
            << " Start Index=" << startIndex
            << " Size=" << size
            << " Start Coord: " << startCoordinate
            << std::endl;
            //            }
            //            std::cout << "   dimension: ";
            //            for (int32_t jDim = 0; jDim < 4; jDim++) {
            //                 std::cout << dimension[jDim];
            //            }
            //            std::cout << std::endl;
            //            std::cout << "   start: " << start << std::endl;
            //            std::cout << "   size: " << size << std::endl;
            //            std::cout << "   start coordinate: " << startCoordinate << std::endl;
            //            std::cout << "   stored size: " << storedSize << std::endl;
        }
    }
    
    return true;
}

/**
 * Read a DirectoryEntryDV
 * @param file
 *    The QFile
 * @param dataStream
 *    Datastream for reading from file
 * @param errorMessageOut
 *    Contains error information if  operation fails
 * @return True if successful, else false.
 */
bool
CziFileTestReader::SegmentBase::readAttachmentEntryA1(QFile& file,
                                                  QDataStream& dataStream,
                                                  AString& errorMessageOut)
{
    /*
     * SchemaType       Byte[2]     0   2
     * Reserved         Byte[10]    2   10
     * FilePosition     Int64       12  8
     * FilePart         Int         20  4
     * ContentGuid      GUID        24  16
     * ContentFileType  Byte[8]     40  8
     * Name             Byte[80]    48  80
     */

    char schemaType[3];
    dataStream.readRawData(&schemaType[0], 2);
    schemaType[2] = '\0';
    
    file.seek(file.pos() + 10);
    
    int64_t filePosition(readInt64(dataStream));
    int32_t filePart(readInt32(dataStream));
    char contentGUID[17];
    dataStream.readRawData(&contentGUID[0], 16);
    contentGUID[16] = '\0';
    
    char contentFileType[9];
    dataStream.readRawData(&contentFileType[0], 8);
    contentFileType[8] = '\0';
    
    char name[81];
    dataStream.readRawData(&name[0], 80);
    name[80] = '\0';
    
    std::cout << "AttachnmentEntryA1: " << contentFileType << " - " << name << std::endl;
    return true;
}

/* ================================================================ */

CziFileTestReader::SegmentAttachment::SegmentAttachment(const AString& segmentIdName,
                                                    const int64_t fileOffset,
                                                    const int64_t allocatedSize,
                                                    const int64_t usedSize)
: SegmentBase(SegmentType::ATTACHMENT,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWATTACH");
}

bool
CziFileTestReader::SegmentAttachment::readContent(QFile& file,
                                                  QDataStream& dataStream,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    /*
     * DataSize         Int32               0   4
     * <spare>          Byte                4   12
     * AttachmentEntry  AttachementEntryA1  16  128
     * <spare>          Byte                144 112
     * [Data]           <any>               256 <DataSize>
     */
    
    return false;
}

/* ================================================================ */

CziFileTestReader::SegmentAttachmentDirectory::SegmentAttachmentDirectory(const AString& segmentIdName,
                                                                      const int64_t fileOffset,
                                                                      const int64_t allocatedSize,
                                                                      const int64_t usedSize)
: SegmentBase(SegmentType::ATTACHMENT_DIRECTORY,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWATTDIR");
}

bool
CziFileTestReader::SegmentAttachmentDirectory::readContent(QFile& file,
                                                           QDataStream& dataStream,
                                                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    /*
     * EntryCount       Int                 0       4
     * Reserved         Byte                4       252
     * Entry[]          AttachmentEntry1    256     EntryCount*128
     */

    if ( ! seekToStartOfSegmentData(file,
                                    errorMessageOut)) {
        return false;
    }

    int32_t entryCount;
    dataStream >> entryCount;
    
    file.seek(file.pos() + 252);
    
    std::cout << "Attachment Dir Entry Count: " << entryCount << std::endl;
    
    if ((entryCount > 0)
        && (entryCount < 100)) {
        for (int32_t i = 0; i < entryCount; i++) {
            if ( ! readAttachmentEntryA1(file,
                                         dataStream,
                                         errorMessageOut)) {
                return false;
            }
        }
    }
    
    return true;
}

/* ================================================================ */

CziFileTestReader::SegmentDeleted::SegmentDeleted(const AString& segmentIdName,
                                              const int64_t fileOffset,
                                              const int64_t allocatedSize,
                                              const int64_t usedSize)
: SegmentBase(SegmentType::DELETED,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "DELETED");
}

bool
CziFileTestReader::SegmentDeleted::readContent(QFile& file,
                                               QDataStream& dataStream,
                                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    return false;
}

/* ================================================================ */

CziFileTestReader::SegmentFileHeader::SegmentFileHeader(const AString& segmentIdName,
                                                    const int64_t fileOffset,
                                                    const int64_t allocatedSize,
                                                    const int64_t usedSize)
: SegmentBase(SegmentType::FILE_HEADER,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWFILE");
}

bool
CziFileTestReader::SegmentFileHeader::readContent(QFile& file,
                                                  QDataStream& dataStream,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! seekToStartOfSegmentData(file,
                                    errorMessageOut)) {
        return false;
    }

    /*
     * File Header
     *
     * Major                        Int     0   4
     * Minor                        Int     4   4
     * Reserved1                    Int     8   4
     * Reserved2                    Int     12  4
     * PrimaryFileGuid              GUID    16  16
     * FileGuid                     GUID    32  16
     * FilePart                     Int32   48  4
     * DirectoryPosition            Int64   52  8
     * MetadataPosition             Int64   60  8
     * UpdatePending                Bool    68  4
     * AttachmentDirectoryPosition  Int64   72  8
     */
    dataStream >> m_majorVersion;
    dataStream >> m_minorVersion;
    
    std::cout << "Major/minor: " << m_majorVersion << " " << m_minorVersion << std::endl;
    
    return true;
}

/* ================================================================ */

CziFileTestReader::SegmentMetadata::SegmentMetadata(const AString& segmentIdName,
                                                const int64_t fileOffset,
                                                const int64_t allocatedSize,
                                                const int64_t usedSize)
: SegmentBase(SegmentType::METADATA,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWMETADATA");
}

bool
CziFileTestReader::SegmentMetadata::readContent(QFile& file,
                                                QDataStream& dataStream,
                                                AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! seekToStartOfSegmentData(file,
                                    errorMessageOut)) {
        return false;
    }
    
    /*
     * XmlSize          Int32   0   4
     * AttachmentSize   Int32   4   4
     * <spare>          <spare> 8   256-8
     */
    int64_t xmlSize(0);
    dataStream >> xmlSize;
    
    std::cout << "Metadata XML size: " << xmlSize << std::endl;
    
    const int64_t offsetOfXML(256);
    if ( ! seekToStartOfSegmentDataWithOffset(file,
                                              offsetOfXML,
                                              errorMessageOut)) {
        return false;
    }
    
    bool printFlag(false);
    if (printFlag) {
        /*
         * XML is UTF8 (spec 4.3.3)
         */
        QByteArray byteArray = file.read(xmlSize);
        
        AString xmlText(QString::fromUtf8(byteArray));
        
        std::cout << "--- XML START" << std::endl;
        std::cout << xmlText << std::endl;
        std::cout << "--- XML END" << std::endl;
    }

    /*
     * There is lots of XML in a CZI file.  We may only need what is enclosed in the "Information" tag
     * at the end of the file.
     *
     <Information>
     <User Id="0" />
     <Application>
     <Name>ZEN 2 (blue edition)</Name>
     <Version>2.0.0.0</Version>
     </Application>
     <Document>
     <CreationDate>2017-02-28T12:22:08.368857+01:00</CreationDate>
     <UserName>zeiss</UserName>
     </Document>
     <Image>
     <AcquisitionDateAndTime>2017-02-28T11:22:08.368857Z</AcquisitionDateAndTime>
     <ComponentBitCount>8</ComponentBitCount>
     <PixelType>Bgr24</PixelType>
     <SizeC>1</SizeC>
     <SizeS>4</SizeS>
     <SizeX>53460</SizeX>
     <SizeY>37925</SizeY>
     <SizeB>1</SizeB>
     <SizeM>353</SizeM>
     <OriginalCompressionMethod>JpgXr</OriginalCompressionMethod>
     <OriginalEncodingQuality>85</OriginalEncodingQuality>
     <AcquisitionDuration>343275.63420000003</AcquisitionDuration>
     <Dimensions>
     <Channels>
     <Channel Id="Channel:0" Name="TL Brightfield">
     <Color>#FFFFFFFF</Color>
     <Fluor>TL Brightfield</Fluor>
     <AcquisitionMode>WideField</AcquisitionMode>
     <ExposureTime>200000</ExposureTime>
     <Reflector>none</Reflector>
     <IlluminationType>Transmitted</IlluminationType>
     <ContrastMethod>Brightfield</ContrastMethod>
     <PixelType>Bgr24</PixelType>
     <ComponentBitCount>8</ComponentBitCount>
     <DetectorSettings>
     <Binning>1,1</Binning>
     <EMGain>0</EMGain>
     <Detector Id="Detector:Hitachi HV-F202SCL" />
     </DetectorSettings>
     <LightSourcesSettings>
     <LightSourceSettings>
     <Intensity>154 %</Intensity>
     <FlashDuration>0.003</FlashDuration>
     <LightSource Id="LightSource:1" />
     </LightSourceSettings>
     </LightSourcesSettings>
     </Channel>
     </Channels>
     <Tracks>
     <Track Id="Track:1">
     <ChannelRefs>
     <ChannelRef Id="Channel:0" />
     </ChannelRefs>
     </Track>
     </Tracks>
     <T>
     <StartTime>2017-02-28T11:22:08.368857Z</StartTime>
     <Positions>
     <BinaryList>
     <AttachmentName>TimeStamps</AttachmentName>
     </BinaryList>
     </Positions>
     </T>
     <S>
     <Scenes>
     <Scene Index="0" Name="ScanRegion0">
     <RegionId>636238714749932280</RegionId>
     <ScanMode>Comb</ScanMode>
     <CenterPosition>-42658.523,37426.285</CenterPosition>
     <ContourSize>12768.242,7645.078</ContourSize>
     </Scene>
     <Scene Index="1" Name="ScanRegion1">
     <RegionId>636238714749942281</RegionId>
     <ScanMode>Comb</ScanMode>
     <CenterPosition>-24695.875,37306.543</CenterPosition>
     <ContourSize>12346.344,8209.344</ContourSize>
     </Scene>
     <Scene Index="2" Name="ScanRegion2">
     <RegionId>636238714750052288</RegionId>
     <ScanMode>Comb</ScanMode>
     <CenterPosition>-46200.5,20546.805</CenterPosition>
     <ContourSize>21899.195,23231.5</ContourSize>
     </Scene>
     <Scene Index="3" Name="ScanRegion3">
     <RegionId>636238714750092292</RegionId>
     <ScanMode>Comb</ScanMode>
     <CenterPosition>-23515.33,20349.141</CenterPosition>
     <ContourSize>22075.888,22839.03</ContourSize>
     </Scene>
     </Scenes>
     </S>
     </Dimensions>
     <ObjectiveSettings>
     <ObjectiveRef Id="Objective:1" />
     </ObjectiveSettings>
     <MicroscopeRef Id="Microscope:1" />
     <TubeLenses />
     </Image>
     </Information>
     <Scaling>
     <AutoScaling>
     <Type>Measured</Type>
     <Objective>Objective.420130-9900-000</Objective>
     <Reflector>Reflector.none</Reflector>
     <CameraAdapter>CameraAdapter.1x</CameraAdapter>
     <ObjectiveName>Fluar 5x/0.25 M27</ObjectiveName>
     <ReflectorMagnification>1</ReflectorMagnification>
     <CameraName>Hitachi HV-F202SCL</CameraName>
     <CameraAdapterMagnification>1</CameraAdapterMagnification>
     <CameraPixelDistance>4.4,4.4</CameraPixelDistance>
     <CreationDateTime>02/28/2017 11:22:08</CreationDateTime>
     </AutoScaling>
     <Items>
     <Distance Id="X">
     <Value>8.79612970293071E-07</Value>
     <DefaultUnitFormat>µm</DefaultUnitFormat>
     </Distance>
     <Distance Id="Y">
     <Value>8.79612970293071E-07</Value>
     <DefaultUnitFormat>µm</DefaultUnitFormat>
     </Distance>
     </Items>
     </Scaling>
     <DisplaySetting>
     <Channels>
     <Channel Id="Channel:0" Name="TL Brightfield">
     <Gamma>0.45</Gamma>
     <BitCountRange>8</BitCountRange>
     <DyeName>Dye1</DyeName>
     <ShortName>Brigh</ShortName>
     <IlluminationType>Transmitted</IlluminationType>
     <ColorMode>None</ColorMode>
     </Channel>
     </Channels>
     </DisplaySetting>
     <AttachmentInfos>
     <AttachmentInfo Id="Label:1">
     <Label>
     <Barcodes>
     <Barcode Id="Barcode:1">
     <Type></Type>
     <Content></Content>
     </Barcode>
     </Barcodes>
     <OCRs>
     <OCR Id="OCR:1">
     <Content></Content>
     </OCR>
     </OCRs>
     </Label>
     </AttachmentInfo>
     </AttachmentInfos>
     </Metadata>
     </ImageDocument>

     */
                    
    return true;
}

/* ================================================================ */

CziFileTestReader::SegmentSubBlock::SegmentSubBlock(const AString& segmentIdName,
                                                const int64_t fileOffset,
                                                const int64_t allocatedSize,
                                                const int64_t usedSize)
: SegmentBase(SegmentType::SUB_BLOCK,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWSUBBLOCK");
}

bool
CziFileTestReader::SegmentSubBlock::readContent(QFile& file,
                                                QDataStream& dataStream,
                                                AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! seekToStartOfSegmentData(file,
                                    errorMessageOut)) {
        return false;
    }
    
    /*
     * Spec 4.4.2 SubBlockSegment
     *  ITEM            TYPE                OFFSET              SIZE                COMMENTS
     *  MetadataSize    Int32               0                   4
     *  AttachmentSize  Int32               4                   4
     *  DataSize        Int64               8                   8
     *  DirectoryEntry  DirectoryEntryDV    16                  variable
     *  Fill                                variable+16          Max(256-N, 0)
     *  Metadata        String              off=Max(256-N)      <MetadataSize>
     *  Data            <any>               off+<MetadataSize>  <DataSize>          Pixelsx
     *  Attachments     <any>               off+<MetadataSize>
     *                                      + <DataSize>        <AttachmentSize>
     */
    int32_t metadataSize(readInt32(dataStream));
    int32_t attachmentSize(readInt32(dataStream));
    int64_t dataSize(readInt64(dataStream));
    
    int32_t dimensionCount(0);
    const bool newFunctionFlag(true);
    if (newFunctionFlag) {
        readDirectoryEntryDV(file,
                             dataStream,
                             dimensionCount,
                             errorMessageOut);
    }
    else {
        /* START */
        
        /*
         * Spec 4.4.6  DirectoryEntryDV (32 bytes + EntryCount * 20)
         *
         * SchemaType        Byte[2]            0   2   "DV"
         * PixelType         Int32              2   4
         * FilePosition      Int64              6   8
         * FilePart          Int32              14  4
         * Compression       Int32              18  4
         * PyramidType       Byte               22  1
         * spare             Byte               23  1
         * spare             Byte[4]            24  4
         * DimensionCount    Int32              28  4
         * DimensionEntries  DimensionEntryDV   32  DimensionCount*20
         *
         *
         * DimensionEntryDV
         * Dimension        Byte[4] 0   4
         * Start            Int32   4   4
         * Size             Int32   8   4
         * StartCoordinate  Float   12  4
         * StoredSize       Int32   16  4
         */
        char schemaType[3];
        dataStream.readRawData(&schemaType[0], 2);
        schemaType[2] = '\0';
        
        int32_t pixelType(readInt32(dataStream));
        int64_t filePosition(readInt64(dataStream));
        int32_t filePart(readInt32(dataStream));
        int32_t compression(readInt32(dataStream));
        int8_t  pyramidType(readInt8(dataStream));
        int8_t  spare1(readInt8(dataStream));
        char spare2[4];
        dataStream.readRawData(spare2, 4);
        dimensionCount = readInt32(dataStream);
        
        
        //    std::cout << "SchemaType: " << schemaType << std::endl;
        //    std::cout << "Pixel Type: " << pixelType << std::endl;
        //    std::cout << "Compression: " << compression << std::endl;
        //    std::cout << "DimensionCount: " << dimensionCount << std::endl;
        
        AString compressionString;
        switch (compression) {
            case 0:
                compressionString = "Uncompressed";
                break;
            case 1:
                compressionString = "JPEG";
                break;
            case 2:
                compressionString = "LZW";
                break;
            case 4:
                compressionString = "JpgXr";
                break;
            default:
                if ((compression >= 100)
                    && (compression <= 999)) {
                    compressionString = ("Camera specific RAW "
                                         + AString::number(compression));
                }
                else if (compression >= 1000) {
                    compressionString = ("System specific RAW "
                                         + AString::number(compression));
                }
                else {
                    compressionString = ("Unknown "
                                         + AString::number(compression));
                }
        }
        
        AString pixelTypeString;
        switch (pixelType) {
            case 0:
                pixelTypeString = "Gray8";
                break;
            case 1:
                pixelTypeString = "Gray16";
                break;
            case 2:
                pixelTypeString = "Gray32Float";
                break;
            case 3:
                pixelTypeString = "Bgr24";
                break;
            case 4:
                pixelTypeString = "Bgr48";
                break;
            case 8:
                pixelTypeString = "Bgr96Float";
                break;
            case 9:
                pixelTypeString = "Bgra32";
                break;
            case 10:
                pixelTypeString = "Gray64ComplexFloat";
                break;
            case 11:
                pixelTypeString = "Bgr192ComplessFloat";
                break;
            case 12:
                pixelTypeString = "Gray32";
                break;
            case 13:
                pixelTypeString = "Gray64";
                break;
            default:
                pixelTypeString = ("Unknown "
                                   + AString::number(pixelType));
                break;
        }
        std::cout << "Pixel Type: " << pixelTypeString
        << " Compression: " << compressionString
        << " metadataSize: " << metadataSize
        << " dataSize: " << dataSize
        << " attachmentSize: " << attachmentSize
        << std::endl;
        if ((dimensionCount > 0)
            && (dimensionCount < 10)) {
            //        std::vector<char> stuff;
            //        stuff.resize(dimensionCount * 20);
            //        dataStream.readRawData(&stuff[0], stuff.size());
            //        for (int32_t i = 0; i < stuff.size(); i++) {
            //            std::cout << i << ": " << (int32_t)stuff[i] << ", " << (char)stuff[i] << std::endl;
            //        }
            //
            //        return true;
            
            
            for (int32_t i = 0; i < dimensionCount; i++) {
                char dimension[5];
                dataStream.readRawData(&dimension[0], 4);
                dimension[4] = '\0';
                
                int32_t startIndex(readInt32(dataStream));
                int32_t size(readInt32(dataStream));
                float startCoordinate(readFloat(dataStream));
                int32_t storedSize(readInt32(dataStream));
                
                //            if ((dimension[0] == 'X')
                //                || (dimension[0] == 'Y')
                //                || (dimension[0] == 'S')) {
                std::cout << "  Dim=" << dimension[0]
                << " Start Index=" << startIndex
                << " Size=" << size
                << " Start Coord: " << startCoordinate
                << std::endl;
                //            }
                //            std::cout << "   dimension: ";
                //            for (int32_t jDim = 0; jDim < 4; jDim++) {
                //                 std::cout << dimension[jDim];
                //            }
                //            std::cout << std::endl;
                //            std::cout << "   start: " << start << std::endl;
                //            std::cout << "   size: " << size << std::endl;
                //            std::cout << "   start coordinate: " << startCoordinate << std::endl;
                //            std::cout << "   stored size: " << storedSize << std::endl;
            }
        }
        
        /* END */
    }
    
    const int64_t directoryEntrySize(32); /* SchemaType through DimensionCount */
    const int64_t directoryEntriesSize(dimensionCount * 20);
    const int64_t n = (directoryEntrySize + directoryEntriesSize + 16);
    const int64_t fillSize(std::max(256 - n, (int64_t)0));
    if (fillSize > 0) {
        file.seek(file.pos() + fillSize);
    }
    int64_t metadataOffset(file.pos());
    
    if (metadataSize > 0) {
        std::vector<char> metadata;
        metadata.resize(metadataSize);
        dataStream.readRawData(&metadata[0], metadataSize);
        metadata.push_back('\0');
        std::cout << "---Metadata Start" << std::endl;
        std::cout << &metadata[0] << std::endl;
        std::cout << "---Metadata End" << std::endl;
    }
    int64_t dataOffset(metadataOffset + metadataSize);
    
    if (dataSize > 0) {
        file.seek(dataOffset);
        std::vector<char> data;
        data.resize(dataSize);
        dataStream.readRawData(data.data(), dataSize);
        
        for (int32_t i = 0; i < 50; i++) {
            std::cout << i << ": " << (char)data[i] << ", " << (int)data[i] << std::endl;
        }
    }
    return true;
}

/* ================================================================ */

CziFileTestReader::SegmentSubBlockDirectory::SegmentSubBlockDirectory(const AString& segmentIdName,
                                                                  const int64_t fileOffset,
                                                                  const int64_t allocatedSize,
                                                                  const int64_t usedSize)
: SegmentBase(SegmentType::SUB_BLOCK_DIRECTORY,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    CaretAssert(segmentIdName == "ZISRAWDIRECTORY");
}

bool
CziFileTestReader::SegmentSubBlockDirectory::readContent(QFile& file,
                                                         QDataStream& dataStream,
                                                         AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! seekToStartOfSegmentData(file,
                                    errorMessageOut)) {
        return false;
    }
    
    /*
     * Subblock Directory Segment
     *
     * EntryCount   Int                 0   4
     * Reseerved    Byte                4   124
     * Entry[]      DirectoryEntryDV    128 variable
     */
    const int32_t entryCount(readInt32(dataStream));
    
    /*
     * Skip over reserved bytes
     */
    file.seek(file.pos() + 124);
    
    std::cout << "Subblock Directory Count: " << entryCount << std::endl;
    
    int32_t numToRead(entryCount);
    if (numToRead > 25) {
        numToRead = 25;
    }
    for (int i = 0; i < numToRead; i++) {
        std::cout << "Sub block directory entry " << i << " of " << entryCount << std::endl;
        int32_t dimensionCount(0);
        readDirectoryEntryDV(file,
                             dataStream,
                             dimensionCount,
                             errorMessageOut);
    }
    
    return false;
}

/* ================================================================ */

CziFileTestReader::SegmentUnknown::SegmentUnknown(const AString& segmentIdName,
                                              const int64_t fileOffset,
                                              const int64_t allocatedSize,
                                              const int64_t usedSize)
: SegmentBase(SegmentType::UNKNOWN,
              segmentIdName,
              fileOffset,
              allocatedSize,
              usedSize)
{
    
}

bool
CziFileTestReader::SegmentUnknown::readContent(QFile& file,
                                               QDataStream& dataStream,
                                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    return false;
}

