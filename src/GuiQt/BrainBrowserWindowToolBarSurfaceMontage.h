#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__

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

class QCheckBox;
class QStackedWidget;

namespace caret {

    class BrowserTabContent;
    class EnumComboBoxTemplate;
    class Surface;
    class SurfaceMontageCerebellarComponent;
    class SurfaceMontageCerebralComponent;
    class SurfaceMontageFlatMapsComponent;
    class SurfaceMontageHippocampusComponent;
    class SurfaceSelectionViewController;
    
    class BrainBrowserWindowToolBarSurfaceMontage : public BrainBrowserWindowToolBarComponent {
        
        Q_OBJECT

    public:
        BrainBrowserWindowToolBarSurfaceMontage(BrainBrowserWindowToolBar* parentToolBar,
                                                const QString& parentObjectNamePrefix);
        
        virtual ~BrainBrowserWindowToolBarSurfaceMontage();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);

    private slots:
        void surfaceMontageConfigurationTypeEnumComboBoxItemActivated();
        
        void surfaceMontageLayoutOrientationEnumComboBoxItemActivated();
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage(const BrainBrowserWindowToolBarSurfaceMontage&);

        BrainBrowserWindowToolBarSurfaceMontage& operator=(const BrainBrowserWindowToolBarSurfaceMontage&);
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        SurfaceMontageCerebralComponent* m_cerebralComponent;
        
        SurfaceMontageCerebellarComponent* m_cerebellarComponent;
        
        SurfaceMontageFlatMapsComponent* m_flatMapsComponent;
        
        SurfaceMontageHippocampusComponent* m_hippocampusComponent;
        
        QStackedWidget* m_stackedWidget;
        
        EnumComboBoxTemplate* m_surfaceMontageConfigurationTypeEnumComboBox;
        
        EnumComboBoxTemplate* m_surfaceMontageLayoutOrientationEnumComboBox;
        
        // ADD_NEW_MEMBERS_HERE
        
    private slots:
        
    };
    
/* ===========================================================================*/
    
    class SurfaceMontageCerebralComponent : public QWidget {
        
        Q_OBJECT
        
    public:
        SurfaceMontageCerebralComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                        const QString& parentObjectNamePrefix);
        
        ~SurfaceMontageCerebralComponent();
        
        void updateContent(BrowserTabContent* browserTabContent);
        
    private slots:
        void leftSurfaceSelected(Surface*);
        void leftSecondSurfaceSelected(Surface*);
        void rightSurfaceSelected(Surface*);
        void rightSecondSurfaceSelected(Surface*);
        void checkBoxSelected(bool);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage* m_parentToolBarMontage;
        
        SurfaceSelectionViewController* m_leftSurfaceViewController;
        SurfaceSelectionViewController* m_leftSecondSurfaceViewController;
        SurfaceSelectionViewController* m_rightSurfaceViewController;
        SurfaceSelectionViewController* m_rightSecondSurfaceViewController;
        QCheckBox* m_leftCheckBox;
        QCheckBox* m_rightCheckBox;
        QCheckBox* m_surfaceMontageFirstSurfaceCheckBox;
        QCheckBox* m_surfaceMontageSecondSurfaceCheckBox;
        QCheckBox* m_lateralCheckBox;
        QCheckBox* m_medialCheckBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
    };
    
/* ===========================================================================*/
    
    class SurfaceMontageCerebellarComponent : public QWidget {
        
        Q_OBJECT
        
    public:
        SurfaceMontageCerebellarComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                          const QString& parentObjectNamePrefix);
        
        ~SurfaceMontageCerebellarComponent();
        
        void updateContent(BrowserTabContent* browserTabContent);
        
    private slots:
        void firstSurfaceSelected(Surface*);
        void secondSurfaceSelected(Surface*);
        void checkBoxSelected(bool);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage* m_parentToolBarMontage;
        
        SurfaceSelectionViewController* m_firstSurfaceViewController;
        SurfaceSelectionViewController* m_secondSurfaceViewController;
        QCheckBox* m_firstSurfaceCheckBox;
        QCheckBox* m_secondSurfaceCheckBox;
        QCheckBox* m_dorsalCheckBox;
        QCheckBox* m_ventralCheckBox;
        QCheckBox* m_anteriorCheckBox;
        QCheckBox* m_posteriorCheckBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
    };

    /* ===========================================================================*/
    
    class SurfaceMontageFlatMapsComponent : public QWidget {
        
        Q_OBJECT
        
    public:
        SurfaceMontageFlatMapsComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                        const QString& parentObjectNamePrefix);
        
        ~SurfaceMontageFlatMapsComponent();
        
        void updateContent(BrowserTabContent* browserTabContent);
        
        private slots:
        void leftSurfaceSelected(Surface*);
        void rightSurfaceSelected(Surface*);
        void cerebellumSurfaceSelected(Surface*);
        void checkBoxSelected(bool);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage* m_parentToolBarMontage;
        
        SurfaceSelectionViewController* m_leftSurfaceViewController;
        SurfaceSelectionViewController* m_rightSurfaceViewController;
        SurfaceSelectionViewController* m_cerebellumSurfaceViewController;
        QCheckBox* m_leftSurfaceCheckBox;
        QCheckBox* m_rightSurfaceCheckBox;
        QCheckBox* m_cerebellumSurfaceCheckBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
    };
    
    /* ===========================================================================*/
    
    class SurfaceMontageHippocampusComponent : public QWidget {
        
        Q_OBJECT
        
    public:
        SurfaceMontageHippocampusComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                        const QString& parentObjectNamePrefix);
        
        ~SurfaceMontageHippocampusComponent();
        
        void updateContent(BrowserTabContent* browserTabContent);
        
        private slots:
        void leftSurfaceSelected(Surface*);
        void leftSecondSurfaceSelected(Surface*);
        void rightSurfaceSelected(Surface*);
        void rightSecondSurfaceSelected(Surface*);
        void checkBoxSelected(bool);
        
    private:
        BrainBrowserWindowToolBarSurfaceMontage* m_parentToolBarMontage;
        
        SurfaceSelectionViewController* m_leftSurfaceViewController;
        SurfaceSelectionViewController* m_leftSecondSurfaceViewController;
        SurfaceSelectionViewController* m_rightSurfaceViewController;
        SurfaceSelectionViewController* m_rightSecondSurfaceViewController;
        QCheckBox* m_leftCheckBox;
        QCheckBox* m_rightCheckBox;
        QCheckBox* m_surfaceMontageFirstSurfaceCheckBox;
        QCheckBox* m_surfaceMontageSecondSurfaceCheckBox;
        QCheckBox* m_lateralCheckBox;
        QCheckBox* m_medialCheckBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
    };
    

#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_H__
