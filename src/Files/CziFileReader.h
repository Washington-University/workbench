#ifndef __CZI_FILE_READER_H__
#define __CZI_FILE_READER_H__

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

#include "CaretObject.h"

class QByteArray;
class QFile;

namespace caret {

    class DataFileException;
    
    class CziFileReader : public CaretObject {
        
    public:
        static void testReading(const AString& filename);
        
        CziFileReader();
        
        virtual ~CziFileReader();
        
        CziFileReader(const CziFileReader&) = delete;

        CziFileReader& operator=(const CziFileReader&) = delete;
        
        void readFile(const AString& filename);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class SegmentInfo {
        public:
            SegmentInfo(const int64_t fileOffset,
                          const AString& id,
                          const int64_t allocatedSize,
                          const int64_t usedSize);

            AString toString() const;
            
            int64_t m_fileOffset;
            
            AString m_id;
            
            uint64_t m_allocatedSize = 0;
            
            uint64_t m_usedSize = 0;
            
            
        };
        
        void readAllSegments(QFile& file);
        
        SegmentInfo* readNextSegment(QFile& file,
                                       AString& errorMessageOut);

        // ADD_NEW_MEMBERS_HERE

        static constexpr int32_t s_SEGMENT_HEADER_LENGTH = 32;
    };
    
#ifdef __CZI_FILE_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_FILE_READER_DECLARE__

} // namespace
#endif  //__CZI_FILE_READER_H__
