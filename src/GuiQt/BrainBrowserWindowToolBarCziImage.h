#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_H__

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

#include "BrainBrowserWindowToolBarComponent.h"

class QAction;
class QRadioButton;
class QSpinBox;

namespace caret {

    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class CziImageFile;
    
    class BrainBrowserWindowToolBarCziImage : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarCziImage(BrainBrowserWindowToolBar* parentToolBar,
                                           const QString& parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarCziImage();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
    private slots:
        void pyramidLayerChanged(int value);
        
        void reloadActionTriggered();
        
    private:
        BrainBrowserWindowToolBarCziImage(const BrainBrowserWindowToolBarCziImage&);

        BrainBrowserWindowToolBarCziImage& operator=(const BrainBrowserWindowToolBarCziImage&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        CziImageFile* getCziImageFile(BrowserTabContent* browserTabContent);
        
        const BrainOpenGLViewportContent* getBrainOpenGLViewportContent();
        
        // ADD_NEW_MEMBERS_HERE

        BrainBrowserWindowToolBar* m_parentToolBar;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        QAction* m_reloadAction;
        
        QSpinBox* m_pyramidLayerSpinBox;
        
    };
    
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_RESOLUTION_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CZI_IMAGE_H__
