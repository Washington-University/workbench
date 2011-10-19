
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

#include <stdint.h>
#include "BrainConstants.h"
#include "EventListenerInterface.h"

class QMouseEvent;

namespace caret {

    class BrainOpenGL;
    class BrowserTabContent;
    class IdentificationManager;
    class ModelDisplayController;
    class MouseEvent;
    class UserInputModeView;
    
    class BrainOpenGLWidget : public QGLWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainOpenGLWidget(QWidget* parent,
                          const int32_t windowIndex);
        
        ~BrainOpenGLWidget();
        
        //ModelDisplayController* getDisplayedModelController();
        
        void receiveEvent(Event* event);
        
        IdentificationManager* performIdentification(const int x,
                                                     const int y);
        
    protected:
        void initializeGL();
        
        void resizeGL(int w, int h);
        
        void paintGL();
        
        void mouseMoveEvent(QMouseEvent* e);
        
        void mousePressEvent(QMouseEvent* e);
        
        void mouseReleaseEvent(QMouseEvent* e);
        
    private:
        void processMouseEvent(MouseEvent* mouseEvent);
        
        BrainOpenGL* openGL;
        
        BrowserTabContent* browserTabContent;
        
        int32_t windowIndex;
        
        int32_t windowTabIndex;
        
        ModelDisplayController* modelController;
        
        int32_t windowWidth[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        int32_t windowHeight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        int32_t mouseMovementMinimumX;
        int32_t mouseMovementMaximumX;
        int32_t mouseMovementMinimumY;
        int32_t mouseMovementMaximumY;
        
        static const int32_t MOUSE_MOVEMENT_TOLERANCE;
        
        int lastMouseX;
        
        int lastMouseY;
        
        UserInputModeView* userInputViewModeProcessor;
    };
    
#ifdef __BRAIN_OPENGL_WIDGET_DEFINE__
    const int32_t BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE = 2;
#endif // __BRAIN_OPENGL_WIDGET_DEFINE__
    
} // namespace

#endif // __BRAIN_OPENGL_WIDGET_H__
