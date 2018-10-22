#ifndef __SPACER_TAB_INDEX_H__
#define __SPACER_TAB_INDEX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"

namespace caret {
    class SceneClassAssistant;

    class SpacerTabIndex : public CaretObject {
        
    public:
        SpacerTabIndex();
        
        SpacerTabIndex(const int32_t windowIndex,
                       const int32_t rowIndex,
                       const int32_t columnIndex);
        
        virtual ~SpacerTabIndex();
        
        SpacerTabIndex(const SpacerTabIndex& obj);

        SpacerTabIndex& operator=(const SpacerTabIndex& obj);
        
        bool operator==(const SpacerTabIndex& obj) const;
        
        bool operator!=(const SpacerTabIndex& obj) const;
        
        bool operator<(const SpacerTabIndex& rhs) const;
        
        bool isValid() const;
        
        void reset();
        
        int32_t getWindowIndex() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        AString getRowColumnGuiText() const;
        
        AString getWindowRowColumnGuiText() const;
        
        AString getXmlAttributeText() const;
        
        void setFromXmlAttributeText(const AString& text);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;

    private:
        void copyHelperSpacerTabIndex(const SpacerTabIndex& obj);

        int32_t m_windowIndex;
        int32_t m_rowIndex;
        int32_t m_columnIndex;
        
        // ADD_NEW_MEMBERS_HERE

        friend class SpacerTabContent;
    };
    
#ifdef __SPACER_TAB_INDEX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPACER_TAB_INDEX_DECLARE__

} // namespace
#endif  //__SPACER_TAB_INDEX_H__
