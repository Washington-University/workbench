#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_H__

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
#include "VolumeSliceViewPlaneEnum.h"

class QAction;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;
class QToolButton;
class WuQWidgetObjectGroup;

namespace caret {

    class EnumComboBoxTemplate;
    
    class BrainBrowserWindowToolBarSliceSelection : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarSliceSelection(BrainBrowserWindowToolBar* parentToolBar,
                                                const QString parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarSliceSelection();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);

        virtual void receiveEvent(Event* event) override;

        // ADD_NEW_METHODS_HERE

    private slots:
        void volumeIndicesOriginActionTriggered();
        void volumeIndicesParasagittalCheckBoxStateChanged(int state);
        void volumeIndicesCoronalCheckBoxStateChanged(int state);
        void volumeIndicesAxialCheckBoxStateChanged(int state);
        void volumeIndicesParasagittalSpinBoxValueChanged(int sliceIndex);
        void volumeIndicesCoronalSpinBoxValueChanged(int sliceIndex);
        void volumeIndicesAxialSpinBoxValueChanged(int sliceIndex);
        void volumeIndicesXcoordSpinBoxValueChanged(double d);
        void volumeIndicesYcoordSpinBoxValueChanged(double d);
        void volumeIndicesZcoordSpinBoxValueChanged(double d);
        void volumeSliceProjectionTypeEnumComboBoxItemActivated();
        void volumeIdentificationToggled(bool value);
        void obliqueMaskingActionTriggered(bool);
        void intensityModeActionTriggered(bool);
        
    private:
        BrainBrowserWindowToolBarSliceSelection(const BrainBrowserWindowToolBarSliceSelection&);

        BrainBrowserWindowToolBarSliceSelection& operator=(const BrainBrowserWindowToolBarSliceSelection&);
        
        void readVolumeSliceCoordinatesAndUpdateSliceIndices();
        void readVolumeSliceIndicesAndUpdateSliceCoordinates(const VolumeSliceViewPlaneEnum::Enum viewPlane,
                                                             const int64_t sliceIndex);
        void updateSliceIndicesAndCoordinatesRanges();
        
        void updateIntensityMaskingButton();
        
        QPixmap createVolumeIdentificationUpdatesSlicesIcon(const QWidget* widget);
        

        BrainBrowserWindowToolBar* m_parentToolBar;
        
        WuQWidgetObjectGroup* m_volumeIndicesWidgetGroup;
        
        QAction*   m_volumeIdentificationUpdatesSlicesAction;
        QCheckBox* m_volumeIndicesParasagittalCheckBox;
        QCheckBox* m_volumeIndicesCoronalCheckBox;
        QCheckBox* m_volumeIndicesAxialCheckBox;
        QSpinBox*  m_volumeIndicesParasagittalSpinBox;
        QSpinBox*  m_volumeIndicesCoronalSpinBox;
        QSpinBox*  m_volumeIndicesAxialSpinBox;
        QDoubleSpinBox* m_volumeIndicesXcoordSpinBox;
        QDoubleSpinBox* m_volumeIndicesYcoordSpinBox;
        QDoubleSpinBox* m_volumeIndicesZcoordSpinBox;
        QToolButton* m_intensityMaskingToolButton;
        QAction* m_obliqueMaskingAction;
        QAction* m_intensityModeAction;
        
        EnumComboBoxTemplate* m_volumeSliceProjectionTypeEnumComboBox;
        // ADD_NEW_MEMBERS_HERE
        
        
    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_H__
