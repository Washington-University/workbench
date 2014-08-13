
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
    m_matrixLoadingType = ChartMatrixLoadingTypeEnum::CHART_MATRIX_LOAD_BY_ROW;
    m_yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    m_scaleMode  = ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO;
    m_cellWidth  = 10.0;
    m_cellHeight = 10.0;
    m_colorBarDisplayed = false;
    resetPropertiesToDefault();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_cellWidth", &m_cellWidth);
    m_sceneAssistant->add("m_cellHeight", &m_cellHeight);
    m_sceneAssistant->add("m_viewZooming", &m_viewZooming);
    m_sceneAssistant->addArray("m_viewPanning", m_viewPanning, 2, 0.0);
    m_sceneAssistant->add("m_colorBarDisplayed", &m_colorBarDisplayed);
    
    m_sceneAssistant->add<ChartMatrixLoadingTypeEnum, ChartMatrixLoadingTypeEnum::Enum>("m_matrixLoadingType",
                                                                                        &m_matrixLoadingType);
    m_sceneAssistant->add<YokingGroupEnum, YokingGroupEnum::Enum>("m_yokingGroup",
                                                                  &m_yokingGroup);
    
    m_sceneAssistant->add<ChartMatrixScaleModeEnum, ChartMatrixScaleModeEnum::Enum>("m_scaleMode",
                                                                                    &m_scaleMode);
    
}

/**
 * Destructor.
 */
ChartMatrixDisplayProperties::~ChartMatrixDisplayProperties()
{
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
    m_matrixLoadingType = obj.m_matrixLoadingType;
    m_yokingGroup    = obj.m_yokingGroup;
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
 * @return The matrix loading type (by row/column).
 */
ChartMatrixLoadingTypeEnum::Enum
ChartMatrixDisplayProperties::getMatrixLoadingType() const
{
    return m_matrixLoadingType;
}

/**
 * Set the matrix loading type (by row/column).
 *
 * @param matrixLoadingType
 *    New value for matrix loading type.
 */
void
ChartMatrixDisplayProperties::setMatrixLoadingType(const ChartMatrixLoadingTypeEnum::Enum matrixLoadingType)
{
    m_matrixLoadingType = matrixLoadingType;
}

/**
 * @return Selected yoking group.
 */
YokingGroupEnum::Enum
ChartMatrixDisplayProperties::getYokingGroup() const
{
    return m_yokingGroup;
}

/**
 * Set the selected yoking group.
 *
 * @param yokingGroup
 *    New value for yoking group.
 */
void
ChartMatrixDisplayProperties::setYokingGroup(const YokingGroupEnum::Enum yokingGroup)
{
    m_yokingGroup = yokingGroup;
    
    if (m_yokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
//    /*
//     * Find another browser tab using the same yoking as 'me' and copy
//     * yoked data from the other browser tab.
//     */
//    for (std::set<BrowserTabContent*>::iterator iter = s_allBrowserTabContent.begin();
//         iter != s_allBrowserTabContent.end();
//         iter++) {
//        BrowserTabContent* btc = *iter;
//        if (btc != this) {
//            if (btc->getYokingGroup() == m_yokingGroup) {
//                *m_viewingTransformation = *btc->m_viewingTransformation;
//                *m_flatSurfaceViewingTransformation = *btc->m_flatSurfaceViewingTransformation;
//                *m_cerebellumViewingTransformation = *btc->m_cerebellumViewingTransformation;
//                *m_volumeSliceViewingTransformation = *btc->m_volumeSliceViewingTransformation;
//                *m_volumeSliceSettings = *btc->m_volumeSliceSettings;
//                *m_obliqueVolumeRotationMatrix = *btc->m_obliqueVolumeRotationMatrix;
//                *m_clippingPlaneGroup = *btc->m_clippingPlaneGroup;
//                break;
//            }
//        }
//    }
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


