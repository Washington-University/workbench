
#ifndef __RGBA_FILE_H__
#define __RGBA_FILE_H__

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

#include <vector>
#include <stdint.h>

#include "GiftiTypeFile.h"

namespace caret {

    class GiftiDataArray;
    
    /**
     * \brief An RGBA data file.
     */
    class RgbaFile : public GiftiTypeFile {
        
    public:
        RgbaFile();
        
        RgbaFile(const RgbaFile& sf);
        
        RgbaFile& operator=(const RgbaFile& sf);
        
        virtual ~RgbaFile();
        
        virtual void clear();
        
        virtual int32_t getNumberOfNodes() const;
        
        virtual int32_t getNumberOfColumns() const;
        
        void getVertexRGBA(const int32_t vertexIndex,
                           float* rgbaOut) const;
        
    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading();
        
        void copyHelperRgbaFile(const RgbaFile& sf);
        
        void initializeMembersRgbaFile();
        
    private:
        
        GiftiDataArray* m_dataArray = 0;
        
        int32_t m_numberOfVertices = 0;
        
        int32_t m_numberOfComponents = 0;
        
    };

} // namespace

#endif // __RGBA_FILE_H__
