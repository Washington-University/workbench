#ifndef __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_H__

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

class QCheckBox;
class QGroupBox;

namespace caret {

    class Brain;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class ChartMatrixDisplayProperties;
    class ChartableMatrixInterface;
    class ChartableMatrixParcelInterface;
    class ChartableMatrixSeriesInterface;
    class EnumComboBoxTemplate;
    class ModelChart;
    
    class ChartMatrixParcelSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartMatrixParcelSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartMatrixParcelSelectionViewController();
        
    private slots:
        void matrixParcelFileSelected(CaretDataFile* caretDataFile);
        
        void matrixParcelFileLoadingComboBoxActivated();

        void matrixParcelYokingGroupEnumComboBoxActivated();
        
        void matrixParcelColorBarActionTriggered(bool status);
        
        void matrixParcelSettingsActionTriggered();
        
        void parcelLabelFileRemappingFileSelectorChanged();
        
    private:
        ChartMatrixParcelSelectionViewController(const ChartMatrixParcelSelectionViewController&);

        ChartMatrixParcelSelectionViewController& operator=(const ChartMatrixParcelSelectionViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        // ADD_NEW_MEMBERS_HERE

        QGroupBox* createMatrixParcelChartWidget(const Qt::Orientation orientation);
        
        QGroupBox* createParcelRemappingWidget(const Qt::Orientation orientation);
        
        void updateSelectionViewController();
        
        void updateMatrixParcelChartWidget(Brain* brain,
                                     ModelChart* modelChart,
                                     const int32_t browserTabIndex);
        
        bool getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
                                         ChartableMatrixInterface* & chartableMatrixInterfaceOut,
                                         ChartableMatrixParcelInterface* &chartableMatrixParcelInterfaceOut,
                                         ChartableMatrixSeriesInterface* &chartableMatrixSeriesInterfaceOut,
                                         ChartMatrixDisplayProperties* &chartMatrixDisplayPropertiesOut,
                                         int32_t& browserTabIndexOut);
        
        QGroupBox* m_matrixParcelChartWidget;
        
        const int32_t m_browserWindowIndex;
        
        CaretDataFileSelectionComboBox* m_matrixParcelFileSelectionComboBox;
        
        EnumComboBoxTemplate* m_matrixParcelLoadByColumnRowComboBox;
                
        EnumComboBoxTemplate* m_matrixParcelYokingGroupComboBox;

        QAction* m_matrixParcelColorBarAction;
        
        QAction* m_matrixParcelSettingsAction;
        
        QGroupBox* m_parcelRemappingGroupBox;
        
        QCheckBox* m_parcelReorderingEnabledCheckBox;
        
        CaretMappableDataFileAndMapSelectorObject* m_parcelLabelFileRemappingFileSelector;
        
    };
    
#ifdef __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_DECLARE__
#endif // __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_H__
