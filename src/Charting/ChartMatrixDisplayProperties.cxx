
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __CHART_MATRIX_DISPLAY_PROPERTIES_DECLARE__
#include "ChartMatrixDisplayProperties.h"
#undef __CHART_MATRIX_DISPLAY_PROPERTIES_DECLARE__

#include "AnnotationColorBar.h"
#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartMatrixDisplayProperties 
 * \brief Properites for display of matrix charts.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartMatrixDisplayProperties::ChartMatrixDisplayProperties()
: CaretObject()
{
    m_scaleMode  = ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO;
    m_cellWidth  = 10.0;
    m_cellHeight = 10.0;
    m_colorBarDisplayed = false;
    m_highlightSelectedRowColumn = true;
    m_displayGridLines = true;
    m_colorBar = new AnnotationColorBar(AnnotationAttributesDefaultTypeEnum::NORMAL);

    resetPropertiesToDefault();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_cellWidth", &m_cellWidth);
    m_sceneAssistant->add("m_cellHeight", &m_cellHeight);
    m_sceneAssistant->add("m_viewZooming", &m_viewZooming);
    m_sceneAssistant->addArray("m_viewPanning", m_viewPanning, 2, 0.0);
    m_sceneAssistant->add("m_colorBarDisplayed", &m_colorBarDisplayed);
    m_sceneAssistant->add("m_highlightSelectedRowColumn", &m_highlightSelectedRowColumn);
    m_sceneAssistant->add("m_displayGridLines", &m_displayGridLines);
    m_sceneAssistant->add("m_colorBar", "AnnotationColorBar", m_colorBar);
    
    m_sceneAssistant->add<ChartMatrixScaleModeEnum, ChartMatrixScaleModeEnum::Enum>("m_scaleMode",
                                                                                    &m_scaleMode);
    
}

/**
 * Destructor.
 */
ChartMatrixDisplayProperties::~ChartMatrixDisplayProperties()
{
    delete m_colorBar;
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartMatrixDisplayProperties::ChartMatrixDisplayProperties(const ChartMatrixDisplayProperties& obj)
: CaretObject(obj),
SceneableInterface()
{
    this->copyHelperChartMatrixDisplayProperties(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartMatrixDisplayProperties&
ChartMatrixDisplayProperties::operator=(const ChartMatrixDisplayProperties& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartMatrixDisplayProperties(obj);
    }
    return *this;    
}

/**
 * Reset to the default.
 */
void
ChartMatrixDisplayProperties::resetPropertiesToDefault()
{
    m_viewPanning[0] = 0.0;
    m_viewPanning[1] = 0.0;
    m_viewZooming    = 1.0;
//    m_cellWidth      = 10.0;
//    m_cellHeight     = 10.0;
    setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartMatrixDisplayProperties::copyHelperChartMatrixDisplayProperties(const ChartMatrixDisplayProperties& obj)
{
    m_viewPanning[0] = obj.m_viewPanning[0];
    m_viewPanning[1] = obj.m_viewPanning[1];
    m_viewZooming    = obj.m_viewZooming;
    m_cellWidth      = obj.m_cellWidth;
    m_cellHeight     = obj.m_cellHeight;
    m_scaleMode      = obj.m_scaleMode;
    m_colorBarDisplayed = obj.m_colorBarDisplayed;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartMatrixDisplayProperties::toString() const
{
    return "ChartMatrixDisplayProperties";
}


/**
 * @return Widgth of matrix cell in pixels
 */
float
ChartMatrixDisplayProperties::getCellWidth() const
{
    return m_cellWidth;
}

/**
 * Set width of matrix cell in pixels
 * @param cellWidth
 *    New value for size of matrix cell width in pixels
 */
void
ChartMatrixDisplayProperties::setCellWidth(const float cellWidth)
{
    m_cellWidth = cellWidth;
}

/**
 * @return Height of matrix cell in pixels
 */
float
ChartMatrixDisplayProperties::getCellHeight() const
{
    return m_cellHeight;
}

/**
 * Set height of matrix cell in pixels
 * @param cellHeight
 *    New value for size of matrix cell in pixels
 */
void
ChartMatrixDisplayProperties::setCellHeight(const float cellHeight)
{
    m_cellHeight = cellHeight;
}

/**
 * @return zooming for view of matrix
 */
float
ChartMatrixDisplayProperties::getViewZooming() const
{
    return m_viewZooming;
}

/**
 * Set zooming for view of matrix
 * @param viewZooming
 *    New value for zooming for view of matrix
 */
void
ChartMatrixDisplayProperties::setViewZooming(const float viewZooming)
{
    m_viewZooming = viewZooming;
    setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
}

/**
 * @return panning for view of matrix
 */
void
ChartMatrixDisplayProperties::getViewPanning(float viewPanningOut[2]) const
{
    viewPanningOut[0] = m_viewPanning[0];
    viewPanningOut[1] = m_viewPanning[1];
}

/**
 * Set panning for view of matrix
 * @param viewPanning[2]
 *    New value for panning for view of matrix
 */
void
ChartMatrixDisplayProperties::setViewPanning(const float viewPanning[2])
{
    m_viewPanning[0] = viewPanning[0];
    m_viewPanning[1] = viewPanning[1];
    setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
}

/**
 * @return scale mode for view of matrix
 */
ChartMatrixScaleModeEnum::Enum
ChartMatrixDisplayProperties::getScaleMode() const
{
    return m_scaleMode;
}

/**
 * Set the scale mode.
 *
 * @param scaleMode
 *    New value for scale mode.
 */
void
ChartMatrixDisplayProperties::setScaleMode(const ChartMatrixScaleModeEnum::Enum scaleMode)
{
    m_scaleMode = scaleMode;
}

/**
 * @return The color bar displayed in graphics window.
 */
AnnotationColorBar*
ChartMatrixDisplayProperties::getColorBar()
{
    return m_colorBar;
}

/**
 * @return The color bar displayed in graphics window (const method).
 */
const AnnotationColorBar*
ChartMatrixDisplayProperties::getColorBar() const
{
    return m_colorBar;
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
ChartMatrixDisplayProperties::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartMatrixDisplayProperties",
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
ChartMatrixDisplayProperties::restoreFromScene(const SceneAttributes* sceneAttributes,
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

/**
 * Is the colorbar displayed for the given tab?
 *
 * @return
 *     True if colorbar is displayed, else false.
 */
bool
ChartMatrixDisplayProperties::isColorBarDisplayed() const
{
    return m_colorBarDisplayed;
}

/**
 * Set the colorbar displayed for the given tab.
 *
 * @param displayed
 *     True if colorbar is displayed, else false.
 */
void
ChartMatrixDisplayProperties::setColorBarDisplayed(const bool displayed)
{
    m_colorBarDisplayed = displayed;
}

/**
 * Is the selected row/column highlighted?
 */
bool
ChartMatrixDisplayProperties::isSelectedRowColumnHighlighted() const
{
    return m_highlightSelectedRowColumn;
}

/**
 * Set the selected row/column highlighted status.
 *
 * @param highlightStatus
 *    New status for lighlighting selected row/column.
 */
void
ChartMatrixDisplayProperties::setSelectedRowColumnHighlighted(const bool highlightStatus)
{
    m_highlightSelectedRowColumn = highlightStatus;
}

/**
 * Are the grid lines displayed?
 */
bool
ChartMatrixDisplayProperties::isGridLinesDisplayed() const
{
    return m_displayGridLines;
}

/**
 * Set the grid lines displayed for the given tab.
 *
 * @param displayGridLines
 *     True if grid lines are displayed, else false.
 */
void
ChartMatrixDisplayProperties::setGridLinesDisplayed(const bool displayGridLines)
{
    m_displayGridLines = displayGridLines;
}


