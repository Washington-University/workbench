#ifndef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL__H_
#define __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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
