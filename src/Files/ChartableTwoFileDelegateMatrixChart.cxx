
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __CHARTABLE_TWO_FILE_DELEGATE_MATRIX_CHART_DECLARE__
#include "ChartableTwoFileDelegateMatrixChart.h"
#undef __CHARTABLE_TWO_FILE_DELEGATE_MATRIX_CHART_DECLARE__

#include "CaretAssert.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelScalarFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableTwoFileDelegateMatrixChart 
 * \brief Implementation of base chart delegate for matrix charts.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param matrixContentType
 *     Content type of the matrix.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 */
ChartableTwoFileDelegateMatrixChart::ChartableTwoFileDelegateMatrixChart(const ChartTwoMatrixContentTypeEnum::Enum matrixContentType,
                                                                         CaretMappableDataFile* parentCaretMappableDataFile)
: ChartableTwoFileDelegateBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX,
                                    parentCaretMappableDataFile),
m_matrixContentType(matrixContentType)
{
    CaretAssert(m_matrixContentType != ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED);
    m_sceneAssistant = new SceneClassAssistant();
    
}

/**
 * Destructor.
 */
ChartableTwoFileDelegateMatrixChart::~ChartableTwoFileDelegateMatrixChart()
{
    delete m_sceneAssistant;
}

/**
 * @return Content type of the matrix.
 */
ChartTwoMatrixContentTypeEnum::Enum
ChartableTwoFileDelegateMatrixChart::getMatrixContentType() const
{
    return m_matrixContentType;
}

/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Number of rows in the matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the matrix.
 */
void
ChartableTwoFileDelegateMatrixChart::getMatrixDimensions(int32_t& numberOfRowsOut,
                         int32_t& numberOfColumnsOut) const
{
    numberOfRowsOut = 0;
    numberOfColumnsOut = 0;
    
    if (m_matrixContentType == ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
        return;
    }
    
    const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    CaretAssert(ciftiMapFile);
    ciftiMapFile->helpMapFileGetMatrixDimensions(numberOfRowsOut,
                                                 numberOfColumnsOut);
}

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
bool
ChartableTwoFileDelegateMatrixChart::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                       int32_t& numberOfColumnsOut,
                       std::vector<float>& rgbaOut) const
{
    const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    CaretAssert(ciftiMapFile);
    
    bool useMapFileHelperFlag = false;
    bool useMatrixFileHelperFlag = false;
    
    std::vector<int32_t> parcelReorderedRowIndices;
    
    switch (ciftiMapFile->getDataFileType()) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
        {
            useMatrixFileHelperFlag    = true;
            
            const CiftiConnectivityMatrixParcelFile* parcelConnFile = dynamic_cast<const CiftiConnectivityMatrixParcelFile*>(ciftiMapFile);
            CaretAssert(parcelConnFile);
            if (parcelConnFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelConnFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                              parcelLabelReorderingFile,
                                                                              parcelLabelFileMapIndex,
                                                                              reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelConnFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                        parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
        {
            useMapFileHelperFlag = true;
            
            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
            CaretAssert(parcelLabelFile);
            if (parcelLabelFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelLabelFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                               parcelLabelReorderingFile,
                                                                               parcelLabelFileMapIndex,
                                                                               reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelLabelFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                         parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
        {
            useMapFileHelperFlag = true;
            
            const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(ciftiMapFile);
            CaretAssert(parcelScalarFile);
            if (parcelScalarFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelScalarFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                                parcelLabelReorderingFile,
                                                                                parcelLabelFileMapIndex,
                                                                                reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelScalarFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                          parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            useMatrixFileHelperFlag = true;
            break;
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
    }
    
    if (( ! useMapFileHelperFlag)
        && ( ! useMatrixFileHelperFlag)) {
        CaretAssertMessage(0, "Trying to get matrix from a file that does not support matrix display");
        return false;
    }
    
    bool validDataFlag = false;
    if (useMapFileHelperFlag) {
        validDataFlag = ciftiMapFile->helpMapFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                                                          numberOfColumnsOut,
                                                                                          parcelReorderedRowIndices,
                                                                                          rgbaOut);
    }
    else if (useMatrixFileHelperFlag) {
        validDataFlag = ciftiMapFile->helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                                                             numberOfColumnsOut,
                                                                                             parcelReorderedRowIndices,
                                                                                             rgbaOut);
    }
    switch (m_matrixContentType) {
        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED:
            break;
        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE:
            break;
        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SERIES:
            break;
    }
    
    return validDataFlag;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ChartableTwoFileDelegateMatrixChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
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
ChartableTwoFileDelegateMatrixChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

