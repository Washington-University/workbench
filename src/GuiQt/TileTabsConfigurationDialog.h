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
class QStackedWidget;
class QTabWidget;
class QToolButton;

namespace caret {
    class BrainBrowserWindow;
    class BrainBrowserWindowComboBox;
    class BrainOpenGLViewportContent;
    class BrowserWindowContent;
    class CaretPreferences;
    class EnumComboBoxTemplate;
    class TileTabsLayoutBaseConfiguration;
    class TileTabsLayoutGridConfiguration;
    class TileTabsLayoutManualConfiguration;
    class TileTabGridRowColumnWidgets;
    class TileTabsGridRowColumnElement;
    class TileTabsManualTabGeometryWidget;
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
        
        void showConfigurationXmlPushButtonClicked();
        
        void gridConfigurationNumberOfRowsOrColumnsChanged();
        
        void gridConfigurationStretchFactorWasChanged();
        
        void addUserConfigurationPushButtonClicked();
        
        void replaceUserConfigurationPushButtonClicked();
        
        void loadIntoActiveConfigurationPushButtonClicked();

        void automaticCustomButtonClicked(QAbstractButton*);
        
        void tileTabsModificationRequested(EventTileTabsGridConfigurationModification& modification);

        void centeringCorrectionCheckBoxClicked(bool checked);
        
        void manualConfigurationGeometryChanged();

        void manualConfigurationSetToolButtonClicked();
        
        void manualConfigurationSetMenuColumnsItemTriggered();
        
        void manualConfigurationSetMenuFromAutomaticItemTriggered();
        
        void manualConfigurationSetMenuFromCustomItemTriggered();
        
        void userConfigurationSelectionListWidgetItemChanged();
        
        void templateConfigurationSelectionListWidgetItemChanged();
        
        void configurationSourceTabWidgetClicked(int index);
        
    protected:
        void focusGained();
        
        virtual void helpButtonClicked() override;
        
    private:
        enum ConfigurationSourceTypeEnum {
            TEMPLATE,
            USER
        };
        
        // ADD_NEW_MEMBERS_HERE
        
        QWidget* createCopyLoadPushButtonsWidget();
        
        QWidget* createWorkbenchWindowWidget();
        
        TileTabsLayoutGridConfiguration* getCustomTileTabsGridConfiguration();
        
        AString getSelectedUserTileTabsConfigurationUniqueIdentifier() const;
        
        QWidget* createUserConfigurationSelectionWidget();
        
        QWidget* createConfigurationTypeWidget();
        
        QWidget* createConfigurationSettingsWidget();
        
        QWidget* createGridRowColumnStretchWidget();
        
        QWidget* createGridCustomOptionsWidget();
        
        QWidget* createManualGeometryEditingWidget();
        
        void updateRowColumnStretchWidgets(TileTabsLayoutGridConfiguration* configuration);
        
        void addRowColumnStretchWidget(const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                                       QGridLayout* gridLayout,
                                       std::vector<TileTabGridRowColumnWidgets*>& elementVector);
        
        void updateConfigurationEditingWidget();
        
        void updateManualGeometryEditorWidget();
        
        void addManualGeometryWidget(QGridLayout* gridLayout,
                                     std::vector<TileTabsManualTabGeometryWidget*>& widgetsVector);
        
        QToolButton* createManualConfigurationSetToolButton();
        
        void updateGridStretchFactors();
        
        void updateGraphicsWindow();
        
        void updateCustomOptionsWidget();
        
        void readUserConfigurationsFromPreferences();
        
        void updateUserConfigurationListWidget();
        
        void updateTemplateConfigurationListWidget();
        
        void manualConfigurationSetMenuFromGridConfiguration(TileTabsLayoutGridConfiguration* gridConfiguration);
        
        BrainBrowserWindow* getBrowserWindow();
        
        const BrainBrowserWindow* getBrowserWindow() const;
        
        BrowserWindowContent* getBrowserWindowContent();
        
        AString getNewConfigurationName(QWidget* dialogParent);
        
        const BrainOpenGLViewportContent* getViewportContentForTab(const int32_t tabIndex) const;

        TileTabsLayoutManualConfiguration* createManualConfigurationFromCurrentTabs() const;
        
        std::unique_ptr<TileTabsLayoutBaseConfiguration> getSelectedUserConfiguration() const;
        
        std::unique_ptr<TileTabsLayoutBaseConfiguration> getSelectedTemplateConfiguration() const;
        
        ConfigurationSourceTypeEnum getSelectedConfigurationSourceType() const;
        
        void loadConfigurationPreviewLabel(TileTabsLayoutBaseConfiguration* configuration);
        
        void updatePushButtons(const ConfigurationSourceTypeEnum sourceType);
        
        void loadTemplateLayoutConfigurations();
        
        void loadTemplateLayoutConfigurationFromXML(const QString& xml);
        
        bool warnIfGridConfigurationTooSmallDialog(const TileTabsLayoutGridConfiguration* gridConfiguration) const;
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        QStackedWidget* m_editConfigurationStackedWidget;
        
        QWidget* m_customGridConfigurationWidget;
        
        QRadioButton* m_automaticGridConfigurationRadioButton;
        
        QRadioButton* m_customGridConfigurationRadioButton;
        
        QRadioButton* m_manualConfigurationRadioButton;
        
        QPushButton* m_deleteConfigurationPushButton;
        
        QPushButton* m_renameConfigurationPushButton;
        
        QPushButton* m_showConfigurationXmlPushButton;
        
        QPushButton* m_addConfigurationPushButton;
        
        QPushButton* m_replaceConfigurationPushButton;
        
        QPushButton* m_loadConfigurationPushButton;
        
        QTabWidget* m_configurationSourceTabWidget;
        
        int32_t m_configurationSourceTemplateTabIndex;
        
        int32_t m_configurationSourceUserTabIndex;
        
        WuQListWidget* m_userConfigurationSelectionListWidget;
        
        WuQListWidget* m_templateConfigurationSelectionListWidget;
        
        QSpinBox* m_numberOfGridRowsSpinBox;
        
        QSpinBox* m_numberOfGridColumnsSpinBox;
        
        std::vector<TileTabGridRowColumnWidgets*> m_gridColumnElements;
        
        std::vector<TileTabGridRowColumnWidgets*> m_gridRowElements;
        
        QGridLayout* m_gridRowElementsGridLayout = NULL;
        
        QGridLayout* m_gridColumnElementsGridLayout = NULL;
        
        QCheckBox* m_gridCenteringCorrectionCheckBox;
        
        QWidget* m_manualGeometryWidget;
        
        QGridLayout* m_manualGeometryGridLayout;
        
        QToolButton* m_manualConfigurationSetButton;
        
        std::vector<TileTabsManualTabGeometryWidget*> m_manualGeometryEditorWidgets;
        
        std::vector<std::unique_ptr<TileTabsLayoutBaseConfiguration>> m_templateLayoutConfigurations;
        
        /** Blocks reading of preferences since that may invalidate data pointers */
        bool m_blockReadUserConfigurationsFromPreferences;
        
        /**
         * Keep a pointer to preferences but DO NOT delete it
         * since the preferences are managed by the session
         * manager.
         */
        CaretPreferences* m_caretPreferences;
        
        QLabel* m_configurationPreviewLabel;
        
        friend class TileTabGridRowColumnWidgets;
        
        static const int32_t s_maximumRowsColumns = 50;
    };
    
    
#ifdef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
//    const AString TileTabsConfigurationDialog::s_automaticConfigurationPrefix = "Automatic Configuration";
#endif // __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_DIALOG_H__
