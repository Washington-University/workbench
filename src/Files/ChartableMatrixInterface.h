#ifndef __CHARTABLE_MATRIX_INTERFACE_H__
#define __CHARTABLE_MATRIX_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
