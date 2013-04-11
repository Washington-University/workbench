
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

#include <QAction>
#include <QDir>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QToolButton>
#include <QVBoxLayout>

#define __CHART_VIEW_CONTROLLER_DECLARE__
#include "ChartSetViewController.h"
#undef __CHART_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartSetViewController.h"
#include "EventManager.h"
#include "EventListenerInterface.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "QCheckBox"
#include "QFileDialog"
#include "QHBoxLayout"
#include "QLabel"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "QGroupBox"
#include "QHBoxLayout"
#include "QListWidget"
#include "QPushButton"
#include "QSpacerItem"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ChartSetViewController 
 * \brief View-Controller for the ChartModel
 * \addtogroup GuiQt
 */
 
/**
 * Constructor.
 */
ChartSetViewController::ChartSetViewController(const Qt::Orientation orientation,
                                                                     const int32_t /*browserWindowIndex*/,
                                                                     /*const DataFileTypeEnum::Enum connectivityFileType, */                                                                    
                                                                     QWidget* parent)
: QWidget(parent)
{
    



    if (orientation == Qt::Horizontal)
    {

        


    }
}

/**
 * Destructor.
 */
ChartSetViewController::~ChartSetViewController()
{    
    
}



/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
ChartSetViewController::receiveEvent(Event* event)
{
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        
        
        
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {

        
    }


}


