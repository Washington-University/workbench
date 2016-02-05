#ifndef __OPERATION_SHOW_SCENE_H__
#define __OPERATION_SHOW_SCENE_H__

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


#include "AbstractOperation.h"

namespace caret {

    class BrainOpenGLFixedPipeline;
    
    class OperationShowScene : public AbstractOperation {

    public:
        static OperationParameters* getParameters();

        static void useParameters(OperationParameters* myParams, 
                                  ProgressObject* myProgObj);

        static AString getCommandSwitch();

        static AString getShortDescription();

        static bool isShowSceneCommandAvailable();
        
    private:
        static BrainOpenGLFixedPipeline* createBrainOpenGL(const int32_t windowIndex);
        
        static void writeImage(const AString& imageFileName,
                                  const int32_t imageIndex,
                                  const unsigned char* imageContent,
                                  const int32_t imageWidth,
                                  const int32_t imageHeight);
        
        static void estimateGraphicsSize(const SceneClass* windowSceneClass,
                                         float& estimatedWidthOut,
                                         float& estimatedHeightOut);
        
        static bool getToolBoxSize(const SceneClass* toolBoxClass,
                                   const SceneClass* activeToolBoxClass,
                                   float& overlayToolBoxWidthOut,
                                   float& overlayToolBoxHeightOut,
                                   QString& overlayToolBoxOrientationOut);

    };

    typedef TemplateAutoOperation<OperationShowScene> AutoOperationShowScene;

} // namespace

#endif  //__OPERATION_SHOW_SCENE_H__

