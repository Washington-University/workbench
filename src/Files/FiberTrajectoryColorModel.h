#ifndef __FIBER_TRAJECTORY_COLOR_MODEL_H__
#define __FIBER_TRAJECTORY_COLOR_MODEL_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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
