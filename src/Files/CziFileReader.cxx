
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

#define __CZI_FILE_READER_DECLARE__
#include "CziFileReader.h"
#undef __CZI_FILE_READER_DECLARE__

#include <array>
#include <cstdint>

#include <QDataStream>
#include <QFile>

#include "CaretAssert.h"
#include "DataFileException.h"

using namespace caret;


    
/**
 * \class caret::CziFileReader
 * \brief Test reading of CZI file to understand the file structure
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziFileReader::CziFileReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
CziFileReader::~CziFileReader()
{
}

/**
 * Test reading a CZI file and print info to terminal.
 * @param filename
 *    Name of file.
 */
void
CziFileReader::testReading(const AString& filename)
{
    try {
        CziFileReader reader;
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
CziFileReader::readFile(const AString& filename)
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
    
    readAllSegments(file);
    
}

/**
 * Read all segments from the file.
 * @param file
 *    File to read from.
 */
void
CziFileReader::readAllSegments(QFile& file)
{
    int32_t segmentHeaderCount(0);
    
    while ( ! file.atEnd()) {
        
        AString errorMessage;
        std::unique_ptr<SegmentInfo> segmentInfo(readNextSegment(file,
                                                         errorMessage));
        
        if ( ! segmentInfo) {
            throw DataFileException("Error reading segment header index="
                                    + AString::number(segmentHeaderCount)
                                    + ": "
                                    + errorMessage);
        }
        
        std::cout << "Segment " << segmentHeaderCount << ": " << segmentInfo->toString() << std::endl;
//        ": id=" << segmentInfo->m_id
//        << ", allocated=" << segmentInfo->m_allocatedSize
//        << ", used=" << segmentInfo->m_usedSize
//        << ", fileOffset=" << segmentInfo->m_fileOffset << std::endl;
        
        segmentHeaderCount++;
        
        const AString idText(segmentInfo->m_id);
        if (idText == "DELETED") {
            
        }
        else {
            if (idText == "ZISRAWATTACH") { /* ATTACH (attachment) */
                
            }
            if (idText == "ZISRAWATTDIR") { /* ATTDIR (attatchment directory) */
                
            }
            else if (idText == "ZISRAWDIRECTORY") { /* DIRECTORY */
                
            }
            else if (idText == "ZISRAWFILE") { /* FILE header */
                
            }
            else if (idText == "ZISRAWMETADATA") { /* METADATA */
                
            }
            else if (idText == "ZISRAWSUBBLOCK") { /* SUBBLOCK */
                
            }
            else {
                
            }

        }

        
        const int64_t nextSegmentOffset(segmentInfo->m_fileOffset
                                        + s_SEGMENT_HEADER_LENGTH
                                        + segmentInfo->m_allocatedSize);
        if ( ! file.seek(nextSegmentOffset)) {
            throw DataFileException("Failed to seek to offset="
                                    + AString::number(nextSegmentOffset));
        }
//        break;
    }

}

/**
 * Read the next segment.
 * @param file
 *    File to read from
 *@param errorMessageOut
 *    Contains error information if failure to read segment
 *@return
 *    Pointer to segment or NULL if there is an error.
 */
CziFileReader::SegmentInfo*
CziFileReader::readNextSegment(QFile& file,
                               AString& errorMessageOut)
{
    int64_t segmentOffset(file.pos());
    errorMessageOut.clear();
    
    /*
     * Segment begins with 32 byte header
     * - 16 byte (character) ID
     * - 64-bit integer with size of segment data
     * - 64-bit ingeger with amount of segment data used (0 is all)
     *
     * All data is LittleEndian
     */
    const QByteArray byteArray = file.read(s_SEGMENT_HEADER_LENGTH);
    if (byteArray.length() == s_SEGMENT_HEADER_LENGTH) {
        QDataStream headerStream(byteArray);
        headerStream.setByteOrder(QDataStream::LittleEndian);
        
        std::array<char, 17> idChars;
        const int32_t bytesRead(headerStream.readRawData(idChars.data(), 16));
        CaretAssert(bytesRead == 16);
        idChars[16] = '\0';
        const AString idText(idChars.data());
        
        int64_t allocatedSize(0);
        headerStream >> allocatedSize;
        
        int64_t usedSize(0);
        headerStream >> usedSize;
        
        return new SegmentInfo(segmentOffset,
                                 idText,
                                 allocatedSize,
                                 usedSize);
    }
    else {
        errorMessageOut = ("Error reading segment header.  Wrong number of bytes="
                            + AString::number(byteArray.length())
                            + ", should be="
                           + AString::number(s_SEGMENT_HEADER_LENGTH));
    }
    
    return NULL;
}

/**
 * Constructor.
 */
CziFileReader::SegmentInfo::SegmentInfo(const int64_t fileOffset,
                                            const AString& id,
                                            const int64_t allocatedSize,
                                            const int64_t usedSize)
:
m_fileOffset(fileOffset),
m_id(id),
m_allocatedSize(allocatedSize),
m_usedSize(usedSize)
{
}

AString
CziFileReader::SegmentInfo::toString() const
{
    AString s("id=" + m_id
              + ", allocated=" + AString::number(m_allocatedSize)
              + ", used=" + AString::number(m_usedSize)
              + ", fileOffset=" + AString::number(m_fileOffset));
    return s;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
CziFileReader::toString() const
{
    return "CziFileReader";
}


