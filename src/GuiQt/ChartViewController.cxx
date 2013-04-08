
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
#include "ChartViewController.h"
#undef __CHART_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartViewController.h"
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
 * \class caret::ChartViewController 
 * \brief View-Controller for the ChartModel
 *
 */
/**
 * Constructor.
 */
ChartViewController::ChartViewController(const Qt::Orientation orientation,
                                                                     const int32_t /*browserWindowIndex*/,
                                                                     /*const DataFileTypeEnum::Enum connectivityFileType, */                                                                    
                                                                     QWidget* parent)
: QWidget(parent)
{
    this->m_hBoxLayout = new QHBoxLayout(this);
    this->m_dataControlsGroupBox = new QGroupBox("Data Controls");
    this->m_dataControlsLayout = new QVBoxLayout();
    this->m_XMinSpinBox = new QDoubleSpinBox();
    this->m_XMaxSpinBox = new QDoubleSpinBox();
    this->m_YMinSpinBox = new QDoubleSpinBox();
    this->m_YMaxSpinBox = new QDoubleSpinBox();
    this->m_lineWidthSpinBox = new QSpinBox();

    this->m_autofitTimeLinesCB = new QCheckBox("Autofit Plot");
    this->m_zoomXAxisCB = new QCheckBox("Zoom X Axis");
    this->m_zoomYAxisCB = new QCheckBox("Zoom Y Axis");
    this->m_showAverageCB = new QCheckBox("Show Average");
    this->m_clearChartPB = new QPushButton("Clear Chart");
    this->m_resetViewPB = new QPushButton("Reset View");
    this->m_openTimeLinePB = new QPushButton("Open...");
    this->m_exportTimeLinePB = new QPushButton("Export...");

    this->m_historyGroupBox = new QGroupBox("History");
    this->m_historyLayout = new QVBoxLayout();
    this->m_keepLastSpinBox = new QSpinBox();
    this->m_historyList = new QListWidget();



    if (orientation == Qt::Horizontal)
    {
        QHBoxLayout *temp = new QHBoxLayout();
        QLabel *tempLabel;

        this->m_hBoxLayout->addWidget(m_dataControlsGroupBox);
        this->m_dataControlsGroupBox->setLayout(this->m_dataControlsLayout);
        this->m_dataControlsLayout->addLayout(temp);
        
        tempLabel = new QLabel("X");
        tempLabel->setFixedWidth(50);
        tempLabel->setAlignment(Qt::AlignRight);
        temp->addWidget(tempLabel);    
        this->m_XMinSpinBox->setFixedWidth(80);
        this->m_XMaxSpinBox->setFixedWidth(80);
        temp->addWidget(this->m_XMinSpinBox);
        temp->addWidget(this->m_XMaxSpinBox);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        tempLabel = new QLabel("Y");
        tempLabel->setFixedWidth(50);
        tempLabel->setAlignment(Qt::AlignRight);
        temp->addWidget(tempLabel);
        this->m_YMinSpinBox->setFixedWidth(80);
        this->m_YMaxSpinBox->setFixedWidth(80);
        temp->addWidget(this->m_YMinSpinBox);
        temp->addWidget(this->m_YMaxSpinBox);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        tempLabel = new QLabel("Line Width");
        tempLabel->setFixedWidth(50);
        tempLabel->setAlignment(Qt::AlignRight);
        temp->addWidget(tempLabel);
        temp->addWidget(this->m_lineWidthSpinBox);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        this->m_autofitTimeLinesCB->setFixedWidth(105);
        temp->addWidget(this->m_autofitTimeLinesCB);
        temp->addWidget(this->m_zoomXAxisCB);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        this->m_showAverageCB->setFixedWidth(105);
        temp->addWidget(this->m_showAverageCB);
        temp->addWidget(this->m_zoomYAxisCB);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        temp->addWidget(this->m_clearChartPB);
        temp->addWidget(this->m_resetViewPB);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        temp->addWidget(this->m_openTimeLinePB);
        temp->addWidget(this->m_exportTimeLinePB);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        temp = new QHBoxLayout();
        this->m_dataControlsLayout->addLayout(temp);
        temp->addWidget(this->m_openTimeLinePB);
        temp->addWidget(this->m_exportTimeLinePB);
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));

        this->m_dataControlsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum,
            QSizePolicy::MinimumExpanding));

        QVBoxLayout *tempV;
        this->m_hBoxLayout->addWidget(m_historyGroupBox);
        this->m_historyGroupBox->setLayout(this->m_historyLayout);
        tempV = new QVBoxLayout();
        temp = new QHBoxLayout();
        temp->addWidget(new QLabel("Keep Last"));
        temp->addWidget(this->m_keepLastSpinBox);
        
        temp->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding,
            QSizePolicy::Maximum ));
        this->m_historyLayout->addLayout(tempV);
        
        tempV->addWidget(this->m_historyList);
        tempV->addLayout(temp);


    }
}

/**
 * Destructor.
 */
ChartViewController::~ChartViewController()
{    
    
}



/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
ChartViewController::receiveEvent(Event* event)
{
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        
        
        
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {

        
    }


}


