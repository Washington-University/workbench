#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include "BrainBrowserWindowToolBarComponent.h"

class QLabel;
class QToolButton;

namespace caret {
    class BrainBrowserWindowToolBar;
    class EnumComboBoxTemplate;
    
    class BrainBrowserWindowToolBarTab : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                     QAction* windowAspectRatioLockedAction,
                                     QAction* tabAspectRatioLockedAction,
                                     BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarTab();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void yokeToGroupComboBoxIndexChanged();
        
        void tabAspectCustomContextMenuRequested(const QPoint& pos);
        
        void windowAspectCustomContextMenuRequested(const QPoint& pos);
        
    private:
        BrainBrowserWindowToolBarTab(const BrainBrowserWindowToolBarTab&);

        BrainBrowserWindowToolBarTab& operator=(const BrainBrowserWindowToolBarTab&);
        
        float getAspectRatioFromDialog(const QString& title,
                                       const float aspectRatio,
                                       QWidget* parent) const;
        
        QLabel* m_yokeToLabel;
        
        EnumComboBoxTemplate* m_yokingGroupComboBox;

        const int32_t m_browserWindowIndex;
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QAction* m_tabAspectRatioLockedAction;
        
        QToolButton* m_tabAspectRatioLockedToolButton;
        
        QToolButton* m_windowAspectRatioLockedToolButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_H__
