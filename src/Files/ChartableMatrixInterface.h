#ifndef __CHARTABLE_MATRIX_INTERFACE_H__
#define __CHARTABLE_MATRIX_INTERFACE_H__

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


#include "ChartableInterface.h"

namespace caret {

    class ChartableMatrixInterface : public ChartableInterface {
        
    public:
        ChartableMatrixInterface() { }
        
        virtual ~ChartableMatrixInterface() { }
        
        /**
         * Get the matrix RGBA coloring for this matrix data creator.
         *
         * @param numberOfRowsOut
         *    Number of rows in the coloring matrix.
         * @param numberOfColumnsOut
         *    Number of rows in the coloring matrix.
         * @param rgbaOut
         *    RGBA coloring output with number of elements
         *    (numberOfRowsOut * numberOfColumnsOut * 4).
         * @return
         *    True if data output data is valid, else false.
         */
        virtual bool getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                       int32_t& numberOfColumnsOut,
                                       std::vector<float>& rgbaOut) const = 0;
        
        /**
         * Get the value, row name, and column name for a cell in the matrix.
         *
         * @param rowIndex
         *     The row index.
         * @param columnIndex
         *     The column index.
         * @param cellValueOut
         *     Output containing value in the cell.
         * @param rowNameOut
         *     Name of row corresponding to row index.
         * @param columnNameOut
         *     Name of column corresponding to column index.
         * @return
         *     True if the output values are valid (valid row/column indices).
         */
        virtual bool getMatrixCellAttributes(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             float& cellValueOut,
                                             AString& rowNameOut,
                                             AString& columnNameOut) const = 0;

        // ADD_NEW_METHODS_HERE

    private:
        ChartableMatrixInterface(const ChartableMatrixInterface&);

        ChartableMatrixInterface& operator=(const ChartableMatrixInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_MATRIX_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_MATRIX_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_MATRIX_INTERFACE_H__
