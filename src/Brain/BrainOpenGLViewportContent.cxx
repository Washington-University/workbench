
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

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "Margin.h"

using namespace caret;


    
/**
 * \class BrainOpenGLViewportContent 
 * \brief Dimensions and model for a viewport in the graphics window.
 *
 * Dimensions and model for a viewport in the graphics window.
 */
/**
 * Constructor.
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
BrainOpenGLViewportContent::BrainOpenGLViewportContent(const int windowViewport[4],
                                                       const int modelViewport[4],
                                                       const int32_t windowIndex,
                                                       const bool highlightTabFlag,
                                                       BrowserTabContent* browserTabContent)
: CaretObject()
{
    initializeMembersBrainOpenGLViewportContent();
    
    m_windowViewport[0] = windowViewport[0];
    m_windowViewport[1] = windowViewport[1];
    m_windowViewport[2] = windowViewport[2];
    m_windowViewport[3] = windowViewport[3];
    
    m_tabViewport[0] = modelViewport[0];
    m_tabViewport[1] = modelViewport[1];
    m_tabViewport[2] = modelViewport[2];
    m_tabViewport[3] = modelViewport[3];
    
    m_modelViewport[0] = modelViewport[0];
    m_modelViewport[1] = modelViewport[1];
    m_modelViewport[2] = modelViewport[2];
    m_modelViewport[3] = modelViewport[3];
    
    if (browserTabContent != NULL) {
        int32_t left = 0, right = 0, bottom = 0, top = 0;
        browserTabContent->getMargin()->getMargins(left, right, bottom, top);
        const int32_t marginHorizSize = (left + right);
        const int32_t marginVertSize  = (bottom + top);
        if ((marginHorizSize < modelViewport[2])
            && (marginVertSize < modelViewport[3])) {
            m_modelViewport[0] = modelViewport[0] + left;
            m_modelViewport[1] = modelViewport[1] + bottom;
            m_modelViewport[2] = modelViewport[2] - marginHorizSize;
            m_modelViewport[3] = modelViewport[3] - marginVertSize;
        }
        else {
            CaretLogSevere("Margins are too big for tab "
                           + AString::number(browserTabContent->getTabNumber() + 1)
                           + " viewport.  Viewport (x,y,w,h)="
                           + AString::fromNumbers(modelViewport, 4, ",")
                           + " margin (l,r,b,t)="
                           + AString::number(left) + ","
                           + AString::number(right) + ","
                           + AString::number(bottom) + ","
                           + AString::number(top));
        }
    }
    
    /*
     * If margins are too big, they could make the viewport invalid
     * so test for it and if the viewport is invalid,
     */
    bool validViewportFlag = true;
    for (int32_t i = 0; i < 4; i++) {
        if (m_modelViewport[i] < 0) {
            validViewportFlag = false;
        }
    }
    if ( ! validViewportFlag) {
        m_modelViewport[0] = modelViewport[0];
        m_modelViewport[1] = modelViewport[1];
        m_modelViewport[2] = modelViewport[2];
        m_modelViewport[3] = modelViewport[3];
    }

    
    m_windowIndex       = windowIndex;
    m_browserTabContent = browserTabContent;
    
    m_highlightTab = highlightTabFlag;
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
: CaretObject(obj)
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
    m_modelViewport[0]  = 0;
    m_modelViewport[1]  = 0;
    m_modelViewport[2]  = 0;
    m_modelViewport[3]  = 0;
    m_tabViewport[0]    = 0;
    m_tabViewport[1]    = 0;
    m_tabViewport[2]    = 0;
    m_tabViewport[3]    = 0;
    m_windowViewport[0] = 0;
    m_windowViewport[1] = 0;
    m_windowViewport[2] = 0;
    m_windowViewport[3] = 0;
    
    m_browserTabContent = NULL;
    m_windowIndex       = -1;
    m_highlightTab      = false;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
BrainOpenGLViewportContent::copyHelperBrainOpenGLViewportContent(const BrainOpenGLViewportContent& obj)
{
    m_modelViewport[0]   = obj.m_modelViewport[0];
    m_modelViewport[1]   = obj.m_modelViewport[1];
    m_modelViewport[2]   = obj.m_modelViewport[2];
    m_modelViewport[3]   = obj.m_modelViewport[3];
    m_tabViewport[0]     = obj.m_tabViewport[0];
    m_tabViewport[1]     = obj.m_tabViewport[1];
    m_tabViewport[2]     = obj.m_tabViewport[2];
    m_tabViewport[3]     = obj.m_tabViewport[3];
    m_windowViewport[0]  = obj.m_windowViewport[0];
    m_windowViewport[1]  = obj.m_windowViewport[1];
    m_windowViewport[2]  = obj.m_windowViewport[2];
    m_windowViewport[3]  = obj.m_windowViewport[3];
    
    m_browserTabContent = obj.m_browserTabContent;
    m_windowIndex       = obj.m_windowIndex;
    m_highlightTab      = obj.m_highlightTab;
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
    modelViewport[0] = m_modelViewport[0];
    modelViewport[1] = m_modelViewport[1];
    modelViewport[2] = m_modelViewport[2];
    modelViewport[3] = m_modelViewport[3];
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
    tabViewportOut[0] = m_tabViewport[0];
    tabViewportOut[1] = m_tabViewport[1];
    tabViewportOut[2] = m_tabViewport[2];
    tabViewportOut[3] = m_tabViewport[3];
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
    windowViewportOut[0] = m_windowViewport[0];
    windowViewportOut[1] = m_windowViewport[1];
    windowViewportOut[2] = m_windowViewport[2];
    windowViewportOut[3] = m_windowViewport[3];
}

//Brain*
//BrainOpenGLViewportContent::getBrain()
//{
//    return m_brain;
//}

int
BrainOpenGLViewportContent::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Pointer to tab content in viewport.
 */
BrowserTabContent* 
BrainOpenGLViewportContent::getBrowserTabContent()
{
    return m_browserTabContent;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLViewportContent::toString() const
{
    return "BrainOpenGLViewportContent";
}

/**
 * Create Viewport Contents for the given tab contents, window sizes, and tile sizes.
 *
 * @paramj tabContents
 *     Content of each tab.
 * @param windowIndex
 *     Index of the window.
 * @param windowWidth
 *     Width of the window.
 * @param windowHeight
 *     Height of the window.
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
                                                             const int32_t windowWidth,
                                                             const int32_t windowHeight,
                                                             const std::vector<int32_t>& rowHeights,
                                                             const std::vector<int32_t>& columnWidths,
                                                             const int32_t highlightTabIndex)
{
    const int32_t numRows = static_cast<int32_t>(rowHeights.size());
    const int32_t numCols = static_cast<int32_t>(columnWidths.size());
    const int32_t numTabs = static_cast<int32_t>(tabContents.size());
    
    std::vector<BrainOpenGLViewportContent*> viewportContentsOut;
    
    /*
     * Arrange models left-to-right and top-to-bottom.
     */
    int32_t vpX = 0;
    int32_t vpY = windowHeight;
    
    int32_t iTab = 0;
    for (int32_t i = 0; i < numRows; i++) {
        const int32_t vpHeight = rowHeights[i];
        vpX = 0;
        vpY -= vpHeight;
        for (int32_t j = 0; j < numCols; j++) {
            const int32_t vpWidth = columnWidths[j];
            if (iTab < numTabs) {
                const int windowViewport[4] = {
                    0,
                    0,
                    windowWidth,
                    windowHeight
                };
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
                new BrainOpenGLViewportContent(windowViewport,
                                               modelViewport,
                                               highlightTab,
                                               windowIndex,
                                               tabContent);
                viewportContentsOut.push_back(vc);
            }
            iTab++;
            vpX += vpWidth;
            
            if (iTab >= numTabs) {
                /*
                 * More cells than models for drawing so set loop
                 * indices so that loops terminate
                 */
                j = numCols;
                i = numRows;
            }
        }
    }

    return viewportContentsOut;
}


