#ifndef __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_H__
#define __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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


#include "SelectionItem.h"



namespace caret {
    class CiftiMappableConnectivityMatrixDataFile;
    
    class SelectionItemCiftiConnectivityMatrixRowColumn : public SelectionItem {
        
    public:
        SelectionItemCiftiConnectivityMatrixRowColumn();
        
        virtual ~SelectionItemCiftiConnectivityMatrixRowColumn();
        
        SelectionItemCiftiConnectivityMatrixRowColumn(const SelectionItemCiftiConnectivityMatrixRowColumn& obj);
        
        SelectionItemCiftiConnectivityMatrixRowColumn& operator=(const SelectionItemCiftiConnectivityMatrixRowColumn& obj);
        
        virtual bool isValid() const;
        
        virtual void reset();
        
        CiftiMappableConnectivityMatrixDataFile* getCiftiConnectivityMatrixFile() const;
        
        int32_t getMatrixRowIndex() const;
        
        int32_t getMatrixColumnIndex() const;
        
        void setFileRow(CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                        const int32_t matrixRowIndex);
        
        void setFileColumn(CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                           const int32_t matrixColumnIndex);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void copyHelperSelectionItemCiftiConnectivityMatrixRowColumn(const SelectionItemCiftiConnectivityMatrixRowColumn& obj);
        
        CiftiMappableConnectivityMatrixDataFile* m_ciftiConnectivityMatrixFile;
        
        int32_t m_matrixRowIndex;
        
        int32_t m_matrixColumnIndex;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_DECLARE__
    
} // namespace
#endif  //__SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_H__
