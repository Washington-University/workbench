#ifndef __MAP_SETTINGS_LABELS_WIDGET_H__
#define __MAP_SETTINGS_LABELS_WIDGET_H__

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

class QCheckBox;

namespace caret {

    class CaretColorEnumComboBox;
    class EnumComboBoxTemplate;
    class Overlay;
    
    class MapSettingsLabelsWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsLabelsWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsLabelsWidget();
        
        void updateContent(Overlay* overlay);
        
    private slots:
        void applySelections();
        
        void editLabelTablePushButtonClicked();
        
        // ADD_NEW_METHODS_HERE

    private:
        MapSettingsLabelsWidget(const MapSettingsLabelsWidget&);

        MapSettingsLabelsWidget& operator=(const MapSettingsLabelsWidget&);
        
        // ADD_NEW_MEMBERS_HERE
        
        Overlay* m_overlay;
        
        CaretColorEnumComboBox* m_outlineColorComboBox;
        
        EnumComboBoxTemplate* m_drawingTypeComboBox;
        
        QCheckBox* m_drawMedialWallFilledCheckBox;
    };
    
#ifdef __MAP_SETTINGS_LABELS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_LABELS_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_LABELS_WIDGET_H__
