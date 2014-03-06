
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
#include "ChartDataMatrixCreatorInterface.h"
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
ChartDataMatrix::ChartDataMatrix(ChartDataMatrixCreatorInterface* matrixCreatorInterface)
: ChartData(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX),
m_matrixCreatorInterface(matrixCreatorInterface)
{
    CaretAssert(matrixCreatorInterface);
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
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartDataMatrix::copyHelperChartDataMatrix(const ChartDataMatrix& obj)
{
    m_matrixCreatorInterface = obj.m_matrixCreatorInterface;
}

/**
 * Return the object that created this chart matrix.
 */
ChartDataMatrixCreatorInterface*
ChartDataMatrix::getParentMatrixCreator()
{
    return m_matrixCreatorInterface;
}


/**
 * Get the matrix RGBA coloring for this matrix data.
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
bool
ChartDataMatrix::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                               int32_t& numberOfColumnsOut,
                               std::vector<float>& rgbaOut) const
{
    CaretAssert(m_matrixCreatorInterface);
    
    return m_matrixCreatorInterface->getMatrixDataRGBA(numberOfRowsOut,
                                                       numberOfColumnsOut,
                                                       rgbaOut);
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
    const SceneClass* chartDataMatrix = sceneClass->getClass("chartDataMatrix");
    if (chartDataMatrix == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, chartDataMatrix);
    
}


