#ifndef __CHART_LINES_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_LINES_SELECTION_VIEW_CONTROLLER_H__

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
class QGridLayout;
class QLineEdit;
class QSignalMapper;

namespace caret {

    class Brain;
    class ChartableLineSeriesInterface;
    class ChartableLineSeriesBrainordinateInterface;
    class MapYokingGroupComboBox;
    class ModelChart;
    
    class ChartLinesSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartLinesSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartLinesSelectionViewController();
        
    private slots:
        void brainordinateSelectionCheckBoxClicked(int);
        
        void brainordinateYokingComboBoxActivated(int);
        
    private:
        ChartLinesSelectionViewController(const ChartLinesSelectionViewController&);

        ChartLinesSelectionViewController& operator=(const ChartLinesSelectionViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        // ADD_NEW_MEMBERS_HERE

        void updateSelectionViewController();
        
        const int32_t m_browserWindowIndex;
        
        struct FileInfoRow {
            QLineEdit* m_fileNameLineEdit;
            
            QCheckBox* m_fileEnableCheckBox;
            
            MapYokingGroupComboBox* m_fileYokingComboBox;
            
            ChartableLineSeriesInterface* m_lineSeriesFile;
        };
        
        std::vector<FileInfoRow> m_fileInfoRows;
        
        QGridLayout* m_brainordinateGridLayout;
        
        QSignalMapper* m_signalMapperBrainordinateFileEnableCheckBox;
        
        QSignalMapper* m_signalMapperBrainordinateYokingComboBox;
        
        static const int BRAINORDINATE_COLUMN_CHECKBOX;
        static const int BRAINORDINATE_COLUMN_YOKING_COMBO_BOX;
        static const int BRAINORDINATE_COLUMN_LINE_EDIT;
    };
    
#ifdef __CHART_LINES_SELECTION_VIEW_CONTROLLER_DECLARE__
    const int ChartLinesSelectionViewController::BRAINORDINATE_COLUMN_CHECKBOX         = 0;
    const int ChartLinesSelectionViewController::BRAINORDINATE_COLUMN_YOKING_COMBO_BOX = 1;
    const int ChartLinesSelectionViewController::BRAINORDINATE_COLUMN_LINE_EDIT        = 2;
#endif // __CHART_LINES_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_LINES_SELECTION_VIEW_CONTROLLER_H__
