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

class QStackedWidget;

namespace caret {
    class ChartLinesSelectionViewController;
    class ChartMatrixParcelSelectionViewController;
    class ChartMatrixSeriesSelectionViewController;

    class ChartSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     const QString& parentObjectName,
                                     QWidget* parent);
        
        virtual ~ChartSelectionViewController();
        
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

        
        void updateSelectionViewController();
        
        QStackedWidget* m_stackedWidget;
        
        ChartLinesSelectionViewController* m_brainordinateChartWidget;
        
        ChartMatrixParcelSelectionViewController* m_matrixParcelChartWidget;
        
        ChartMatrixSeriesSelectionViewController* m_matrixSeriesChartWidget;
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
    };
    
#ifdef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
#endif // __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_SELECTION_VIEW_CONTROLLER_H__
