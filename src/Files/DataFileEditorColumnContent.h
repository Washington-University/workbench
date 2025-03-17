#ifndef __DATA_FILE_EDITOR_COLUMN_CONTENT_H__
#define __DATA_FILE_EDITOR_COLUMN_CONTENT_H__

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


#include <cstdint>
#include <memory>

#include "CaretObject.h"
#include "DataFileEditorItemTypeEnum.h"

namespace caret {

    class DataFileEditorColumnContent : public CaretObject {
        
    public:
        DataFileEditorColumnContent();
        
        virtual ~DataFileEditorColumnContent();
        
        DataFileEditorColumnContent(const DataFileEditorColumnContent&) = delete;
        
        DataFileEditorColumnContent& operator=(const DataFileEditorColumnContent&) = delete;
        
        void addColumn(const DataFileEditorItemTypeEnum::Enum columnDataType,
                       const QString& columnName);

        void setDefaultSortingColumnDataType(const DataFileEditorItemTypeEnum::Enum columnDataType);
        
        DataFileEditorItemTypeEnum::Enum getDefaultSortingColumnDataType() const;
        
        int32_t getNumberOfColumns() const;
        
        DataFileEditorItemTypeEnum::Enum getColumnDataType(const int32_t columnIndex) const;
        
        QString getColumnName(const int32_t columnIndex) const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class ColumnInfo {
        public:
            ColumnInfo(const DataFileEditorItemTypeEnum::Enum dataType,
                       const QString& name)
            : m_dataType(dataType),
            m_name(name) { }
            
            DataFileEditorItemTypeEnum::Enum m_dataType;
            QString m_name;
        };
        
        std::vector<ColumnInfo> m_columnInfo;

        DataFileEditorItemTypeEnum::Enum m_defaultSortingColumnDataType = DataFileEditorItemTypeEnum::NAME;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_COLUMN_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_EDITOR_COLUMN_CONTENT_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_COLUMN_CONTENT_H__
