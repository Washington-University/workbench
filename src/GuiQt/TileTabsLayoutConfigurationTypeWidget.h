#ifndef __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_H__
#define __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <QWidget>

#include "TileTabsLayoutConfigurationTypeEnum.h"

class QButtonGroup;
class QLabel;
class QSpinBox;
class QRadioButton;
class QToolButton;

namespace caret {

    class BrainBrowserWindow;
    class BrowserWindowContent;
    class TileTabsLayoutGridConfiguration;
    
    class TileTabsLayoutConfigurationTypeWidget : public QWidget {
        
        Q_OBJECT

    public:
        enum class ParentType {
            BROWSER_WINDOW_TOOLBAR,
            TILE_TABS_DIALOG
        };
        
        TileTabsLayoutConfigurationTypeWidget(const ParentType parentType,
                                              QWidget* parent = 0);
        
        virtual ~TileTabsLayoutConfigurationTypeWidget();
        
        TileTabsLayoutConfigurationTypeWidget(const TileTabsLayoutConfigurationTypeWidget&) = delete;

        TileTabsLayoutConfigurationTypeWidget& operator=(const TileTabsLayoutConfigurationTypeWidget&) = delete;

        void updateContent(const int32_t windowIndex);

        // ADD_NEW_METHODS_HERE

    private slots:
        void layoutTypeRadioButtonClicked(const TileTabsLayoutConfigurationTypeEnum::Enum layoutType);
        
        void customGridColumnSpinBoxValueChanged(const int columns);
        
        void customGridRowSpinBoxValueChanged(const int rows);
        
        void manualConfigurationSetToolButtonClicked();
        
        void manualConfigurationSetMenuColumnsItemTriggered();
        
        void manualConfigurationSetMenuFromAutomaticItemTriggered();
        
        void manualConfigurationSetMenuFromCustomItemTriggered();

    private:
        TileTabsLayoutGridConfiguration* getCustomTileTabsGridConfiguration();

        BrainBrowserWindow* getBrowserWindow();
        
        BrowserWindowContent* getBrowserWindowContent();
        
        void updateGraphicsAndUserInterface();
        
        void customGridRowsColumnsChanged(const int32_t rows,
                                          const int32_t columns);
        
        AString getRowsColumnsLabelText(const int32_t numRows,
                                        const int32_t numCols) const;

        QToolButton* createManualConfigurationSetToolButton();
        
        // ADD_NEW_MEMBERS_HERE

        const ParentType m_parentType;
        
        int32_t m_windowIndex = -1;
        
        QButtonGroup* m_buttonGroup;
        
        std::vector<QRadioButton*> m_layoutTypeRadioButtons;
        
        QLabel* m_automaticGridRowsColumnsLabel = NULL;
        
        QLabel* m_customGridRowsColumnsLabel = NULL;
        
        QSpinBox* m_customGridColumnsSpinBox = NULL;
        
        QSpinBox* m_customGridRowsSpinBox = NULL;
        
        QToolButton* m_manualConfigurationSetButton;
        
        QString m_setManualToAutomaticGridActionText;
        QString m_setManualToCustomGridActionText;
        QString m_setManualToGridColumnsActionText;
    };
    
#ifdef __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_DECLARE__

} // namespace
#endif  //__TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_H__
