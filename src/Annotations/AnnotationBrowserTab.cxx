
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_BROWSER_TAB_DECLARE__
#include "AnnotationBrowserTab.h"
#undef __ANNOTATION_BROWSER_TAB_DECLARE__

#include <cmath>

#include "AnnotationCoordinate.h"
#include "CaretAssert.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "TileTabsBrowserTabGeometry.h"

using namespace caret;


    
/**
 * \class caret::AnnotationBrowserTab
 * \brief An annotation box.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationBrowserTab::AnnotationBrowserTab(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationOneCoordinateShape(AnnotationTypeEnum::BROWSER_TAB,
                                attributeDefaultType)
{
    initializeMembersAnnotationBrowserTab();
}

/**
 * Destructor.
 */
AnnotationBrowserTab::~AnnotationBrowserTab()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationBrowserTab::AnnotationBrowserTab(const AnnotationBrowserTab& obj)
: AnnotationOneCoordinateShape(obj)
{
    this->initializeMembersAnnotationBrowserTab();
    this->copyHelperAnnotationBrowserTab(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationBrowserTab&
AnnotationBrowserTab::operator=(const AnnotationBrowserTab& obj)
{
    if (this != &obj) {
        AnnotationOneCoordinateShape::operator=(obj);
        this->copyHelperAnnotationBrowserTab(obj);
    }
    return *this;
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationBrowserTab::initializeMembersAnnotationBrowserTab()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
        
    }
    
    setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationBrowserTab::copyHelperAnnotationBrowserTab(const AnnotationBrowserTab& /*obj*/)
{
    /* nothing to copy here */
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
AnnotationBrowserTab::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationOneCoordinateShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
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
AnnotationBrowserTab::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationOneCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Set the browser tab and index that owns this annotation
 *
 * @param browserTabContent
 *     Browser tab that owns this annotation
 * @param tabIndex
 *     Index of the browser tab
 */
void
AnnotationBrowserTab::setBrowserTabContent(BrowserTabContent* browserTabContent,
                                           const int32_t tabIndex)
{
    m_browserTabContent = browserTabContent;
    m_tabIndex          = tabIndex;
}

/**
 * @return The browser tab that owns this annotation
 */
BrowserTabContent*
AnnotationBrowserTab::getBrowserTabContent()
{
    return m_browserTabContent;
}

/**
 * @return The browser tab that owns this annotation (const method)
 */
const BrowserTabContent*
AnnotationBrowserTab::getBrowserTabContent() const
{
    return m_browserTabContent;
}

/**
 * @return Index of browser tab that owns this annotation
 */
int32_t
AnnotationBrowserTab::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return Display status of tab
 */
bool
AnnotationBrowserTab::isBrowserTabDisplayed() const
{
    return m_displayStatus;
}

/**
 * Set the display status of this tab
 *
 * @param status
 *     New display status for this tab
 */
void
AnnotationBrowserTab::setBrowserTabDisplayed(const bool status) 
{
    m_displayStatus = status;
}

/**
 * Set the browser tab annotation dimensions using the tile tabs geometry
 *
 * @param geometry
 *     The tile tabs geometry
 */
void
AnnotationBrowserTab::setFromTileTabsGeometry(const TileTabsBrowserTabGeometry* geometry)
{
    CaretAssert(geometry);
    setBounds2D(geometry->getMinX(),
                geometry->getMaxX(),
                geometry->getMinY(),
                geometry->getMaxY());
    m_displayStatus  = geometry->isDisplayed();
    setStackingOrder(geometry->getStackingOrder());
    m_backgroundType = geometry->getBackgroundType();
}

/**
 * Load the browser tab annotation dimensions into the tile tabs geometry
 *
 * @param geometry
 *     The tile tabs geometry
 */
void
AnnotationBrowserTab::getTileTabsGeometry(TileTabsBrowserTabGeometry* geometryOut) const
{
    CaretAssert(geometryOut);
    
    float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
    getBounds2D(minX, maxX, minY, maxY);
    
    geometryOut->setBounds(minX, maxX, minY, maxY);
    geometryOut->setDisplayed(m_displayStatus);
    geometryOut->setStackingOrder(getStackingOrder());
    geometryOut->setBackgroundType(m_backgroundType);
}

/**
 * Get the 2D bounds (note: BrowserTab is always aligned with X/Y axes)
 *
 * @param minX
 *     Output with minimum X
 * @param maxX
 *     Output with maximum X
 * @param minY
 *     Output with minimum X
 * @param maxY
 *     Output with maximum X
 */
void
AnnotationBrowserTab::getBounds2D(float& minX,
                                  float& maxX,
                                  float& minY,
                                  float& maxY) const
{
    float xyz[3];
    getCoordinate()->getXYZ(xyz);
    
    minX = xyz[0] - (getWidth() / 2.0);
    maxX = minX + getWidth();
    minY = xyz[1] - (getHeight() / 2.0);
    maxY = minY + getHeight();
}

/**
 * Set the 2D bounds (note: BrowserTab is always aligned with X/Y axes)
 *
 * @param minX
 *     The minimum X
 * @param maxX
 *     The maximum X
 * @param minY
 *     The minimum X
 * @param maxY
 *     The maximum X
 */
void
AnnotationBrowserTab::setBounds2D(const float minX,
                                  const float maxX,
                                  const float minY,
                                  const float maxY)
{
    const float centerX = (minX + maxX) / 2.0;
    const float centerY = (minY + maxY) / 2.0;
    const float width   = (maxX - minX);
    const float height  = (maxY - minY);
    
    getCoordinate()->setXYZ(centerX, centerY, 0.0);
    setWidth(width);
    setHeight(height);
}

/**
 * @return Type of background (opaque / transparent) for tab
 */
TileTabsLayoutBackgroundTypeEnum::Enum
AnnotationBrowserTab::getBackgroundType() const
{
    return m_backgroundType;
}

/**
 * Set Type of background (opaque / transparent) for tab
 *
 * @param backgroundType
 *    New value for Type of background (opaque / transparent) for tab
 */
void
AnnotationBrowserTab::setBackgroundType(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType)
{
    m_backgroundType = backgroundType;
}

/**
 * @return true if this and the given geometry intersect.
 *         NOTE: if 'other' is 'this' true is returned (overlaps self) but this
 *         could change so it is best to avoid testing overlap of self.
 *         NOTE: Display status is ignored
 *
 * @param other
 *     Other geometry for intersection test
 * @param windowIndex
 *     Index of window
 */
bool
AnnotationBrowserTab::intersectionTest(const Annotation* otherAnn,
                                       const int32_t /*windowIndex*/) const
{
    CaretAssert(otherAnn);
    const AnnotationBrowserTab* other = dynamic_cast<const AnnotationBrowserTab*>(otherAnn);
    if (other == NULL) {
        return false;
    }
    
    /*
     * Does self overlap
     */
    if (this == other) {
        return true;
    }
    
    float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
    getBounds2D(minX, maxX, minY, maxY);

    float otherMinX(0.0), otherMaxX(0.0), otherMinY(0.0), otherMaxY(0.0);
    other->getBounds2D(otherMinX, otherMaxX, otherMinY, otherMaxY);
    
    /*
     * Note: Since the geometry is aligned with the X- and Y-axes,
     * we only need to test for one to be above or the the right of the other
     *
     * https://www.geeksforgeeks.org/find-two-rectangles-overlap/
     * https://leetcode.com/articles/rectangle-overlap/
     */
    /* 'this' is on right side of 'other' */
    if (minX >= otherMaxX) {
        return false;
    }
    
    /* 'other' is on right side of 'this' */
    if (otherMinX >= maxX) {
        return false;
    }
    
    /* 'this' is above 'other */
    if (minY >= otherMaxY) {
        return false;
    }
    
    /* 'other' is above 'this' */
    if (otherMinY >= maxY) {
        return false;
    }
    
    return true;
}

/**
 * This class is used to find an expandable region
 * for a browser tab
 */
class PercentageGrid {
public:
    /*
     * Constructor with grid size
     */
    PercentageGrid(const int32_t minXY,
                   const int32_t maxXY)
    : m_minXY(minXY),
    m_maxXY(maxXY),
    m_rangeXY((maxXY - minXY) + 1) /* Add 1 since inclusive range */
    {
        const int32_t gridSize(m_rangeXY * m_rangeXY);
        m_grid.resize(gridSize, 0);
    }
    
    /*
     * Test to see of ALL of the grid points with the given bounds
     * are off
     */
    bool isRangeOff(const int32_t minX,
                    const int32_t maxX,
                    const int32_t minY,
                    const int32_t maxY) const {
        for (int32_t i = minX; i <= maxX; i++) {
            for (int32_t j = minY; j <= maxY; j++) {
                const int32_t offset = (m_rangeXY * j) + i;
                CaretAssertVectorIndex(m_grid, offset);
                if (m_grid[offset] != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    /*
     * Test to see of ALL of the grid points with the given bounds
     * are off
     */
    bool isRangeAllOn(const int32_t minX,
                    const int32_t maxX,
                    const int32_t minY,
                    const int32_t maxY) const {
        for (int32_t i = minX; i <= maxX; i++) {
            for (int32_t j = minY; j <= maxY; j++) {
                const int32_t offset = (m_rangeXY * j) + i;
                CaretAssertVectorIndex(m_grid, offset);
                if (m_grid[offset] == 0) {
                    return false;
                }
            }
        }
        return true;
    }
    
    /*
     * Test to see of ANY of the grid points with the given bounds
     * are on
     */
    bool isRangeOn(const int32_t minX,
                    const int32_t maxX,
                    const int32_t minY,
                    const int32_t maxY) const {
        for (int32_t i = minX; i <= maxX; i++) {
            for (int32_t j = minY; j <= maxY; j++) {
                const int32_t offset = (m_rangeXY * j) + i;
                CaretAssertVectorIndex(m_grid, offset);
                if (m_grid[offset] != 0) {
                    return true;
                }
            }
        }
        return false;
    }
    
    /**
     * Set grid points on for the given range
     */
    void setRange(float minXIn,
                  float maxXIn,
                  float minYIn,
                  float maxYIn) {
        /*
         * Tile Layout sizes are floating point with one decimal.  Grid is integer so need
         * to account for any fractional values when converting to integer.
         */
        minXIn = std::floor(minXIn);
        maxXIn = std::ceil(maxXIn);
        minYIn = std::floor(minYIn);
        maxYIn = std::ceil(maxYIn);
        
        const int32_t minX = MathFunctions::limitRange(static_cast<int32_t>(minXIn), m_minXY, m_maxXY);
        const int32_t maxX = MathFunctions::limitRange(static_cast<int32_t>(maxXIn), m_minXY, m_maxXY);
        const int32_t minY = MathFunctions::limitRange(static_cast<int32_t>(minYIn), m_minXY, m_maxXY);
        const int32_t maxY = MathFunctions::limitRange(static_cast<int32_t>(maxYIn), m_minXY, m_maxXY);

        for (int32_t i = minX; i <= maxX; i++) {
            for (int32_t j = minY; j <= maxY; j++) {
                const int32_t offset = (m_rangeXY * j) + i;
                CaretAssertVectorIndex(m_grid, offset);
                m_grid[offset] = 1;
            }
        }
    }
    
    const int32_t m_minXY;
    const int32_t m_maxXY;
    const int32_t m_rangeXY;
    
    std::vector<int32_t> m_grid;
};

/**
 * Expand the given tab using any available space around it
 *
 * @param browserTabsInWindow
 *      Tabs in the window
 * @param tabToExpand
 *      Tab that is expanded to fill empty space
 * @param boundsOut
 *      Output with new bounds
 * @return
 *      True if new bounds are valid
 */
bool
AnnotationBrowserTab::shrinkAndExpandToFillEmptySpace(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                                      const AnnotationBrowserTab* tabToExpand,
                                                      std::array<float, 4>& boundsOut)
{
    float origBounds[4];
    tabToExpand->getBounds2D(origBounds[0], origBounds[1], origBounds[2], origBounds[3]);
    
    /*
     * First, try to shrink the tab so that it does not
     * overlap any other tabs
     */
    std::array<float, 4> shrinkBounds;
    shrinkBounds.fill(0.0);
    const bool shrinkResultFlag = shrinkTab(browserTabsInWindow,
                                            tabToExpand,
                                            shrinkBounds);

    /*
     * Second, try to expand the tab to fill any extra space
     */
    std::array<float, 4> expandBounds;
    expandBounds.fill(0.0);
    const bool expandResultFlag = expandTab(browserTabsInWindow,
                                            tabToExpand,
                                            shrinkBounds,
                                            shrinkResultFlag,
                                            expandBounds);

    bool resultFlag(false);
    if (expandResultFlag) {
        boundsOut = expandBounds;
        resultFlag = true;
    }
    else if (shrinkResultFlag) {
        boundsOut = shrinkBounds;
        resultFlag = true;
    }
    
    /*
     * Verify that bounds have changed
     */
    if (resultFlag) {
        resultFlag = false;
        for (int32_t i = 0; i < 4; i++) {
            if (origBounds[i] != boundsOut[i]) {
                resultFlag = true;
                break;
            }
        }
    }
    
    return resultFlag;
}


/**
 * Expand the given tab using any available space around it
 *
 * @param browserTabsInWindow
 *      Tabs in the window
 * @param tabToExpand
 *      Tab that is expanded to fill empty space
 * @param boundsOut
 *      Output with new bounds
 * @return
 *      True if new bounds are valid
 */
bool
AnnotationBrowserTab::expandTab(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                const AnnotationBrowserTab* tabToExpand,
                                const std::array<float, 4>& boundsIn,
                                const bool boundsInValidFlag,
                                std::array<float, 4>& boundsOut)
{
    if (tabToExpand == NULL) {
        return false;
    }
    
    /*
     * Ensure tab for expansion is not in list of other tabs
     */
    std::vector<const AnnotationBrowserTab*> tabs;
    for (auto bt : browserTabsInWindow) {
        if (bt != tabToExpand) {
            tabs.push_back(bt);
        }
    }
    
    /*
     * If no other tabs, fill window
     */
    if (tabs.empty()) {
        boundsOut[0] =   0.0;
        boundsOut[1] = 100.0;
        boundsOut[2] =   0.0;
        boundsOut[3] = 100.0;
        return true;
    }
    
    /*
     * Since the percentage coordinates in the window range [0, 100],
     * create a grid that is 101 x 101 (grid will add one in constructor)
     */
    PercentageGrid grid(0, 100);
    
    /*
     * Fill in grid points that are overlapped by
     * all of the tabs
     */
    for (auto t : tabs) {
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        t->getBounds2D(minX, maxX, minY, maxY);
        grid.setRange(minX, maxX, minY, maxY);
    }
    
    /*
     * Get bounds of tab for expansion and clip to grid
     */
    float tabMinX(0.0), tabMaxX(0.0), tabMinY(0.0), tabMaxY(0.0);
    tabToExpand->getBounds2D(tabMinX, tabMaxX, tabMinY, tabMaxY);
    
    /*
     * Override tab's bounds if input bounds are valid
     */
    if (boundsInValidFlag) {
        tabMinX = boundsIn[0];
        tabMaxX = boundsIn[1];
        tabMinY = boundsIn[2];
        tabMaxY = boundsIn[3];
    }
    
    /*
     * Limit the tab to the valid range [0, 100].
     * Optionally shrink around edges.  If the maximum of a tab
     * is the same as a minimum of this tab, it will detect
     * overlap and prevent expansion
     */
    const int32_t shrinkFactor(0);
    int32_t x1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinX + shrinkFactor), 0, 100);
    int32_t x2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxX - shrinkFactor), 0, 100);
    int32_t y1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinY + shrinkFactor), 0, 100);
    int32_t y2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxY - shrinkFactor), 0, 100);

    bool leftDone(x1 <= 0);
    bool rightDone(x2 >= 100);
    bool bottomDone(y1 <= 0);
    bool topDone(y2 >= 100);
    
    /*
     * When expanding an edge, we want to exclude the "corners" since they may overlap
     * the edge of another tab.  For example, when expanding to the right, the bottom
     * may be at Y=50 and the tab below ends at Y=50 and this will prevent expansion to
     * the right.  So, by excluding these corners, it allows the tab to expand to the right.
     */
    const int32_t cornerOffset(1);
    
    bool done(leftDone && rightDone && bottomDone && topDone);
    while ( ! done) {
        const int32_t xl(x1);
        const int32_t xr(x2);
        const int32_t yb(y1);
        const int32_t yt(y2);
        
        const int32_t cornerOffset(1);
        
        /*
         * Try to expand at bottom by one row
         */
        if ( ! bottomDone) {
            if (grid.isRangeOff(xl + cornerOffset, xr - cornerOffset, yb - 1, yb - 1)) {
                y1--;
            }
            else {
                bottomDone = true;
            }
        }
        
        /*
         * Try to expand at top by one row
         */
        if ( ! topDone) {
            if (grid.isRangeOff(xl + cornerOffset, xr - cornerOffset, yt + 1, yt + 1)) {
                y2++;
            }
            else {
                topDone = true;
            }
        }

        /*
         * Try to expand at left by one column
         */
        if ( ! leftDone) {
            if (grid.isRangeOff(xl - 1, xl - 1, yb + cornerOffset, yt - cornerOffset)) {
                x1--;
            }
            else {
                leftDone = true;
            }
        }
        
        /*
         * Try to expand at right by one column
         */
        if ( ! rightDone) {
            if (grid.isRangeOff(xr + 1, xr + 1, yb + cornerOffset, yt - cornerOffset)) {
                x2++;
            }
            else {
                rightDone = true;
            }
        }
        
        /*
         * Test for any sides that no longer are expandable
         */
        if (x1 <= 0) {
            leftDone = true;
        }
        if (x2 >= 100) {
            rightDone = true;
        }
        if (y1 <= 0) {
            bottomDone = true;
        }
        if (y2 >= 100) {
            topDone = true;
        }
        
        /*
         * Done if no expansion available
         */
        done = (leftDone && rightDone && bottomDone && topDone);
    }
    
    /*
     * When ON, an edge of the tab will expand until it overlaps the edge of the neighboring tab
     * When OFF, an edge will move to one row or column from the neighboring tab
     */
    const bool allowOverlapFlag(true);
    if (allowOverlapFlag) {
        /*
         * Move each edge to overlap adjacent tab(s)
         */
        const int32_t xl(x1);
        const int32_t xr(x2);
        const int32_t yb(y1);
        const int32_t yt(y2);
        if (grid.isRangeOff(xl, xl, yb + cornerOffset, yt - cornerOffset)) {   /* left */
            x1 = std::max(x1 - 1, 0);
        }
        if (grid.isRangeOff(xr, xr, yb + cornerOffset, yt - cornerOffset)) {   /* right */
            x2 = std::min(x2 + 1, 100);
        }
        if (grid.isRangeOff(xl + cornerOffset, xr - cornerOffset, yb, yb)) {   /* bottom */
            y1 = std::max(y1 - 1, 0);
        }
        if (grid.isRangeOff(xl + cornerOffset, xr - cornerOffset, yt, yt)) {   /* top */
            y2 = std::min(y2 + 1, 100);
        }
    }
    
    /*
     * If expansion available, report new bounds
     */
    if ((x1 < tabMinX)
        || (x2 > tabMaxX)
        || (y1 < tabMinY)
        || (y2 > tabMaxY)) {
        boundsOut[0] = x1;
        boundsOut[1] = x2;
        boundsOut[2] = y1;
        boundsOut[3] = y2;
        return true;
    }
    
    return false;
}

/**
 * Shrink the given tab until it does not overlay any other tabs
 *
 * @param browserTabsInWindow
 *      Tabs in the window
 * @param tabToShrink
 *      Tab that is expanded to fill empty space
 * @param boundsOut
 *      Output with new bounds
 * @return
 *      True if new bounds are valid
 */
bool
AnnotationBrowserTab::shrinkTab(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                const AnnotationBrowserTab* tabToShrink,
                                std::array<float, 4>& boundsOut)
{
    int32_t bounds[4] { 0, 100, 0, 100 };
    
    /*
     * Two passes are performed to minimize shrinking of the tab
     */
    
    /*
     * First time, erode when an ENTIRE edge overlaps other tab(s)
     */
    if ( ! shrinkTabAux(browserTabsInWindow, tabToShrink, bounds, true, boundsOut)) {
        return false;
    }
    
    bounds[0] = boundsOut[0];
    bounds[1] = boundsOut[1];
    bounds[2] = boundsOut[2];
    bounds[3] = boundsOut[3];
    
    /*
     * Second pass, erode an edge when ANY point in edge overlaps other tab(s)
     */
    shrinkTabAux(browserTabsInWindow, tabToShrink, bounds, false, boundsOut);
  
    return true;
}

/**
 * Shrink the given tab until it does not overlay any other tabs
 *
 * @param browserTabsInWindow
 *      Tabs in the window
 * @param tabToShrink
 *      Tab that is expanded to fill empty space
 * @param startingBounds
 *      Starting bounds for tab
 * @param testAllOnFlag
 *      If true test for all voxels on along edge, else any voxel on along edge
 * @param boundsOut
 *      Output with new bounds
 * @return
 *      True if new bounds are valid
 */
bool
AnnotationBrowserTab::shrinkTabAux(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                   const AnnotationBrowserTab* tabToShrink,
                                   const int32_t startingBounds[4],
                                   const bool testAllOnFlag,
                                   std::array<float, 4>& boundsOut)
{
    if (tabToShrink == NULL) {
        return false;
    }
    
    /*
     * Ensure tab for expansion is not in list of other tabs
     */
    std::vector<const AnnotationBrowserTab*> tabs;
    for (auto bt : browserTabsInWindow) {
        if (bt != tabToShrink) {
            tabs.push_back(bt);
        }
    }
    
    /*
     * If no other tabs, do not change siz
     */
    if (tabs.empty()) {
        return false;
    }
    
    /*
     * Since the percentage coordinates in the window range [0, 100],
     * create a grid that is 101 x 101 (grid will add one in constructor)
     */
    PercentageGrid grid(0, 100);
    
    /*
     * Fill in grid points that are overlapped by
     * all of the tabs
     */
    for (auto t : tabs) {
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        t->getBounds2D(minX, maxX, minY, maxY);
        grid.setRange(minX, maxX, minY, maxY);
    }
    
    /*
     * Get bounds of tab for expansion and clip to grid
     */
    float tabMinX(0.0), tabMaxX(0.0), tabMinY(0.0), tabMaxY(0.0);
    tabToShrink->getBounds2D(tabMinX, tabMaxX, tabMinY, tabMaxY);
    
    /*
     * Shrink by 1 point around edges.  If the maximum of a tab
     * is the same as a minimum of this tab, it will detect
     * overlap and prevent expansion
     */
    int32_t x1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinX), startingBounds[0], startingBounds[1]);
    int32_t x2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxX), startingBounds[0], startingBounds[1]);
    int32_t y1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinY), startingBounds[2], startingBounds[3]);
    int32_t y2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxY), startingBounds[2], startingBounds[3]);
    
    bool leftDone(x1 <= 0);
    bool rightDone(x2 >= 100);
    bool bottomDone(y1 <= 0);
    bool topDone(y2 >= 100);
    
    bool done(leftDone && rightDone && bottomDone && topDone);
    while ( ! done) {
        const int32_t xl(x1);
        const int32_t xr(x2);
        const int32_t yb(y1);
        const int32_t yt(y2);
        
        /*
         * Try to shrink at bottom by one row
         */
        if ( ! bottomDone) {
            if (testAllOnFlag) {
                if (grid.isRangeAllOn(xl, xr, yb, yb)) {
                    y1++;
                }
                else {
                    bottomDone = true;
                }
            }
            else if (grid.isRangeOn(xl, xr, yb, yb)) {
                y1++;
            }
            else {
                bottomDone = true;
            }
        }
        
        /*
         * Try to shrink at top by one row
         */
        if ( ! topDone) {
            if (testAllOnFlag) {
                if (grid.isRangeAllOn(xl, xr, yt, yt)) {
                    y2--;
                }
                else {
                    topDone = true;
                }
            }
            else if (grid.isRangeOn(xl, xr, yt, yt)) {
                y2--;
            }
            else {
                topDone = true;
            }
        }
        
        /*
         * Try to shrink at left by one column
         */
        if ( ! leftDone) {
            if (testAllOnFlag) {
                if (grid.isRangeAllOn(xl, xl, yb, yt)) {
                    x1++;
                }
                else {
                    leftDone = true;
                }
            }
            else if (grid.isRangeOn(xl, xl, yb, yt)) {
                x1++;
            }
            else {
                leftDone = true;
            }
        }
        
        /*
         * Try to shrink at right by one column
         */
        if ( ! rightDone) {
            if (testAllOnFlag) {
                if (grid.isRangeAllOn(xr, xr, yb, yt)) {
                    x2--;
                }
                else {
                    rightDone = true;
                }
            }
            else if (grid.isRangeOn(xr, xr, yb, yt)) {
                x2--;
            }
            else {
                rightDone = true;
            }
        }
        
        /*
         * Test for any sides that no longer are expandable
         */
        if (x1 >= 100) {
            leftDone = true;
        }
        if (x2 <= 0) {
            rightDone = true;
        }
        if (y1 >= 100) {
            bottomDone = true;
        }
        if (y2 <= 0) {
            topDone = true;
        }
        if (x1 >= x2) {
            leftDone = true;
            rightDone = true;
        }
        if (y1 >= y2) {
            bottomDone = true;
            topDone    = true;
        }
        
        /*
         * Done if no shrinking available
         */
        done = (leftDone && rightDone && bottomDone && topDone);
    }
    
    /*
     * If tab collapsed to empty, then no changes
     */
    if (x1 >= x2) {
        return false;
    }
    if (y1 >= y2) {
        return false;
    }

    /*
     * If shrinking was performed, report new bounds
     */
    if ((x1 > tabMinX)
        || (x2 < tabMaxX)
        || (y1 > tabMinY)
        || (y2 < tabMaxY)) {
        boundsOut[0] = x1;
        boundsOut[1] = x2;
        boundsOut[2] = y1;
        boundsOut[3] = y2;
        return true;
    }
    
    return false;
}
