
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __GRAPHICS_OPEN_G_L_ERROR_DECLARE__
#include "GraphicsOpenGLError.h"
#undef __GRAPHICS_OPEN_G_L_ERROR_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsOpenGLError 
 * \brief Contains information about OpenGL Errors.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsOpenGLError::GraphicsOpenGLError(const AString& userMessage)
: CaretObject(),
m_userMessage(userMessage)
{
    glGetIntegerv(GL_PROJECTION_STACK_DEPTH,
                  &m_projectionStackDepth);
    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,
                  &m_modelviewStackDepth);
    glGetIntegerv(GL_NAME_STACK_DEPTH,
                  &m_nameStackDepth);
    glGetIntegerv(GL_ATTRIB_STACK_DEPTH,
                  &m_attribStackDepth);
    
    SystemUtilities::getBackTrace(m_backtrace);
}

/**
 * Destructor.
 */
GraphicsOpenGLError::~GraphicsOpenGLError()
{
}

/**
 * Add an error code.
 * 
 * @param errorCode
 */
void
GraphicsOpenGLError::addError(const GLenum errorCode)
{
    m_errorCodes.push_back(errorCode);
}

/**
 * @return Number of errors.  OpenGL may have more than one error
 * available.  Set the man page for glGetError().
 */
int32_t
GraphicsOpenGLError::getNumberOfErrors() const
{
    return m_errorCodes.size();
}

/**
 * @return Error code for the given error index.
 *
 * @param errorIndex
 *     Error index.
 */
GLenum
GraphicsOpenGLError::getErrorCode(const int32_t errorIndex) const
{
    CaretAssertVectorIndex(m_errorCodes, errorIndex);
    return m_errorCodes[errorIndex];
}

/**
 * @return Description for the given error index.
 *
 * @param errorIndex
 *     Error index.
 */
AString
GraphicsOpenGLError::getErrorDescription(const int32_t errorIndex) const
{
    CaretAssertVectorIndex(m_errorCodes, errorIndex);
    AString description((char*)gluErrorString(m_errorCodes[errorIndex]));
    
    return description;
}

/**
 * @return String with verbose description of error information.
 *         An empty string is returned if there are no errors.
 */
AString 
GraphicsOpenGLError::getVerboseDescription() const
{
    if (m_errorCodes.empty()) {
        return "";
    }
    
    AString msg;
    
    if ( ! m_userMessage.isEmpty()) {
        msg.appendWithNewLine(m_userMessage);
    }
    
    for (auto errorCode : m_errorCodes) {
        msg.appendWithNewLine("OpenGL Error: "
                              + AString((char*)gluErrorString(errorCode))
                              + " ("
                              + AString::number(static_cast<int32_t>(errorCode))
                              + ")");
    }
    
    msg.appendWithNewLine("OpenGL Version: " + AString((char*)glGetString(GL_VERSION)));
    msg.appendWithNewLine("OpenGL Vendor:  " + AString((char*)glGetString(GL_VENDOR)));
    
    GLint maxNameStackDepth, maxModelStackDepth, maxProjStackDepth, maxAttribStackDepth;
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,
                  &maxProjStackDepth);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,
                  &maxModelStackDepth);
    glGetIntegerv(GL_MAX_NAME_STACK_DEPTH,
                  &maxNameStackDepth);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH,
                  &maxAttribStackDepth);
    
    msg.appendWithNewLine("Projection Matrix Stack Depth "
                          + AString::number(m_projectionStackDepth)
                          + "  Max Depth "
                          + AString::number(maxProjStackDepth));
    msg.appendWithNewLine("Model Matrix Stack Depth "
                          + AString::number(m_modelviewStackDepth)
                          + "  Max Depth "
                          + AString::number(maxModelStackDepth));
    msg.appendWithNewLine("Name Matrix Stack Depth "
                          + AString::number(m_nameStackDepth)
                          + "  Max Depth "
                          + AString::number(maxNameStackDepth));
    msg.appendWithNewLine("Attribute Matrix Stack Depth "
                          + AString::number(m_attribStackDepth)
                          + "  Max Depth "
                          + AString::number(maxAttribStackDepth));
    msg.appendWithNewLine("Backtrace: "
                          + m_backtrace.toSymbolString());
    
    return msg;
}

