#ifndef __CHART_VIEW_CONTROLLER__H_
#define __CHART_VIEW_CONTROLLER__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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
#include <set>
#include <QWidget>

#include "ChartViewController.h"
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"

class QAction;
class QPushButton;
class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QGroupBox;
class QToolButton;
class QHBoxLayout;
class QLabel;
class QListWidget;
class QSpinBox;
class QVBoxLayout;
class WuQGridLayout;


namespace caret {

    class ChartViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartViewController(const Qt::Orientation orientation,
                                          const int32_t browserWindowIndex,
                                          /*const DataFileTypeEnum::Enum connectivityFileType,*/
                                          QWidget* parent = 0);
        
        virtual ~ChartViewController();

        void receiveEvent(Event* event);
        
    private slots:
        
        
    private:
        /*ChartViewController(const ChartViewController&);

        ChartViewController& operator=(const ChartViewController&);*/
        
        int32_t browserWindowIndex;
        
        std::vector<ChartViewController*> chartViewControllers;

        QHBoxLayout *chartButtonLayout;
        
        
        static std::set<ChartViewController*> allChartViewControllers;
        WuQGridLayout* gridLayout;

        QCheckBox* enabledCheckBox;

        QAction* settingsAction;

        QLabel* timeLineName;

        
        
    };
    
#ifdef __CHART_VIEW_CONTROLLER_DECLARE__
    std::set<ChartViewController*> ChartViewController::allChartViewControllers;
    
#endif // __CHART_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_VIEW_CONTROLLER__H_
