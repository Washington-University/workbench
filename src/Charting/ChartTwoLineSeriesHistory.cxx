
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

#define __CHART_TWO_LINE_SERIES_HISTORY_DECLARE__
#include "ChartTwoLineSeriesHistory.h"
#undef __CHART_TWO_LINE_SERIES_HISTORY_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartTwoDataCartesian.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoLineSeriesHistory 
 * \brief Contains history of line series chart for a tab.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoLineSeriesHistory::ChartTwoLineSeriesHistory()
: CaretObjectTracksModification(),
SceneableInterface()
{
    initializeInstance();
}

/**
 * Destructor.
 */
ChartTwoLineSeriesHistory::~ChartTwoLineSeriesHistory()
{
    clearHistory();
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoLineSeriesHistory::ChartTwoLineSeriesHistory(const ChartTwoLineSeriesHistory& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj)
{
    initializeInstance();
    this->copyHelperChartTwoLineSeriesHistory(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoLineSeriesHistory&
ChartTwoLineSeriesHistory::operator=(const ChartTwoLineSeriesHistory& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperChartTwoLineSeriesHistory(obj);
    }
    return *this;    
}

bool
ChartTwoLineSeriesHistory::isLoadingEnabled() const
{
    return m_loadingEnabled;
}

/**
 * Set loading line series charts enabled
 *
 * @param loadingEnabled
 *    New value for loading line series charts enabled
 */
void
ChartTwoLineSeriesHistory::setLoadingEnabled(const bool loadingEnabled)
{
    m_loadingEnabled = loadingEnabled;
}

/**
 * Generate the default color.
 */
CaretColorEnum::Enum
ChartTwoLineSeriesHistory::generateDefaultColor()
{
    CaretColorEnum::Enum color = CaretColorEnum::BLACK;
    
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getColorEnums(colors);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    CaretAssert(numColors > 0);
    if (s_defaultColorIndexGenerator < 0) {
        s_defaultColorIndexGenerator = 0;
    }
    else if (s_defaultColorIndexGenerator >= numColors) {
        s_defaultColorIndexGenerator = 0;
    }
    
    CaretAssertVectorIndex(colors, s_defaultColorIndexGenerator);
    color = colors[s_defaultColorIndexGenerator];

    /* move to next color */
    ++s_defaultColorIndexGenerator;
    
    return color;
}

/**
 * Validate the default color.
 */
void
ChartTwoLineSeriesHistory::validateDefaultColor()
{
    std::vector<CaretColorEnum::Enum> allEnums;
    CaretColorEnum::getColorAndOptionalEnums(allEnums, (CaretColorEnum::ColorOptions::OPTION_INCLUDE_CUSTOM_COLOR
                                                        || CaretColorEnum::CaretColorEnum::OPTION_INCLUDE_NONE_COLOR));
    if (std::find(allEnums.begin(),
                  allEnums.end(),
                  m_defaultColor) == allEnums.end()) {
        const AString msg("Default color enum is invalid.  Integer value: " + AString::number((int)m_defaultColor));
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
    
    if (m_defaultColor == CaretColorEnum::CUSTOM) {
        const AString msg("Default color CUSTOM is not allowed");
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
    else if (m_defaultColor == CaretColorEnum::NONE) {
        const AString msg("Default color NONE is not allowed");
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
}

/**
 * Initialize an instance of this class.
 */
void
ChartTwoLineSeriesHistory::initializeInstance()
{
    m_defaultColor = ChartTwoLineSeriesHistory::generateDefaultColor();
    validateDefaultColor();
    
    const int32_t defaultHistoryCount = 5;
    m_loadingEnabled = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_displayCount = defaultHistoryCount;
    
    m_sceneAssistant->add("m_loadingEnabled",
                          &m_loadingEnabled);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_defaultColor",
                                                                &m_defaultColor);
    m_sceneAssistant->add("m_displayCount",
                          &m_displayCount);
}


/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoLineSeriesHistory::copyHelperChartTwoLineSeriesHistory(const ChartTwoLineSeriesHistory& obj)
{    m_loadingEnabled = obj.m_loadingEnabled;
    
    clearHistory();
    
    for (const auto item : m_chartHistory) {
        ChartTwoDataCartesian* cartData = item->clone();
        CaretAssert(cartData);
        
        addHistoryItem(cartData);
    }
    
    m_defaultColor = obj.m_defaultColor;
    m_displayCount = obj.m_displayCount;
    validateDefaultColor();
}

/**
 * @return The default color.
 */
CaretColorEnum::Enum
ChartTwoLineSeriesHistory::getDefaultColor() const
{
    return m_defaultColor;
}

/**
 * Set the default color.
 *
 * @param defaultColor New value for default color.
 */
void
ChartTwoLineSeriesHistory::setDefaultColor(const CaretColorEnum::Enum defaultColor)
{
    if (defaultColor != m_defaultColor) {
        m_defaultColor = defaultColor;
        validateDefaultColor();
        setModified();
    }
    
//    for (auto chartData : m_chartHistory) {
//        chartData->setColor(defaultColor);
//    }
}

/**
 * @return Count of items for display.
 */
int32_t
ChartTwoLineSeriesHistory::getDisplayCount() const
{
    return m_displayCount;
}

/**
 * Set count of items for display.
 *
 * @param count
 *     New value for count.
 */
void
ChartTwoLineSeriesHistory::setDisplayCount(const int32_t count)
{
    if (count != m_displayCount) {
        m_displayCount = count;
        updateDisplayedHistoryItems();
        setModified();
    }
}

/**
 * Update the number of displayed history items.
 */
void ChartTwoLineSeriesHistory::updateDisplayedHistoryItems()
{
    int32_t itemsDisplayedCount = 0;
    const int32_t numItems = getHistoryCount();
    for (int32_t i = 0; i < numItems; i++) {
        ChartTwoDataCartesian* item = getHistoryItem(i);
        if (item->isSelected()) {
            itemsDisplayedCount++;
            if (itemsDisplayedCount > m_displayCount) {
                item->setSelected(false);
            }
        }
    }
}

/**
 * @return Count of items in history.
 */
int32_t ChartTwoLineSeriesHistory::getHistoryCount() const
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
ChartTwoLineSeriesHistory::addHistoryItem(ChartTwoDataCartesian* historyItem)
{
    CaretAssert(historyItem);
    historyItem->setColor(m_defaultColor);
    m_chartHistory.push_front(historyItem);
    updateDisplayedHistoryItems();
}

/**
 * @return History item at the given index.
 *
 * @param index
 *      Index of the item.
 */
ChartTwoDataCartesian*
ChartTwoLineSeriesHistory::getHistoryItem(const int32_t index)
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
ChartTwoLineSeriesHistory::getHistoryItem(const int32_t index) const
{
    CaretAssertVectorIndex(m_chartHistory, index);
    return m_chartHistory[index];
}

void
ChartTwoLineSeriesHistory::removeHistoryItem(const int32_t index)
{
    CaretAssertVectorIndex(m_chartHistory, index);
    delete m_chartHistory[index];
    m_chartHistory.erase(m_chartHistory.begin() + index);
}

/**
 * @return Clear the history.
 */
void
ChartTwoLineSeriesHistory::clearHistory()
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
ChartTwoLineSeriesHistory::toString() const
{
    return "ChartTwoLineSeriesHistory";
}

/**
 * @return Is this instance modified?
 */
bool
ChartTwoLineSeriesHistory::isModified() const
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
ChartTwoLineSeriesHistory::clearModified()
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
ChartTwoLineSeriesHistory::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoLineSeriesHistory",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    /*
     * Save chart history
     */
    const int32_t numHistory = static_cast<int32_t>(m_chartHistory.size());
    if (numHistory > 0) {
        SceneObjectMapIntegerKey* chartHistoryMap = new SceneObjectMapIntegerKey("m_chartHistoryMap",
                                                                                 SceneObjectDataTypeEnum::SCENE_CLASS);
        for (int32_t i = 0; i < numHistory; i++) {
            CaretAssertVectorIndex(m_chartHistory, i);
            chartHistoryMap->addClass(i,
                                      m_chartHistory[i]->saveToScene(sceneAttributes,
                                                                            "m_chartHistory"));
        }
        sceneClass->addChild(chartHistoryMap);
    }

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
ChartTwoLineSeriesHistory::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    clearHistory();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    validateDefaultColor();
    
    /*
     * Restore chart matrix properties
     */
    const SceneObjectMapIntegerKey* chartHistoryMap = sceneClass->getMapIntegerKey("m_chartHistoryMap");
    if (chartHistoryMap != NULL) {
        const std::vector<int32_t> indices = chartHistoryMap->getKeys();
        const int32_t numIndices = static_cast<int32_t>(indices.size());
        for (int32_t i = 0; i < numIndices; i++) {
            const SceneClass* historyClass = chartHistoryMap->classValue(i);
            ChartTwoDataCartesian* historyItem = new ChartTwoDataCartesian(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                                                           GraphicsPrimitive::PrimitiveType::LINES);
            historyItem->restoreFromScene(sceneAttributes,
                                          historyClass);
            
            /*
             * Note: addHistoryItem() assigns the default color so
             * need to update color
             */
            const CaretColorEnum::Enum color = historyItem->getColor();
            addHistoryItem(historyItem);
            historyItem->setColor(color);
        }
    }

    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

