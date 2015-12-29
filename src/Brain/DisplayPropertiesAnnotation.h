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
#include "DisplayProperties.h"



namespace caret {
    class Brain;

    class DisplayPropertiesAnnotation : public DisplayProperties {
        
    public:
        DisplayPropertiesAnnotation(Brain* parentBrain);
        
        virtual ~DisplayPropertiesAnnotation();
        
        virtual void reset();
        
        virtual void update();
        
        bool isDisplayModelAnnotationsInTab(const int32_t tabIndex) const;
        
        bool isDisplayModelAnnotationsInTabs(const std::vector<int32_t>& tabIndices) const;
        
        void setDisplayModelAnnotationsInTab(const int32_t tabIndex,
                                        const bool status);
        
        bool isDisplaySurfaceAnnotationsInTab(const int32_t tabIndex) const;
        
        bool isDisplaySurfaceAnnotationsInTabs(const std::vector<int32_t>& tabIndices) const;
        
        void setDisplaySurfaceAnnotationsInTab(const int32_t tabIndex,
                                        const bool status);
        
        bool isDisplayTabAnnotationsInTab(const int32_t tabIndex) const;
        
        void setDisplayTabAnnotationsInTab(const int32_t tabIndex,
                                        const bool status);
        
        bool isDisplayWindowAnnotationsInTab(const int32_t windowIndex) const;
        
        void setDisplayWindowAnnotationsInTab(const int32_t windowIndex,
                                        const bool status);
        
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
        
        Brain* m_parentBrain;
        
        bool m_displayModelAnnotations[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displaySurfaceAnnotations[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayTabAnnotations[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayWindowAnnotations[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_ANNOTATION_H__
