
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

#define __EVENT_FOCUS_FILE_GET_COLOR_DECLARE__
#include "EventFocusFileGetColor.h"
#undef __EVENT_FOCUS_FILE_GET_COLOR_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventFocusFileGetColor 
 * \brief Event to get a focus name or class color for use by a sample
 * \ingroup Files
 */

/**
 * Constructor.
 * @param fociFileName
 *    Name of foci file
 * @param samplesColorMode
 *    The samples color mode
 * @param focusOrClassName
 *    Name of focus or class name (samplesColorMode indicates which one)
 */
EventFocusFileGetColor::EventFocusFileGetColor(const AString& fociFileName,
                                               const SamplesColorModeEnum::Enum samplesColorMode,
                                               const AString& focusOrClassName)
: Event(EventTypeEnum::EVENT_FOCUS_FILE_GET_COLOR),
m_fociFileName(fociFileName),
m_samplesColorMode(samplesColorMode),
m_focusOrClassName(focusOrClassName)
{
    
}

/**
 * Destructor.
 */
EventFocusFileGetColor::~EventFocusFileGetColor()
{
}

/**
 * @return Name of foci file
 */
AString
EventFocusFileGetColor::getFociFileName() const
{
    return m_fociFileName;
}

/**
 * @return Sample color mode indicates searching for focus color or class color
 */
SamplesColorModeEnum::Enum
EventFocusFileGetColor::getSamplesColorMode() const
{
    return m_samplesColorMode;
}

/**
 * @return Name of focus or class (getSamplesColorMode() indicates which one)
 */
AString
EventFocusFileGetColor::getFocusOrClassName() const
{
    return m_focusOrClassName;
}

/**
 * Set the RGBA color components
 * @param rgba
 *    The RGBA values
 */
void
EventFocusFileGetColor::setColorRGBA(const std::array<uint8_t, 4>& rgba)
{
    m_rgba = rgba;
}

/**
 * @return The RGBA color components
 */
std::array<uint8_t, 4>
EventFocusFileGetColor::getColorRGBA() const
{
    return m_rgba;
}

