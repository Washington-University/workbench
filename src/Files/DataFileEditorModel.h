#ifndef __DATA_FILE_EDITOR_MODEL_H__
#define __DATA_FILE_EDITOR_MODEL_H__

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
#include <vector>

#include <QStandardItemModel>

#include "DataFileEditorItemTypeEnum.h"

namespace caret {
    class DataFileEditorItem;
    class DataFileEditorColumnContent;
    class FociFile;

    class DataFileEditorModel : public QStandardItemModel {
        
        Q_OBJECT

    public:        
        DataFileEditorModel();
        
        virtual ~DataFileEditorModel();
        
        DataFileEditorModel(const DataFileEditorModel&) = delete;

        DataFileEditorModel& operator=(const DataFileEditorModel&) = delete;
        
        std::vector<DataFileEditorItem*> getItemsFromIndices(const QModelIndexList& indexesList) const;

        DataFileEditorItem* getDataFileItemAtRowColumn(const int32_t rowIndex,
                                                       const int32_t columnIndex) const;

        int32_t getDefaultSortingColumnIndex() const;
        
        void setDefaultSortingColumnIndex(const int32_t columnIndex);
        
        void setNumberOfColumnsAndColumnTitles(const DataFileEditorColumnContent& modelContent);
        
        QModelIndex findItemsWithText(const AString& firstText,
                                      const AString& secondText) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        int32_t m_defaultSortingColumnIndex = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_EDITOR_MODEL_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_MODEL_H__
