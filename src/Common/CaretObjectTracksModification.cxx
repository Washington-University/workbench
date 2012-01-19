
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

#define __CARET_OBJECT_TRACKS_MODIFICATION_DECLARE__
#include "CaretObjectTracksModification.h"
#undef __CARET_OBJECT_TRACKS_MODIFICATION_DECLARE__

using namespace caret;


    
/**
 * \class caret::CaretObjectTracksModification 
 * \brief CaretObject base class with implementation of tracks modification interface.
 */
/**
 * Constructor.
 */
CaretObjectTracksModification::CaretObjectTracksModification()
: CaretObject(),
  TracksModificationInterface()
{
    this->modifiedFlag = false;
}

/**
 * Destructor.
 */
CaretObjectTracksModification::~CaretObjectTracksModification()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CaretObjectTracksModification::CaretObjectTracksModification(const CaretObjectTracksModification& obj)
: CaretObject(obj),
  TracksModificationInterface()
{
    this->copyHelperCaretObjectTracksModification(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CaretObjectTracksModification&
CaretObjectTracksModification::operator=(const CaretObjectTracksModification& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCaretObjectTracksModification(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CaretObjectTracksModification::copyHelperCaretObjectTracksModification(const CaretObjectTracksModification& /*obj*/)
{
    this->modifiedFlag = false; // do not copy modification status
}

/**
 * Set the status to modified.
 */
void 
CaretObjectTracksModification::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set the status to unmodified.
 */
void 
CaretObjectTracksModification::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool 
CaretObjectTracksModification::isModified() const
{
    return this->modifiedFlag;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CaretObjectTracksModification::toString() const
{
    const AString text = (CaretObject::toString()
                          + "\nCaretObjectTracksModification::modifiedFlag=" + AString::fromBool(this->modifiedFlag));
    
    return text;
}
