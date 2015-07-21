
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

#define __MARGIN_DECLARE__
#include "Margin.h"
#undef __MARGIN_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::Margin 
 * \brief Margins (left, right, bottom, top).  Typicallay around a tab or window region.
 * \ingroup Brain
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
Margin::Margin()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addArray("m_margin", m_margin, 4, 0);
}

/**
 * Destructor.
 */
Margin::~Margin()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Margin::Margin(const Margin& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperMargin(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Margin&
Margin::operator=(const Margin& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperMargin(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Margin::copyHelperMargin(const Margin& obj)
{
    m_margin[0] = obj.m_margin[0];
    m_margin[1] = obj.m_margin[1];
    m_margin[2] = obj.m_margin[2];
    m_margin[3] = obj.m_margin[3];
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
Margin::getMargins(int32_t& leftOut,
                                      int32_t& rightOut,
                                      int32_t& bottomOut,
                                      int32_t& topOut) const
{
    leftOut   = m_margin[0];
    rightOut  = m_margin[1];
    bottomOut = m_margin[2];
    topOut    = m_margin[3];
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
Margin::setMargins(const int32_t left,
                                      const int32_t right,
                                      const int32_t bottom,
                                      const int32_t top)
{
    m_margin[0] = left;
    m_margin[1] = right;
    m_margin[2] = bottom;
    m_margin[3] = top;
}

/**
 * Reset the margins to zero.
 */
void
Margin::reset()
{
    m_margin[0] = 0;
    m_margin[1] = 0;
    m_margin[2] = 0;
    m_margin[3] = 0;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Margin::toString() const
{
    return "Margin";
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
Margin::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Margin",
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
Margin::restoreFromScene(const SceneAttributes* sceneAttributes,
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

