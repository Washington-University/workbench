
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_DECLARE__
#include "BrainBrowserWindowToolBarAllSurface.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ALL_SURFACE_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QWidgetAction>

#include "Brain.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "ModelWholeBrain.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "SurfaceTypeEnum.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarAllSurface 
 * \brief Toolbar component for ALL view surface selection
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parentObjectName
 *     Name of the parent object
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarAllSurface::BrainBrowserWindowToolBarAllSurface(const QString& parentObjectName,
                                                                         BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(parentObjectName
                                   + ":All:");
    
    this->wholeBrainSurfaceTypeComboBox = WuQFactory::newComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceTypeComboBox,
                                          "Select the geometric type of surface for display");
    QObject::connect(this->wholeBrainSurfaceTypeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(wholeBrainSurfaceTypeComboBoxIndexChanged(int)));
    this->wholeBrainSurfaceTypeComboBox->setObjectName(objectNamePrefix
                                                       + "SurfaceType");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceTypeComboBox,
                                          "Select all view surface type");
    
    /*
     * Left
     */
    this->wholeBrainSurfaceLeftCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceLeftCheckBox,
                                          "Enable/Disable display of the left cortical surface");
    QObject::connect(this->wholeBrainSurfaceLeftCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceLeftCheckBoxStateChanged(int)));
    this->wholeBrainSurfaceLeftCheckBox->setObjectName(objectNamePrefix
                                                       + "EnableLeft");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceLeftCheckBox,
                                          "Enable all view left surface");
    
    QToolButton* wholeBrainLeftSurfaceToolButton = new QToolButton();
    QAction* leftSurfaceAction = WuQtUtilities::createAction("Left",
                                                             "Select the whole brain left surface",
                                                             wholeBrainLeftSurfaceToolButton,
                                                             this,
                                                             SLOT(wholeBrainSurfaceLeftToolButtonTriggered(bool)));
    WuQtUtilities::setToolButtonStyleForQt5Mac(wholeBrainLeftSurfaceToolButton);
    wholeBrainLeftSurfaceToolButton->setDefaultAction(leftSurfaceAction);
    
    /*
     * Left menu is displayed when tool button is clicked
     */
    this->wholeBrainSurfaceLeftMenu = new QMenu(wholeBrainLeftSurfaceToolButton);
    QObject::connect(this->wholeBrainSurfaceLeftMenu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceLeftMenuTriggered);
    this->wholeBrainSurfaceLeftMenu->setObjectName(objectNamePrefix
                                                   + "SelectLeftSurfaceMenu");
    this->wholeBrainSurfaceLeftMenu->setToolTip("Select all view left surface");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceLeftMenu,
                                          "Select all view left surface");
    
    /*
     * Right
     */
    this->wholeBrainSurfaceRightCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceRightCheckBox,
                                          "Enable/Disable display of the right cortical surface");
    QObject::connect(this->wholeBrainSurfaceRightCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceRightCheckBoxStateChanged(int)));
    this->wholeBrainSurfaceRightCheckBox->setObjectName(objectNamePrefix
                                                        + "EnableRight");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceRightCheckBox,
                                          "Enable all view right surface");
    
    QToolButton* wholeBrainRightSurfaceToolButton = new QToolButton();
    QAction* rightSurfaceAction = WuQtUtilities::createAction("Right",
                                                              "Select the whole brain right surface",
                                                              wholeBrainRightSurfaceToolButton,
                                                              this,
                                                              SLOT(wholeBrainSurfaceRightToolButtonTriggered(bool)));
    WuQtUtilities::setToolButtonStyleForQt5Mac(wholeBrainRightSurfaceToolButton);
    wholeBrainRightSurfaceToolButton->setDefaultAction(rightSurfaceAction);
    
    /*
     * Right menu is displayed when tool button is clicked
     */
    this->wholeBrainSurfaceRightMenu = new QMenu(wholeBrainRightSurfaceToolButton);
    QObject::connect(this->wholeBrainSurfaceRightMenu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceRightMenuTriggered);
    this->wholeBrainSurfaceRightMenu->setObjectName(objectNamePrefix
                                                    + "SelectRightSurfaceMenu");
    this->wholeBrainSurfaceRightMenu->setToolTip("Select all view right surface");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceRightMenu,
                                          "Select all view right surface");
    
    /*
     * Cerebellum
     */
    this->wholeBrainSurfaceCerebellumCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceCerebellumCheckBox,
                                          "Enable/Disable display of the cerebellum surface");
    QObject::connect(this->wholeBrainSurfaceCerebellumCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceCerebellumCheckBoxStateChanged(int)));
    this->wholeBrainSurfaceCerebellumCheckBox->setObjectName(objectNamePrefix
                                                             + "EnableCerebellum");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceCerebellumCheckBox,
                                          "Enable all view cerebellum");
    
    QToolButton* wholeBrainCerebellumSurfaceToolButton = new QToolButton();
    QAction* cerebellumSurfaceAction = WuQtUtilities::createAction("Cerebellum",
                                                                   "Select the whole brain cerebellum surface",
                                                                   wholeBrainCerebellumSurfaceToolButton,
                                                                   this,
                                                                   SLOT(wholeBrainSurfaceCerebellumToolButtonTriggered(bool)));
    WuQtUtilities::setToolButtonStyleForQt5Mac(wholeBrainCerebellumSurfaceToolButton);
    wholeBrainCerebellumSurfaceToolButton->setDefaultAction(cerebellumSurfaceAction);
    
    /*
     * Cerebellum menu is displayed when tool button is clicked
     */
    this->wholeBrainSurfaceCerebellumMenu = new QMenu(wholeBrainCerebellumSurfaceToolButton);
    QObject::connect(this->wholeBrainSurfaceCerebellumMenu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceCerebellumMenuTriggered);
    this->wholeBrainSurfaceCerebellumMenu->setObjectName(objectNamePrefix
                                                         + "SelectCerebellumSurfaceMenu");
    this->wholeBrainSurfaceCerebellumMenu->setToolTip("Select all view cerebellum surface");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceCerebellumMenu,
                                          "Select all view cerebellum surface");
    
    /*
     * Hippocampus
     */
    this->wholeBrainSurfaceHippocampusCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceHippocampusCheckBox, 
                                          "Enable/Disable display of the hippocampus surfaces");
    QObject::connect(this->wholeBrainSurfaceHippocampusCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceHippocampusCheckBoxStateChanged(int)));
    this->wholeBrainSurfaceHippocampusCheckBox->setObjectName(objectNamePrefix
                                                             + "EnableHippocampus");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceHippocampusCheckBox,
                                          "Enable all view hippocampus");
    
    m_wholeBrainSurfaceHippocampusToolButton = new QToolButton();
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_wholeBrainSurfaceHippocampusToolButton);
    QObject::connect(m_wholeBrainSurfaceHippocampusToolButton, &QToolButton::clicked,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainHippocampusToolButtonClicked);
    m_wholeBrainHippcampusMenu = new QMenu();
    QWidgetAction* wholeBrainHippcampusMenuAction = new QWidgetAction(m_wholeBrainSurfaceHippocampusToolButton);
    wholeBrainHippcampusMenuAction->setDefaultWidget(createHippocampusSufaceSelectionWidget(objectNamePrefix));
    m_wholeBrainHippcampusMenu->addAction(wholeBrainHippcampusMenuAction);
    QObject::connect(m_wholeBrainHippcampusMenu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainHippocampusMenuAboutToShow);

    /*
     * Left/Right separation
     */
    const int separationSpinngerWidth = 48;
    this->wholeBrainSurfaceSeparationLeftRightSpinBox = WuQFactory::newDoubleSpinBox();
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setDecimals(0);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setFixedWidth(separationSpinngerWidth);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationLeftRightSpinBox,
                                          "Adjust the separation of the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationLeftRightSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double)));
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setObjectName(objectNamePrefix
                                                                     + "LeftRightSeparation");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceSeparationLeftRightSpinBox,
                                          "Set all view left/right separation");
    
    /*
     * Cerebellum separation
     */
    this->wholeBrainSurfaceSeparationCerebellumSpinBox = WuQFactory::newDoubleSpinBox();
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setDecimals(0);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setFixedWidth(separationSpinngerWidth);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationCerebellumSpinBox,
                                          "Adjust the separation of the cerebellum from the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationCerebellumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double)));
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setObjectName(objectNamePrefix
                                                                      + "CortexCerebellumSeparation");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceSeparationCerebellumSpinBox,
                                          "Set all view cerebral/cerebellum separation");
    
    this->wholeBrainSurfaceMatchCheckBox = new QCheckBox("Match");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceMatchCheckBox,
                                          "Match position and size of all surfaces to primary anatomical.  Useful for "
                                          "animation (surface interpolation) and recording movies.");
    QObject::connect(this->wholeBrainSurfaceMatchCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceMatchCheckBoxClicked);
    this->wholeBrainSurfaceMatchCheckBox->setObjectName(objectNamePrefix
                                                        + "MatchSurface");
    macroManager->addMacroSupportToObject(this->wholeBrainSurfaceMatchCheckBox,
                                          "Match position and size of all surfaces to primary anatomical");
    
    wholeBrainLeftSurfaceToolButton->setText("L");
    wholeBrainRightSurfaceToolButton->setText("R");
    wholeBrainCerebellumSurfaceToolButton->setText("C");
    m_wholeBrainSurfaceHippocampusToolButton->setText("H");
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(2);
    gridLayout->setHorizontalSpacing(2);
    gridLayout->addWidget(this->wholeBrainSurfaceTypeComboBox, 0, 0, 1, 6);
    gridLayout->addWidget(this->wholeBrainSurfaceLeftCheckBox, 1, 0);
    gridLayout->addWidget(wholeBrainLeftSurfaceToolButton, 1, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceRightCheckBox, 2, 0);
    gridLayout->addWidget(wholeBrainRightSurfaceToolButton, 2, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceCerebellumCheckBox, 3, 0);
    gridLayout->addWidget(wholeBrainCerebellumSurfaceToolButton, 3, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationLeftRightSpinBox, 1, 2, 2, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationCerebellumSpinBox, 3, 2);
    gridLayout->addWidget(this->wholeBrainSurfaceHippocampusCheckBox, 4, 0);
    gridLayout->addWidget(m_wholeBrainSurfaceHippocampusToolButton, 4, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceMatchCheckBox, 5, 0, 1, 6);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(gridLayout);
    
    addToWidgetGroup(this->wholeBrainSurfaceTypeComboBox);
    addToWidgetGroup(this->wholeBrainSurfaceLeftCheckBox);
    addToWidgetGroup(wholeBrainLeftSurfaceToolButton);
    addToWidgetGroup(this->wholeBrainSurfaceRightCheckBox);
    addToWidgetGroup(wholeBrainRightSurfaceToolButton);
    addToWidgetGroup(this->wholeBrainSurfaceCerebellumCheckBox);
    addToWidgetGroup(wholeBrainCerebellumSurfaceToolButton);
    addToWidgetGroup(this->wholeBrainSurfaceHippocampusCheckBox);
    addToWidgetGroup(m_wholeBrainSurfaceHippocampusToolButton);
    addToWidgetGroup(this->wholeBrainSurfaceSeparationLeftRightSpinBox);
    addToWidgetGroup(this->wholeBrainSurfaceSeparationCerebellumSpinBox);
    addToWidgetGroup(this->wholeBrainSurfaceMatchCheckBox);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarAllSurface::~BrainBrowserWindowToolBarAllSurface()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarAllSurface::updateContent(BrowserTabContent* browserTabContent)
{
    ModelWholeBrain* wholeBrainModel = browserTabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        const int32_t tabNumber = browserTabContent->getTabNumber();
        
        blockAllSignals(true);
        
        std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
        wholeBrainModel->getAvailableSurfaceTypes(availableSurfaceTypes);
        
        const SurfaceTypeEnum::Enum selectedSurfaceType = wholeBrainModel->getSelectedSurfaceType(tabNumber);
        
        int32_t defaultIndex = 0;
        this->wholeBrainSurfaceTypeComboBox->clear();
        int32_t numSurfaceTypes = static_cast<int32_t>(availableSurfaceTypes.size());
        for (int32_t i = 0; i < numSurfaceTypes; i++) {
            const SurfaceTypeEnum::Enum st = availableSurfaceTypes[i];
            if (st == selectedSurfaceType) {
                defaultIndex = this->wholeBrainSurfaceTypeComboBox->count();
            }
            const AString name = SurfaceTypeEnum::toGuiName(st);
            const int integerCode = SurfaceTypeEnum::toIntegerCode(st);
            this->wholeBrainSurfaceTypeComboBox->addItem(name,
                                                         integerCode);
        }
        if (defaultIndex < this->wholeBrainSurfaceTypeComboBox->count()) {
            this->wholeBrainSurfaceTypeComboBox->setCurrentIndex(defaultIndex);
        }
        
        this->wholeBrainSurfaceLeftCheckBox->setChecked(browserTabContent->isWholeBrainLeftEnabled());
        this->wholeBrainSurfaceRightCheckBox->setChecked(browserTabContent->isWholeBrainRightEnabled());
        this->wholeBrainSurfaceCerebellumCheckBox->setChecked(browserTabContent->isWholeBrainCerebellumEnabled());
        this->wholeBrainSurfaceHippocampusCheckBox->setChecked(browserTabContent->isWholeBrainHippocampusEnabled());
        
        updateAllWholeBrainSurfaceMenus();
        
        this->wholeBrainSurfaceSeparationLeftRightSpinBox->setValue(browserTabContent->getWholeBrainLeftRightSeparation());
        this->wholeBrainSurfaceSeparationCerebellumSpinBox->setValue(browserTabContent->getWholeBrainCerebellumSeparation());
        this->wholeBrainSurfaceMatchCheckBox->setChecked(wholeBrainModel->getBrain()->isSurfaceMatchingToAnatomical());
        
        blockAllSignals(false);
    }
}

/**
 * Called when the whole brain surface type combo box is changed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceTypeComboBoxIndexChanged(int /*indx*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    int32_t comboBoxIndex = this->wholeBrainSurfaceTypeComboBox->currentIndex();
    if (comboBoxIndex >= 0) {
        const int32_t integerCode = this->wholeBrainSurfaceTypeComboBox->itemData(comboBoxIndex).toInt();
        bool isValid = false;
        const SurfaceTypeEnum::Enum surfaceType = SurfaceTypeEnum::fromIntegerCode(integerCode, &isValid);
        if (isValid) {
            wholeBrainModel->setSelectedSurfaceType(tabIndex, surfaceType);
            m_parentToolBar->updateVolumeIndicesWidget(btc); /* slices may get deselected */
            this->updateAllWholeBrainSurfaceMenus();
            this->updateGraphicsWindowAndYokedWindows();
        }
    }
}

/**
 * Called when whole brain surface left check box is toggled.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceLeftCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainLeftEnabled(this->wholeBrainSurfaceLeftCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Update all whole brain surface selection menus
 */
void
BrainBrowserWindowToolBarAllSurface::updateAllWholeBrainSurfaceMenus()
{
    updateWholeBrainSurfaceMenu(this->wholeBrainSurfaceLeftMenu,
                                StructureEnum::CORTEX_LEFT);
    updateWholeBrainSurfaceMenu(this->wholeBrainSurfaceRightMenu,
                                StructureEnum::CORTEX_RIGHT);
    updateWholeBrainSurfaceMenu(this->wholeBrainSurfaceCerebellumMenu,
                                StructureEnum::CEREBELLUM);
}

/**
 * Update the menu to contain surface for the given structure
 *
 * @param menu
 *      Menu that is updated
 * @param structure
 *      Structure for surfaces
 */
void
BrainBrowserWindowToolBarAllSurface::updateWholeBrainSurfaceMenu(QMenu* menu,
                                                       const StructureEnum::Enum structure)
{
    CaretAssert(menu);
    menu->clear();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    const int32_t tabIndex = btc->getTabNumber();
    
    Brain* brain = GuiManager::get()->getBrain();
    BrainStructure* brainStructure = brain->getBrainStructure(structure, false);
    if (brainStructure != NULL) {
        std::vector<Surface*> surfaces;
        brainStructure->getSurfacesOfType(wholeBrainModel->getSelectedSurfaceType(tabIndex),
                                          surfaces);
        
        const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
        if (numSurfaces > 0) {
            Surface* selectedSurface = wholeBrainModel->getSelectedSurface(structure,
                                                                           tabIndex);
            for (int32_t i = 0; i < numSurfaces; i++) {
                QString name = surfaces[i]->getFileNameNoPath();
                QAction* action = new QAction(name);
                action->setCheckable(true);
                if (surfaces[i] == selectedSurface) {
                    action->setChecked(true);
                }
                action->setData(QVariant::fromValue((void*)surfaces[i]));
                menu->addAction(action);
            }
        }
    }
}

/**
 * Called when the left surface tool button is pressed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceLeftToolButtonTriggered(bool /*checked*/)
{
    updateAllWholeBrainSurfaceMenus();
    if ( ! this->wholeBrainSurfaceLeftMenu->isEmpty()) {
        this->wholeBrainSurfaceLeftMenu->exec(QCursor::pos());
    }
}

/**
 * Called when left surface is selected from menu
 *
 * @param action
 *     Action that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceLeftMenuTriggered(QAction* action)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    if (action != NULL) {
        QVariant data = action->data();
        void* p = data.value<void*>();
        Surface* surface = (Surface*)p;
        wholeBrainModel->setSelectedSurface(StructureEnum::CORTEX_LEFT,
                                            btc->getTabNumber(),
                                            surface);
        this->updateGraphicsWindowAndYokedWindows();
    }
}

/**
 * Called when the right surface tool button is pressed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceRightToolButtonTriggered(bool /*checked*/)
{
    updateAllWholeBrainSurfaceMenus();
    if ( ! this->wholeBrainSurfaceRightMenu->isEmpty()) {
        this->wholeBrainSurfaceRightMenu->exec(QCursor::pos());
    }
}

/**
 * Called when right surface is selected from menu
 *
 * @param action
 *     Action that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceRightMenuTriggered(QAction* action)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    if (action != NULL) {
        QVariant data = action->data();
        void* p = data.value<void*>();
        Surface* surface = (Surface*)p;
        wholeBrainModel->setSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                            btc->getTabNumber(),
                                            surface);
        this->updateGraphicsWindowAndYokedWindows();
    }
}


/**
 * Called when the cerebellum surface tool button is pressed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceCerebellumToolButtonTriggered(bool /*checked*/)
{
    updateAllWholeBrainSurfaceMenus();
    if ( ! this->wholeBrainSurfaceCerebellumMenu->isEmpty()) {
        this->wholeBrainSurfaceCerebellumMenu->exec(QCursor::pos());
    }
}

/**
 * Called when hippocampus  tool button is clicked
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainHippocampusToolButtonClicked()
{
    m_wholeBrainHippcampusMenu->exec(m_wholeBrainSurfaceHippocampusToolButton->mapToGlobal(QPoint(0,0)));
}

/**
 * Called when hippocampus menu is about to show
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainHippocampusMenuAboutToShow()
{
    m_leftHippoSurfaceSelector->updateControl();
    m_rightHippoSurfaceSelector->updateControl();
    m_dentateLeftHippoSurfaceSelector->updateControl();
    m_dentateRightHippoSurfaceSelector->updateControl();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        const int32_t tabIndex(btc->getTabNumber());
        m_leftHippoSurfaceSelector->setSurface(wholeBrainModel->getSelectedSurface(StructureEnum::HIPPOCAMPUS_LEFT,
                                                                                   tabIndex));
        m_rightHippoSurfaceSelector->setSurface(wholeBrainModel->getSelectedSurface(StructureEnum::HIPPOCAMPUS_RIGHT,
                                                                                    tabIndex));
        m_dentateLeftHippoSurfaceSelector->setSurface(wholeBrainModel->getSelectedSurface(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT,
                                                                                          tabIndex));
        m_dentateRightHippoSurfaceSelector->setSurface(wholeBrainModel->getSelectedSurface(StructureEnum::HIPPOCAMPUS_DENTATE_RIGHT,
                                                                                           tabIndex));
    }
}

/**
 * @return New instance of widget for selecting hippocampus surfaces
 * @param parentObjectName
 *   Name of parent object for macros
 */
QWidget*
BrainBrowserWindowToolBarAllSurface::createHippocampusSufaceSelectionWidget(const AString& parentObjectName)
{
    std::vector<SurfaceTypeEnum::Enum> surfaceTypes;
    surfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    
    m_leftHippSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_LEFT,
                                                                surfaceTypes);
    m_rightHippSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_RIGHT,
                                                                 surfaceTypes);
    m_leftDentateHippSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT,
                                                                       surfaceTypes);
    m_rightDentateHippSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT,
                                                                        surfaceTypes);
    
    QLabel* leftLabel(new QLabel("Left"));
    m_leftHippoSurfaceSelector = new SurfaceSelectionViewController(this,
                                                                    m_leftHippSurfaceSelectionModel,
                                                                    (parentObjectName + ":SelectLeftHippocampus"),
                                                                    "Select left hippocampus surface");
    QObject::connect(m_leftHippoSurfaceSelector, &SurfaceSelectionViewController::surfaceSelected,
                     this, &BrainBrowserWindowToolBarAllSurface::leftHippoSurfaceSelected);
    
    QLabel* rightLabel(new QLabel("Right"));
    m_rightHippoSurfaceSelector = new SurfaceSelectionViewController(this,
                                                                     m_rightHippSurfaceSelectionModel,
                                                                     (parentObjectName + ":SelectRightHippocampus"),
                                                                     "Select right hippocampus surface");
    QObject::connect(m_rightHippoSurfaceSelector, &SurfaceSelectionViewController::surfaceSelected,
                     this, &BrainBrowserWindowToolBarAllSurface::rightHippoSurfaceSelected);
    
    QLabel* dentateLeftLabel(new QLabel("Dentate Left"));
    m_dentateLeftHippoSurfaceSelector = new SurfaceSelectionViewController(this,
                                                                           m_leftDentateHippSurfaceSelectionModel,
                                                                           (parentObjectName + ":SelectDentateLeftHippocampus"),
                                                                           "Select left dentate hippocampus surface");
    QObject::connect(m_dentateLeftHippoSurfaceSelector, &SurfaceSelectionViewController::surfaceSelected,
                     this, &BrainBrowserWindowToolBarAllSurface::dentateLeftHippoSurfaceSelected);
    
    QLabel* dentateRightLabel(new QLabel("Dentate Right"));
    m_dentateRightHippoSurfaceSelector = new SurfaceSelectionViewController(this,
                                                                            m_rightDentateHippSurfaceSelectionModel,
                                                                            (parentObjectName + ":SelectDentateRightHippocampus"),
                                                                            "Select right dentate hippocampus surface");
    QObject::connect(m_dentateRightHippoSurfaceSelector, &SurfaceSelectionViewController::surfaceSelected,
                     this, &BrainBrowserWindowToolBarAllSurface::dentateRightHippoSurfaceSelected);

    QWidget* widget(new QWidget());
    QGridLayout* gridLayout(new QGridLayout(widget));
    int row(gridLayout->rowCount());
    gridLayout->addWidget(leftLabel, row, 0);
    gridLayout->addWidget(m_leftHippoSurfaceSelector->getWidget(), row, 1);
    row = gridLayout->rowCount();
    gridLayout->addWidget(rightLabel, row, 0);
    gridLayout->addWidget(m_rightHippoSurfaceSelector->getWidget(), row, 1);
    row = gridLayout->rowCount();
    gridLayout->addWidget(dentateLeftLabel, row, 0);
    gridLayout->addWidget(m_dentateLeftHippoSurfaceSelector->getWidget(), row, 1);
    row = gridLayout->rowCount();
    gridLayout->addWidget(dentateRightLabel, row, 0);
    gridLayout->addWidget(m_dentateRightHippoSurfaceSelector->getWidget(), row, 1);
    row = gridLayout->rowCount();
    return widget;
}

/**
 * Called when hippocampus surface selected
 * @param surface
 *    Surface that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::leftHippoSurfaceSelected(Surface* surface)
{
    selectSurface(StructureEnum::HIPPOCAMPUS_LEFT,
                  surface);
}

/**
 * Called when hippocampus surface selected
 * @param surface
 *    Surface that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::rightHippoSurfaceSelected(Surface* surface)
{
    selectSurface(StructureEnum::HIPPOCAMPUS_RIGHT,
                  surface);
}

/**
 * Called when hippocampus surface selected
 * @param surface
 *    Surface that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::dentateLeftHippoSurfaceSelected(Surface* surface)
{
    selectSurface(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT,
                  surface);
}

/**
 * Called when hippocampus surface selected
 * @param surface
 *    Surface that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::dentateRightHippoSurfaceSelected(Surface* surface)
{
    selectSurface(StructureEnum::HIPPOCAMPUS_DENTATE_RIGHT,
                  surface);
}

/**
 * Select a surface for the given structure for Whole Brain View
 * @param structure
 *    The structure
 * @param surface
 *    Surface that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::selectSurface(const StructureEnum::Enum structure,
                                                   Surface* surface)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    wholeBrainModel->setSelectedSurface(structure,
                                        btc->getTabNumber(),
                                        surface);
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when cerebellum surface is selected from menu
 *
 * @param action
 *     Action that was selected
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceCerebellumMenuTriggered(QAction* action)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    if (action != NULL) {
        QVariant data = action->data();
        void* p = data.value<void*>();
        Surface* surface = (Surface*)p;
        wholeBrainModel->setSelectedSurface(StructureEnum::CEREBELLUM,
                                            btc->getTabNumber(),
                                            surface);
        this->updateGraphicsWindowAndYokedWindows();
    }
}

/**
 * Called when whole brain surface right checkbox is toggled.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceRightCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainRightEnabled(this->wholeBrainSurfaceRightCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when whole brain cerebellum check box is toggled.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceCerebellumCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainCerebellumEnabled(this->wholeBrainSurfaceCerebellumCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when whole brain hippocampus check box is toggled.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceHippocampusCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainHippocampusEnabled(this->wholeBrainSurfaceHippocampusCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}


/**
 * Called when whole brain separation left/right spin box value is changed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double /*d*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainLeftRightSeparation(this->wholeBrainSurfaceSeparationLeftRightSpinBox->value());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when whole brain left&right/cerebellum spin box value is changed.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double /*d*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainCerebellumSeparation(this->wholeBrainSurfaceSeparationCerebellumSpinBox->value());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when match check box is clicked
 *
 * @param checked
 *     New checked status.
 */
void
BrainBrowserWindowToolBarAllSurface::wholeBrainSurfaceMatchCheckBoxClicked(bool checked)
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    
    brain->setSurfaceMatchingToAnatomical(checked);
    this->updateGraphicsWindowAndYokedWindows();
    this->updateUserInterface();
}
