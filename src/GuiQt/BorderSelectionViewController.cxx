
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
#include <QLabel>
#include <QLayout>
#include <QToolButton>

#define __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "BorderSelectionViewController.h"
#undef __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "BorderDrawingTypeEnum.h"
#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ClassAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesBorders.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventToolBoxUpdate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQDataEntryDialog.h"
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
    
    m_bordersContralateralCheckBox = new QCheckBox("Contralateral");
    QObject::connect(m_bordersContralateralCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processBorderSelectionChanges()));
    
    m_bordersDisplayCheckBox = new QCheckBox("Display Borders");
    QObject::connect(m_bordersDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processBorderSelectionChanges()));
    
    m_borderClassNameHierarchyViewController = new ClassAndNameHierarchyViewController(m_browserWindowIndex);
    QObject::connect(m_borderClassNameHierarchyViewController, SIGNAL(itemSelected(ClassAndNameHierarchySelectedItem*)),
                     this, SLOT(bordersSelectionsChanged(ClassAndNameHierarchySelectedItem*)));
    
    QAction* attributesAction = WuQtUtilities::createAction("Attributes",
                                                            "Show attributes editor",
                                                            this,
                                                            this,
                                                            SLOT(showAttributesDialog()));
    QToolButton* attributesToolButton = new QToolButton();
    attributesToolButton->setDefaultAction(attributesAction);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(attributesToolButton);
    layout->addWidget(m_bordersDisplayCheckBox);  
    layout->addWidget(m_bordersContralateralCheckBox);  
    layout->addLayout(groupLayout);  
    layout->addWidget(m_borderClassNameHierarchyViewController);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_UPDATE);
    
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
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    dsb->setDisplayGroup(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateBorderSelectionViewController();
    
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
BorderSelectionViewController::bordersSelectionsChanged(ClassAndNameHierarchySelectedItem* /*selectedItem*/)
{
    processBorderSelectionChanges();
}

/**
 * Update the border selection widget.
 */
void 
BorderSelectionViewController::updateBorderSelectionViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    setWindowTitle("Borders");
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    
    m_bordersDisplayCheckBox->setChecked(dsb->isDisplayed(browserTabIndex));
    m_bordersContralateralCheckBox->setChecked(dsb->isContralateralDisplayed(browserTabIndex));
    m_bordersDisplayGroupComboBox->setSelectedDisplayGroup(dsb->getDisplayGroup(browserTabIndex));
    
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
    m_borderClassNameHierarchyViewController->updateContents(allBorderFiles);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
BorderSelectionViewController::updateOtherBorderSelectionViewControllers()
{
    for (std::set<BorderSelectionViewController*>::iterator iter = BorderSelectionViewController::allBorderSelectionViewControllers.begin();
         iter != BorderSelectionViewController::allBorderSelectionViewControllers.end();
         iter++) {
        BorderSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateBorderSelectionViewController();
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
    dsb->setDisplayed(browserTabIndex,
                      m_bordersDisplayCheckBox->isChecked());
    dsb->setContralateralDisplayed(browserTabIndex,
                                   m_bordersContralateralCheckBox->isChecked());
    dsb->setDisplayGroup(browserTabIndex, 
                         m_bordersDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
BorderSelectionViewController::processSelectionChanges()
{
    updateOtherBorderSelectionViewControllers();
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
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        updateBorderSelectionViewController();
        event->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_TOOLBOX_UPDATE) {
        EventToolBoxUpdate* tbEvent =
        dynamic_cast<EventToolBoxUpdate*>(event);
        bool doUpdate = false;
        if (tbEvent->isUpdateAllWindows()) {
            doUpdate = true;
        }
        else if (tbEvent->getBrowserWindowIndex() == m_browserWindowIndex) {
            doUpdate = true;
        }
        
        if (doUpdate) {
            updateBorderSelectionViewController();
        }
        
        tbEvent->setEventProcessed();
    }
}

/**
 * Show the attributes dialog.
 */ 
void 
BorderSelectionViewController::showAttributesDialog()
{
    DisplayPropertiesBorders* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesBorders();
    
    WuQDataEntryDialog ded("Border Attributes",
                           this);

    std::vector<BorderDrawingTypeEnum::Enum> drawingTypeEnums;
    BorderDrawingTypeEnum::getAllEnums(drawingTypeEnums);
    const int32_t numDrawingTypeEnums = static_cast<int32_t>(drawingTypeEnums.size());
    
    const BorderDrawingTypeEnum::Enum selectedDrawingType = dpb->getDrawingType();
    int32_t selectedDrawingTypeIndex = 0;
    QStringList drawingTypeNames;
    for (int32_t i = 0; i < numDrawingTypeEnums; i++) {
        if (drawingTypeEnums[i] == selectedDrawingType) {
            selectedDrawingTypeIndex = i;
        }
        drawingTypeNames.append(BorderDrawingTypeEnum::toGuiName(drawingTypeEnums[i]));
    }
    
    m_attributesDialogWidgets.m_drawTypeComboBox = ded.addComboBox("Draw As", 
                                                                     drawingTypeNames);
    m_attributesDialogWidgets.m_drawTypeComboBox->setCurrentIndex(selectedDrawingTypeIndex);
    
    QObject::connect(m_attributesDialogWidgets.m_drawTypeComboBox, SIGNAL(activated(int)),
                     this, SLOT(attributesDialogDataModified()));
    
    float minLineWidth = 0;
    float maxLineWidth = 0;
    BrainOpenGL::getMinMaxLineWidth(minLineWidth,
                                    maxLineWidth);
    
    m_attributesDialogWidgets.m_lineWidthSpinBox = ded.addDoubleSpinBox("Line Width",
                                                                          dpb->getLineWidth());
    m_attributesDialogWidgets.m_lineWidthSpinBox->setRange(minLineWidth,
                                                           maxLineWidth);
    QObject::connect(m_attributesDialogWidgets.m_lineWidthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(attributesDialogDataModified()));
    
    m_attributesDialogWidgets.m_pointSizeSpinBox = ded.addDoubleSpinBox("Point Size", 
                                                                          dpb->getPointSize());
    QObject::connect(m_attributesDialogWidgets.m_pointSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(attributesDialogDataModified()));
    
    ded.exec();
    
}

/**
 * Called when a widget on the attributes dialog has 
 * its value changed.
 */
void 
BorderSelectionViewController::attributesDialogDataModified()
{
    DisplayPropertiesBorders* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesBorders();
    
    const AString selectedDrawingTypeName = m_attributesDialogWidgets.m_drawTypeComboBox->currentText();
    const BorderDrawingTypeEnum::Enum selectedDrawingType = BorderDrawingTypeEnum::fromGuiName(selectedDrawingTypeName, 
                                                                                               NULL);
    dpb->setDrawingType(selectedDrawingType);
    dpb->setLineWidth(m_attributesDialogWidgets.m_lineWidthSpinBox->value());
    dpb->setPointSize(m_attributesDialogWidgets.m_pointSizeSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


