#ifndef __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_H__
#define __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "CaretColorEnum.h"

class QLabel;
class QIcon;
class QSignalMapper;
class QSpinBox;
class QTableWidget;

namespace caret {

    class CaretColorEnumComboBox;
    class CaretMappableDataFile;
    class ChartTwoLineSeriesHistory;
    class ChartTwoOverlay;
    class WuQDoubleSpinBox;

    class MapSettingsChartTwoLineHistoryWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsChartTwoLineHistoryWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsChartTwoLineHistoryWidget();
        
        void updateContent(ChartTwoOverlay* chartOverlay);
                
        void updateIfThisChartOverlayIsInDialog(const ChartTwoOverlay* chartOverlay);

        // ADD_NEW_METHODS_HERE

    private slots:
        void tableWidgetCellChanged(int rowIndex, int columnIndex);
        
        void removeHistoryItemSelected(int rowIndex);
        
        void moveUpHistoryItemSelected(int rowIndex);
        
        void moveDownHistoryItemSelected(int rowIndex);
        
        void colorItemSelected(int rowIndex);
        
        void lineWidthItemSelected(int rowIndex);
        
        void removeAllHistoryButtonClicked();
        
        void defaultColorSelected(const CaretColorEnum::Enum color);
        
        void defaultLineWidthChanged(double);
        
        void viewedMaximumSpinBoxValueChanged(int);
        
    private:
        enum class IconType {
            ARROW_DOWN,
            ARROW_UP
        };
        
        MapSettingsChartTwoLineHistoryWidget(const MapSettingsChartTwoLineHistoryWidget&);

        MapSettingsChartTwoLineHistoryWidget& operator=(const MapSettingsChartTwoLineHistoryWidget&);
        
        void updateDialogContentPrivate();
        
        void loadHistoryIntoTableWidget(ChartTwoLineSeriesHistory* lineSeriesHistory);
        
        ChartTwoOverlay* getChartOverlay();
        
        ChartTwoLineSeriesHistory* getLineSeriesHistory();
        
        CaretMappableDataFile* getMapFile();
        
        QPixmap createIcon(const QWidget* widget,
                           const IconType iconType) const;
        
        std::weak_ptr<ChartTwoOverlay> m_chartOverlayWeakPointer;
        
        QTableWidget* m_tableWidget;
        
        QIcon* m_removeAllHistoryIcon;
        
        QIcon* m_downArrowIcon;
        
        QIcon* m_upArrowIcon;
        
        QSignalMapper* m_removeHistoryItemSignalMapper;
        
        QSignalMapper* m_colorItemSignalMapper;
        
        QSignalMapper* m_lineWidthItemSignalMapper;
        
        std::vector<CaretColorEnumComboBox*> m_colorComboBoxes;
        
        std::vector<WuQDoubleSpinBox*> m_lineWidthSpinBoxes;
        
        CaretColorEnumComboBox* m_defaultColorComboBox;
        
        WuQDoubleSpinBox* m_defaultLineWidthSpinBox;
        
        QSpinBox* m_viewedMaximumCountSpinBox;
        
        static const int32_t COLUMN_VIEW   = 0;
        static const int32_t COLUMN_REMOVE = 1;
        static const int32_t COLUMN_MOVE   = 2;
        static const int32_t COLUMN_COLOR  = 3;
        static const int32_t COLUMN_LINE_WIDTH = 4;
        static const int32_t COLUMN_DESCRIPTION = 5;
        static const int32_t COLUMN_COUNT  = 6;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_H__
