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


#include "WuQDialogNonModal.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QSpinBox;

namespace caret {
    class BrainBrowserWindow;
    class CaretPreferences;
    class TileTabsConfiguration;
    
    class TileTabsConfigurationDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        TileTabsConfigurationDialog(QWidget* parent);
        
        virtual ~TileTabsConfigurationDialog();
        
        void updateDialogWithSelectedTileTabsFromWindow(BrainBrowserWindow* brainBrowserWindow);
        
        void updateDialog();
        
    private:
        TileTabsConfigurationDialog(const TileTabsConfigurationDialog&);

        TileTabsConfigurationDialog& operator=(const TileTabsConfigurationDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private slots:
        void newConfigurationButtonClicked();
        
        void deleteConfigurationButtonClicked();
        
        void renameConfigurationButtonClicked();
        
        void configurationComboBoxItemSelected(int);
        
        void numberOfRowsOrColumnsChanged();
        
        void configurationStretchFactorWasChanged();
        
    protected:
        void focusGained();
        
    private:
        // ADD_NEW_MEMBERS_HERE
        
        enum {
            GRID_LAYOUT_COLUMN_INDEX_FOR_LABELS = 0,
            GRID_LAYOUT_COLUMN_INDEX_FOR_ROW_CONTROLS = 1,
            GRID_LAYOUT_COLUMN_INDEX_FOR_COLUMN_CONTROLS = 2
        };
        
        void selectTileTabConfigurationByUniqueID(const AString& uniqueID);
        
        AString getSelectedTileTabsConfigurationUniqueID();
        
        TileTabsConfiguration* getSelectedTileTabsConfiguration();
        
        QWidget* createConfigurationSelectionWidget();
        
        QWidget* createEditConfigurationWidget();
        
        void updateBrowserWindowsTileTabsConfigurationSelection();
        
        void updateStretchFactors();
        
        void updateGraphicsWindows();
        
        void selectConfigurationFromComboBoxIndex(int indx);
        
        void readConfigurationsFromPreferences();
        
        QPushButton* m_newConfigurationPushButton;
        
        QPushButton* m_deleteConfigurationPushButton;
        
        QPushButton* m_renameConfigurationPushButton;
        
        QComboBox* m_configurationSelectionComboBox;
        
        QLineEdit* m_nameLineEdit;
        
        QSpinBox* m_numberOfRowsSpinBox;
        
        QSpinBox* m_numberOfColumnsSpinBox;
        
        QScrollArea* m_stretchFactorScrollArea;
        QWidget* m_stretchFactorWidget;
        
        std::vector<QLabel*> m_stretchFactorIndexLabels;
        
        std::vector<QDoubleSpinBox*> m_rowStretchFactorSpinBoxes;
        
        std::vector<QDoubleSpinBox*> m_columnStretchFactorSpinBoxes;
        
        /** Blocks reading of preferences since that may invalidate data pointers */
        bool m_blockReadConfigurationsFromPreferences;
        
        /** browser window from which this dialog was last displayed */
        BrainBrowserWindow* m_brainBrowserWindow;
        
        /** 
         * Keep a pointer to preferences but DO NOT delete it
         * since the preferences are managed by the session
         * manager.
         */
        CaretPreferences* m_caretPreferences;
    };
    
#ifdef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_DIALOG_H__
