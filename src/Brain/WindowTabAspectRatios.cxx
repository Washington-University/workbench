
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __WINDOW_TAB_ASPECT_RATIOS_DECLARE__
#include "WindowTabAspectRatios.h"
#undef __WINDOW_TAB_ASPECT_RATIOS_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::WindowTabAspectRatios 
 * \brief Contains aspect ratios for a window and its tabs
 * \ingroup Brain
 */

/**
 * Constructor.
 */
WindowTabAspectRatios::WindowTabAspectRatios()
: CaretObject()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * Destructor.
 */
WindowTabAspectRatios::~WindowTabAspectRatios()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WindowTabAspectRatios::WindowTabAspectRatios(const WindowTabAspectRatios& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperWindowTabAspectRatios(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WindowTabAspectRatios&
WindowTabAspectRatios::operator=(const WindowTabAspectRatios& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperWindowTabAspectRatios(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WindowTabAspectRatios::copyHelperWindowTabAspectRatios(const WindowTabAspectRatios& obj)
{
    m_windowIndex           = obj.m_windowIndex;
    m_windowAspectRatio     = obj.m_windowAspectRatio;
    m_tabIndexAspectRatios  = obj.m_tabIndexAspectRatios;
}

/**
 * @param Is this instance valid?
 */
bool
WindowTabAspectRatios::isValid() const
{
    if ((m_windowIndex >= 0)
        && (m_windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)
        && ( ! m_tabIndexAspectRatios.empty())) {
        return true;
    }
    return false;
}

/**
 * @return The window index
 */
int32_t
WindowTabAspectRatios::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The window's aspect ratio
 */
float
WindowTabAspectRatios::getWindowAspectRatio() const
{
    return m_windowAspectRatio;
}

/**
 * Set the window's aspect ratio
 * @param windowIndex
 *    Index of the window
 * @param windowAspectRatio
 *    The window's aspect ratio
 */
void
WindowTabAspectRatios::setWindowAspectRatio(const int32_t windowIndex,
                                            const float windowAspectRatio)
{
    m_windowIndex       = windowIndex;
    m_windowAspectRatio = windowAspectRatio;
}

/**
 * Add the tab's aspect ratio
 * @param tabIndex
 *    Index of the tab
 * @param tabAspectRatio
 *    The tab's aspect ratio
 */
void
WindowTabAspectRatios::addTabAspectRatio(const int32_t tabIndex,
                                         const float tabAspectRatio)
{
    m_tabIndexAspectRatios.emplace(tabIndex,
                                   tabAspectRatio);
}

/**
 * @return Aspect ratio of tab with given index or negative if not found
 * @param tabIndex
 *    Index of the tab
 */
float
WindowTabAspectRatios::getTabAspectRatio(const int32_t tabIndex) const
{
    const auto iter(m_tabIndexAspectRatios.find(tabIndex));
    if (iter != m_tabIndexAspectRatios.end()) {
        return iter->second;
    }
    return -1.0;
}

std::set<int32_t>
WindowTabAspectRatios::getTabIndices() const
{
    std::set<int32_t> tabIndicesOut;
    for (const auto indexAndRatio : m_tabIndexAspectRatios) {
        tabIndicesOut.insert(indexAndRatio.first);
    }
    return tabIndicesOut;
}

/**
 * Test to see if the given tabs match those in this instance
 * @param displayedTabIndices
 *    Tabs displayed
 * @param differentTabsMessageOut
 *    Contains tab differences message
 * @return
 *    True if the tabs do not match, else false.
 */
bool
WindowTabAspectRatios::testMatchingTabs(const std::set<int32_t>& displayedTabIndices,
                                        AString& differentTabsMessageOut) const
{
    differentTabsMessageOut.clear();
    
    const std::set<int32_t> tabIndicesPreviouslyLocked(getTabIndices());
    
    if (displayedTabIndices != tabIndicesPreviouslyLocked) {
        AString newTabsMessage;
        for (const auto ti : displayedTabIndices) {
            if (tabIndicesPreviouslyLocked.find(ti) == tabIndicesPreviouslyLocked.end()) {
                newTabsMessage.append(" " + AString::number(ti + 1));
            }
        }
        
        AString closedTabsMessage;
        for (const auto ti : tabIndicesPreviouslyLocked) {
            if (displayedTabIndices.find(ti) == displayedTabIndices.end()) {
                closedTabsMessage.append(" " + AString::number(ti + 1));
            }
        }
        
        AString detailText;
        if ( ! newTabsMessage.isEmpty()) {
            differentTabsMessageOut.appendWithNewLine("These tabs have been added to the window:"
                                                      + newTabsMessage
                                                      + (".  These new tabs are locked using their "
                                                         "current aspect ratio(s)."));
        }
        if ( ! closedTabsMessage.isEmpty()) {
            differentTabsMessageOut.appendWithNewLine("These tabs have been closed since tabs were locked:"
                                                      + closedTabsMessage);
        }
        if ( ! differentTabsMessageOut.isEmpty()) {
            differentTabsMessageOut.appendWithNewLine("\n");
            differentTabsMessageOut.appendWithNewLine("Continue relocking tab aspect ratios?");
        }
    }
    return differentTabsMessageOut.isEmpty();
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
WindowTabAspectRatios::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    CaretAssertToDoFatal(); /* Not implemented yet */
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "WindowTabAspectRatios",
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
WindowTabAspectRatios::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    CaretAssertToDoFatal(); /* Not implemented yet */
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

