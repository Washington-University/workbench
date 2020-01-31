
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <algorithm>

#define __TILE_TABS_GRID_CONFIGURATION_MODIFIER_DECLARE__
#include "TileTabsGridConfigurationModifier.h"
#undef __TILE_TABS_GRID_CONFIGURATION_MODIFIER_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabNewClone.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabNew.h"
#include "EventBrowserWindowTileTabOperation.h"
#include "EventManager.h"
#include "EventTileTabsGridConfigurationModification.h"
#include "GuiManager.h"
#include "SpacerTabContent.h"
#include "TileTabsLayoutGridConfiguration.h"

using namespace caret;

static bool debugFlag = false;
    
/**
 * \class caret::TileTabsGridConfigurationModifier 
 * \brief Modifies a tile tabs grid configuration.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param existingTabs
 *     All tabs in the window.
 * @param windowIndex
 *     Index of window requesting this operation
 * @param modifyEvent
 *     Event describing modification.
 */
TileTabsGridConfigurationModifier::TileTabsGridConfigurationModifier(const std::vector<const BrainOpenGLViewportContent*>& existingTabs,
                                                                     const int32_t windowIndex,
                                                                     EventTileTabsGridConfigurationModification* modifyEvent)
: CaretObject(),
m_existingTabs(existingTabs),
m_windowIndex(windowIndex),
m_modifyEvent(modifyEvent)
{
    CaretAssert(modifyEvent);
    m_currentTileTabsConfiguration = modifyEvent->getTileTabsConfiguration();
    CaretAssert(m_currentTileTabsConfiguration);
}

/**
 * Destructor.
 */
TileTabsGridConfigurationModifier::~TileTabsGridConfigurationModifier()
{
    for (auto rc : m_rowColumns) {
        delete rc;
    }
    m_rowColumns.clear();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TileTabsGridConfigurationModifier::toString() const
{
    AString s;
    for (const auto rc : m_rowColumns) {
        if ( ! s.isEmpty()) {
            s.append("\n");
        }
        s.append(rc->toString());
    }
    return s;
}

/**
 * Run to perform the operation.
 *
 * @param errorMessageOut
 *     Contains error information if operation fails.
 * @return 
 *     True if successful, else false.
 */
bool
TileTabsGridConfigurationModifier::run(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    loadRowColumnsFromTileTabsConfiguration();
    if (debugFlag) {
        std::cout << "Loaded: " << toString() << std::endl << std::flush;
    }
    bool validFlag = performModification(errorMessageOut);
    if (debugFlag) {
        std::cout << "After Modification: " << toString() << std::endl << std::endl << std::flush;
    }
    
    if (validFlag) {
        validFlag = loadRowColumnsIntoTileTabsConfiguration(errorMessageOut);
    }
    
    return validFlag;
}

/**
 * Load the rows or columns from the Tile Tabs Configuration
 */
void
TileTabsGridConfigurationModifier::loadRowColumnsFromTileTabsConfiguration()
{
    const int32_t numRows    = m_currentTileTabsConfiguration->getNumberOfRows();
    const int32_t numColumns = m_currentTileTabsConfiguration->getNumberOfColumns();
    
    switch (m_modifyEvent->getRowColumnType()) {
        case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
            for (int32_t jCol = 0; jCol < numColumns; jCol++) {
                m_rowColumns.push_back(new RowColumnContent(m_existingTabs,
                                                            m_modifyEvent->getTileTabsConfiguration(),
                                                            jCol,
                                                            false));
            }
            break;
        case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
            for (int32_t iRow = 0; iRow < numRows; iRow++) {
                m_rowColumns.push_back(new RowColumnContent(m_existingTabs,
                                                            m_modifyEvent->getTileTabsConfiguration(),
                                                            iRow,
                                                            true));
            }
            break;
    }
}

/**
 * Perform the modification.
 *
 * @param errorMessageOut
 *     Contains error information if operation fails.
 * @return
 *     True if successful, else false.
 */
bool
TileTabsGridConfigurationModifier::performModification(AString& errorMessageOut)
{

    const int32_t rowColumnIndex = m_modifyEvent->getRowColumnIndex();
    
    const int32_t numRowColumns = static_cast<int32_t>(m_rowColumns.size());
    switch (m_modifyEvent->getOperation()) {
        case EventTileTabsGridConfigurationModification::Operation::DELETE_IT:
            if (numRowColumns <= 1) {
                errorMessageOut = "Cannot delete ROWCOL when there is only one ROWCOL";
            }
            else if ((rowColumnIndex >= 0)
                     && (rowColumnIndex < numRowColumns)) {
                CaretAssertVectorIndex(m_rowColumns, rowColumnIndex);
                RowColumnContent* deleteRowColumn = m_rowColumns[rowColumnIndex];
                CaretAssert(deleteRowColumn);
                m_rowColumns.erase(m_rowColumns.begin() + rowColumnIndex);
                CaretAssert(std::find(m_rowColumns.begin(),
                                      m_rowColumns.end(),
                                      deleteRowColumn) == m_rowColumns.end());
                
                for (auto t : deleteRowColumn->m_tabElements) {
                    if (t->m_browserTabContent != NULL) {
                        m_browserTabsToDelete.push_back(t->m_browserTabContent);
                    }
                }
                delete deleteRowColumn;
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when deleting";
            }
            break;
        case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_AFTER:
            if ((rowColumnIndex >= 0)
                && (rowColumnIndex < numRowColumns)) {
                RowColumnContent* rowColumnCopy = m_rowColumns[rowColumnIndex]->clone(errorMessageOut);
                if (rowColumnCopy != NULL) {
                    const int32_t insertOffset = (rowColumnIndex + 1);
                    m_rowColumns.insert(m_rowColumns.begin() +
                                        insertOffset,
                                        rowColumnCopy);
                }
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when duplicating";
            }
            break;
        case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_BEFORE:
            if ((rowColumnIndex >= 0)
                && (rowColumnIndex < numRowColumns)) {
                RowColumnContent* rowColumnCopy = m_rowColumns[rowColumnIndex]->clone(errorMessageOut);
                if (rowColumnCopy != NULL) {
                    const int32_t insertOffset = rowColumnIndex;
                    m_rowColumns.insert(m_rowColumns.begin() +
                                        insertOffset,
                                        rowColumnCopy);
                }
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when duplicating";
            }
            break;
        case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER:
        case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_BEFORE:
            if ((rowColumnIndex >= 0)
                && (rowColumnIndex < numRowColumns)) {
                CaretAssertVectorIndex(m_rowColumns, 0);
                const int32_t numberOfElements = m_rowColumns[0]->m_tabElements.size();
                RowColumnContent* rowColumnSpacer = RowColumnContent::newInstanceContainingSpacers(numberOfElements);
                if (rowColumnSpacer != NULL) {
                    int32_t insertOffset = rowColumnIndex;
                    if (m_modifyEvent->getOperation() == EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER) {
                        insertOffset++;
                    }
                    m_rowColumns.insert(m_rowColumns.begin() +
                                        insertOffset,
                                        rowColumnSpacer);
                }
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when insert spacer";
            }
            break;
        case EventTileTabsGridConfigurationModification::Operation::MOVE_AFTER:
            if (numRowColumns <= 1) {
                errorMessageOut = "Cannot move ROWCOL when there is only one ROWCOL";
            }
            else if (rowColumnIndex == (numRowColumns - 1)) {
                errorMessageOut = "Cannot move last ROWCOL after itself";
            }
            else if ((rowColumnIndex >= 0)
                     && (rowColumnIndex < (numRowColumns - 1))) {
                CaretAssertVectorIndex(m_rowColumns, rowColumnIndex);
                CaretAssertVectorIndex(m_rowColumns, rowColumnIndex + 1);
                std::swap(m_rowColumns[rowColumnIndex],
                          m_rowColumns[rowColumnIndex + 1]);
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when moving";
            }
            break;
        case EventTileTabsGridConfigurationModification::Operation::MOVE_BEFORE:
            if (numRowColumns <= 1) {
                errorMessageOut = "Cannot move ROWCOL when there is only one ROWCOL";
            }
            else if (rowColumnIndex == 0) {
                errorMessageOut = "Cannot move last ROWCOL before itself";
            }
            else if ((rowColumnIndex >= 1)
                     && (rowColumnIndex < numRowColumns)) {
                CaretAssertVectorIndex(m_rowColumns, rowColumnIndex);
                CaretAssertVectorIndex(m_rowColumns, rowColumnIndex - 1);
                std::swap(m_rowColumns[rowColumnIndex],
                          m_rowColumns[rowColumnIndex - 1]);
            }
            else {
                errorMessageOut = "Invalid ROWCOL index=RCINDEX when moving";
            }
            break;
    }

    if ( ! errorMessageOut.isEmpty()) {
        AString nameText;
        switch (m_modifyEvent->getRowColumnType()) {
            case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
                nameText = " Column ";
                break;
            case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
                nameText = " Row ";
                break;
        }
        
        /*
         * "substite names"
         */
        errorMessageOut = errorMessageOut.replace("ROWCOL", nameText);
        errorMessageOut = errorMessageOut.replace("RCINDEX", AString::number(rowColumnIndex + 1));
    }
    
    const bool validFlag = errorMessageOut.isEmpty();
    return validFlag;
}

/**
 * Load the modified rows/columns into the tile tabs configuration current in the browser window
 *
 * @param errorMessageOut
 *     Contains error information if operation fails.
 * @return
 *     True if successful, else false.
 */
bool
TileTabsGridConfigurationModifier::loadRowColumnsIntoTileTabsConfiguration(AString& errorMessageOut)
{
    TileTabsLayoutGridConfiguration newConfiguration(*m_currentTileTabsConfiguration);
    std::vector<BrowserTabContent*> browserTabs;
    
    switch (m_modifyEvent->getRowColumnType()) {
        case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
        {
            const int32_t numRows = static_cast<int32_t>(m_rowColumns[0]->m_tabElements.size());
            CaretAssert(numRows == m_currentTileTabsConfiguration->getNumberOfRows());
            const int32_t numColumns = static_cast<int32_t>(m_rowColumns.size());
            
            newConfiguration.setNumberOfRows(numRows);
            newConfiguration.setNumberOfColumns(numColumns);
            
            for (int32_t iRow = 0; iRow < numRows; iRow++) {
                for (int32_t jCol = 0; jCol < numColumns; jCol++) {
                    RowColumnContent* columnContent = m_rowColumns[jCol];
                    CaretAssertVectorIndex(columnContent->m_tabElements, iRow);
                    BrowserTabContent* btc = columnContent->m_tabElements[iRow]->m_browserTabContent;
                    if (btc != NULL) {
                        browserTabs.push_back(btc);
                    }
                }
            }
            
            for (int32_t jCol = 0; jCol < numColumns; jCol++) {
                TileTabsGridRowColumnElement* rce = newConfiguration.getColumn(jCol);
                CaretAssertVectorIndex(m_rowColumns, jCol);
                CaretAssert(m_rowColumns[jCol]->m_stretching);
                *rce = *m_rowColumns[jCol]->m_stretching;
            }
        }
            break;
        case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
        {
            const int32_t numRows = static_cast<int32_t>(m_rowColumns.size());
            const int32_t numColumns = static_cast<int32_t>(m_rowColumns[0]->m_tabElements.size());
            CaretAssert(numColumns == m_currentTileTabsConfiguration->getNumberOfColumns());
            
            newConfiguration.setNumberOfRows(numRows);
            newConfiguration.setNumberOfColumns(numColumns);
            
            for (int32_t iRow = 0; iRow < numRows; iRow++) {
                CaretAssertVectorIndex(m_rowColumns, iRow);
                RowColumnContent* rowContent = m_rowColumns[iRow];
                for (int32_t jCol = 0; jCol < numColumns; jCol++) {
                    CaretAssertVectorIndex(rowContent->m_tabElements, jCol);
                    BrowserTabContent* btc = rowContent->m_tabElements[jCol]->m_browserTabContent;
                    if (btc != NULL) {
                        browserTabs.push_back(btc);
                    }
                }
            }
            
            for (int32_t iRow = 0; iRow < numRows; iRow++) {
                TileTabsGridRowColumnElement* rce = newConfiguration.getRow(iRow);
                CaretAssertVectorIndex(m_rowColumns, iRow);
                CaretAssert(m_rowColumns[iRow]->m_stretching);
                *rce = *m_rowColumns[iRow]->m_stretching;
            }
        }
            break;
    }
    
    /*
     * Copy new tile tabs configuration into the current configuration
     */
    *m_currentTileTabsConfiguration = newConfiguration;
    
    /*
     * Update tabs in the window's toolbar
     */
    QWidget* parentWindow(GuiManager::get()->getBrowserWindowByWindowIndex(m_modifyEvent->getWindowIndex()));
    const int32_t invalidTabIndex(-1);
    const int32_t dummyWindowViewport[4] { -1, -1, -1, -1 };
    const int32_t dummyMouseX(-1);
    const int32_t dummyMouseY(-1);
    EventBrowserWindowTileTabOperation updateTabsEvent(EventBrowserWindowTileTabOperation::OPERATION_REPLACE_TABS,
                                                       parentWindow,
                                                       m_modifyEvent->getWindowIndex(),
                                                       invalidTabIndex,
                                                       dummyWindowViewport,
                                                       dummyMouseX,
                                                       dummyMouseY,
                                                       browserTabs);
    EventManager::get()->sendEvent(updateTabsEvent.getPointer());
    
    if (updateTabsEvent.isError()) {
        errorMessageOut.appendWithNewLine(updateTabsEvent.getErrorMessage());
    }
    
    if ( ! m_browserTabsToDelete.empty()) {
        for (auto btc : m_browserTabsToDelete) {
            EventBrowserTabDelete deleteEvent(btc,
                                              btc->getTabNumber(),
                                              m_windowIndex);
            EventManager::get()->sendEvent(deleteEvent.getPointer());
            if (deleteEvent.isError()) {
                errorMessageOut.appendWithNewLine(deleteEvent.getErrorMessage());
            }
        }
        m_browserTabsToDelete.clear();
    }
    
    if (errorMessageOut.isEmpty()) {
        return true;
    }
    
    return false;
}

/*
 * Constructor for element in the row column matrix.
 *
 * @param rowIndex
 *     The row index
 * @param columnIndex
 *     The column index
 * @param browserTabContent
 *     Browser tab content at (rowIndex, columnIndex) in the current
 *     Tile Tabs Configuration
 */
TileTabsGridConfigurationModifier::Element::Element(const int32_t rowIndex,
                                                const int32_t columnIndex,
                                                BrowserTabContent* browserTabContent)
:
m_targetRowIndex(rowIndex),
m_targetColumnIndex(columnIndex),
m_sourceRowIndex(rowIndex),
m_sourceColumnIndex(columnIndex),
m_browserTabContent(browserTabContent)
{
    
}

/**
 * @return String representation of object.
 */
AString
TileTabsGridConfigurationModifier::Element::toString() const 
{
    AString s("(row=%1, column=%2)");
    s = s.arg(m_sourceRowIndex).arg(m_sourceColumnIndex);
    return s;
}

/*
 * Constructor for content of a row/column element that
 * contains the tabs and stretching for one row or one column
 *
 * @param existingTabs
 *     Existing browser tabs.
 * @param tileTabsConfiguration
 *     The current tile tabs configuration in the browser window.
 * @param rowColumnIndex
 *     Index of the row or column related to modification.
 * @param rowFlag
 *     True if rows are being operated upon, false if operating on columns
 */
TileTabsGridConfigurationModifier::RowColumnContent::RowColumnContent(const std::vector<const BrainOpenGLViewportContent*>& existingTabs,
                                                                  TileTabsLayoutGridConfiguration* tileTabsConfiguration,
                                                                  const int32_t rowColumnIndex,
                                                                  const bool rowFlag)
{
    const int32_t numRows = tileTabsConfiguration->getNumberOfRows();
    const int32_t numColumns = tileTabsConfiguration->getNumberOfColumns();
    const int32_t numberOfTabs =static_cast<int32_t>(existingTabs.size());
    
    if (rowFlag) {
        for (int32_t jCol = 0; jCol < numColumns; jCol++) {
            BrowserTabContent* browserTabContent(NULL);
            const int32_t tabIndex = (rowColumnIndex * numColumns) + jCol;
            if (tabIndex < numberOfTabs) {
                CaretAssertVectorIndex(existingTabs, tabIndex);
                browserTabContent = existingTabs[tabIndex]->getBrowserTabContent();
            }
            
            m_tabElements.push_back(new Element(rowColumnIndex,
                                                jCol,
                                                browserTabContent));
        }
        
        m_stretching = new TileTabsGridRowColumnElement(*tileTabsConfiguration->getRow(rowColumnIndex));
    }
    else {
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            BrowserTabContent* browserTabContent(NULL);
            const int32_t tabIndex = (iRow * numColumns) + rowColumnIndex;
            if (tabIndex < numberOfTabs) {
                CaretAssertVectorIndex(existingTabs, tabIndex);
                browserTabContent = existingTabs[tabIndex]->getBrowserTabContent();
            }
            
            m_tabElements.push_back(new Element(iRow,
                                                rowColumnIndex,
                                                browserTabContent));
        }
        
        m_stretching = new TileTabsGridRowColumnElement(*tileTabsConfiguration->getColumn(rowColumnIndex));
    }
}

/*
 * Constructor that creates the given number of elements.
 * 
 * @param numberOfElements
 *     Number of elements for row/column.
 */
TileTabsGridConfigurationModifier::RowColumnContent::RowColumnContent(const int32_t numberOfElements)
{
    for (int32_t i = 0; i < numberOfElements; i++) {
        m_tabElements.push_back(new Element(i, i, NULL));
    }
    m_stretching = new TileTabsGridRowColumnElement();
}

/**
 * @return New instance containing the given number of elements setup as spacers.
 *
 * @param numberOfElements
 *     Number of elements for row/column.
 */
TileTabsGridConfigurationModifier::RowColumnContent*
TileTabsGridConfigurationModifier::RowColumnContent::newInstanceContainingSpacers(const int32_t numberOfElements)
{
    RowColumnContent* content = new RowColumnContent(numberOfElements);
    
    content->m_stretching->setContentType(TileTabsGridRowColumnContentTypeEnum::SPACE);
    content->m_stretching->setStretchType(TileTabsGridRowColumnStretchTypeEnum::WEIGHT);
    content->m_stretching->setWeightStretch(1.0);
    
    return content;
}

/**
 * Destructor.
 */
TileTabsGridConfigurationModifier::RowColumnContent::~RowColumnContent()
{
    for (auto te : m_tabElements) {
        delete te;
    }
    m_tabElements.clear();
    
    delete m_stretching;
    m_stretching = NULL;
}

/**
 * Copy constructor.
 *
 * @param obj
 *     Instance that is copied.
 */
TileTabsGridConfigurationModifier::RowColumnContent::RowColumnContent(const RowColumnContent& obj)
: CaretObject(obj)
{
    for (auto te : obj.m_tabElements) {
        m_tabElements.push_back(new Element(*te));
    }
    
    m_stretching = new TileTabsGridRowColumnElement(*obj.m_stretching);
}

/**
 * @return String representation of object.
 */
AString
TileTabsGridConfigurationModifier::RowColumnContent::toString() const
{
    AString s;
    for (const auto te : m_tabElements) {
        s += (" " + te->toString());
    }
    return s;
}

/**
 * Clone this Row/column content instance.
 *
 * @param errorMessageOut
 *     Output with error message.
 * @return
 *     True if successful, else false
 */
TileTabsGridConfigurationModifier::RowColumnContent*
TileTabsGridConfigurationModifier::RowColumnContent::clone(AString& errorMessageOut) const
{
    RowColumnContent* cloned = new RowColumnContent(*this);
    CaretAssert(cloned);
    
    for (auto te : cloned->m_tabElements) {
        if (te->m_browserTabContent != NULL) {
            EventBrowserTabNewClone cloneTabEvent(te->m_browserTabContent->getTabNumber());
            EventManager::get()->sendEvent(cloneTabEvent.getPointer());
            if (cloneTabEvent.isError()) {
                errorMessageOut.appendWithNewLine(cloneTabEvent.getErrorMessage());
                te->m_browserTabContent = NULL;
            }
            else {
                te->m_browserTabContent = cloneTabEvent.getNewBrowserTab();
            }
        }
    }

    return cloned;
}

