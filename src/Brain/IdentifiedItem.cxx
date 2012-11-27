
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

using namespace caret;


    
/**
 * \class caret::IdentifiedItem
 * \brief Describes an identified item.
 */

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
    
}

/**
 * Destructor.
 */
IdentifiedItem::~IdentifiedItem()
{
    
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
    return "IdentifiedItem";
}
