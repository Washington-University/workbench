
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

#define __BRAIN_OPEN_G_L_INFO_DECLARE__
#include "BrainOpenGLInfo.h"
#undef __BRAIN_OPEN_G_L_INFO_DECLARE__

#include "BrainOpenGL.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLInfo 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
BrainOpenGLInfo::BrainOpenGLInfo()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLInfo::~BrainOpenGLInfo()
{
    
}
/**
 * Initialize the drawing mode using the most optimal drawing given
 * the compile time and run time constraints.
 */
void
BrainOpenGLInfo::initializeDrawMode()
{
    if (s_drawModeInitialized) {
        return;
    }
    s_drawModeInitialized = true;
    
    s_drawingMode = DRAW_MODE_INVALID;
    s_supportsDisplayLists = false;
    s_supportsImmediateMode = false;
    s_supportsVertexBuffers = false;
    
#if BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
    if (BrainOpenGL::getRuntimeVersionOfOpenGL() < 4.0) {
        s_drawingMode = DRAW_MODE_IMMEDIATE;
        s_supportsImmediateMode = true;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
    
#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    if (BrainOpenGL::getRuntimeVersionOfOpenGL() < 4.0) {
        s_drawingMode = DRAW_MODE_DISPLAY_LISTS;
        s_supportsDisplayLists = true;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGL::getRuntimeVersionOfOpenGL() >= 2.1) {
        s_drawMode = DRAW_MODE_VERTEX_BUFFERS;
        s_supportsVertexBuffers = true;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    
    if (s_drawingMode == DRAW_MODE_INVALID) {
        CaretAssert(0);
    }
}

/**
 * @return Text string describing the drawing mode for shapes.
 */
QString
BrainOpenGLInfo::getBestDrawingModeName()
{
    initializeDrawMode();
    
    QString modeName = "Invalid";
    
    switch (s_drawingMode) {
        case BrainOpenGLInfo::DRAW_MODE_DISPLAY_LISTS:
            modeName = "Display Lists";
            break;
        case BrainOpenGLInfo::DRAW_MODE_IMMEDIATE:
            modeName = "Immediate";
            break;
        case BrainOpenGLInfo::DRAW_MODE_INVALID:
            modeName = "Invalid";
            break;
        case BrainOpenGLInfo::DRAW_MODE_VERTEX_BUFFERS:
            modeName = "Vertex Buffers";
            break;
    }
    
    return modeName;
}




