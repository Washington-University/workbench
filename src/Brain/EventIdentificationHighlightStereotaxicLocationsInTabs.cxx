
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_DECLARE__
#include "EventIdentificationHighlightStereotaxicLocationsInTabs.h"
#undef __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"
#include "HtmlStringBuilder.h"

using namespace caret;


    
/**
 * \class caret::EventIdentificationHighlightStereotaxicLocationsInTabs 
 * \brief Identify stereotaxic locations that may be different in different tabs
 * \ingroup Brain
 */

/**
 * Constructor for identification of labels in a label hierarchy
 * @param labelHierarchText
 *    Text of the label hierarchy
 */
EventIdentificationHighlightStereotaxicLocationsInTabs::EventIdentificationHighlightStereotaxicLocationsInTabs(const AString& labelHierarchText)
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS),
m_mode(Mode::LABELS_IN_TABS),
m_labelHierarchyText(labelHierarchText)
{
    
}

/**
 * Destructor.
 */
EventIdentificationHighlightStereotaxicLocationsInTabs::~EventIdentificationHighlightStereotaxicLocationsInTabs()
{
}

/**
 * Add a label name and its stereotaxic xyz
 * @param labelName
 *    Name of label
 * @param xyz
 *    Stereotaxic XYZ of label
 */
void
EventIdentificationHighlightStereotaxicLocationsInTabs::addLabelAndStereotaxicXYZ(const AString& labelName,
                               const Vector3D& xyz)
{
    m_labels.emplace_back(labelName,
                          xyz);
}

/**
 * @return The mode
 */
EventIdentificationHighlightStereotaxicLocationsInTabs::Mode
EventIdentificationHighlightStereotaxicLocationsInTabs::getMode() const
{
    return m_mode;
}

/**
 * @return The text in the label hierarchy
 */
const AString&
EventIdentificationHighlightStereotaxicLocationsInTabs::getLabelHierarchyText() const
{
    return m_labelHierarchyText;
}

/**
 * @return Number of labels
 */
int32_t
EventIdentificationHighlightStereotaxicLocationsInTabs::getNumberOfLabels() const
{
    return m_labels.size();
}

/**
 * @return Name of label at the given index
 * @param index
 *    The index
 */
const AString&
EventIdentificationHighlightStereotaxicLocationsInTabs::getLabelName(const int32_t index) const
{
    return m_labels[index].m_labelName;
}

/**
 * @return Stereotaxic XYZ of label at the given index
 * @param index
 *    The index
 */
const Vector3D&
EventIdentificationHighlightStereotaxicLocationsInTabs::getLabelStereotaxicXYZ(const int32_t index) const
{
    return m_labels[index].m_xyz;
}

/**
 * Add identification text displaying info about data that was loaded
 * @param text
 *    Text to add
 */
void
EventIdentificationHighlightStereotaxicLocationsInTabs::addIdentificationText(const AString& text)
{
    if ( ! text.isEmpty()) {
        m_identificationTextLines.push_back(text);
    }
}

/**
 * @return The identification text
 */
AString
EventIdentificationHighlightStereotaxicLocationsInTabs::getIdentificationText() const
{
    AString text;
    
    if ( ! m_identificationTextLines.empty()) {
        HtmlStringBuilder hsb;
        for (const AString& line : m_identificationTextLines) {
            hsb.addLine(line);
        }
        hsb.addLineBreak();
        text = hsb.toStringWithHtmlBody();
    }
    
    return text;
}

