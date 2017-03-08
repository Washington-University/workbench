
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

#define __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_DECLARE__
#include "ChartTwoMatrixDisplayProperties.h"
#undef __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoMatrixDisplayProperties 
 * \brief Display properties for version two of Matrix charting.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoMatrixDisplayProperties::ChartTwoMatrixDisplayProperties()
: CaretObject()
{
    initializeInstance();
}

/**
 * Destructor.
 */
ChartTwoMatrixDisplayProperties::~ChartTwoMatrixDisplayProperties()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Initialize a new instance of this class.
 */
void
ChartTwoMatrixDisplayProperties::initializeInstance()
{
    resetPropertiesToDefault();
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_cellPercentageZoomWidth",
                          &m_cellPercentageZoomWidth);
    m_sceneAssistant->add("m_cellPercentageZoomHeight",
                          &m_cellPercentageZoomHeight);
    m_sceneAssistant->add("m_displayGridLinesFlag",
                          &m_displayGridLinesFlag);
    m_sceneAssistant->add("m_highlightSelectedRowColumnFlag",
                          &m_highlightSelectedRowColumnFlag);
    
    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoMatrixDisplayProperties::ChartTwoMatrixDisplayProperties(const ChartTwoMatrixDisplayProperties& obj)
: CaretObject(obj)
{
    initializeInstance();
    
    this->copyHelperChartTwoMatrixDisplayProperties(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoMatrixDisplayProperties&
ChartTwoMatrixDisplayProperties::operator=(const ChartTwoMatrixDisplayProperties& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoMatrixDisplayProperties(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoMatrixDisplayProperties::copyHelperChartTwoMatrixDisplayProperties(const ChartTwoMatrixDisplayProperties& obj)
{
    m_cellPercentageZoomWidth        = obj.m_cellPercentageZoomWidth;
    m_cellPercentageZoomHeight       = obj.m_cellPercentageZoomHeight;
    m_displayGridLinesFlag           = obj.m_displayGridLinesFlag;
    m_highlightSelectedRowColumnFlag = obj.m_highlightSelectedRowColumnFlag;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoMatrixDisplayProperties::toString() const
{
    return "ChartTwoMatrixDisplayProperties";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoMatrixDisplayProperties::receiveEvent(Event* event)
{
}

/**
 * Reset the display properties to their default values.
 */
void
ChartTwoMatrixDisplayProperties::resetPropertiesToDefault()
{
    m_cellPercentageZoomWidth        = 100.0;
    m_cellPercentageZoomHeight       = 100.0;
    m_highlightSelectedRowColumnFlag = true;
    m_displayGridLinesFlag           = false;
}

/**
 * @return Display grid lines status.
 */
bool
ChartTwoMatrixDisplayProperties::isGridLinesDisplayed() const
{
    return m_displayGridLinesFlag;
}

/**
 * Set the grid lines display status.
 *
 * @param displayGridLines
 *     New status for display of grid lines.
 */
void
ChartTwoMatrixDisplayProperties::setGridLinesDisplayed(const bool displayGridLines)
{
    m_displayGridLinesFlag = displayGridLines;
}

/**
 * @return highlight selected row/column status.
 */
bool
ChartTwoMatrixDisplayProperties::isSelectedRowColumnHighlighted() const
{
    return m_highlightSelectedRowColumnFlag;
}

/**
 * Set the row/column highlighting status.
 *
 * @param highlightStatus
 *     New highlighting status.
 */
void
ChartTwoMatrixDisplayProperties::setSelectedRowColumnHighlighted(const bool highlightStatus)
{
    m_highlightSelectedRowColumnFlag = highlightStatus;
}

/**
 * @return The cell percentage zoom width.
 */
float
ChartTwoMatrixDisplayProperties::getCellPercentageZoomWidth() const
{
    return m_cellPercentageZoomWidth;
}

/**
 * Set the cell percentage zoom width.
 *
 * @param cellPercentageZoomWidth
 *     New value for cell percentage zoom width.
 */
void
ChartTwoMatrixDisplayProperties::setCellPercentageZoomWidth(const float cellPercentageZoomWidth)
{
    m_cellPercentageZoomWidth = cellPercentageZoomWidth;
}

/**
 * @return The cell percentage zoom height.
 */
float
ChartTwoMatrixDisplayProperties::getCellPercentageZoomHeight() const
{
    return m_cellPercentageZoomHeight;
}

/**
 * Set the cell percentage zoom height.
 *
 * @param cellPercentageZoomHeight
 *     New value for cell percentage zoom height.
 */
void ChartTwoMatrixDisplayProperties::setCellPercentageZoomHeight(const float cellPercentageZoomHeight)
{
    m_cellPercentageZoomHeight = cellPercentageZoomHeight;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartTwoMatrixDisplayProperties::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoMatrixDisplayProperties",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartTwoMatrixDisplayProperties::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

