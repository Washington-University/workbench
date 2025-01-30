
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ANNOTATION_GROUP_KEY_DECLARE__
#include "AnnotationGroupKey.h"
#undef __ANNOTATION_GROUP_KEY_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationGroupKey 
 * \brief This class is a "key" for an annotation group.
 * \ingroup Annotations
 *
 * This class is a member of each annotation and tracks the
 * group to which the annotation belongs.
 *
 * There are four types of annotation groups.
 * By default, annotations are assigned to a 'Space' group when
 * added to an annotation file.  In an annotation file, there
 * is one group for each space (stereotaxic, surface, each tab,
 * and each window).  A user may create 'User' groups to group
 * annotation that are in the same space.
 *
 * SAMPLES_ACTUAL and SAMPLES_DESIRED are used
 * by the SamplesFiles to group polyhedron samples.
 *
 * Creating a user group:
 *    - The group type is set to USER.
 *    - The user group unique key is set to the key in the user group.
 *    - The space key is invalidate.
 *
 * Ungrouping a user group:
 *    - The group type is set to SPACE.
 *    - The space group unique key is set to the key in the space group.
 *    - The user group unique is not changed.  It is used by a regroup operation.
 *
 * Regrouping a user group:
 *    - The group type is changed to USER.
 *    - A user group is created with the user group unique key and all annotations
 *    that have the user group unique key are added to the user group.
 */

/**
 * Constructor.
 */
AnnotationGroupKey::AnnotationGroupKey()
: CaretObject(),
m_annotationFile(NULL),
m_groupType(AnnotationGroupTypeEnum::INVALID),
m_spaceGroupUniqueKey(-1),
m_userGroupUniqueKey(-1),
m_samplesActualGroupUniqueKey(-1),
m_samplesDesiredGroupUniqueKey(-1)
{
    reset();
}

/**
 * Destructor.
 */
AnnotationGroupKey::~AnnotationGroupKey()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationGroupKey::AnnotationGroupKey(const AnnotationGroupKey& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationGroupKey(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationGroupKey&
AnnotationGroupKey::operator=(const AnnotationGroupKey& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationGroupKey(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationGroupKey::copyHelperAnnotationGroupKey(const AnnotationGroupKey& obj)
{
    m_annotationFile      = obj.m_annotationFile;
    m_groupType           = obj.m_groupType;
    m_spaceGroupUniqueKey = obj.m_spaceGroupUniqueKey;
    m_userGroupUniqueKey  = obj.m_userGroupUniqueKey;
    m_samplesActualGroupUniqueKey  = obj.m_samplesActualGroupUniqueKey;
    m_samplesDesiredGroupUniqueKey = obj.m_samplesDesiredGroupUniqueKey;
}

/**
 * Reset an instance to invalid.
 */
void
AnnotationGroupKey::reset()
{
    m_annotationFile      = NULL;
    m_groupType           = AnnotationGroupTypeEnum::INVALID;
    m_spaceGroupUniqueKey = -1;
    m_userGroupUniqueKey  = -1;
    m_samplesActualGroupUniqueKey  = -1;
    m_samplesDesiredGroupUniqueKey = -1;
}


/**
 * Equality operator.  Equal if annotation file is equal AND
 * either the group type is space and the space unique keys are
 * equal or the group type is user and the user unique keys are
 * equal.
 * 
 * @param groupKey
 *     The group key that is tested for equality.
 * @return
 *     True if these group keys are equal, else false.
 */
bool
AnnotationGroupKey::operator==(const AnnotationGroupKey& groupKey) const
{
    if (this == &groupKey) {
        return true;
    }
    if (m_annotationFile == groupKey.m_annotationFile) {
        if (m_groupType == groupKey.m_groupType) {
            switch (m_groupType) {
                case AnnotationGroupTypeEnum::INVALID:
                    CaretAssertMessage(0, "Should never get here.");
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_ACTUAL:
                    if (m_samplesActualGroupUniqueKey == groupKey.m_samplesActualGroupUniqueKey) {
                        return true;
                    }
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_DESIRED:
                    if (m_samplesDesiredGroupUniqueKey == groupKey.m_samplesDesiredGroupUniqueKey) {
                        return true;
                    }
                    break;
                case AnnotationGroupTypeEnum::SPACE:
                    if (m_spaceGroupUniqueKey == groupKey.m_spaceGroupUniqueKey) {
                        return true;
                    }
                    break;
                case AnnotationGroupTypeEnum::USER:
                    if (m_userGroupUniqueKey == groupKey.m_userGroupUniqueKey) {
                        return true;
                    }
                    break;
            }
        }
    }
    
    return false;
}

/**
 * Less than operator.  
 *
 * @param groupKey
 *     The group key that is tested for equality.
 * @return
 *     True if these group keys are equal, else false.
 */
bool
AnnotationGroupKey::operator<(const AnnotationGroupKey& groupKey) const
{
    if (this == &groupKey) {
        return false;
    }
    
    bool lessThanFlag = false;
    
    if (m_annotationFile == groupKey.m_annotationFile) {
        if (m_groupType == groupKey.m_groupType) {
            switch (m_groupType) {
                case AnnotationGroupTypeEnum::INVALID:
                    CaretAssertMessage(0, "Should never get here.");
                    lessThanFlag = false;
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_ACTUAL:
                    lessThanFlag = (m_samplesActualGroupUniqueKey < groupKey.m_samplesActualGroupUniqueKey);
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_DESIRED:
                    lessThanFlag = (m_samplesDesiredGroupUniqueKey < groupKey.m_samplesDesiredGroupUniqueKey);
                    break;
                case AnnotationGroupTypeEnum::SPACE:
                    lessThanFlag = (m_spaceGroupUniqueKey < groupKey.m_spaceGroupUniqueKey);
                    break;
                case AnnotationGroupTypeEnum::USER:
                    lessThanFlag = (m_userGroupUniqueKey < groupKey.m_userGroupUniqueKey);
                    break;
            }
            
        }
        else {
            lessThanFlag = (AnnotationGroupTypeEnum::toIntegerCode(m_groupType)
                            < AnnotationGroupTypeEnum::toIntegerCode(groupKey.m_groupType));
        }
    }
    else {
        lessThanFlag = (m_annotationFile < groupKey.m_annotationFile);
    }
    
    return lessThanFlag;
}


/**
 * @return The annotation file.
 */
AnnotationFile*
AnnotationGroupKey::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * Set the annotation file.
 *
 * @param annotationFile
 *     The annotation file.
 */
void
AnnotationGroupKey::setAnnotationFile(AnnotationFile* annotationFile)
{
    m_annotationFile = annotationFile;
}

/**
 * @return The group type.
 */
AnnotationGroupTypeEnum::Enum
AnnotationGroupKey::getGroupType() const
{
    return m_groupType;
}

/**
 * Set the group type.
 *    
 * @param groupType
 *     The group type.
 */
void
AnnotationGroupKey::setGroupType(const AnnotationGroupTypeEnum::Enum groupType)
{
    m_groupType = groupType;
}

/**
 * @return The space group unique key.
 */
int32_t
AnnotationGroupKey::getSpaceGroupUniqueKey() const
{
    return m_spaceGroupUniqueKey;
}

/**
 * Set the space group unique key.
 *
 * @param spaceGroupUniqueKey
 *     The space group unique key.
 */
void
AnnotationGroupKey::setSpaceGroupUniqueKey(const int32_t spaceGroupUniqueKey)
{
    m_spaceGroupUniqueKey = spaceGroupUniqueKey;
}

/**
 * @return The user group unique key.
 */
int32_t
AnnotationGroupKey::getUserGroupUniqueKey() const
{
    return m_userGroupUniqueKey;
}

/**
 * @return The samples actual group unique key.
 */
int32_t
AnnotationGroupKey::getSamplesActualUniqueKey() const
{
    return m_samplesActualGroupUniqueKey;
}

/**
 * @return The samples desired group unique key.
 */
int32_t
AnnotationGroupKey::getSamplesDesiredUniqueKey() const
{
    return m_samplesDesiredGroupUniqueKey;
}

/**
 * Set the user group unique key.
 *
 * @param userGroupUniqueKey
 *     The user group unique key.
 */
void
AnnotationGroupKey::setUserGroupUniqueKey(const int32_t userGroupUniqueKey)
{
    m_userGroupUniqueKey = userGroupUniqueKey;
}

/**
 * Set the samles actual group unique key.
 *
 * @param samplesActualUniqueKey
 *     The samples actual group unique key.
 */
void
AnnotationGroupKey::setSamplesActualUniqueKey(const int32_t samplesActualUniqueKey)
{
    m_samplesActualGroupUniqueKey = samplesActualUniqueKey;
}

/**
 * Set the samples desired group unique key.
 *
 * @param samplesDesiredUniqueKey
 *     The samples desired group unique key.
 */
void
AnnotationGroupKey::setSamplesDesiredUniqueKey(const int32_t samplesDesiredUniqueKey)
{
    m_samplesDesiredGroupUniqueKey = samplesDesiredUniqueKey;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationGroupKey::toString() const
{
    
    return ("AnnotationGroupKey: "
            + AnnotationGroupTypeEnum::toName(m_groupType)
            + " spaceKey=" + AString::number(m_spaceGroupUniqueKey)
            + " userGroupKey=" + AString::number(m_userGroupUniqueKey)
            + " samplesActualGroupKey=" + AString::number(m_samplesActualGroupUniqueKey)
            + " samplesDesiredGroupKey=" + AString::number(m_samplesDesiredGroupUniqueKey));
}

