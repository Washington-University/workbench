#ifndef __TILE_TABS_CONFIGURATION_DIALOG_H__
#define __TILE_TABS_CONFIGURATION_DIALOG_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
