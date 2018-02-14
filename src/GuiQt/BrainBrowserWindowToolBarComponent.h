#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_H__

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


#include <QWidget>

#include "EventListenerInterface.h"


namespace caret {

    class BrainBrowserWindowToolBar;
    class BrowserTabContent;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarComponent : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    protected:
        BrainBrowserWindowToolBarComponent(BrainBrowserWindowToolBar* parentToolBar);
        
    public:
        virtual ~BrainBrowserWindowToolBarComponent();
        
        virtual void updateContent(BrowserTabContent* browserTabContent) = 0;
        
        BrowserTabContent* getTabContentFromSelectedTab();
        
        void invalidateColoringAndUpdateGraphicsWindow();
        
        void updateGraphicsWindow();
        
        void updateGraphicsWindowAndYokedWindows();
        
        void updateUserInterface();
        
    private:
        BrainBrowserWindowToolBarComponent(const BrainBrowserWindowToolBarComponent&);

        BrainBrowserWindowToolBarComponent& operator=(const BrainBrowserWindowToolBarComponent&);
        
    protected:
        void addToWidgetGroup(QObject* qObject);
        
        void blockAllSignals(const bool status);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        WuQWidgetObjectGroup* m_widgetGroup;
        
        // ADD_NEW_MEMBERS_HERE

        friend class BrainBrowserWindowToolBarChartTwoOrientation;
    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_H__
