#ifndef __CONNECTIVITY_CORRELATION_SETTINGS_MENU_H__
#define __CONNECTIVITY_CORRELATION_SETTINGS_MENU_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include <QMenu>

class QAbstractButton;
class QCheckBox;
class QRadioButton;

namespace caret {
    class ConnectivityCorrelationSettings;
    
    class ConnectivityCorrelationSettingsMenu : public QMenu {
        
        Q_OBJECT

    public:
        ConnectivityCorrelationSettingsMenu(ConnectivityCorrelationSettings* settings,
                                            QWidget* parent);
        
        virtual ~ConnectivityCorrelationSettingsMenu();
        
        ConnectivityCorrelationSettingsMenu(const ConnectivityCorrelationSettingsMenu&) = delete;

        ConnectivityCorrelationSettingsMenu& operator=(const ConnectivityCorrelationSettingsMenu&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void modeButtonClicked(QAbstractButton *button);
        
        void optionFisherZCheckBoxClicked(bool checked);
        
        void optionNoDemeanCheckBoxClicked(bool checked);
        
    private:
        void updateMenu();
        
        ConnectivityCorrelationSettings* m_settings;
        
        QRadioButton* m_modeCorrelationRadioButton;
        
        QRadioButton* m_modeCovarianceRadioButton;
        
        QCheckBox* m_optionFisherZCheckBox;
        
        QCheckBox* m_optionNoDemeanCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CONNECTIVITY_CORRELATION_SETTINGS_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_CORRELATION_SETTINGS_MENU_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_CORRELATION_SETTINGS_MENU_H__
