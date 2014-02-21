
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

#define __CHART_DATA_MATRIX_DECLARE__
#include "ChartDataMatrix.h"
#undef __CHART_DATA_MATRIX_DECLARE__

#include "CaretAssert.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartDataMatrix 
 * \brief Chart data for a matrix.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartDataMatrix::ChartDataMatrix()
: ChartData(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX)
{
    initializeMembersChartDataMatrix();
}

/**
 * Destructor.
 */
ChartDataMatrix::~ChartDataMatrix()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartDataMatrix::ChartDataMatrix(const ChartDataMatrix& obj)
: ChartData(obj)
{
    initializeMembersChartDataMatrix();
    
    this->copyHelperChartDataMatrix(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartDataMatrix&
ChartDataMatrix::operator=(const ChartDataMatrix& obj)
{
    if (this != &obj) {
        ChartData::operator=(obj);
        this->copyHelperChartDataMatrix(obj);
    }
    return *this;    
}

/**
 * At times a copy of chart data will be needed BUT it must be
 * the proper subclass so copy constructor and assignment operator
 * will no function when this abstract, base class is used.  Each
 * subclass will override this method so that the returned class
 * is of the proper type.
 *
 * @return Copy of this instance that is the actual subclass.
 */
ChartData*
ChartDataMatrix::clone() const
{
    ChartDataMatrix* cloneCopy = new ChartDataMatrix(*this);
    return cloneCopy;
}

/**
 * Initialize data.
 */
void
ChartDataMatrix::initializeMembersChartDataMatrix()
{
    clearMatrixData();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_numberOfRows",
                          &m_numberOfRows);
    m_sceneAssistant->add("m_numberOfColumns",
                          &m_numberOfColumns);
}

/**
 * Clear the matrix data.
 */
void
ChartDataMatrix::clearMatrixData()
{
    m_matrixData.clear();
    m_matrixRGBA.clear();
    m_numberOfRows    = 0;
    m_numberOfColumns = 0;
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartDataMatrix::copyHelperChartDataMatrix(const ChartDataMatrix& obj)
{
    m_numberOfColumns = obj.m_numberOfColumns;
    m_numberOfRows    = obj.m_numberOfRows;
    m_matrixData      = obj.m_matrixData;
    m_matrixRGBA      = obj.m_matrixRGBA;
}

/**
 * Set (replace) the matrix data.
 *
 * @param matrixData
 *     Pointer to the matrix data.
 * @param matrixRGBA
 *     Pointer to RGBA coloring for matrix.
 * @param numberOfRows
 *     Number of rows in matrix.
 * @param numberOfColumns
 *     Number of columns in matrix.
 */
void
ChartDataMatrix::setMatrix(const float* matrixData,
                           const float* matrixRGBA,
                           const int32_t numberOfRows,
                           const int32_t numberOfColumns)
{
    const int32_t numElements = numberOfRows * numberOfColumns;
    if (numElements > 0) {
        m_matrixData.resize(numElements);
        for (int32_t i = 0; i < numElements; i++) {
            m_matrixData[i] = matrixData[i];
        }
        
        const int32_t numElements4 = numElements * 4;
        m_matrixRGBA.resize(numElements4);
        for (int32_t i = 0; i < numElements4; i++) {
            m_matrixRGBA[i] = matrixRGBA[i];
        }
    }
    else {
        m_matrixData.clear();
        m_matrixRGBA.clear();
        m_numberOfRows    = 0;
        m_numberOfColumns = 0;
    }
}

/**
 * @return Number of rows.
 */
int32_t
ChartDataMatrix::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return Number of columns.
 */
int32_t
ChartDataMatrix::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * Get a matrix data element.
 *
 * @param row
 *     Row in the matrix.
 * @param column
 *     Column in the matrix.
 * @return 
 *     Value in matrix row and column.
 */
float
ChartDataMatrix::getMatrixElement(const int32_t row,
                                  const int32_t column) const
{
    CaretAssert((row >= 0)
                && (row < m_numberOfRows));
    CaretAssert((column >= 0)
                && (column < m_numberOfColumns));
    const int32_t offset = getMatrixOffset(row,
                                           column);
    CaretAssertVectorIndex(m_matrixData, offset);
    return m_matrixData[offset];
}

/**
 * Get a matrix data element's RGBA coloring.
 *
 * @param row
 *     Row in the matrix.
 * @param column
 *     Column in the matrix.
 * @param rgbaOut
 *     Output containing RGBA values (0 to 1.0) for matrix element.
 */
void
ChartDataMatrix::getMatrixElementRGBA(const int32_t row,
                          const int32_t column,
                          float rgbaOut[4]) const
{
    CaretAssert((row >= 0)
                && (row < m_numberOfRows));
    CaretAssert((column >= 0)
                && (column < m_numberOfColumns));
    
    const int32_t offset4 = (4
                             * getMatrixOffset(row,
                                             column));
    CaretAssertVectorIndex(m_matrixRGBA, offset4 + 3);
    rgbaOut[0] = m_matrixRGBA[offset4 + 0];
    rgbaOut[1] = m_matrixRGBA[offset4 + 1];
    rgbaOut[2] = m_matrixRGBA[offset4 + 2];
    rgbaOut[3] = m_matrixRGBA[offset4 + 3];
}

/**
 * Save subclass data to the scene.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
ChartDataMatrix::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    SceneClass* chartDataMatrix = new SceneClass("chartDataMatrix",
                                                    "ChartDataMatrix",
                                                    1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  chartDataMatrix);
    
    const int32_t numElements = m_numberOfRows * m_numberOfColumns;
    if (numElements > 0) {
        chartDataMatrix->addFloatArray("m_matrixData",
                                       &m_matrixData[0],
                                       numElements);
        
        const int32_t numElements4 = numElements * 4;
        chartDataMatrix->addFloatArray("m_matrixRGBA",
                                       &m_matrixRGBA[0],
                                       numElements4);
    }
    
    sceneClass->addClass(chartDataMatrix);
}

/**
 * Restore file data from the scene.  The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartDataMatrix::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    clearMatrixData();
    
    
    const SceneClass* chartDataMatrix = sceneClass->getClass("chartDataMatrix");
    if (chartDataMatrix == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, chartDataMatrix);
    
    const int32_t numElements = m_numberOfRows * m_numberOfColumns;
    if (numElements > 0) {
        const SceneClassArray* matrixArray = chartDataMatrix->getClassArray("m_matrixData");
        if (matrixArray != NULL) {
            const int32_t arrayNumberOfElements = matrixArray->getNumberOfArrayElements();
            if (arrayNumberOfElements == numElements) {
                m_matrixData.resize(numElements,
                                    0.0);
                chartDataMatrix->getFloatArrayValue("m_matrixData",
                                                    &m_matrixData[0],
                                                    arrayNumberOfElements);
                
                const SceneClassArray* matrixRgbaArray = chartDataMatrix->getClassArray("m_matrixRGBA");
                if (matrixRgbaArray != NULL) {
                    const int32_t numElements4 = numElements * 4;
                    const int32_t arrayNumberOfElements4 = matrixRgbaArray->getNumberOfArrayElements();
                    if (arrayNumberOfElements4 == numElements4) {
                        m_matrixRGBA.resize(numElements4,
                                            0.0);
                        chartDataMatrix->getFloatArrayValue("m_matrixRGBA",
                                                            &m_matrixRGBA[0],
                                                            arrayNumberOfElements4);
                    }
                    else {
                        clearMatrixData();
                        
                        sceneAttributes->addToErrorMessage("Need "
                                                           + AString::number(numElements4)
                                                           + " to restore matrix RGBA but only have "
                                                           + AString::number(arrayNumberOfElements4)
                                                           + " in scene data");
                    }
                }
            }
            else {
                clearMatrixData();
                
                sceneAttributes->addToErrorMessage("Need "
                                                   + AString::number(numElements)
                                                   + " to restore matrix but only have "
                                                   + AString::number(arrayNumberOfElements)
                                                   + " in scene data");
            }
        }
    }
}


