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
#include "EventTileTabsGridConfigurationModification.h"
#include "TileTabsGridRowColumnContentTypeEnum.h"
#include "TileTabsGridRowColumnStretchTypeEnum.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QToolButton;

namespace caret {
    class BrainBrowserWindow;
    class BrainBrowserWindowComboBox;
    class BrowserWindowContent;
    class CaretPreferences;
    class EnumComboBoxTemplate;
    class TileTabsLayoutBaseConfiguration;
    class TileTabsLayoutGridConfiguration;
    class TileTabsLayoutManualConfiguration;
    class TileTabGridRowColumnWidgets;
    class TileTabsGridRowColumnElement;
    class WuQGridLayoutGroup;
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
        
        void deleteUserConfigurationButtonClicked();
        
        void renameUserConfigurationButtonClicked();
        
        void configurationNumberOfRowsOrColumnsChanged();
        
        void configurationStretchFactorWasChanged();
        
        void addUserConfigurationPushButtonClicked();
        
        void replaceUserConfigurationPushButtonClicked();
        
        void loadIntoActiveConfigurationPushButtonClicked();

        void automaticCustomButtonClicked(QAbstractButton*);
        
        void tileTabsModificationRequested(EventTileTabsGridConfigurationModification& modification);

        void centeringCorrectionCheckBoxClicked(bool checked);

    protected:
        void focusGained();
        
        virtual void helpButtonClicked() override;
        
    private:
        // ADD_NEW_MEMBERS_HERE
        
        QWidget* createCopyLoadPushButtonsWidget();
        
        QWidget* createWorkbenchWindowWidget();
        
        void selectTileTabConfigurationByUniqueID(const AString& uniqueID);
        
        TileTabsLayoutGridConfiguration* getCustomTileTabsGridConfiguration();
        
        AString getSelectedUserTileTabsConfigurationUniqueIdentifier();
        
        bool getSelectedUserConfigurationNameAndUniqueID(AString& nameOut,
                                                         AString& uniqueIDOut) const;
        
        QWidget* createUserConfigurationSelectionWidget();
        
        QWidget* createActiveConfigurationWidget();
        
        QWidget* createRowColumnStretchWidget();
        
        QWidget* createCustomOptionsWidget();
        
        void updateRowColumnStretchWidgets(TileTabsLayoutGridConfiguration* configuration);
        
        void addRowColumnStretchWidget(const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                                       QGridLayout* gridLayout,
                                       std::vector<TileTabGridRowColumnWidgets*>& elementVector);
        
        void updateStretchFactors();
        
        void updateGraphicsWindow();
        
        void updateCustomOptionsWidget();
        
        void readConfigurationsFromPreferences();
        
        BrainBrowserWindow* getBrowserWindow();
        
        BrowserWindowContent* getBrowserWindowContent();
        
        AString getNewConfigurationName(QWidget* dialogParent);
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        QWidget* m_customConfigurationWidget;
        
        QWidget* m_customOptionsWidget;
        
        QRadioButton* m_automaticConfigurationRadioButton;
        
        QRadioButton* m_customConfigurationRadioButton;
        
        QPushButton* m_deleteConfigurationPushButton;
        
        QPushButton* m_renameConfigurationPushButton;
        
        QPushButton* m_addPushButton;
        
        QPushButton* m_replacePushButton;
        
        QPushButton* m_loadPushButton;
        
        WuQListWidget* m_userConfigurationSelectionListWidget;
        
        QSpinBox* m_numberOfRowsSpinBox;
        
        QSpinBox* m_numberOfColumnsSpinBox;
        
        std::vector<TileTabGridRowColumnWidgets*> m_columnElements;
        
        std::vector<TileTabGridRowColumnWidgets*> m_rowElements;
        
        QGridLayout* m_rowElementsGridLayout = NULL;
        
        QGridLayout* m_columnElementsGridLayout = NULL;
        
        QCheckBox* m_centeringCorrectionCheckBox;
        
        /** Blocks reading of preferences since that may invalidate data pointers */
        bool m_blockReadConfigurationsFromPreferences;
        
        /**
         * Keep a pointer to preferences but DO NOT delete it
         * since the preferences are managed by the session
         * manager.
         */
        CaretPreferences* m_caretPreferences;
        
        friend class TileTabGridRowColumnWidgets;
        friend class TileTabGridRowColumnWidgets;
        
        static const int32_t s_maximumRowsColumns = 50;
    };
    
    
#ifdef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
//    const AString TileTabsConfigurationDialog::s_automaticConfigurationPrefix = "Automatic Configuration";
#endif // __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_DIALOG_H__
