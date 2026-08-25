
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_KEY_DECLARE__
#include "FeatureKey.h"
#undef __FEATURE_KEY_DECLARE__

#include <QList>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::FeatureKey 
 * \brief Uniquely identifies a feature by its Group ID and its Unique ID
 * \ingroup Files
 */

/**
 * Generate a FeatureKey from a string that was produced by "toSceneKeyString".
 * @param sceneKeyString
 *    String containing encoded Group and Feature IDs
 * @return
 *    A FeatureKey instance decoded from the string.  Call isValid() to verify validity.
 */
FeatureKey
FeatureKey::fromSceneKeyString(const AString& sceneKeyString)
{
    QStringList sl(sceneKeyString.split("::"));
    bool validGroupIDFlag(false);
    bool validUniqueIDFlag(false);
    
    uint64_t groupID(0);
    uint64_t uniqueID(0);
    
    if (sl.size() == 2) {
        const AString groupString(sl.at(0));
        const AString featureString(sl.at(1));
        
        groupID = groupString.toULong(&validGroupIDFlag);
        
        uniqueID = featureString.toULong(&validUniqueIDFlag);
    }
    
    FeatureKey fk(groupID,
                  uniqueID);
    fk.m_validFlag =  (validGroupIDFlag
                       && validUniqueIDFlag);
    return fk;
}

/*
 * Constructor
 * @param groupID
 *    The group ID
 * @param uniqueID
 *    The unique ID
 */
FeatureKey::FeatureKey(const uint64_t groupID,
                       const uint64_t uniqueID)
: m_groupID(groupID),
m_uniqueID(uniqueID) {
    m_validFlag = true;
}

/**
 * Constructor of invalid instance.
 */
FeatureKey::FeatureKey()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
FeatureKey::~FeatureKey()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
FeatureKey::FeatureKey(const FeatureKey& obj)
: CaretObject(obj)
{
    this->copyHelperFeatureKey(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
FeatureKey&
FeatureKey::operator=(const FeatureKey& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperFeatureKey(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
FeatureKey::copyHelperFeatureKey(const FeatureKey& obj)
{
    m_groupID  = obj.m_groupID;
    m_uniqueID = obj.m_uniqueID;
}

/**
 * @return True if the group and feature id's are valid
 */
bool
FeatureKey::isValid() const
{
    return m_validFlag;
}

/**
 * @return The Group ID
 */
uint64_t
FeatureKey::getGroupID() const
{
    return m_groupID;
}

/**
 * @return The Unique ID
 */
uint64_t
FeatureKey::getUniqueID() const
{
    return m_uniqueID;
}

/**
 * Set the unique ID
 * @param uniqueID
 *    New unique ID
 */
void
FeatureKey::setUniqueID(const uint64_t uniqueID)
{
    m_uniqueID = uniqueID;
}

/**
 * @return Group ID and Feature ID encoded in a string
 * for use with scenes.
 */
AString
FeatureKey::toSceneKeyString() const {
    return (AString::number(m_groupID)
            + "::"
            + AString::number(m_uniqueID));
}


/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
FeatureKey::operator==(const FeatureKey& obj) const
{
    if (this == &obj) {
        return true;    
    }

    if (m_validFlag
        && obj.m_validFlag) {
        if ((m_groupID == obj.m_groupID)
            && (m_uniqueID == obj.m_uniqueID)) {
            return true;
        }
    }
    return false;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
FeatureKey::toString() const
{
    return "FeatureKey";
}

