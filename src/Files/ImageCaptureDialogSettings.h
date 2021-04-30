#ifndef __IMAGE_CAPTURE_DIALOG_SETTINGS_H__
#define __IMAGE_CAPTURE_DIALOG_SETTINGS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "ImageCaptureDimensionsModeEnum.h"
#include "ImageResolutionUnitsEnum.h"
#include "ImageSpatialUnitsEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ImageCaptureDialogSettings : public CaretObject, public SceneableInterface {
        
    public:
        ImageCaptureDialogSettings();
        
        virtual ~ImageCaptureDialogSettings();
        
        ImageCaptureDialogSettings(const ImageCaptureDialogSettings& obj);

        ImageCaptureDialogSettings& operator=(const ImageCaptureDialogSettings& obj);
        
        AString getSettingsAsText(const std::vector<int32_t>& windowIndices,
                                  const std::vector<int32_t>& windowWidths,
                                  const std::vector<int32_t>& windowHeights) const;
        
        int32_t getPixelWidth() const;
        
        int32_t getPixelHeight() const;
        
        float getSpatialWidth() const;
        
        float getSpatialHeight() const;
        
        float getImageResolutionInCentimeters() const;
        
        float getImageResolutionInSelectedUnits() const;
        
        void setPixelWidthAndHeight(const int32_t pixelWidth,
                                    const int32_t pixelHeight);
        
        void setPixelWidth(const int32_t pixelWidth);
        
        void setPixelHeight(const int32_t pixelHeight);
        
        void setSpatialWidth(const float spatialWidth);
        
        void setSpatialHeight(const float spatialHeight);
        
        void setImageResolutionInSelectedUnits(const float imageResolutionInSelectedUnits);
        
        void updateForAspectRatio(const float width,
                                  const float height);

        int32_t getMargin() const;
        
        void setMargin(const int32_t margin);
        
        bool isCroppingEnabled() const;
        
        void setCroppingEnabled(const bool enabled);
        
        bool isCopyToClipboardEnabled() const;
        
        void setCopyToClipboardEnabled(const bool enabled);
        
        bool isSaveToFileEnabled() const;
        
        void setSaveToFileEnabled(const bool enabled);
        
        AString getImageFileName() const;
        
        void setImageFileName(const AString& filename);
        
        ImageCaptureDimensionsModeEnum::Enum getImageCaptureDimensionsMode() const;
        
        void setImageCaptureDimensionsMode(const ImageCaptureDimensionsModeEnum::Enum mode);
        
        ImageResolutionUnitsEnum::Enum getImageResolutionUnits() const;
        
        void setImageResolutionUnits(const ImageResolutionUnitsEnum::Enum imageResolutionUnits);
        
        ImageSpatialUnitsEnum::Enum getSpatialUnits() const;
        
        void setSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits);
        
        bool isScaleProportionately() const;
        
        void setScaleProportionately(const bool enabled);
        
        bool isCropToTabWindowLockAspectRegionEnabled() const;
        
        void setCropToTabWindowLockAspectRegionEnabled(const bool enabled);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperImageDimensionsModel(const ImageCaptureDialogSettings& obj);

        SceneClassAssistant* m_sceneAssistant;

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
        
        bool m_scaleProportionatelyEnabled;
        
        int32_t m_margin;
        
        bool m_croppingEnabled;
        
        bool m_copyToClipboardEnabled;
        
        bool m_saveToFileEnabled;
        
        AString m_imageFileName;
        
        bool m_cropToTabWindowLockAspectRegionEnabled;
        
        ImageCaptureDimensionsModeEnum::Enum m_dimensionsMode;
        
        ImageResolutionUnitsEnum::Enum m_imageResolutionUnits;
        
        ImageSpatialUnitsEnum::Enum m_spatialUnits;
        
        static const float CENTIMETERS_PER_INCH;
        static const float CENTIMETERS_PER_METER;
        static const float CENTIMETERS_PER_MILLIMETER;
        static const float METERS_PER_CENTIMETER;
        static const float MILLIMETERS_PER_CENTIMETER;

    };
    
#ifdef __IMAGE_CAPTURE_DIALOG_SETTINGS_DECLARE__
    const float ImageCaptureDialogSettings::CENTIMETERS_PER_INCH = 2.54;
    const float ImageCaptureDialogSettings::METERS_PER_CENTIMETER = 0.01;
    const float ImageCaptureDialogSettings::MILLIMETERS_PER_CENTIMETER = 10.0;
    const float ImageCaptureDialogSettings::CENTIMETERS_PER_METER = 100.0;
    const float ImageCaptureDialogSettings::CENTIMETERS_PER_MILLIMETER = 0.10;
#endif // __IMAGE_CAPTURE_DIALOG_SETTINGS_DECLARE__

} // namespace
#endif  //__IMAGE_CAPTURE_DIALOG_SETTINGS_H__
