#ifndef __GIFTI_FILE_WRITER__H_
#define __GIFTI_FILE_WRITER__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <fstream>

#include "CaretObject.h"
#include "GiftiFile.h"
#include "GiftiEncodingEnum.h"

namespace caret {

    class GiftiDataArray;
    class GiftiMetaData;
    class GiftiLabelTable;
    class XmlWriter;
    
    class GiftiFileWriter : public CaretObject {
        
    public:
        GiftiFileWriter(const AString& filename,
                        const GiftiEncodingEnum::Enum encoding);
        
        virtual ~GiftiFileWriter();
         
        void start(const int numberOfDataArrays,
                   GiftiMetaData* metadata,
                   GiftiLabelTable* labelTable);
        void writeDataArray(GiftiDataArray* gda);
        
        void finish();
        
        long getMaximumExternalFileSize() const;
        
        void setMaximumExternalFileSize(const long size);
        
    private:
        GiftiFileWriter(const GiftiFileWriter&);

        GiftiFileWriter& operator=(const GiftiFileWriter&);
        
        void closeFiles();
        
        void verifyOpened();
        
        void removeExternalFiles();
        
        AString getExternalFileNamePrefix() const;
        
        AString getExternalFileNameForWriting() const;
        
    public:
        virtual AString toString() const;
        
    private:
        /** The file output stream for the XML file. */
        std::ofstream* xmlFileOutputStream;
        
        /** The file output stream for the external data file. */
        std::ofstream* externalFileOutputStream; 
        
        /** The number of data arrays in the file being written. */
        int numberOfDataArrays;
        
        /** Start a new external file when the external file reaches this size. */
        long maximumExternalFileSize;
        
        /** name of file to write. */
        AString filename;
        
        /** encoding of file. */
        GiftiEncodingEnum::Enum encoding;
        
        /** The XML writer. */
        XmlWriter* xmlWriter;
        
        /** Counts the number of data arrays that have been written. */
        int dataArraysWrittenCounter;
        
    };
    
#ifdef __GIFTI_FILE_WRITER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_FILE_WRITER_DECLARE__

} // namespace
#endif  //__GIFTI_FILE_WRITER__H_
