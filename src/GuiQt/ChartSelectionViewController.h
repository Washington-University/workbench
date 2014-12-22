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
class QComboBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSignalMapper;
class QStackedWidget;

namespace caret {

    class Brain;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class ChartableBrainordinateInterface;
    class ChartMatrixDisplayProperties;
    class ChartableMatrixInterface;
    class ChartableMatrixParcelInterface;
    class ChartableMatrixSeriesInterface;
    class ChartModel;
    class EnumComboBoxTemplate;
    class MapYokingGroupComboBox;
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
        
        void matrixParcelFileSelected(CaretDataFile* caretDataFile);
        
        void matrixParcelFileLoadingComboBoxActivated();

        void matrixParcelYokingGroupEnumComboBoxActivated();
        
        void matrixParcelColorBarActionTriggered(bool status);
        
        void matrixParcelSettingsActionTriggered();
        
        void parcelLabelFileRemappingFileSelectorChanged();
        
        void matrixSeriesFileSelected(CaretDataFile* caretDataFile);
        
        void matrixSeriesColorBarActionTriggered(bool status);
        
        void matrixSeriesSettingsActionTriggered();
        
        void matrixSeriesYokingGroupActivated();
        
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
            MODE_MATRIX_LAYER,
            MODE_MATRIX_SERIES
        };
        
        // ADD_NEW_MEMBERS_HERE

        QWidget* createBrainordinateChartWidget();
        
        QWidget* createMatrixParcelChartWidget(const Qt::Orientation orientation);
        
        QWidget* createMatrixSeriesChartWidget(const Qt::Orientation orientation);
        
        void updateSelectionViewController();
        
        void updateBrainordinateChartWidget(Brain* brain,
                                            ModelChart* modelChart,
                                            const int32_t browserTabIndex);
        
        void updateMatrixParcelChartWidget(Brain* brain,
                                     ModelChart* modelChart,
                                     const int32_t browserTabIndex);
        
        void updateMatrixSeriesChartWidget(Brain* brain,
                                           ModelChart* modelChart,
                                           const int32_t browserTabIndex);
        
        ChartableBrainordinateInterface* getBrainordinateFileAtIndex(const int32_t indx);
        
//        ChartableMatrixInterface* getMatrixFileAtIndex(const int32_t indx);
        
//        ChartModel* getSelectedChartModel();
        
        bool getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
                                         ChartableMatrixInterface* & chartableMatrixInterfaceOut,
                                         ChartableMatrixParcelInterface* &chartableMatrixParcelInterfaceOut,
                                         ChartableMatrixSeriesInterface* &chartableMatrixSeriesInterfaceOut,
                                         ChartMatrixDisplayProperties* &chartMatrixDisplayPropertiesOut,
                                         int32_t& browserTabIndexOut);
        
        QStackedWidget* m_stackedWidget;
        
        QWidget* m_brainordinateChartWidget;
        
        QWidget* m_matrixParcelChartWidget;
        
        QWidget* m_matrixSeriesChartWidget;
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<QCheckBox*> m_brainordinateFileEnableCheckBoxes;
        
        std::vector<QLineEdit*> m_brainordinateFileNameLineEdits;
        
        QGridLayout* m_brainordinateGridLayout;
        
        QSignalMapper* m_signalMapperBrainordinateFileEnableCheckBox;
        
        CaretDataFileSelectionComboBox* m_matrixParcelFileSelectionComboBox;
        
        EnumComboBoxTemplate* m_matrixParcelLoadByColumnRowComboBox;
                
        EnumComboBoxTemplate* m_matrixParcelYokingGroupComboBox;

        QAction* m_matrixParcelColorBarAction;
        
        QAction* m_matrixParcelSettingsAction;
        
        QGroupBox* m_parcelRemappingGroupBox;
        
        QCheckBox* m_parcelReorderingEnabledCheckBox;
        
        CaretMappableDataFileAndMapSelectorObject* m_parcelLabelFileRemappingFileSelector;
        
        QAction* m_matrixSeriesColorBarAction;
        
        QAction* m_matrixSeriesSettingsAction;
        
        CaretDataFileSelectionComboBox* m_matrixSeriesFileSelectionComboBox;
        
        MapYokingGroupComboBox* m_matrixSeriesYokingComboBox;
        
        static const int COLUMN_CHECKBOX;
        static const int COLUMN_LINE_EDIT;
        
    };
    
#ifdef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
    const int ChartSelectionViewController::COLUMN_CHECKBOX    = 0;
    const int ChartSelectionViewController::COLUMN_LINE_EDIT   = 1;
#endif // __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_SELECTION_VIEW_CONTROLLER_H__
