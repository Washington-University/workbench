#ifndef __CONNECTIVITY_MANAGER_VIEW_CONTROLLER__H_
#define __CONNECTIVITY_MANAGER_VIEW_CONTROLLER__H_

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

#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"

class QAction;
class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QToolButton;
class QHBoxLayout;
class QLabel;
class QSpinBox;
class TimeCourseDialog;

namespace caret {

    class ChartableInterface;
    class ConnectivityTimeSeriesViewController;
    class CiftiConnectivityMatrixViewController;
    class ConnectivityManagerViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ConnectivityManagerViewController(const Qt::Orientation orientation,
                                          const int32_t browserWindowIndex,
                                          const DataFileTypeEnum::Enum connectivityFileType,
                                          QWidget* parent = 0);
        
        virtual ~ConnectivityManagerViewController();

        void receiveEvent(Event* event);
        
    private slots:
        void graphActionTriggered();

        void movieActionTriggered(bool status);

        void renderMovieActionTriggered(bool status);
        
    private:
        ConnectivityManagerViewController(const ConnectivityManagerViewController&);

        ConnectivityManagerViewController& operator=(const ConnectivityManagerViewController&);
        
        void updateManagerViewController();
        
        void updateForTimeSeriesFiles(const std::vector<ChartableInterface*>& timeSeriesFiles);

        void captureFrame(AString filename);

		void processRotateTransformation(const double dx,
			const double dy,
			const double dz);

        Qt::Orientation orientation;
        
        int32_t browserWindowIndex;
        
        DataFileTypeEnum::Enum connectivityFileType;
        
        QGridLayout* viewControllerGridLayout;
        
        std::vector<CiftiConnectivityMatrixViewController*> connectivityMatrixViewControllers;

        std::vector<ConnectivityTimeSeriesViewController*> timeSeriesViewControllers;

        QHBoxLayout *timeSeriesButtonLayout;

        QToolButton *graphToolButton;

        QAction* graphAction;

        QLabel *frameRepeatLabel;

        QSpinBox *frameRepeatSpinBox;

        QLabel *frameRotateLabel;

        QLabel *frameRotateXLabel;

        QDoubleSpinBox *frameRotateXSpinBox;

        QLabel *frameRotateYLabel;

        QDoubleSpinBox *frameRotateYSpinBox;

        QLabel *frameRotateZLabel;

        QDoubleSpinBox *frameRotateZSpinBox;

        QLabel *frameRotateCountLabel;

        QSpinBox *frameRotateCountSpinBox;

        QLabel *frameRotateReverseDirectionLabel;

        QCheckBox *frameRotateReverseDirection;

        QToolButton *renderMovieButton;

        QAction *renderMovieAction;

        QToolButton *movieToolButton;

        QAction* movieAction;

		int frame_number;

        double dx;
        double dy;
        double dz;               
        bool frameCountEnabled;
        int frameCount;
        bool reverseDirection;

        TimeCourseDialog *tcDialog;
        
        static std::set<ConnectivityManagerViewController*> allManagerViewControllers;
        
        static AString previousNetworkUsername;
        static AString previousNetworkPassword;
    };
    
#ifdef __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__
    std::set<ConnectivityManagerViewController*> ConnectivityManagerViewController::allManagerViewControllers;
    AString ConnectivityManagerViewController::previousNetworkUsername = "";
    AString ConnectivityManagerViewController::previousNetworkPassword = "";
#endif // __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_MANAGER_VIEW_CONTROLLER__H_
