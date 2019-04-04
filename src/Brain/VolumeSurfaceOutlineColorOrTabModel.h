#ifndef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL__H_
#define __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL__H_

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

#include "BrainConstants.h"
#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "SceneableInterface.h"

namespace caret {
    class BrowserTabContent;
    class SceneAttributes;

    class VolumeSurfaceOutlineColorOrTabModel : public CaretObject, public SceneableInterface {
    public:
        
        class Item : public SceneableInterface {
        public:
           /**
             * Type of item.
             */
            enum ItemType {
                /** Item is a browser tab */
                ITEM_TYPE_BROWSER_TAB,
                /** Item is a color */
                ITEM_TYPE_COLOR
            };
            
            Item(const CaretColorEnum::Enum color);
            
            Item(const int32_t browserTabIndex);
            
            Item(const Item& item);
            
            ~Item();

            bool isValid() const;
            
            bool equals(const Item& item) const;
            
            AString getName();
            
            ItemType getItemType() const;
            
            BrowserTabContent* getBrowserTabContent();
            
            const BrowserTabContent* getBrowserTabContent() const;
            
            int32_t getBrowserTabIndex() const;
            
            CaretColorEnum::Enum getColor();
            
            virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                            const AString& instanceName);
            
            virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass);
        private:
            int32_t m_browserTabIndex;
            
            CaretColorEnum::Enum m_color;
            
            ItemType m_itemType;
        };
        
    public:
        VolumeSurfaceOutlineColorOrTabModel();
        
        virtual ~VolumeSurfaceOutlineColorOrTabModel();
        
        std::vector<Item*> getValidItems();
        
        void copyVolumeSurfaceOutlineColorOrTabModel(VolumeSurfaceOutlineColorOrTabModel* modelToCopy);
        
        Item* getSelectedItem();
        
        void setSelectedItem(const Item* item);
        
        void setColor(const CaretColorEnum::Enum color);
        
        void setBrowserTabIndex(const int32_t browserTabIndex);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        VolumeSurfaceOutlineColorOrTabModel(const VolumeSurfaceOutlineColorOrTabModel&);

        VolumeSurfaceOutlineColorOrTabModel& operator=(const VolumeSurfaceOutlineColorOrTabModel&);
        
        std::vector<Item*> m_allItems;
        
        Item* m_selectedItem;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL__H_
