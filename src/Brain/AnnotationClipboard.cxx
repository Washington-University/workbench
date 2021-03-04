
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
    m_annotationFile = NULL;
    m_annotation.reset();
    m_annotationWindowCoordinates.clear();
    m_mouseWindowCoordinates = Vector3D();
}

/**
 * @return const pointer to annotation on clipboard (NULL is no annotation on clipboard)
 */
const Annotation*
AnnotationClipboard::getAnnotation() const
{
    return m_annotation.get();
}

/**
 * @return A copy of the annotation on the clipboard (returns NULL if no annotation on clipboard)
 */
Annotation*
AnnotationClipboard::getCopyOfAnnotation() const
{
    Annotation* ann(NULL);
    if (m_annotation != NULL) {
        ann = m_annotation->clone();
    }
    return ann;
}

/**
 * @return Pointer to annotation file that contained annotation on clipboard.
 * Returned file is a valid file if not NULL.
 */
AnnotationFile*
AnnotationClipboard::getAnnotationFile() const
{
    /*
     * It is possible that the file has been destroyed.
     * If so, invalidate the file (set it to NULL).
     */
    std::vector<AnnotationFile*> allAnnotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
    
    if (std::find(allAnnotationFiles.begin(),
                  allAnnotationFiles.end(),
                  m_annotationFile) == allAnnotationFiles.end()) {
        m_annotationFile = NULL;
    }
    
    return m_annotationFile;
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

/**
 * @return True if the annotation the clipboard is valid.
 */
bool
AnnotationClipboard::isAnnotationValid() const
{
    return (m_annotation != NULL);
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
AnnotationClipboard::set(AnnotationFile* annotationFile,
                         const Annotation* annotation,
                         std::vector<Vector3D>& annotationWindowCoordinates,
                         Vector3D& mouseWindowCoordinates)
{
    m_annotation.reset(annotation->clone());
    m_annotationFile              = annotationFile;
    m_annotationWindowCoordinates = annotationWindowCoordinates;
    m_mouseWindowCoordinates      = mouseWindowCoordinates;
}
