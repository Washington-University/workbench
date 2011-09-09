
#ifndef __BRAIN_OPENGL_H__
#define __BRAIN_OPENGL_H__

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <stdint.h>

#include "BrainConstants.h"
#include "CaretObject.h"

namespace caret {
    
    class Brain;
    class Surface;
    class ModelDisplayController;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGL : public CaretObject {
        
    private:
        BrainOpenGL();

        BrainOpenGL(const BrainOpenGL&);
        BrainOpenGL& operator=(const BrainOpenGL&);
        
    public:
        static BrainOpenGL* getBrainOpenGL();
        
        ~BrainOpenGL();
        
        void drawModel(Brain* brain,
                        const int32_t windowIndex,
                        const int32_t viewport[4],
                        ModelDisplayController* controller);
        
        void initializeOpenGL();
        
        void updateOrthoSize(const int32_t windowIndex, 
                             const int32_t width, 
                             const int32_t height);

        static float getModelViewingHalfWindowHeight() { return 125.0f; }
        
    private:
        void initializeMembersBrainOpenGL();
        
        void drawSurface(const Surface* surface);
        
        void drawSurfaceTriangles(const Surface* surface);
        
        void checkForOpenGLError();
        
        void setOrthographicProjection(const int32_t viewport[4]);
        
        void checkForOpenGLError(const ModelDisplayController* modelController,
                                 const AString& msg);
        
        /** contains single instance of this class */
        static BrainOpenGL* brainOpenGLSingleton;
        
        /** version number of OpenGL */
        static float versionOfOpenGL;

        Brain* brain;
        
        /** Viewport of drawing */
        //int32_t viewport[4];
        
        /** Indicates OpenGL has been initialized */
        bool initializedOpenGLFlag;
        
        /** Index of window */
        int windowIndex;
        
        /** Caret Window that OpenGL is being drawn within */
        //CaretWindowEnum CaretWindowEnum;
        
        double orthographicLeft[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        double orthographicRight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        double orthographicBottom[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        double orthographicTop[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        double orthographicFar[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        double orthographicNear[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };

#ifdef __BRAIN_OPENGL_DEFINE_H
    BrainOpenGL* BrainOpenGL::brainOpenGLSingleton = NULL;
    float BrainOpenGL::versionOfOpenGL = 0.0f;
#endif //__BRAIN_OPENGL_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_H__
