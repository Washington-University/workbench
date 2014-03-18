#ifndef __MAP_SETTINGS_PARCELS_WIDGET_H__
#define __MAP_SETTINGS_PARCELS_WIDGET_H__

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


#include <QWidget>

#include "CaretColorEnum.h"


namespace caret {

    class CaretColorEnumComboBox;
    class CiftiMappableConnectivityMatrixDataFile;
    class EnumComboBoxTemplate;
    
    class MapSettingsParcelsWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsParcelsWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsParcelsWidget();
        
        void updateEditor(CiftiMappableConnectivityMatrixDataFile* ciftiMatrixFile);
        
        void updateWidget();
        
    private slots:
        void ciftiParcelColoringModeEnumComboBoxItemActivated();

        void parcelColorSelected(const CaretColorEnum::Enum);
        
        // ADD_NEW_METHODS_HERE

    private:
        MapSettingsParcelsWidget(const MapSettingsParcelsWidget&);

        MapSettingsParcelsWidget& operator=(const MapSettingsParcelsWidget&);
        
        EnumComboBoxTemplate* m_parcelColoringModeEnumComboBox;
        
        CaretColorEnumComboBox* m_parcelColorEnumComboBox;
        
        CiftiMappableConnectivityMatrixDataFile* m_ciftiMatrixFile;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_PARCELS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_PARCELS_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_PARCELS_WIDGET_H__
