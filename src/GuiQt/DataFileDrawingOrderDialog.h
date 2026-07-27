#ifndef __DATA_FILE_DRAWING_ORDER_DIALOG_H__
#define __DATA_FILE_DRAWING_ORDER_DIALOG_H__

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
    
    class DataFileDrawingOrderDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        DataFileDrawingOrderDialog(const DataFileTypeEnum::Enum dataFileType,
                                   QWidget* parent = 0);
        
        virtual ~DataFileDrawingOrderDialog();
        
        DataFileDrawingOrderDialog(const DataFileDrawingOrderDialog&) = delete;

        DataFileDrawingOrderDialog& operator=(const DataFileDrawingOrderDialog&) = delete;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private:
        const DataFileTypeEnum::Enum m_dataFileType;
        
        void dataFileTypeSelected();
        
        void orderOfFilesChanged();
        
        QListWidget* m_dataFilesListWidget;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__

} // namespace
#endif  //__DATA_FILE_DRAWING_ORDER_DIALOG_H__
