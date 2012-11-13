
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

#define __FOCI_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "FociSelectionViewController.h"
#undef __FOCI_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesFoci.h"
#include "EnumComboBoxTemplate.h"
#include "FeatureColoringTypeEnum.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQDataEntryDialog.h"
#include "WuQTabWidget.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::FociSelectionViewController 
 * \brief Widget for controlling display of foci
 *
 * Widget for controlling the display of foci including
 * different display groups.
 */

/**
 * Constructor.
 */
FociSelectionViewController::FociSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_fociDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_fociDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(fociDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_fociDisplayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    m_fociDisplayCheckBox = new QCheckBox("Display Foci");
    m_fociDisplayCheckBox->setToolTip("Enable the display of foci");
    QObject::connect(m_fociDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    
    //QTabWidget* tabWidget = new QTabWidget();
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
    layout->addWidget(m_fociDisplayCheckBox);
    layout->addWidget(tabWidget->getWidget(), 0, Qt::AlignLeft);
    layout->addStretch();    
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    FociSelectionViewController::allFociSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
FociSelectionViewController::~FociSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    FociSelectionViewController::allFociSelectionViewControllers.erase(this);
}


QWidget* 
FociSelectionViewController::createSelectionWidget()
{
    m_fociClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex);
    
    return m_fociClassNameHierarchyViewController;
}

/**
 * @return The attributes widget.
 */
QWidget*
FociSelectionViewController::createAttributesWidget()
{
    m_fociContralateralCheckBox = new QCheckBox("Contralateral");
    m_fociContralateralCheckBox->setToolTip("Enable display of foci from contralateral brain structure");
    QObject::connect(m_fociContralateralCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    m_pasteOntoSurfaceCheckBox = new QCheckBox("Paste Onto Surface");
    m_pasteOntoSurfaceCheckBox->setToolTip("Place the foci onto the surface");
    QObject::connect(m_pasteOntoSurfaceCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* coloringLabel = new QLabel("Coloring");
    m_coloringTypeComboBox = new EnumComboBoxTemplate(this);
    m_coloringTypeComboBox->setup<FeatureColoringTypeEnum,
                                  FeatureColoringTypeEnum::Enum>();
    m_coloringTypeComboBox->getWidget()->setToolTip("Select the coloring assignment for foci");
    QObject::connect(m_coloringTypeComboBox, SIGNAL(itemSelected()),
                     this, SLOT(processAttributesChanges()));
    
    std::vector<FociDrawingTypeEnum::Enum> drawingTypeEnums;
    FociDrawingTypeEnum::getAllEnums(drawingTypeEnums);
    const int32_t numDrawingTypeEnums = static_cast<int32_t>(drawingTypeEnums.size());
    
    QLabel* drawAsLabel = new QLabel("Draw As");
    m_drawTypeComboBox = new QComboBox(); 
    for (int32_t i = 0; i < numDrawingTypeEnums; i++) {
        FociDrawingTypeEnum::Enum drawType = drawingTypeEnums[i];
        m_drawTypeComboBox->addItem(FociDrawingTypeEnum::toGuiName(drawType),
                                    (int)drawType);
    }
    m_drawTypeComboBox->setToolTip("Select the drawing style of foci");
    QObject::connect(m_drawTypeComboBox, SIGNAL(activated(int)),
                     this, SLOT(processAttributesChanges()));
    
    float minLineWidth = 0;
    float maxLineWidth = 1000;
    //BrainOpenGL::getMinMaxLineWidth(minLineWidth,
    //                                maxLineWidth);
        
    QLabel* pointSizeLabel = new QLabel("Symbol Size");
    m_sizeSpinBox = new QDoubleSpinBox();
    m_sizeSpinBox->setFixedWidth(80);
    m_sizeSpinBox->setRange(minLineWidth,
                                 maxLineWidth);
    m_sizeSpinBox->setSingleStep(1.0);
    m_sizeSpinBox->setDecimals(1);
    m_sizeSpinBox->setToolTip("Adjust the size of foci");
    QObject::connect(m_sizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_fociContralateralCheckBox, row, 0, 1, 2);
    row++;
    gridLayout->addWidget(m_pasteOntoSurfaceCheckBox, row, 0, 1, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    gridLayout->addWidget(coloringLabel, row, 0);
    gridLayout->addWidget(m_coloringTypeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(drawAsLabel, row, 0);
    gridLayout->addWidget(m_drawTypeComboBox , row, 1);
    row++;
    gridLayout->addWidget(pointSizeLabel, row, 0);
    gridLayout->addWidget(m_sizeSpinBox, row, 1);
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
FociSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesFoci* dpf = GuiManager::get()->getBrain()->getDisplayPropertiesFoci();
    
    const FeatureColoringTypeEnum::Enum selectedColoringType = m_coloringTypeComboBox->getSelectedItem<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>();

    const int selectedDrawTypeIndex = m_drawTypeComboBox->currentIndex();
    const int drawTypeInteger = m_drawTypeComboBox->itemData(selectedDrawTypeIndex).toInt();
    const FociDrawingTypeEnum::Enum selectedDrawingType = static_cast<FociDrawingTypeEnum::Enum>(drawTypeInteger);
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpf->getDisplayGroupForTab(browserTabIndex);
    
    dpf->setDisplayed(displayGroup,
                      browserTabIndex,
                      m_fociDisplayCheckBox->isChecked());
    dpf->setContralateralDisplayed(displayGroup,
                                   browserTabIndex,
                                   m_fociContralateralCheckBox->isChecked());
    dpf->setPasteOntoSurface(displayGroup,
                             browserTabIndex,
                             m_pasteOntoSurfaceCheckBox->isChecked());
    dpf->setColoringType(displayGroup,
                         browserTabIndex,
                         selectedColoringType);
    dpf->setFociSize(displayGroup,
                     browserTabIndex,
                     m_sizeSpinBox->value());
    dpf->setDrawingType(displayGroup,
                        browserTabIndex,
                        selectedDrawingType);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    updateOtherFociViewControllers();
}

/**
 * Called when the foci display group combo box is changed.
 */
void 
FociSelectionViewController::fociDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesFoci* dpf = brain->getDisplayPropertiesFoci();
    dpf->setDisplayGroupForTab(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateFociViewController();
    
    /*
     * Apply the changes.
     */
    processFociSelectionChanges();
}

/**
 * Update the foci widget.
 */
void 
FociSelectionViewController::updateFociViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFoci* dpf = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = dpf->getDisplayGroupForTab(browserTabIndex);
//    dpf->setDisplayGroupForTab(browserTabIndex, 
//                               m_fociDisplayGroupComboBox->getSelectedDisplayGroup());
    
    setWindowTitle("Foci");
    
    m_fociDisplayGroupComboBox->setSelectedDisplayGroup(dpf->getDisplayGroupForTab(browserTabIndex));
    
    /*;
     * Get all of foci files.
     */
    std::vector<FociFile*> allFociFiles;
    const int32_t numberOfFociFiles = brain->getNumberOfFociFiles();
    for (int32_t iff = 0; iff < numberOfFociFiles; iff++) {
        allFociFiles.push_back(brain->getFociFile(iff));
    }
    
    /*
     * Update the class/name hierarchy
     */
    m_fociClassNameHierarchyViewController->updateContents(allFociFiles,
                                                           displayGroup);
    
    
    std::vector<FociDrawingTypeEnum::Enum> drawingTypeEnums;
    FociDrawingTypeEnum::getAllEnums(drawingTypeEnums);
    const int32_t numDrawingTypeEnums = static_cast<int32_t>(drawingTypeEnums.size());
    
    m_fociDisplayCheckBox->setChecked(dpf->isDisplayed(displayGroup,
                                                       browserTabIndex));
    m_fociContralateralCheckBox->setChecked(dpf->isContralateralDisplayed(displayGroup,
                                                                          browserTabIndex));
    m_pasteOntoSurfaceCheckBox->setChecked(dpf->isPasteOntoSurface(displayGroup,
                                                                   browserTabIndex));
    
    m_coloringTypeComboBox->setSelectedItem<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>(dpf->getColoringType(displayGroup,
                                                                                                                         browserTabIndex));
    
    const FociDrawingTypeEnum::Enum selectedDrawingType = dpf->getDrawingType(displayGroup,
                                                                              browserTabIndex);
    int32_t selectedDrawingTypeIndex = 0;
    
    for (int32_t i = 0; i < numDrawingTypeEnums; i++) {
        FociDrawingTypeEnum::Enum drawType = drawingTypeEnums[i];
        if (drawType == selectedDrawingType) {
            selectedDrawingTypeIndex = i;
        }
    }
    m_drawTypeComboBox->setCurrentIndex(selectedDrawingTypeIndex);
    
    m_sizeSpinBox->blockSignals(true);
    m_sizeSpinBox->setValue(dpf->getFociSize(displayGroup,
                                             browserTabIndex));
    m_sizeSpinBox->blockSignals(false);
}

/**
 * Update other foci view controllers.
 */
void 
FociSelectionViewController::updateOtherFociViewControllers()
{
    for (std::set<FociSelectionViewController*>::iterator iter = FociSelectionViewController::allFociSelectionViewControllers.begin();
         iter != FociSelectionViewController::allFociSelectionViewControllers.end();
         iter++) {
        FociSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateFociViewController();
        }
    }
}

/**
 * Gets called when foci selections are changed.
 */
void 
FociSelectionViewController::processFociSelectionChanges()
{
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
FociSelectionViewController::processSelectionChanges()
{
    updateOtherFociViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
FociSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isFociUpdate()
                || uiEvent->isToolBoxUpdate()) {
                doUpdate = true;
                uiEvent->setEventProcessed();
            }
        }
    }

    if (doUpdate) {
        updateFociViewController();
    }
}


