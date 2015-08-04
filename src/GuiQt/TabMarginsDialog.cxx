
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __TAB_MARGINS_DIALOG_DECLARE__
#include "TabMarginsDialog.h"
#undef __TAB_MARGINS_DIALOG_DECLARE__

#include <QLabel>
#include <QGridLayout>
#include <QSignalMapper>
#include <QSpinBox>

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "Margin.h"
#include "WuQGridLayoutGroup.h"

using namespace caret;


    
/**
 * \class caret::TabMarginsDialog 
 * \brief Dialog for adjustment of tab margins.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
TabMarginsDialog::TabMarginsDialog(QWidget* parent)
: WuQDialogNonModal("Tab Margins",
                    parent)
{
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    
    m_tabIndexSignalMapper = new QSignalMapper(this);
    QObject::connect(m_tabIndexSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(tabMarginChanged(int)));
    
    int32_t columnCounter = 0;
    const int32_t COLUMN_LABEL = columnCounter++;
    const int32_t COLUMN_LEFT   = columnCounter++;
    const int32_t COLUMN_RIGHT  = columnCounter++;
    const int32_t COLUMN_BOTTOM = columnCounter++;
    const int32_t COLUMN_TOP    = columnCounter++;
    
    gridLayout->addWidget(new QLabel("Tab"),    0, COLUMN_LABEL);
    gridLayout->addWidget(new QLabel("Left"),   0, COLUMN_LEFT);
    gridLayout->addWidget(new QLabel("Right"),  0, COLUMN_RIGHT);
    gridLayout->addWidget(new QLabel("Bottom"), 0, COLUMN_BOTTOM);
    gridLayout->addWidget(new QLabel("Top"),    0, COLUMN_TOP);
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        QLabel* tabLabel = new QLabel(QString::number(iTab + 1));
        m_tabNumberLabels.push_back(tabLabel);
        
        const int maxValue = 10000;
        QSpinBox* leftMarginSpinBox = new QSpinBox();
        leftMarginSpinBox->setRange(0, maxValue);
        leftMarginSpinBox->setSingleStep(1);
        m_leftMarginSpinBoxes.push_back(leftMarginSpinBox);
        QObject::connect(leftMarginSpinBox, SIGNAL(valueChanged(int)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(leftMarginSpinBox, iTab);
        

        QSpinBox* rightMarginSpinBox = new QSpinBox();
        rightMarginSpinBox->setRange(0, maxValue);
        rightMarginSpinBox->setSingleStep(1);
        m_rightMarginSpinBoxes.push_back(rightMarginSpinBox);
        QObject::connect(rightMarginSpinBox, SIGNAL(valueChanged(int)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(rightMarginSpinBox, iTab);

        QSpinBox* bottomMarginSpinBox = new QSpinBox();
        bottomMarginSpinBox->setRange(0, maxValue);
        bottomMarginSpinBox->setSingleStep(1);
        m_bottomMarginSpinBoxes.push_back(bottomMarginSpinBox);
        QObject::connect(bottomMarginSpinBox, SIGNAL(valueChanged(int)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(bottomMarginSpinBox, iTab);

        QSpinBox* topMarginSpinBox = new QSpinBox();
        topMarginSpinBox->setRange(0, maxValue);
        topMarginSpinBox->setSingleStep(1);
        m_topMarginSpinBoxes.push_back(topMarginSpinBox);
        QObject::connect(topMarginSpinBox, SIGNAL(valueChanged(int)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(topMarginSpinBox, iTab);

        const int32_t gridRow = iTab + 1;
        gridLayout->addWidget(tabLabel,            gridRow, COLUMN_LABEL);
        gridLayout->addWidget(leftMarginSpinBox,   gridRow, COLUMN_LEFT);
        gridLayout->addWidget(rightMarginSpinBox,  gridRow, COLUMN_RIGHT);
        gridLayout->addWidget(bottomMarginSpinBox, gridRow, COLUMN_BOTTOM);
        gridLayout->addWidget(topMarginSpinBox,    gridRow, COLUMN_TOP);
    }
    
    setApplyButtonText("");
    
    setCentralWidget(widget,
                     SCROLL_AREA_AS_NEEDED);

    updateDialog();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
TabMarginsDialog::~TabMarginsDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the content of the dialog.
 */
void
TabMarginsDialog::updateDialog()
{
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    
    const std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
    
    BrowserTabContent* nullValue = NULL;
    std::vector<BrowserTabContent*> allTabsContent(100,
                                                   nullValue);  // error on linux32    NULL);
    const int32_t numValidTabs = allTabsEvent.getNumberOfBrowserTabs();
    for (int32_t iValid = 0; iValid < numValidTabs; iValid++) {
        BrowserTabContent* tabContent = allTabsEvent.getBrowserTab(iValid);
        CaretAssert(tabContent);
        
        const int32_t tabIndex = tabContent->getTabNumber();
        CaretAssertVectorIndex(allTabsContent, tabIndex);
        allTabsContent[tabIndex] = tabContent;
    }
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        CaretAssertVectorIndex(allTabsContent, iTab);
        BrowserTabContent* tabContent = allTabsContent[iTab];
        const bool valid = (tabContent != NULL);
        
        CaretAssertVectorIndex(m_tabNumberLabels, iTab);
        m_tabNumberLabels[iTab]->setVisible(valid);
        
        CaretAssertVectorIndex(m_leftMarginSpinBoxes, iTab);
        m_leftMarginSpinBoxes[iTab]->setVisible(valid);
        
        CaretAssertVectorIndex(m_rightMarginSpinBoxes, iTab);
        m_rightMarginSpinBoxes[iTab]->setVisible(valid);
        
        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, iTab);
        m_bottomMarginSpinBoxes[iTab]->setVisible(valid);
        
        CaretAssertVectorIndex(m_topMarginSpinBoxes, iTab);
        m_topMarginSpinBoxes[iTab]->setVisible(valid);
        
        if (valid) {
            const Margin* margin = tabContent->getMargin();
            int32_t left, right, bottom, top;
            margin->getMargins(left, right, bottom, top);

            m_leftMarginSpinBoxes[iTab]->blockSignals(true);
            m_leftMarginSpinBoxes[iTab]->setValue(left);
            m_leftMarginSpinBoxes[iTab]->blockSignals(false);
            
            m_rightMarginSpinBoxes[iTab]->blockSignals(true);
            m_rightMarginSpinBoxes[iTab]->setValue(right);
            m_rightMarginSpinBoxes[iTab]->blockSignals(false);
            
            m_bottomMarginSpinBoxes[iTab]->blockSignals(true);
            m_bottomMarginSpinBoxes[iTab]->setValue(bottom);
            m_bottomMarginSpinBoxes[iTab]->blockSignals(false);
            
            m_topMarginSpinBoxes[iTab]->blockSignals(true);
            m_topMarginSpinBoxes[iTab]->setValue(top);
            m_topMarginSpinBoxes[iTab]->blockSignals(false);
        }
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
TabMarginsDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* eventUpdate = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUpdate);

        updateDialog();
        
        event->setEventProcessed();
    }
}

/**
 * Gets called when a tab's margin is changed.
 *
 * @param tabIndex
 *    Index of the tab.
 */
void
TabMarginsDialog::tabMarginChanged(int tabIndex)
{
    EventBrowserTabGet getTabEvent(tabIndex);
    EventManager::get()->sendEvent(getTabEvent.getPointer());
    
    BrowserTabContent* tabContent = getTabEvent.getBrowserTab();
    if (tabContent != NULL) {
        CaretAssertVectorIndex(m_leftMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_rightMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_topMarginSpinBoxes, tabIndex);
        Margin* margin = tabContent->getMargin();
        margin->setMargins(m_leftMarginSpinBoxes[tabIndex]->value(),
                           m_rightMarginSpinBoxes[tabIndex]->value(),
                           m_bottomMarginSpinBoxes[tabIndex]->value(),
                           m_topMarginSpinBoxes[tabIndex]->value());
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


