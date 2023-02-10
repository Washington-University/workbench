#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_H__

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

class QAction;
class QCheckBox;
class QDoubleSpinBox;
class QMenu;
class QSpinBox;

namespace caret {
    class EnumComboBoxTemplate;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarVolumeMontage : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarVolumeMontage(const QString& parentObjectName,
                                               BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarVolumeMontage();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);

        // ADD_NEW_METHODS_HERE

    private slots:
        void montageRowsSpinBoxValueChanged(int i);
        void montageColumnsSpinBoxValueChanged(int i);
        void montageSpacingSpinBoxValueChanged(int i);
        
        void montageEnabledActionToggled(bool);
        
        void showSliceCoordinateToolButtonClicked(bool);
        void sliceCoordinateTypeMenuAboutToShow();
        void sliceCoordinateTypeMenuTriggered(QAction* action);
        void slicePrecisionSpinBoxValueChanged(int);
        void sliceCoordinateFontHeightValueChanged(double);
        void sliceCoordinateTextAlignmentEnumComboBoxItemActivated();
        
    private:
        BrainBrowserWindowToolBarVolumeMontage(const BrainBrowserWindowToolBarVolumeMontage&);

        BrainBrowserWindowToolBarVolumeMontage& operator=(const BrainBrowserWindowToolBarVolumeMontage&);
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QSpinBox* m_montageRowsSpinBox;
        QSpinBox* m_montageColumnsSpinBox;
        QSpinBox* m_montageSpacingSpinBox;
        
        QAction* m_showSliceCoordinateAction;
        QMenu*    m_sliceCoordinateTypeMenu;
        QSpinBox* m_sliceCoordinatePrecisionSpinBox;
        QDoubleSpinBox* m_sliceCoordinateFontHeightSpinBox;
        EnumComboBoxTemplate* m_sliceCoordinateTextAlignmentEnumComboBox;
        
        QAction* m_montageEnabledAction;
        
        WuQWidgetObjectGroup* m_volumeMontageWidgetGroup;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_H__
