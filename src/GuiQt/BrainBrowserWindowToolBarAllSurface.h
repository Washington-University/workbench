#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <memory>

#include "BrainBrowserWindowToolBarComponent.h"
#include "StructureEnum.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QMenu;

namespace caret {
    class WuQWidgetObjectGroup;

    class BrainBrowserWindowToolBarAllSurface : public BrainBrowserWindowToolBarComponent {
        
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarAllSurface(const QString& parentObjectName,
                                            BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarAllSurface();
        
        BrainBrowserWindowToolBarAllSurface(const BrainBrowserWindowToolBarAllSurface&) = delete;

        BrainBrowserWindowToolBarAllSurface& operator=(const BrainBrowserWindowToolBarAllSurface&) = delete;
        
        virtual void updateContent(BrowserTabContent* browserTabContent) override;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void wholeBrainSurfaceTypeComboBoxIndexChanged(int indx);
        void wholeBrainSurfaceLeftCheckBoxStateChanged(int state);
        void wholeBrainSurfaceRightCheckBoxStateChanged(int state);
        void wholeBrainSurfaceCerebellumCheckBoxStateChanged(int state);
        void wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double d);
        void wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double d);
        void wholeBrainSurfaceLeftToolButtonTriggered(bool checked);
        void wholeBrainSurfaceRightToolButtonTriggered(bool checked);
        void wholeBrainSurfaceCerebellumToolButtonTriggered(bool checked);
        void wholeBrainSurfaceMatchCheckBoxClicked(bool checked);
        
        void wholeBrainSurfaceLeftMenuTriggered(QAction*);
        void wholeBrainSurfaceRightMenuTriggered(QAction*);
        void wholeBrainSurfaceCerebellumMenuTriggered(QAction*);
        
    private:
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QComboBox*      wholeBrainSurfaceTypeComboBox;
        QCheckBox*      wholeBrainSurfaceLeftCheckBox;
        QCheckBox*      wholeBrainSurfaceRightCheckBox;
        QCheckBox*      wholeBrainSurfaceCerebellumCheckBox;
        QMenu*          wholeBrainSurfaceLeftMenu;
        QMenu*          wholeBrainSurfaceRightMenu;
        QMenu*          wholeBrainSurfaceCerebellumMenu;
        QDoubleSpinBox* wholeBrainSurfaceSeparationLeftRightSpinBox;
        QDoubleSpinBox* wholeBrainSurfaceSeparationCerebellumSpinBox;
        QCheckBox*      wholeBrainSurfaceMatchCheckBox;
        void updateAllWholeBrainSurfaceMenus();
        void updateWholeBrainSurfaceMenu(QMenu* menu,
                                         const StructureEnum::Enum structure);
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_H__
