
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
    class ModelDisplayController;
    
    class BrainOpenGLWidget : public QGLWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainOpenGLWidget(QWidget* parent,
                          const int32_t windowIndex);
        
        ~BrainOpenGLWidget();
        
        //ModelDisplayController* getDisplayedModelController();
        
        void mouseMoveEvent(QMouseEvent* e);
        
        void mousePressEvent(QMouseEvent* e);
        
        void mouseReleaseEvent(QMouseEvent* e);
        
        void receiveEvent(Event* event);
        
    protected:
        void initializeGL();
        
        void resizeGL(int w, int h);
        
        void paintGL();
        
    private:
        BrainOpenGL* openGL;
        
        BrowserTabContent* browserTabContent;
        
        int32_t windowIndex;
        
        int32_t windowTabIndex;
        
        ModelDisplayController* modelController;
        
        int32_t windowWidth[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        int32_t windowHeight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        int32_t mouseMovedBounds[4];
        
        /// left mouse button pressonly
        static int leftMouseButtonPressMask;
        
        /// left mouse button + shift press
        static int leftShiftMouseButtonPressMask;
        
        /// left mouse button + control press
        static int leftControlMouseButtonPressMask;
        
        /// left mouse button only move
        static int leftMouseButtonMoveMask;
        
        /// left mouse button + shift move
        static int leftShiftMouseButtonMoveMask;
        
        /// left mouse button + control move
        static int leftControlMouseButtonMoveMask;
        
        /// left mouse button + alt move
        static int leftAltMouseButtonMoveMask;
        
        int lastMouseX;
        
        int lastMouseY;
    };
    
#ifdef __BRAIN_OPENGL_WIDGET_DEFINE__
    int BrainOpenGLWidget::leftMouseButtonPressMask = 0;
    int BrainOpenGLWidget::leftShiftMouseButtonPressMask = 0;
    int BrainOpenGLWidget::leftControlMouseButtonPressMask = 0;
    int BrainOpenGLWidget::leftMouseButtonMoveMask = 0;
    int BrainOpenGLWidget::leftShiftMouseButtonMoveMask = 0;
    int BrainOpenGLWidget::leftControlMouseButtonMoveMask = 0;
    int BrainOpenGLWidget::leftAltMouseButtonMoveMask = 0;
#endif // __BRAIN_OPENGL_WIDGET_DEFINE__
    
} // namespace

#endif // __BRAIN_OPENGL_WIDGET_H__
