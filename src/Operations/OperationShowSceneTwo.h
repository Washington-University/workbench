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
#include "ImageResolutionUnitsEnum.h"
#include "ImageSpatialUnitsEnum.h"
#include "MapYokingGroupEnum.h"

namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserWindowContent;
    class EventImageCapture;
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
        
        static std::vector<AString> getCompatibilitySwitches() { std::vector<AString> ret; ret.push_back("-show-scene-two"); return ret; }
        
        static void loadSceneFileAndRestoreScene(const AString& sceneFileName,
                                                 const AString& sceneNameOrNumber,
                                                 const bool doNotUseSceneColorsFlag,
                                                 AString& errorMessageOut);

    private:
        class Inputs {
        public:
            Inputs(OffScreenSceneRendererBase* offscreenRenderer,
                   BrowserWindowContent* browserWindowContent,
                   EventImageCapture* imageCaptureEvent,
                   const QString imageFileName,
                   const int32_t outputImageIndex,
                   const bool doNotUseSceneColorsFlag)
            : m_offscreenRenderer(offscreenRenderer),
            m_browserWindowContent(browserWindowContent),
            m_imageCaptureEvent(imageCaptureEvent),
            m_imageFileName(imageFileName),
            m_outputImageIndex(outputImageIndex),
            m_doNotUseSceneColorsFlag(doNotUseSceneColorsFlag)
            { }
            
            OffScreenSceneRendererBase* m_offscreenRenderer;
            BrowserWindowContent* m_browserWindowContent;
            EventImageCapture* m_imageCaptureEvent;
            const QString m_imageFileName;
            const int32_t m_outputImageIndex;
            const bool m_doNotUseSceneColorsFlag;
        };
        
        static BrainOpenGLFixedPipeline* createBrainOpenGL();
        
        static void applyMapYoking(const MapYokingGroupEnum::Enum mapYokingGroup,
                                   const int32_t mapYokingMapIndex);
                
        static void setRemoteLoginAndPassword(const AString& username,
                                              const AString& password);
        
        static void renderWindowToImage(Inputs& inputs);
        
        static void writeImageFile(const AString& imageFileName,
                                   const int32_t imageIndex,
                                   const ImageFile* imageFile);

        static AString getExamplesOfUsage();
        
        static std::vector<std::unique_ptr<OffScreenSceneRendererBase>> getOffScreenRenderers();
        
        static const ImageResolutionUnitsEnum::Enum s_defaultResolutionUnits = ImageResolutionUnitsEnum::PIXELS_PER_INCH;
        
        static const ImageSpatialUnitsEnum::Enum s_defaultImageWidthHeightUnits = ImageSpatialUnitsEnum::PIXELS;
        
        static constexpr float s_defaultResolutionNumberOfPixels = 300.0f;
    };

    typedef TemplateAutoOperation<OperationShowSceneTwo> AutoOperationShowSceneTwo;

} // namespace

#endif  //__OPERATION_SHOW_SCENE_TWO_H__

