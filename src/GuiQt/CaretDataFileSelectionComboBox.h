#ifndef __CARET_DATA_FILE_SELECTION_COMBO_BOX_H__
#define __CARET_DATA_FILE_SELECTION_COMBO_BOX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <utility>
#include <vector>

#include "DataFileTypeEnum.h"
#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class CaretDataFile;
    class CaretDataFileSelectionModel;
    
    class CaretDataFileSelectionComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        static std::pair<CaretDataFileSelectionModel*, CaretDataFileSelectionComboBox*>
                  newInstanceForFileType(const DataFileTypeEnum::Enum dataFileType,
                                         QObject* parent);
        
        CaretDataFileSelectionComboBox(QObject* parent);
        
        virtual ~CaretDataFileSelectionComboBox();
        
        void updateComboBox(CaretDataFileSelectionModel* selectionModel);
        
        CaretDataFileSelectionModel* getSelectionModel();
        
        CaretDataFile* getSelectedFile() const;
        
        virtual QWidget* getWidget();
        
        void setSelectedFile(CaretDataFile* caretDataFile);
        
        void setNoFilesText(const AString& noFilesText);
        
        // ADD_NEW_METHODS_HERE

    signals:
        void fileSelected(CaretDataFile* caretDataFile);
        
    private slots:
        void slotFileIndexSelected(int indx);
        
    private:
        CaretDataFileSelectionComboBox(const CaretDataFileSelectionComboBox&);

        CaretDataFileSelectionComboBox& operator=(const CaretDataFileSelectionComboBox&);
        
        QComboBox* m_comboBox;
        
        CaretDataFileSelectionModel* m_selectionModel;
        
        AString m_noFilesText;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_DATA_FILE_SELECTION_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_DATA_FILE_SELECTION_COMBO_BOX_DECLARE__

} // namespace
#endif  //__CARET_DATA_FILE_SELECTION_COMBO_BOX_H__
