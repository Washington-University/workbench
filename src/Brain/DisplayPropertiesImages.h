#ifndef __DISPLAY_PROPERTIES_IMAGES_H__
#define __DISPLAY_PROPERTIES_IMAGES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "BrainConstants.h"
#include "CaretPointer.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"
#include "DisplayPropertyDataEnum.h"
#include "ImageDepthPositionEnum.h"

namespace caret {

    class Brain;
    class DisplayPropertyDataBoolean;
    class DisplayPropertyDataFloat;
    class ImageFile;
    
    class DisplayPropertiesImages : public DisplayProperties {
        
    public:
        DisplayPropertiesImages(Brain* parentBrain);
        
        virtual ~DisplayPropertiesImages();
        
        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayStatus);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                                   const DisplayGroupEnum::Enum  displayGroup);
        
        ImageFile* getSelectedImageFile(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex) const;
        
        void setSelectedImageFile(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             ImageFile* imageFile);
        
        float getThresholdMinimum(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex) const;
        
        void setThresholdMinimum(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const float value);
        
        float getThresholdMaximum(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex) const;
        
        void setThresholdMaximum(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const float value);
        
        float getOpacity(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setOpacity(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex,
                        const float value);
        
        ImageDepthPositionEnum::Enum getImagePosition(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex) const;
        
        void setImagePosition(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const ImageDepthPositionEnum::Enum positionType);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        

        // ADD_NEW_METHODS_HERE

    private:
        DisplayPropertiesImages(const DisplayPropertiesImages&);

        DisplayPropertiesImages& operator=(const DisplayPropertiesImages&);
        
        ImageFile* findImageFile(const AString& imageFileName) const;
        
        Brain* m_parentBrain;
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CaretPointer<DisplayPropertyDataBoolean> m_displayStatus;
        
        mutable ImageFile* m_imageFileInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        mutable ImageFile* m_imageFileInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        DisplayPropertyDataEnum<ImageDepthPositionEnum, ImageDepthPositionEnum::Enum> m_imageDepthPosition;
        
        CaretPointer<DisplayPropertyDataFloat> m_thresholdMinimum;
        
        CaretPointer<DisplayPropertyDataFloat> m_thresholdMaximum;
        
        CaretPointer<DisplayPropertyDataFloat> m_opacity;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTIES_IMAGES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_IMAGES_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_IMAGES_H__
