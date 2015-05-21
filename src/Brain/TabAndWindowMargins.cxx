
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __TAB_AND_WINDOW_MARGINS_DECLARE__
#include "TabAndWindowMargins.h"
#undef __TAB_AND_WINDOW_MARGINS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::TabAndWindowMargins 
 * \brief Margins for Windows and Tabs
 * \ingroup Brain
 */

/**
 * Constructor.
 */
TabAndWindowMargins::TabAndWindowMargins()
: CaretObject()
{
    m_tabMargins[0] = 0;
    m_tabMargins[1] = 0;
    m_tabMargins[2] = 0;
    m_tabMargins[3] = 0;
    
    m_windowMargins[0] = 0;
    m_windowMargins[1] = 0;
    m_windowMargins[2] = 0;
    m_windowMargins[3] = 0;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addArray("m_tabMargins", m_tabMargins, 4, 0);
    m_sceneAssistant->addArray("m_windowMargins", m_windowMargins, 4, 0);
}

/**
 * Destructor.
 */
TabAndWindowMargins::~TabAndWindowMargins()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TabAndWindowMargins::TabAndWindowMargins(const TabAndWindowMargins& obj)
: CaretObject(obj)
{
    this->copyHelperTabAndWindowMargins(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TabAndWindowMargins&
TabAndWindowMargins::operator=(const TabAndWindowMargins& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTabAndWindowMargins(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TabAndWindowMargins::copyHelperTabAndWindowMargins(const TabAndWindowMargins& obj)
{
    
}

/**
 * Get the window margins.
 *
 * @param leftOut
 *    left margin
 * @param rightOut
 *    right margin
 * @param bottomOut
 *    bottom margin
 * @param topOut
 *    top margin
 */
void
TabAndWindowMargins::getWindowMargins(int32_t& leftOut,
                                      int32_t& rightOut,
                                      int32_t& bottomOut,
                                      int32_t& topOut) const
{
    leftOut   = m_windowMargins[0];
    rightOut  = m_windowMargins[1];
    bottomOut = m_windowMargins[2];
    topOut    = m_windowMargins[3];
}

/**
 * Set the window margins.
 *
 * @param left
 *    left margin
 * @param right
 *    right margin
 * @param bottom
 *    bottom margin
 * @param top
 *    top margin
 */
void
TabAndWindowMargins::setWindowMargins(const int32_t left,
                                      const int32_t right,
                                      const int32_t bottom,
                                      const int32_t top)
{
    m_windowMargins[0] = left;
    m_windowMargins[1] = right;
    m_windowMargins[2] = bottom;
    m_windowMargins[3] = top;
}

/**
 * Get the tab margins.
 *
 * @param leftOut
 *    left margin
 * @param rightOut
 *    right margin
 * @param bottomOut
 *    bottom margin
 * @param topOut
 *    top margin
 */
void
TabAndWindowMargins::getTabMargins(int32_t& leftOut,
                                      int32_t& rightOut,
                                      int32_t& bottomOut,
                                      int32_t& topOut) const
{
    leftOut   = m_tabMargins[0];
    rightOut  = m_tabMargins[1];
    bottomOut = m_tabMargins[2];
    topOut    = m_tabMargins[3];
}

/**
 * Set the tab margins.
 *
 * @param left
 *    left margin
 * @param right
 *    right margin
 * @param bottom
 *    bottom margin
 * @param top
 *    top margin
 */
void
TabAndWindowMargins::setTabMargins(const int32_t left,
                                      const int32_t right,
                                      const int32_t bottom,
                                      const int32_t top)
{
    m_tabMargins[0] = left;
    m_tabMargins[1] = right;
    m_tabMargins[2] = bottom;
    m_tabMargins[3] = top;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TabAndWindowMargins::toString() const
{
    return "TabAndWindowMargins";
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
TabAndWindowMargins::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "TabAndWindowMargins",
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
TabAndWindowMargins::restoreFromScene(const SceneAttributes* sceneAttributes,
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

