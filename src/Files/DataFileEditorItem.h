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
#include "DataFileEditorItemTypeEnum.h"

class QCollator;

namespace caret {
    class Annotation;
    class Border;
    class Focus;
 
    class DataFileEditorItem : public QStandardItem {
        
    public:
        DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                           std::shared_ptr<Annotation> annotation,
                           const AString& text,
                           const AString& sortingKeyText,
                           const float iconRGBA[4]);
        
        DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                           std::shared_ptr<Focus> focus,
                           const AString& text,
                           const AString& sortingKeyText,
                           const float iconRGBA[4]);

        DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                           std::shared_ptr<Border> border,
                           const AString& text,
                           const AString& sortingKeyText,
                           const float iconRGBA[4]);
        
        virtual ~DataFileEditorItem();
        
        DataFileEditorItem(const DataFileEditorItem& obj);

        DataFileEditorItem& operator=(const DataFileEditorItem& obj) = delete;

        virtual QStandardItem* clone() const override;

        virtual int type() const;
        
        virtual bool operator<(const QStandardItem &other) const override;
        
        const Annotation* getAnnotation() const;
        
        const Border* getBorder() const;
        
        const Focus* getFocus() const;
        
        const Annotation* getSample() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        QIcon createIcon(const float rgba[4]) const;
        
        DataFileEditorItemTypeEnum::Enum m_dataItemType;
        
        /*
         * All items in one row represent the same annotation
         */
        std::shared_ptr<Annotation> m_annotation;
        
        /*
         * All items in one row represent the same border
         */
        std::shared_ptr<Border> m_border;
        
        /*
         * All items in one row represent the same focus
         */
        std::shared_ptr<Focus> m_focus;

        /*
         * Text used for sorting this item
         */
        AString m_sortingKeyText;
        
        static QCollator* s_collator;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_ITEM_DECLARE__
    QCollator* DataFileEditorItem::s_collator = NULL;
#endif // __DATA_FILE_EDITOR_ITEM_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_ITEM_H__
