
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
    m_stackingOrder  = geometry->getStackingOrder();
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
    geometryOut->setStackingOrder(m_stackingOrder);
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
 * @return Stacking order (depth in screen) of tab, greater value is 'in front'
 */
int32_t
AnnotationBrowserTab::getStackingOrder() const
{
    return m_stackingOrder;
}

/**
 * Set Stacking order (depth in screen) of tab, greater value is 'in front'
 *
 * @param stackingOrder
 *    New value for Stacking order (depth in screen) of tab, greater value is 'in front'
 */
void
AnnotationBrowserTab::setStackingOrder(const int32_t stackingOrder)
{
    m_stackingOrder = stackingOrder;
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
 */
bool
AnnotationBrowserTab::intersectionTest(const AnnotationBrowserTab* other) const
{
    CaretAssert(other);
    
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

