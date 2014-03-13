
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

#define __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartableMatrixFileSelectionViewController.h"
#undef __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QButtonGroup>
#include <QRadioButton>
#include <QVariant>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixFileSelectionModel.h"
#include "ChartableMatrixInterface.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"

using namespace caret;
    
/**
 * \class caret::ChartableMatrixFileSelectionViewController 
 * \brief View controller for use with ChartableMatrixFileSelectionModel
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of the browser window.
 * @param parent
 *    Parent of this object.
 */
ChartableMatrixFileSelectionViewController::ChartableMatrixFileSelectionViewController(const int32_t browserWindowIndex,
                                                                                       QWidget* parent)
: WuQWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_selectionModel = NULL;
    
    m_radioButtonWidget = new QWidget;
    m_radioButtonLayout = new QVBoxLayout(m_radioButtonWidget);
    
    m_radioButtonGroup = new QButtonGroup(this);
    QObject::connect(m_radioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(radioButtonClicked(QAbstractButton*)));
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartableMatrixFileSelectionViewController::~ChartableMatrixFileSelectionViewController()
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
ChartableMatrixFileSelectionViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)
            || uiEvent->isToolBoxUpdate()) {
            updateViewController();
            uiEvent->setEventProcessed();
        }
    }
}
/**
 * @return the actual widget for adding to a layout.
 */
QWidget*
ChartableMatrixFileSelectionViewController::getWidget()
{
    return m_radioButtonWidget;
}

/**
 * Update the view controller.
 */
void
ChartableMatrixFileSelectionViewController::updateViewController()
{
    updateViewController(m_selectionModel);
}

/**
 * Update the view controller with the given selection model.
 *
 * @param selectionModel
 *    The new selection model for this view controller.
 */
void
ChartableMatrixFileSelectionViewController::updateViewController(ChartableMatrixFileSelectionModel* selectionModel)
{
    m_selectionModel = selectionModel;
    
    int32_t numberOfChartFiles = 0;
    
    if (m_selectionModel != NULL) {
        std::vector<ChartableMatrixInterface*> chartFiles = m_selectionModel->getAvailableFiles();
        numberOfChartFiles = static_cast<int32_t>(chartFiles.size());
        
        ChartableMatrixInterface* selectedFile = m_selectionModel->getSelectedFile();
        int32_t selectedFileIndex = -1;
        
        for (int32_t i = 0; i < numberOfChartFiles; i++) {
            QRadioButton* radioButton = NULL;
            
            if (i >= static_cast<int32_t>(m_radioButtons.size())) {
                radioButton = new QRadioButton();
                m_radioButtons.push_back(radioButton);
                m_radioButtonLayout->addWidget(radioButton);
            }
            else {
                radioButton = m_radioButtons[i];
            }
            
            ChartableMatrixInterface* cf = chartFiles[i];
            radioButton->setText(cf->getCaretMappableDataFile()->getFileName());
            radioButton->setProperty(FILE_POINTER_PROPERTY_NAME.toAscii().constData(),
                                     qVariantFromValue((void*)cf));
            if (cf == selectedFile) {
                selectedFileIndex = i;
            }
        }
        
        if (selectedFileIndex < 0) {
            selectedFileIndex = 0;
        }
        
        if (selectedFileIndex < numberOfChartFiles) {
            m_radioButtonGroup->blockSignals(true);
            m_radioButtons[selectedFileIndex]->setChecked(true);
            m_radioButtonGroup->blockSignals(false);
        }
    }
    
    const int32_t numberOfRadioButtons = static_cast<int32_t>(m_radioButtons.size());
    for (int32_t i = 0; i < numberOfRadioButtons; i++) {
        bool showIt = false;
        if (i < numberOfChartFiles) {
            showIt = true;
        }
        
        m_radioButtons[i]->setVisible(showIt);
    }
}

/**
 * Gets called when a radio button is selected.
 *
 * @param abstractButton
 *    Button the was selected.
 */
void
ChartableMatrixFileSelectionViewController::radioButtonClicked(QAbstractButton* abstractButton)
{
    if (abstractButton != NULL) {
        QRadioButton* radioButton = qobject_cast<QRadioButton*>(abstractButton);
        CaretAssert(radioButton);
        void* ptr = radioButton->property(FILE_POINTER_PROPERTY_NAME.toAscii().constData()).value<void*>();
        ChartableMatrixInterface* chartFile = (ChartableMatrixInterface*)ptr;
        if (m_selectionModel != NULL) {
            m_selectionModel->setSelectedFile(chartFile);
        }
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    
}
