#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_H__

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

namespace caret {

    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class EnumComboBoxTemplate;
    class HistologySlice;
    class HistologySlicesFile;
    class Vector3D;
    class WuQDoubleSpinBox;
    class WuQSpinBox;

    class BrainBrowserWindowToolBarHistology : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarHistology(BrainBrowserWindowToolBar* parentToolBar,
                                           const QString& parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarHistology();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        void receiveEvent(Event* event) override;
        
    private:
        BrainBrowserWindowToolBarHistology(const BrainBrowserWindowToolBarHistology&);

        BrainBrowserWindowToolBarHistology& operator=(const BrainBrowserWindowToolBarHistology&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private slots:
        void sliceIndexValueChanged(int);
        
        void sliceNumberValueChanged(int);
        
        void planeXyzSpinBoxValueChanged();

        void stereotaxicXyzSpinBoxValueChanged();
        
        void identificationMovesSlicesActionTriggered(bool);
        
        void moveToCenterActionTriggered();
        
        void histologyDisplayCoordinateModeEnumComboBoxItemActivated();
        
    private:
        HistologySlicesFile* getHistologySlicesFile(BrowserTabContent* browserTabContent);
        
        const BrainOpenGLViewportContent* getBrainOpenGLViewportContent() const;
        
//        bool getPlaneAndStereotaxicAtViewportCenter(const HistologySlice* histologySlice,
//                                                    Vector3D& planeXyzOut,
//                                                    Vector3D& stereotaxicXyzOut);
        
        bool getPlaneCoordinateAtViewportCenter(Vector3D& planeXyzOut) const;
        
        bool getStereotaxicCoordinateAtViewportCenter(const HistologySlice* histologySlice,
                                                      Vector3D& stereotaxicXyzOut) const;
        // ADD_NEW_MEMBERS_HERE

        BrainBrowserWindowToolBar* m_parentToolBar;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        WuQSpinBox* m_sliceIndexSpinBox;
        
        WuQSpinBox* m_sliceNumberSpinBox;
        
        WuQDoubleSpinBox* m_planeXyzSpinBox[3];
        
        WuQDoubleSpinBox* m_stereotaxicXyzSpinBox[3];
        
        QAction* m_identificationMovesSlicesAction;
        
        QAction* m_moveToCenterAction;
        
        EnumComboBoxTemplate* m_histologyDisplayCoordinateModeEnumComboBox;
};
    
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_H__
