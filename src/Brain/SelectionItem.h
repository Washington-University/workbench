#ifndef __SELECTION_ITEM__H_
#define __SELECTION_ITEM__H_

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


#include "CaretObject.h"
#include "SelectionItemDataTypeEnum.h"

namespace caret {
     
    class Brain;
    
    class SelectionItem : public CaretObject {
        
    protected:
        SelectionItem(const SelectionItemDataTypeEnum::Enum itemDataType);
    
        SelectionItem(const SelectionItem&);
        
        SelectionItem& operator=(const SelectionItem&);
        
    public:
        virtual ~SelectionItem();
        
        int64_t getIdentifiedItemUniqueIdentifier() const;
        
        void setIdentifiedItemUniqueIdentifier(const int64_t uniqueIdentifier);
        
        SelectionItemDataTypeEnum::Enum getItemDataType() const;
        
        bool isEnabledForSelection() const;
        
        void setEnabledForSelection(const bool enabled);
        
        Brain* getBrain();
        
        void setBrain(Brain* brain);
        
        bool isOtherScreenDepthCloserToViewer(const double otherScreenDepth) const;
        
        double getScreenDepth() const;
        
        void setScreenDepth(const double screenDepth);
        
        void getScreenXYZ(double screenXYZ[3]) const;
        
        void setScreenXYZ(const double screenXYZ[3]);
        
        void setScreenXYZ(const float screenXYZ[3]);
        
        void getModelXYZ(double modelXYZ[3]) const;
        
        void getModelXYZ(float modelXYZ[3]) const;
        
        void setModelXYZ(const double modelXYZ[3]);
        
        void setModelXYZ(const float modelXYZ[3]);
        
        /**
         * @return  Is the selected item valid?
         */
        virtual bool isValid() const = 0;
        
        virtual void reset();
        
    private:
    public:
        virtual AString toString() const;
        
    protected:
        SelectionItemDataTypeEnum::Enum m_itemDataType;
        
        bool m_enabledForSelection;
        
        Brain* m_brain;
        
        double m_screenDepth;
        
        double m_screenXYZ[3];
        
        double m_modelXYZ[3];
        
        int64_t m_identifiedItemUniqueIdentifier = -1;
        
    private:
        void copyHelperSelectionItem(const SelectionItem& idItem);
    };
    
#ifdef __SELECTION_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM__H_
