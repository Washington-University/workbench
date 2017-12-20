
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

#define __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
#include "BrainOpenGLViewportContent.h"
#undef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GapsAndMargins.h"
#include "MathFunctions.h"
#include "ModelSurfaceMontage.h"
#include "SurfaceMontageConfigurationAbstract.h"
#include "TileTabsConfiguration.h"

using namespace caret;


    
/**
 * \class BrainOpenGLViewportContent 
 * \brief Dimensions and model for a viewport in the graphics window.
 *
 * Dimensions and model for a viewport in the graphics window.
 */


/**
 * Constructor.
 *
 * @param windowViewport
 *    Viewport of WINDOW in which drawing takes place.
 * @param tabViewport
 *    Viewport for TAB in which drawing takes place.
 * @param modelViewport
 *    Viewport for MODEL in which drawing takes place.
 * @param windowIndex
 *    Index of browser window.
 * @param highlightTabFlag
 *    True indicates that the tab is highlighted (used in
 *    Tile Tabs mode so user knows graphics region corresponding
 *    to the tab that was recently selected).
 * @param browserTabContent
 *    Tab's content that is being drawn.
 */
BrainOpenGLViewportContent::BrainOpenGLViewportContent(const int windowViewport[4],
                                                       const int tabViewport[4],
                                                       const int modelViewport[4],
                                                       const int windowIndex,
                                                       const bool highlightTabFlag,
                                                       BrowserTabContent* browserTabContent)
: CaretObject(),
m_windowIndex(windowIndex),
m_highlightTab(highlightTabFlag),
m_browserTabContent(browserTabContent)
{
    m_windowX      = windowViewport[0];
    m_windowY      = windowViewport[1];
    m_windowWidth  = windowViewport[2];
    m_windowHeight = windowViewport[3];
    
    m_tabX      = tabViewport[0];
    m_tabY      = tabViewport[1];
    m_tabWidth  = tabViewport[2];
    m_tabHeight = tabViewport[3];
    
    m_modelX      = modelViewport[0];
    m_modelY      = modelViewport[1];
    m_modelWidth  = modelViewport[2];
    m_modelHeight = modelViewport[3];
    
    m_chartDataProjectionMatrix.identity();
    m_chartDataModelViewMatrix.identity();
    m_chartDataX = 0;
    m_chartDataY = 0;
    m_chartDataWidth = 0;
    m_chartDataHeight = 0;
    m_chartDataViewportValidFlag = false;
}

/**
 * Destructor.
 */
BrainOpenGLViewportContent::~BrainOpenGLViewportContent()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainOpenGLViewportContent::BrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj)
: CaretObject(obj),
m_windowIndex(obj.m_windowIndex),
m_highlightTab(obj.m_highlightTab)
{
    this->initializeMembersBrainOpenGLViewportContent();
    this->copyHelperBrainOpenGLViewportContent(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
BrainOpenGLViewportContent&
BrainOpenGLViewportContent::operator=(const BrainOpenGLViewportContent& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBrainOpenGLViewportContent(obj);
    }
    return *this;
}

/**
 * Initialize members of a new instance.
 */
void
BrainOpenGLViewportContent::initializeMembersBrainOpenGLViewportContent()
{
    m_chartDataProjectionMatrix.identity();
    m_chartDataModelViewMatrix.identity();
    m_chartDataX      = 0;
    m_chartDataY      = 0;
    m_chartDataWidth  = 0;
    m_chartDataHeight = 0;
    m_chartDataViewportValidFlag = false;
    m_modelX       = 0;
    m_modelY       = 0;
    m_modelWidth   = 0;
    m_modelHeight  = 0;
    m_tabX         = 0;
    m_tabY         = 0;
    m_tabWidth     = 0;
    m_tabHeight    = 0;
    m_windowX      = 0;
    m_windowY      = 0;
    m_windowWidth  = 0;
    m_windowHeight = 0;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
BrainOpenGLViewportContent::copyHelperBrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj)
{
    m_chartDataProjectionMatrix = obj.m_chartDataProjectionMatrix;
    m_chartDataModelViewMatrix  = obj.m_chartDataModelViewMatrix;
    m_chartDataX      = obj.m_chartDataX;
    m_chartDataY      = obj.m_chartDataY;
    m_chartDataWidth  = obj.m_chartDataWidth;
    m_chartDataHeight = obj.m_chartDataHeight;
    m_chartDataViewportValidFlag = obj.m_chartDataViewportValidFlag;
    m_modelX       = obj.m_modelX;
    m_modelY       = obj.m_modelY;
    m_modelWidth   = obj.m_modelWidth;
    m_modelHeight  = obj.m_modelHeight;
    m_tabX         = obj.m_tabX;
    m_tabY         = obj.m_tabY;
    m_tabWidth     = obj.m_tabWidth;
    m_tabHeight    = obj.m_tabHeight;
    m_windowX      = obj.m_windowX;
    m_windowY      = obj.m_windowY;
    m_windowWidth  = obj.m_windowWidth;
    m_windowHeight = obj.m_windowHeight;
    
    m_browserTabContent = obj.m_browserTabContent;
}

/**
 * Adjust the width/height using the aspect ratio
 */
void
BrainOpenGLViewportContent::adjustWidthHeightForAspectRatio(const float aspectRatio,
                                                            int32_t& width,
                                                            int32_t& height)
{
    if (aspectRatio > 0.0) {
        int32_t heightInt = height;
        float widthFloat  = width;
        float heightFloat = height;
        
        float preferredHeightFloat = MathFunctions::round(widthFloat * aspectRatio);
        const int32_t preferredHeightInt = static_cast<int32_t>(preferredHeightFloat);
        if (heightInt == preferredHeightInt) {
            /*
             * Viewport matches aspect ratio so do not need to 
             * adjust the width and height.
             *
             * Due to floating point error, when lock is enabled,
             * the preferred height may be a very small difference
             * from the current height.  So rounding and then
             * converting to an int prevents the graphics region
             * from a small resizing.
             */
        }
        else if (preferredHeightFloat > heightFloat) {
            /*
             * Shrink width
             */
            const float percentage = heightFloat / preferredHeightFloat;
            width = static_cast<int32_t>(widthFloat * percentage);
        }
        else {
            /*
             * Shrink height
             */
            height = preferredHeightInt;
        }
    }
}


/**
 * Adjust the given viewport by applying the given aspect ratio.
 *
 * Sets the new height to be width * aspect ratio.  If this new height
 * is too tall, the viewport width and height is scaled down so that 
 * the height fits the original viewport size and the viewport is
 * horizontally centered.  If the new height is less than the original
 * height, the viewport is centered vertically.
 *
 * @param viewport
 *     The viewport
 * @param aspectRatio
 *     The aspect ratio (height ~= width * aspect ratio)
 */
void
BrainOpenGLViewportContent::adjustViewportForAspectRatio(int viewport[4],
                                                         const float aspectRatio)
{
    int32_t heightInt = viewport[3];
    float widthFloat  = viewport[2];
    float heightFloat = viewport[3];
    
    float preferredHeightFloat = MathFunctions::round(widthFloat * aspectRatio);
    const int32_t preferredHeightInt = static_cast<int32_t>(preferredHeightFloat);
    if (heightInt == preferredHeightInt) {
        /*
         * Due to floating point error, when lock is enabled,
         * the preferred height may be a very small difference
         * from the current height.  So rounding and then
         * converting to an int prevents the graphics region
         * from a small resizing.
         */
    }
    else if (preferredHeightFloat > heightFloat) {
        const float percentage = heightFloat / preferredHeightFloat;
        widthFloat *= percentage;
        
        const float xOffset = (viewport[2] - widthFloat) / 2.0;
        viewport[0] += xOffset;
        viewport[2]  = widthFloat;
    }
    else {
        const float yOffset = (viewport[3] - preferredHeightFloat) / 2.0;
        viewport[1] += yOffset;
        viewport[3] = preferredHeightFloat;
    }
}


/**
 * @return  True indicates that the tab is highlighted (used in
 *    Tile Tabs mode so user knows graphics region corresponding
 *    to the tab that was recently selected).
 */
bool
BrainOpenGLViewportContent::isTabHighlighted() const
{
    return m_highlightTab;
}

/**
 * Get the data bounds and viewport for drawing the chart data.
 *
 * @param chartDataProjectionMatrixOut
 *    Output into which projection matrix for drawing chart data loaded.
 * @param chartDataModelViewMatrixOut
 *    Output into which model viewing matrix for drawing chart data loaded.
 * @param chartViewportOut
 *    Output into which viewport is loaded.
 *    (x, y, width, height)
 * @return 
 *    True if the chart data viewport is valid.
 */
bool
BrainOpenGLViewportContent::getChartDataMatricesAndViewport(Matrix4x4& chartDataProjectionMatrixOut,
                                                         Matrix4x4& chartDataModelViewMatrixOut,
                                                         int chartViewportOut[4]) const
{
    chartDataProjectionMatrixOut = m_chartDataProjectionMatrix;
    chartDataModelViewMatrixOut  = m_chartDataModelViewMatrix;
    chartViewportOut[0] = m_chartDataX;
    chartViewportOut[1] = m_chartDataY;
    chartViewportOut[2] = m_chartDataWidth;
    chartViewportOut[3] = m_chartDataHeight;
    return m_chartDataViewportValidFlag;
}

/**
 * Set the viewport for drawing the chart data.
 *
 * @param chartDataProjectionMatrix
 *    Pojection matrix for drawing chart data.
 * @param chartDataModelViewMatrix
 *    Viewing matrix for drawing chart data.
 * @param chartDataViewport
 *    Viewport (x, y, width, height).
 */
void
BrainOpenGLViewportContent::setChartDataMatricesAndViewport(const Matrix4x4& chartDataProjectionMatrix,
                                                         const Matrix4x4& chartDataModelViewMatrix,
                                                         const int chartViewport[4]) const
{
    m_chartDataProjectionMatrix = chartDataProjectionMatrix;
    m_chartDataModelViewMatrix  = chartDataModelViewMatrix;
    m_chartDataX = chartViewport[0];
    m_chartDataY = chartViewport[1];
    m_chartDataWidth = chartViewport[2];
    m_chartDataHeight = chartViewport[3];
    m_chartDataViewportValidFlag = true;
}


/**
 * Get the viewport for drawing the model (has been reduced
 * from tab viewport by applying the margin).
 *
 * @param modelViewport
 *    Output into which model viewport dimensions are loaded.
 *    (x, y, width, height)
 */
void
BrainOpenGLViewportContent::getModelViewport(int modelViewport[4]) const
{
    modelViewport[0] = m_modelX;
    modelViewport[1] = m_modelY;
    modelViewport[2] = m_modelWidth;
    modelViewport[3] = m_modelHeight;
}

/**
 * Get the viewport for drawing the tab (includes margin).
 *
 * @param tabViewport
 *    Output into which tab viewport dimensions are loaded.
 *    (x, y, width, height)
 */
void
BrainOpenGLViewportContent::getTabViewportBeforeApplyingMargins(int tabViewportOut[4]) const
{
    tabViewportOut[0] = m_tabX;
    tabViewportOut[1] = m_tabY;
    tabViewportOut[2] = m_tabWidth;
    tabViewportOut[3] = m_tabHeight;
}

/**
 * @return Pointer to the viewport for the window.
 *
 * @param windowViewportOut
 *    Output into which window viewport dimensions are loaded.
 *    (x, y, width, height)
 */
void
BrainOpenGLViewportContent::getWindowViewport(int windowViewportOut[4]) const
{
    windowViewportOut[0] = m_windowX;
    windowViewportOut[1] = m_windowY;
    windowViewportOut[2] = m_windowWidth;
    windowViewportOut[3] = m_windowHeight;
}

/**
 * @return The window index.
 */
int
BrainOpenGLViewportContent::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Pointer to tab content in viewport.
 */
BrowserTabContent* 
BrainOpenGLViewportContent::getBrowserTabContent() const
{
    return m_browserTabContent;
}

/**
 * @return Index of browser tab or -1 if there is not browser tab for this viewport.
 */
int32_t
BrainOpenGLViewportContent::getTabIndex() const
{
    if (m_browserTabContent != NULL) {
        return m_browserTabContent->getTabNumber();
    }
    
    return -1;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLViewportContent::toString() const
{
    const QString windowMsg = QString("   Window x=%1 y=%2 w=%3 h=%4").arg(m_windowX).arg(m_windowY).arg(m_windowWidth).arg(m_windowHeight);
    const QString tabMsg    = QString("   Tab    x=%1 y=%2 w=%3 h=%4").arg(m_tabX).arg(m_tabY).arg(m_tabWidth).arg(m_tabHeight);
    const QString modelMsg  = QString("   Model  x=%1 y=%2 w=%3 h=%4").arg(m_modelX).arg(m_modelY).arg(m_modelWidth).arg(m_modelHeight);
    
    AString msgOut;
    if (m_chartDataViewportValidFlag) {
        const QString chartProjectionMsg = QString("   Chart Projection=" + m_chartDataProjectionMatrix.toFormattedString("      "));
        const QString chartModelMsg = QString("   Chart Model View=" + m_chartDataModelViewMatrix.toFormattedString("      "));
        const QString chartViewportMsg = QString("   Chart x=%1 y=%2 w=%3 h=%4").arg(m_chartDataX).arg(m_chartDataY).arg(m_chartDataWidth).arg(m_chartDataHeight);
        msgOut.appendWithNewLine(chartProjectionMsg);
        msgOut.appendWithNewLine(chartModelMsg);
        msgOut.appendWithNewLine(chartViewportMsg);
    }
    else {
        msgOut.appendWithNewLine("   Chart Invalid.");
    }
    msgOut.appendWithNewLine(windowMsg);
    msgOut.appendWithNewLine(tabMsg);
    msgOut.appendWithNewLine(modelMsg);
    
    return msgOut;
}

/**
 * Create viewport contents for a single tab using the given window content and window sizes.
 *
 * @param browserTabContent
 *     Tab's content that is being drawn.
 * @param gapsAndMargins
 *     Gaps and margins
 * @param windowIndex
 *    Index of browser window.
 * @param windowViewport
 *    Viewport of WINDOW in which drawing takes place.
 * @return
 *     Viewport content for the single tab.
 */
BrainOpenGLViewportContent*
BrainOpenGLViewportContent::createViewportForSingleTab(BrowserTabContent* browserTabContent,
                                                              const GapsAndMargins* gapsAndMargins,
                                                              const int32_t windowIndex,
                                                              const int32_t windowViewport[4])
{
    int tabViewport[4] = {
        windowViewport[0],
        windowViewport[1],
        windowViewport[2],
        windowViewport[3]
    };
    
    int modelViewport[4] = {
        tabViewport[0],
        tabViewport[1],
        tabViewport[2],
        tabViewport[3]
    };
    
    if (browserTabContent != NULL) {
        if (browserTabContent->isAspectRatioLocked()) {
            const float aspectRatio = browserTabContent->getAspectRatio();
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(tabViewport,
                                                                     aspectRatio);
        }
        
        const int32_t tabIndex = browserTabContent->getTabNumber();
        createModelViewport(tabViewport,
                            tabIndex,
                            gapsAndMargins,
                            modelViewport);
    }
    

    
    BrainOpenGLViewportContent* vpContent = new BrainOpenGLViewportContent(windowViewport,
                                                                           tabViewport,
                                                                           modelViewport,
                                                                           windowIndex,
                                                                           false,
                                                                           browserTabContent);
    
    return vpContent;
}

/**
 * Create Viewport Contents for the given tab contents, window sizes, and tile sizes.
 *
 * @param tabContents
 *     Content of each tab.
 * @param tileTabsConfiguration
 *     The tile tabs configuration
 * @param gapsAndMargins
 *     Contains margins around edges of tabs
 * @param windowIndex
 *     Index of the window.
 * @param windowViewport
 *     The window's viewport.
 * @param hightlightTabIndex
 *     Index of tab that is highlighted when selected by user.
 * @return
 *     Vector containing data for drawing each model.
 */
std::vector<BrainOpenGLViewportContent*>
BrainOpenGLViewportContent::createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                             TileTabsConfiguration* tileTabsConfiguration,
                                                             const GapsAndMargins* gapsAndMargins,
                                                             const int32_t windowIndex,
                                                             const int32_t windowViewport[4],
                                                             const int32_t highlightTabIndex)
{
    CaretAssert(tileTabsConfiguration);
    CaretAssert(gapsAndMargins);
    
    std::vector<BrainOpenGLViewportContent*> viewportContentsOut;
    
    const int32_t numberOfTabs = static_cast<int32_t>(tabContents.size());
    if (numberOfTabs <= 0) {
        return viewportContentsOut;
    }
    
    const int32_t windowWidth  = windowViewport[2];
    const int32_t windowHeight = windowViewport[3];
    
    /*
     * The tile tabs configuration provides the sizes of the 
     * rows and columns.  The user may "stretch" rows and/or
     * columns.  Thus, the tabs viewports may not be uniformly sized.
     */
    std::vector<int32_t> tabConfigRowHeights;
    std::vector<int32_t> tabConfigColumnWidths;
    tileTabsConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                     windowHeight,
                                                                     numberOfTabs,
                                                                     tabConfigRowHeights,
                                                                     tabConfigColumnWidths);
    
    const int32_t numRows    = static_cast<int32_t>(tabConfigRowHeights.size());
    const int32_t numColumns = static_cast<int32_t>(tabConfigColumnWidths.size());
    const int32_t numCells   = numRows * numColumns;
    if (numCells <= 0) {
        return viewportContentsOut;
    }
    CaretAssert(numRows > 0);
    CaretAssert(numColumns > 0);
    
    /*
     * Due to aspect ratios, the width or height
     * of tab viewports may shrink so we will
     * need to recompute the row heights and column
     * widths.
     */
    std::vector<int32_t> rowHeights(numRows, 0);
    std::vector<int32_t> columnWidths(numColumns, 0);
    
    /*
     * Create the sizes for the tabs before and after application
     * of aspect ratio.
     */
    std::vector<TileTabsViewportSizingInfo> tabSizeInfoVector;
    int32_t iTab = 0;
    for (int32_t iRowFromTop = 0; iRowFromTop < numRows; iRowFromTop++) {
        const int32_t vpHeight = tabConfigRowHeights[iRowFromTop];
        for (int32_t jCol = 0; jCol < numColumns; jCol++) {
            const int32_t vpWidth = tabConfigColumnWidths[jCol];
            if (iTab < numberOfTabs) {
                CaretAssertVectorIndex(tabContents,
                                       iTab);
                
                /*
                 * Note: the constructor will adjust the width and
                 * height if lock aspect ratio is enabled.
                 */
                TileTabsViewportSizingInfo tsi(tabContents[iTab],
                                  iRowFromTop,
                                  jCol,
                                  vpWidth,
                                  vpHeight);
                
                rowHeights[iRowFromTop] = std::max(rowHeights[iRowFromTop],
                                                   tsi.m_height);
                columnWidths[jCol] = std::max(columnWidths[jCol],
                                              tsi.m_width);
                
                tabSizeInfoVector.push_back(tsi);
                iTab++;
            }
            else {
                /*
                 * Get out of loop
                 */
                iRowFromTop = numRows;
                jCol        = numColumns;
            }
        }
    }

    /*
     * Note: There may be more tabs than there are cells (rows * columns)
     * so some tabs may not be displayed.
     */
    const int32_t numberOfDisplayedTabs = static_cast<int32_t>(tabSizeInfoVector.size());
    
    /*
     * Now that we know the height of each row, and width of each column,
     * we can get the total width and height of ALL tab viewports.
     */
    const int32_t allTabsHeight = std::accumulate(rowHeights.begin(), rowHeights.end(), 0);
    const int32_t allTabsWidth  = std::accumulate(columnWidths.begin(), columnWidths.end(), 0);
    
    /*
     * The total width/height of the tabs may be less than the size
     * of the window viewport.  We want to center the tabs inside of
     * the window viewport so find any extra space in the window.
     */
    const int32_t windowExtraWidth  = windowWidth  - allTabsWidth;
    const int32_t windowExtraHeight = windowHeight - allTabsHeight;
    CaretAssert(windowExtraWidth >= 0);
    CaretAssert(windowExtraHeight >= 0);

    /*
     * Set the X and Y-coordinates for the tab viewports
     * We start at the bottom row, left corner.
     */
    int32_t vpY = windowViewport[1] + (windowExtraHeight / 2);
    for (int32_t iRow = (numRows - 1); iRow >= 0; iRow--) {
        int32_t vpX = windowViewport[0] + (windowExtraWidth / 2);
        for (int32_t jCol = 0; jCol < numColumns; jCol++) {
            TileTabsViewportSizingInfo* tabSizePtr = NULL;
            for (int32_t iTab = 0; iTab < numberOfDisplayedTabs; iTab++) {
                CaretAssertVectorIndex(tabSizeInfoVector, iTab);
                if ((tabSizeInfoVector[iTab].m_rowIndexFromTop == iRow)
                    && (tabSizeInfoVector[iTab].m_columnIndex == jCol)) {
                    tabSizePtr = &tabSizeInfoVector[iTab];
                    break;
                }
            }
            
            if (tabSizePtr != NULL) {
                CaretAssertVectorIndex(columnWidths, jCol);
                CaretAssertVectorIndex(rowHeights, iRow);
                /*
                 * Center tab inside of its region
                 */
                const int32_t tabExtraWidth = columnWidths[jCol] - tabSizePtr->m_width;
                const int32_t tabExtraHeight = rowHeights[iRow]  - tabSizePtr->m_height;
                
                const int32_t tabX = vpX + (tabExtraWidth / 2);
                const int32_t tabY = vpY + (tabExtraHeight / 2);
                
                const int tabViewport[4] = {
                    tabX,
                    tabY,
                    tabSizePtr->m_width,
                    tabSizePtr->m_height
                };

                /*
                 * Model is drawn in the model viewport inside any margins.
                 */
                const int32_t tabIndex = tabSizePtr->m_browserTabContent->getTabNumber();
                int modelViewport[4] = { 0, 0, 0, 0 };
                createModelViewport(tabViewport,
                                    tabIndex,
                                    gapsAndMargins,
                                    modelViewport);
                
                //tabSizePtr->print(tabX, tabY);

                BrainOpenGLViewportContent* vpContent = new BrainOpenGLViewportContent(windowViewport,
                                                                                       tabViewport,
                                                                                       modelViewport,
                                                                                       windowIndex,
                                                                                       (highlightTabIndex ==tabIndex),
                                                                                       tabSizePtr->m_browserTabContent);
                viewportContentsOut.push_back(vpContent);
            }
            else {
                /*
                 * If the number of tabs is less than the number of tiles,
                 * empty viewport content is needed so that user can draw
                 * annotations in these regions.
                 */
                const int tabViewport[4] = {
                    vpX,
                    vpY,
                    columnWidths[jCol],
                    rowHeights[iRow]
                };
                BrainOpenGLViewportContent* vpContent = new BrainOpenGLViewportContent(windowViewport,
                                                                                       tabViewport,
                                                                                       tabViewport,
                                                                                       windowIndex,
                                                                                       false,
                                                                                       NULL);
                viewportContentsOut.push_back(vpContent);
            }
            
            CaretAssertVectorIndex(columnWidths, jCol);
            vpX += columnWidths[jCol];
        }
        CaretAssertVectorIndex(rowHeights, iRow);
        vpY += rowHeights[iRow];
    }
    
    return viewportContentsOut;
}

/**
 * Create viewport from the model using the tab's viewport and the tabs margins.
 *
 * @paramj tabViewport
 *     Viewport for the tab.
 * @param tabIndex
 *     Index of the tab.
 * @param gapsAndMargins
 *     Gaps and margins.
 * @param modelViewportOut
 *     Output containing viewport for model.
 */
void
BrainOpenGLViewportContent::createModelViewport(const int tabViewport[4],
                                                const int32_t tabIndex,
                                                const GapsAndMargins* gapsAndMargins,
                                                int modelViewportOut[4])
{
    int32_t leftMargin   = 0;
    int32_t rightMargin  = 0;
    int32_t bottomMargin = 0;
    int32_t topMargin    = 0;
    
    modelViewportOut[0] = tabViewport[0];
    modelViewportOut[1] = tabViewport[1];
    modelViewportOut[2] = tabViewport[2];
    modelViewportOut[3] = tabViewport[3];
    
    if (gapsAndMargins != NULL) {
        gapsAndMargins->getMarginsInPixelsForDrawing(tabIndex,
                                                     tabViewport[2],
                                                     tabViewport[3],
                                                     leftMargin,
                                                     rightMargin,
                                                     bottomMargin,
                                                     topMargin);
        const int32_t marginHorizSize = (leftMargin   + rightMargin);
        const int32_t marginVertSize  = (bottomMargin + topMargin);
        if ((marginHorizSize < modelViewportOut[2])
            && (marginVertSize < modelViewportOut[3])) {
            modelViewportOut[0] += leftMargin;
            modelViewportOut[1] += bottomMargin;
            modelViewportOut[2] -= marginHorizSize;
            modelViewportOut[3] -= marginVertSize;
        }
        else {
            CaretLogSevere("Margins are too big for tab "
                           + AString::number(tabIndex + 1)
                           + " viewport.  Viewport (x,y,w,h)="
                           + AString::fromNumbers(modelViewportOut, 4, ",")
                           + " margin (l,r,b,t)="
                           + AString::number(leftMargin) + ","
                           + AString::number(rightMargin) + ","
                           + AString::number(bottomMargin) + ","
                           + AString::number(topMargin));
        }
    }
}

/**
 * When viewing a surface montage, "getModelViewport()" returns the viewport
 * for the entire surface montage (viewport containing all of the surfaces).
 * However, the are instance where we need the viewport for one of surfaces
 * in a surface montage.  This method will find the viewport for one of the 
 * surfaces within the surface montage using the given X and Y coordinates.
 *
 * @param montageX
 *     X-coordinate within the surface montage.
 * @param montageX
 *     X-coordinate within the surface montage.
 * @param subViewportOut
 *     Viewport for individual surface in the montage at the given X, Y 
 *     coordinates.   Note: if a surface montage is not displayed, 
 *     this method returns the same viewport as getModelViewport().
 */
void
BrainOpenGLViewportContent::getSurfaceMontageModelViewport(const int32_t montageX,
                                                           const int32_t montageY,
                                                           int subViewportOut[4]) const
{
    getModelViewport(subViewportOut);
    
    if (this->m_browserTabContent == NULL) {
        return;
    }
    
    ModelSurfaceMontage* msm = m_browserTabContent->getDisplayedSurfaceMontageModel();
    if (msm != NULL) {
        std::vector<const SurfaceMontageViewport*> montageViewports;
        msm->getSurfaceMontageViewportsForTransformation(m_browserTabContent->getTabNumber(),
                                                         montageViewports);
        
        const int x = montageX; // + m_tabX;
        const int y = montageY; // + m_tabY;
        
        for (std::vector<const SurfaceMontageViewport*>::const_iterator iter = montageViewports.begin();
             iter != montageViewports.end();
             iter++) {
            const SurfaceMontageViewport* smv = *iter;
            
            int vp[4];
            smv->getViewport(vp);
            
            const int offsetX = x - vp[0];
            if ((offsetX >= 0)
                && (offsetX < vp[2])) {
                const int offsetY = y - vp[1];
                
                if ((offsetY >= 0)
                    && (offsetY < vp[3])) {
                    smv->getViewport(subViewportOut);
                    return;
                }
            }
            
        }
    }
}


/* =================================================================================================== */


/**
 * Contructor for tile tabs viewport sizing.
 *
 * @param browserTabContent
 *    Content of the browser tab.
 * @param rowIndexFromTop
 *    Row index starting with top row
 * @param columnIndex
 *    Column index starting on left.
 * @param initialWidth
 *    Initial width of the tab prior to application of aspect ratio.
 * @param initialHeight
 *    Initial height of the tab prior to application of aspect ratio.
 */
BrainOpenGLViewportContent::TileTabsViewportSizingInfo::TileTabsViewportSizingInfo(BrowserTabContent* browserTabContent,
                                                                                   const int32_t rowIndexFromTop,
                                                                                   const int32_t columnIndex,
                                                                                   const float initialWidth,
                                                                                   const float initialHeight)
: m_browserTabContent(browserTabContent),
m_rowIndexFromTop(rowIndexFromTop),
m_columnIndex(columnIndex),
m_initialWidth(initialWidth),
m_initialHeight(initialHeight),
m_width(initialWidth),
m_height(initialHeight)
{
    if (browserTabContent->isAspectRatioLocked()) {
        const float aspectRatio = browserTabContent->getAspectRatio();
        if (aspectRatio > 0.0) {
            BrainOpenGLViewportContent::adjustWidthHeightForAspectRatio(aspectRatio,
                                                                        m_width,
                                                                        m_height);
        }
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainOpenGLViewportContent::TileTabsViewportSizingInfo&
BrainOpenGLViewportContent::TileTabsViewportSizingInfo::operator=(const TileTabsViewportSizingInfo& obj)
{
    if (this != &obj) {
        m_browserTabContent = obj.m_browserTabContent;
        m_rowIndexFromTop   = obj.m_rowIndexFromTop;
        m_columnIndex       = obj.m_columnIndex;
        m_initialWidth      = obj.m_initialWidth;
        m_initialHeight     = obj.m_initialHeight;
        m_width             = obj.m_initialWidth;
        m_height            = obj.m_initialHeight;
    }
    
    return *this;
}

/**
 * Print for debugging.
 *
 * @param x
 *     X-coordinate of tab viewport.
 * @param y
 *     Y-coordinate of tab viewport.
 */
void
BrainOpenGLViewportContent::TileTabsViewportSizingInfo::print(const int32_t x,
                                                              const int32_t y)
{
    const QString msg("Model: " + m_browserTabContent->getName()
                      + "\n   row/col: " + QString::number(m_rowIndexFromTop) + ", " + QString::number(m_columnIndex)
                      + "\n   x/y: " + QString::number(x) + ", " + QString::number(y)
                      + "\n   width/height: " + QString::number(m_width) + ", " + QString::number(m_height));
    std::cout << qPrintable(msg) << std::endl;
}


/**
 * Get the viewport for a slice in all slices view.
 *
 * @param tabViewport
 *    The viewport for the tab containing all slices.
 * @param sliceViewPlane
 *    The plane for slice drawing.  Note: "ALL" is used for orientation axes in oblique view.
 * @param allPlanesLayout
 *    The layout in ALL slices view.
 * @param viewportOut
 *    Output viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLViewportContent::getSliceAllViewViewport(const int32_t tabViewport[4],
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                                       int32_t viewportOut[4])
{
    const int32_t gap = 2;
    const int32_t tabViewportX      = tabViewport[0];
    const int32_t tabViewportY      = tabViewport[1];
    const int32_t tabViewportWidth  = tabViewport[2];
    const int32_t tabViewportHeight = tabViewport[3];
    
    switch (allPlanesLayout) {
        case VolumeSliceViewAllPlanesLayoutEnum::COLUMN_LAYOUT:
        {
            const int32_t vpHeight = (tabViewportHeight - (gap * 2)) / 3;
            const int32_t vpOffsetY = vpHeight + gap;
            const int32_t vpWidth  = tabViewportWidth;
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    viewportOut[0] = 0;
                    viewportOut[1] = 0;
                    viewportOut[2] = 0;
                    viewportOut[3] = 0;
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY + vpOffsetY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY + (vpOffsetY * 2);
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
            }
        }
            break;
        case VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT:
        {
            const int32_t vpWidth   = (tabViewportWidth  - gap) / 2;
            const int32_t vpHeight  = (tabViewportHeight - gap) / 2;
            const int32_t vpOffsetX = vpWidth  + gap;
            const int32_t vpOffsetY = vpHeight + gap;
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    viewportOut[0] = tabViewportX + vpOffsetX;
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    viewportOut[0] = tabViewportX + vpOffsetX;
                    viewportOut[1] = tabViewportY + vpOffsetY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY + vpOffsetY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
            }
        }
            break;
        case VolumeSliceViewAllPlanesLayoutEnum::ROW_LAYOUT:
        {
            const int32_t vpWidth   = (tabViewportWidth - (gap * 2)) / 3;
            const int32_t vpOffsetX = vpWidth + gap;
            const int32_t vpHeight  = tabViewportHeight;
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    viewportOut[0] = 0;
                    viewportOut[1] = 0;
                    viewportOut[2] = 0;
                    viewportOut[3] = 0;
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    viewportOut[0] = tabViewportX + (vpOffsetX * 2);
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    viewportOut[0] = tabViewportX;
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    viewportOut[0] = tabViewportX + vpOffsetX;
                    viewportOut[1] = tabViewportY;
                    viewportOut[2] = vpWidth;
                    viewportOut[3] = vpHeight;
                    break;
            }
        }
            break;
    }
}

/*
 * @return The slice view plane for the given viewport coordinate.
 * If ALL is returned, is indicates that the given viewport coordinate
 * is in the bottom left region in which volume slices are not displayed.
 *
 * @param viewport
 *   The viewport.
 * @param mousePressX
 *   X Location of the mouse press.
 * @param mousePressY
 *   Y Location of the mouse press.
 * @param allPlanesLayout
 *    The layout in ALL slices view.
 * @param sliceViewportOut
 *    Output viewport (region of graphics area) for drawing slices.
 */
VolumeSliceViewPlaneEnum::Enum
BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(const int32_t viewport[4],
                                                                   const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                                                   const int32_t mousePressX,
                                                                   const int32_t mousePressY,
                                                                   int32_t sliceViewportOut[4])
{
    VolumeSliceViewPlaneEnum::Enum view = VolumeSliceViewPlaneEnum::ALL;
    
    std::vector<VolumeSliceViewPlaneEnum::Enum> allSlicePlanes;
    VolumeSliceViewPlaneEnum::getAllEnums(allSlicePlanes);
    for (auto slicePlane : allSlicePlanes) {
        BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                                           slicePlane,
                                                           allPlanesLayout,
                                                           sliceViewportOut);
        const int32_t vpX = mousePressX - sliceViewportOut[0];
        const int32_t vpY = mousePressY - sliceViewportOut[1];
        if ((vpX >= 0)
            && (vpY >= 0)
            && (vpX < sliceViewportOut[2])
            && (vpY < sliceViewportOut[3])) {
            return slicePlane;
        }
    }
    
    CaretLogSevere("Failed to find slice plane in all sliced view");
    return view;
}

