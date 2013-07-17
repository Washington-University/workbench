#ifndef __IMAGE_DIMENSIONS_MODEL_H__
#define __IMAGE_DIMENSIONS_MODEL_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
