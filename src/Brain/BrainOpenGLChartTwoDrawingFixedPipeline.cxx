
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
#include "ChartMatrixScaleModeEnum.h"
#include "ChartOverlay.h"
#include "ChartOverlaySet.h"
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
                                                             ChartOverlaySet* chartOverlaySet,
                                                             const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                             const int32_t viewport[4],
                                                             const int32_t tabIndex)
{
    m_brain = brain;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_textRenderer = textRenderer;
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
    
    bool applyTransformationsFlag = false;
    float panningXY[2] = { 0.0, 0.0 };
    float zooming = 1.0;
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
        glTranslatef(panningXY[0],
                     panningXY[1],
                     0.0);
        
        const float chartWidth  = cellWidth  * numberOfCols;
        const float chartHeight = cellHeight * numberOfRows;
        const float halfWidth   = chartWidth  / 2.0;
        const float halfHeight  = chartHeight / 2.0;
        glTranslatef(halfWidth,
                     halfHeight,
                     0.0);
        glScalef(zooming,
                 zooming,
                 1.0);
        glTranslatef(-halfWidth,
                     -halfHeight,
                     0.0);
    }

    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        const ChartOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
        if (chartOverlay->isEnabled()) {
            const ChartTwoCompoundDataType cdt = chartOverlay->getChartTwoCompoundDataType();
            CaretAssert(cdt.getChartDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
            const int32_t numberOfRows = cdt.getMatrixNumberOfRows();
            const int32_t numberOfCols = cdt.getMatrixNumberOfColumns();
            if ((numberOfRows > 0)
                && (numberOfCols > 0)) {
                /*
                 * Auto scale 'fills' the matrix region
                 * and updates the width and height in the
                 * matrix properties for use in manual mode.
                 * There is NO zooming or panning for Auto scale.
                 */
                cellWidth  = graphicsWidth  / numberOfCols;
                cellHeight = graphicsHeight / numberOfRows;
            }
        }
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
//        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartableMatrixSelectionTypeForIdentification,
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
