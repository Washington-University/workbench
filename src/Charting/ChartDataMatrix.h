#ifndef __CHART_DATA_MATRIX_H__
#define __CHART_DATA_MATRIX_H__

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


#include "ChartData.h"



namespace caret {

    class ChartDataMatrix : public ChartData {
        
    public:
        ChartDataMatrix();
        
        virtual ~ChartDataMatrix();
        
        ChartDataMatrix(const ChartDataMatrix& obj);

        ChartDataMatrix& operator=(const ChartDataMatrix& obj);
        
        virtual ChartData* clone() const;
        
        void setMatrix(const float* matrixData,
                       const float* matrixRGBA,
                       const int32_t numberOfRows,
                       const int32_t numberOfColumns);
        
        int32_t getNumberOfRows() const;
        
        int32_t getNumberOfColumns() const;
        
        float getMatrixElement(const int32_t row,
                               const int32_t column) const;
        
        void getMatrixElementRGBA(const int32_t row,
                                  const int32_t column,
                                  float rgbaOut[4]) const;
        
        // ADD_NEW_METHODS_HERE

        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
    private:
        void copyHelperChartDataMatrix(const ChartDataMatrix& obj);

        void initializeMembersChartDataMatrix();
        
        void clearMatrixData();
        
        /**
         * Get offset of a matrix data element.
         *
         * @param row
         *     Row in the matrix.
         * @param column
         *     Column in the matrix.
         * @return 
         *     Offset of a matrix element.
         */
        inline int32_t getMatrixOffset(const int32_t row,
                                       const int32_t column) const {
            const int32_t offset = (row * m_numberOfColumns) + column;
            return offset;
        }
        
        SceneClassAssistant* m_sceneAssistant;
        
        int32_t m_numberOfRows;
        
        int32_t m_numberOfColumns;
        
        std::vector<float> m_matrixData;
        
        std::vector<float> m_matrixRGBA;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_DATA_MATRIX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_DATA_MATRIX_DECLARE__

} // namespace
#endif  //__CHART_DATA_MATRIX_H__
