
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

#include "CaretAssert.h"
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

#include "CaretMappableDataFile.h"
#include "CiftiMappableDataFile.h"

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
    
    m_sceneAssistant = new SceneClassAssistant();
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

//ChartableTwoFileBaseChart::ChartableTwoFileBaseChart()
//{
//    
//}

/**
 * Destructor.
 */
ChartableTwoFileBaseChart::~ChartableTwoFileBaseChart()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
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
ChartableTwoFileBaseChart::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
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
                                            1);
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
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
    
}

