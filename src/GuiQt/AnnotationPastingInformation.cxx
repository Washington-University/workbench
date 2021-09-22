
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

#define __ANNOTATION_PASTING_INFORMATION_DECLARE__
#include "AnnotationPastingInformation.h"
#undef __ANNOTATION_PASTING_INFORMATION_DECLARE__

#include "Annotation.h"
#include "AnnotationClipboard.h"
#include "AnnotationCoordinateInformation.h"
#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationPastingInformation 
 * \brief Information for pasting annotation with valid annotation space coordinates
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param mouseEvent
 *     Mouse event for pasting the annotation
 * @param clipboard
 *     Clipboard containing the annotation for pasting
 */
AnnotationPastingInformation::AnnotationPastingInformation(const MouseEvent& mouseEvent,
                                                           const AnnotationClipboard* clipboard)
: CaretObject(),
m_mouseEvent(mouseEvent)
{
    CaretAssert(clipboard);
    
    m_mouseCoordinateInformation.reset(new AnnotationCoordinateInformation());
    m_validForPastingInformation.reset(new AnnotationCoordinateInformation());
    
    const Annotation* annotation(clipboard->getAnnotation(0));
    if (annotation == NULL) {
        m_invalidDescription = "No annotation is on the clipboard.";
        return;
    }
    
    const bool validFlag(AnnotationCoordinateInformation::createCoordinateInformationForPasting(mouseEvent,
                                                                                                clipboard,
                                                                                                *m_mouseCoordinateInformation,
                                                                                                m_allCoordinatesInformation,
                                                                                                m_invalidDescription));
    if ( ! validFlag) {
        return;
    }
    
    if (annotation->getNumberOfCoordinates() != static_cast<int32_t>(m_allCoordinatesInformation.size())) {
        m_invalidDescription = "PROGRAM ERROR: Sizes of coordinates for pasting and annotation coordinates are different.";
        return;
    }
    
    *m_validForPastingInformation = AnnotationCoordinateInformation::getValidCoordInfoForAll(m_allCoordinatesInformation);
    
    if (clipboard->getNumberOfAnnotations() >= 2) {
        /*
         * Only TAB and WINDOW allow pasting of multiple annotations
         */
        m_validForPastingInformation->m_chartSpaceInfo.m_validFlag     = false;
        m_validForPastingInformation->m_mediaSpaceInfo.m_validFlag     = false;
        m_validForPastingInformation->m_spacerTabSpaceInfo.m_validFlag = false;
        m_validForPastingInformation->m_modelSpaceInfo.m_validFlag     = false;
        m_validForPastingInformation->m_surfaceSpaceInfo.m_validFlag   = false;
    }
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> allSpaces;
    AnnotationCoordinateSpaceEnum::getAllEnums(allSpaces);
    for (const auto& space : allSpaces) {
        switch (space) {
            case AnnotationCoordinateSpaceEnum::CHART:
                if (m_validForPastingInformation->m_chartSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                if (m_validForPastingInformation->m_mediaSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                if (m_validForPastingInformation->m_spacerTabSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                if (m_validForPastingInformation->m_modelSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                if (m_validForPastingInformation->m_surfaceSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                if (m_validForPastingInformation->m_tabSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                /* Viewport space not used */
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                if (m_validForPastingInformation->m_windowSpaceInfo.m_validFlag) {
                    m_pasteableSpaces.push_back(space);
                }
                break;
        }
    }
}

/**
 * Destructor.
 */
AnnotationPastingInformation::~AnnotationPastingInformation()
{
}

/**
 * @return Mouse event for pasting the annotation
 */
const MouseEvent&
AnnotationPastingInformation::getMouseEvent() const
{
    return m_mouseEvent;
}

/*
 * @return Pasting information for each of the annotations coordinates
 */
const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>&
AnnotationPastingInformation::getAllCoordinatesInformation() const
{
    return m_allCoordinatesInformation;
}

/**
 * @return Coordinate information for the mouse position
 */
const std::unique_ptr<AnnotationCoordinateInformation>&
AnnotationPastingInformation::getMouseCoordinateInformation() const
{
    return m_mouseCoordinateInformation;
}

/**
 * @return All pasteable spaces
 */
std::vector<AnnotationCoordinateSpaceEnum::Enum>
AnnotationPastingInformation::getPasteableSpaces() const
{
    return m_pasteableSpaces;
}


/**
 * @return True if annotation can be pasted in at least one space.
 * Call getInvalidDescription() to find out why information is invalid.
 */
bool
AnnotationPastingInformation::isValid() const
{
    return ( ! m_pasteableSpaces.empty());
}

/**
 * @return Description of why information is invalid
 */
AString
AnnotationPastingInformation::getInvalidDescription() const
{
    return m_invalidDescription;
}

/**
 * @return True if the annotation can be pasted in the given space
 * @param space
 *    The coordinate space.
 */
bool
AnnotationPastingInformation::isPasteableInSpace(const AnnotationCoordinateSpaceEnum::Enum space) const
{
    if (std::find(m_pasteableSpaces.begin(),
                  m_pasteableSpaces.end(),
                  space) != m_pasteableSpaces.end()) {
        return true;
    }
    
    return false;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationPastingInformation::toString() const
{
    return "AnnotationPastingInformation";
}

