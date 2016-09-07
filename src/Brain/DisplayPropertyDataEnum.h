#ifndef __DISPLAY_PROPERTY_DATA_ENUM_H__
#define __DISPLAY_PROPERTY_DATA_ENUM_H__

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
#include "CaretAssert.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneableInterface.h"

/**
 * \class caret::DisplayPropertyDataEnum
 * \brief Enumerated data for display property tab/display group
 * \ingroup Brain
 */

namespace caret {
    class SceneClassAssistant;

    template <class T, typename ET>
    class DisplayPropertyDataEnum : public CaretObject, public SceneableInterface {
        
    public:
        /**
         * Constructor.
         */
        DisplayPropertyDataEnum()
        : CaretObject()
        {
            m_sceneAssistant.grabNew(NULL);
        }
        
        /**
         * Initializes with the given default value.
         *
         * MUST BE CALLED BEFORE ANY OTHER METHOD !!!
         *
         * @param defaultValue
         *    Default data value.
         */
        void initialize(const ET defaultValue)
        {
            m_sceneAssistant.grabNew(new SceneClassAssistant());
            
            /*
             * NOTE: must be called after creating scene class assistant
             */
            setAllValues(defaultValue);
            
            m_sceneAssistant->addTabIndexedEnumeratedTypeArray<T,ET>("m_tabValues",
                                                                     m_tabValues);
            m_sceneAssistant->addArray<T, ET>("m_displayGroupValues",
                                              m_displayGroupValues,
                                              DisplayGroupEnum::NUMBER_OF_GROUPS,
                                              defaultValue);
        }
        
        /**
         * Destructor.
         */
        virtual ~DisplayPropertyDataEnum()
        {
            
        }
        
        /**
         * Set all display groups and tabs to the given value.
         *
         * @param value
         *     The value.
         */
        void setAllValues(const ET value)
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
                m_tabValues[i] = value;
            }
            
            for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
                m_displayGroupValues[i] = value;
            }
        }
        
        /**
         * Copy the values from one tab to another.
         *
         * @param sourceTabIndex
         *    Index of tab from which values are copied.
         * @param targetTabIndex
         *    Index of tab to which values are copied.
         */
        void copyValues(const int32_t sourceTabIndex,
                        const int32_t targetTabIndex)
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            CaretAssertArrayIndex(m_tabValues,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  sourceTabIndex);
            CaretAssertArrayIndex(m_tabValues,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  targetTabIndex);
            
            m_tabValues[targetTabIndex] = m_tabValues[sourceTabIndex];
        }
        
        /**
         * @return The data value.
         *
         * @param displayGroup
         *     Display group.
         * @param tabIndex
         *     Tab index.
         */
        ET getValue(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            CaretAssertArrayIndex(m_displayGroupValues,
                                  DisplayGroupEnum::NUMBER_OF_GROUPS,
                                  static_cast<int32_t>(displayGroup));
            if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
                CaretAssertArrayIndex(m_tabValues,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      tabIndex);
                return m_tabValues[tabIndex];
            }
            return m_displayGroupValues[displayGroup];
        }
        
        /**
         * Set the data value.
         *
         * @param displayGroup
         *     Display group.
         * @param tabIndex
         *     Tab index.
         * @param value
         *     New value.
         */
        void setValue(const DisplayGroupEnum::Enum displayGroup,
                      const int32_t tabIndex,
                      const ET value)
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            CaretAssertArrayIndex(m_displayGroupValues,
                                  DisplayGroupEnum::NUMBER_OF_GROUPS,
                                  static_cast<int32_t>(displayGroup));
            if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
                CaretAssertArrayIndex(m_tabValues,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      tabIndex);
                m_tabValues[tabIndex] = value;
            }
            else {
                m_displayGroupValues[displayGroup] = value;
            }
        }
        

        /**
         * Save information specific to this type of model to the scene.
         *
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    saving the scene.
         *
         * @param instanceName
         *    Name of instance in the scene.
         */
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName)
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            SceneClass* sceneClass = new SceneClass(instanceName,
                                                    "DisplayPropertyDataEnum",
                                                    1);
            m_sceneAssistant->saveMembers(sceneAttributes,
                                          sceneClass);
            
            return sceneClass;
        }

        /**
         * Restore information specific to the type of model from the scene.
         *
         * @param sceneAttributes
         *    Attributes for the scene.  Scenes may be of different types
         *    (full, generic, etc) and the attributes should be checked when
         *    restoring the scene.
         *
         * @param sceneClass
         *     sceneClass from which model specific information is obtained.
         */
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
        {
            CaretAssertMessage(m_sceneAssistant.getPointer(), "Failed to call initialize(defaultValue) for the enumerated type");
            
            if (sceneClass == NULL) {
                return;
            }
            
            m_sceneAssistant->restoreMembers(sceneAttributes,
                                             sceneClass);
        }

          
          
          
          
          
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
        DisplayPropertyDataEnum(const DisplayPropertyDataEnum& obj);
        
        DisplayPropertyDataEnum& operator=(const DisplayPropertyDataEnum& obj);
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        ET m_displayGroupValues[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        ET m_tabValues[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_PROPERTY_DATA_ENUM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTY_DATA_ENUM_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTY_DATA_ENUM_H__
