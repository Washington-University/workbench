
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

#define __SPACER_TAB_INDEX_DECLARE__
#include "SpacerTabIndex.h"
#undef __SPACER_TAB_INDEX_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::SpacerTabIndex 
 * \brief Index of a spacer tab in a tile tabs configuration
 * \ingroup Scenes
 */

/**
 * Constructor of invalid element.
 */
SpacerTabIndex::SpacerTabIndex()
: CaretObject(),
m_windowIndex(-1),
m_rowIndex(-1),
m_columnIndex(-1)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_windowIndex",
                          &m_windowIndex);
    m_sceneAssistant->add("m_rowIndex",
                          &m_rowIndex);
    m_sceneAssistant->add("m_columnIndex",
                          &m_columnIndex);
}

/**
 * Constructor with indices
 */
SpacerTabIndex::SpacerTabIndex(const int32_t windowIndex,
                               const int32_t rowIndex,
                               const int32_t columnIndex)
: CaretObject(),
m_windowIndex(windowIndex),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_windowIndex",
                          &m_windowIndex);
    m_sceneAssistant->add("m_rowIndex",
                          &m_rowIndex);
    m_sceneAssistant->add("m_columnIndex",
                          &m_columnIndex);
}

/**
 * Destructor.
 */
SpacerTabIndex::~SpacerTabIndex()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SpacerTabIndex::SpacerTabIndex(const SpacerTabIndex& obj)
: CaretObject(obj)
{
    this->copyHelperSpacerTabIndex(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SpacerTabIndex&
SpacerTabIndex::operator=(const SpacerTabIndex& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperSpacerTabIndex(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SpacerTabIndex::copyHelperSpacerTabIndex(const SpacerTabIndex& obj)
{
    m_windowIndex = obj.m_windowIndex;
    m_rowIndex    = obj.m_rowIndex;
    m_columnIndex = obj.m_columnIndex;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
SpacerTabIndex::operator==(const SpacerTabIndex& obj) const
{
    if (this == &obj) {
        return true;    
    }

    if ((m_windowIndex == obj.m_windowIndex)
        && (m_rowIndex == obj.m_rowIndex)
        && (m_columnIndex == obj.m_columnIndex)) {
        return true;
    }
    
    return false;    
}

/**
 * Comparison operator.
 *
 * @param rhs
 *     Other instance for comparison
 * @return True if 'this' is less than 'rhs', else false.
 */
bool
SpacerTabIndex::operator<(const SpacerTabIndex& rhs) const
{
    if (m_windowIndex != rhs.m_windowIndex) {
        return (m_windowIndex < rhs.m_windowIndex);
    }
    if (m_rowIndex != rhs.m_rowIndex) {
        return (m_rowIndex < rhs.m_rowIndex);
    }
    if (m_columnIndex != rhs.m_columnIndex) {
        return (m_columnIndex < rhs.m_columnIndex);
    }
    
    return false;
}

/**
 * @return The window index
 */
int32_t
SpacerTabIndex::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The row index
 */
int32_t
SpacerTabIndex::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return The column index
 */
int32_t
SpacerTabIndex::getColumnIndex() const
{
    return m_columnIndex;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpacerTabIndex::toString() const
{
    AString s("SpacerTabIndex: windowIndex=%1, rowIndex=%2, columnIndex=%3");
    s = s.arg(m_windowIndex).arg(m_rowIndex).arg(m_columnIndex);

    return s;
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
SpacerTabIndex::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SpacerTabIndex",
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
SpacerTabIndex::restoreFromScene(const SceneAttributes* sceneAttributes,
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

