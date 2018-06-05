#ifndef __TILE_TABS_CONFIGURATION_DIALOG_H__
#define __TILE_TABS_CONFIGURATION_DIALOG_H__

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

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QSpinBox;

namespace caret {
    class BrainBrowserWindow;
    class BrainBrowserWindowComboBox;
    class BrowserWindowContent;
    class CaretPreferences;
    class TileTabsConfiguration;
    class WuQListWidget;
    
    class TileTabsConfigurationDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        TileTabsConfigurationDialog(BrainBrowserWindow* parentBrainBrowserWindow);
        
        virtual ~TileTabsConfigurationDialog();
        
        void updateDialogWithSelectedTileTabsFromWindow(BrainBrowserWindow* brainBrowserWindow);
        
        void updateDialog();
        
        virtual void receiveEvent(Event* event) override;
        
    private:
        TileTabsConfigurationDialog(const TileTabsConfigurationDialog&);

        TileTabsConfigurationDialog& operator=(const TileTabsConfigurationDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private slots:
        void browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow);
        
        void newUserConfigurationButtonClicked();
        
        void deleteUserConfigurationButtonClicked();
        
        void renameUserConfigurationButtonClicked();
        
        void configurationNumberOfRowsOrColumnsChanged();
        
        void configurationStretchFactorWasChanged();
        
        void replaceUserConfigurationPushButtonClicked();
        
        void loadIntoActiveConfigurationPushButtonClicked();

        void automaticCustomButtonClicked(QAbstractButton*);
        
    protected:
        void focusGained();
        
    private:
        // ADD_NEW_MEMBERS_HERE
        
        QWidget* createCopyLoadPushButtonsWidget();
        
        QWidget* createWorkbenchWindowWidget();
        
        void selectTileTabConfigurationByUniqueID(const AString& uniqueID);
        
        TileTabsConfiguration* getAutomaticTileTabsConfiguration();
        
        TileTabsConfiguration* getCustomTileTabsConfiguration();
        
        TileTabsConfiguration* getSelectedUserTileTabsConfiguration();
        
        QWidget* createUserConfigurationSelectionWidget();
        
        QWidget* createActiveConfigurationWidget();
        
        QWidget* createCustomConfigurationWidget();
        
        void updateStretchFactors();
        
        void updateGraphicsWindow();
        
        void readConfigurationsFromPreferences();
        
        BrainBrowserWindow* getBrowserWindow();
        
        BrowserWindowContent* getBrowserWindowContent();
        
        void updatePercentageLabels(const std::vector<QDoubleSpinBox*>& factorSpinBoxes,
                                    std::vector<QLabel*>& percentageLabels,
                                    const int32_t validCount);
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        QWidget* m_customConfigurationWidget;
        
        QRadioButton* m_automaticConfigurationRadioButton;
        
        QRadioButton* m_customConfigurationRadioButton;
        
        QPushButton* m_newConfigurationPushButton;
        
        QPushButton* m_deleteConfigurationPushButton;
        
        QPushButton* m_renameConfigurationPushButton;
        
        QPushButton* m_replacePushButton;
        
        QPushButton* m_loadPushButton;
        
        WuQListWidget* m_userConfigurationSelectionListWidget;
        
        QSpinBox* m_numberOfRowsSpinBox;
        
        QSpinBox* m_numberOfColumnsSpinBox;
        
        std::vector<QLabel*> m_rowStretchFactorIndexLabels;
        
        std::vector<QDoubleSpinBox*> m_rowStretchFactorSpinBoxes;
        
        std::vector<QLabel*> m_rowStretchPercentageLabels;
        
        std::vector<QLabel*> m_columnStretchFactorIndexLabels;
        
        std::vector<QDoubleSpinBox*> m_columnStretchFactorSpinBoxes;
        
        std::vector<QLabel*> m_columnStretchPercentageLabels;
        
        /** Blocks reading of preferences since that may invalidate data pointers */
        bool m_blockReadConfigurationsFromPreferences;
        
        /**
         * Keep a pointer to preferences but DO NOT delete it
         * since the preferences are managed by the session
         * manager.
         */
        CaretPreferences* m_caretPreferences;
    };
    
#ifdef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
//    const AString TileTabsConfigurationDialog::s_automaticConfigurationPrefix = "Automatic Configuration";
#endif // __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_DIALOG_H__
