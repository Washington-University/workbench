
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __BRAIN_OPEN_G_L_SHAPE_DECLARE__
#include "BrainOpenGLShape.h"
#undef __BRAIN_OPEN_G_L_SHAPE_DECLARE__

#include "BrainOpenGL.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShape 
 * \brief Abstract class for shapes drawn in OpenGL.
 */

/**
 * Constructor.
 */
BrainOpenGLShape::BrainOpenGLShape()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLShape::~BrainOpenGLShape()
{
    for (std::set<GLuint>::iterator iter = m_bufferIDs.begin();
         iter != m_bufferIDs.end();
         iter++) {
        releaseBufferIDInternal(*iter,
                                false);
    }
    m_bufferIDs.clear();
}

/**
 * @return 'true' is OpenGL buffers are supported.
 * For the return value to be true, compilation must be on
 * a system that supports OpenGL 2.1 or later and the 
 * runtime version of OpenGL must be 2.1 or later.
 */
bool
BrainOpenGLShape::isBuffersSupported()
{
#ifdef CARET_OS_MACOSX
#ifdef GL_VERSION_2_1
    if (BrainOpenGL::getRuntimeVersionOfOpenGL() >= 2.1) {
        return true;
    }
#endif // GL_VERSION_2_1
#endif // CARET_OS_MACOSX
    return false;
}

/**
 * @return A new buffer ID for use with OpenGL.
 * A return value of zero indicates that creation of buffer ID failed.
 * Values greater than zero are valid buffer IDs.
 */
GLuint
BrainOpenGLShape::createBufferID()
{
    GLuint id = 0;
#ifdef CARET_OS_MACOSX
#ifdef GL_VERSION_2_1
    glGenBuffers(1, &id);
    
    m_bufferIDs.insert(id);
#else  // GL_VERSION_2_1
    CaretLogSevere("PROGRAM ERROR: Creating OpenGL buffer but buffers not supported.");
#endif // GL_VERSION_2_1
#else // CARET_OS_MACOSX
    CaretLogSevere("PROGRAM ERROR: Creating OpenGL buffer but buffers not supported.");
#endif // CARET_OS_MACOSX
    return id;
}

/**
 * Release an allocated buffer ID.
 *
 * @param bufferID
 *    Buffer ID that was previously returned by createBufferID().
 */
void
BrainOpenGLShape::releaseBufferID(const GLuint bufferID)
{
    releaseBufferIDInternal(bufferID,
                            true);
}

/**
 * Release an allocated buffer ID.
 *
 * @param bufferID
 *    Buffer ID that was previously returned by createBufferID().
 * @param isRemoveFromTrackedIDs
 *    If true, remove the ID from the bufferID that are tracked
 *    by this object.
 */
void
BrainOpenGLShape::releaseBufferIDInternal(const GLuint bufferID,
                                          const bool isRemoveFromTrackedIDs)
{
#ifdef CARET_OS_MACOSX
#ifdef GL_VERSION_2_1
    if (glIsBuffer(bufferID)) {
        glDeleteBuffers(1, &bufferID);
        
        if (isRemoveFromTrackedIDs) {
            m_bufferIDs.erase(bufferID);
        }
    }
    else {
        CaretLogSevere("PROGRAM ERROR: Attempting to delete invalid OpenGL BufferID="
                       + AString::number(bufferID));
    }
#else  // GL_VERSION_2_1
    CaretLogSevere("PROGRAM ERROR: Releasing OpenGL buffer but buffers not supported.");
#endif // GL_VERSION_2_1
#else // CARET_OS_MACOSX
    CaretLogSevere("PROGRAM ERROR: Releasing OpenGL buffer but buffers not supported.");
#endif // CARET_OS_MACOSX
}



