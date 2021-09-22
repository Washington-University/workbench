
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __ANNOTATION_CLIPBOARD_DECLARE__
#include "AnnotationClipboard.h"
#undef __ANNOTATION_CLIPBOARD_DECLARE__

#include "Annotation.h"
#include "Brain.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::AnnotationClipboard 
 * \brief Contains annotation and related information for the clipboard
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param brain
 *    The brain
 */
AnnotationClipboard::AnnotationClipboard(Brain* brain)
: CaretObject(),
m_brain(brain)
{
    CaretAssert(m_brain);
    
}

/**
 * Destructor.
 */
AnnotationClipboard::~AnnotationClipboard()
{
    clear();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationClipboard::toString() const
{
    return "AnnotationClipboard";
}

/**
 * Clear the content of the clipboard
 */
void
AnnotationClipboard::clear()
{
    m_annotationWindowCoordinates.clear();
    /*
     * Clipboard owns its annotations
     */
    for (auto& element : m_clipboardContent) {
        delete element.getAnnotation();
    }
    m_clipboardContent.clear();
    m_mouseWindowCoordinates = Vector3D();
    m_allAnnotationsInSameUserGroupFlag = false;
}

/**
 * @return True if the clipboard is empty, else false.
 */
bool
AnnotationClipboard::isEmpty() const
{
    return m_clipboardContent.empty();
}

/**
 * @return Number of annotations on clipboard
 */
int32_t
AnnotationClipboard::getNumberOfAnnotations() const
{
    return m_clipboardContent.size();
}


/**
 * @return const pointer to annotation on clipboard (NULL is no annotation on clipboard)
 * @param index
 *    Index of annotation
 */
const Annotation*
AnnotationClipboard::getAnnotation(const int32_t index) const
{
    CaretAssertVectorIndex(m_clipboardContent, index);
    return m_clipboardContent[index].getAnnotation();
}

/**
 * @return A copy of the annotation on the clipboard (returns NULL if no annotation on clipboard)
 * @param index
 *    Index of annotation
 */
Annotation*
AnnotationClipboard::getCopyOfAnnotation(const int32_t index) const
{
    CaretAssertVectorIndex(m_clipboardContent, index);
    return m_clipboardContent[index].getAnnotation()->clone();
}

/**
 * @return Pointer to annotation file that contained annotation on clipboard.
 * Returned file is a valid file if not NULL.
 * @param index
 *    Index of annotation
 */
AnnotationFile*
AnnotationClipboard::getAnnotationFile(const int32_t index) const
{
    CaretAssertVectorIndex(m_clipboardContent, index);
    AnnotationFile* annotationFile(m_clipboardContent[index].getFile());
    
    /*
     * It is possible that the file has been destroyed.
     * If so, invalidate the file (set it to NULL).
     */
    std::vector<AnnotationFile*> allAnnotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
    
    if (std::find(allAnnotationFiles.begin(),
                  allAnnotationFiles.end(),
                  annotationFile) == allAnnotationFiles.end()) {
        annotationFile = NULL;
    }
    
    return annotationFile;
}

/**
 * @return Group key for the annotation at the given index.
 * @param index
 *    Index of annotation
 */
AnnotationGroupKey
AnnotationClipboard::getAnnotationGroupKey(const int32_t index) const
{
    CaretAssertVectorIndex(m_clipboardContent, index);
    return m_clipboardContent[index].getGroupKey();
}

/**
 * @return Window coordinates of mouse when annotation was copied to clipboard
 */
const Vector3D&
AnnotationClipboard::getMouseWindowCoordinates() const
{
    return m_mouseWindowCoordinates;
}

/**
 * @return Window coordinates of annotation's coordinates when annotation was copied to clipboard
 */
const std::vector<Vector3D>&
AnnotationClipboard::getAnnotationWindowCoordinates() const
{
    return m_annotationWindowCoordinates;
}

/*
 * @param annotationFile
 *   The annotation file from which annotation was copied
 * @param annotation
 *   The annotation on the clipboard
 * @param annotationWindowCoordinates
 *   Window coordinates of the annotation when it was placed on the clipboard
 * @param mouseWindowCoordinates
 *   Window coordinates of mouse when the annotation was copied to the clipboard
 */
void
AnnotationClipboard::setContent(AnnotationFile* annotationFile,
                                const Annotation* annotation,
                                std::vector<Vector3D>& annotationWindowCoordinates,
                                Vector3D& mouseWindowCoordinates)
{
    clear();
    
    m_annotationWindowCoordinates = annotationWindowCoordinates;
    m_mouseWindowCoordinates      = mouseWindowCoordinates;

    m_clipboardContent.emplace_back(annotation->clone(),
                                    annotationFile,
                                    annotation->getAnnotationGroupKey());
}

/*
 * @param annotationsAndFile
 *   Annotations/File/Window Coordinates for clipboard
 * @param annotationWindowCoordinates
 *   Window coordinates of the annotation when it was placed on the clipboard
 * @param mouseWindowCoordinates
 *   Window coordinates of mouse when the annotation was copied to the clipboard
 */
bool
AnnotationClipboard::setContent(const std::vector<AnnotationAndFile>& annotationsAndFile,
                                std::vector<Vector3D>& annotationWindowCoordinates,
                                const Vector3D& mouseWindowCoordinates)
{
    clear();
    
    if ( ! areAnnotationsClipboardEligible(annotationsAndFile)) {
        return false;
    }
    
    std::set<int32_t> uniqueUserGroupKeys;
    bool allInUserGroupFlag(true);
    for (const auto& af : annotationsAndFile) {
        switch (af.getGroupKey().getGroupType()) {
            case AnnotationGroupTypeEnum::INVALID:
                allInUserGroupFlag = false;
                break;
            case AnnotationGroupTypeEnum::SPACE:
                allInUserGroupFlag = false;
                break;
            case AnnotationGroupTypeEnum::USER:
                uniqueUserGroupKeys.insert(af.getGroupKey().getUserGroupUniqueKey());
                break;
        }
        
        m_clipboardContent.emplace_back(af.getAnnotation()->clone(),
                                        af.getFile(),
                                        af.getGroupKey());
    }
    
    m_allAnnotationsInSameUserGroupFlag = ((getNumberOfAnnotations() > 1)
                                           && allInUserGroupFlag
                                           && (uniqueUserGroupKeys.size() == 1));
    
    m_annotationWindowCoordinates = annotationWindowCoordinates;
    m_mouseWindowCoordinates = mouseWindowCoordinates;
    
    return true;
}

/**
 * @return True if all annontations are in the same user group (must be more than one annotation also).
 */
bool
AnnotationClipboard::areAllAnnotationsInSameUserGroup() const
{
    return m_allAnnotationsInSameUserGroupFlag;
}

/**
 * @return Are the given annotations eligible for placement onto the clipboard?
 * All Annotations must be in same file and in a limited number of coordinate spaces
 * @param annotationsAndFile
 *    The annotations
 */
bool
AnnotationClipboard::areAnnotationsClipboardEligible(const std::vector<AnnotationAndFile>& annotationsAndFile)
{
    if (annotationsAndFile.empty()) {
        return false;
    }

    const AnnotationFile* firstFile(NULL);
    const Annotation* firstAnnotation(NULL);
    const int32_t numAnn(annotationsAndFile.size());
    for (int32_t i = 0; i < numAnn; i++) {
        CaretAssertVectorIndex(annotationsAndFile, i);
        const AnnotationAndFile& annAndFile(annotationsAndFile[i]);
        const Annotation* ann(annAndFile.getAnnotation());
        CaretAssert(ann);
        if ( ! ann->testProperty(Annotation::Property::DELETION)) {
            return false;
        }
        if ( ! ann->testProperty(Annotation::Property::COPY_CUT_PASTE)) {
            return false;
        }
        const AnnotationFile* annFile(annAndFile.getFile());
        CaretAssert(annFile);
        if (i == 0) {
            firstFile = annFile;
            firstAnnotation = ann;
            bool validSpaceFlag(false);
            switch (firstAnnotation->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    validSpaceFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    validSpaceFlag = true;
                    break;
            }
            
            if ( ! validSpaceFlag) {
                return false;
            }
        }
        else {
            if (ann->isInSameCoordinateSpace(firstAnnotation)
                && (annFile == firstFile)) {
                /* Is in same file space */
            }
            else {
                return false;
            }
        }
    }
    
    return true;
}

