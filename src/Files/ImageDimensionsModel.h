#ifndef __IMAGE_DIMENSIONS_MODEL_H__
#define __IMAGE_DIMENSIONS_MODEL_H__

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


#include "CaretObject.h"
#include "ImagePixelsPerSpatialUnitsEnum.h"
#include "ImageSpatialUnitsEnum.h"



namespace caret {

    class ImageDimensionsModel : public CaretObject {
        
    public:
        ImageDimensionsModel();
        
        virtual ~ImageDimensionsModel();
        
        ImageDimensionsModel(const ImageDimensionsModel& obj);

        ImageDimensionsModel& operator=(const ImageDimensionsModel& obj);
        
        int32_t getPixelWidth() const;
        
        int32_t getPixelHeight() const;
        
        float getSpatialWidth(const ImageSpatialUnitsEnum::Enum spatialUnits) const;
        
        float getSpatialHeight(const ImageSpatialUnitsEnum::Enum spatialUnits) const;
        
        float getNumberOfPixelsPerSpatialUnit(const ImagePixelsPerSpatialUnitsEnum::Enum pixelsPerSpatialUnit) const;
        
        void setPixelWidthAndHeight(const int32_t pixelWidth,
                                    const int32_t pixelHeight);
        
        void setPixelWidth(const int32_t pixelWidth,
                           const bool preserveAspectRatio);
        
        void setPixelHeight(const int32_t pixelHeight,
                           const bool preserveAspectRatio);
        
        void setSpatialWidth(const float spatialWidth,
                             const ImageSpatialUnitsEnum::Enum spatialUnit,
                             const bool preserveAspectRatio);
        
        void setSpatialHeight(const float spatialHeight,
                              const ImageSpatialUnitsEnum::Enum spatialUnit,
                              const bool preserveAspectRatio);
        
        void setNumberOfPixelsPerSpatialUnit(const float numberOfPixelsPerSpatialUnit,
                                     const ImagePixelsPerSpatialUnitsEnum::Enum pixelsPerSpatialUnit);
        
        void updateForAspectRatio(const float width,
                                  const float height);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperImageDimensionsModel(const ImageDimensionsModel& obj);

        void updatePixelWidthAndHeightFromSpatialWidthAndHeight();
        
        void updateSpatialWidthAndHeightFromPixelWidthAndHeight();
        
        float getAspectRatio() const;
        
        // ADD_NEW_MEMBERS_HERE

        /** 
         * Width/height of pixels are float.  Otherwise, small changes in
         * pixel dimensions will get lost (truncated).
         */
        float m_pixelWidth;
        
        float m_pixelHeight;
        
        float m_centimetersWidth;
        
        float m_centimetersHeight;
        
        float m_pixelsPerCentimeter;
        
        float m_aspectRatio;
        
        static const float CENTIMETERS_PER_INCH;
        static const float MILLIMETERS_PER_CENTIMETER;
    };
    
#ifdef __IMAGE_DIMENSIONS_MODEL_DECLARE__
    const float ImageDimensionsModel::CENTIMETERS_PER_INCH = 2.54;
    const float ImageDimensionsModel::MILLIMETERS_PER_CENTIMETER = 10.0;
#endif // __IMAGE_DIMENSIONS_MODEL_DECLARE__

} // namespace
#endif  //__IMAGE_DIMENSIONS_MODEL_H__
