#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

class QToolButton;

namespace caret {

    class BrainBrowserWindowToolBarChartOrientation : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartOrientation(BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarChartOrientation();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void customViewActionTriggered();
        
        void orientationResetToolButtonTriggered(bool /*checked*/);
    
    private:
        BrainBrowserWindowToolBarChartOrientation(const BrainBrowserWindowToolBarChartOrientation&);

        BrainBrowserWindowToolBarChartOrientation& operator=(const BrainBrowserWindowToolBarChartOrientation&);
        
        QAction* m_orientationResetToolButtonAction;
        
        QAction* m_customViewAction;
        
        QToolButton* m_orientationCustomViewSelectToolButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_H__
