#ifndef __BORDER_FILE_SPLIT_DIALOG_H__
#define __BORDER_FILE_SPLIT_DIALOG_H__

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


#include "CaretPointer.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QGridLayout;
class QLineEdit;
class QPushButton;
class QSignalMapper;

namespace caret {

    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    
    class BorderFileSplitDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BorderFileSplitDialog(QWidget* parent = 0);
        
        virtual ~BorderFileSplitDialog();
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private slots:
        void fileNameToolButtonClicked(int);
        
        void borderMultiStructureFileSelected(CaretDataFile* caretDataFile);

    private:
        BorderFileSplitDialog(const BorderFileSplitDialog&);

        BorderFileSplitDialog& operator=(const BorderFileSplitDialog&);
        
        QGridLayout* m_gridLayout;
        
        /* Use CaretPointer so that the file selection model gets destroyed */
        CaretPointer<CaretDataFileSelectionModel> m_fileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
        
        struct StructureRow {
            QPushButton* m_fileNamePushButton;
            QLineEdit* m_fileNameLineEdit;
            StructureEnum::Enum m_structure;
        };
        
        std::vector<StructureRow> m_structureRows;
        
        QSignalMapper* m_fileNameToolButtonSignalMapper;
        
        bool m_dialogIsBeingCreatedFlag;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BORDER_FILE_SPLIT_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_FILE_SPLIT_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_FILE_SPLIT_DIALOG_H__
