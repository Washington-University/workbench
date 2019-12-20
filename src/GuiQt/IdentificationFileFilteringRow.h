#ifndef __IDENTIFICATION_FILE_FILTERING_ROW_H__
#define __IDENTIFICATION_FILE_FILTERING_ROW_H__

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



#include <memory>

#include <QObject>

class QCheckBox;
class QComboBox;
class QGridLayout;
class QLabel;



namespace caret {
    class CaretMappableDataFile;
    class EnumComboBoxTemplate;
    class WuQValueChangedSignalWatcher;
    class WuQWidgetObjectGroup;

    class IdentificationFileFilteringRow : public QObject {
        
        Q_OBJECT

    public:
        IdentificationFileFilteringRow(QGridLayout *gridLayout);
        
        virtual ~IdentificationFileFilteringRow();
        
        IdentificationFileFilteringRow(const IdentificationFileFilteringRow&) = delete;

        IdentificationFileFilteringRow& operator=(const IdentificationFileFilteringRow&) = delete;
        
        static void addGridLayoutColumnTitles(QGridLayout* gridLayout);
        
        void updateContent(CaretMappableDataFile* mapFile,
                           const bool fileDisplayedFlag);

        // ADD_NEW_METHODS_HERE

    private slots:
        void widgetValueChanged();
        
    private:
        enum COLUMNS {
            COLUMN_DISPLAYED_LABEL,
            COLUMN_FILE_NAME_LABEL,
            COLUMN_ENABLED_CHECKBOX,
            COLUMN_MAP_MODE_COMBO_BOX,
            COLUMN_MAP_NAME_COMBO_BOX
        };
        
        CaretMappableDataFile* m_mapFile = NULL;
        
        QCheckBox* m_enabledCheckBox;
        
        QLabel* m_displayedLabel;
        
        QLabel* m_fileNameLabel;
        
        EnumComboBoxTemplate* m_mapModeComboBox;
        
        QComboBox* m_mapNameComboBox;
        
        WuQValueChangedSignalWatcher* m_signalWatcher;
        
        WuQWidgetObjectGroup* m_objectGroup;
        
        // ADD_NEW_MEMBERS_HERE

        friend class IdentificationFileFilteringTableWidget;
    };
    
#ifdef __IDENTIFICATION_FILE_FILTERING_ROW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_FILE_FILTERING_ROW_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_FILE_FILTERING_ROW_H__
