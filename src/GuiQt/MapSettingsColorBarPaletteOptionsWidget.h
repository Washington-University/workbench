#ifndef __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_H__
#define __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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


#include <QGroupBox>

class QAction;
class QCheckBox;

namespace caret {

    class CaretMappableDataFile;
    
    class MapSettingsColorBarPaletteOptionsWidget : public QGroupBox {
        
        Q_OBJECT

    public:
        MapSettingsColorBarPaletteOptionsWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsColorBarPaletteOptionsWidget();
        
        void updateEditor(CaretMappableDataFile* caretMappableDataFile,
                          const int32_t mapIndex);

        void applyOptions();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void applyToAllMapsCheckBoxClicked(bool checked);
    
        void applyToAllFilesToolButtonClicked();
        
    private:
        MapSettingsColorBarPaletteOptionsWidget(const MapSettingsColorBarPaletteOptionsWidget&);

        MapSettingsColorBarPaletteOptionsWidget& operator=(const MapSettingsColorBarPaletteOptionsWidget&);
        
        void updateColoringAndGraphics();
        
        QCheckBox* m_applyToAllMapsCheckBox;
        
        QAction* m_applyToAllFilesAction;
        
        CaretMappableDataFile* m_mapFile = NULL;
        
        int32_t m_mapFileIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_H__
