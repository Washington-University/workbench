#ifndef __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE__H_
#define __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include <QWidget>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "MultiPageDialogPage.h"

class QCheckBox;
class QDoubleSpinBox;

namespace caret {
    class OutlineWidget;
    class SurfaceSelectionViewController;
    class VolumeSurfaceOutlineColorOrTabViewController;
    
    class DisplayControlVolumeSurfaceOutlinePage : public MultiPageDialogPage, public EventListenerInterface {
        
    public:
        DisplayControlVolumeSurfaceOutlinePage(QObject* parent);
        
        virtual ~DisplayControlVolumeSurfaceOutlinePage();
        
        virtual bool isPageValid();
        
        void receiveEvent(Event* event);
        
    private:
        DisplayControlVolumeSurfaceOutlinePage(const DisplayControlVolumeSurfaceOutlinePage&);

        DisplayControlVolumeSurfaceOutlinePage& operator=(const DisplayControlVolumeSurfaceOutlinePage&);
        
    protected:
        virtual void applyPageContent();
        
        virtual QWidget* createPageContent();
        
        virtual void updatePageContent();
        
    private:
        std::vector<OutlineWidget*> outlineWidgets;

        friend class OutlineWidget;
    };
    
    /// Contains controls for one volume outline selection controls
    class OutlineWidget : public CaretObject {
        
    public:
        OutlineWidget(DisplayControlVolumeSurfaceOutlinePage* parentPage,
                      const int32_t outlineIndex);
        
        ~OutlineWidget();
        
        void applyWidget();
        
        void updateWidget();

    private:
        QCheckBox* selectionCheckBox;
        
        VolumeSurfaceOutlineColorOrTabViewController* colorOrTabSelectionControl;
        
        QDoubleSpinBox* thicknessSpinBox;
        
        SurfaceSelectionViewController* surfaceSelectionViewController;
        
        friend class DisplayControlVolumeSurfaceOutlinePage;
        
        int32_t outlineIndex;
    };
#ifdef __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE_DECLARE__

} // namespace
#endif  //__DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE__H_
