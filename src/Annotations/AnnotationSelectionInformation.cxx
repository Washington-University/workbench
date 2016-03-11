
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

#include <set>

#define __ANNOTATION_SELECTION_INFORMATION_DECLARE__
#include "AnnotationSelectionInformation.h"
#undef __ANNOTATION_SELECTION_INFORMATION_DECLARE__

#include "Annotation.h"
#include "AnnotationGroup.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSelectionInformation 
 * \brief Contains selected annotations and related information.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *    Index of window in which annotations are selected.
 */
AnnotationSelectionInformation::AnnotationSelectionInformation(const int32_t windowIndex)
: CaretObject(),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
AnnotationSelectionInformation::~AnnotationSelectionInformation()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationSelectionInformation::AnnotationSelectionInformation(const AnnotationSelectionInformation& obj)
: CaretObject(obj),
m_windowIndex(obj.m_windowIndex)
{
    this->copyHelperAnnotationSelectionInformation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationSelectionInformation&
AnnotationSelectionInformation::operator=(const AnnotationSelectionInformation& obj)
{
    if (this != &obj) {
        if (m_windowIndex == obj.m_windowIndex) {
            CaretObject::operator=(obj);
            this->copyHelperAnnotationSelectionInformation(obj);
        }
        else {
            const QString msg("Cannot copy AnnotationSelectionInformation to different window.");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
            m_annotations.clear();
        }
    }
    
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationSelectionInformation::copyHelperAnnotationSelectionInformation(const AnnotationSelectionInformation& obj)
{
    CaretAssertMessage(0, "Copying not allowed");
    m_annotations = obj.m_annotations;
}

/**
 *
 */
void
AnnotationSelectionInformation::clear()
{
    m_annotations.clear();
    m_annotationGroupKeys.clear();
    
    m_groupingValid = false;
    m_regroupValid  = false;
    m_ungroupValid  = false;
}

/**
 * @return Index of window in which annotations are selected.
 */
int32_t
AnnotationSelectionInformation::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Update the selected annotations.  If any of the given annotations
 * are in a 'user' annotation group, all annotations in the 'user' 
 * annotation group are selected.
 *
 * @param annotation.
 */
void
AnnotationSelectionInformation::update(const std::vector<Annotation*>& selectedAnnotations)
{
    clear();
    
    m_annotations = selectedAnnotations;
    
    std::set<AnnotationGroupKey> groupKeysSet;
    for (std::vector<Annotation*>::iterator annIter = m_annotations.begin();
         annIter != m_annotations.end();
         annIter++) {
        groupKeysSet.insert((*annIter)->getAnnotationGroupKey());
    }
    
//    std::set<const AnnotationGroup*> groupSet;
//    for (std::vector<Annotation*>::iterator annIter = m_annotations.begin();
//         annIter != m_annotations.end();
//         annIter++) {
//        const AnnotationGroup* annGroup = (*annIter)->getAnnotationGroup();
//        CaretAssert(annGroup);
//        groupSet.insert(annGroup);
//    }
    
    m_annotationGroupKeys.insert(m_annotationGroupKeys.end(),
                              groupKeysSet.begin(),
                              groupKeysSet.end());
    
    const int32_t numGroups = static_cast<int32_t>(m_annotationGroupKeys.size());
    
    if (m_annotations.size() > 1) {
        if (numGroups == 1) {
            CaretAssertVectorIndex(m_annotationGroupKeys, 0);
            const AnnotationGroupKey& groupKey = m_annotationGroupKeys[0];
            
            switch (groupKey.getGroupType()) {
                case AnnotationGroupTypeEnum::INVALID:
                    break;
                case AnnotationGroupTypeEnum::SPACE:
                    /*
                     * All annotations in a space group, allow creating a user group
                     */
                    m_groupingValid = true;
                    break;
                case AnnotationGroupTypeEnum::USER:
                    /*
                     * All annotations in a user group, allow ungrouping to a space group
                     */
                    m_ungroupValid = true;
                    break;
            }
        }
    }
    
    if (numGroups > 0) {
        /*
         * If all annotations are in a user group and were 
         * in the same previous user group, enable regroup.
         */
        if (numGroups == 1) {
            
        }
    }
}

/**
 * @return Vector containing the selected annotation group keys.
 */
std::vector<AnnotationGroupKey>
AnnotationSelectionInformation::getSelectedAnnotationGroupKeys() const
{
    return m_annotationGroupKeys;
}

/**
 * @return Number of annotations selected.
 */
int32_t
AnnotationSelectionInformation::getNumberOfSelectedAnnotations() const
{
    return m_annotations.size();
}

/**
 * @return True if any annotations are selected.
 */
bool
AnnotationSelectionInformation::isAnyAnnotationSelected() const
{
    return ( ! m_annotations.empty());
}

/**
 * @return Vector containing the selected annotations.
 */
std::vector<Annotation*>
AnnotationSelectionInformation::getSelectedAnnotations() const
{
    return m_annotations;
}

/**
 * Get the selected annotations.
 *
 * @param annotationsOut
 *    Output containing the selected anntotations.
 */
void
AnnotationSelectionInformation::getSelectedAnnotations(std::vector<Annotation*>& annotationsOut) const
{
    annotationsOut = m_annotations;
}

bool
AnnotationSelectionInformation::isGroupingModeValid(const AnnotationGroupingModeEnum::Enum groupingMode) const
{
    bool valid = false;
    
    switch (groupingMode) {
        case  AnnotationGroupingModeEnum::GROUP:
            valid = m_groupingValid;
            break;
        case AnnotationGroupingModeEnum::REGROUP:
            valid = m_regroupValid;
            break;
        case AnnotationGroupingModeEnum::UNGROUP:
            valid = m_ungroupValid;
            break;
    }
    
    return valid;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationSelectionInformation::toString() const
{
    return "AnnotationSelectionInformation";
}

