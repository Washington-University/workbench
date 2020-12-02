
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__
#include "BrainOpenGLMediaDrawing.h"
#undef __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__

#include "BrainOpenGLFixedPipeline.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "ImageFile.h"
#include "ModelMedia.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLMediaDrawing
 * \brief Draw media data
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLMediaDrawing::BrainOpenGLMediaDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLMediaDrawing::~BrainOpenGLMediaDrawing()
{
}

/**
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param browserTabContent
 *    Content of the browser tab
 * @param mediaModel
 *    Media model for drawing
 * @param viewport
 *    Size of the viewport
 */
void
BrainOpenGLMediaDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                   BrowserTabContent* browserTabContent,
                                   ModelMedia* mediaModel,
                                   const std::array<int32_t, 4>& viewport)
{
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    CaretAssert(mediaModel);
    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_browserTabContent    = browserTabContent;
    m_mediaModel           = mediaModel;
    m_viewport             = viewport;
    
    if ((m_viewport[2] < 1)
        || (m_viewport[3] < 1)) {
        return;
    }

    
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);
    
    const float halfHeight(500);
    const float aspectRatio = (static_cast<float>(m_viewport[3])
                               / static_cast<float>(m_viewport[2]));
    const float halfWidth(halfHeight / aspectRatio);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-halfWidth,  halfWidth,
            -halfHeight, halfHeight,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    const float scaling = m_browserTabContent->getScaling();
    
    glTranslatef(translation[0], translation[1], 0.0);
    glScalef(scaling, scaling, 1.0);
    
    drawModelLayers();
}

/**
 * Draw the media models layers
 */
void
BrainOpenGLMediaDrawing::drawModelLayers()
{
    MediaOverlaySet* mediaOverlaySet = m_mediaModel->getMediaOverlaySet(m_browserTabContent->getTabNumber());
    CaretAssert(mediaOverlaySet);
    const int32_t numberOfOverlays = mediaOverlaySet->getNumberOfDisplayedOverlays();
    
    bool firstFlag(true);
    for (int32_t i = (numberOfOverlays - 1); i >= 0; i--) {
        MediaOverlay* overlay = mediaOverlaySet->getOverlay(i);
        CaretAssert(overlay);
        
        if (overlay->isEnabled()) {
            MediaFile* selectedFile(NULL);
            int32_t selectedIndex(-1);
            overlay->getSelectionData(selectedFile,
                                      selectedIndex);
            
            if (selectedFile != NULL) {
                ImageFile* imageFile = selectedFile->castToImageFile();
                if (imageFile != NULL) {
                    GraphicsPrimitiveV3fT3f* primitive = imageFile->getGraphicsPrimitiveForMediaDrawing();
                    if (primitive) {
                        GraphicsEngineDataOpenGL::draw(primitive);
                    }
                }
                else {
                    CaretAssertMessage(0, ("Unrecogized file type "
                                           + DataFileTypeEnum::toName(selectedFile->getDataFileType())
                                           + " for media drawing."));
                }
            }
        }
        
        if (firstFlag) {
            firstFlag = false;
        }
    }
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLMediaDrawing::toString() const
{
    return "BrainOpenGLMediaDrawing";
}

