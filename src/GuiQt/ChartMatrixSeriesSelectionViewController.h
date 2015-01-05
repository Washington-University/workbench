#ifndef __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_H__

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

namespace caret {

    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretMappableDataFile;
    class ChartMatrixDisplayProperties;
    class ChartableMatrixInterface;
    class ChartableMatrixParcelInterface;
    class ChartableMatrixSeriesInterface;
    class MapYokingGroupComboBox;
    
    class ChartMatrixSeriesSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartMatrixSeriesSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartMatrixSeriesSelectionViewController();
        
    private slots:
        void matrixSeriesFileSelected(CaretDataFile* caretDataFile);
        
        void matrixSeriesColorBarActionTriggered(bool status);
        
        void matrixSeriesSettingsActionTriggered();
        
        void matrixSeriesYokingGroupActivated();
        
    private:
        ChartMatrixSeriesSelectionViewController(const ChartMatrixSeriesSelectionViewController&);

        ChartMatrixSeriesSelectionViewController& operator=(const ChartMatrixSeriesSelectionViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        // ADD_NEW_MEMBERS_HERE

        void updateSelectionViewController();
        
        bool getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
                                         ChartableMatrixInterface* & chartableMatrixInterfaceOut,
                                         ChartableMatrixParcelInterface* &chartableMatrixParcelInterfaceOut,
                                         ChartableMatrixSeriesInterface* &chartableMatrixSeriesInterfaceOut,
                                         ChartMatrixDisplayProperties* &chartMatrixDisplayPropertiesOut,
                                         int32_t& browserTabIndexOut);
        
        const int32_t m_browserWindowIndex;
        
        QAction* m_matrixSeriesColorBarAction;
        
        QAction* m_matrixSeriesSettingsAction;
        
        CaretDataFileSelectionComboBox* m_matrixSeriesFileSelectionComboBox;
        
        MapYokingGroupComboBox* m_matrixSeriesYokingComboBox;
    };
    
#ifdef __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_DECLARE__
#endif // __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_H__
