#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

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

#include <stdint.h>

#include <QDockWidget>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QTabWidget;

namespace caret {
    class AnnotationSelectionViewController;
    class BorderSelectionViewController;
    class ChartOverlaySetViewController;
    class ChartToolBoxViewController;
    class CiftiConnectivityMatrixViewController;
    class FiberOrientationSelectionViewController;
    class FociSelectionViewController;
    class ImageSelectionViewController;
    class LabelSelectionViewController;
    class OverlaySetViewController;
    class VolumeSurfaceOutlineSetViewController;
    
    class BrainBrowserWindowOrientedToolBox :  public QDockWidget, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        enum ToolBoxType {
            TOOL_BOX_FEATURES,
            TOOL_BOX_OVERLAYS_HORIZONTAL,
            TOOL_BOX_OVERLAYS_VERTICAL,
        };
        
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const ToolBoxType toolBoxType,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();

        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void floatingStatusChanged(bool);
        
        void restoreMinimumAndMaximumSizesAfterSceneRestored();
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        int addToTabWidget(QWidget* page,
                           const QString& label);
        
        OverlaySetViewController* m_overlaySetViewController;
        
        AnnotationSelectionViewController* m_annotationViewController;
        
        BorderSelectionViewController* m_borderSelectionViewController;

        ChartOverlaySetViewController* m_chartOverlaySetViewController;

        CiftiConnectivityMatrixViewController* m_connectivityMatrixViewController;
        
        ChartToolBoxViewController* m_chartToolBoxViewController;
        
        FiberOrientationSelectionViewController* m_fiberOrientationViewController;
        
        FociSelectionViewController* m_fociSelectionViewController;
        
        ImageSelectionViewController* m_imageSelectionViewController;
        
        LabelSelectionViewController* m_labelSelectionViewController;
        
        VolumeSurfaceOutlineSetViewController* m_volumeSurfaceOutlineSetViewController;
        
        QTabWidget* m_tabWidget;
        
        QString m_toolBoxTitle;
        
        int32_t m_browserWindowIndex;
        
        int32_t m_chartOverlayTabIndex;
        
        int32_t m_overlayTabIndex;
        
        int32_t m_annotationTabIndex;
        
        int32_t m_borderTabIndex;

        int32_t m_connectivityTabIndex;
        
        int32_t m_chartTabIndex;
        
        int32_t m_fiberOrientationTabIndex;
        
        int32_t m_fociTabIndex;
        
        int32_t m_imageTabIndex;
        
        int32_t m_labelTabIndex;
        
        int32_t m_volumeSurfaceOutlineTabIndex;
        
        QSize m_minimumSizeAfterSceneRestored;
        QSize m_maximumSizeAfterSceneRestored;
        
    };
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

