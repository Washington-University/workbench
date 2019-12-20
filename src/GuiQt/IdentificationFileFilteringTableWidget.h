#ifndef __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_H__
#define __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <QTableWidget>

#include "AString.h"

class QComboBox;
class QTableWidgetItem;

namespace caret {
    class CaretMappableDataFile;
    class EnumComboBoxTemplate;
    
    class IdentificationFileFilteringTableWidget : public QTableWidget {
        
        Q_OBJECT

    public:
        IdentificationFileFilteringTableWidget();
        
        virtual ~IdentificationFileFilteringTableWidget();
        
        IdentificationFileFilteringTableWidget(const IdentificationFileFilteringTableWidget&) = delete;

        IdentificationFileFilteringTableWidget& operator=(const IdentificationFileFilteringTableWidget&) = delete;
        
        void updateContent();

        // ADD_NEW_METHODS_HERE

    private slots:
        void tableCellClicked(int row, int column);
        
        void mapModeComboBoxChanged(int row);
        
        void mapNameComboBoxChanged(int row);
        
    private:
        enum COLUMNS {
            COLUMN_FILE_NAME_LABEL = 0,
            COLUMN_DISPLAYED_LABEL = 1,
            COLUMN_ENABLED_CHECKBOX = 2,
            COLUMN_MAP_MODE_COMBO_BOX = 3,
            COLUMN_MAP_NAME_COMBO_BOX = 4,
            COLUMN_COUNT = 5
        };

        AString getColumnName(const int32_t) const;
        
        void updateTableRows(const int32_t numberOfFiles);
        
        int32_t m_lastNumberOfFiles = 0;
        
        std::vector<CaretMappableDataFile*> m_mapFiles;
        
        std::vector<EnumComboBoxTemplate*> m_mapModeComboBoxes;
        
        std::vector<QComboBox*> m_mapNameComboBoxes;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_H__
