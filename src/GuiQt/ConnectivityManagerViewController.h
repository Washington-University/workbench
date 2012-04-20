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

class QGridLayout;

namespace caret {

    class ConnectivityLoaderFile;
    class ConnectivityTimeSeriesViewController;
    class ConnectivityDenseViewController;
    
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
        void processLoadFileFromWeb();
        
    private:
        ConnectivityManagerViewController(const ConnectivityManagerViewController&);

        ConnectivityManagerViewController& operator=(const ConnectivityManagerViewController&);
        
        void updateManagerViewController();
        
        void updateForDenseFiles(const std::vector<ConnectivityLoaderFile*>& denseFiles);
        
        void updateForTimeSeriesFiles(const std::vector<ConnectivityLoaderFile*>& timeSeriesFiles);
        
        Qt::Orientation orientation;
        
        int32_t browserWindowIndex;
        
        DataFileTypeEnum::Enum connectivityFileType;
        
        QGridLayout* viewControllerGridLayout;
        
        std::vector<ConnectivityDenseViewController*> denseViewControllers;

        std::vector<ConnectivityTimeSeriesViewController*> timeSeriesViewControllers;
        
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
