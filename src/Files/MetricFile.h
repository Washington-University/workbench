
#ifndef __METRIC_FILE_H__
#define __METRIC_FILE_H__

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
    
    /**
     * \brief A Metric data file.
     */
    class MetricFile : public GiftiTypeFile {
        
    public:
        MetricFile();
        
        MetricFile(const MetricFile& sf);
        
        MetricFile& operator=(const MetricFile& sf);
        
        virtual ~MetricFile();
        
        virtual void clear();
        
        virtual int32_t getNumberOfNodes() const;
        
        virtual int32_t getNumberOfColumns() const;
        
        virtual void setNumberOfNodesAndColumns(int32_t nodes, int32_t columns);

        float getValue(const int32_t nodeIndex,
                       const int32_t columnIndex) const;
        
        void setValue(const int32_t nodeIndex,
                      const int32_t columnIndex,
                      const float value);
        
        const float* getValuePointerForColumn(const int32_t columnIndex) const;
        
        void setValuesForColumn(const int32_t columnIndex, const float* valuesIn);
        
        void initializeColumn(const int32_t columnIndex, const float& value = 0.0f);
        
    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading() throw (DataFileException);
        
        void copyHelperMetricFile(const MetricFile& sf);
        
        void initializeMembersMetricFile();
        
    private:
        /** Points to actual data in each Gifti Data Array */
        std::vector<float*> columnDataPointers;
    };

} // namespace

#endif // __METRIC_FILE_H__
