
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
#include <QToolButton>

#define __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "BorderSelectionViewController.h"
#undef __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "BorderDrawingTypeEnum.h"
#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesBorders.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
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
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    m_tabWidget->addTab(attributesWidget, 
                      "Attributes");
    m_tabWidget->addTab(selectionWidget, 
                      "Selection");
    m_tabWidget->setCurrentWidget(attributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    //WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(m_bordersDisplayCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(groupLayout);
    layout->addWidget(m_tabWidget->getWidget(), 0, Qt::AlignLeft);
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
    
    QLabel* coloringLabel = new QLabel("Coloring");
    m_coloringTypeComboBox = new EnumComboBoxTemplate(this);
    m_coloringTypeComboBox->setup<FeatureColoringTypeEnum,
    FeatureColoringTypeEnum::Enum>();
    m_coloringTypeComboBox->getWidget()->setToolTip("Select the coloring assignment for borders");
    QObject::connect(m_coloringTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* standardColorLabel = new QLabel("Standard Color");
    m_standardColorComboBox = new CaretColorEnumComboBox(this);
    m_standardColorComboBox->getWidget()->setToolTip("Select the standard color");
    QObject::connect(m_standardColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(processAttributesChanges()));

    float minLineWidth = 0;
    float maxLineWidth = 1000;
    //BrainOpenGL::getMinMaxLineWidth(minLineWidth,
    //                                maxLineWidth);
    
    QLabel* lineWidthLabel = new QLabel("Line Diameter");
    m_lineWidthSpinBox = WuQFactory::newDoubleSpinBox();
    m_lineWidthSpinBox->setFixedWidth(80);
    m_lineWidthSpinBox->setRange(minLineWidth,
                                 maxLineWidth);
    m_lineWidthSpinBox->setSingleStep(1.0);
    m_lineWidthSpinBox->setDecimals(1);
    m_lineWidthSpinBox->setSuffix("px");
    m_lineWidthSpinBox->setToolTip("Adjust the width of borders drawn as lines.\n"
                                   "Units is pixels\n"
                                   "The maximum width is dependent upon the \n"
                                   "graphics system.  There is no maximum value\n"
                                   "for this control and the drawn width of the \n"
                                   "lines will stop increasing even though the\n"
                                   "value of this control is changing");
    QObject::connect(m_lineWidthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* pointSizeLabel = new QLabel("Symbol Diameter");
    m_pointSizeSpinBox = WuQFactory::newDoubleSpinBox();
    m_pointSizeSpinBox->setFixedWidth(80);
    m_pointSizeSpinBox->setRange(minLineWidth,
                                 maxLineWidth);
    m_pointSizeSpinBox->setSingleStep(1.0);
    m_pointSizeSpinBox->setDecimals(1);
    m_pointSizeSpinBox->setToolTip("Adjust the size of borders drawn as points");
    m_pointSizeSpinBox->setSuffix("mm");
    QObject::connect(m_pointSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_enableUnstretchedLinesCheckBox = new QCheckBox("Unstretched Lines");
    QObject::connect(m_enableUnstretchedLinesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    m_unstretchedLinesLengthSpinBox = WuQFactory::newDoubleSpinBox();
    m_unstretchedLinesLengthSpinBox->setFixedWidth(80);
    m_unstretchedLinesLengthSpinBox->setRange(0.0,
                                              10000000.0);
    m_unstretchedLinesLengthSpinBox->setSingleStep(1.0);
    m_unstretchedLinesLengthSpinBox->setDecimals(1);
    m_unstretchedLinesLengthSpinBox->setToolTip(WuQtUtilities::createWordWrappedToolTipText("Ratio = (length of border on flat surface divided by length of border of 3d (primary anatomical) surface.  "
                                                "When ratio is greater than the unstretched lines value, the border segment is NOT drawn."));
    m_unstretchedLinesLengthSpinBox->setSuffix("mm");
    QObject::connect(m_unstretchedLinesLengthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* aboveSurfaceLabel = new QLabel("Above Offset");
    m_aboveSurfaceOffsetSpinBox =WuQFactory::newDoubleSpinBox();
    m_aboveSurfaceOffsetSpinBox->setFixedWidth(80);
    m_aboveSurfaceOffsetSpinBox->setRange(-100.0,
                                           100.0);
    m_aboveSurfaceOffsetSpinBox->setSingleStep(0.1);
    m_aboveSurfaceOffsetSpinBox->setDecimals(1);
    m_aboveSurfaceOffsetSpinBox->setToolTip(WuQtUtilities::createWordWrappedToolTipText("Moves surface away from borders (in depth) so that borders are above surface.  "
                                                                                        "Use with caution."));
    QObject::connect(m_aboveSurfaceOffsetSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_bordersContralateralCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    gridLayout->addWidget(drawAsLabel, row, 0);
    gridLayout->addWidget(m_drawTypeComboBox, row, 1);
    row++;
    gridLayout->addWidget(coloringLabel, row, 0);
    gridLayout->addWidget(m_coloringTypeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(standardColorLabel, row, 0);
    gridLayout->addWidget(m_standardColorComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(lineWidthLabel, row, 0);
    gridLayout->addWidget(m_lineWidthSpinBox, row, 1);
    row++;
    gridLayout->addWidget(pointSizeLabel, row, 0);
    gridLayout->addWidget(m_pointSizeSpinBox, row, 1);
    row++;
    gridLayout->addWidget(m_enableUnstretchedLinesCheckBox, row, 0);
    gridLayout->addWidget(m_unstretchedLinesLengthSpinBox, row, 1);
    row++;
    gridLayout->addWidget(aboveSurfaceLabel, row, 0);
    gridLayout->addWidget(m_aboveSurfaceOffsetSpinBox, row, 1);
    
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
    const FeatureColoringTypeEnum::Enum selectedColoringType = m_coloringTypeComboBox->getSelectedItem<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>();
    const CaretColorEnum::Enum standardColorType = m_standardColorComboBox->getSelectedColor();
    
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
    dpb->setColoringType(displayGroup,
                         browserTabIndex,
                         selectedColoringType);
    dpb->setStandardColorType(displayGroup,
                           browserTabIndex,
                           standardColorType);
    dpb->setLineWidth(displayGroup,
                      browserTabIndex,
                      m_lineWidthSpinBox->value());
    dpb->setPointSize(displayGroup,
                      browserTabIndex,
                      m_pointSizeSpinBox->value());
    dpb->setUnstretchedLinesEnabled(displayGroup,
                                    browserTabIndex,
                                    m_enableUnstretchedLinesCheckBox->isChecked());
    dpb->setUnstretchedLinesLength(displayGroup,
                                   browserTabIndex,
                                   m_unstretchedLinesLengthSpinBox->value());
    dpb->setAboveSurfaceOffset(m_aboveSurfaceOffsetSpinBox->value());
    
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
    
    m_coloringTypeComboBox->setSelectedItem<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>(dpb->getColoringType(displayGroup,
                                                                                                                         browserTabIndex));
    m_standardColorComboBox->setSelectedColor(dpb->getStandardColorType(displayGroup,
                                                                        browserTabIndex));
    m_lineWidthSpinBox->blockSignals(true);
    m_lineWidthSpinBox->setValue(dpb->getLineWidth(displayGroup,
                                                   browserTabIndex));
    m_lineWidthSpinBox->blockSignals(false);
    
    m_pointSizeSpinBox->blockSignals(true);
    m_pointSizeSpinBox->setValue(dpb->getPointSize(displayGroup,
                                                   browserTabIndex));
    m_pointSizeSpinBox->blockSignals(false);
    
    m_enableUnstretchedLinesCheckBox->setChecked(dpb->isUnstretchedLinesEnabled(displayGroup,
                                                                                browserTabIndex));
    m_unstretchedLinesLengthSpinBox->blockSignals(true);
    m_unstretchedLinesLengthSpinBox->setValue(dpb->getUnstretchedLinesLength(displayGroup,
                                                                             browserTabIndex));
    m_unstretchedLinesLengthSpinBox->blockSignals(false);
    
    m_aboveSurfaceOffsetSpinBox->blockSignals(true);
    m_aboveSurfaceOffsetSpinBox->setValue(dpb->getAboveSurfaceOffset());
    m_aboveSurfaceOffsetSpinBox->blockSignals(false);
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
    CaretAssert(browserTabContent);
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
BorderSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                               const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BorderSelectionViewController",
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
BorderSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                    const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}


