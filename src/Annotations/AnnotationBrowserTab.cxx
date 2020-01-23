
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
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::BROWSER_TAB,
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
: AnnotationTwoDimensionalShape(obj)
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
        AnnotationTwoDimensionalShape::operator=(obj);
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
    AnnotationTwoDimensionalShape::saveSubClassDataToScene(sceneAttributes,
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
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
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

    /**
     * Set grid points on for the given range
     */
    void setRange(const int32_t minXIn,
                  const int32_t maxXIn,
                  const int32_t minYIn,
                  const int32_t maxYIn) {
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
AnnotationBrowserTab::expandTab(const std::vector<const AnnotationBrowserTab*>& browserTabsInWindow,
                                const AnnotationBrowserTab* tabToExpand,
                                float boundsOut[4])
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
    
//    grid.setRange(tabMinX, tabMaxX, tabMinY, tabMaxY);
    /*
     * Shrink by 1 point around edges.  If the maximum of a tab
     * is the same as a minimum of this tab, it will detect
     * overlap and prevent expansion
     */
    int32_t x1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinX + 1), 0, 100);
    int32_t x2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxX - 1), 0, 100);
    int32_t y1 = MathFunctions::limitRange(static_cast<int32_t>(tabMinY + 1), 0, 100);
    int32_t y2 = MathFunctions::limitRange(static_cast<int32_t>(tabMaxY - 1), 0, 100);
    grid.setRange(x1, x2, y1, y2);

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
         * Try to expand at bottom by one row
         */
        if ( ! bottomDone) {
            if (grid.isRangeOff(xl, xr, yb - 1, yb - 1)) {
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
            if (grid.isRangeOff(xl, xr, yt + 1, yt + 1)) {
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
            if (grid.isRangeOff(xl - 1, xl - 1, yb, yt)) {
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
            if (grid.isRangeOff(xr + 1, xr + 1, yb, yt)) {
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
        if (grid.isRangeOff(xl, xl, yb, yt)) {       // left
            x1 = std::max(x1 - 1, 0);
        }
        if (grid.isRangeOff(xr, xr, yb, yt)) {   // right
            x2 = std::min(x2 + 1, 100);
        }
        if (grid.isRangeOff(xl, xr, yb, yb)) {    // bottom
            y1 = std::max(y1 - 1, 0);
        }
        if (grid.isRangeOff(xl, xr, yt, yt)) { // top
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

