
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QSignalMapper>

#define __IMAGE_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ImageSelectionViewController.h"
#undef __IMAGE_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesImages.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQMacroManager.h"
#include "WuQSpinBoxGroup.h"
#include "WuQtUtilities.h"
#include "WuQTabWidget.h"

using namespace caret;

static const int COLUMN_RADIO_BUTTON = 0;

/**
 * \class caret::ImageSelectionViewController 
 * \brief View controller for image selection
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ImageSelectionViewController::ImageSelectionViewController(const int32_t browserWindowIndex,
                                                           const QString& parentObjectName,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex),
m_objectNamePrefix(parentObjectName
                   + ":Image")
{
    setWindowTitle("Images");
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    QLabel* groupLabel = new QLabel("Group");
    m_imagesDisplayGroupComboBox = new DisplayGroupEnumComboBox(this,
                                                                (m_objectNamePrefix
                                                                 + ":DisplayGroup"),
                                                                "images");
    QObject::connect(m_imagesDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(imageDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_imagesDisplayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    m_imageDisplayCheckBox = new QCheckBox("Display Image");
    QObject::connect(m_imageDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_imageDisplayCheckBox->setToolTip("Display Image");
    m_imageDisplayCheckBox->setObjectName(m_objectNamePrefix
                                          + ":Display");
    macroManager->addMacroSupportToObject(m_imageDisplayCheckBox,
                                          "Enable display of image");
    
    m_controlPointsDisplayCheckBox = new QCheckBox("Display Control Points");
    QObject::connect(m_controlPointsDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_controlPointsDisplayCheckBox->setToolTip("Display Control Points");
    m_controlPointsDisplayCheckBox->setObjectName(m_objectNamePrefix
                                          + ":DisplayControlPoints");
    macroManager->addMacroSupportToObject(m_controlPointsDisplayCheckBox,
                                          "Display image control points");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                   this);
    m_tabWidget->addTab(attributesWidget,
                        "Attributes");
    m_tabWidget->addTab(selectionWidget,
                        "Selection");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                               + ":Tab");
    macroManager->addMacroSupportToObjectWithToolTip(m_tabWidget->getTabBar(),
                                                     "Select features toolbox image tab",
                                                     "Features ToolBox Image Tab");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addLayout(groupLayout, 0);
    layout->addWidget(m_imageDisplayCheckBox, 0);
    layout->addWidget(m_controlPointsDisplayCheckBox, 0);
    layout->addSpacing(10);
    layout->addWidget(m_tabWidget->getWidget(), 100);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    s_allImageSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
ImageSelectionViewController::~ImageSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    s_allImageSelectionViewControllers.erase(this);
    
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ImageSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isToolBoxUpdate()) {
                doUpdate = true;
                uiEvent->setEventProcessed();
            }
        }
    }
    
    if (doUpdate) {
        updateImageViewController();
    }
}

/**
 * Create the image selection widget.
 */
QWidget*
ImageSelectionViewController::createSelectionWidget()
{
    m_imageRadioButtonGroup = new QButtonGroup(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_imageRadioButtonGroup, &QButtonGroup::idClicked,
                     this, &ImageSelectionViewController::imageRadioButtonClicked);
#else
    QObject::connect(m_imageRadioButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(imageRadioButtonClicked(int)));
#endif
    
    QWidget* imageRadioButtonWidget = new QWidget();
    imageRadioButtonWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_imageRadioButtonLayout = new QGridLayout(imageRadioButtonWidget);
    m_imageRadioButtonLayout->setColumnStretch(COLUMN_RADIO_BUTTON, 100);
    const int imageLayoutRow = m_imageRadioButtonLayout->rowCount();
    m_imageRadioButtonLayout->addWidget(new QLabel("Image"),
                                        imageLayoutRow, COLUMN_RADIO_BUTTON,
                                        Qt::AlignHCenter);
    
    QScrollArea* imageRadioButtonScrollArea = new QScrollArea();
    imageRadioButtonScrollArea->setWidget(imageRadioButtonWidget);
    imageRadioButtonScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    imageRadioButtonScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    imageRadioButtonScrollArea->setWidgetResizable(true);
    
    return imageRadioButtonScrollArea;
}

/**
 * @return The attributes widget.
 */
QWidget*
ImageSelectionViewController::createAttributesWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(m_objectNamePrefix
                                   + ":Attributes");
    
    QLabel* depthLabel = new QLabel("Depth");
    m_depthComboBox = new EnumComboBoxTemplate(this);
    m_depthComboBox->setup<ImageDepthPositionEnum, ImageDepthPositionEnum::Enum>();
    m_depthComboBox->getWidget()->setToolTip("Set the depth position of the image3");
    QObject::connect(m_depthComboBox, SIGNAL(itemActivated()),
                     this, SLOT(processAttributesChanges()));
    m_depthComboBox->getComboBox()->setObjectName(objectNamePrefix
                                                  + ":DepthPosition");
    macroManager->addMacroSupportToObject(m_depthComboBox->getComboBox(),
                                          "Set image depth");

    const float threshMin = -1.0;
    const float threshMax = 100000.0;
    
    QLabel* thresholdMinimumLabel = new QLabel("Minimum Threshold");
    m_thresholdMinimumSpinBox = WuQFactory::newDoubleSpinBox();
    m_thresholdMinimumSpinBox->setFixedWidth(80);
    m_thresholdMinimumSpinBox->setRange(threshMin,
                                        threshMax);
    m_thresholdMinimumSpinBox->setSingleStep(1.0);
    m_thresholdMinimumSpinBox->setDecimals(1);
    m_thresholdMinimumSpinBox->setToolTip("Do not display image pixels containing a color component less than this value");
    QObject::connect(m_thresholdMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    m_thresholdMinimumSpinBox->setObjectName(objectNamePrefix
                                             + "MinimumThreshold");
    macroManager->addMacroSupportToObject(m_thresholdMinimumSpinBox,
                                          "Set image threshold minimum");
    
    QLabel* thresholdMaximumLabel = new QLabel("Maximum Threshold");
    m_thresholdMaximumSpinBox = WuQFactory::newDoubleSpinBox();
    m_thresholdMaximumSpinBox->setFixedWidth(80);
    m_thresholdMaximumSpinBox->setRange(threshMin,
                                        threshMax);
    m_thresholdMaximumSpinBox->setSingleStep(1.0);
    m_thresholdMaximumSpinBox->setDecimals(1);
    m_thresholdMaximumSpinBox->setToolTip("Do not display image pixels containing a color component greater than this value");
    QObject::connect(m_thresholdMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    m_thresholdMaximumSpinBox->setObjectName(objectNamePrefix
                                             + "MaximumThreshold");
    macroManager->addMacroSupportToObject(m_thresholdMaximumSpinBox,
                                          "Set image threshold maximum");
    
    const float minOpacity = 0.0;
    const float maxOpacity = 1.0;
    QLabel* opacityLabel = new QLabel("Opacity");
    m_opacitySpinBox = WuQFactory::newDoubleSpinBox();
    m_opacitySpinBox->setFixedWidth(80);
    m_opacitySpinBox->setRange(minOpacity,
                               maxOpacity);
    m_opacitySpinBox->setSingleStep(0.1);
    m_opacitySpinBox->setDecimals(1);
    m_opacitySpinBox->setToolTip("Opacity for image");
    QObject::connect(m_opacitySpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    m_opacitySpinBox->setObjectName(objectNamePrefix
                                    + ":Opacity");
    macroManager->addMacroSupportToObject(m_opacitySpinBox,
                                          "Set image opacity");
    
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(depthLabel, row, 0);
    gridLayout->addWidget(m_depthComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(thresholdMinimumLabel, row, 0);
    gridLayout->addWidget(m_thresholdMinimumSpinBox, row, 1);
    row++;
    gridLayout->addWidget(thresholdMaximumLabel, row, 0);
    gridLayout->addWidget(m_thresholdMaximumSpinBox, row, 1);
    row++;
    gridLayout->addWidget(opacityLabel, row, 0);
    gridLayout->addWidget(m_opacitySpinBox, row, 1);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(gridWidget);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when a selection changes.
 */
void
ImageSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesImages* dpi = GuiManager::get()->getBrain()->getDisplayPropertiesImages();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpi->getDisplayGroupForTab(browserTabIndex);
    dpi->setDisplayed(displayGroup,
                      browserTabIndex,
                      m_imageDisplayCheckBox->isChecked());
    dpi->setControlPointsDisplayed(displayGroup,
                      browserTabIndex,
                      m_controlPointsDisplayCheckBox->isChecked());
    dpi->setImagePosition(displayGroup,
                          browserTabIndex,
                          m_depthComboBox->getSelectedItem<ImageDepthPositionEnum, ImageDepthPositionEnum::Enum>());
    
    dpi->setThresholdMinimum(displayGroup,
                             browserTabIndex,
                             m_thresholdMinimumSpinBox->value());
    
    dpi->setThresholdMaximum(displayGroup,
                             browserTabIndex,
                             m_thresholdMaximumSpinBox->value());
    
    dpi->setOpacity(displayGroup,
                             browserTabIndex,
                             m_opacitySpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    updateOtherImageViewControllers();
}

/**
 * Called when the display group is changed.
 *
 * @param displayGroup
 *    Display group selected.
 */
void
ImageSelectionViewController::imageDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesImages* dsb = brain->getDisplayPropertiesImages();
    dsb->setDisplayGroupForTab(browserTabIndex,
                               displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateImageViewController();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when an image radio button is clicked.
 *
 * @param buttonID
 *    ID of button that was clicked.
 */
void
ImageSelectionViewController::imageRadioButtonClicked(int buttonID)
{
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesImages* dpi = brain->getDisplayPropertiesImages();
    const DisplayGroupEnum::Enum displayGroup = dpi->getDisplayGroupForTab(browserTabIndex);
    std::vector<ImageFile*> allImageFiles = brain->getAllImagesFiles();
    CaretAssertVectorIndex(allImageFiles, buttonID);
    
    dpi->setSelectedImageFile(displayGroup,
                              browserTabIndex,
                              allImageFiles[buttonID]);

    updateOtherImageViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the image selection widget.
 */
void
ImageSelectionViewController::updateImageViewController()
{
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesImages* dpi = brain->getDisplayPropertiesImages();
    const DisplayGroupEnum::Enum displayGroup = dpi->getDisplayGroupForTab(browserTabIndex);
    
    m_imagesDisplayGroupComboBox->setSelectedDisplayGroup(dpi->getDisplayGroupForTab(browserTabIndex));
    
    std::vector<ImageFile*> allImageFiles = brain->getAllImagesFiles();
    
    
    m_imageDisplayCheckBox->setChecked(dpi->isDisplayed(displayGroup,
                                                          browserTabIndex));
    m_controlPointsDisplayCheckBox->setChecked(dpi->isControlPointsDisplayed(displayGroup,
                                                                             browserTabIndex));
    
    
    const int32_t numImageFiles = static_cast<int32_t>(allImageFiles.size());
    int32_t numRadioButtons = static_cast<int32_t>(m_imageRadioButtons.size());

    if (numImageFiles > numRadioButtons) {
        const int32_t numToAdd = numImageFiles - numRadioButtons;
        for (int32_t i = 0; i < numToAdd; i++) {
            const int buttonID = static_cast<int>(m_imageRadioButtons.size());
            QRadioButton* rb = new QRadioButton("");
            const int32_t buttonNumber(m_imageRadioButtons.size() + 1);
            m_imageRadioButtons.push_back(rb);
            
            const int row = m_imageRadioButtonLayout->rowCount();
            m_imageRadioButtonLayout->addWidget(rb,
                                                row, COLUMN_RADIO_BUTTON);
            
            m_imageRadioButtonGroup->addButton(rb,
                                               buttonID);
            
            const QString buttonName(m_objectNamePrefix
                                     + ":Selection:Image"
                                     + QString("%1").arg((int)buttonNumber, 2, 10, QLatin1Char('0')));
            rb->setObjectName(buttonName);
            const QString descriptiveName("Select Image "
                                          + QString("%1").arg(buttonNumber));
            WuQMacroManager::instance()->addMacroSupportToObjectWithToolTip(rb,
                                                                            descriptiveName,
                                                                            "");
        }
        
        numRadioButtons = static_cast<int32_t>(m_imageRadioButtons.size());
    }
    
    const ImageFile* selectedImageFile = dpi->getSelectedImageFile(displayGroup,
                                                                   browserTabIndex);
    for (int32_t i = 0; i < numRadioButtons; i++) {
        CaretAssertVectorIndex(m_imageRadioButtons, i);
        QRadioButton* rb = m_imageRadioButtons[i];
        
        if (i < numImageFiles) {
            rb->setText(allImageFiles[i]->getFileNameNoPath());
            if (allImageFiles[i] == selectedImageFile) {
                rb->setChecked(true);
            }
            rb->setVisible(true);
        }
        else {
            rb->setVisible(false);
        }
    }
    
    m_depthComboBox->setSelectedItem<ImageDepthPositionEnum, ImageDepthPositionEnum::Enum>(dpi->getImagePosition(displayGroup,
                                                                                                                 browserTabIndex));
    m_thresholdMinimumSpinBox->blockSignals(true);
    m_thresholdMinimumSpinBox->setValue(dpi->getThresholdMinimum(displayGroup, browserTabIndex));
    m_thresholdMinimumSpinBox->blockSignals(false);
    m_thresholdMaximumSpinBox->blockSignals(true);
    m_thresholdMaximumSpinBox->setValue(dpi->getThresholdMaximum(displayGroup, browserTabIndex));
    m_thresholdMaximumSpinBox->blockSignals(false);
    m_opacitySpinBox->blockSignals(true);
    m_opacitySpinBox->setValue(dpi->getOpacity(displayGroup, browserTabIndex));
    m_opacitySpinBox->blockSignals(false);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void
ImageSelectionViewController::updateOtherImageViewControllers()
{
    for (std::set<ImageSelectionViewController*>::iterator iter = s_allImageSelectionViewControllers.begin();
         iter != s_allImageSelectionViewControllers.end();
         iter++) {
        ImageSelectionViewController* ivc = *iter;
        if (ivc != this) {
            ivc->updateImageViewController();
        }
    }
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ImageSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ImageSelectionViewController",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ImageSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

