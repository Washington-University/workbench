#ifndef __MAP_SETTINGS_MODULATE_WIDGET_H__
#define __MAP_SETTINGS_MODULATE_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include <memory>

class QCheckBox;

namespace caret {

    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class DataFileColorModulateSelector;
    
    class MapSettingsModulateWidget : public QWidget {
        Q_OBJECT
        
    public:
        MapSettingsModulateWidget(QWidget* parent = NULL);
        
        virtual ~MapSettingsModulateWidget();
        
        MapSettingsModulateWidget(const MapSettingsModulateWidget&) = delete;

        MapSettingsModulateWidget& operator=(const MapSettingsModulateWidget&) = delete;
        
        void updateEditor(CaretMappableDataFile* caretMappableDataFile,
                          const int32_t mapIndex);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void modulateEnabledCheckBoxClicked(bool status);
        
        void modulateMapFileIndexSelectorChanged();
        
    private:
        DataFileColorModulateSelector* getSelector();
        
        QCheckBox* m_enabledCheckBox;
        
        CaretMappableDataFileAndMapSelectorObject* m_modulateMapFileIndexSelector;
        
        CaretMappableDataFile* m_caretMappableDataFile = NULL;
        
        int32_t m_mapIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_MODULATE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_MODULATE_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_MODULATE_WIDGET_H__
