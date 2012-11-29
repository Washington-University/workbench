
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __IDENTIFIED_ITEM_DECLARE__
#include "IdentifiedItem.h"
#undef __IDENTIFIED_ITEM_DECLARE__

#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItem
 * \brief Describes an identified item.
 */


/**
 * Constructor.
 *
 */
IdentifiedItem::IdentifiedItem()
: CaretObject(),
  m_text("")
{
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_text", &m_text);
}

/**
 * Constructor.
 *
 * @param text
 *    Text describing the identified item.
 */
IdentifiedItem::IdentifiedItem(const AString& text)
: CaretObject(),
m_text(text)
{
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_text", &m_text);
}

/**
 * Destructor.
 */
IdentifiedItem::~IdentifiedItem()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItem::IdentifiedItem(const IdentifiedItem& obj)
: CaretObject(obj)
{
    this->copyHelperIdentifiedItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItem&
IdentifiedItem::operator=(const IdentifiedItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperIdentifiedItem(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItem::copyHelperIdentifiedItem(const IdentifiedItem& obj)
{
    m_text = obj.m_text;
}

/**
 * Append text to this item's text.
 */
void
IdentifiedItem::appendText(const AString& text)
{
    m_text += text;
}

/**
 * Clear the text for this item.
 */
void
IdentifiedItem::clearText()
{
    m_text = "";
}

/**
 * @return The text describing the identified item.
 */
AString
IdentifiedItem::getText() const
{
    return m_text;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentifiedItem::toString() const
{
    return ("m_text=" + m_text);
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
IdentifiedItem::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItem",
                                            1);
    
    saveMembers(sceneAttributes, sceneClass);
    
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
IdentifiedItem::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    restoreMembers(sceneAttributes, sceneClass);
}

/**
 * Restore members (protected function for derived classes).
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.
 */
void
IdentifiedItem::restoreMembers(const SceneAttributes* sceneAttributes,
                    const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Save members (protected function for derived classes).
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.
 */
void
IdentifiedItem::saveMembers(const SceneAttributes* sceneAttributes,
                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes, sceneClass);
}
