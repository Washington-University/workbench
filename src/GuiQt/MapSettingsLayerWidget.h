#ifndef __MAP_SETTINGS_LAYER_WIDGET_H__
#define __MAP_SETTINGS_LAYER_WIDGET_H__

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

class QDoubleSpinBox;

namespace caret {

    class EnumComboBoxTemplate;
    class Overlay;
    
    class MapSettingsLayerWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsLayerWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsLayerWidget();
        
        void updateContent(Overlay* overlay);
        
    private slots:
        void applyWholeBrainSelections();
        
        void applyVolumeToImageSelections();
        
    private:
        MapSettingsLayerWidget(const MapSettingsLayerWidget&);

        MapSettingsLayerWidget& operator=(const MapSettingsLayerWidget&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

        QWidget* createWholeBrainWidget();
        
        QWidget* createVolumeToImageWidget();
        
        QWidget* m_wholeBrainWidget;
        
        QWidget* m_volumeToImageWidget;
        
        Overlay* m_overlay;
        
        EnumComboBoxTemplate* m_wholeBrainVoxelDrawingModeComboBox;
        
        EnumComboBoxTemplate* m_volumeToImageModeComboBox;
        
        QDoubleSpinBox* m_volumeToImageThicknessSpinBox;
    };
    
#ifdef __MAP_SETTINGS_LAYER_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_LAYER_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_LAYER_WIDGET_H__
