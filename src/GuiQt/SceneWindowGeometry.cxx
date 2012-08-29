
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>

#define __SCENE_WINDOW_GEOMETRY_DECLARE__
#include "SceneWindowGeometry.h"
#undef __SCENE_WINDOW_GEOMETRY_DECLARE__

#include "BrainBrowserWindow.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SceneWindowGeometry 
 * \brief Assists with saving and restoring a window's geometry
 */

/**
 * Constructor for window that is child of the first browser window.
 * @param window
 *    The window for which geometry is restored/saved
 */
SceneWindowGeometry::SceneWindowGeometry(QWidget* window)
: CaretObject()
{
    m_window = window;
    m_parentWindow = NULL;
}

/**
 * Constructor for window that is child of the given parent window.
 * The window is ALWAYS positioned relative to the parent.  
 * @param window
 *    The window for which geometry is restored/saved
 * @param parentWindow
 *    The parent window of 'window'.
 */
SceneWindowGeometry::SceneWindowGeometry(QWidget* window,
                                         QWidget* parentWindow)
: CaretObject()
{
    m_window = window;
    m_parentWindow = parentWindow;
}

/**
 * Destructor.
 */
SceneWindowGeometry::~SceneWindowGeometry()
{
    
}

/**
 * Set the coordinates of the first browser window invalid.
 * Normally called at beginning of scene restoration prior
 * to restoration of windows.
 */
void
SceneWindowGeometry::setFirstBrowserWindowCoordinatesInvalid()
{
    s_firstBrowserWindowCoordinatesValid = false;
    s_firstBrowserWindow = NULL;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
SceneWindowGeometry::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SceneWindowGeometry",
                                            1);
    
    /*
     * Position and size
     */
    sceneClass->addInteger("geometryX",
                           m_window->x());
    sceneClass->addInteger("geometryY",
                           m_window->y());
    sceneClass->addInteger("geometryWidth",
                           m_window->width());
    sceneClass->addInteger("geometryHeight",
                           m_window->height());
    int32_t geometryOffsetX = 0;
    int32_t geometryOffsetY = 0;
    if (m_parentWindow != NULL) {
        geometryOffsetX = m_window->x() - m_parentWindow->x();
        geometryOffsetY = m_window->y() - m_parentWindow->y();
    }
    sceneClass->addInteger("geometryOffsetX",
                           geometryOffsetX);
    sceneClass->addInteger("geometryOffsetY",
                           geometryOffsetY);
    sceneClass->addBoolean("visibility",
                           m_window->isVisible());
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
SceneWindowGeometry::restoreFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Get window geometry from the scene
     */
    const int32_t sceneX = sceneClass->getIntegerValue("geometryX",
                                                       100);
    const int32_t sceneY = sceneClass->getIntegerValue("geometryY",
                                                       100);
    const int32_t sceneWidth = sceneClass->getIntegerValue("geometryWidth",
                                                           -1);
    const int32_t sceneHeight = sceneClass->getIntegerValue("geometryHeight",
                                                            -1);
    
    /*
     * Is this window always positions relative to parent window?
     */
    if (m_parentWindow != NULL) {
        /*
         * Get offset from parent
         */
        const int32_t offsetX = sceneClass->getIntegerValue("geometryOffsetX", 100);
        const int32_t offsetY = sceneClass->getIntegerValue("geometryOffsetY", 100);
        const int32_t windowX = m_parentWindow->x() + offsetX;
        const int32_t windowY = m_parentWindow->y() + offsetY;
        
        /*
         * Move the window to its desired position and set its width/height
         * but limit to available screen space
         */
        int32_t xywh[4];
        WuQtUtilities::moveAndSizeWindow(m_window,
                                         windowX,
                                         windowY,
                                         sceneWidth,
                                         sceneHeight,
                                         xywh);
        return;
    }
        
    /*
     * Visibility
     */
    const bool windowVisible = sceneClass->getBooleanValue("visibility",
                                                           true);
    m_window->setVisible(windowVisible);
    
    /*
     * Is this a browser window?
     */
    bool isFirstBrowserWindow = false;
    BrainBrowserWindow* browserWindow = dynamic_cast<BrainBrowserWindow*>(m_window);
    if (browserWindow != NULL) {
        /*
         * Is this the first browser window being restored?
         * If so, save its positions from scene file and
         * its actual position.
         */
        if (s_firstBrowserWindowCoordinatesValid == false) {
            isFirstBrowserWindow = true;
        }
    }
    
    /*
     * Determine how windows are positions and sized
     */
    bool isResizeWindow = false;
    bool isMoveWindow   = false;
    bool isMoveWindowRelative = false;
    switch (sceneAttributes->getRestoreWindowBehavior()) {
        case SceneAttributes::RESTORE_WINDOW_USE_ALL_POSITIONS_AND_SIZES:
            isResizeWindow = true;
            isMoveWindow   = true;
            break;
        case SceneAttributes::RESTORE_WINDOW_IGNORE_ALL_POSITIONS_AND_SIZES:
            break;
        case SceneAttributes::RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES:
            if (isFirstBrowserWindow == false) {
                isMoveWindow = true;
                isMoveWindowRelative = true;
            }
            isResizeWindow = true;
            break;
    }
    
//    const QDesktopWidget* desktopWidget = QApplication::desktop();
//    const QSize screenSize = WuQtUtilities::getMinimumScreenSize();
//    const int maxX = screenSize.width() - 100;
//    const int maxY = screenSize.height() - 100;
    
    /*
     * Position of window defaults to window's current position
     */
    int32_t windowX = m_window->x();
    int32_t windowY = m_window->y();
    if (isMoveWindow) {
        if ((sceneX > 0)
            & (sceneY > 0)) {
            /*
             * Use position in scene
             */
            windowX = sceneX;
            windowY = sceneY;
            
            /*
             * Move position relative to first window?
             */
            if (isMoveWindowRelative) {
                if ((s_firstBrowserWindowRestoredX > 0)
                    && (s_firstBrowserWindowRestoredY > 0)) {
                    const int32_t dx = windowX - s_firstBrowserWindowSceneX;
                    const int32_t dy = windowY - s_firstBrowserWindowSceneY;
                    windowX = s_firstBrowserWindowRestoredX + dx;
                    windowY = s_firstBrowserWindowRestoredY + dy;
                }
                else {
                    
                }
            }
        }
    }
    
    /*
     * NOT Resize window?
     */
    int32_t windowWidth = sceneWidth;
    int32_t windowHeight = sceneHeight;
    if (isResizeWindow == false) {
        windowWidth  = -1;
        windowHeight = -1;
    }
    
    /*
     * Move the window to its desired position and set its width/height
     * but limit to available screen space
     */
    int32_t xywh[4];
    WuQtUtilities::moveAndSizeWindow(m_window,
                                     windowX,
                                     windowY,
                                     windowWidth,
                                     windowHeight,
                                     xywh);
    
    /*
     * Is this a browser window?
     */
    if (isFirstBrowserWindow) {
        s_firstBrowserWindowCoordinatesValid = true;
        s_firstBrowserWindow          = browserWindow;
        s_firstBrowserWindowSceneX    = sceneX;
        s_firstBrowserWindowSceneY    = sceneY;
        s_firstBrowserWindowRestoredX = xywh[0];
        s_firstBrowserWindowRestoredY = xywh[1];
    }
}
