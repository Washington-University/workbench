
#ifndef __LABEL_FILE_H__
#define __LABEL_FILE_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <vector>
#include <stdint.h>

#include "GiftiTypeFile.h"

namespace caret {

    class GiftiDataArray;
    class GiftiLabelTable;
    
    /**
     * \brief A Label data file.
     */
    class LabelFile : public GiftiTypeFile {
        
    public:
        LabelFile();
        
        LabelFile(const LabelFile& sf);
        
        LabelFile& operator=(const LabelFile& sf);
        
        virtual ~LabelFile();
        
        void clear();
        
        int32_t getNumberOfNodes() const;
        
        int32_t getNumberOfColumns() const;
        
        GiftiLabelTable* getLabelTable();
        
        const GiftiLabelTable* getLabelTable() const;
        
        int32_t getLabelKey(const int32_t nodeIndex,
                            const int32_t columnIndex) const;
        
        void setLabelKey(const int32_t nodeIndex,
                         const int32_t columnIndex,
                         const int32_t labelIndex);
        
        const int32_t* getLabelKeyPointerForColumn(const int32_t columnIndex) const;
        
    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading() throw (DataFileException);
        
        void copyHelperLabelFile(const LabelFile& sf);
        
        void initializeMembersLabelFile();
        
    private:
        /** Points to actual data in each Gifti Data Array */
        std::vector<int32_t*> columnDataPointers;
    };

} // namespace

#endif // __LABEL_FILE_H__
