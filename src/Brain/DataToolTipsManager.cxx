
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

#define __DATA_TOOL_TIPS_MANAGER_DECLARE__
#include "DataToolTipsManager.h"
#undef __DATA_TOOL_TIPS_MANAGER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "IdentificationStringBuilder.h"
#include "IdentificationTextGenerator.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SelectionManager.h"

using namespace caret;

/**
 * \class caret::DataToolTipsManager 
 * \brief Manages Data ToolTips.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param enabledStatus
 *     Enabled status for data tool tips
 */
DataToolTipsManager::DataToolTipsManager(const bool enabledStatus)
: CaretObject()
{
    m_enabledFlag = enabledStatus;
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    /*EventManager::get()->addEventListener(this, EventTypeEnum::);*/
}

/**
 * Destructor.
 */
DataToolTipsManager::~DataToolTipsManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get text for the tooltip.
 *
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 */
AString
DataToolTipsManager::getToolTip(const Brain* brain,
                                const BrowserTabContent* browserTab,
                                const SelectionManager* selectionManager) const
{
    CaretAssert(brain);
    CaretAssert(browserTab);
    CaretAssert(selectionManager);
    
    IdentificationTextGenerator itg;
    const AString text = itg.createToolTipText(brain,
                                               browserTab,
                                               selectionManager,
                                               this);

    return text;
}

/**
 * @return Is tips enabled?
 */
bool
DataToolTipsManager::isEnabled() const
{
    return m_enabledFlag;
}

/**
 * Set status for tips enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setEnabled(const bool status)
{
    m_enabledFlag = status;
}

/**
 * @return Is show primary anatomical surface enabled?
 */
bool
DataToolTipsManager::isShowSurfacePrimaryAnatomical() const
{
    return m_showSurfacePrimaryAnatomicalFlag;
}

/**
 * Set status for primary anatomical surface enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowSurfacePrimaryAnatomical(const bool status)
{
    m_showSurfacePrimaryAnatomicalFlag = status;
}

/**
 * @return Is show viewed surface enabled?
 */
bool
DataToolTipsManager::isShowSurfaceViewed() const
{
    return m_showSurfaceViewedFlag;
}

/**
 * Set status for viewed surface enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowSurfaceViewed(const bool status)
{
    m_showSurfaceViewedFlag = status;
}

/**
 * @return Is show volume underlayenabled?
 */
bool
DataToolTipsManager::isShowVolumeUnderlay() const
{
    return m_showVolumeUnderlayFlag;
}

/**
 * Set status for volume underlay enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowVolumeUnderlay(const bool status)
{
    m_showVolumeUnderlayFlag = status;
}


/**
 * @return Is show top layer enabled?
 */
bool
DataToolTipsManager::isShowTopLayer() const
{
    return m_showTopLayerFlag;
}

/**
 * Set status for top layer enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowTopLayer(const bool status)
{
    m_showTopLayerFlag = status;
}

/**
 * @return Is show border enabled?
 */
bool
DataToolTipsManager::isShowBorder() const
{
    return m_showBorderFlag;
}

/**
 * Set status for show border
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowBorder(const bool status)
{
    m_showBorderFlag = status;
}

/**
 * @return Is show focus enabled?
 */
bool
DataToolTipsManager::isShowFocus() const
{
    return m_showFocusFlag;
}

/**
 * Set status for show focus
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowFocus(const bool status)
{
    m_showFocusFlag = status;
}

/**
 * @return Is show chart enabled?
 */
bool
DataToolTipsManager::isShowChart() const
{
    return m_showChartFlag;
}

/**
 * Set status for show chart
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowChart(const bool status)
{
    m_showChartFlag = status;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
DataToolTipsManager::toString() const
{
    return "DataToolTipsManager";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
DataToolTipsManager::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
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
DataToolTipsManager::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DataToolTipsManager",
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
DataToolTipsManager::restoreFromScene(const SceneAttributes* sceneAttributes,
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

