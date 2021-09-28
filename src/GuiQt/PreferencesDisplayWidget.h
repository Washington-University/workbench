#ifndef __PREFERENCES_DISPLAY_WIDGET_H__
#define __PREFERENCES_DISPLAY_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

class QComboBox;

namespace caret {
    class CaretPreferences;
    class EnumComboBoxTemplate;

    class PreferencesDisplayWidget : public QWidget {
        
        Q_OBJECT

    public:
        PreferencesDisplayWidget(QWidget* parent = 0);
        
        virtual ~PreferencesDisplayWidget();
        
        PreferencesDisplayWidget(const PreferencesDisplayWidget&) = delete;

        PreferencesDisplayWidget& operator=(const PreferencesDisplayWidget&) = delete;
        
        void updateContent(CaretPreferences* preferences);

        // ADD_NEW_METHODS_HERE

    signals:
        void updateDialog();
        
        
    private slots:
        void htmlLinkClicked(const QString& htmlLink);
        
        void displayHighDpiModeEnumComboBoxItemActivated();
        
    private:
        
        QWidget* getDisplayInfoTextWidget() const;
        
        /* DO NOT delete */
        CaretPreferences* m_preferences = NULL;
        
        EnumComboBoxTemplate* m_displayHighDpiModeEnumComboBox;
        
        QComboBox* m_dimensionComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PREFERENCES_DISPLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PREFERENCES_DISPLAY_DECLARE__

} // namespace
#endif  //__PREFERENCES_DISPLAY_WIDGET_H__
