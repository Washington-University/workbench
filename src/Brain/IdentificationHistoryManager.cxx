
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __IDENTIFICATION_HISTORY_MANAGER_DECLARE__
#include "IdentificationHistoryManager.h"
#undef __IDENTIFICATION_HISTORY_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "IdentificationHistoryRecord.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentificationHistoryManager 
 * \brief Manages history of identification operations
 * \ingroup Brain
 */

/**
 * Constructor.
 */
IdentificationHistoryManager::IdentificationHistoryManager()
: CaretObject()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_showLastHistoryCount",
                          &m_showLastHistoryCount);
    
}

/**
 * Destructor.
 */
IdentificationHistoryManager::~IdentificationHistoryManager()
{
}

/**
 * @return The identification text contaiing the last "history count" records
 */
AString
IdentificationHistoryManager::getHtml() const
{
    AString text("<html>\n"
                 "<head>\n"
                 "<style type=\"text/css\">\n"
                 "table { border-width: 1px; border-style: ridge; }\n"
                 "table td { padding: 3px; }"
                 "table th { padding: 2px; }"
                 "</style>\n"
                 "</head>\n"
                 "</body>\n");
    
    /*
     * Show last history ZERO means show all records
     */
    const bool showAllFlag = (getShowLastHistoryCount() == 0);
    
    const int32_t numRecords = (showAllFlag
                                ? getNumberOfHistoryRecords()
                                : std::min(getShowLastHistoryCount(),
                                           getNumberOfHistoryRecords()));
    if (numRecords > 0) {
        for (int32_t i = (numRecords - 1); i >= 0; i--) {
            if ( ! text.isEmpty()) {
                text.append("\n");
            }
            
            text.append(getHistoryRecord(i)->getText());
        }
    }
    
    text.append("</body></html>\n");
    
//    std::cout << std::endl << std::endl << text << std::endl << std::endl;
    
    return text;
}


/**
 * @return History count that is displayed
 */
int32_t
IdentificationHistoryManager::getShowLastHistoryCount() const
{
    return m_showLastHistoryCount;
}

/**
 * Set the history count
 * @param historyCount
 *  New count
 */
void
IdentificationHistoryManager::setShowLastHistoryCount(const int32_t historyCount)
{
    m_showLastHistoryCount = historyCount;
}

/**
 * Clear the history
 */
void
IdentificationHistoryManager::clearHistory()
{
    m_historyRecords.clear();
}

/**
 * Add a history record
 * @param historyRecord
 *   The history record
 */
void
IdentificationHistoryManager::addHistoryRecord(IdentificationHistoryRecord* historyRecord)
{
    CaretAssert(historyRecord);
    std::unique_ptr<IdentificationHistoryRecord> ptr(historyRecord);
    m_historyRecords.push_front(std::move(ptr));
}

/**
 * @return Number of history records
 */
int32_t
IdentificationHistoryManager::getNumberOfHistoryRecords() const
{
    return m_historyRecords.size();
}

/**
 * @return History record at the given index
 * @param historyIndex
 *    Index of the history record
 */
const IdentificationHistoryRecord*
IdentificationHistoryManager::getHistoryRecord(const int32_t historyIndex) const
{
    CaretAssert((historyIndex >= 0)
                && (historyIndex < static_cast<int32_t>(m_historyRecords.size())));
    return m_historyRecords[historyIndex].get();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationHistoryManager::toString() const
{
    return "IdentificationHistoryManager";
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
IdentificationHistoryManager::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentificationHistoryManager",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    for (auto& record : m_historyRecords) {
        sceneClass->addClass(record->saveToScene(sceneAttributes,
                                                 "historyRecord"));
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
IdentificationHistoryManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_historyRecords.clear();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const int32_t numChildren = sceneClass->getNumberOfObjects();
    for (int32_t i = 0; i < numChildren; i++) {
        const SceneObject* so = sceneClass->getObjectAtIndex(i);
        if (so->getName() == "historyRecord") {
            const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
            if (sc != NULL) {
                IdentificationHistoryRecord* record = new IdentificationHistoryRecord();
                record->restoreFromScene(sceneAttributes, sc);
                addHistoryRecord(record);
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

