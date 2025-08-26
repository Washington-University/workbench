#ifndef __DISPLAY_PROPERTIES_ANNOTATION_H__
#define __DISPLAY_PROPERTIES_ANNOTATION_H__

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


#include "BrainConstants.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"



namespace caret {
    class Annotation;
    class Brain;

    class DisplayPropertiesAnnotation : public DisplayProperties {
        
    public:
        DisplayPropertiesAnnotation(Brain* parentBrain);
        
        virtual ~DisplayPropertiesAnnotation();
        
        virtual void reset();
        
        virtual void update();
        
        bool isDisplayAnnotations() const;
        
        void setDisplayAnnotations(const bool status);
        
        bool isDisplayTextAnnotations() const;
        
        void setDisplayTextAnnotations(const bool status);
        
        bool isTextSizeSmallSurfaceCorrectionsEnabled() const;
        
        void setTextSizeSmallSurfaceCorrectionsEnabled(const bool status);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                                   const DisplayGroupEnum::Enum displayGroup);
        
        bool isDisplayWindowAnnotationsInSingleTabViews(const int32_t windowIndex) const;
        
        void setDisplayWindowAnnotationsInSingleTabViews(const int32_t windowIndex,
                                                   const bool status);
        
        void updateForNewAnnotations(const std::vector<Annotation*>& annotations);
        
        void updateForNewAnnotation(const Annotation* annotation);
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

    private:
        DisplayPropertiesAnnotation(const DisplayPropertiesAnnotation&);

        DisplayPropertiesAnnotation& operator=(const DisplayPropertiesAnnotation&);
        
        void resetPrivate();
        
        void restoreVersionOne(const SceneClass* sceneClass);
        
        Brain* m_parentBrain;
        
        bool m_displayAnnotations;
        
        bool m_displayTextAnnotations;
        
        bool m_textSizeSmallSurfaceCorrectionsEnabled = false;
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayWindowAnnotationsInSingleTabViews[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_ANNOTATION_H__
