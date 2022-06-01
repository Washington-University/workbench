#ifndef __VOLUME_MPR_SETTINGS_WIDGET_H__
#define __VOLUME_MPR_SETTINGS_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include <QWidget>

#include "EventListenerInterface.h"

class QCheckBox;
class QDoubleSpinBox;

namespace caret {

    class BrowserTabContent;
    class EnumComboBoxTemplate;
    
    class VolumeMprSettingsWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        VolumeMprSettingsWidget(const QString& objectNamePrefix,
                                QWidget* parent = 0);
        
        virtual ~VolumeMprSettingsWidget();
        
        VolumeMprSettingsWidget(const VolumeMprSettingsWidget&) = delete;

        VolumeMprSettingsWidget& operator=(const VolumeMprSettingsWidget&) = delete;
        
        void updateContent(const int32_t tabIndex);
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void modeComboBoxActivated();
        
        void orientationComboBoxActivated();
        
        void sliceThicknessSpinBoxValueChanged(double);
        
        void allViewThicknessCheckBoxClicked(bool);
        
        void axialThickessCheckBoxClicked(bool);
        
        void coronalThickessCheckBoxClicked(bool);
        
        void parasagittalThickessCheckBoxClicked(bool);
        
    private:
        BrowserTabContent* getBrowserTabContent();
        
        void updateGraphicsWindow();
        
        void updateOrientationComboBoxColor();
        
        const QString m_objectNamePrefix;
        
        int32_t m_tabIndex = -1;
        
        EnumComboBoxTemplate* m_viewModeComboBox;
        
        QPalette m_viewModeComboBoxTextPalette;
        
        QPalette m_viewModeComboBoxRedTextPalette;
        
        EnumComboBoxTemplate* m_orientationComboBox;
        
        QDoubleSpinBox* m_sliceThicknessSpinBox;
        
        QCheckBox* m_allViewThicknessCheckBox;
        
        QCheckBox* m_axialSliceThicknessCheckBox;
        
        QCheckBox* m_coronalSliceThicknessCheckBox;
        
        QCheckBox* m_parasagittalSliceThicknessCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_MPR_SETTINGS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MPR_SETTINGS_WIDGET_DECLARE__

} // namespace
#endif  //__VOLUME_MPR_SETTINGS_WIDGET_H__
