#ifndef __CARET_DATA_FILE__H_
#define __CARET_DATA_FILE__H_

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


#include "DataFile.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"

namespace caret {

    class GiftiMetaData;
    
    class CaretDataFile : public DataFile {
        
    public:
        CaretDataFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CaretDataFile();
        
        /**
         * @return The structure for this file.
         */
        virtual StructureEnum::Enum getStructure() const = 0;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        virtual void setStructure(const StructureEnum::Enum structure) = 0;
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        /**
         * @return Get access to the file's metadata.
         */
        virtual GiftiMetaData* getFileMetaData() = 0;
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        virtual const GiftiMetaData* getFileMetaData() const = 0;
        
    protected:
        CaretDataFile(const CaretDataFile& cdf);

        CaretDataFile& operator=(const CaretDataFile& cdf);
        
        void setDataFileType(const DataFileTypeEnum::Enum dataFileType);
        
    private:
        void copyDataCaretDataFile(const CaretDataFile& cdf);
        
        DataFileTypeEnum::Enum dataFileType;
        
        /** A counter that is used when creating default file names */
        static int64_t defaultFileNameCounter;
    };
    
#ifdef __CARET_DATA_FILE_DECLARE__
    int64_t CaretDataFile::defaultFileNameCounter = 1;
#endif // __CARET_DATA_FILE_DECLARE__

} // namespace
#endif  //__CARET_DATA_FILE__H_
