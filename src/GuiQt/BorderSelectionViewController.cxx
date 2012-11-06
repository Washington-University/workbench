
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
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
//#include <QTabWidget>
#include <QToolButton>

#define __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "BorderSelectionViewController.h"
#undef __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "BorderDrawingTypeEnum.h"
#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesBorders.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQDataEntryDialog.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderSelectionViewController 
 * \brief Widget for controlling display of borders
 *
 * Widget for controlling the display of borders including
 * different display groups.
 */

/**
 * Constructor.
 */
BorderSelectionViewController::BorderSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_bordersDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_bordersDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(borderDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_bordersDisplayGroupComboBox->getWidget());
    groupLayout->addStretch(); 
    
    m_bordersDisplayCheckBox = new QCheckBox("Display Borders");
    QObject::connect(m_bordersDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
//    QTabWidget* tabWidget = new QTabWidget();
    WuQTabWidget* tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    tabWidget->addTab(attributesWidget, 
                      "Attributes");
    tabWidget->addTab(selectionWidget, 
                      "Selection");
    tabWidget->setCurrentWidget(attributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 2, 2);
    layout->addLayout(groupLayout);
    layout->addSpacing(10);
    layout->addWidget(m_bordersDisplayCheckBox);
    layout->addWidget(tabWidget->getWidget(), 0, Qt::AlignLeft);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    BorderSelectionViewController::allBorderSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
BorderSelectionViewController::~BorderSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    BorderSelectionViewController::allBorderSelectionViewControllers.erase(this);
}


QWidget* 
BorderSelectionViewController::createSelectionWidget()
{
    m_borderClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex);
//    QObject::connect(m_borderClassNameHierarchyViewController, SIGNAL(itemSelected(GroupAndNameHierarchySelectedItem*)),
//                     this, SLOT(bordersSelectionsChanged(GroupAndNameHierarchySelectedItem*)));
    
    return m_borderClassNameHierarchyViewController;
}

/**
 * @return The attributes widget.
 */
QWidget* 
BorderSelectionViewController::createAttributesWidget()
{
    m_bordersContralateralCheckBox = new QCheckBox("Contralateral");
    QObject::connect(m_bordersContralateralCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    std::vector<BorderDrawingTypeEnum::Enum> drawingTypeEnums;
    BorderDrawingTypeEnum::getAllEnums(drawingTypeEnums);
    const int32_t numDrawingTypeEnums = static_cast<int32_t>(drawingTypeEnums.size());
    
    QLabel* drawAsLabel = new QLabel("Draw As");
    m_drawTypeComboBox = new QComboBox(); 
    for (int32_t i = 0; i < numDrawingTypeEnums; i++) {
        BorderDrawingTypeEnum::Enum drawType = drawingTypeEnums[i];
        m_drawTypeComboBox->addItem(BorderDrawingTypeEnum::toGuiName(drawType),
                                    (int)drawType);
    }
    m_drawTypeComboBox->setToolTip("Select the drawing style of borders");
    QObject::connect(m_drawTypeComboBox, SIGNAL(activated(int)),
                     this, SLOT(processAttributesChanges()));
    
    float minLineWidth = 0;
    float maxLineWidth = 1000;
    //BrainOpenGL::getMinMaxLineWidth(minLineWidth,
    //                                maxLineWidth);
    
    QLabel* lineWidthLabel = new QLabel("Line Width");
    m_lineWidthSpinBox = new QDoubleSpinBox();
    m_lineWidthSpinBox->setFixedWidth(80);
    m_lineWidthSpinBox->setRange(minLineWidth,
                                 maxLineWidth);
    m_lineWidthSpinBox->setSingleStep(1.0);
    m_lineWidthSpinBox->setDecimals(1);
    m_lineWidthSpinBox->setToolTip("Adjust the width of borders drawn as lines.\n"
                                   "The maximum width is dependent upon the \n"
                                   "graphics system.  There is no maximum value\n"
                                   "for this control and the drawn width of the \n"
                                   "lines will stop increasing even though the\n"
                                   "value of this control is changing");
    QObject::connect(m_lineWidthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* pointSizeLabel = new QLabel("Symbol Size");
    m_pointSizeSpinBox = new QDoubleSpinBox();
    m_pointSizeSpinBox->setFixedWidth(80);
    m_pointSizeSpinBox->setRange(minLineWidth,
                                 maxLineWidth);
    m_pointSizeSpinBox->setSingleStep(1.0);
    m_pointSizeSpinBox->setDecimals(1);
    m_pointSizeSpinBox->setToolTip("Adjust the size of borders drawn as points");
    QObject::connect(m_pointSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_bordersContralateralCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    gridLayout->addWidget(drawAsLabel, row, 0);
    gridLayout->addWidget(m_drawTypeComboBox, row, 1);
    row++;
    gridLayout->addWidget(lineWidthLabel, row, 0);
    gridLayout->addWidget(m_lineWidthSpinBox, row, 1);
    row++;
    gridLayout->addWidget(pointSizeLabel, row, 0);
    gridLayout->addWidget(m_pointSizeSpinBox, row, 1);
    gridWidget->setSizePolicy(QSizePolicy::Fixed,
                              QSizePolicy::Fixed);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(gridWidget);
    layout->addStretch();
        
    return widget;
}

/**
 * Called when a widget on the attributes page has 
 * its value changed.
 */
void 
BorderSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesBorders* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesBorders();
    
    const int selectedDrawTypeIndex = m_drawTypeComboBox->currentIndex();
    const int drawTypeInteger = m_drawTypeComboBox->itemData(selectedDrawTypeIndex).toInt();
    const BorderDrawingTypeEnum::Enum selectedDrawingType = static_cast<BorderDrawingTypeEnum::Enum>(drawTypeInteger);

    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    dpb->setDisplayed(displayGroup,
                      browserTabIndex,
                      m_bordersDisplayCheckBox->isChecked());
    dpb->setContralateralDisplayed(displayGroup,
                                   browserTabIndex,
                                   m_bordersContralateralCheckBox->isChecked());

    dpb->setDrawingType(displayGroup,
                        browserTabIndex,
                        selectedDrawingType);
    dpb->setLineWidth(displayGroup,
                      browserTabIndex,
                      m_lineWidthSpinBox->value());
    dpb->setPointSize(displayGroup,
                      browserTabIndex,
                      m_pointSizeSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    updateOtherBorderViewControllers();
}

/**
 * Called when the border display group combo box is changed.
 */
void 
BorderSelectionViewController::borderDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    /*
     * Update selected display group in model.
     */
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    dsb->setDisplayGroupForTab(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateBorderViewController();
    
    /*
     * Apply the changes.
     */
    processBorderSelectionChanges();
}

/**
 * Called when the border selections are changed.
 * Updates border display information and redraws
 * graphics.
 */
void 
BorderSelectionViewController::bordersSelectionsChanged(GroupAndNameHierarchySelectedItem* /*selectedItem*/)
{
    processBorderSelectionChanges();
}

/**
 * Update the border selection widget.
 */
void 
BorderSelectionViewController::updateBorderViewController()
{
    setWindowTitle("Borders");
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dpb = brain->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    
    m_bordersDisplayGroupComboBox->setSelectedDisplayGroup(dpb->getDisplayGroupForTab(browserTabIndex));
    
    /*;
     * Get all of border files.
     */
    std::vector<BorderFile*> allBorderFiles;
    const int32_t numberOfBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t ibf= 0; ibf < numberOfBorderFiles; ibf++) {
        allBorderFiles.push_back(brain->getBorderFile(ibf));
    }
    
    /*
     * Update the class/name hierarchy
     */
    m_borderClassNameHierarchyViewController->updateContents(allBorderFiles,
                                                             displayGroup);

    std::vector<BorderDrawingTypeEnum::Enum> drawingTypeEnums;
    BorderDrawingTypeEnum::getAllEnums(drawingTypeEnums);
    const int32_t numDrawingTypeEnums = static_cast<int32_t>(drawingTypeEnums.size());
    
    m_bordersDisplayCheckBox->setChecked(dpb->isDisplayed(displayGroup,
                                                          browserTabIndex));
    m_bordersContralateralCheckBox->setChecked(dpb->isContralateralDisplayed(displayGroup,
                                                                             browserTabIndex));
    
    const BorderDrawingTypeEnum::Enum selectedDrawingType = dpb->getDrawingType(displayGroup,
                                                                                browserTabIndex);
    int32_t selectedDrawingTypeIndex = 0;
    
    for (int32_t i = 0; i < numDrawingTypeEnums; i++) {
        BorderDrawingTypeEnum::Enum drawType = drawingTypeEnums[i];
        if (drawType == selectedDrawingType) {
            selectedDrawingTypeIndex = i;
        }
    }
    m_drawTypeComboBox->setCurrentIndex(selectedDrawingTypeIndex);
    
    m_lineWidthSpinBox->blockSignals(true);
    m_lineWidthSpinBox->setValue(dpb->getLineWidth(displayGroup,
                                                   browserTabIndex));
    m_lineWidthSpinBox->blockSignals(false);
    
    m_pointSizeSpinBox->blockSignals(true);
    m_pointSizeSpinBox->setValue(dpb->getPointSize(displayGroup,
                                                   browserTabIndex));
    m_pointSizeSpinBox->blockSignals(false);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
BorderSelectionViewController::updateOtherBorderViewControllers()
{
    for (std::set<BorderSelectionViewController*>::iterator iter = BorderSelectionViewController::allBorderSelectionViewControllers.begin();
         iter != BorderSelectionViewController::allBorderSelectionViewControllers.end();
         iter++) {
        BorderSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateBorderViewController();
        }
    }
}

/**
 * Gets called when border selections are changed.
 */
void 
BorderSelectionViewController::processBorderSelectionChanges()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    dsb->setDisplayGroupForTab(browserTabIndex, 
                         m_bordersDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
BorderSelectionViewController::processSelectionChanges()
{
    updateOtherBorderViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BorderSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isBorderUpdate()
                || uiEvent->isToolBoxUpdate()) {
                doUpdate = true;
                uiEvent->setEventProcessed();
            }
        }
    }

    if (doUpdate) {
        updateBorderViewController();
    }
}


