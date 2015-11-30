
#ifndef __BRAIN_OPENGL_WIDGET_H__
#define __BRAIN_OPENGL_WIDGET_H__

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

#include "CaretOpenGLInclude.h"

#include <QGLWidget>
#include <QImage>

#include <stdint.h>
#include "BrainConstants.h"
#include "EventListenerInterface.h"

class QMouseEvent;

namespace caret {

    class Border;
    class BrainOpenGL;
    class BrainOpenGLTextRenderInterface;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class EventImageCapture;
    class SelectionItemAnnotation;
    class SelectionManager;
    class Model;
    class MouseEvent;
    class SurfaceProjectedItem;
    class UserInputModeAnnotations;
    class UserInputModeBorders;
    class UserInputModeFoci;
    class UserInputModeView;
    class UserInputModeVolumeEdit;
    class UserInputModeAbstract;
    class VolumeFile;
    
    class BrainOpenGLWidget : public QGLWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainOpenGLWidget(QWidget* parent,
                          const int32_t windowIndex);
        
        ~BrainOpenGLWidget();
        
        void receiveEvent(Event* event);
        
        SelectionManager* performIdentification(const int x,
                                                const int y,
                                                const bool applySelectionBackgroundFiltering);
        
        SelectionItemAnnotation* performIdentificationAnnotations(const int x,
                                                           const int y);
        
        SelectionManager* performIdentificationVoxelEditing(VolumeFile* editingVolumeFile,
                                                            const int x,
                                                            const int y);
        
        void performProjection(const int x,
                               const int y,
                               SurfaceProjectedItem& projectionOut);
 
        Border* getBorderBeingDrawn();
        
        static void initializeDefaultGLFormat();
        
        QString getOpenGLInformation();

        void updateCursor();
        
        std::vector<const BrainOpenGLViewportContent*> getViewportContent() const;

    protected:
        virtual void initializeGL();
        
        virtual void resizeGL(int w, int h);
        
        virtual void paintGL();
        
        virtual void contextMenuEvent(QContextMenuEvent* contextMenuEvent);
        
        virtual bool event(QEvent* event);
        
        virtual void keyPressEvent(QKeyEvent* e);
        
        virtual void keyReleaseEvent(QKeyEvent* e);
        
        virtual void mouseDoubleClickEvent(QMouseEvent* e);
        
        virtual void mouseMoveEvent(QMouseEvent* e);
        
        virtual void mousePressEvent(QMouseEvent* e);
        
        virtual void mouseReleaseEvent(QMouseEvent* e);
        
        virtual void wheelEvent(QWheelEvent* e);
        
    private:
        
        BrainOpenGLTextRenderInterface* createTextRenderer();
        
        void clearDrawingViewportContents();
        
        BrainOpenGLViewportContent* getViewportContentAtXY(const int x,
                                                           const int y);
        
        void checkForMiddleMouseButton(Qt::MouseButtons& mouseButtons,
                                       Qt::MouseButton& button,
                                       Qt::KeyboardModifiers& keyModifiers,
                                       const bool isMouseMoving);
        
        void captureImage(EventImageCapture* imageCaptureEvent);
        
        BrainOpenGL* openGL;
        
        int32_t windowIndex;
        
        std::vector<BrainOpenGLViewportContent*> drawingViewportContents;
        
        int32_t windowWidth[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        int32_t windowHeight[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        int32_t mouseMovementMinimumX;
        int32_t mouseMovementMaximumX;
        int32_t mouseMovementMinimumY;
        int32_t mouseMovementMaximumY;
        
        bool mouseNewDraggingStartedFlag;
        
        static const int32_t MOUSE_MOVEMENT_TOLERANCE;
        
        int mousePressX;
        int mousePressY;
        bool isMousePressedNearToolBox;
        
        int lastMouseX;
        
        int lastMouseY;
        
        bool m_newKeyPressStartedFlag;
        
        UserInputModeAbstract* selectedUserInputProcessor;
        UserInputModeAnnotations* userInputAnnotationsModeProcessor;
        UserInputModeView* userInputViewModeProcessor;
        UserInputModeBorders* userInputBordersModeProcessor;
        UserInputModeFoci* userInputFociModeProcessor;
        UserInputModeVolumeEdit* userInputVolumeEditModeProcessor;
        
        Border* borderBeingDrawn;
        
        static bool s_defaultGLFormatInitialized;
    };
    
#ifdef __BRAIN_OPENGL_WIDGET_DEFINE__
    const int32_t BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE = 0; //10;
    bool BrainOpenGLWidget::s_defaultGLFormatInitialized = false;
#endif // __BRAIN_OPENGL_WIDGET_DEFINE__
    
} // namespace

#endif // __BRAIN_OPENGL_WIDGET_H__
