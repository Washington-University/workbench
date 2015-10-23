
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
 * @param tileTabsRowIndex
 *    Row index when tile tabs is enabled.
 * @param tileTabsColumnIndex
 *    Column index when tile tabs is enabled.
 * @param windowViewport
 *    Viewport of WINDOW in which drawing takes place.
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
BrainOpenGLViewportContent::BrainOpenGLViewportContent(const int32_t tileTabsRowIndex,
                                                       const int32_t tileTabsColumnIndex,
                                                       const int windowViewport[4],
                                                       const int modelViewportIn[4],
                                                       const int32_t windowIndex,
                                                       const bool highlightTabFlag,
                                                       const GapsAndMargins* gapsAndMargins,
                                                       BrowserTabContent* browserTabContent)
: CaretObject(),
m_tileTabsRowIndex(tileTabsRowIndex),
m_tileTabsColumnIndex(tileTabsColumnIndex),
m_windowIndex(windowIndex),
m_highlightTab(highlightTabFlag),
m_browserTabContent(browserTabContent)
{
    initializeMembersBrainOpenGLViewportContent();
    
    m_windowX = windowViewport[0];
    m_windowY = windowViewport[1];
    m_windowWidth = windowViewport[2];
    m_windowHeight = windowViewport[3];
    
    int modelViewport[4] = {
        modelViewportIn[0],
        modelViewportIn[1],
        modelViewportIn[2],
        modelViewportIn[3]
    };
    
    if (m_browserTabContent != NULL) {
        if (m_browserTabContent->isAspectRatioLocked()) {
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(modelViewport,
                                                                     m_browserTabContent->getAspectRatio());
        }
    }
    
    m_tabX = modelViewport[0];
    m_tabY = modelViewport[1];
    m_tabWidth = modelViewport[2];
    m_tabHeight = modelViewport[3];
    
    m_modelX = modelViewport[0];
    m_modelY = modelViewport[1];
    m_modelWidth = modelViewport[2];
    m_modelHeight = modelViewport[3];
    
    
    if (browserTabContent != NULL) {
        /*
         * An "ALL" selection applies first tab margin to ALL tabs
         */
        const int32_t tabIndex = browserTabContent->getTabNumber();
        int32_t leftMargin   = 0;
        int32_t rightMargin  = 0;
        int32_t bottomMargin = 0;
        int32_t topMargin    = 0;
        if (gapsAndMargins != NULL) {
            gapsAndMargins->getMarginsInPixelsForDrawing(tabIndex,
                                                         modelViewport[2],
                                                         modelViewport[3],
                                                         leftMargin,
                                                         rightMargin,
                                                         bottomMargin,
                                                         topMargin);
        }
        
        const int32_t marginHorizSize = (leftMargin   + rightMargin);
        const int32_t marginVertSize  = (bottomMargin + topMargin);
        if ((marginHorizSize < modelViewport[2])
            && (marginVertSize < modelViewport[3])) {
            m_modelX = modelViewport[0] + leftMargin;
            m_modelY = modelViewport[1] + bottomMargin;
            m_modelWidth = modelViewport[2] - marginHorizSize;
            m_modelHeight = modelViewport[3] - marginVertSize;
        }
        else {
            CaretLogSevere("Margins are too big for tab "
                           + AString::number(browserTabContent->getTabNumber() + 1)
                           + " viewport.  Viewport (x,y,w,h)="
                           + AString::fromNumbers(modelViewport, 4, ",")
                           + " margin (l,r,b,t)="
                           + AString::number(leftMargin) + ","
                           + AString::number(rightMargin) + ","
                           + AString::number(bottomMargin) + ","
                           + AString::number(topMargin));
        }
    }
    
    /*
     * If margins are too big, they could make the viewport invalid
     * so test for it and if the viewport is invalid,
     * override with original viewport.
     */
    bool validViewportFlag = true;
    for (int32_t i = 0; i < 4; i++) {
        if (m_modelViewport[i] < 0) {
            validViewportFlag = false;
        }
    }
    if ( ! validViewportFlag) {
        m_modelX = modelViewport[0];
        m_modelY = modelViewport[1];
        m_modelWidth = modelViewport[2];
        m_modelHeight = modelViewport[3];
    }
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
m_tileTabsRowIndex(obj.m_tileTabsRowIndex),
m_tileTabsColumnIndex(obj.m_tileTabsColumnIndex),
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
BrainOpenGLViewportContent::getTabViewport(int tabViewportOut[4]) const
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
    const QString rowColMsg = QString("Row=%1 Col=%2").arg(m_tileTabsRowIndex).arg(m_tileTabsColumnIndex);
    const QString windowMsg = QString("   Window x=%1 y=%2 w=%3 h=%4").arg(m_windowX).arg(m_windowY).arg(m_windowWidth).arg(m_windowHeight);
    const QString tabMsg    = QString("   Tab    x=%1 y=%2 w=%3 h=%4").arg(m_tabX).arg(m_tabY).arg(m_tabWidth).arg(m_tabHeight);
    const QString modelMsg  = QString("   Model  x=%1 y=%2 w=%3 h=%4").arg(m_modelX).arg(m_modelY).arg(m_modelWidth).arg(m_modelHeight);
    
    AString msgOut(rowColMsg);
    msgOut.appendWithNewLine(windowMsg);
    msgOut.appendWithNewLine(tabMsg);
    msgOut.appendWithNewLine(modelMsg);
    
    return msgOut;
}

/**
 * Create viewport contents for a single tab using the given window content and window sizes.
 *
 * @param windowViewport
 *    Viewport of WINDOW in which drawing takes place.
 * @param modelViewport
 *    Viewport for MODEL in which drawing takes place.
 * @param windowIndex
 *    Index of browser window.
 * @param highlightTabFlag
 *    True indicates that the tab is highlighted (used in
 *    Tile Tabs mode so user knows graphics region corresponding
 *    to the tab that was recently selected).
 * @param gapsAndMargins
 *     Gaps and margins
 * @param browserTabContent
 *     Tab's content that is being drawn.
 * @return
 *     Viewport content for the single tab.
 */
BrainOpenGLViewportContent*
BrainOpenGLViewportContent::createViewportForSingleTab(const int windowViewport[4],
                                                       const int modelViewport[4],
                                                       const int windowIndex,
                                                       const bool highlightTabFlag,
                                                       const GapsAndMargins* gapsAndMargins,
                                                       BrowserTabContent* browserTabContent)
{
    BrainOpenGLViewportContent* vpContent = new BrainOpenGLViewportContent(0,
                                                                           0,
                                                                           windowViewport,
                                                                           modelViewport,
                                                                           windowIndex,
                                                                           highlightTabFlag,
                                                                           gapsAndMargins,
                                                                           browserTabContent);
    
    std::vector<BrainOpenGLViewportContent*> viewportsVector;
    viewportsVector.push_back(vpContent);
    
    wrapWindowViewportAroundTabViewports(viewportsVector);
    
    return vpContent;
}

/**
 * Create Viewport Contents for the given tab contents, window sizes, and tile sizes.
 *
 * @paramj tabContents
 *     Content of each tab.
 * @param windowIndex
 *     Index of the window.
 * @param windowViewport
 *     The window's viewport.
 * @param rowHeights
 *     Height of each row.
 * @param columnWidths
 *     Width of each column.
 * @param hightlightTabIndex
 *     Index of tab that is highlighted when selected by user.
 * @return
 *     Vector containing data for drawing each model.
 */
std::vector<BrainOpenGLViewportContent*>
BrainOpenGLViewportContent::createViewportContentForTileTabs(std::vector<BrowserTabContent*>& tabContents,
                                                             const int32_t windowIndex,
                                                             const int32_t windowViewport[4],
                                                             const std::vector<int32_t>& rowHeights,
                                                             const std::vector<int32_t>& columnWidths,
                                                             const int32_t highlightTabIndex,
                                                             const GapsAndMargins* gapsAndMargins)
{
    std::vector<BrainOpenGLViewportContent*> viewportContentsOut;
    
    const int32_t numRows = static_cast<int32_t>(rowHeights.size());
    const int32_t numCols = static_cast<int32_t>(columnWidths.size());
    const int32_t numCells = numRows * numCols;
    if (numCells <= 0) {
        return viewportContentsOut;
    }
    const int32_t numTabs = static_cast<int32_t>(tabContents.size());
    
    CaretAssert(numCols > 0);
    std::vector<int32_t> maxTabWidthInEachColumn(numCols, 0);
    CaretAssert(numRows > 0);
    std::vector<int32_t> maxTabHeightInEachRow(numRows, 0);
    
    /*
     * Arrange models left-to-right and top-to-bottom.
     */
    int32_t vpX = 0;
    int32_t vpY = windowViewport[1] + windowViewport[3];
    
    int32_t iTab = 0;
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        const int32_t vpHeight = rowHeights[iRow];
        vpX = windowViewport[0];
        vpY -= vpHeight;
        for (int32_t jCol = 0; jCol < numCols; jCol++) {
            const int32_t vpWidth = columnWidths[jCol];
            if (iTab < numTabs) {
                const int modelViewport[4] = {
                    vpX,
                    vpY,
                    vpWidth,
                    vpHeight
                };
                
                CaretAssertVectorIndex(tabContents, iTab);
                BrowserTabContent* tabContent = tabContents[iTab];
                const bool highlightTab = (highlightTabIndex == tabContent->getTabNumber());
                BrainOpenGLViewportContent* vc =
                new BrainOpenGLViewportContent(iRow,
                                               jCol,
                                               windowViewport,
                                               modelViewport,
                                               windowIndex,
                                               highlightTab,
                                               gapsAndMargins,
                                               tabContent);
                viewportContentsOut.push_back(vc);
                
                CaretAssertVectorIndex(maxTabHeightInEachRow, iRow);
                maxTabHeightInEachRow[iRow] = std::max(maxTabHeightInEachRow[iRow], vc->m_tabHeight);
                
                CaretAssertVectorIndex(maxTabWidthInEachColumn, jCol);
                maxTabWidthInEachColumn[jCol] = std::max(maxTabWidthInEachColumn[jCol], vc->m_tabWidth);
                
            }
            iTab++;
            vpX += vpWidth;
            
            if (iTab >= numTabs) {
                /*
                 * More cells than models for drawing so set loop
                 * indices so that loops terminate
                 */
                jCol = numCols;
                iRow = numRows;
            }
        }
    }

    /*
     * May need to adjust viewport positions so that any extra space
     * is along the sides of the window and not between the viewports.
     */
    centerViewportsInWindow(viewportContentsOut,
                            rowHeights,
                            columnWidths);

//    for (std::vector<BrainOpenGLViewportContent*>::iterator iter = viewportContentsOut.begin();
//         iter != viewportContentsOut.end();
//         iter++) {
//        BrainOpenGLViewportContent* vpContent = *iter;
//        std::cout << qPrintable(vpContent->toString()) << std::endl;
//    }
    
    return viewportContentsOut;
}

/**
 * The width and/or height of a tab viewport may be smaller than the width and/or
 * height of the cell created by tile tabs space allocation.  As a result, there may
 * be large gaps between adjacent tiles, especially if "lock aspect" is enabled.
 * This method will adjust the tile viewports so that they are centered in the window
 * and any extra space is around the sides of the window.
 *
 * @paramj tabContents
 *     Content of each tab.
 * @param rowHeights
 *     Height of each row from tile tabs.
 * @param columnWidths
 *     Width of each column from tile tabs.
 */
void
BrainOpenGLViewportContent::centerViewportsInWindow(std::vector<BrainOpenGLViewportContent*>& viewports,
                                                    const std::vector<int32_t>& rowHeights,
                                                    const std::vector<int32_t>& columnWidths)
{
    const int32_t numRows = static_cast<int32_t>(rowHeights.size());
    const int32_t numCols = static_cast<int32_t>(columnWidths.size());
    std::vector<int32_t> maxTabWidthInEachColumn(numCols, 0);
    std::vector<int32_t> maxTabHeightInEachRow(numRows, 0);
    
    /*
     * Find the maximum width from tab content in each row and column
     */
    for (std::vector<BrainOpenGLViewportContent*>::iterator iter = viewports.begin();
         iter != viewports.end();
         iter++) {
        BrainOpenGLViewportContent* vpContent = *iter;
        
        CaretAssertVectorIndex(maxTabHeightInEachRow, vpContent->m_tileTabsRowIndex);
        maxTabHeightInEachRow[vpContent->m_tileTabsRowIndex] = std::max(maxTabHeightInEachRow[vpContent->m_tileTabsRowIndex],
                                                                        vpContent->m_tabHeight);

        CaretAssertVectorIndex(maxTabWidthInEachColumn, vpContent->m_tileTabsColumnIndex);
        maxTabWidthInEachColumn[vpContent->m_tileTabsColumnIndex] = std::max(maxTabWidthInEachColumn[vpContent->m_tileTabsColumnIndex],
                                                                        vpContent->m_tabWidth);
    }
    
    const int32_t totalWidth = std::accumulate(columnWidths.begin(),
                                               columnWidths.end(),
                                               0);
    const int32_t totalTabWidth = std::accumulate(maxTabWidthInEachColumn.begin(),
                                                  maxTabWidthInEachColumn.end(),
                                                  0);
    const int32_t widthDiff = totalWidth - totalTabWidth;
    if (widthDiff > 0) {
        /*
         * Width of tabs is less than window width so adjust tab viewport
         * X-coordinates to center all tab viewports in the window.
         */
        const int32_t leftOffset = widthDiff / 2;
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            int32_t x = leftOffset;
            for (int32_t jCol = 0; jCol < numCols; jCol++) {
                CaretAssertVectorIndex(maxTabWidthInEachColumn, jCol);
                const int32_t cellWidth = maxTabWidthInEachColumn[jCol];
                
                BrainOpenGLViewportContent* vp = findViewportAtRowColumn(viewports, iRow, jCol);
                if (vp != NULL) {
                    const int32_t extraTabSpace = cellWidth - vp->m_tabWidth;
                    const int32_t tabOffset     = extraTabSpace / 2;
                    vp->m_tabX                  = x + tabOffset;
                    
                    const int32_t extraModelSpace = vp->m_tabWidth - vp->m_modelWidth;
                    const int32_t modelOffsetX    = extraModelSpace / 2;
                    vp->m_modelX                  = vp->m_tabX + modelOffsetX;
                }
                
                x += cellWidth;
            }
        }
    }
    const int32_t totalHeight = std::accumulate(rowHeights.begin(),
                                                rowHeights.end(),
                                                0);
    const int32_t totalTabHeight = std::accumulate(maxTabHeightInEachRow.begin(),
                                                   maxTabHeightInEachRow.end(),
                                                   0);
    const int32_t heightDiff = totalHeight - totalTabHeight;
    if (heightDiff > 0) {
        /*
         * Height of tabs is less than window height so adjust tab viewport
         * Y-coordinates to center all tab viewports in the window.
         */
        const int32_t bottomOffset = heightDiff / 2;
        for (int32_t jCol = 0; jCol < numCols; jCol++) {
            int32_t y = bottomOffset;
            for (int32_t iRow = (numRows - 1); iRow >= 0; iRow--) {
                CaretAssertVectorIndex(maxTabHeightInEachRow, iRow);
                const int32_t cellHeight = maxTabHeightInEachRow[iRow];
                
                BrainOpenGLViewportContent* vp = findViewportAtRowColumn(viewports, iRow, jCol);
                if (vp != NULL) {
                    const int32_t extraTabSpace = cellHeight - vp->m_tabHeight;
                    const int32_t tabOffset     = extraTabSpace / 2;
                    vp->m_tabY                  = y + tabOffset;
                    
                    const int32_t extraModelSpace = vp->m_tabHeight - vp->m_modelHeight;
                    const int32_t modelOffsetY    = extraModelSpace / 2;
                    vp->m_modelY                  = vp->m_tabY + modelOffsetY;
                }
                
                y += cellHeight;
            }
        }
    }
    
    if ((widthDiff > 0)
        || (heightDiff > 0)) {
        wrapWindowViewportAroundTabViewports(viewports);
    }
}

/**
 * Adjust the window viewport so that it encloses the tab viewports.
 *
 * @param viewports
 *     Viewports that are searched.
 */
void
BrainOpenGLViewportContent::wrapWindowViewportAroundTabViewports(std::vector<BrainOpenGLViewportContent*>& viewports)
{
    int32_t boundsMinX = std::numeric_limits<int32_t>::max();
    int32_t boundsMaxX = std::numeric_limits<int32_t>::min();
    int32_t boundsMinY = std::numeric_limits<int32_t>::max();
    int32_t boundsMaxY = std::numeric_limits<int32_t>::min();
    for (std::vector<BrainOpenGLViewportContent*>::iterator iter = viewports.begin();
         iter != viewports.end();
         iter++) {
        BrainOpenGLViewportContent* vpContent = *iter;
        const int32_t tabMinX = vpContent->m_tabX;
        const int32_t tabMaxX = tabMinX + vpContent->m_tabWidth;
        const int32_t tabMinY = vpContent->m_tabY;
        const int32_t tabMaxY = tabMinY + vpContent->m_tabHeight;
        
        boundsMinX = std::min(boundsMinX, tabMinX);
        boundsMaxX = std::max(boundsMaxX, tabMaxX);
        boundsMinY = std::min(boundsMinY, tabMinY);
        boundsMaxY = std::max(boundsMaxY, tabMaxY);
    }
    
    if ((boundsMaxX > boundsMinX)
        && (boundsMaxY > boundsMinY)) {
        const int32_t vpX = boundsMinX;
        const int32_t vpY = boundsMinY;
        const int32_t vpWidth  = boundsMaxX - boundsMinX;
        const int32_t vpHeight = boundsMaxY - boundsMinY;
        
        for (std::vector<BrainOpenGLViewportContent*>::iterator iter = viewports.begin();
             iter != viewports.end();
             iter++) {
            BrainOpenGLViewportContent* vpContent = *iter;
            vpContent->m_windowX = vpX;
            vpContent->m_windowY = vpY;
            vpContent->m_windowWidth = vpWidth;
            vpContent->m_windowHeight = vpHeight;
        }
    }
}

/**
 * Find the viewport for the given row and column.
 *
 * @param viewports
 *     Viewports that are searched.
 * @param rowIndex
 *     Row index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Viewport at the given row and column indices or NULL if not found.
 */
BrainOpenGLViewportContent*
BrainOpenGLViewportContent::findViewportAtRowColumn(std::vector<BrainOpenGLViewportContent*>& viewports,
                                                    const int32_t rowIndex,
                                                    const int32_t columnIndex)
{
    for (std::vector<BrainOpenGLViewportContent*>::iterator iter = viewports.begin();
         iter != viewports.end();
         iter++) {
        BrainOpenGLViewportContent* vp = *iter;
        if ((vp->m_tileTabsRowIndex == rowIndex)
            && (vp->m_tileTabsColumnIndex == columnIndex)) {
            return vp;
        }
    }
    
    return NULL;
}

