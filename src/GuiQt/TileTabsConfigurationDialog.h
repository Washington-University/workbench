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
#include "EventTileTabsConfigurationModification.h"
#include "TileTabsRowColumnContentTypeEnum.h"
#include "TileTabsRowColumnStretchTypeEnum.h"
#include "WuQDialogNonModal.h"

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
    class TileTabsConfiguration;
    class TileTabElementWidgets;
    class TileTabsRowColumnElement;
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
        
        void newUserConfigurationButtonClicked();
        
        void deleteUserConfigurationButtonClicked();
        
        void renameUserConfigurationButtonClicked();
        
        void configurationNumberOfRowsOrColumnsChanged();
        
        void configurationStretchFactorWasChanged();
        
        void replaceUserConfigurationPushButtonClicked();
        
        void loadIntoActiveConfigurationPushButtonClicked();

        void automaticCustomButtonClicked(QAbstractButton*);
        
        void tileTabsModificationRequested(EventTileTabsConfigurationModification& modification);
        

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
        
        QWidget* createRowColumnStretchWidget();
        
        void updateRowColumnStretchWidgets(TileTabsConfiguration* configuration);
        
        void addRowColumnStretchWidget(const EventTileTabsConfigurationModification::RowColumnType rowColumnType,
                                       QGridLayout* gridLayout,
                                       std::vector<TileTabElementWidgets*>& elementVector);
        
        void updateStretchFactors();
        
        void updateGraphicsWindow();
        
        void readConfigurationsFromPreferences();
        
        BrainBrowserWindow* getBrowserWindow();
        
        BrowserWindowContent* getBrowserWindowContent();
        
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
        
        std::vector<TileTabElementWidgets*> m_columnElements;
        
        std::vector<TileTabElementWidgets*> m_rowElements;
        
        QGridLayout* m_rowElementsGridLayout = NULL;
        
        QGridLayout* m_columnElementsGridLayout = NULL;
        
        /** Blocks reading of preferences since that may invalidate data pointers */
        bool m_blockReadConfigurationsFromPreferences;
        
        /**
         * Keep a pointer to preferences but DO NOT delete it
         * since the preferences are managed by the session
         * manager.
         */
        CaretPreferences* m_caretPreferences;
        
        static const int32_t s_maximumRowsColumns = 50;
    };
    
    
    /**
     * Contains widgets for one row or column of stretching.
     */
    class TileTabElementWidgets : public QObject {
        Q_OBJECT
        
    public:
        TileTabElementWidgets(const EventTileTabsConfigurationModification::RowColumnType rowColumnType,
                              const int32_t index,
                              QGridLayout* gridLayout,
                              QObject* parent);
        
        virtual ~TileTabElementWidgets();

        void updateContent(TileTabsRowColumnElement* element);
        
    signals:
        void itemChanged();
        
        void modificationRequested(EventTileTabsConfigurationModification& modification);
        
    private slots:
        void constructionMenuAboutToShow();
        
        void constructionMenuTriggered(QAction*);
        
        void contentTypeActivated();
        
        void stretchTypeActivated();
        
        void stretchValueChanged(double);
        
    private:
        QMenu* createConstructionMenu(QToolButton* toolButton);
        
        const EventTileTabsConfigurationModification::RowColumnType m_rowColumnType;
        const int32_t m_index;
        TileTabsRowColumnElement* m_element;
        
        QLabel* m_indexLabel;
        QAction* m_constructionAction;
        QToolButton* m_constructionToolButton;
        EnumComboBoxTemplate* m_contentTypeComboBox;
        EnumComboBoxTemplate* m_stretchTypeComboBox;
        QDoubleSpinBox* m_stretchValueSpinBox;
        
        QAction* m_menuDeleteAction;
        QAction* m_menuDuplicateAfterAction;
        QAction* m_menuDuplicateBeforeAction;
        QAction* m_menuMoveAfterAction;
        QAction* m_menuMoveBeforeAction;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
    };
    
#ifdef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
//    const AString TileTabsConfigurationDialog::s_automaticConfigurationPrefix = "Automatic Configuration";
#endif // __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_DIALOG_H__
