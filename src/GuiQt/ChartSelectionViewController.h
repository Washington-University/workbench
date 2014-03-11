#ifndef __CHART_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_SELECTION_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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

namespace caret {

    class ChartableBrainordinateInterface;
    class ChartableMatrixInterface;
    class ChartModel;
    
    class ChartSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartSelectionViewController();
        
    private slots:
        void selectionCheckBoxClicked(int);
        
        void selectionRadioButtonClicked(int);
        
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

        void updateSelectionViewController();
        
        ChartableBrainordinateInterface* getBrainordinateFileAtIndex(const int32_t indx);
        
        ChartableMatrixInterface* getMatrixFileAtIndex(const int32_t indx);
        
//        ChartModel* getSelectedChartModel();
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<QCheckBox*> m_fileEnableCheckBoxes;
        
        std::vector<QLineEdit*> m_fileNameLineEdits;
        
        std::vector<QRadioButton*> m_fileSelectionRadioButtons;
        
        QButtonGroup* m_selectionRadioButtonGroup;
        
        QGridLayout* m_gridLayout;
        
        QSignalMapper* m_signalMapperFileEnableCheckBox;
        
        QLabel* m_averageLabel;
        
        static const int COLUMN_CHECKBOX;
        static const int COLUMN_RADIOBUTTON;
        static const int COLUMN_LINE_EDIT;
        
    };
    
#ifdef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
    const int ChartSelectionViewController::COLUMN_CHECKBOX    = 0;
    const int ChartSelectionViewController::COLUMN_RADIOBUTTON = 1;
    const int ChartSelectionViewController::COLUMN_LINE_EDIT   = 2;
#endif // __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_SELECTION_VIEW_CONTROLLER_H__
