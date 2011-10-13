#ifndef __GIFTI_FILE_WRITER__H_
#define __GIFTI_FILE_WRITER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <iosfwd>

#include "CaretObject.h"
#include "GiftiFile.h"

namespace caret {

    class GiftiFileWriter : public CaretObject {
        
    public:
        GiftiFileWriter(const AString& filename,
                        const GiftiEncodingEnum::Enum encoding);
        
        virtual ~GiftiFileWriter();
         
        void start(const int numberOfDataArrays,
                   GiftiMetaData* metadata,
                   GiftiLabelTable* labelTable) throw (GiftiException);
        void writeDataArray(GiftiDataArray* gda) throw (GiftiException);
        
        void finish() throw (GiftiException);
        
        long getMaximumExternalFileSize() const;
        
        void setMaximumExternalFileSize(const long size);
        
    private:
        GiftiFileWriter(const GiftiFileWriter&);

        GiftiFileWriter& operator=(const GiftiFileWriter&);
        
        void closeFiles();
        
        void verifyOpened() throw (GiftiException);
        
        void removeExternalFiles() throw (GiftiException);
        
        AString getExternalFileNamePrefix() const;
        
        AString getExternalFileNameForWriting() const;
        
    public:
        virtual AString toString() const;
        
    private:
        /** The file output stream for the XML file. */
        std::ostream* xmlFileOutputStream;
        
        /** The file output stream for the external data file. */
        std::ostream* externalFileOutputStream; 
        
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
