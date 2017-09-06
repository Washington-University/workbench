#ifndef __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_H__
#define __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_H__

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

#include "CaretColorEnum.h"
#include "WuQDialogNonModal.h"

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

    class ChartTwoLineSeriesHistoryDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        ChartTwoLineSeriesHistoryDialog(QWidget* parent);
        
        virtual ~ChartTwoLineSeriesHistoryDialog();
        
        void updateDialogContent(ChartTwoOverlay* chartOverlay);
        
        virtual void updateDialog() override;
        
        void updateIfThisChartOverlayIsInDialog(const ChartTwoOverlay* chartOverlay);

        // ADD_NEW_METHODS_HERE

    private slots:
        void tableWidgetCellChanged(int rowIndex, int columnIndex);
        
        void removeHistoryItemSelected(int rowIndex);
        
        void colorItemSelected(int rowIndex);
        
        void lineWidthItemSelected(int rowIndex);
        
        void removeAllHistoryButtonClicked();
        
        void defaultColorSelected(const CaretColorEnum::Enum color);
        
        void defaultLineWidthChanged(double);
        
        void viewedMaximumSpinBoxValueChanged(int);
        
    private:
        
        ChartTwoLineSeriesHistoryDialog(const ChartTwoLineSeriesHistoryDialog&);

        ChartTwoLineSeriesHistoryDialog& operator=(const ChartTwoLineSeriesHistoryDialog&);
        
        void updateDialogContentPrivate();
        
        void loadHistoryIntoTableWidget(ChartTwoLineSeriesHistory* lineSeriesHistory);
        
        ChartTwoOverlay* getChartOverlay();
        
        ChartTwoLineSeriesHistory* getLineSeriesHistory();
        
        CaretMappableDataFile* getMapFile();
        
        std::weak_ptr<ChartTwoOverlay> m_chartOverlayWeakPointer;
        
        QTableWidget* m_tableWidget;
        
        QIcon* m_removeAllHistoryIcon;
        
        QSignalMapper* m_removeHistoryItemSignalMapper;
        
        QSignalMapper* m_colorItemSignalMapper;
        
        QSignalMapper* m_lineWidthItemSignalMapper;
        
        std::vector<CaretColorEnumComboBox*> m_colorComboBoxes;
        
        std::vector<WuQDoubleSpinBox*> m_lineWidthSpinBoxes;
        
        QLabel* m_filenameLabel;
        
        CaretColorEnumComboBox* m_defaultColorComboBox;
        
        WuQDoubleSpinBox* m_defaultLineWidthSpinBox;
        
        QSpinBox* m_viewedMaximumCountSpinBox;
        
        static const int32_t COLUMN_VIEW   = 0;
        static const int32_t COLUMN_REMOVE = 1;
        static const int32_t COLUMN_COLOR  = 2;
        static const int32_t COLUMN_LINE_WIDTH = 3;
        static const int32_t COLUMN_DESCRIPTION = 4;
        static const int32_t COLUMN_COUNT  = 5;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_DECLARE__

} // namespace
#endif  //__CHART_TWO_LINE_SERIES_HISTORY_DIALOG_H__
