
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

#define __CHARTABLE_TWO_FILE_BASE_CHART_DECLARE__
#include "ChartableTwoFileBaseChart.h"
#undef __CHARTABLE_TWO_FILE_BASE_CHART_DECLARE__

#include "AnnotationPercentSizeText.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CiftiMappableDataFile.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableTwoFileBaseChart 
 * \brief Base class for File Charting Delegates.
 * \ingroup Files
 *
 * The first version of charting used interfaces for each
 * charting type.  This required some files to subclass
 * several interfaces and significant modification to any
 * class that supported charting.
 *
 * The reasons for using this 'delegate' are to avoid
 * extensive modifications to many file classes and to
 * avoid name clashes with the version one of charting.
 * Some of the original charting needs to be preserved
 * to support loading of older scenes.
 */


/**
 * Constructor.
 *
 * @param chartType
 *     Type of chart provided by this delegate.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 */
ChartableTwoFileBaseChart::ChartableTwoFileBaseChart(const ChartTwoDataTypeEnum::Enum chartType,
                                                                     CaretMappableDataFile* parentCaretMappableDataFile)
: CaretObjectTracksModification(),
m_chartType(chartType),
m_parentCaretMappableDataFile(parentCaretMappableDataFile)
{
    CaretAssert(m_chartType != ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID);
    CaretAssert(m_parentCaretMappableDataFile);
    
    /* will be NULL if file is not a cifti mappable file */
    m_parentCiftiMappableDataFile = dynamic_cast<CiftiMappableDataFile*>(m_parentCaretMappableDataFile);
    
    setDefaultAxisTitles();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_bottomAxisTitleText", // Use OLD name before top axis was added
                          &m_bottomTopAxisTitleText);
    m_sceneAssistant->add("m_leftRightAxisTitleText",
                          &m_leftRightAxisTitleText);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_CHART_LABEL_GET);
}

/**
 * Destructor.
 */
ChartableTwoFileBaseChart::~ChartableTwoFileBaseChart()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Set the default titles for the axes.
 */
void
ChartableTwoFileBaseChart::setDefaultAxisTitles()
{
    m_bottomTopAxisTitleText = setDefaultAxisTitle(ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM);
    m_leftRightAxisTitleText = setDefaultAxisTitle(ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT);
}

/**
 * Set the default axis title.
 *
 * @param axisLocation
 *     Location of the axis.
 */
AString
ChartableTwoFileBaseChart::setDefaultAxisTitle(const ChartAxisLocationEnum::Enum axisLocation)
{
    AString title;
    
    switch (axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            switch (m_chartType) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    title = "Data";
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                    title = CaretUnitsTypeEnum::toGuiName(m_compoundChartDataType.getLineChartUnitsAxisX());
                    switch (m_compoundChartDataType.getLineChartUnitsAxisX()) {
                        case CaretUnitsTypeEnum::NONE:
                            title = "Data";
                            break;
                        case CaretUnitsTypeEnum::HERTZ:
                            break;
                        case CaretUnitsTypeEnum::METERS:
                            break;
                        case CaretUnitsTypeEnum::PARTS_PER_MILLION:
                            break;
                        case CaretUnitsTypeEnum::RADIANS:
                            break;
                        case CaretUnitsTypeEnum::SECONDS:
                            break;
                    }
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    title = CaretUnitsTypeEnum::toGuiName(m_compoundChartDataType.getLineChartUnitsAxisX());
                    switch (m_compoundChartDataType.getLineChartUnitsAxisX()) {
                        case CaretUnitsTypeEnum::NONE:
                            title = "Data";
                            break;
                        case CaretUnitsTypeEnum::HERTZ:
                            break;
                        case CaretUnitsTypeEnum::METERS:
                            break;
                        case CaretUnitsTypeEnum::PARTS_PER_MILLION:
                            break;
                        case CaretUnitsTypeEnum::RADIANS:
                            break;
                        case CaretUnitsTypeEnum::SECONDS:
                            break;
                    }
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
            }
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
        {
            switch (m_chartType) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    title = "Counts";
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                    title = "Value";
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    title = "Value";
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
            }
        }
            break;
    }
    
    return title;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartableTwoFileBaseChart::toString() const
{
    return "ChartableTwoFileBaseChart";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartableTwoFileBaseChart::receiveEvent(Event* /*event*/)
{
}

/**
 * @return The CaretMappableDataFile that provided this delegate.
 */
CaretMappableDataFile*
ChartableTwoFileBaseChart::getCaretMappableDataFile()
{
    return m_parentCaretMappableDataFile;
}

/**
 * @return The CaretMappableDataFile that provided this delegate.
 */
const CaretMappableDataFile*
ChartableTwoFileBaseChart::getCaretMappableDataFile() const
{
    return m_parentCaretMappableDataFile;
}

/**
 * @return The CiftiMappableDataFile that provided this delegate.
 *         NULL is returned if not a CiftiMappableDataFile.
 */
CiftiMappableDataFile*
ChartableTwoFileBaseChart::getCiftiMappableDataFile()
{
    return m_parentCiftiMappableDataFile;
}

/**
 * @return The CiftiMappableDataFile that provided this delegate.
 *         NULL is returned if not a CiftiMappableDataFile.
 */
const CiftiMappableDataFile*
ChartableTwoFileBaseChart::getCiftiMappableDataFile() const
{
    return m_parentCiftiMappableDataFile;
}

/**
 * @return Chart version two data type
 */
ChartTwoDataTypeEnum::Enum
ChartableTwoFileBaseChart::getChartTwoDataType() const
{
    return m_chartType;
}

/**
 * @return Chart compound data type supported by subclass.
 */
ChartTwoCompoundDataType
ChartableTwoFileBaseChart::getChartTwoCompoundDataType() const
{
    CaretAssertMessage((m_compoundChartDataType.getChartTwoDataType() != ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID),
                       "Data type is invalid, was updateChartCompoundDataTypeAfterFileChanges() called by "
                       "implementing class.");
    return m_compoundChartDataType;
}

/**
 * Set the chart compound data type.  Subclasses of this base class MUST
 * call this method whenever the content of the delegatee file changes.
 *
 * @param compoundChartDataType
 *     Updated chart compound data type.  Its basic data type MUST
 *     match the data type passed to the constructor.
 */
void
ChartableTwoFileBaseChart::updateChartTwoCompoundDataTypeAfterFileChanges(const ChartTwoCompoundDataType compoundChartDataType)
{
    CaretAssert(m_chartType == compoundChartDataType.getChartTwoDataType());
    
    m_compoundChartDataType = compoundChartDataType;

    setDefaultAxisTitles();
}

/**
 * @return Annotation for the bottom top axis title (const method)
 */
AString
ChartableTwoFileBaseChart::getBottomTopAxisTitle() const
{
    return m_bottomTopAxisTitleText;
}

/**
 * Set the bottom top axis title.
 *
 * @param title
 *     New bottom axis title.
 */
void
ChartableTwoFileBaseChart::setBottomTopAxisTitle(const AString& title)
{
    if (title != m_bottomTopAxisTitleText) {
        m_bottomTopAxisTitleText = title;
        setModified();
    }
}

/**
 * @return Annotation for the bottom axis title (const method)
 */
AString
ChartableTwoFileBaseChart::getLeftRightAxisTitle() const
{
    return m_leftRightAxisTitleText;
}

/**
 * Set the left/right axis title.
 *
 * @param title
 *     New left/right axis title.
 */
void
ChartableTwoFileBaseChart::setLeftRightAxisTitle(const AString& title)
{
    if (title != m_leftRightAxisTitleText) {
        m_leftRightAxisTitleText = title;
        setModified();
    }
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
ChartableTwoFileBaseChart::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartableTwoFileBaseChart",
                                            3);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
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
ChartableTwoFileBaseChart::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    setDefaultAxisTitles();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
    
    if (sceneClass->getVersionNumber() <= 2) {
        AnnotationPercentSizeText bottomTopTitle(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                 AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
        bottomTopTitle.restoreFromScene(sceneAttributes,
                                        sceneClass->getClass("m_bottomAxisTitle")); // Use name before top axis was added
        if ( ! bottomTopTitle.getText().isEmpty()) {
            m_bottomTopAxisTitleText = bottomTopTitle.getText();
        }
        
        AnnotationPercentSizeText leftRightTitle(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                              AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
        leftRightTitle.restoreFromScene(sceneAttributes,
                                        sceneClass->getClass("m_leftRightAxisTitle"));
        if ( ! leftRightTitle.getText().isEmpty()) {
            m_leftRightAxisTitleText = leftRightTitle.getText();
        }
    }
}

