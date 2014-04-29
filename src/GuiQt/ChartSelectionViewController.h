#ifndef __CHART_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_SELECTION_VIEW_CONTROLLER_H__

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

#include <stdint.h>

#include <QWidget>

#include "EventListenerInterface.h"

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSignalMapper;
class QStackedWidget;

namespace caret {

    class Brain;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class ChartableBrainordinateInterface;
    class ChartMatrixDisplayProperties;
    class ChartableMatrixInterface;
    class ChartModel;
    class EnumComboBoxTemplate;
    class ModelChart;
    
    class ChartSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartSelectionViewController();
        
    private slots:
        void brainordinateSelectionCheckBoxClicked(int);
        
        void matrixFileSelected(CaretDataFile* caretDataFile);
        
        void matrixFileLoadingButtonClicked();

        void matrixYokingGroupEnumComboBoxActivated();
        
    private:
        ChartSelectionViewController(const ChartSelectionViewController&);

        ChartSelectionViewController& operator=(const ChartSelectionViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        enum Mode {
            MODE_INVALID,
            MODE_BRAINORDINATE,
            MODE_MATRIX
        };
        
        // ADD_NEW_MEMBERS_HERE

        QWidget* createBrainordinateChartWidget();
        
        QWidget* createMatrixChartWidget();
        
        void updateSelectionViewController();
        
        void updateBrainordinateChartWidget(Brain* brain,
                                            ModelChart* modelChart,
                                            const int32_t browserTabIndex);
        
        void updateMatrixChartWidget(Brain* brain,
                                     ModelChart* modelChart,
                                     const int32_t browserTabIndex);
        
        ChartableBrainordinateInterface* getBrainordinateFileAtIndex(const int32_t indx);
        
//        ChartableMatrixInterface* getMatrixFileAtIndex(const int32_t indx);
        
//        ChartModel* getSelectedChartModel();
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties();
        
        QStackedWidget* m_stackedWidget;
        
        QWidget* m_brainordinateChartWidget;
        
        QWidget* m_matrixChartWidget;
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<QCheckBox*> m_brainordinateFileEnableCheckBoxes;
        
        std::vector<QLineEdit*> m_brainordinateFileNameLineEdits;
        
        QGridLayout* m_brainordinateGridLayout;
        
        QSignalMapper* m_signalMapperBrainordinateFileEnableCheckBox;
        
        CaretDataFileSelectionComboBox* m_matrixFileSelectionComboBox;
        
        QRadioButton* m_matrixLoadByColumnRadioButton;
        
        QRadioButton* m_matrixLoadByRowRadioButton;
                
        EnumComboBoxTemplate* m_matrixYokingGroupComboBox;

        static const int COLUMN_CHECKBOX;
        static const int COLUMN_LINE_EDIT;
        
    };
    
#ifdef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
    const int ChartSelectionViewController::COLUMN_CHECKBOX    = 0;
    const int ChartSelectionViewController::COLUMN_LINE_EDIT   = 1;
#endif // __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_SELECTION_VIEW_CONTROLLER_H__
