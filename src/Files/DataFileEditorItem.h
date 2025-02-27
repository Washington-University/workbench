#ifndef __DATA_FILE_EDITOR_ITEM_H__
#define __DATA_FILE_EDITOR_ITEM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include <QStandardItem>

#include "AString.h"

class QCollator;

namespace caret {
    class Border;
    class Focus;
 
    class DataFileEditorItem : public QStandardItem {
        
    public:
        enum class ItemType {
            NAME  = QStandardItem::UserType + 1,
            CLASS = QStandardItem::UserType + 2,
            XYZ   = QStandardItem::UserType + 3
        };
        
        DataFileEditorItem(const ItemType dataItemType,
                           std::shared_ptr<Focus> focus,
                           const AString& text,
                           const float iconRGBA[4]);        

        DataFileEditorItem(const ItemType dataItemType,
                           std::shared_ptr<Border> border,
                           const AString& text,
                           const float iconRGBA[4]);
        
        virtual ~DataFileEditorItem();
        
        DataFileEditorItem(const DataFileEditorItem& obj);

        DataFileEditorItem& operator=(const DataFileEditorItem& obj) = delete;

        virtual QStandardItem* clone() const override;

        virtual int type() const;
        
        virtual bool operator<(const QStandardItem &other) const override;
        
        const Border* getBorder() const;
        
        const Focus* getFocus() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        QIcon createIcon(const float rgba[4]) const;
        
        ItemType m_dataItemType;
        
        /*
         * All items in one row represent the same focus
         */
        std::shared_ptr<Border> m_border;
        
        /*
         * All items in one row represent the same focus
         */
        std::shared_ptr<Focus> m_focus;

        static QCollator* s_collator;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_ITEM_DECLARE__
    QCollator* DataFileEditorItem::s_collator = NULL;
#endif // __DATA_FILE_EDITOR_ITEM_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_ITEM_H__
