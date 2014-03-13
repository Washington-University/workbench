#ifndef __FIBER_TRAJECTORY_COLOR_MODEL_H__
#define __FIBER_TRAJECTORY_COLOR_MODEL_H__

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

#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class FiberTrajectoryColorModel : public CaretObject, public SceneableInterface {
        
    public:
        
        class Item : public SceneableInterface {
        public:
            enum ItemType {
                /**
                 * Type of item
                 */
                ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE,
                ITEM_TYPE_CARET_COLOR
            };
            
            Item(const CaretColorEnum::Enum caretColor);
            
            Item();
            
            ~Item();
            
            bool equals(const Item& item) const;
            
            AString getName() const;
            
            ItemType getItemType() const;
            
            CaretColorEnum::Enum getCaretColor() const;
            
            virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                            const AString& instanceName);
            
            virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass);
        private:
            void initializeAtEndOfConstruction();
            
            SceneClassAssistant* m_sceneAssistant;
            
            CaretColorEnum::Enum m_caretColor;
            
            ItemType m_itemType;
        };
        
        
        FiberTrajectoryColorModel();
        
        virtual ~FiberTrajectoryColorModel();
        
        std::vector<Item*> getValidItems();
        
        Item* getSelectedItem();
        
        const Item* getSelectedItem() const;
        
        void setSelectedItem(const Item* item);
        
        void setCaretColor(const CaretColorEnum::Enum color);
        
        bool isFiberOrientationColoringTypeSelected() const;
        
        void setFiberOrientationColoringTypeSelected();

        void copy(const FiberTrajectoryColorModel& other);
    
    private:
        FiberTrajectoryColorModel(const FiberTrajectoryColorModel&);

        FiberTrajectoryColorModel& operator=(const FiberTrajectoryColorModel&);
        
    public:

        // ADD_NEW_METHODS_HERE
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

    private:
        SceneClassAssistant* m_sceneAssistant;

        std::vector<Item*> m_allItems;
        
        Item* m_selectedItem;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FIBER_TRAJECTORY_COLOR_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_TRAJECTORY_COLOR_MODEL_DECLARE__

} // namespace
#endif  //__FIBER_TRAJECTORY_COLOR_MODEL_H__
