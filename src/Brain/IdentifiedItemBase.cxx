
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __IDENTIFIED_ITEM_BASE_DECLARE__
#include "IdentifiedItemBase.h"
#undef __IDENTIFIED_ITEM_BASE_DECLARE__

#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItemBase
 * \brief Base class for identified items
 */


/**
 * Constructor.
 *
 */
IdentifiedItemBase::IdentifiedItemBase()
: CaretObject()
{
    initializeMembers();
}

/**
 * Constructor.
 *
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 */
IdentifiedItemBase::IdentifiedItemBase(const AString& simpleText,
                               const AString& formattedText)
: CaretObject()
{
    initializeMembers();
    m_text = simpleText;
    m_formattedText = formattedText;
}

/**
 * Destructor.
 */
IdentifiedItemBase::~IdentifiedItemBase()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItemBase::IdentifiedItemBase(const IdentifiedItemBase& obj)
: CaretObject(obj), SceneableInterface()
{
    this->initializeMembers();
    
    this->copyHelperIdentifiedItemBase(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItemBase&
IdentifiedItemBase::operator=(const IdentifiedItemBase& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperIdentifiedItemBase(obj);
    }
    return *this;    
}

/**
 * Initialize a new instance of this class.
 */
void
IdentifiedItemBase::initializeMembers()
{
    m_text.clear();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_text", &m_text);
    m_sceneAssistant->add("m_formattedText", &m_formattedText);
}
                          

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItemBase::copyHelperIdentifiedItemBase(const IdentifiedItemBase& obj)
{
    m_text = obj.m_text;
}

/**
 * @return Is this item valid?  Typically only used when restoring
 * from scene.
 */
bool
IdentifiedItemBase::isValid() const
{
    if (m_text.isEmpty() == false) {
        return true;
    }
    return false;
}

/**
 * Append text to this item's text.
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 */
void
IdentifiedItemBase::appendText(const AString& simpleText,
                           const AString& formattedText)
{
    m_text += simpleText;
    if (m_formattedText.isEmpty()) {
        m_formattedText.append("\n");
    }
    m_formattedText += formattedText;
}

/**
 * Clear the text for this item.
 */
void
IdentifiedItemBase::clearText()
{
    m_text = "";
}

/**
 * @return The simple text describing the identified item.
 */
AString
IdentifiedItemBase::getSimpleText() const
{
    return m_text;
}

/**
 * @return The formatted text describing the identified item.
 */
AString
IdentifiedItemBase::getFormattedText() const
{
    return m_formattedText;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentifiedItemBase::toString() const
{
    return ("m_text=" + m_text
            + "m_formattedText=" + m_formattedText);
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
IdentifiedItemBase::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItemBase",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, sceneClass);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
IdentifiedItemBase::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

