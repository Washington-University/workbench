
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CHART_TWO_DATA_CARTESIAN_HISTORY_DECLARE__
#include "ChartTwoDataCartesianHistory.h"
#undef __CHART_TWO_DATA_CARTESIAN_HISTORY_DECLARE__

#include "CaretAssert.h"
#include "ChartTwoDataCartesian.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoDataCartesianHistory 
 * \brief Contains history of Cartesian Charts
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoDataCartesianHistory::ChartTwoDataCartesianHistory()
: CaretObjectTracksModification()
{
    initializeInstance();
}

/**
 * Destructor.
 */
ChartTwoDataCartesianHistory::~ChartTwoDataCartesianHistory()
{
    clearHistory();
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoDataCartesianHistory::ChartTwoDataCartesianHistory(const ChartTwoDataCartesianHistory& obj)
: CaretObjectTracksModification(obj)
{
    initializeInstance();
    this->copyHelperChartTwoDataCartesianHistory(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoDataCartesianHistory&
ChartTwoDataCartesianHistory::operator=(const ChartTwoDataCartesianHistory& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperChartTwoDataCartesianHistory(obj);
    }
    return *this;    
}

/**
 * Initialize an instance of this class.
 */
void
ChartTwoDataCartesianHistory::initializeInstance()
{
    const int32_t defaultHistoryCount = 5;
    
    m_sceneAssistant = new SceneClassAssistant();
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayCountInTab[i] = defaultHistoryCount;
    }
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_defaultColor",
                                                                &m_defaultColor);
    m_sceneAssistant->addTabIndexedIntegerArray("m_displayCountInTab", m_displayCountInTab);
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoDataCartesianHistory::copyHelperChartTwoDataCartesianHistory(const ChartTwoDataCartesianHistory& obj)
{
    clearHistory();
    
    for (const auto item : m_chartHistory) {
        ChartTwoData* chartData = item->clone();
        ChartTwoDataCartesian* cartData = dynamic_cast<ChartTwoDataCartesian*>(chartData);
        CaretAssert(cartData);
        
        addHistoryItem(cartData);
    }
    
    m_defaultColor = obj.m_defaultColor;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayCountInTab[i] = obj.m_displayCountInTab[i];
    }
}

/**
 * @return The default color.
 */
CaretColorEnum::Enum
ChartTwoDataCartesianHistory::getDefaultColor() const
{
    return m_defaultColor;
}

/**
 * Set the default color.
 *
 * @param defaultColor New value for default color.
 */
void
ChartTwoDataCartesianHistory::setDefaultColor(const CaretColorEnum::Enum defaultColor)
{
    if (defaultColor != m_defaultColor) {
        m_defaultColor = defaultColor;
        setModified();
    }
}

/**
 * @return Count of items for display in tab.
 *
 * @param tabIndex
 *     Index of tab.
 */
int32_t
ChartTwoDataCartesianHistory::getDisplayCountInTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayCountInTab, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_displayCountInTab[tabIndex];
}

/**
 * Set count of items for display in tab.
 *
 * @param tabIndex
 *     Index of tab.
 * @param count
 *     New value for count.
 */
void
ChartTwoDataCartesianHistory::setDisplayCountInTab(const int32_t tabIndex,
                                                   const int count)
{
    CaretAssertArrayIndex(m_displayCountInTab, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    if (count != m_displayCountInTab[tabIndex]) {
        m_displayCountInTab[tabIndex] = count;
        setModified();
    }
}

/**
 * @return Count of items in history.
 */
int32_t ChartTwoDataCartesianHistory::getHistoryCount()
{
    return m_chartHistory.size();
}

/**
 * Add a history item.  This instance takes ownership of the item
 * and will delete it.  Default color is assigned to item.
 *
 * @param historyItem
 *     Added to history.
 */
void
ChartTwoDataCartesianHistory::addHistoryItem(ChartTwoDataCartesian* historyItem)
{
    CaretAssert(historyItem);
    historyItem->setColor(m_defaultColor);
    m_chartHistory.push_front(historyItem);
}

/**
 * @return History item at the given index.
 *
 * @param index
 *      Index of the item.
 */
ChartTwoDataCartesian*
ChartTwoDataCartesianHistory::getHistoryItem(const int32_t index)
{
    CaretAssertVectorIndex(m_chartHistory, index);
    return m_chartHistory[index];
}

/**
 * @return History item at the given index (const method).
 *
 * @param index
 *      Index of the item.
 */
const ChartTwoDataCartesian*
ChartTwoDataCartesianHistory::getHistoryItem(const int32_t index) const
{
    CaretAssertVectorIndex(m_chartHistory, index);
    return m_chartHistory[index];
}

/**
 * @return Clear the history.
 */
void
ChartTwoDataCartesianHistory::clearHistory()
{
    for (auto item : m_chartHistory) {
        delete item;
    }
    m_chartHistory.clear();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ChartTwoDataCartesianHistory::toString() const
{
    return "ChartTwoDataCartesianHistory";
}

/**
 * @return Is this instance modified?
 */
bool
ChartTwoDataCartesianHistory::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    for (const auto item : m_chartHistory) {
        if (item->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear modified status for this instance.
 */
void
ChartTwoDataCartesianHistory::clearModified()
{
    for (const auto item : m_chartHistory) {
        item->clearModified();
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
ChartTwoDataCartesianHistory::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoDataCartesianHistory",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    /*
     * Save chart history
     */
    SceneObjectMapIntegerKey* chartHistoryMap = new SceneObjectMapIntegerKey("m_chartHistoryMap",
                                                                                      SceneObjectDataTypeEnum::SCENE_CLASS);
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    for (auto tabIndex : tabIndices) {
        CaretAssertVectorIndex(m_chartHistory, tabIndex);
        chartHistoryMap->addClass(tabIndex,
                                  m_chartHistory[tabIndex]->saveToScene(sceneAttributes,
                                                                        "m_chartHistory"));
    }
    sceneClass->addChild(chartHistoryMap);

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
ChartTwoDataCartesianHistory::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    clearHistory();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    /*
     * Restore chart matrix properties
     */
    const SceneObjectMapIntegerKey* chartHistoryMap = sceneClass->getMapIntegerKey("m_chartHistoryMap");
    if (chartHistoryMap != NULL) {
        const std::vector<int32_t> tabIndices = chartHistoryMap->getKeys();
        for (auto tabIndex : tabIndices) {
            const SceneClass* sceneClass = chartHistoryMap->classValue(tabIndex);
            CaretAssertVectorIndex(m_chartHistory, tabIndex);
            m_chartHistory[tabIndex]->restoreFromScene(sceneAttributes,
                                                       sceneClass);
        }
    }

    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

