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

#include <array>
#include <memory>

#include <xtensor.hpp>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "OmeCoordinateTransformations.h"
#include "OmeDimensionIndices.h"
#include "OmeImage.h"
#include "Vector3D.h"
#include "ZarrDataTypeEnum.h"
#include "ZarrDriverTypeEnum.h"

namespace caret {

    class ZarrImageReader;
    
    class OmeDataSet : public CaretObject {
        
    public:
        OmeDataSet();
        
        virtual ~OmeDataSet();
        
        OmeDataSet(const OmeDataSet& obj) = delete;

        OmeDataSet& operator=(const OmeDataSet& obj) = delete;

        int32_t getNumberOfCoordinateTransformations() const;
        
        void addCoordinateTransformation(const OmeCoordinateTransformations& oct);
        
        OmeCoordinateTransformations getCoordinateTransfomation(const int32_t index) const;

        AString getRelativePath() const;
        
        void setRelativePath(const AString& relativePath);
        
        ZarrDataTypeEnum::Enum getZarrDataType() const;
        
        std::vector<int64_t> getDimensions() const;
        
        void setDimensionIndices(const OmeDimensionIndices& dimensionIndices);
        
        int64_t getWidth() const;

        int64_t getHeight() const;
        
        int64_t getNumberOfSlices() const;
        
        int64_t getNumberOfTimePoints() const;
        
        int64_t getNumberOfChannels() const;
        
        Vector3D getPixelCoordinate(const int64_t pixelI,
                                    const int64_t pixelJ,
                                    const int64_t pixelK) const;
        
        FunctionResult initializeForReading(const ZarrDriverTypeEnum::Enum driverType,
                                            const AString& zarrPath);
        
        FunctionResultValue<OmeImage*> readSlice(const int64_t sliceIndex) const;
        
        FunctionResultValue<std::array<uint8_t, 4>> readSlicePixel(const int64_t sliceIndex,
                                                                   const int64_t pixelI,
                                                                   const int64_t pixelJ) const;

        FunctionResultValue<uint8_t*> readDataSetForImage(const int64_t sliceIndex) const;
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;

    private:
        /** The subdirectory path*/
        AString m_relativePath;

        std::vector<OmeCoordinateTransformations> m_coordinateTransformations;
        
        ZarrDataTypeEnum::Enum m_zarrDataType = ZarrDataTypeEnum::UNKNOWN;

        std::vector<int64_t> m_dimensions;
        
        std::unique_ptr<ZarrImageReader> m_zarrImageReader;
        
        OmeDimensionIndices m_dimensionIndices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_DATA_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_DATA_SET_DECLARE__

} // namespace
#endif  //__OME_DATA_SET_H__
