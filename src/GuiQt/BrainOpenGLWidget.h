
#ifndef __BRAIN_OPENGL_WIDGET_H__
#define __BRAIN_OPENGL_WIDGET_H__

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

#include <QGLWidget>
#include <QImage>

#include <stdint.h>
#include "BrainConstants.h"
#include "EventListenerInterface.h"

class QMouseEvent;

namespace caret {

    class Border;
    class BrainOpenGL;
    class BrainOpenGLWidgetTextRenderer;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class IdentificationManager;
    class Model;
    class MouseEvent;
    class SurfaceProjectedItem;
    class UserInputModeBorders;
    class UserInputModeView;
    class UserInputReceiverInterface;
    
    class BrainOpenGLWidget : public QGLWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainOpenGLWidget(QWidget* parent,
                          const int32_t windowIndex);
        
        ~BrainOpenGLWidget();
        
        //Model* getDisplayedModelController();
        
        void receiveEvent(Event* event);
        
        IdentificationManager* performIdentification(const int x,
                                                     const int y);
        
        void performProjection(const int x,
                               const int y,
                               SurfaceProjectedItem& projectionOut);
 
        QImage  captureImage(const int32_t imageSizeX,
                             const int32_t imageSizeY);
        
        Border* getBorderBeingDrawn();
        
    protected:
        void initializeGL();
        
        void resizeGL(int w, int h);
        
        void paintGL();
        
        void contextMenuEvent(QContextMenuEvent* contextMenuEvent);
        
        void mouseMoveEvent(QMouseEvent* e);
        
        void mousePressEvent(QMouseEvent* e);
        
        void mouseReleaseEvent(QMouseEvent* e);
        
        void wheelEvent(QWheelEvent* e);
        
    private:
        void processMouseEvent(MouseEvent* mouseEvent);
        
        void clearDrawingViewportContents();
        
        BrainOpenGLViewportContent* getViewportContentAtXY(const int x,
                                                           const int y);
        
        BrainOpenGL* openGL;
        
        //BrowserTabContent* browserTabContent;
        
        int32_t windowIndex;
        
        //int32_t windowTabIndex;
        
        //Model* modelController;
        
        std::vector<BrainOpenGLViewportContent*> drawingViewportContents;
        
        int32_t windowWidth[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        int32_t windowHeight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        int32_t mouseMovementMinimumX;
        int32_t mouseMovementMaximumX;
        int32_t mouseMovementMinimumY;
        int32_t mouseMovementMaximumY;
        
        static const int32_t MOUSE_MOVEMENT_TOLERANCE;
        
        int mousePressX;
        int mousePressY;
        bool isMousePressedNearToolBox;
        
        int lastMouseX;
        
        int lastMouseY;
        
        BrainOpenGLWidgetTextRenderer* textRenderer;
        
        UserInputReceiverInterface* selectedUserInputProcessor;
        UserInputModeView* userInputViewModeProcessor;
        UserInputModeBorders* userInputBordersModeProcessor;
        
        Border* borderBeingDrawn;
    };
    
#ifdef __BRAIN_OPENGL_WIDGET_DEFINE__
    const int32_t BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE = 2;
#endif // __BRAIN_OPENGL_WIDGET_DEFINE__
    
} // namespace

#endif // __BRAIN_OPENGL_WIDGET_H__
