#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_H__

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


#include "BrainBrowserWindowToolBarComponent.h"

class QActionGroup;
class QComboBox;
class QDoubleSpinBox;
class QMenu;
class QPixmap;

namespace caret {

    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarSlicePlane : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarSlicePlane(const QString& parentObjectName,
                                            BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarSlicePlane();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void volumePlaneAxialToolButtonActionTriggered(bool checked);
        
        void volumePlaneCoronalToolButtonActionTriggered(bool checked);
        
        void volumePlaneParasagittalToolButtonActionTriggered(bool checked);
        
        void volumeAxisCrosshairsTriggered(bool checked);
        void volumeAxisCrosshairLabelsTriggered(bool checked);
        
        void volumeLayoutComboBoxActivated(int index);
        
        void crosshairMenuAboutToShow();
        
    private:
        BrainBrowserWindowToolBarSlicePlane(const BrainBrowserWindowToolBarSlicePlane&);

        BrainBrowserWindowToolBarSlicePlane& operator=(const BrainBrowserWindowToolBarSlicePlane&);
        
        QMenu* createCrosshairMenu(const QString& objectNamePrefix);
        
        void crosshairGapSpinBoxValueChanged(double value);
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QAction* m_volumePlaneParasagittalToolButtonAction;
        QAction* m_volumePlaneCoronalToolButtonAction;
        QAction* m_volumePlaneAxialToolButtonAction;
        QAction* m_volumePlaneAllToolButtonAction;
        QAction* m_volumeAxisCrosshairsToolButtonAction;
        QAction* m_volumeAxisCrosshairLabelsToolButtonAction;
        
        QComboBox* m_volumeLayoutComboBox;
        
        QDoubleSpinBox* m_crosshairGapSpinBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_H__
