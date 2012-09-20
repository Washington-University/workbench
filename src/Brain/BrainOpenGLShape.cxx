
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
#include "CaretAssert.h"
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
    if (s_drawModeInitialized == false) {
        s_drawMode = BrainOpenGL::getBestDrawingMode();
        s_drawModeInitialized = true;
    }
    
    m_shapeSetupComplete = false;
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
    
    for (std::set<GLuint>::iterator iter = m_displayLists.begin();
         iter != m_displayLists.end();
         iter++) {
        releaseDisplayListInternal(*iter,
                                   false);
    }
    m_displayLists.clear();
}



/**
 * Draw the shape.
 */
void
BrainOpenGLShape::draw()
{
    if (m_shapeSetupComplete == false) {
        setupShape(s_drawMode);
        m_shapeSetupComplete = true;
    }
    
    drawShape(s_drawMode);
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
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGLInfo::isVertexBuffersSupported()) {
        glGenBuffers(1, &id);
        
        if (id > 0) {
            m_bufferIDs.insert(id);
        }
        else {
            CaretLogSevere("Failed to create a new OpenGL Vertex Buffer");
        }
    }
#else // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    CaretLogSevere("PROGRAM ERROR: Creating OpenGL vertex buffer but vertex buffers not supported.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
        
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
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGLInfo::isVertexBuffersSupported()) {
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
    }
#else  // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    CaretLogSevere("PROGRAM ERROR: Releasing OpenGL vertex buffer but vertex buffers not supported.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
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
BrainOpenGLShape::releaseDisplayListInternal(const GLuint displayList,
                                             const bool isRemoveFromTrackedLists)
{
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    if (BrainOpenGLInfo::isDisplayListsSupported()) {
        if (glIsList(displayList)) {
            glDeleteLists(displayList, 1);
            
            if (isRemoveFromTrackedLists) {
                m_displayLists.erase(displayList);
            }
        }
        else {
            CaretLogSevere("PROGRAM ERROR: Attempting to delete invalid OpenGL DisplayList="
                           + AString::number(displayList));
        }
    }
#else  // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    CaretLogSevere("PROGRAM ERROR: Releasing OpenGL display list but display lists not supported.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
}

/**
 * @return A new display list for use with OpenGL.
 * A return value of zero indicates that creation of display list failed.
 * Values greater than zero are valid display list.
 */
GLuint
BrainOpenGLShape::createDisplayList()
{
    GLuint displayList = 0;
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    if (BrainOpenGLInfo::isDisplayListsSupported()) {
        displayList = glGenLists(1);
        
        if (displayList > 0) {
            m_displayLists.insert(displayList);
        }
        else {
            CaretLogSevere("Failed to create a new OpenGL Display List");
        }
    }
#else // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    CaretLogSevere("PROGRAM ERROR: Creating OpenGL display list but display lists not supported.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    
    return displayList;
}
    
/**
 * Release an allocated display list.
 *
 * @param displayList
 *    Display list that was previously returned by createDisplayList().
 */
void
BrainOpenGLShape::releaseDisplayList(const GLuint displayList)
{
    releaseDisplayListInternal(displayList, true);
}
    

