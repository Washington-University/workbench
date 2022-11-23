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

class QAction;
class QCheckBox;
class QLabel;
class QMenu;
class QToolButton;

namespace caret {
    class BrainBrowserWindowToolBar;
    class ClippingPlanesWidget;
    class EnumComboBoxTemplate;
    class ScaleBarWidget;
    
    class BrainBrowserWindowToolBarTab : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                     QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                     BrainBrowserWindowToolBar* parentToolBar,
                                     const QString& objectNamePrefix);
        
        virtual ~BrainBrowserWindowToolBarTab();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void yokeToGroupComboBoxIndexChanged();
        
        void lightingEnabledCheckBoxChecked(bool checked);
        
        void clippingPlanesActionToggled(bool checked);
        
        void clippingPlanesMenuAboutToShow();
        
        void scaleBarActionToggled(bool checked);
        
        void scaleBarMenuAboutToShow();
        
    private:
        BrainBrowserWindowToolBarTab(const BrainBrowserWindowToolBarTab&);

        BrainBrowserWindowToolBarTab& operator=(const BrainBrowserWindowToolBarTab&);
        
        QMenu* createClippingPlanesMenu();
        
        QMenu* createScaleBarMenu();
        
        QPixmap createScaleBarPixmap(const QWidget* widget);
        
        QString m_objectNamePrefix;
        
        QLabel* m_yokeToLabel;
        
        EnumComboBoxTemplate* m_yokingGroupComboBox;

        const int32_t m_browserWindowIndex;
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QToolButton* m_lockWindowAndAllTabAspectButton;
        
        QAction* m_lightingAction;
        
        QLabel* m_macroRecordingLabel;
        
        QAction* m_clippingPlanesAction;
        
        ClippingPlanesWidget* m_clippingPlanesWidget;
        
        QAction* m_scaleBarAction;
        
        ScaleBarWidget* m_scaleBarWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_H__
