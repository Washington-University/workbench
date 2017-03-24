
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CHART_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
#include "ChartTwoOverlaySetViewController.h"
#undef __CHART_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartTwoOverlayViewController.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoOverlaySetViewController 
 * \brief View controller for a chart overlay set
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param orientation
 *    Orientation for layout
 * @param browserWindowIndex
 *    Index of browser window that contains this view controller.
 * @param parent
 *    Parent widget.
 */
ChartTwoOverlaySetViewController::ChartTwoOverlaySetViewController(const Qt::Orientation orientation,
                                                             const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    
   for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        ChartTwoOverlayViewController* ovc = new ChartTwoOverlayViewController(orientation,
                                                                         m_browserWindowIndex,
                                                                         i,
                                                                         this);
        m_chartOverlayViewControllers.push_back(ovc);
        m_chartOverlayGridLayoutGroups.push_back(new WuQGridLayoutGroup(gridLayout,
                                                                        this));
        
        QObject::connect(ovc, SIGNAL(requestAddOverlayAbove(const int32_t)),
                         this, SLOT(processAddOverlayAbove(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestAddOverlayBelow(const int32_t)),
                         this, SLOT(processAddOverlayBelow(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestRemoveOverlay(const int32_t)),
                         this, SLOT(processRemoveOverlay(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestMoveOverlayUp(const int32_t)),
                         this, SLOT(processMoveOverlayUp(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestMoveOverlayDown(const int32_t)),
                         this, SLOT(processMoveOverlayDown(const int32_t)));
    }
    
    if (orientation == Qt::Horizontal) {
        static const int COLUMN_ON = 0;
        static const int COLUMN_SETTINGS_EDIT = 1;
        static const int COLUMN_SETTINGS_COLOR_BAR = 2;
        static const int COLUMN_SETTINGS_CONSTRUCTION = 3;
        static const int COLUMN_HISTORY = 4;
        static const int COLUMN_MATRIX_VIEW = 5;
        static const int COLUMN_AXIS = 6;
        static const int COLUMN_FILE = 7;
        static const int COLUMN_YOKE = 8;
        static const int COLUMN_MAP_INDEX = 9;
        static const int COLUMN_ALL_MAPS = 10;
        static const int COLUMN_MAP_NAME = 11;
        
        gridLayout->setColumnStretch(COLUMN_ON, 0);
        gridLayout->setColumnStretch(COLUMN_SETTINGS_EDIT, 0);
        gridLayout->setColumnStretch(COLUMN_SETTINGS_COLOR_BAR, 0);
        gridLayout->setColumnStretch(COLUMN_SETTINGS_CONSTRUCTION, 0);
        gridLayout->setColumnStretch(COLUMN_HISTORY, 0);
        gridLayout->setColumnStretch(COLUMN_MATRIX_VIEW, 0);
        gridLayout->setColumnStretch(COLUMN_AXIS, 0);
        gridLayout->setColumnStretch(COLUMN_FILE, 100);
        gridLayout->setColumnStretch(COLUMN_YOKE, 0);
        gridLayout->setColumnStretch(COLUMN_ALL_MAPS, 0);
        gridLayout->setColumnStretch(COLUMN_MAP_INDEX, 0);
        gridLayout->setColumnStretch(COLUMN_MAP_NAME, 100);
        
        QLabel* onLabel       = new QLabel("On");
        QLabel* settingsLabel = new QLabel("Settings");
        //QLabel* historyLabel  = new QLabel("History");
        QLabel* fileLabel     = new QLabel("File");
        QLabel* yokeLabel     = new QLabel("Yoke");
        QLabel* allMapsLabel  = new QLabel("All");
        m_mapRowOrColumnIndexLabel = new QLabel("Index");
        m_mapRowOrColumnNameLabel  = new QLabel("Name");
        
        int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, COLUMN_ON, Qt::AlignHCenter);
        gridLayout->addWidget(settingsLabel, row, COLUMN_SETTINGS_EDIT, 1, 5, Qt::AlignHCenter);
        //gridLayout->addWidget(historyLabel, row, COLUMN_HISTORY, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, COLUMN_FILE, Qt::AlignHCenter);
        gridLayout->addWidget(yokeLabel, row, COLUMN_YOKE, Qt::AlignHCenter);
        gridLayout->addWidget(allMapsLabel, row, COLUMN_ALL_MAPS, Qt::AlignHCenter);
        gridLayout->addWidget(m_mapRowOrColumnIndexLabel, row, COLUMN_MAP_INDEX, Qt::AlignHCenter);
        gridLayout->addWidget(m_mapRowOrColumnNameLabel, row, COLUMN_MAP_NAME, Qt::AlignHCenter);
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
            row = gridLayout->rowCount();
            ChartTwoOverlayViewController* covc = m_chartOverlayViewControllers[i];
            WuQGridLayoutGroup* glg = m_chartOverlayGridLayoutGroups[i];
            
            glg->addWidget(covc->m_enabledCheckBox, row, COLUMN_ON, Qt::AlignHCenter);
            glg->addWidget(covc->m_settingsToolButton, row, COLUMN_SETTINGS_EDIT, Qt::AlignHCenter);
            glg->addWidget(covc->m_colorBarToolButton, row, COLUMN_SETTINGS_COLOR_BAR, Qt::AlignHCenter);
            glg->addWidget(covc->m_constructionToolButton, row, COLUMN_SETTINGS_CONSTRUCTION, Qt::AlignHCenter);
            glg->addWidget(covc->m_historyToolButton, row, COLUMN_HISTORY, Qt::AlignHCenter);
            glg->addWidget(covc->m_matrixTriangularViewModeToolButton, row, COLUMN_MATRIX_VIEW, Qt::AlignHCenter);
            glg->addWidget(covc->m_axisLocationToolButton, row, COLUMN_AXIS, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapFileComboBox, row, COLUMN_FILE);
            glg->addWidget(covc->m_mapRowOrColumnYokingGroupComboBox->getWidget(), row, COLUMN_YOKE, Qt::AlignHCenter);
            glg->addWidget(covc->m_allMapsCheckBox, row, COLUMN_ALL_MAPS, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapRowOrColumnIndexSpinBox, row, COLUMN_MAP_INDEX, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapRowOrColumnNameComboBox, row, COLUMN_MAP_NAME);
        }
    }
    else {
        static const int ROW_ONE_COLUMN_ON = 0;
        static const int ROW_ONE_COLUMN_SETTINGS_EDIT = 1;
        static const int ROW_ONE_COLUMN_SETTINGS_COLOR_BAR = 2;
        static const int ROW_ONE_COLUMN_SETTINGS_CONSTRUCTION = 3;
        static const int ROW_ONE_COLUMN_HISTORY = 4;
        static const int ROW_ONE_COLUMN_MATRIX_VIEW = 5;
        static const int ROW_ONE_COLUMN_AXIS = 6;
        static const int ROW_ONE_COLUMN_FILE_LABEL = 7;
        static const int ROW_ONE_COLUMN_FILE_COMBO_BOX = 8;
        
        static const int ROW_TWO_COLUMN_YOKE = 0;
        static const int ROW_TWO_COLUMN_ALL_MAPS = 3;
        static const int ROW_TWO_COLUMN_MAP_INDEX = 7;
        static const int ROW_TWO_COLUMN_MAP_NAME = 8;
        
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 0);
        gridLayout->setColumnStretch(6, 0);
        gridLayout->setColumnStretch(7, 0);
        gridLayout->setColumnStretch(8, 100);

        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
            WuQGridLayoutGroup* glg = m_chartOverlayGridLayoutGroups[i];
            
            int row = gridLayout->rowCount();
            if (i > 0) {
                QFrame* bottomHorizontalLineWidget = new QFrame();
                bottomHorizontalLineWidget->setLineWidth(0);
                bottomHorizontalLineWidget->setMidLineWidth(1);
                bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
                glg->addWidget(bottomHorizontalLineWidget,
                                                 row, 0, 1, -1);
                row++;
            }

            ChartTwoOverlayViewController* covc = m_chartOverlayViewControllers[i];

            QLabel* fileLabel = new QLabel("File");
            glg->addWidget(covc->m_enabledCheckBox, row, ROW_ONE_COLUMN_ON, Qt::AlignHCenter);
            glg->addWidget(covc->m_settingsToolButton, row, ROW_ONE_COLUMN_SETTINGS_EDIT, Qt::AlignHCenter);
            glg->addWidget(covc->m_colorBarToolButton, row, ROW_ONE_COLUMN_SETTINGS_COLOR_BAR, Qt::AlignHCenter);
            glg->addWidget(covc->m_constructionToolButton, row, ROW_ONE_COLUMN_SETTINGS_CONSTRUCTION, Qt::AlignHCenter);
            glg->addWidget(covc->m_historyToolButton, row, ROW_ONE_COLUMN_HISTORY, Qt::AlignHCenter);
            glg->addWidget(covc->m_matrixTriangularViewModeToolButton, row, ROW_ONE_COLUMN_MATRIX_VIEW, Qt::AlignHCenter);
            glg->addWidget(covc->m_axisLocationToolButton, row, ROW_ONE_COLUMN_AXIS, Qt::AlignHCenter);
            glg->addWidget(fileLabel, row, ROW_ONE_COLUMN_FILE_LABEL, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapFileComboBox, row, ROW_ONE_COLUMN_FILE_COMBO_BOX, 1, 2);
            row++;
            
            glg->addWidget(covc->m_mapRowOrColumnYokingGroupComboBox->getWidget(), row, ROW_TWO_COLUMN_YOKE, 1, 2, Qt::AlignHCenter);
//            glg->addWidget(covc->m_historyToolButton, row, ROW_TWO_COLUMN_HISTORY, 1, 2, Qt::AlignHCenter);
            glg->addWidget(covc->m_allMapsCheckBox, row, ROW_TWO_COLUMN_ALL_MAPS, 1, 4, Qt::AlignRight);
            glg->addWidget(covc->m_mapRowOrColumnIndexSpinBox, row, ROW_TWO_COLUMN_MAP_INDEX, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapRowOrColumnNameComboBox, row, ROW_TWO_COLUMN_MAP_NAME);
        }
    }
    
    if (orientation == Qt::Horizontal) {
        QVBoxLayout* verticalLayout = new QVBoxLayout(this);
        WuQtUtilities::setLayoutSpacingAndMargins(verticalLayout, 2, 2);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
    }
    else {
        /*
         * Resolve WB-649
         */
        QVBoxLayout* verticalLayout = new QVBoxLayout(this);
        WuQtUtilities::setLayoutSpacingAndMargins(verticalLayout, 1, 1);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartTwoOverlaySetViewController::~ChartTwoOverlaySetViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoOverlaySetViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isToolBoxUpdate()) {
                this->updateViewController();
                uiEvent->setEventProcessed();
            }
        }
    }
}

/**
 * Update this overlay set view controller using the given overlay set.
 */
void
ChartTwoOverlaySetViewController::updateViewController()
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet == NULL) {
        return;
    }
    
    const int32_t numberOfOverlays = static_cast<int32_t>(m_chartOverlayViewControllers.size());
    const int32_t numberOfDisplayedOverlays = chartOverlaySet->getNumberOfDisplayedOverlays();
    
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        ChartTwoOverlay* chartOverlay = NULL;
        if (chartOverlaySet != NULL) {
            chartOverlay = chartOverlaySet->getOverlay(i);
        }
        m_chartOverlayViewControllers[i]->updateViewController(chartOverlay);
        
        bool displayOverlay = (chartOverlay != NULL);
        if (i >= numberOfDisplayedOverlays) {
            displayOverlay = false;
        }
        CaretAssertVectorIndex(m_chartOverlayGridLayoutGroups, i);
        m_chartOverlayGridLayoutGroups[i]->setVisible(displayOverlay);
    }
    
//    const ChartTwoOverlay* primaryOverlay = chartOverlaySet->getPrimaryOverlay();
//    if (primaryOverlay != NULL) {
//        AString mapRowOrColumnName = "Map";
//        switch (primaryOverlay->getChartTwoDataType()) {
//            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                break;
//            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
//                break;
//            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
//                break;
//            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
//                mapRowOrColumnName = "Row";
//                break;
//        }
//        
//        if (m_mapRowOrColumnIndexLabel != NULL) {
//            m_mapRowOrColumnIndexLabel->setText(mapRowOrColumnName+ " Index");
//        }
//        if (m_mapRowOrColumnNameLabel != NULL) {
//            m_mapRowOrColumnNameLabel->setText(mapRowOrColumnName + " Name");
//        }
//    }
}

/**
 * @return The overlay set in this view controller.
 */
ChartTwoOverlaySet*
ChartTwoOverlaySetViewController::getChartTwoOverlaySet()
{
    ChartTwoOverlaySet* chartOverlaySet = NULL;
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent != NULL) {
        chartOverlaySet = browserTabContent->getChartTwoOverlaySet();
    }
    
    return chartOverlaySet;
}

/**
 * Add an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will have an overlay added above it.
 */
void
ChartTwoOverlaySetViewController::processAddOverlayAbove(const int32_t overlayIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet != NULL) {
        chartOverlaySet->insertOverlayAbove(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Add an overlay below the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will have an overlay added below it.
 */
void
ChartTwoOverlaySetViewController::processAddOverlayBelow(const int32_t overlayIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet != NULL) {
        chartOverlaySet->insertOverlayBelow(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void
ChartTwoOverlaySetViewController::processRemoveOverlay(const int32_t overlayIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet != NULL) {
        chartOverlaySet->removeDisplayedOverlay(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void
ChartTwoOverlaySetViewController::processMoveOverlayDown(const int32_t overlayIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet != NULL) {
        chartOverlaySet->moveDisplayedOverlayDown(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void
ChartTwoOverlaySetViewController::processMoveOverlayUp(const int32_t overlayIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = getChartTwoOverlaySet();
    if (chartOverlaySet != NULL) {
        chartOverlaySet->moveDisplayedOverlayUp(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Update surface coloring and graphics after overlay changes.
 */
void
ChartTwoOverlaySetViewController::updateColoringAndGraphics()
{
    this->updateViewController();
    
    EventGraphicsUpdateOneWindow graphicsUpdate(m_browserWindowIndex);
    EventManager::get()->sendEvent(graphicsUpdate.getPointer());
}
