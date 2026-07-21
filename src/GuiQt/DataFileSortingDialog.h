#ifndef __DATA_FILE_SORTING_DIALOG_H__
#define __DATA_FILE_SORTING_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include "DataFileTypeEnum.h"
#include "WuQDialogModal.h"

class QListWidget;

namespace caret {

    class EnumComboBoxTemplate;
    
    class DataFileSortingDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        DataFileSortingDialog(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                              const DataFileTypeEnum::Enum defaultDataFileType,
                              QWidget* parent = 0);
        
        virtual ~DataFileSortingDialog();
        
        DataFileSortingDialog(const DataFileSortingDialog&) = delete;

        DataFileSortingDialog& operator=(const DataFileSortingDialog&) = delete;

        DataFileTypeEnum::Enum getDataFileTypeSelected() const;

        // ADD_NEW_METHODS_HERE

    protected slots:
        void dataFileTypeSelected();
        
    protected:
        virtual void okButtonClicked();
        
    private:
        void orderOfFilesChanged();
        
        EnumComboBoxTemplate* m_dataFileTypeComboBox;
        
        QListWidget* m_dataFilesListWidget;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_SORTING_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_SORTING_DIALOG_DECLARE__

} // namespace
#endif  //__DATA_FILE_SORTING_DIALOG_H__
