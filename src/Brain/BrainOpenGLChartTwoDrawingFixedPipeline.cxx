
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

#define __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLChartTwoDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartMatrixScaleModeEnum.h"
#include "ChartOverlay.h"
#include "ChartOverlaySet.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
//#include "CiftiParcelLabelFile.h"
//#include "CiftiParcelScalarFile.h"
//#include "CiftiScalarDataSeriesFile.h"
//#include "ConnectivityDataLoaded.h"
//#include "EventCaretMappableDataFileMapsViewedInOverlays.h"
//#include "EventManager.h"

#include "IdentificationWithColor.h"
#include "SessionManager.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLChartTwoDrawingFixedPipeline 
 * \brief Drawing of version two charts.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::BrainOpenGLChartTwoDrawingFixedPipeline()
: BrainOpenGLChartTwoDrawingInterface()
{
    m_preferences = SessionManager::get()->getCaretPreferences();
}

/**
 * Destructor.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::~BrainOpenGLChartTwoDrawingFixedPipeline()
{
}

/**
 * Draw charts from a chart overlay.
 *
 * @param brain
 *     Brain.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param textRenderer
 *     Text rendering.
 * @param translation
 *     The chart's translation.
 * @param zooming
 *     The chart's zooming.
 * @param chartOverlaySet
 *     Chart overlay set that is drawn.
 * @param selectionItemDataType
 *     Selected data type.
 * @param viewport
 *     Viewport for the chart.
 * @param tabIndex
 *     Index of the tab.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartOverlaySet(Brain* brain,
                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                             BrainOpenGLTextRenderInterface* textRenderer,
                                                             const float translation[3],
                                                             const float zooming,
                                                             ChartOverlaySet* chartOverlaySet,
                                                             const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                             const int32_t viewport[4],
                                                             const int32_t tabIndex)
{
    m_brain = brain;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_textRenderer = textRenderer;
    m_translation[0] = translation[0];
    m_translation[1] = translation[1];
    m_translation[2] = translation[2];
    m_zooming        = zooming;
    m_chartOverlaySet = chartOverlaySet;
    m_selectionItemDataType = selectionItemDataType;
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
    m_tabIndex = tabIndex;


    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            CaretAssertToDoWarning();
            return;
//            if (chartDataSeriesID->isEnabledForSelection()
//                || chartFrequencySeriesID->isEnabledForSelection()
//                || chartTimeSeriesID->isEnabledForSelection()) {
//                m_identificationModeFlag = true;
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            }
//            else {
//                return;
//            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    saveStateOfOpenGL();
    
    resetIdentification();
    
    const int32_t numberOfOverlays = chartOverlaySet->getNumberOfDisplayedOverlays();
    if (numberOfOverlays > 0) {
        ChartOverlay* topOverlay = chartOverlaySet->getOverlay(0);
        if (topOverlay->isEnabled()) {
            CaretMappableDataFile* cmdf = NULL;
            int32_t mapIndex = -1;
            topOverlay->getSelectionData(cmdf, mapIndex);
            if (cmdf != NULL) {
                const ChartTwoDataTypeEnum::Enum chartDataType = topOverlay->getChartDataType();
                switch (chartDataType) {
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        drawMatrixChart();
                        break;
                }
            }
        }
        for (int32_t iOverlay = (numberOfOverlays -1); iOverlay >= 0; iOverlay--) {
            
        }
    }
    
    if (m_identificationModeFlag) {
        processIdentification();
    }
    
    restoreStateOfOpenGL();
}

/**
 * Draw a matrix chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawMatrixChart()
{
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);
    
    bool applyTransformationsFlag = true;
//    float panningXY[2] = { 0.0, 0.0 };
//    float zooming = 1.0;
    float cellWidth = 1.0;
    float cellHeight = 1.0;
    
    /*
     * Setup width/height of area in which matrix is drawn with a
     * small margin along all of the edges
     */
    float margin = 10.0;
    if ((m_viewport[2] < (margin * 3.0))
        || (m_viewport[3] < (margin * 3.0))) {
        margin = 0.0;
    }
    const float graphicsWidth  = m_viewport[2] - (margin * 2.0);
    const float graphicsHeight = m_viewport[3] - (margin * 2.0);

    /*
     * First overlay is ALWAYS ON and since all matrices must have 
     * same number of rows/columns, use first matrix for rows/columns
     */
    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
    CaretAssert(numberOfOverlays > 0);
    const ChartOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
    const ChartTwoCompoundDataType cdt = topOverlay->getChartTwoCompoundDataType();
    CaretAssert(cdt.getChartDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
    const int32_t numberOfRows = cdt.getMatrixNumberOfRows();
    const int32_t numberOfCols = cdt.getMatrixNumberOfColumns();
    if ((numberOfRows > 0)
        && (numberOfCols > 0)) {
        cellWidth  = graphicsWidth  / numberOfCols;
        cellHeight = graphicsHeight / numberOfRows;
    }
    else {
        return;
    }
    
    /*
     * Set the width and neight of each matrix cell.
     */
    //ChartMatrixDisplayProperties* matrixProperties = chartMatrixInterface->getChartMatrixDisplayProperties(m_tabIndex);
    //CaretAssert(matrixProperties);
    const ChartMatrixScaleModeEnum::Enum scaleMode = ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO; // matrixProperties->getScaleMode();
    switch (scaleMode) {
        case ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO:
            /*
             * Auto scale 'fills' the matrix region
             * and updates the width and height in the
             * matrix properties for use in manual mode.
             * There is NO zooming or panning for Auto scale.
             */
            //cellWidth  = graphicsWidth  / numberOfCols;
            //cellHeight = graphicsHeight / numberOfRows;
            
            //matrixProperties->setCellWidth(cellWidth);
            //matrixProperties->setCellHeight(cellHeight);
            break;
        case ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL:
            /*
             * Use the cell width and height for manual mode
             * and allow both panning and zooming.
             */
            //cellWidth = matrixProperties->getCellWidth();
            //cellHeight = matrixProperties->getCellHeight();
            
            //matrixProperties->getViewPanning(panningXY);
            //zooming = matrixProperties->getViewZooming();
            //applyTransformationsFlag = true;
            break;
    }
    /*
     * Set the coordinates for the area in which the matrix is drawn.
     */
    const float xMin = -margin;
    const float xMax = graphicsWidth + margin;
    const float yMin = -margin;
    const float yMax = graphicsHeight + margin;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin, xMax,
            yMin, yMax,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (applyTransformationsFlag) {
        glTranslatef(m_translation[0],
                     m_translation[1],
                     0.0);
        
        const float chartWidth  = cellWidth  * numberOfCols;
        const float chartHeight = cellHeight * numberOfRows;
        const float halfWidth   = chartWidth  / 2.0;
        const float halfHeight  = chartHeight / 2.0;
        glTranslatef(halfWidth,
                     halfHeight,
                     0.0);
        glScalef(m_zooming,
                 m_zooming,
                 1.0);
        glTranslatef(-halfWidth,
                     -halfHeight,
                     0.0);
    }

    /*
     * For testing
     */
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(xMin, yMin);
    glVertex2f(xMax, yMax);
    glEnd();
    
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        ChartOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
        if ( ! chartOverlay->isEnabled()) {
            continue;
        }
        
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = 1;
        chartOverlay->getSelectionData(mapFile,
                                       mapIndex);
        if (mapFile == NULL) {
            continue;
        }
        
        const ChartableTwoFileMatrixChart* matrixChart = mapFile->getChartingDelegate()->getMatrixCharting();
        int32_t overlayRows = 0;
        int32_t overlayColumns = 0;
        matrixChart->getMatrixDimensions(overlayRows,
                                         overlayColumns);
        
        /*
         * All matrices must have same rows/columns
         */
        if ((overlayRows == numberOfRows)
            && (overlayColumns == numberOfCols)) {
            drawMatrixChartContent(matrixChart,
                                ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL,
                                //ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL,
                                //ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL,
                                //ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL,
                                cellWidth,
                                cellHeight,
                                m_zooming);
        }
    }
}

///**
// * Get highlighting of rows or columns for a matrix.
// *
// * @param matrixChart
// *     Matrix chart that is drawn.
// * @param rowIndicesOut
// *     Output with row indices for highlighting.
// * @param columnIndicesOut
// *     Output with column indices for highlighting.
// */
//void
//BrainOpenGLChartTwoDrawingFixedPipeline::getMatrixHighlighting(const ChartableTwoFileMatrixChart* matrixChart,
//                                                               std::set<int32_t>& rowIndicesOut,
//                                                               std::set<int32_t>& columnIndicesOut) const
//{
//    const CiftiMappableDataFile* ciftiMapFile = matrixChart->getCiftiMappableDataFile();
//    const CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(ciftiMapFile);
//    if (connMapFile != NULL) {
//        const ConnectivityDataLoaded* connDataLoaded = connMapFile->getConnectivityDataLoaded();
//        if (connDataLoaded != NULL) {
//            int64_t loadedRowIndex = -1;
//            int64_t loadedColumnIndex = -1;
//            connDataLoaded->getRowColumnLoading(loadedRowIndex,
//                                                loadedColumnIndex);
//            if (loadedRowIndex >= 0) {
//                rowIndicesOut.insert(loadedRowIndex);
//            }
//            else if (loadedColumnIndex >= 0) {
//                columnIndicesOut.insert(loadedColumnIndex);
//            }
//        }
//    }
//    
//    const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(ciftiMapFile);
//    if (parcelScalarFile != NULL) {
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelScalarFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        columnIndicesOut = mapOverlayEvent.getSelectedMapIndices();
//    }
//    
//    const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
//    if (parcelLabelFile != NULL) {
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelLabelFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        columnIndicesOut = mapOverlayEvent.getSelectedMapIndices();
//    }
//    
//    const CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<const CiftiScalarDataSeriesFile*>(ciftiMapFile);
//    if (scalarDataSeriesFile != NULL) {
//        const int32_t scalarDataSeriesMapIndex = scalarDataSeriesFile->getSelectedMapIndex(m_tabIndex);
//        if (scalarDataSeriesMapIndex >= 0) {
//            rowIndicesOut.insert(scalarDataSeriesMapIndex);
//        }
//    }
//}

/*
 * Draw a matrix chart.
 *
 * @param matrixChart
 *     Matrix chart that is drawn.
 * @param chartViewingType
 *     Type of chart viewing.
 * @param cellWidth
 *     Width of cell.
 * @param cellHeight
 *     Height of cell.
 * @param zooming
 *     Current zooming.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
                                                             const ChartTwoMatrixViewingTypeEnum::Enum chartViewingType,
                                                             const float cellWidth,
                                                             const float cellHeight,
                                                             const float zooming)
{
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    std::vector<float> matrixRGBA;
    if ( ! matrixChart->getMatrixDataRGBA(numberOfRows,
                                       numberOfColumns,
                                    matrixRGBA)) {
        CaretLogWarning("Matrix RGBA invalid");
        return;
    }
    
    CaretAssertToDoWarning(); // need to set these
    bool displayGridLinesFlag = false;
    const bool highlightSelectedRowColumnFlag = true;
    
    
    std::vector<int32_t> selectedColumnIndices;
    std::vector<int32_t> selectedRowIndices;
    ChartTwoMatrixLoadingDimensionEnum::Enum selectedRowColumnDimension;
    matrixChart->getSelectedRowColumnIndices(m_tabIndex,
                                             selectedRowColumnDimension,
                                             selectedRowIndices,
                                             selectedColumnIndices);
//    getMatrixHighlighting(matrixChart
//                          selectedRowIndices,
//                          selectedColumnIndices);
    
    uint8_t highlightRGBByte[3];
    m_preferences->getBackgroundAndForegroundColors()->getColorForegroundChartView(highlightRGBByte);
    const float highlightRGB[3] = {
        highlightRGBByte[0] / 255.0,
        highlightRGBByte[1] / 255.0,
        highlightRGBByte[2] / 255.0
    };

    int32_t rgbaOffset = 0;
    std::vector<float> quadVerticesXYZ;
    quadVerticesXYZ.reserve(numberOfRows * numberOfColumns * 3);
    std::vector<float> quadVerticesFloatRGBA;
    quadVerticesFloatRGBA.reserve(numberOfRows * numberOfColumns * 4);
    std::vector<uint8_t> quadVerticesByteRGBA;
    quadVerticesByteRGBA.reserve(numberOfRows * numberOfColumns * 4);
    
    float cellY = (numberOfRows - 1) * cellHeight;
    for (int32_t rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
        float cellX = 0;
        for (int32_t columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
            CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
            const float* rgba = &matrixRGBA[rgbaOffset];
            rgbaOffset += 4;
            
            bool drawCellFlag = true;
            if (chartViewingType != ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL) {
                if (numberOfRows == numberOfColumns) {
                    drawCellFlag = false;
                    switch (chartViewingType) {
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL:
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                            if (rowIndex != columnIndex) {
                                drawCellFlag = true;
                            }
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                            if (rowIndex > columnIndex) {
                                drawCellFlag = true;
                            }
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                            if (rowIndex < columnIndex) {
                                drawCellFlag = true;
                            }
                            break;
                    }
                }
                else {
                    drawCellFlag = true;
                    switch (chartViewingType) {
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL:
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                            break;
                        case ChartTwoMatrixViewingTypeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                            break;
                    }
                }
            }
            
            if (drawCellFlag) {
                uint8_t idRGBA[4];
                if (m_identificationModeFlag) {
                    addToChartMatrixIdentification(rowIndex,
                                                   columnIndex,
                                                   idRGBA);
                }
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX + cellWidth);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX + cellWidth);
                quadVerticesXYZ.push_back(cellY + cellHeight);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY + cellHeight);
                quadVerticesXYZ.push_back(0.0);
            }
            
            cellX += cellWidth;
        }
        
        cellY -= cellHeight;
    }
    
    /*
     * Draw the matrix elements.
     */
    if (m_identificationModeFlag) {
        CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
        const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < numberQuadVertices; i++) {
            CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
            glColor4ubv(&quadVerticesByteRGBA[i*4]);
            CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
            glVertex3fv(&quadVerticesXYZ[i*3]);
        }
        glEnd();
    }
    else {
        /*
         * Enable alpha blending so voxels that are not drawn from higher layers
         * allow voxels from lower layers to be seen.
         */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
        const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < numberQuadVertices; i++) {
            CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
            glColor4fv(&quadVerticesFloatRGBA[i*4]);
            CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
            glVertex3fv(&quadVerticesXYZ[i*3]);
        }
        glEnd();
        
        glDisable(GL_BLEND);
        
        
        /*
         * Drawn an outline around the matrix elements.
         */
        if (displayGridLinesFlag) {
            uint8_t gridLineColorBytes[3];
            m_preferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridLineColorBytes);
            float gridLineColorFloats[4];
            CaretPreferences::byteRgbToFloatRgb(gridLineColorBytes,
                                                gridLineColorFloats);
            gridLineColorFloats[3] = 1.0;
            std::vector<float> outlineRGBA;
            outlineRGBA.reserve(numberQuadVertices * 4);
            for (int32_t i = 0; i < numberQuadVertices; i++) {
                outlineRGBA.push_back(gridLineColorFloats[0]);
                outlineRGBA.push_back(gridLineColorFloats[1]);
                outlineRGBA.push_back(gridLineColorFloats[2]);
                outlineRGBA.push_back(gridLineColorFloats[3]);
            }
            
            glPolygonMode(GL_FRONT, GL_LINE);
            glLineWidth(1.0);
            glBegin(GL_QUADS);
            for (int32_t i = 0; i < numberQuadVertices; i++) {
                CaretAssertVectorIndex(outlineRGBA, i*4 + 3);
                glColor4fv(&outlineRGBA[i*4]);
                CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                glVertex3fv(&quadVerticesXYZ[i*3]);
            }
            glEnd();
        }
        
        if ( (! selectedRowIndices.empty())
            && highlightSelectedRowColumnFlag) {
            std::vector<float> rowXYZ;
            std::vector<float> rowRGBA;
            
            for (auto rowIndex : selectedRowIndices) {
                const float rowY = (numberOfRows - rowIndex - 1) * cellHeight;
                
                
                rowXYZ.push_back(0.0);
                rowXYZ.push_back(rowY);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                rowXYZ.push_back(0.0);
                rowXYZ.push_back(rowY + cellHeight);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                rowXYZ.push_back(numberOfColumns * cellWidth);
                rowXYZ.push_back(rowY + cellHeight);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                
                rowXYZ.push_back(numberOfColumns * cellWidth);
                rowXYZ.push_back(rowY);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
            }
            
            CaretAssert((rowXYZ.size() / 3) == (rowRGBA.size() / 4));
            
            const int32_t numberOfVertices = static_cast<int32_t>(rowXYZ.size() / 3);
            const int32_t numberOfQuads = numberOfVertices / 4;
            CaretAssert((numberOfQuads * 4) == numberOfVertices);
            
            /*
             * As cells get larger, increase linewidth for selected row
             */
            const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
            glLineWidth(highlightLineWidth);
            
            for (int32_t iQuad = 0; iQuad < numberOfQuads; iQuad++) {
                glBegin(GL_LINE_LOOP);
                for (int32_t iVert = 0; iVert < 4; iVert++) {
                    const int32_t rgbaOffset = (iQuad * 16) + (iVert * 4);
                    CaretAssertVectorIndex(rowRGBA, rgbaOffset + 3);
                    glColor4fv(&rowRGBA[rgbaOffset]);
                    
                    const int32_t xyzOffset = (iQuad * 12) + (iVert * 3);
                    CaretAssertVectorIndex(rowXYZ, xyzOffset + 2);
                    glVertex3fv(&rowXYZ[xyzOffset]);
                }
                glEnd();
            }
            glLineWidth(1.0);
        }
        
        if ( (! selectedColumnIndices.empty())
            && highlightSelectedRowColumnFlag) {
            std::vector<float> columnXYZ;
            std::vector<float> columnRGBA;
            
            for (auto columnIndex : selectedColumnIndices) {
                const float colX = columnIndex * cellWidth;
                
                columnXYZ.push_back(colX);
                columnXYZ.push_back(0.0);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                columnXYZ.push_back(colX + cellWidth);
                columnXYZ.push_back(0.0);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                columnXYZ.push_back(colX + cellWidth);
                columnXYZ.push_back(numberOfRows * cellHeight);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                
                columnXYZ.push_back(colX);
                columnXYZ.push_back(numberOfRows * cellHeight);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
            }
            
            CaretAssert((columnXYZ.size() / 3) == (columnRGBA.size() / 4));
            
            const int32_t numberOfVertices = static_cast<int32_t>(columnXYZ.size() / 3);
            const int32_t numberOfQuads = numberOfVertices / 4;
            CaretAssert((numberOfQuads * 4) == numberOfVertices);
            
            /*
             * As cells get larger, increase linewidth for selected row
             */
            const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
            glLineWidth(highlightLineWidth);
            
            for (int32_t iQuad = 0; iQuad < numberOfQuads; iQuad++) {
                glBegin(GL_LINE_LOOP);
                for (int32_t iVert = 0; iVert < 4; iVert++) {
                    const int32_t rgbaOffset = (iQuad * 16) + (iVert * 4);
                    CaretAssertVectorIndex(columnRGBA, rgbaOffset + 3);
                    glColor4fv(&columnRGBA[rgbaOffset]);
                    
                    const int32_t xyzOffset = (iQuad * 12) + (iVert * 3);
                    CaretAssertVectorIndex(columnXYZ, xyzOffset + 2);
                    glVertex3fv(&columnXYZ[xyzOffset]);
                }
                glEnd();
            }
            glLineWidth(1.0);
        }
        
        glPolygonMode(GL_FRONT, GL_FILL);
    }
}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::saveStateOfOpenGL()
{
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

/**
 * Add an item for matrix identification.
 *
 * @param matrixRowIndex
 *     Index of the row
 * @param matrixColumnIndex
 *     Index of the column
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::addToChartMatrixIdentification(const int32_t matrixRowIndex,
                                                                     const int32_t matrixColumnIndex,
                                                                     uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         m_selectionItemDataType,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(matrixRowIndex);
    m_identificationIndices.push_back(matrixColumnIndex);
}

/**
 * Restore the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_restore_gl_state().
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::restoreStateOfOpenGL()
{
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    glPopClientAttrib();
    
}

/**
 * Reset identification.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::resetIdentification()
{
    m_identificationIndices.clear();
    
    if (m_identificationModeFlag) {
        const int32_t estimatedNumberOfItems = 1000;
        m_identificationIndices.reserve(estimatedNumberOfItems);
    }
}

/**
 * Process identification.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::processIdentification()
{
    CaretAssertToDoWarning();
    
//    int32_t identifiedItemIndex;
//    float depth = -1.0;
//    
//    if (m_chartModelDataSeriesBeingDrawnForIdentification != NULL) {
//        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
//                                                           m_fixedPipelineDrawing->mouseX,
//                                                           m_fixedPipelineDrawing->mouseY,
//                                                           identifiedItemIndex,
//                                                           depth);
//        if (identifiedItemIndex >= 0) {
//            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
//            const int32_t chartDataIndex = m_identificationIndices[idIndex];
//            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
//            
//            SelectionItemChartDataSeries* chartDataSeriesID = m_brain->getSelectionManager()->getChartDataSeriesIdentification();
//            if (chartDataSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
//                ChartDataCartesian* chartDataCartesian =
//                dynamic_cast<ChartDataCartesian*>(m_chartModelDataSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
//                CaretAssert(chartDataCartesian);
//                chartDataSeriesID->setChart(m_chartModelDataSeriesBeingDrawnForIdentification,
//                                            chartDataCartesian,
//                                            chartLineIndex);
//                
//                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
//                const float lineXYZ[3] = {
//                    chartPoint->getX(),
//                    chartPoint->getY(),
//                    0.0
//                };
//                
//                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartDataSeriesID,
//                                                                 lineXYZ);
//            }
//        }
//    }
//    else if (m_chartModelFrequencySeriesBeingDrawnForIdentification != NULL) {
//        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
//                                                           m_fixedPipelineDrawing->mouseX,
//                                                           m_fixedPipelineDrawing->mouseY,
//                                                           identifiedItemIndex,
//                                                           depth);
//        if (identifiedItemIndex >= 0) {
//            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
//            const int32_t chartDataIndex = m_identificationIndices[idIndex];
//            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
//            
//            SelectionItemChartFrequencySeries* chartFrequencySeriesID = m_brain->getSelectionManager()->getChartFrequencySeriesIdentification();
//            if (chartFrequencySeriesID->isOtherScreenDepthCloserToViewer(depth)) {
//                ChartDataCartesian* chartDataCartesian =
//                dynamic_cast<ChartDataCartesian*>(m_chartModelFrequencySeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
//                CaretAssert(chartDataCartesian);
//                chartFrequencySeriesID->setChart(m_chartModelFrequencySeriesBeingDrawnForIdentification,
//                                                 chartDataCartesian,
//                                                 chartLineIndex);
//                
//                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
//                const float lineXYZ[3] = {
//                    chartPoint->getX(),
//                    chartPoint->getY(),
//                    0.0
//                };
//                
//                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartFrequencySeriesID,
//                                                                 lineXYZ);
//            }
//        }
//    }
//    else if (m_chartModelTimeSeriesBeingDrawnForIdentification != NULL) {
//        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
//                                                           m_fixedPipelineDrawing->mouseX,
//                                                           m_fixedPipelineDrawing->mouseY,
//                                                           identifiedItemIndex,
//                                                           depth);
//        if (identifiedItemIndex >= 0) {
//            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
//            const int32_t chartDataIndex = m_identificationIndices[idIndex];
//            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
//            
//            SelectionItemChartTimeSeries* chartTimeSeriesID = m_brain->getSelectionManager()->getChartTimeSeriesIdentification();
//            if (chartTimeSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
//                ChartDataCartesian* chartDataCartesian =
//                dynamic_cast<ChartDataCartesian*>(m_chartModelTimeSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
//                CaretAssert(chartDataCartesian);
//                chartTimeSeriesID->setChart(m_chartModelTimeSeriesBeingDrawnForIdentification,
//                                            chartDataCartesian,
//                                            chartLineIndex);
//                
//                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
//                const float lineXYZ[3] = {
//                    chartPoint->getX(),
//                    chartPoint->getY(),
//                    0.0
//                };
//                
//                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartTimeSeriesID,
//                                                                 lineXYZ);
//            }
//        }
//    }
//    else if (m_chartableMatrixInterfaceBeingDrawnForIdentification != NULL) {
//        m_fixedPipelineDrawing->getIndexFromColorSelection(m_selectionItemDataType,
//                                                           m_fixedPipelineDrawing->mouseX,
//                                                           m_fixedPipelineDrawing->mouseY,
//                                                           identifiedItemIndex,
//                                                           depth);
//        if (identifiedItemIndex >= 0) {
//            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
//            const int32_t rowIndex = m_identificationIndices[idIndex];
//            const int32_t columnIndex = m_identificationIndices[idIndex + 1];
//            
//            SelectionItemChartMatrix* chartMatrixID = m_brain->getSelectionManager()->getChartMatrixIdentification();
//            if (chartMatrixID->isOtherScreenDepthCloserToViewer(depth)) {
//                chartMatrixID->setChartMatrix(m_chartableMatrixInterfaceBeingDrawnForIdentification,
//                                              rowIndex,
//                                              columnIndex);
//            }
//        }
//    }
}
