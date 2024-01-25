
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
#include "CaretColorEnumComboBox.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesFoci.h"
#include "EnumComboBoxTemplate.h"
#include "FeatureColoringTypeEnum.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQTabWidget.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::FociSelectionViewController 
 * \brief Widget for controlling display of foci
 * \ingroup GuiQt
 *
 * Widget for controlling the display of foci including
 * different display groups.
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window
 * @param parentObjectName
 *    Name of parent object
 * @param parent
 *    The parent object
 */
FociSelectionViewController::FociSelectionViewController(const int32_t browserWindowIndex,
                                                         const QString& parentObjectName,
                                                         QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":Foci")
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
    m_fociDisplayCheckBox->setObjectName(m_objectNamePrefix
                                            + ":DisplayFoci");
    WuQMacroManager::instance()->addMacroSupportToObject(m_fociDisplayCheckBox,
                                                         "Enable foci display");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    m_tabWidget->addTab(attributesWidget, 
                      "Attributes");
    m_tabWidget->addTab(selectionWidget, 
                      "Selection");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setToolTip("Select foci tab");
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                                            + ":Tab");
    WuQMacroManager::instance()->addMacroSupportToObject(m_tabWidget->getTabBar(),
                                                         "Select features toolbox foci tab");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    //WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(m_fociDisplayCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(groupLayout);
    layout->addWidget(m_tabWidget->getWidget(), 0, Qt::AlignLeft);
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

/**
 * @return New instance of foci selection widget
 */
QWidget* 
FociSelectionViewController::createSelectionWidget()
{
    m_fociClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex,
                                                                                     (m_objectNamePrefix
                                                                                      + ":Selection"),
                                                                                     "foci",
                                                                                     this);
    
    return m_fociClassNameHierarchyViewController;
}

/**
 * @return The attributes widget.
 */
QWidget*
FociSelectionViewController::createAttributesWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    m_fociContralateralCheckBox = new QCheckBox("Contralateral");
    m_fociContralateralCheckBox->setToolTip("Enable display of foci from contralateral brain structure");
    QObject::connect(m_fociContralateralCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_fociContralateralCheckBox->setObjectName(m_objectNamePrefix
                                                  + ":Contralateral");
    macroManager->addMacroSupportToObject(m_fociContralateralCheckBox,
                                          "Enable contralateral foci");
    
    m_pasteOntoSurfaceCheckBox = new QCheckBox("Paste Onto Surface");
    m_pasteOntoSurfaceCheckBox->setToolTip("Place the foci onto the surface");
    QObject::connect(m_pasteOntoSurfaceCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_pasteOntoSurfaceCheckBox->setObjectName(m_objectNamePrefix
                                               + ":PasteOntoSurface");
    macroManager->addMacroSupportToObject(m_pasteOntoSurfaceCheckBox,
                                          "Enable paste foci onto surface");
    
    QLabel* projectionLabel = new QLabel("Coordinate Type");
    m_drawingProjectionTypeComboBox = new EnumComboBoxTemplate(this);
    m_drawingProjectionTypeComboBox->setup<FociDrawingProjectionTypeEnum, FociDrawingProjectionTypeEnum::Enum>();
    QObject::connect(m_drawingProjectionTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &FociSelectionViewController::processAttributesChanges);
    m_drawingProjectionTypeComboBox->getComboBox()->setToolTip("Select foci projection type");
    m_drawingProjectionTypeComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                                                  + ":ProjectionType");
    macroManager->addMacroSupportToObject(m_drawingProjectionTypeComboBox->getComboBox(),
                                          "Select foci projection type");
    m_drawingProjectionTypeComboBox->setToolTip(FociDrawingProjectionTypeEnum::getToolTip());
    
    QLabel* coloringLabel = new QLabel("Coloring");
    m_coloringTypeComboBox = new EnumComboBoxTemplate(this);
    m_coloringTypeComboBox->setup<FeatureColoringTypeEnum,
                                           FeatureColoringTypeEnum::Enum>();
    m_coloringTypeComboBox->getWidget()->setToolTip("Select the coloring assignment for foci");
    QObject::connect(m_coloringTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(processAttributesChanges()));
    m_coloringTypeComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                                     + ":ColorType");
    macroManager->addMacroSupportToObject(m_coloringTypeComboBox->getComboBox(),
                                          "Select foci color type");
    
    QLabel* standardColorLabel = new QLabel("Standard Color");
    m_standardColorComboBox = new CaretColorEnumComboBox(this);
    m_standardColorComboBox->getWidget()->setObjectName((m_objectNamePrefix
                                                         + ":Color"));
    m_standardColorComboBox->getWidget()->setToolTip("Select the standard color");
    WuQMacroManager::instance()->addMacroSupportToObject(m_standardColorComboBox->getComboBox(),
                                                         "Select foci standard color");
    QObject::connect(m_standardColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
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
    m_drawTypeComboBox->setObjectName(m_objectNamePrefix
                       + ":DrawingStyle");
    macroManager->addMacroSupportToObject(m_drawTypeComboBox,
                                          "Select foci drawing style");
    
    QLabel* symbolSizeTypeLabel = new QLabel("Diameter Type:");
    m_symbolSizeTypeComboBox = new EnumComboBoxTemplate(this);
    m_symbolSizeTypeComboBox->setup<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>();
    QObject::connect(m_symbolSizeTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &FociSelectionViewController::symbolSizeTypeComboBoxActivated);
    m_symbolSizeTypeComboBox->setToolTip(IdentificationSymbolSizeTypeEnum::getToolTip("focus"));

    QLabel* pointSizeLabel = new QLabel("Symbol Diameter");
    m_sizeSpinBox = WuQFactory::newDoubleSpinBox();
    m_sizeSpinBox->setFixedWidth(80);
    m_sizeSpinBox->setRange(0.1, 1000.0);
    m_sizeSpinBox->setSingleStep(0.1);
    m_sizeSpinBox->setDecimals(1);
    m_sizeSpinBox->setSuffix("mm");
    m_sizeSpinBox->setToolTip("Adjust the diameter of foci");
    QObject::connect(m_sizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    m_sizeSpinBox->setObjectName(m_objectNamePrefix
                       + ":Diameter");
    macroManager->addMacroSupportToObject(m_sizeSpinBox,
                                          "Set foci size");
        
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_fociContralateralCheckBox, row, 0, 1, 2);
    row++;
    gridLayout->addWidget(m_pasteOntoSurfaceCheckBox, row, 0, 1, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    gridLayout->addWidget(projectionLabel, row, 0);
    gridLayout->addWidget(m_drawingProjectionTypeComboBox->getComboBox(), row, 1);
    row++;
    gridLayout->addWidget(coloringLabel, row, 0);
    gridLayout->addWidget(m_coloringTypeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(standardColorLabel, row, 0);
    gridLayout->addWidget(m_standardColorComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(drawAsLabel, row, 0);
    gridLayout->addWidget(m_drawTypeComboBox , row, 1);
    row++;
    gridLayout->addWidget(symbolSizeTypeLabel, row, 0);
    gridLayout->addWidget(m_symbolSizeTypeComboBox->getWidget(), row, 1);
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
    const FociDrawingProjectionTypeEnum::Enum selectedDrawingProjectionType = m_drawingProjectionTypeComboBox->getSelectedItem<FociDrawingProjectionTypeEnum, FociDrawingProjectionTypeEnum::Enum>();
    const CaretColorEnum::Enum standardColorType = m_standardColorComboBox->getSelectedColor();
    
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
    dpf->setDrawingProjectionType(displayGroup,
                                  browserTabIndex,
                                  selectedDrawingProjectionType);
    dpf->setColoringType(displayGroup,
                         browserTabIndex,
                         selectedColoringType);
    dpf->setStandardColorType(displayGroup,
                              browserTabIndex,
                              standardColorType);
    
    switch (dpf->getFociSymbolSizeType(displayGroup, browserTabIndex)) {
        case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
            dpf->setFociSizeMillimeters(displayGroup,
                                        browserTabIndex,
                                        m_sizeSpinBox->value());
            break;
        case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
            dpf->setFociSizePercentage(displayGroup,
                                       browserTabIndex,
                                       m_sizeSpinBox->value());
            break;
    }
    
    dpf->setDrawingType(displayGroup,
                        browserTabIndex,
                        selectedDrawingType);
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
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
 * Called when symbol size type combo box changed
 */
void
FociSelectionViewController::symbolSizeTypeComboBoxActivated()
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
    
    dpf->setFociSymbolSizeType(displayGroup,
                               browserTabIndex,
                               m_symbolSizeTypeComboBox->getSelectedItem<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>());

    /*
     * Update controls since size type has changed
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
    
    m_drawingProjectionTypeComboBox->setSelectedItem<FociDrawingProjectionTypeEnum,FociDrawingProjectionTypeEnum::Enum>(dpf->getDrawingProjectionType(displayGroup,
                                                                                                                                                      browserTabIndex));
    m_standardColorComboBox->setSelectedColor(dpf->getStandardColorType(displayGroup,
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
    
    m_symbolSizeTypeComboBox->setSelectedItem<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>(dpf->getFociSymbolSizeType(displayGroup,
                                                                                                                                                   browserTabIndex));

    m_sizeSpinBox->blockSignals(true);
    m_sizeSpinBox->setValue(dpf->getFociSizeMillimeters(displayGroup,
                                             browserTabIndex));
    switch (dpf->getFociSymbolSizeType(displayGroup, browserTabIndex)) {
        case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
            m_sizeSpinBox->setValue(dpf->getFociSizeMillimeters(displayGroup,
                                                                browserTabIndex));
            m_sizeSpinBox->setSuffix("mm");
            break;
        case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
            m_sizeSpinBox->setValue(dpf->getFociSizePercentage(displayGroup,
                                                               browserTabIndex));
            m_sizeSpinBox->setSuffix("%");
            break;
    }    m_sizeSpinBox->blockSignals(false);
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
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
FociSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FociSelectionViewController",
                                            1);
    sceneClass->addClass(m_tabWidget->saveToScene(sceneAttributes,
                                                  "m_tabWidget"));
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
FociSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}


