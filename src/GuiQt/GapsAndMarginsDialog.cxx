
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

#define __GAPS_AND_MARGINS_DIALOG_DECLARE__
#include "GapsAndMarginsDialog.h"
#undef __GAPS_AND_MARGINS_DIALOG_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QSignalMapper>

#include "Brain.h"
#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GapsAndMargins.h"
#include "GuiManager.h"
#include "WuQGridLayoutGroup.h"

using namespace caret;


    
/**
 * \class caret::GapsAndMarginsDialog 
 * \brief Dialog for adjustment of tab margins.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
GapsAndMarginsDialog::GapsAndMarginsDialog(QWidget* parent)
: WuQDialogNonModal("Gaps and Margins",
                    parent)
{
    setApplyButtonText("");
 
    QWidget* gapsWidget = createGapsWidget();
    
    QWidget* marginsWidget = createMarginsWidget();
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(gapsWidget, 0, Qt::AlignHCenter);
    dialogLayout->addWidget(marginsWidget);
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_AS_NEEDED);

    updateDialog();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
GapsAndMarginsDialog::~GapsAndMarginsDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return Create the gaps widget.
 */
QWidget*
GapsAndMarginsDialog::createGapsWidget()
{
    QLabel* surfaceLabel    = new QLabel("Surface");
    QLabel* volumeLabel     = new QLabel("Volume");
    QLabel* horizontalLabel = new QLabel("Horizontal");
    QLabel* verticalLabel   = new QLabel("Vertical");
    QLabel* scaleProportionatelyLabel = new QLabel("Scale\nProportionately");
    scaleProportionatelyLabel->setAlignment(Qt::AlignCenter);
    
    m_surfaceMontageHorizontalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_surfaceMontageHorizontalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceMontageGapChanged()));
    
    m_surfaceMontageVerticalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_surfaceMontageVerticalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceMontageGapChanged()));
    
    m_surfaceMontageScaleProportionatelyCheckBox = new QCheckBox("");
    m_surfaceMontageScaleProportionatelyCheckBox->setToolTip("When checked, horizontal gap will be same size as vertical gap in graphics region");
    QObject::connect(m_surfaceMontageScaleProportionatelyCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(surfaceMontageScaleProportionatelyCheckBoxClicked()));
    
    m_volumeMontageHorizontalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_volumeMontageHorizontalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeMontageGapChanged()));
    
    m_volumeMontageVerticalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_volumeMontageVerticalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeMontageGapChanged()));
    
    m_volumeMontageScaleProportionatelyCheckBox = new QCheckBox("");
    m_volumeMontageScaleProportionatelyCheckBox->setToolTip("When checked, horizontal gap will be same size as vertical gap in graphics region");
    QObject::connect(m_volumeMontageScaleProportionatelyCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(volumeMontageScaleProportionatelyCheckBoxClicked()));
    
    const int COLUMN_LABEL      = 0;
    const int COLUMN_HORIZONTAL = 1;
    const int COLUMN_VERTICAL   = 2;
    const int COLUMN_SCALE      = 3;
    QWidget* widget = new QGroupBox("Montage Gaps");
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(horizontalLabel,
                          row, COLUMN_HORIZONTAL, Qt::AlignHCenter);
    gridLayout->addWidget(verticalLabel,
                          row, COLUMN_VERTICAL, Qt::AlignHCenter);
    gridLayout->addWidget(scaleProportionatelyLabel,
                          row, COLUMN_SCALE, Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(surfaceLabel,
                          row, COLUMN_LABEL);
    gridLayout->addWidget(m_surfaceMontageHorizontalGapSpinBox,
                          row, COLUMN_HORIZONTAL);
    gridLayout->addWidget(m_surfaceMontageVerticalGapSpinBox,
                          row, COLUMN_VERTICAL);
    gridLayout->addWidget(m_surfaceMontageScaleProportionatelyCheckBox,
                          row, COLUMN_SCALE, Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(volumeLabel,
                          row, COLUMN_LABEL);
    gridLayout->addWidget(m_volumeMontageHorizontalGapSpinBox,
                          row, COLUMN_HORIZONTAL);
    gridLayout->addWidget(m_volumeMontageVerticalGapSpinBox,
                          row, COLUMN_VERTICAL);
    gridLayout->addWidget(m_volumeMontageScaleProportionatelyCheckBox,
                          row, COLUMN_SCALE, Qt::AlignHCenter);
    row++;
    
    widget->setSizePolicy(QSizePolicy::Fixed,
                          QSizePolicy::Fixed);
    
    return widget;
}

/**
 * @return Create the margins widget.
 */
QWidget*
GapsAndMarginsDialog::createMarginsWidget()
{
    m_tabIndexSignalMapper = new QSignalMapper(this);
    QObject::connect(m_tabIndexSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(tabMarginChanged(int)));
    
    int32_t columnCounter = 0;
    const int32_t COLUMN_LABEL  = columnCounter++;
    const int32_t COLUMN_LEFT   = columnCounter++;
    const int32_t COLUMN_RIGHT  = columnCounter++;
    const int32_t COLUMN_BOTTOM = columnCounter++;
    const int32_t COLUMN_TOP    = columnCounter++;
    
    m_leftMarginCheckBox = new QCheckBox(" "); //"Left");
    QObject::connect(m_leftMarginCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(leftTabMarginCheckBoxClicked()));
    m_leftMarginCheckBox->setToolTip("When checked, tab 1 left margin is applied to ALL tabs");
    
    m_rightMarginCheckBox = new QCheckBox(" "); //"Right");
    QObject::connect(m_rightMarginCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(rightTabMarginCheckBoxClicked()));
    m_rightMarginCheckBox->setToolTip("When checked, tab 1 right margin is applied to ALL tabs");
    
    m_bottomMarginCheckBox = new QCheckBox(" "); //"Bottom");
    QObject::connect(m_bottomMarginCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(bottomTabMarginCheckBoxClicked()));
    m_bottomMarginCheckBox->setToolTip("When checked, tab 1 bottom margin applied to ALL tabs");
    
    m_topMarginCheckBox = new QCheckBox(" "); //"Top");
    QObject::connect(m_topMarginCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(topTabMarginCheckBoxClicked()));
    m_topMarginCheckBox->setToolTip("When checked, tab 1 top margin is applied to ALL tabs");
    
    QWidget* tabsWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(tabsWidget);
    
    const int32_t titlesRow = gridLayout->rowCount();
    //gridLayout->addWidget(new QLabel("Tab"), titlesRow, COLUMN_LABEL,  Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Left"), titlesRow, COLUMN_LEFT,  Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Right"), titlesRow, COLUMN_RIGHT,  Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Bottom"), titlesRow, COLUMN_BOTTOM,  Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Top"), titlesRow, COLUMN_TOP,  Qt::AlignHCenter);
    
    const int32_t applyToAllRow = gridLayout->rowCount();
    gridLayout->addWidget(new QLabel("Apply tab 1\nto all tabs"), applyToAllRow, COLUMN_LABEL, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(m_leftMarginCheckBox,   applyToAllRow, COLUMN_LEFT,   Qt::AlignHCenter);
    gridLayout->addWidget(m_rightMarginCheckBox,  applyToAllRow, COLUMN_RIGHT,  Qt::AlignHCenter);
    gridLayout->addWidget(m_bottomMarginCheckBox, applyToAllRow, COLUMN_BOTTOM, Qt::AlignHCenter);
    gridLayout->addWidget(m_topMarginCheckBox,    applyToAllRow, COLUMN_TOP,    Qt::AlignHCenter);
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        QLabel* tabLabel = new QLabel("Tab "
                                      + QString::number(iTab + 1));
        m_tabNumberLabels.push_back(tabLabel);
        
        QDoubleSpinBox* leftMarginSpinBox = createPercentageSpinBox();
        m_leftMarginSpinBoxes.push_back(leftMarginSpinBox);
        QObject::connect(leftMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(leftMarginSpinBox, iTab);
        
        QDoubleSpinBox* rightMarginSpinBox = createPercentageSpinBox();
        m_rightMarginSpinBoxes.push_back(rightMarginSpinBox);
        QObject::connect(rightMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(rightMarginSpinBox, iTab);
        
        QDoubleSpinBox* bottomMarginSpinBox = createPercentageSpinBox();
        m_bottomMarginSpinBoxes.push_back(bottomMarginSpinBox);
        QObject::connect(bottomMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(bottomMarginSpinBox, iTab);
        
        QDoubleSpinBox* topMarginSpinBox = createPercentageSpinBox();
        m_topMarginSpinBoxes.push_back(topMarginSpinBox);
        QObject::connect(topMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(topMarginSpinBox, iTab);
        
        const int32_t gridRow = gridLayout->rowCount();
        gridLayout->addWidget(tabLabel,            gridRow, COLUMN_LABEL, Qt::AlignRight);
        gridLayout->addWidget(leftMarginSpinBox,   gridRow, COLUMN_LEFT);
        gridLayout->addWidget(rightMarginSpinBox,  gridRow, COLUMN_RIGHT);
        gridLayout->addWidget(bottomMarginSpinBox, gridRow, COLUMN_BOTTOM);
        gridLayout->addWidget(topMarginSpinBox,    gridRow, COLUMN_TOP);
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(tabsWidget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    QWidget* widget = new QGroupBox("Tab Margins");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(scrollArea);
    
    return widget;
}

/**
 * @return Create a double spin box for percentage values.
 */
QDoubleSpinBox*
GapsAndMarginsDialog::createPercentageSpinBox()
{
    const double minValue   = 0.0;
    const double maxValue   = 100.0;
    const int decimals      = 1;
    const double singleStep = 0.1;
    
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setMinimum(minValue);
    doubleSpinBox->setMaximum(maxValue);
    doubleSpinBox->setSingleStep(singleStep);
    doubleSpinBox->setDecimals(decimals);
    doubleSpinBox->setKeyboardTracking(false);
    doubleSpinBox->setSuffix("%");
    
    return doubleSpinBox;
}


/**
 * Update the content of the dialog.
 */
void
GapsAndMarginsDialog::updateDialog()
{
//    EventBrowserTabGetAll allTabsEvent;
//    EventManager::get()->sendEvent(allTabsEvent.getPointer());
//    
//    const std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
//    
//    BrowserTabContent* nullValue = NULL;
//    std::vector<BrowserTabContent*> allTabsContent(100,
//                                                   nullValue);  // error on linux32    NULL);
//    const int32_t numValidTabs = allTabsEvent.getNumberOfBrowserTabs();
//    for (int32_t iValid = 0; iValid < numValidTabs; iValid++) {
//        BrowserTabContent* tabContent = allTabsEvent.getBrowserTab(iValid);
//        CaretAssert(tabContent);
//        
//        const int32_t tabIndex = tabContent->getTabNumber();
//        CaretAssertVectorIndex(allTabsContent, tabIndex);
//        allTabsContent[tabIndex] = tabContent;
//    }
    
//    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
//    const bool leftAllFlag   = gapsAndMargins->isTabMarginLeftAllSelected();
//    const bool rightAllFlag  = gapsAndMargins->isTabMarginRightAllSelected();
//    const bool bottomAllFlag = gapsAndMargins->isTabMarginBottomAllSelected();
//    const bool topAllFlag    = gapsAndMargins->isTabMarginTopAllSelected();
//    
//    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
//        CaretAssertVectorIndex(allTabsContent, iTab);
//        BrowserTabContent* tabContent = allTabsContent[iTab];
//        const bool valid = (tabContent != NULL);
//        
//        CaretAssertVectorIndex(m_tabNumberLabels, iTab);
//        m_tabNumberLabels[iTab]->setEnabled(valid);
//        
//        CaretAssertVectorIndex(m_leftMarginSpinBoxes, iTab);
//        m_leftMarginSpinBoxes[iTab]->setEnabled(valid);
//        
//        CaretAssertVectorIndex(m_rightMarginSpinBoxes, iTab);
//        m_rightMarginSpinBoxes[iTab]->setEnabled(valid);
//        
//        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, iTab);
//        m_bottomMarginSpinBoxes[iTab]->setEnabled(valid);
//        
//        CaretAssertVectorIndex(m_topMarginSpinBoxes, iTab);
//        m_topMarginSpinBoxes[iTab]->setEnabled(valid);
//        
////        if (iTab <= maximumValidTabIndex) {
//            const float leftMargin   = gapsAndMargins->getTabMarginLeft(iTab);
//            const float rightMargin  = gapsAndMargins->getTabMarginRight(iTab);
//            const float bottomMargin = gapsAndMargins->getTabMarginBottom(iTab);
//            const float topMargin    = gapsAndMargins->getTabMarginTop(iTab);
//
//            m_leftMarginSpinBoxes[iTab]->blockSignals(true);
//            m_leftMarginSpinBoxes[iTab]->setValue(leftMargin);
//            m_leftMarginSpinBoxes[iTab]->blockSignals(false);
//            
//            m_rightMarginSpinBoxes[iTab]->blockSignals(true);
//            m_rightMarginSpinBoxes[iTab]->setValue(rightMargin);
//            m_rightMarginSpinBoxes[iTab]->blockSignals(false);
//            
//            m_bottomMarginSpinBoxes[iTab]->blockSignals(true);
//            m_bottomMarginSpinBoxes[iTab]->setValue(bottomMargin);
//            m_bottomMarginSpinBoxes[iTab]->blockSignals(false);
//            
//            m_topMarginSpinBoxes[iTab]->blockSignals(true);
//            m_topMarginSpinBoxes[iTab]->setValue(topMargin);
//            m_topMarginSpinBoxes[iTab]->blockSignals(false);
////        }
//    }
    
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();

    m_leftMarginCheckBox->setChecked(gapsAndMargins->isTabMarginLeftAllSelected());
    m_rightMarginCheckBox->setChecked(gapsAndMargins->isTabMarginRightAllSelected());
    m_bottomMarginCheckBox->setChecked(gapsAndMargins->isTabMarginBottomAllSelected());
    m_topMarginCheckBox->setChecked(gapsAndMargins->isTabMarginTopAllSelected());
    
    m_surfaceMontageHorizontalGapSpinBox->blockSignals(true);
    m_surfaceMontageHorizontalGapSpinBox->setValue(gapsAndMargins->getSurfaceMontageHorizontalGap());
    m_surfaceMontageHorizontalGapSpinBox->blockSignals(false);
    
    m_surfaceMontageVerticalGapSpinBox->blockSignals(true);
    m_surfaceMontageVerticalGapSpinBox->setValue(gapsAndMargins->getSurfaceMontageVerticalGap());
    m_surfaceMontageVerticalGapSpinBox->blockSignals(false);
    
    m_volumeMontageHorizontalGapSpinBox->blockSignals(true);
    m_volumeMontageHorizontalGapSpinBox->setValue(gapsAndMargins->getVolumeMontageHorizontalGap());
    m_volumeMontageHorizontalGapSpinBox->blockSignals(false);
    
    m_volumeMontageVerticalGapSpinBox->blockSignals(true);
    m_volumeMontageVerticalGapSpinBox->setValue(gapsAndMargins->getVolumeMontageVerticalGap());
    m_volumeMontageVerticalGapSpinBox->blockSignals(false);
    
    m_surfaceMontageScaleProportionatelyCheckBox->setChecked(gapsAndMargins->isSurfaceMontageScaleProportionatelySelected());
    m_volumeMontageScaleProportionatelyCheckBox->setChecked(gapsAndMargins->isVolumeMontageScaleProportionatelySelected());
    
    updateMarginSpinBoxes();
    
    enableDisableHorizontalMontageSpinBoxes();
}

/**
 * Update the margin spin boxes.
 */
void
GapsAndMarginsDialog::updateMarginSpinBoxes()
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
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    const bool leftAllFlag   = gapsAndMargins->isTabMarginLeftAllSelected();
    const bool rightAllFlag  = gapsAndMargins->isTabMarginRightAllSelected();
    const bool bottomAllFlag = gapsAndMargins->isTabMarginBottomAllSelected();
    const bool topAllFlag    = gapsAndMargins->isTabMarginTopAllSelected();
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
//        CaretAssertVectorIndex(allTabsContent, iTab);
//        BrowserTabContent* tabContent = allTabsContent[iTab];
//        const bool valid = (tabContent != NULL);
        
        bool leftValid = true;
        if (leftAllFlag) {
            if (iTab > 0) {
                leftValid = false;
            }
        }
        
        bool rightValid = true;
        if (rightAllFlag) {
            if (iTab > 0) {
                rightValid = false;
            }
        }
        
        bool bottomValid = true;
        if (bottomAllFlag) {
            if (iTab > 0) {
                bottomValid = false;
            }
        }
        
        bool topValid = true;
        if (topAllFlag) {
            if (iTab > 0) {
                topValid = false;
            }
        }
        
        CaretAssertVectorIndex(m_tabNumberLabels, iTab);
        m_tabNumberLabels[iTab]->setEnabled(true);
        
        CaretAssertVectorIndex(m_leftMarginSpinBoxes, iTab);
        m_leftMarginSpinBoxes[iTab]->setEnabled(leftValid);
        
        CaretAssertVectorIndex(m_rightMarginSpinBoxes, iTab);
        m_rightMarginSpinBoxes[iTab]->setEnabled(rightValid);
        
        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, iTab);
        m_bottomMarginSpinBoxes[iTab]->setEnabled(bottomValid);
        
        CaretAssertVectorIndex(m_topMarginSpinBoxes, iTab);
        m_topMarginSpinBoxes[iTab]->setEnabled(topValid);
        
        const float leftMargin   = gapsAndMargins->getTabMarginLeft(iTab);
        const float rightMargin  = gapsAndMargins->getTabMarginRight(iTab);
        const float bottomMargin = gapsAndMargins->getTabMarginBottom(iTab);
        const float topMargin    = gapsAndMargins->getTabMarginTop(iTab);
        
        m_leftMarginSpinBoxes[iTab]->blockSignals(true);
        m_leftMarginSpinBoxes[iTab]->setValue(leftMargin);
        m_leftMarginSpinBoxes[iTab]->blockSignals(false);
        
        m_rightMarginSpinBoxes[iTab]->blockSignals(true);
        m_rightMarginSpinBoxes[iTab]->setValue(rightMargin);
        m_rightMarginSpinBoxes[iTab]->blockSignals(false);
        
        m_bottomMarginSpinBoxes[iTab]->blockSignals(true);
        m_bottomMarginSpinBoxes[iTab]->setValue(bottomMargin);
        m_bottomMarginSpinBoxes[iTab]->blockSignals(false);
        
        m_topMarginSpinBoxes[iTab]->blockSignals(true);
        m_topMarginSpinBoxes[iTab]->setValue(topMargin);
        m_topMarginSpinBoxes[iTab]->blockSignals(false);
    }
}


/**
 * Enable/disable the horizontal montage spin boxes
 */
void
GapsAndMarginsDialog::enableDisableHorizontalMontageSpinBoxes()
{
    /*
     * A horizontal gap spin box is disabled when scale proportionately is checked.
     */
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    m_surfaceMontageHorizontalGapSpinBox->setEnabled( ! gapsAndMargins->isSurfaceMontageScaleProportionatelySelected());
    m_volumeMontageHorizontalGapSpinBox->setEnabled( ! gapsAndMargins->isVolumeMontageScaleProportionatelySelected());
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GapsAndMarginsDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* eventUpdate = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUpdate);

        updateDialog();
        
        eventUpdate->setEventProcessed();
    }
}

/**
 * Gets called when a tab's margin is changed.
 *
 * @param tabIndex
 *    Index of the tab.
 */
void
GapsAndMarginsDialog::tabMarginChanged(int tabIndex)
{
    EventBrowserTabGet getTabEvent(tabIndex);
    EventManager::get()->sendEvent(getTabEvent.getPointer());
    
    BrowserTabContent* tabContent = getTabEvent.getBrowserTab();
    if (tabContent != NULL) {
        GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();

        CaretAssertVectorIndex(m_leftMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_rightMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, tabIndex);
        CaretAssertVectorIndex(m_topMarginSpinBoxes, tabIndex);
        
        gapsAndMargins->setTabMarginLeft(tabIndex,
                                         m_leftMarginSpinBoxes[tabIndex]->value());
        gapsAndMargins->setTabMarginRight(tabIndex,
                                         m_rightMarginSpinBoxes[tabIndex]->value());
        gapsAndMargins->setTabMarginBottom(tabIndex,
                                         m_bottomMarginSpinBoxes[tabIndex]->value());
        gapsAndMargins->setTabMarginTop(tabIndex,
                                         m_topMarginSpinBoxes[tabIndex]->value());

        /*
         * May need to update due to apply one to all or scale proportionately
         */
        updateMarginSpinBoxes();
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Gets called when right tab margin is changed.
 */
void
GapsAndMarginsDialog::rightTabMarginCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setTabMarginRightAllSelected(m_rightMarginCheckBox->isChecked());
    
    /*
     * Update dialog since "select all" will change all margins to the first margin value
     */
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when left tab margin is changed.
 */
void
GapsAndMarginsDialog::leftTabMarginCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setTabMarginLeftAllSelected(m_leftMarginCheckBox->isChecked());
    
    /*
     * Update dialog since "select all" will change all margins to the first margin value
     */
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when bottom tab margin is changed.
 */
void
GapsAndMarginsDialog::bottomTabMarginCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setTabMarginBottomAllSelected(m_bottomMarginCheckBox->isChecked());
    
    /*
     * Update dialog since "select all" will change all margins to the first margin value
     */
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when top tab margin is changed.
 */
void
GapsAndMarginsDialog::topTabMarginCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setTabMarginTopAllSelected(m_topMarginCheckBox->isChecked());
    
    /*
     * Update dialog since "select all" will change all margins to the first margin value
     */
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when a surface montage gap is changed.
 */
void
GapsAndMarginsDialog::surfaceMontageGapChanged()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setSurfaceMontageHorizontalGap(m_surfaceMontageHorizontalGapSpinBox->value());
    gapsAndMargins->setSurfaceMontageVerticalGap(m_surfaceMontageVerticalGapSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when a volume montage gap is changed.
 */
void
GapsAndMarginsDialog::volumeMontageGapChanged()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setVolumeMontageHorizontalGap(m_volumeMontageHorizontalGapSpinBox->value());
    gapsAndMargins->setVolumeMontageVerticalGap(m_volumeMontageVerticalGapSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the surface montage scaled proportionately check box is clicked.
 */
void
GapsAndMarginsDialog::surfaceMontageScaleProportionatelyCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setSurfaceMontageScaleProportionatelySelected(m_surfaceMontageScaleProportionatelyCheckBox->isChecked());
    
    enableDisableHorizontalMontageSpinBoxes();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the volume montage scaled proportionately check box is clicked.
 */
void
GapsAndMarginsDialog::volumeMontageScaleProportionatelyCheckBoxClicked()
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setVolumeMontageScaleProportionatelySelected(m_volumeMontageScaleProportionatelyCheckBox->isChecked());
    
    enableDisableHorizontalMontageSpinBoxes();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}




