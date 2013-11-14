#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include "BrainBrowserWindowToolBarComponent.h"

class QCheckBox;
class QStackedWidget;

namespace caret {

    class BrowserTabContent;
    class Surface;
    class SurfaceMontageCerebralComponent;
    class SurfaceSelectionViewController;
    
    class BrainBrowserWindowToolBarSurfaceMontage : public BrainBrowserWindowToolBarComponent {
        
        Q_OBJECT

    public:
        BrainBrowserWindowToolBarSurfaceMontage(BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarSurfaceMontage();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage(const BrainBrowserWindowToolBarSurfaceMontage&);

        BrainBrowserWindowToolBarSurfaceMontage& operator=(const BrainBrowserWindowToolBarSurfaceMontage&);
        
    private:
        SurfaceMontageCerebralComponent* m_cerebralComponent;
        
        QStackedWidget* m_stackedWidget;
        // ADD_NEW_MEMBERS_HERE
        
    private slots:
        
    };
    
    class SurfaceMontageCerebralComponent : public QWidget {
        
        Q_OBJECT
        
    public:
        SurfaceMontageCerebralComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage);
        
        ~SurfaceMontageCerebralComponent();
        
        void updateContent(BrowserTabContent* browserTabContent);
        
    private slots:
        void surfaceMontageLeftSurfaceSelected(Surface*);
        void surfaceMontageLeftSecondSurfaceSelected(Surface*);
        void surfaceMontageRightSurfaceSelected(Surface*);
        void surfaceMontageRightSecondSurfaceSelected(Surface*);
        void surfaceMontageCheckBoxSelected(bool);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage* m_parentToolBarMontage;
        
        SurfaceSelectionViewController* m_surfaceMontageLeftSurfaceViewController;
        SurfaceSelectionViewController* m_surfaceMontageLeftSecondSurfaceViewController;
        SurfaceSelectionViewController* m_surfaceMontageRightSurfaceViewController;
        SurfaceSelectionViewController* m_surfaceMontageRightSecondSurfaceViewController;
        QCheckBox* m_surfaceMontageLeftCheckBox;
        QCheckBox* m_surfaceMontageRightCheckBox;
        QCheckBox* m_surfaceMontageFirstSurfaceCheckBox;
        QCheckBox* m_surfaceMontageSecondSurfaceCheckBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
    };
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__
