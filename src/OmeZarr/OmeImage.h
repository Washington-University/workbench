#ifndef __OME_IMAGE_H__
#define __OME_IMAGE_H__

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


#include <cstdint>
#include <memory>

#include <xtensor/xarray.hpp>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "OmeDimensionSizes.h"

namespace caret {

    class OmeDimensionIndices;
    
    class OmeImage : public CaretObject {
        
    public:
        OmeImage(xt::xarray<uint8_t>* dataArray,
                 const OmeDimensionIndices& dimensionIndices);
        
        virtual ~OmeImage();
        
        OmeImage(const OmeImage&) = delete;

        OmeImage& operator=(const OmeImage&) = delete;
        
        bool isValid() const;

        uint8_t getElement(const int64_t xIndex,
                           const int64_t yIndex,
                           const int64_t zIndex,
                           const int64_t timeIndex,
                           const int64_t channelIndex) const;
        
        int64_t getNumberOfDimensions() const;
        
        const OmeDimensionSizes& getDimensionSizes() const;
        
        FunctionResultValue<uint8_t*> getDataForOpenGLTexture() const;
        
        FunctionResult writeAsQImage(const AString& filename) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        bool elementIndicesToDataIndices(const int64_t xIndex,
                                         const int64_t yIndex,
                                         const int64_t zIndex,
                                         const int64_t timeIndex,
                                         const int64_t channelIndex,
                                         std::vector<uint64_t>& dataIndicesOut) const;
        
        std::unique_ptr<xt::xarray<uint8_t>> m_dataArray;
        
        const int64_t m_xDimensionIndex;
        
        const int64_t m_yDimensionIndex;
        
        const int64_t m_zDimensionIndex;
        
        const int64_t m_timeDimensionIndex;
        
        const int64_t m_channelDimensionIndex;
        
        int64_t m_numberOfValidDimensionIndices;
        
        OmeDimensionSizes m_dimensionSizes;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_IMAGE_DECLARE__

} // namespace
#endif  //__OME_IMAGE_H__
