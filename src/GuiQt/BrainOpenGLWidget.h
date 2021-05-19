
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

#include <memory>
#include <set>

/*
 * When GLEW is used, CaretOpenGLInclude.h will include "Gl/glew.h".
 * Gl/glew.h MUST BE BEFORE Gl/gl.h and Gl/gl.h is included by
 * QGLWidget so, we must include CaretOpenGL.h before QGLWidget.
 */
#include "CaretOpenGLInclude.h"

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#include <QOpenGLWidget>
#else
#include <QGLWidget>
#endif

#include <QImage>

#include <stdint.h>
#include "BrainConstants.h"
#include "BrainOpenGLWindowContent.h"
#include "CaretPointer.h"
#include "EventListenerInterface.h"
#include "MouseEvent.h"
#include "UserInputModeEnum.h"
#include "WuQMacroMouseEventWidgetInterface.h"

class QGestureEvent;
class QMouseEvent;
class QWidget;

namespace caret {

    class BrainOpenGL;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class EventImageCapture;
    class GraphicsFramesPerSecond;
    class SelectionItemAnnotation;
    class SelectionManager;
    class Model;
    class SurfaceProjectedItem;
    class UserInputModeAbstract;
    class VolumeFile;
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    class BrainOpenGLWidget : public QOpenGLWidget, public EventListenerInterface, public WuQMacroMouseEventWidgetInterface {
#else
    class BrainOpenGLWidget : public QGLWidget, public EventListenerInterface, public WuQMacroMouseEventWidgetInterface {
#endif
        Q_OBJECT
        
    public:
        BrainOpenGLWidget(QWidget* parent,
                          const BrainOpenGLWidget* shareWidget,
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
 
        static void initializeDefaultGLFormat();
        
        QString getOpenGLInformation();

        void updateCursor();
        
        std::vector<const BrainOpenGLViewportContent*> getViewportContent() const;

        bool isOpenGLContextSharingValid() const;
        
        QImage performOffScreenImageCapture(const int32_t imageWidth,
                                            const int32_t imageHeight);
        
        virtual void processMouseEventFromMacro(QMouseEvent* me) override;
        
    protected:
        virtual void initializeGL() override;
        
        virtual void resizeGL(int w, int h) override;
        
        virtual void paintGL() override;
        
        virtual void contextMenuEvent(QContextMenuEvent* contextMenuEvent) override;
        
        virtual bool event(QEvent* event) override;
        
        virtual void keyPressEvent(QKeyEvent* e) override;
        
        virtual void keyReleaseEvent(QKeyEvent* e) override;
        
        virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
        
        virtual void mouseMoveEvent(QMouseEvent* e) override;
        
        virtual void mousePressEvent(QMouseEvent* e) override;
        
        virtual void mouseReleaseEvent(QMouseEvent* e) override;
        
        virtual void wheelEvent(QWheelEvent* e) override;
        
#if QT_VERSION >= 0x060000
        virtual void enterEvent(QEnterEvent* e) override;
#else
        virtual void enterEvent(QEvent* e) override;
#endif
        
        virtual void leaveEvent(QEvent* e) override;
        
    private slots:
        void showSelectedChartPointToolTip();
        
    private:
        
        std::vector<BrainOpenGLViewportContent*> getDrawingViewportContent(const int32_t windowViewportIn[4]) const;
        
        void getDrawingWindowContent(const int32_t windowViewportIn[4],
                                     BrainOpenGLWindowContent& windowContent) const;
        
        void clearDrawingViewportContents();
        
        const BrainOpenGLViewportContent* getViewportContentAtXY(const int x,
                                                           const int y);
        
        const BrainOpenGLViewportContent* getViewportContentManualLayoutWithoutLockAspectAtXY(const int x,
                                                                                              const int y);
        
        void checkForMiddleMouseButton(Qt::MouseButtons& mouseButtons,
                                       Qt::MouseButton& button,
                                       Qt::KeyboardModifiers& keyModifiers,
                                       const bool isMouseMoving);
        
        void captureImage(EventImageCapture* imageCaptureEvent);
        
        void repaintGraphics();
        
        bool processGestureEvent(QGestureEvent* gestureEvent);
        
        UserInputModeAbstract* getSelectedInputProcessor() const;
        
        UserInputModeEnum::Enum getSelectedInputMode() const;
        
        const int32_t windowIndex;
        
        BrainOpenGLWindowContent m_windowContent;
        
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
        
        std::vector<MouseEvent::XY> m_mouseHistoryXY;
        
        bool m_newKeyPressStartedFlag;
        
        bool    m_mousePositionValid;
        CaretPointer<MouseEvent> m_mousePositionEvent;
        
        bool m_openGLContextSharingValid = false;
        
        void* m_contextShareGroupPointer = NULL;
        
        std::unique_ptr<GraphicsFramesPerSecond> m_graphicsFramesPerSecond;
        
        struct SelectedChartPointToolTipInfo {
            QPoint m_position;
            QString  m_text;
        };
        
        SelectedChartPointToolTipInfo m_selectedChartPointToolTipInfo;
        
        static bool s_defaultGLFormatInitialized;
        
        static std::set<BrainOpenGLWidget*> s_brainOpenGLWidgets;
        
        static BrainOpenGL* s_singletonOpenGL;
    };
    
#ifdef __BRAIN_OPENGL_WIDGET_DEFINE__
        const int32_t BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE = 0;
        bool BrainOpenGLWidget::s_defaultGLFormatInitialized = false;
        std::set<BrainOpenGLWidget*> BrainOpenGLWidget::s_brainOpenGLWidgets;
        BrainOpenGL* BrainOpenGLWidget::s_singletonOpenGL = NULL;
#endif // __BRAIN_OPENGL_WIDGET_DEFINE__
    
} // namespace

#endif // __BRAIN_OPENGL_WIDGET_H__
