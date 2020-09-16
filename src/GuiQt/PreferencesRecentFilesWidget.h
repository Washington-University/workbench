#ifndef __PREFERENCES_RECENT_FILES_WIDGET_H__
#define __PREFERENCES_RECENT_FILES_WIDGET_H__

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



#include <memory>

#include <QListWidget>
#include <QSpinBox>
#include <QWidget>



namespace caret {
    class CaretPreferences;
    class EnumComboBoxTemplate;
    class WuQTrueFalseComboBox;

    class PreferencesRecentFilesWidget : public QWidget {
        
        Q_OBJECT

    public:
        PreferencesRecentFilesWidget(QWidget* parent = 0);
        
        virtual ~PreferencesRecentFilesWidget();
        
        PreferencesRecentFilesWidget(const PreferencesRecentFilesWidget&) = delete;

        PreferencesRecentFilesWidget& operator=(const PreferencesRecentFilesWidget&) = delete;
        
        void updateContent(CaretPreferences* preferences);

        // ADD_NEW_METHODS_HERE

    signals:
        void updateDialog();
        
    private slots:
        void recentFilesSystemAccessModeEnumComboBoxItemActivated();
        
        void numberOfRecentSceneAndSpecFilesSpinBoxValueChanged(int);
        
        void numberOfRecentDirectoriesSpinBoxValueChanged(int);
        
        void clearRecentSceneAndSpecFilesButtonClicked();
        
        void clearRecentDirectoriesButtonClicked();
        
        void addExclusionPathButtonClicked();
        
        void removeExclusionPathButtonClicked();
        
        void showOpenRecentFilesDialogAtStartupComboBoxActivated(bool status);
        
    private:
        void getMaximumSpinBoxAndClearButtonToolTips(const QString& typeName,
                                                     QString& spinBoxToolTipOut,
                                                     QString& clearButtonToolTipOut) const;
        
        /* DO NOT delete */
        CaretPreferences* m_preferences = NULL;
        
        EnumComboBoxTemplate* m_recentFilesSystemAccessModeEnumComboBox;
        
        QSpinBox* m_numberOfRecentSceneAndSpecFilesSpinBox;
        
        QSpinBox* m_numberOfRecentDirectoriesSpinBox;
        
        QListWidget* m_exclusionPathsListWidget;
        
        WuQTrueFalseComboBox* m_showOpenRecentFilesDialogAtStartupComboBox;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PREFERENCES_RECENT_FILES_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PREFERENCES_RECENT_FILES_WIDGET_DECLARE__

} // namespace
#endif  //__PREFERENCES_RECENT_FILES_WIDGET_H__
