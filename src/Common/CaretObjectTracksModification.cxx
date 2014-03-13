
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
