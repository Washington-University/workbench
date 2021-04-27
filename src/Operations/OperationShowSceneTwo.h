#ifndef __OPERATION_SHOW_SCENE_TWO_H__
#define __OPERATION_SHOW_SCENE_TWO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <memory>

#ifdef HAVE_GLEW
#include <GL/glew.h>
#endif

#ifdef HAVE_OSMESA
#include <GL/osmesa.h>
#endif // HAVE_OSMESA

#include "AbstractOperation.h"
#include "MapYokingGroupEnum.h"

namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserWindowContent;
    class ImageFile;
    class OffScreenSceneRendererBase;
    class SceneClass;
    
    class OperationShowSceneTwo : public AbstractOperation {

    public:
        static OperationParameters* getParameters();

        static void useParameters(OperationParameters* myParams, 
                                  ProgressObject* myProgObj);

        static AString getCommandSwitch();

        static AString getShortDescription();

        static bool isShowSceneCommandAvailable();
        
    private:
        class Inputs {
        public:
            Inputs(OffScreenSceneRendererBase* offscreenRenderer,
                   BrowserWindowContent* browserWindowContent,
                   const QString imageFileName,
                   const int32_t outputImageIndex,
                   const int32_t imageWidth,
                   const int32_t imageHeight,
                   const bool doNotUseSceneColorsFlag)
            : m_offscreenRenderer(offscreenRenderer),
            m_browserWindowContent(browserWindowContent),
            m_imageFileName(imageFileName),
            m_outputImageIndex(outputImageIndex),
            m_imageWidth(imageWidth),
            m_imageHeight(imageHeight),
            m_doNotUseSceneColorsFlag(doNotUseSceneColorsFlag)
            { }
            
            OffScreenSceneRendererBase* m_offscreenRenderer;
            BrowserWindowContent* m_browserWindowContent;
            const QString m_imageFileName;
            const int32_t m_outputImageIndex;
            const int32_t m_imageWidth;
            const int32_t m_imageHeight;
            const bool m_doNotUseSceneColorsFlag;
        };
        
        static BrainOpenGLFixedPipeline* createBrainOpenGL();
        
        static void applyMapYoking(const MapYokingGroupEnum::Enum mapYokingGroup,
                                   const int32_t mapYokingMapIndex);
        
        static void loadSceneFileAndRestoreScene(const AString& sceneFileName,
                                                 const AString& sceneNameOrNumber,
                                                 const bool doNotUseSceneColorsFlag,
                                                 AString& errorMessageOut);
        
        static void setRemoteLoginAndPassword(const AString& username,
                                              const AString& password);
        
//        static void getImageWidthAndHeight(const Inputs& inputs,
//                                           int32_t& widthOut,
//                                           int32_t& heightOut);
        
        static void renderWindowToImage(Inputs& inputs);
        
        static void writeImageFile(const AString& imageFileName,
                                   const int32_t imageIndex,
                                   const ImageFile* imageFile);

        static void estimateGraphicsSize(const SceneClass* windowSceneClass,
                                         float& estimatedWidthOut,
                                         float& estimatedHeightOut);
        
        static bool getToolBoxSize(const SceneClass* toolBoxClass,
                                   const SceneClass* activeToolBoxClass,
                                   float& overlayToolBoxWidthOut,
                                   float& overlayToolBoxHeightOut,
                                   QString& overlayToolBoxOrientationOut);
        
        static std::vector<std::unique_ptr<OffScreenSceneRendererBase>> getOffScreenRenderers();
    };

    typedef TemplateAutoOperation<OperationShowSceneTwo> AutoOperationShowSceneTwo;

} // namespace

#endif  //__OPERATION_SHOW_SCENE_TWO_H__

