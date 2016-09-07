#ifndef __DISPLAY_PROPERTY_DATA_BOOLEAN_H__
#define __DISPLAY_PROPERTY_DATA_BOOLEAN_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class DisplayPropertyDataBoolean : public CaretObject, public SceneableInterface {
        
    public:
        DisplayPropertyDataBoolean(const bool defaultValue);
        
        virtual ~DisplayPropertyDataBoolean();
        
        void setAllValues(const bool value);
        
        void copyValues(const int32_t sourceTabIndex,
                        const int32_t targetTabIndex);
        
        bool getValue(const DisplayGroupEnum::Enum displayGroup,
                       const int32_t tabIndex) const;
        
        void setValue(const DisplayGroupEnum::Enum displayGroup,
                      const int32_t tabIndex,
                      const bool value);
        

        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        DisplayPropertyDataBoolean(const DisplayPropertyDataBoolean& obj);
        
        DisplayPropertyDataBoolean& operator=(const DisplayPropertyDataBoolean& obj);
        
        SceneClassAssistant* m_sceneAssistant;

        bool m_displayGroupValues[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_tabValues[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTY_DATA_BOOLEAN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTY_DATA_BOOLEAN_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTY_DATA_BOOLEAN_H__
