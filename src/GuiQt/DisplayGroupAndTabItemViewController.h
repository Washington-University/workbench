#ifndef __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__
#define __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__

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


#include <QWidget>

#include "DisplayGroupEnum.h"

class QTreeWidget;

namespace caret {

    class DisplayGroupAndTabItemInterface;
    class DisplayGroupAndTabItemTreeWidgetItem;
    
    class DisplayGroupAndTabItemViewController : public QWidget {
        
        Q_OBJECT

    public:
        DisplayGroupAndTabItemViewController(const int32_t browserWindowIndex,
                                             QWidget* parent = 0);
        
        virtual ~DisplayGroupAndTabItemViewController();

        void updateContent(std::vector<DisplayGroupAndTabItemInterface*>& contentItems,
                           const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex);

        // ADD_NEW_METHODS_HERE

    private:
        DisplayGroupAndTabItemViewController(const DisplayGroupAndTabItemViewController&);

        DisplayGroupAndTabItemViewController& operator=(const DisplayGroupAndTabItemViewController&);
        
        DisplayGroupAndTabItemInterface *m_displayGroupAndTabItem;
        
        const int32_t m_browserWindowIndex;
        
        QTreeWidget* m_treeWidget;
        
        std::vector<DisplayGroupAndTabItemInterface*> m_contentItems;
        
        std::vector<DisplayGroupAndTabItemTreeWidgetItem*> m_contentItemWidgets;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__
