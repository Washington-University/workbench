#ifndef __DISPLAY_PROPERTIES_ANNOTATION_TEXT_SUBSTITUTION_H__
#define __DISPLAY_PROPERTIES_ANNOTATION_TEXT_SUBSTITUTION_H__

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
#include "EventListenerInterface.h"


namespace caret {
    class Annotation;
    class AnnotationTextSubstitutionFile;
    class Brain;

    class DisplayPropertiesAnnotationTextSubstitution : public DisplayProperties, public EventListenerInterface {
        
    public:
        DisplayPropertiesAnnotationTextSubstitution(Brain* parentBrain);
        
        virtual ~DisplayPropertiesAnnotationTextSubstitution();
        
        virtual void receiveEvent(Event* event) override;
        
        virtual void reset();
        
        virtual void update();
        
        bool isEnableSubstitutions() const;
        
        void setEnableSubstitutions(const bool status);
        
        AnnotationTextSubstitutionFile* getSelectedFile();
        
        const AnnotationTextSubstitutionFile* getSelectedFile() const;
        
        void setSelectedFile(AnnotationTextSubstitutionFile* selectedFile);
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

    private:
        DisplayPropertiesAnnotationTextSubstitution(const DisplayPropertiesAnnotationTextSubstitution&);

        DisplayPropertiesAnnotationTextSubstitution& operator=(const DisplayPropertiesAnnotationTextSubstitution&);
        
        void resetPrivate();
        
        void validateSelectedFile() const;
        
        Brain* m_parentBrain;
        
        mutable AnnotationTextSubstitutionFile* m_selectedFile;
        
        bool m_enableSubstitutionsFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTIES_ANNOTATION_EXT_SUBSTITUTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_ANNOTATION_EXT_SUBSTITUTION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_ANNOTATION_TEXT_SUBSTITUTION_H__
