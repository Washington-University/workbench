
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
#include <QGroupBox>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorToolButton.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartTwoOverlayViewController.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "WuQDoubleSpinBox.h"
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
 * @param parentObjectName
 *    Name of parent object for macros
 * @param parent
 *    Parent widget.
 */
ChartTwoOverlaySetViewController::ChartTwoOverlaySetViewController(const Qt::Orientation orientation,
                                                                   const int32_t browserWindowIndex,
                                                                   const QString& parentObjectName,
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
                                                                              parentObjectName,
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
        int32_t columnCounter(0);
        static const int COLUMN_ON         = columnCounter++;
        static const int COLUMN_LOAD       = columnCounter++;
        static const int COLUMN_SETTINGS   = columnCounter++;
        static const int COLUMN_LINE_WIDTH = columnCounter++;
        static const int COLUMN_POINT      = columnCounter++;
        static const int COLUMN_FILE       = columnCounter++;
        static const int COLUMN_YOKE       = columnCounter++;
        static const int COLUMN_MAP_INDEX  = columnCounter++;
        static const int COLUMN_ALL_MAPS   = columnCounter++;
        static const int COLUMN_MAP_NAME   = columnCounter++;
        
        gridLayout->setColumnStretch(COLUMN_ON, 0);
        gridLayout->setColumnStretch(COLUMN_LOAD, 0);
        gridLayout->setColumnStretch(COLUMN_SETTINGS, 0);
        gridLayout->setColumnStretch(COLUMN_LINE_WIDTH, 0);
        gridLayout->setColumnStretch(COLUMN_FILE, 100);
        gridLayout->setColumnStretch(COLUMN_YOKE, 0);
        gridLayout->setColumnStretch(COLUMN_ALL_MAPS, 0);
        gridLayout->setColumnStretch(COLUMN_MAP_INDEX, 0);
        gridLayout->setColumnStretch(COLUMN_MAP_NAME, 100);
        
        QLabel* onLabel       = new QLabel("On");
        m_loadLabel           = new QLabel("Load");
        QLabel* settingsLabel = new QLabel("Settings");
        m_lineWidthLabel      = new QLabel("Width");
        m_pointLabel          = new QLabel("Point");
        QLabel* fileLabel     = new QLabel("File");
        QLabel* yokeLabel     = new QLabel("Yoke");
        m_allMapsLabel        = new QLabel("All");
        m_mapRowOrColumnIndexLabel = new QLabel("Index");
        m_mapRowOrColumnNameLabel  = new QLabel("Name");
        
        int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, COLUMN_ON, Qt::AlignHCenter);
        gridLayout->addWidget(m_loadLabel, row, COLUMN_LOAD, Qt::AlignHCenter);
        gridLayout->addWidget(settingsLabel, row, COLUMN_SETTINGS, Qt::AlignHCenter);
        gridLayout->addWidget(m_lineWidthLabel, row, COLUMN_LINE_WIDTH, Qt::AlignHCenter);
        gridLayout->addWidget(m_pointLabel, row, COLUMN_POINT, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, COLUMN_FILE, Qt::AlignHCenter);
        gridLayout->addWidget(yokeLabel, row, COLUMN_YOKE, Qt::AlignHCenter);
        gridLayout->addWidget(m_allMapsLabel, row, COLUMN_ALL_MAPS, Qt::AlignHCenter);
        gridLayout->addWidget(m_mapRowOrColumnIndexLabel, row, COLUMN_MAP_INDEX, Qt::AlignHCenter);
        gridLayout->addWidget(m_mapRowOrColumnNameLabel, row, COLUMN_MAP_NAME, Qt::AlignHCenter);
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
            ChartTwoOverlayViewController* covc = m_chartOverlayViewControllers[i];

            QWidget* settingsWidget = new QWidget();
            QHBoxLayout* settingsLayout = new QHBoxLayout(settingsWidget);
            settingsLayout->setContentsMargins(0, 0, 0, 0);
            settingsLayout->setSpacing(3);
            settingsLayout->addWidget(covc->m_settingsToolButton);
            settingsLayout->addWidget(covc->m_colorBarToolButton);
            settingsLayout->addWidget(covc->m_constructionToolButton);
            settingsLayout->addWidget(covc->m_matrixTriangularViewModeToolButton);
            settingsLayout->addWidget(covc->m_axisLocationToolButton);
            settingsLayout->addWidget(covc->m_lineLayerColorToolButton);
            settingsLayout->addWidget(covc->m_lineLayerToolTipOffsetToolButton);

            QWidget* pointWidget = new QWidget();
            QHBoxLayout* pointLayout = new QHBoxLayout(pointWidget);
            pointLayout->setContentsMargins(0, 0, 0, 0);
            pointLayout->addWidget(covc->m_selectedPointCheckBox);
            pointLayout->addWidget(covc->m_selectedPointIndexSpinBox);
            
            row = gridLayout->rowCount();
            WuQGridLayoutGroup* glg = m_chartOverlayGridLayoutGroups[i];
            
            glg->addWidget(covc->m_enabledCheckBox, row, COLUMN_ON, Qt::AlignHCenter);
            glg->addWidget(covc->m_lineSeriesLoadingEnabledCheckBox, row, COLUMN_LOAD, Qt::AlignHCenter);
            glg->addWidget(settingsWidget, row, COLUMN_SETTINGS, Qt::AlignHCenter);
            glg->addWidget(covc->m_lineLayerWidthSpinBox->getWidget(), row, COLUMN_LINE_WIDTH);
            glg->addWidget(pointWidget, row, COLUMN_POINT);
            glg->addWidget(covc->m_mapFileComboBox, row, COLUMN_FILE);
            glg->addWidget(covc->m_mapRowOrColumnYokingGroupComboBox->getWidget(), row, COLUMN_YOKE, Qt::AlignHCenter);
            glg->addWidget(covc->m_allMapsCheckBox, row, COLUMN_ALL_MAPS, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapRowOrColumnIndexSpinBox, row, COLUMN_MAP_INDEX, Qt::AlignHCenter);
            glg->addWidget(covc->m_mapRowOrColumnNameComboBox, row, COLUMN_MAP_NAME);
        }
    }
    else {
        int32_t columnCounter(0);
        static const int COLUMN_ONE   = columnCounter++;
        static const int COLUMN_TWO   = columnCounter++;
        static const int COLUMN_THREE   = columnCounter++;

        for (int32_t i = 0; i < columnCounter; i++) {
            gridLayout->setColumnStretch(i, 0);
        }
        gridLayout->setColumnStretch(COLUMN_THREE, 100);
        gridLayout->setVerticalSpacing(0);

        const bool showTitlesFlag(true);
        if (showTitlesFlag) {
            m_pointLabel = new QLabel("Point");
            
            const int titleRow(gridLayout->rowCount());
            gridLayout->addWidget(new QLabel("Settings"),
                                  titleRow, COLUMN_ONE, Qt::AlignHCenter);
            gridLayout->addWidget(m_pointLabel,
                                  titleRow, COLUMN_TWO, Qt::AlignHCenter);
            gridLayout->addWidget(new QLabel("Yoke / File / Map"),
                                  titleRow, COLUMN_THREE, Qt::AlignLeft);
        }

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

            QWidget* topLeftWidget = new QWidget();
            QHBoxLayout* topLeftLayout = new QHBoxLayout(topLeftWidget);
            topLeftLayout->setContentsMargins(0, 0, 0, 0);
            topLeftLayout->setSpacing(3);
            topLeftLayout->addWidget(covc->m_enabledCheckBox);
            topLeftLayout->addWidget(covc->m_lineSeriesLoadingEnabledCheckBox);
            topLeftLayout->addWidget(covc->m_lineLayerWidthSpinBox->getWidget());
            
            QWidget* topRightWidget = new QWidget();
            QHBoxLayout* topRightLayout = new QHBoxLayout(topRightWidget);
            topRightLayout->setContentsMargins(0, 0, 0, 0);
            topRightLayout->setSpacing(3);
            topRightLayout->addWidget(covc->m_mapRowOrColumnYokingGroupComboBox->getWidget(), 0);
            topRightLayout->addWidget(covc->m_mapFileComboBox, 100);
            
            QWidget* bottomLeftWidget = new QWidget();
            QHBoxLayout* bottomLeftLayout = new QHBoxLayout(bottomLeftWidget);
            bottomLeftLayout->setContentsMargins(0, 0, 0, 0);
            bottomLeftLayout->setSpacing(3);
            bottomLeftLayout->addWidget(covc->m_settingsToolButton);
            bottomLeftLayout->addWidget(covc->m_colorBarToolButton);
            bottomLeftLayout->addWidget(covc->m_constructionToolButton);
            bottomLeftLayout->addWidget(covc->m_matrixTriangularViewModeToolButton);
            bottomLeftLayout->addWidget(covc->m_axisLocationToolButton);
            bottomLeftLayout->addWidget(covc->m_lineLayerColorToolButton);
            bottomLeftLayout->addWidget(covc->m_lineLayerToolTipOffsetToolButton);

            QWidget* bottomRightWidget = new QWidget();
            QHBoxLayout* bottomRightLayout = new QHBoxLayout(bottomRightWidget);
            bottomRightLayout->setContentsMargins(0, 0, 0, 0);
            bottomRightLayout->setSpacing(3);
            bottomRightLayout->addWidget(covc->m_allMapsCheckBox, 0);
            bottomRightLayout->addWidget(covc->m_mapRowOrColumnIndexSpinBox, 0);
            bottomRightLayout->addWidget(covc->m_mapRowOrColumnNameComboBox, 100);
            
            glg->addWidget(topLeftWidget, row, COLUMN_ONE, Qt::AlignLeft);
            glg->addWidget(covc->m_selectedPointCheckBox, row, COLUMN_TWO);
            glg->addWidget(topRightWidget, row, COLUMN_THREE);
            row++;
            glg->addWidget(bottomLeftWidget, row, COLUMN_ONE, Qt::AlignLeft);
            glg->addWidget(covc->m_selectedPointIndexSpinBox, row, COLUMN_TWO);
            glg->addWidget(bottomRightWidget, row, COLUMN_THREE);
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
        bool showAllMapsLabelFlag(false);
        bool showLoadLabelFlag(false);
        bool showLineWidthLabelFlag(false);
        bool showPointLabelFlag(false);
        switch (chartOverlaySet->getChartTwoDataType()) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                showAllMapsLabelFlag = true;
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                showLineWidthLabelFlag = true;
                showPointLabelFlag     = true;
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                showLoadLabelFlag = true;
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                break;
        }
        if (m_allMapsLabel != NULL) {
            m_allMapsLabel->setVisible(showAllMapsLabelFlag);
        }
        if (m_loadLabel != NULL) {
            m_loadLabel->setVisible(showLoadLabelFlag);
        }
        if (m_lineWidthLabel != NULL) {
            m_lineWidthLabel->setVisible(showLineWidthLabelFlag);
        }
        if (m_pointLabel != NULL) {
            m_pointLabel->setVisible(showPointLabelFlag);
        }

        ChartTwoOverlay* chartOverlay = NULL;
        if (chartOverlaySet != NULL) {
            chartOverlay = chartOverlaySet->getOverlay(i);
        }
        
        bool displayOverlay = (chartOverlay != NULL);
        if (i >= numberOfDisplayedOverlays) {
            displayOverlay = false;
        }
        if (displayOverlay) {
            CaretAssertVectorIndex(m_chartOverlayViewControllers, i);
            m_chartOverlayViewControllers[i]->updateViewController(chartOverlay);
            
        }
        CaretAssertVectorIndex(m_chartOverlayGridLayoutGroups, i);
        m_chartOverlayGridLayoutGroups[i]->setVisible(displayOverlay);
    }
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
