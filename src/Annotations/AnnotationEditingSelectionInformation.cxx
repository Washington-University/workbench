
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
#include "AnnotationEditingSelectionInformation.h"
#undef __ANNOTATION_SELECTION_INFORMATION_DECLARE__

#include "Annotation.h"
#include "AnnotationGroup.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::AnnotationEditingSelectionInformation 
 * \brief Contains selected annotations and related information.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *    Index of window in which annotations are selected.
 */
AnnotationEditingSelectionInformation::AnnotationEditingSelectionInformation(const int32_t windowIndex)
: CaretObject(),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
AnnotationEditingSelectionInformation::~AnnotationEditingSelectionInformation()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationEditingSelectionInformation::AnnotationEditingSelectionInformation(const AnnotationEditingSelectionInformation& obj)
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
AnnotationEditingSelectionInformation&
AnnotationEditingSelectionInformation::operator=(const AnnotationEditingSelectionInformation& obj)
{
    if (this != &obj) {
        if (m_windowIndex == obj.m_windowIndex) {
            CaretObject::operator=(obj);
            this->copyHelperAnnotationSelectionInformation(obj);
        }
        else {
            const QString msg("Cannot copy AnnotationEditingSelectionInformation to different window.");
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
AnnotationEditingSelectionInformation::copyHelperAnnotationSelectionInformation(const AnnotationEditingSelectionInformation& obj)
{
    CaretAssertMessage(0, "Copying not allowed");
    m_annotations = obj.m_annotations;
}

/**
 *
 */
void
AnnotationEditingSelectionInformation::clear()
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
AnnotationEditingSelectionInformation::getWindowIndex() const
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
AnnotationEditingSelectionInformation::update(const std::vector<Annotation*>& selectedAnnotations)
{
    clear();
    
    m_annotations = selectedAnnotations;
    
    bool allAnnotationsGroupableFlag = false;
    std::set<AnnotationGroupKey> groupKeysSet;
    if ( ! m_annotations.empty()) {
        allAnnotationsGroupableFlag = true;
        for (auto ann : m_annotations) {
            if (ann->testProperty(Annotation::Property::GROUP)) {
                groupKeysSet.insert(ann->getAnnotationGroupKey());
            }
            else {
                allAnnotationsGroupableFlag = false;
            }
        }
    }
    
    if (allAnnotationsGroupableFlag) {
        m_annotationGroupKeys.insert(m_annotationGroupKeys.end(),
                                     groupKeysSet.begin(),
                                     groupKeysSet.end());
    }
    
    const int32_t numGroups = static_cast<int32_t>(m_annotationGroupKeys.size());
    
    /*
     * Are TWO or more annotations selected
     */
        if (m_annotations.size() > 1) {
            if (numGroups == 1) {
                CaretAssertVectorIndex(m_annotationGroupKeys, 0);
                const AnnotationGroupKey& groupKey = m_annotationGroupKeys[0];
                
                switch (groupKey.getGroupType()) {
                    case AnnotationGroupTypeEnum::INVALID:
                        break;
                    case AnnotationGroupTypeEnum::SAMPLES_ACTUAL:
                        break;
                    case AnnotationGroupTypeEnum::SAMPLES_DESIRED:
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
    
    /*
     * Is ANY annotation selected
     */
    if (m_annotations.size() > 0) {
        /*
         * If all annotations are in a space group and were
         * in the same previous user group, enable regroup.
         */
        if (numGroups == 1) {
            CaretAssertVectorIndex(m_annotationGroupKeys, 0);
            const AnnotationGroupKey& groupKey = m_annotationGroupKeys[0];
            
            switch (groupKey.getGroupType()) {
                case AnnotationGroupTypeEnum::INVALID:
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_ACTUAL:
                    break;
                case AnnotationGroupTypeEnum::SAMPLES_DESIRED:
                    break;
                case AnnotationGroupTypeEnum::SPACE:
                    /*
                     * The current group type is a 'space' group but 
                     * the annotation(s) were previously in a 'user'
                     * group so enable 'regroup'.
                     */
                    if (groupKey.getUserGroupUniqueKey() > 0) {
                        m_regroupValid = true;
                    }
                    break;
                case AnnotationGroupTypeEnum::USER:
                    break;
            }
        }
    }
}

/**
 * @return Vector containing the selected annotation group keys.
 */
std::vector<AnnotationGroupKey>
AnnotationEditingSelectionInformation::getSelectedAnnotationGroupKeys() const
{
    return m_annotationGroupKeys;
}

/**
 * @return Number of annotations selected.
 */
int32_t
AnnotationEditingSelectionInformation::getNumberOfSelectedAnnotations() const
{
    return m_annotations.size();
}

/**
 * @return True if any annotations are selected.
 */
bool
AnnotationEditingSelectionInformation::isAnyAnnotationSelected() const
{
    return ( ! m_annotations.empty());
}

/**
 * @return Vector containing the selected annotations.
 */
std::vector<Annotation*>
AnnotationEditingSelectionInformation::getAnnotationsSelectedForEditing() const
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
AnnotationEditingSelectionInformation::getAnnotationsSelectedForEditing(std::vector<Annotation*>& annotationsOut) const
{
    annotationsOut = m_annotations;
}

/**
 * Is the given grouping mode valid?
 *
 * @param groupingMode
 *     The grouping mode.
 * @return
 *     True if the grouping mode is valid, else false.
 */
bool
AnnotationEditingSelectionInformation::isGroupingModeValid(const AnnotationGroupingModeEnum::Enum groupingMode) const
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
AnnotationEditingSelectionInformation::toString() const
{
    return "AnnotationEditingSelectionInformation";
}

