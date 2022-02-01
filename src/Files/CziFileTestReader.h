#ifndef __CZI_FILE_TEST_READER_H__
#define __CZI_FILE_TEST_READER_H__

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


#include <cstdint>
#include <memory>

#include <QtGlobal>
#include <QDataStream>

#include "CaretObject.h"

class QByteArray;
class QDataStream;
class QFile;

namespace caret {

class DataFileException;

class CziFileTestReader : public CaretObject {
    
public:
    static void testReading(const AString& filename);
    
    CziFileTestReader();
    
    virtual ~CziFileTestReader();
    
    CziFileTestReader(const CziFileTestReader&) = delete;
    
    CziFileTestReader& operator=(const CziFileTestReader&) = delete;
    
    void readFile(const AString& filename);
    
    // ADD_NEW_METHODS_HERE
    
    virtual AString toString() const;
    
private:
    class SegmentBase {
    public:
        enum class SegmentType {
            ATTACHMENT,
            ATTACHMENT_DIRECTORY,
            DELETED,
            FILE_HEADER,
            METADATA,
            SUB_BLOCK,
            SUB_BLOCK_DIRECTORY,
            UNKNOWN
        };
        
    protected:
        SegmentBase(const SegmentType segmentType,
                    const AString& segmentIdName,
                    const int64_t fileOffset,
                    const int64_t allocatedSize,
                    const int64_t usedSize);
        
        bool readAttachmentEntryA1(QFile& file,
                                   QDataStream& dataStream,
                                   AString& errorMessageOut);
        
        bool readDirectoryEntryDV(QFile& file,
                                  QDataStream& dataStream,
                                  int32_t& dimensionCountOut,
                                  AString& errorMessageOut);
        
    public:
       virtual ~SegmentBase();
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut) = 0;
        
        bool seekToStartOfSegmentData(QFile& file,
                                      AString& errorMessageOut);
        
        bool seekToStartOfSegmentDataWithOffset(QFile& file,
                                                int64_t offset,
                                                AString& errorMessageOut);
        
        inline int8_t readInt8(QDataStream& dataStream) {
            int8_t d;
            dataStream >> d;
            return d;
        }
        
        inline int32_t readInt32(QDataStream& dataStream) {
            int32_t d;
            dataStream >> d;
            return d;
        }
        
        inline int64_t readInt64(QDataStream& dataStream) {
            int64_t d;
            dataStream >> d;
            return d;
        }
        
        inline float readFloat(QDataStream& dataStream) {
            float d;
            dataStream >> d;
            return d;
        }
        
        AString toString() const;
        
        const SegmentType m_segmentType;
        
        const AString m_segmentIdName;
        
        const int64_t m_fileOffset;
        
        const uint64_t m_allocatedSize;
        
        const uint64_t m_usedSize;
    };
    
    class SegmentDeleted : public SegmentBase {
    public:
        SegmentDeleted(const AString& segmentIdName,
                       const int64_t fileOffset,
                       const int64_t allocatedSize,
                       const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    
    class SegmentFileHeader : public SegmentBase {
    public:
        SegmentFileHeader(const AString& segmentIdName,
                          const int64_t fileOffset,
                          const int64_t allocatedSize,
                          const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
        int32_t m_majorVersion = -1;
        
        int32_t m_minorVersion = -1; 
        
    };
    
    class SegmentMetadata : public SegmentBase {
    public:
        SegmentMetadata(const AString& segmentIdName,
                        const int64_t fileOffset,
                        const int64_t allocatedSize,
                        const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    class SegmentSubBlock : public SegmentBase {
    public:
        SegmentSubBlock(const AString& segmentIdName,
                        const int64_t fileOffset,
                        const int64_t allocatedSize,
                        const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    class SegmentSubBlockDirectory : public SegmentBase {
    public:
        SegmentSubBlockDirectory(const AString& segmentIdName,
                                 const int64_t fileOffset,
                                 const int64_t allocatedSize,
                                 const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    class SegmentAttachment : public SegmentBase {
    public:
        SegmentAttachment(const AString& segmentIdName,
                          const int64_t fileOffset,
                          const int64_t allocatedSize,
                          const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    class SegmentAttachmentDirectory : public SegmentBase {
    public:
        SegmentAttachmentDirectory(const AString& segmentIdName,
                                   const int64_t fileOffset,
                                   const int64_t allocatedSize,
                                   const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    class SegmentUnknown : public SegmentBase {
    public:
        SegmentUnknown(const AString& segmentIdName,
                       const int64_t fileOffset,
                       const int64_t allocatedSize,
                       const int64_t usedSize);
        
        virtual bool readContent(QFile& file,
                                     QDataStream& dataStream,
                                     AString& errorMessageOut);
        
    };
    
    void readAllSegments(QFile& file,
                         QDataStream& dataStream);
    
    const SegmentBase* readNextSegment(QFile& file,
                                       QDataStream& dataStream,
                                       AString& errorMessageOut,
                                       AString& warningMessageOut);

    // ADD_NEW_MEMBERS_HERE
    
    std::vector<std::shared_ptr<SegmentDeleted>> m_deletedSegments;
    
    std::shared_ptr<SegmentFileHeader> m_fileHeaderSegment;
    
    std::shared_ptr<SegmentMetadata> m_metadataSegment;
    
    std::shared_ptr<SegmentSubBlockDirectory> m_subBlockDirectorySegment;
    
    std::vector<std::shared_ptr<SegmentSubBlock>> m_subBlockSegments;

    std::shared_ptr<SegmentAttachmentDirectory> m_attachmentDirectorySegment;
    
    std::vector<std::shared_ptr<SegmentAttachment>> m_attachmentSegments;
    
    std::vector<std::shared_ptr<SegmentUnknown>> m_unknownSegments;

    int32_t m_segmentReadingCounter = 0;
    
    static constexpr int32_t s_SEGMENT_HEADER_LENGTH = 32;
};

#ifdef __CZI_FILE_TEST_READER_DECLARE__
// <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_FILE_TEST_READER_DECLARE__

} // namespace
#endif  //__CZI_FILE_TEST_READER_H__

