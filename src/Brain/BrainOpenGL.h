
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
#include "IdentificationItemDataTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {
    
    class Brain;
    class BrowserTabContent;
    class IdentificationManager;
    class IdentificationWithColor;
    class Surface;
    class ModelDisplayController;
    class ModelDisplayControllerSurface;
    class ModelDisplayControllerVolume;
    class ModelDisplayControllerWholeBrain;
    class Palette;
    class PaletteColorMapping;
    class PaletteFile;
    class VolumeFile;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGL : public CaretObject {
        
    private:
        enum Mode {
            MODE_DRAWING,
            MODE_IDENTIFICATION
        };
        
        BrainOpenGL();

        BrainOpenGL(const BrainOpenGL&);
        BrainOpenGL& operator=(const BrainOpenGL&);
        
    public:
        static BrainOpenGL* getBrainOpenGL();
        
        ~BrainOpenGL();
        
        void drawModel(ModelDisplayController* controller,
                       BrowserTabContent* browserTabContent,
                       const int32_t windowTabNumber,
                       const int32_t viewport[4]);
        
        void selectModel(ModelDisplayController* controller,
                         BrowserTabContent* browserTabContent,
                         const int32_t windowTabNumber,
                         const int32_t viewport[4],
                         const int32_t mouseX,
                         const int32_t mouseY);
        
        void initializeOpenGL();
        
        void updateOrthoSize(const int32_t windowIndex, 
                             const int32_t width, 
                             const int32_t height);

        static float getModelViewingHalfWindowHeight() { return 100.0f; }
        
        IdentificationManager* getIdentificationManager();

        
    private:
        class VolumeDrawInfo {
        public:
            VolumeDrawInfo(VolumeFile* volumeFile,
                           Palette* palette,
                           const int32_t brickIndex,
                           const float opacity);
            
            VolumeFile* volumeFile;
            Palette* palette;
            PaletteColorMapping* paletteColorMapping;
            int32_t brickIndex;
            float opacity;
        };
        
        bool isIdentifyMode() const { return this->mode == MODE_IDENTIFICATION; }
        
        void drawModelInternal(Mode mode,
                               ModelDisplayController* controller,
                       BrowserTabContent* browserTabContent,
                       const int32_t windowTabNumber,
                       const int32_t viewport[4]);
        
        void initializeMembersBrainOpenGL();
        
        void drawSurfaceController(ModelDisplayControllerSurface* surfaceController,
                                   const int32_t viewport[4]);
        
        void drawSurface(Surface* surface);
        
        void drawSurfaceNodes(Surface* surface);
        
        void drawSurfaceTrianglesWithVertexArrays(const Surface* surface);
        
        void drawSurfaceTriangles(Surface* surface);
        
        void drawVolumeController(BrowserTabContent* browserTabContent,
                                  ModelDisplayControllerVolume* volumeController,
                                  const int32_t viewport[4]);
        
        void drawVolumeOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       std::vector<VolumeDrawInfo>& volumeDrawInfo);
        
        void setupVolumeDrawInfo(BrowserTabContent* browserTabContent,
                                 PaletteFile* paletteFile,
                                 std::vector<VolumeDrawInfo>& volumeDrawInfoOut);
        
        void drawWholeBrainController(BrowserTabContent* browserTabContent,
                                      ModelDisplayControllerWholeBrain* wholeBrainController,
                                      const int32_t viewport[4]);
        
        void setOrthographicProjection(const int32_t viewport[4]);
        
        void checkForOpenGLError(const ModelDisplayController* modelController,
                                 const AString& msg);
        
        void enableLighting();
        
        void disableLighting();
        
        void getIndexFromColorSelection(const IdentificationItemDataTypeEnum::Enum dataType,
                                           const int32_t x,
                                           const int32_t y,
                                           int32_t& indexOut,
                                           float& depthOut);
        
        void setViewportAndOrthographicProjection(const int32_t viewport[4]);
        
        void applyViewingTransformations(const ModelDisplayController* modelDisplayController,
                                         const int32_t tabIndex);
        
        void applyViewingTransformationsVolumeSlice(const ModelDisplayController* modelDisplayController,
                                         const int32_t tabIndex,
                                         const VolumeSliceViewPlaneEnum::Enum viewPlane);
        
        /** contains single instance of this class */
        static BrainOpenGL* brainOpenGLSingleton;
        
        /** version number of OpenGL */
        static float versionOfOpenGL;

        /** Viewport of drawing */
        //int32_t viewport[4];
        
        /** Indicates OpenGL has been initialized */
        bool initializedOpenGLFlag;
        
        /** Content of browser tab being drawn */
        BrowserTabContent* browserTabContent;
        
        /** Index of window tab */
        int32_t windowTabIndex;
        
        /** mode of operation draw/select/etc*/
        Mode mode;
        
        /** Identification manager */
        IdentificationManager* identificationManager;
        
        int32_t mouseX;
        int32_t mouseY;
        
        /** Identify using color */
        IdentificationWithColor* colorIdentification;
        
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
