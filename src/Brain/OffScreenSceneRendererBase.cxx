
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

#define __OFF_SCREEN_SCENE_RENDERER_BASE_DECLARE__
#include "OffScreenSceneRendererBase.h"
#undef __OFF_SCREEN_SCENE_RENDERER_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::OffScreenSceneRendererBase 
 * \brief Base class for off screen scene renderers
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param switchName
 *  Name that user enters when selecting the
 * offscreen renderer by its name after a 'switch' on the
 * command line.
 * @param descriptiveName
 *    Descriptive name of the offscreen renderer.
 *
 */
OffScreenSceneRendererBase::OffScreenSceneRendererBase(const AString switchName,
                                                       const AString descriptiveName)
: CaretObject(),
m_switchName(switchName),
m_descriptiveName(descriptiveName)
{
    
}

/**
 * Destructor.
 */
OffScreenSceneRendererBase::~OffScreenSceneRendererBase()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OffScreenSceneRendererBase::toString() const
{
    return "OffScreenSceneRendererBase";
}

/**
 * @retrurn Name that user enters when selecting the
 * offscreen renderer by its name after a 'switch' on the
 * command line.
 */
AString
OffScreenSceneRendererBase::getSwitchName() const
{
    return m_switchName;
}

/**
 * @return Descriptive name of the offscreen renderer.
 */
AString
OffScreenSceneRendererBase::getDescriptiveName() const
{
    return m_descriptiveName;
}

/**
 * @return True if the offscreen renderer is valid (has been initialized
 * and is ready for drawing), else false.  Call @getErrorMessage() for the
 * reason renderer is not valid.  If the renderer is not available, false is returned.
 */
bool
OffScreenSceneRendererBase::isValid() const
{
    if ( ! isAvailable()) {
        return false;
    }
    
    return m_validFlag;
}

/**
 * @return Description of any errors.  If the renderer is not available,
 * an unavailable message is returned.
 */
AString
OffScreenSceneRendererBase::getErrorMessage() const
{
    if ( ! isAvailable()) {
        return (m_descriptiveName + " is not available");
    }
    return m_errorMessage;
}

/**
 * Set the error message.  If error message is not empty, validity is set to false.
 * @param errorMessage
 *    Description of the error.
 */
void
OffScreenSceneRendererBase::setErrorMessage(const AString& errorMessage)
{
    m_errorMessage = errorMessage;
    if ( ! m_errorMessage.isEmpty()) {
        setValid(false);
    }
}

/**
 * Used by sub-classes to set the validty state.  If the validity is true, the
 * error message is cleared.
 * @param valid
 *    Validity state.
 */
void
OffScreenSceneRendererBase::setValid(const bool valid)
{
    m_validFlag = valid;
    if (m_validFlag) {
        m_errorMessage.clear();
    }
}

