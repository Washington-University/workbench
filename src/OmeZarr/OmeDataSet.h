#ifndef __OME_DATA_SET_H__
#define __OME_DATA_SET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "ZarrDataTypeEnum.h"

namespace caret {

    class ZarrImageReader;
    
    class OmeDataSet : public CaretObject {
        
    public:
        OmeDataSet();
        
        virtual ~OmeDataSet();
        
        OmeDataSet(const OmeDataSet& obj);

        OmeDataSet& operator=(const OmeDataSet& obj);
        

        AString getPath() const;
        
        void setPath(const AString& path);
        
        std::vector<float> getScaling() const;
        
        void setScaling(const std::vector<float>& scaling);
        
        std::vector<float> getTranslation() const;
        
        void setTranslation(const std::vector<float>& translation);

        ZarrDataTypeEnum::Enum getZarrDataType() const;
        
        void setZarrDataType(const ZarrDataTypeEnum::Enum zarrDataType);
        
        std::vector<int64_t> getDimensions() const;
        
        void setDimensions(const std::vector<int64_t>& dimensions);
        
        void setDimensionIndices(const int32_t dimensionIndexX,
                                 const int32_t dimensionIndexY,
                                 const int32_t dimensionIndexZ,
                                 const int32_t dimensionIndexTime,
                                 const int32_t dimensionIndexChannel);
        
        void setZarrImageReader(ZarrImageReader* zarrImageReader);
        
        int64_t getWidth() const;

        int64_t getHeight() const;
        
        int64_t getNumberOfSlices() const;
        
        int64_t getNumberOfTimePoints() const;
        
        int64_t getNumberOfChannels() const;
        
        FunctionResultValue<unsigned char*> readDataSet(const int64_t sliceIndex) const;
        
        FunctionResultValue<unsigned char*> readDataSetForImage(const int64_t sliceIndex) const;
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;

    private:
        void copyHelperOmeDataSet(const OmeDataSet& obj);

    /** The subdirectory path*/
    AString m_path;

    /** scaling*/
    std::vector<float> m_scaling;

    /** translation*/
    std::vector<float> m_translation;

        ZarrDataTypeEnum::Enum m_zarrDataType = ZarrDataTypeEnum::UNKNOWN;

        std::vector<int64_t> m_dimensions;
        
        std::unique_ptr<ZarrImageReader> m_zarrImageReader;
        
        int32_t m_dimensionIndexX = -1;
        
        int32_t m_dimensionIndexY = -1;
        
        int32_t m_dimensionIndexZ = -1;
        
        int32_t m_dimensionIndexTime = -1;
         
        int32_t m_dimensionIndexChannel = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_DATA_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_DATA_SET_DECLARE__

} // namespace
#endif  //__OME_DATA_SET_H__
