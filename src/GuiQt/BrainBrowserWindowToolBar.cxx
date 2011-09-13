
/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <cassert>
#include <iostream>

#include <QActionGroup>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCleanlooksStyle>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabBar>
#include <QToolButton>

#include "BrainBrowserWindowToolBar.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param parent
 *    Parent for this toolbar.
 */
BrainBrowserWindowToolBar::BrainBrowserWindowToolBar(QWidget* parent)
{
    this->toolsToolBoxToolButtonAction = NULL;
    this->updateCounter = 0;
    
    /*
     * Create tab bar that displays models.
     */
    this->tabBar = new QTabBar();
    this->tabBar->setShape(QTabBar::RoundedNorth);
#ifdef Q_OS_MACX
    this->tabBar->setStyle(new QCleanlooksStyle());
#endif // Q_OS_MACX
    this->tabBar->addTab("One");
    this->tabBar->addTab("Two");
    
    /*
     * Create the toolbar's widgets.
     */
    this->viewWidget = this->createViewWidget();
    this->orientationWidget = this->createOrientationWidget();
    this->wholeBrainSurfaceOptionsWidget = this->createWholeBrainSurfaceOptionsWidget();
    this->volumeIndicesWidget = this->createVolumeIndicesWidget();
    this->toolsWidget = this->createToolsWidget();
    this->windowWidget = this->createWindowWidget();
    this->singleSurfaceSelectionWidget = this->createSingleSurfaceOptionsWidget();
    this->volumeMontageWidget = this->createVolumeMontageWidget();
    this->volumePlaneWidget = this->createVolumePlaneWidget();
    this->spacerWidget = new QWidget;
    
    /*
     * Layout the toolbar's widgets.
     */
    this->toolbarWidget = new QWidget();
    this->toolbarWidgetLayout = new QHBoxLayout(this->toolbarWidget);
    this->toolbarWidgetLayout->setSpacing(2);
    
    this->toolbarWidgetLayout->addWidget(this->viewWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->orientationWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->wholeBrainSurfaceOptionsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->singleSurfaceSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumePlaneWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeIndicesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeMontageWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->spacerWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->toolsWidget, 0, Qt::AlignRight);
    
    this->toolbarWidgetLayout->addWidget(this->windowWidget, 0, Qt::AlignRight);

    /*
     * Arrange the tabbar and the toolbar vertically.
     */
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 1, 2, 0);
    layout->addWidget(this->tabBar);
    layout->addWidget(this->toolbarWidget);
    
    this->addWidget(w);
    
    this->updateViewWidget();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBar::~BrainBrowserWindowToolBar()
{
    
}

/**
 * Update the toolbar.
 */
void 
BrainBrowserWindowToolBar::updateToolBar()
{
    this->incrementUpdateCounter(__func__);
    
    bool showOrientationWidget = false;
    bool showWholeBrainSurfaceOptionsWidget = false;
    bool showSingleSurfaceOptionsWidget = false;
    bool showVolumeIndicesWidget = false;
    bool showVolumePlaneWidget = false;
    bool showVolumeMontageWidget = false;
    
    bool showToolsWidget = true;
    bool showWindowWidget = true;
    
    int spacerWidgetStretchFactor = 0;
    int singleSurfaceWidgetStretchFactor = 0;
    
    ViewModeEnum::Enum viewMode = this->getViewMode();
    switch (viewMode) {
        case ViewModeEnum::VIEW_MODE_INVALID:
            break;
        case ViewModeEnum::VIEW_MODE_SURFACE:
            showOrientationWidget = true;
            showSingleSurfaceOptionsWidget = true;
            singleSurfaceWidgetStretchFactor = 100;
            break;
        case ViewModeEnum::VIEW_MODE_VOLUME_SLICES:
            showVolumeIndicesWidget = true;
            showVolumePlaneWidget = true;
            showVolumeMontageWidget = true;
            spacerWidgetStretchFactor = 100;
            break;
        case ViewModeEnum::VIEW_MODE_WHOLE_BRAIN:
            showOrientationWidget = true;
            showWholeBrainSurfaceOptionsWidget = true;
            showVolumeIndicesWidget = true;
            spacerWidgetStretchFactor = 100;
            break;
    }
    
    /*
     * Need to turn off display of all widgets, 
     * otherwise, the toolbar width may be overly
     * expanded with empty space as other widgets
     * are turned on and off.
     */
    this->orientationWidget->setVisible(false);
    this->wholeBrainSurfaceOptionsWidget->setVisible(false);
    this->singleSurfaceSelectionWidget->setVisible(false);
    this->volumeIndicesWidget->setVisible(false);
    this->volumePlaneWidget->setVisible(false);
    this->volumeMontageWidget->setVisible(false);
    this->toolsWidget->setVisible(false);
    this->windowWidget->setVisible(false);
    
    this->toolbarWidgetLayout->setStretchFactor(this->singleSurfaceSelectionWidget, 
                                              singleSurfaceWidgetStretchFactor);
    this->toolbarWidgetLayout->setStretchFactor(this->spacerWidget, 
                                              spacerWidgetStretchFactor);
    
    this->orientationWidget->setVisible(showOrientationWidget);
    this->wholeBrainSurfaceOptionsWidget->setVisible(showWholeBrainSurfaceOptionsWidget);
    this->singleSurfaceSelectionWidget->setVisible(showSingleSurfaceOptionsWidget);
    this->volumeIndicesWidget->setVisible(showVolumeIndicesWidget);
    this->volumePlaneWidget->setVisible(showVolumePlaneWidget);
    this->volumeMontageWidget->setVisible(showVolumeMontageWidget);
    this->toolsWidget->setVisible(showToolsWidget);
    this->windowWidget->setVisible(showWindowWidget);

    this->updateViewWidget();
    this->updateOrientationWidget();
    this->updateWholeBrainSurfaceOptionsWidget();
    this->updateVolumeIndicesWidget();
    this->updateToolsWidget();
    this->updateWindowWidget();
    this->updateSingleSurfaceOptionsWidget();
    this->updateVolumeMontageWidget();
    this->updateVolumePlaneWidget();

    this->decrementUpdateCounter(__func__);
    
    if (this->updateCounter != 0) {
        std::cout << "Update counter is non-zero at end of updateToolBar()" << std::endl;
    }
    
}

/**
 * Get the action for showing the toolbox.
 */
QAction* 
BrainBrowserWindowToolBar::getShowToolBoxAction()
{
    return this->toolsToolBoxToolButtonAction;
}

ViewModeEnum::Enum 
BrainBrowserWindowToolBar::getViewMode() const
{
    ViewModeEnum::Enum viewMode = ViewModeEnum::VIEW_MODE_SURFACE;
    
    if (this->viewModeSurfaceRadioButton->isChecked()) {
        viewMode = ViewModeEnum::VIEW_MODE_SURFACE;
    }
    else if (this->viewModeVolumeRadioButton->isChecked()) {
        viewMode = ViewModeEnum::VIEW_MODE_VOLUME_SLICES;
    }
    else if (this->viewModeWholeBrainRadioButton->isChecked()) {
        viewMode = ViewModeEnum::VIEW_MODE_WHOLE_BRAIN;
    }
    return viewMode;
}

/**
 * Create the view widget.
 *
 * @return The view widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createViewWidget()
{
    this->viewModeSurfaceRadioButton = new QRadioButton("Surface");
    this->viewModeVolumeRadioButton = new QRadioButton("Volume");
    this->viewModeWholeBrainRadioButton = new QRadioButton("Whole Brain");
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 2, 2);
    layout->addWidget(this->viewModeSurfaceRadioButton);
    layout->addWidget(this->viewModeVolumeRadioButton);
    layout->addWidget(this->viewModeWholeBrainRadioButton);
    layout->addStretch();

    QButtonGroup* viewModeRadioButtonGroup = new QButtonGroup(this);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeVolumeRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeWholeBrainRadioButton);
    QObject::connect(viewModeRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(viewModeRadioButtonClicked(QAbstractButton*)));
    
    this->viewWidgetGroup = new WuQWidgetObjectGroup(this);
    this->viewWidgetGroup->add(this->viewModeSurfaceRadioButton);
    this->viewWidgetGroup->add(this->viewModeVolumeRadioButton);
    this->viewWidgetGroup->add(this->viewModeWholeBrainRadioButton);
    
    QWidget* w = this->createToolWidget("View", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    return w;
}

/**
 * Update the view widget.
 */
void 
BrainBrowserWindowToolBar::updateViewWidget()
{
    this->incrementUpdateCounter(__func__);
    
    this->viewWidgetGroup->blockSignals(true);
    
    ViewModeEnum::Enum viewMode = this->getViewMode();

    switch (viewMode) {
        case ViewModeEnum::VIEW_MODE_INVALID:
            break;
        case ViewModeEnum::VIEW_MODE_SURFACE:
            this->viewModeSurfaceRadioButton->setChecked(true);
            break;
        case ViewModeEnum::VIEW_MODE_VOLUME_SLICES:
            this->viewModeVolumeRadioButton->setChecked(true);
            break;
        case ViewModeEnum::VIEW_MODE_WHOLE_BRAIN:
            this->viewModeWholeBrainRadioButton->setChecked(true);
            break;
    }
    
    this->viewWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the orientation widget.
 *
 * @return  The orientation widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createOrientationWidget()
{
    QIcon leftIcon;
    const bool leftIconValid =
        WuQtUtilities::loadIcon(":/view-left.png", 
                                leftIcon);
    
    QIcon rightIcon;
    const bool rightIconValid =
    WuQtUtilities::loadIcon(":/view-right.png", 
                            rightIcon);
    
    QIcon anteriorIcon;
    const bool anteriorIconValid =
    WuQtUtilities::loadIcon(":/view-anterior.png", 
                            anteriorIcon);
    
    QIcon posteriorIcon;
    const bool posteriorIconValid =
    WuQtUtilities::loadIcon(":/view-posterior.png", 
                            posteriorIcon);
    
    QIcon dorsalIcon;
    const bool dorsalIconValid =
    WuQtUtilities::loadIcon(":/view-dorsal.png", 
                            dorsalIcon);
    
    QIcon ventralIcon;
    const bool ventralIconValid =
    WuQtUtilities::loadIcon(":/view-ventral.png", 
                            ventralIcon);
    
    //QSize leftSize = leftIcon.actualSize(QSize(32,32));
    //std::cout << "Left size: " << leftSize.width() << ", " << leftSize.height() << std::endl;
    
    this->orientationLeftToolButtonAction = WuQtUtilities::createAction("L", 
                                                                        "View the brain from a LEFT perspective", 
                                                                        this, 
                                                                        this, 
                                                                        SLOT(orientationLeftToolButtonTriggered(bool)));
    if (leftIconValid) {
        this->orientationLeftToolButtonAction->setIcon(leftIcon);
    }
    
    this->orientationRightToolButtonAction = WuQtUtilities::createAction("R", 
                                                                         "View the brain from a RIGHT perspective", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationRightToolButtonTriggered(bool)));
    if (rightIconValid) {
        this->orientationRightToolButtonAction->setIcon(rightIcon);
    }
    
    this->orientationAnteriorToolButtonAction = WuQtUtilities::createAction("A", 
                                                                            "View the brain from an ANTERIOR perspective", 
                                                                            this, 
                                                                            this, 
                                                                            SLOT(orientationAnteriorToolButtonTriggered(bool)));
    if (anteriorIconValid) {
        this->orientationAnteriorToolButtonAction->setIcon(anteriorIcon);
    }
    
    this->orientationPosteriorToolButtonAction = WuQtUtilities::createAction("P", 
                                                                             "View the brain from a POSTERIOR perspective", 
                                                                             this, 
                                                                             this, 
                                                                             SLOT(orientationPosteriorToolButtonTriggered(bool)));
    if (posteriorIconValid) {
        this->orientationPosteriorToolButtonAction->setIcon(posteriorIcon);
    }
    
    this->orientationDorsalToolButtonAction = WuQtUtilities::createAction("D", 
                                                                          "View the brain from a DORSAL perspective", 
                                                                          this, 
                                                                          this, 
                                                                          SLOT(orientationDorsalToolButtonTriggered(bool)));
    if (dorsalIconValid) {
        this->orientationDorsalToolButtonAction->setIcon(dorsalIcon);
    }
    
    this->orientationVentralToolButtonAction = WuQtUtilities::createAction("V", 
                                                                           "View the brain from a VENTRAL perspective", 
                                                                           this, 
                                                                           this, 
                                                                           SLOT(orientationVentralToolButtonTriggered(bool)));
    if (ventralIconValid) {
        this->orientationVentralToolButtonAction->setIcon(ventralIcon);
    }
    
    this->orientationResetToolButtonAction = WuQtUtilities::createAction("Reset", 
                                                                         "Reset the view to dorsal and remove any panning or zooming", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));

    this->orientationUserViewOneToolButtonAction = WuQtUtilities::createAction("V1", 
                                                                                "Display the model using user view 1", 
                                                                                this, 
                                                                                this, 
                                                                                SLOT(orientationUserViewOneToolButtonTriggered(bool)));
    
    this->orientationUserViewTwoToolButtonAction = WuQtUtilities::createAction("V2", 
                                                                               "Display the model using user view 2", 
                                                                               this, 
                                                                               this, 
                                                                               SLOT(orientationUserViewTwoToolButtonTriggered(bool)));
    

    
    this->orientationUserViewSelectToolButtonMenu = new QMenu(this);
    QObject::connect(this->orientationUserViewSelectToolButtonMenu, SIGNAL(aboutToShow()),
                     this, SLOT(orientationUserViewSelectToolButtonMenuAboutToShow()));
    QObject::connect(this->orientationUserViewSelectToolButtonMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(orientationUserViewSelectToolButtonMenuTriggered(QAction*)));
    this->orientationUserViewSelectToolButtonMenu->addAction("View xx");
    this->orientationUserViewSelectToolButtonMenu->addAction("View yy");
    this->orientationUserViewSelectToolButtonMenu->addSeparator();
    this->orientationUserViewSelectToolButtonMenu->addAction("Set User View 1");
    this->orientationUserViewSelectToolButtonMenu->addAction("Set User View 2");
    this->orientationUserViewSelectToolButtonMenu->addSeparator();
    this->orientationUserViewSelectToolButtonMenu->addAction("Add View...");
    this->orientationUserViewSelectToolButtonMenu->addAction("Edit Views...");
    
    this->orientationUserViewSelectToolButtonAction = new QAction(this);
    this->orientationUserViewSelectToolButtonAction->setText("View");
    this->orientationUserViewSelectToolButtonAction->setMenu(this->orientationUserViewSelectToolButtonMenu);
    this->orientationUserViewSelectToolButtonAction->setToolTip("Add, edit, and select user-defined views");
    this->orientationUserViewSelectToolButtonAction->setStatusTip("Add, edit, and select user-defined views");


    QToolButton* orientationLeftToolButton = new QToolButton();
    orientationLeftToolButton->setDefaultAction(this->orientationLeftToolButtonAction);
    
    QToolButton* orientationRightToolButton = new QToolButton();
    orientationRightToolButton->setDefaultAction(this->orientationRightToolButtonAction);
    
    QToolButton* orientationAnteriorToolButton = new QToolButton();
    orientationAnteriorToolButton->setDefaultAction(this->orientationAnteriorToolButtonAction);
    
    QToolButton* orientationPosteriorToolButton = new QToolButton();
    orientationPosteriorToolButton->setDefaultAction(this->orientationPosteriorToolButtonAction);
    
    QToolButton* orientationDorsalToolButton = new QToolButton();
    orientationDorsalToolButton->setDefaultAction(this->orientationDorsalToolButtonAction);
    
    QToolButton* orientationVentralToolButton = new QToolButton();
    orientationVentralToolButton->setDefaultAction(this->orientationVentralToolButtonAction);
    
    QToolButton* orientationResetToolButton = new QToolButton();
    orientationResetToolButton->setDefaultAction(this->orientationResetToolButtonAction);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(buttonLayout, 0, 2, 0);
    buttonLayout->addWidget(orientationLeftToolButton);
    buttonLayout->addWidget(orientationRightToolButton);
    buttonLayout->addWidget(orientationAnteriorToolButton);
    buttonLayout->addWidget(orientationPosteriorToolButton);
    buttonLayout->addWidget(orientationDorsalToolButton);
    buttonLayout->addWidget(orientationVentralToolButton);
    
    QToolButton* userViewOneToolButton = new QToolButton();
    userViewOneToolButton->setDefaultAction(this->orientationUserViewOneToolButtonAction);
    
    QToolButton* userViewTwoToolButton = new QToolButton();
    userViewTwoToolButton->setDefaultAction(this->orientationUserViewTwoToolButtonAction);
    
    QToolButton* orientationUserViewSelectToolButton = new QToolButton();
    orientationUserViewSelectToolButton->setDefaultAction(this->orientationUserViewSelectToolButtonAction);
    
    QHBoxLayout* userOrientLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(userOrientLayout, 0, 2, 0);
    userOrientLayout->addWidget(userViewOneToolButton);
    userOrientLayout->addWidget(userViewTwoToolButton);
    //userOrientLayout->addWidget(orientationUserViewAddToolButton);
    userOrientLayout->addWidget(orientationUserViewSelectToolButton);
    userOrientLayout->addStretch();
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(buttonLayout);
    layout->addLayout(userOrientLayout);
    layout->addWidget(orientationResetToolButton, 0, Qt::AlignHCenter);
    
    this->orientationWidgetGroup = new WuQWidgetObjectGroup(this);
    this->orientationWidgetGroup->add(this->orientationLeftToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationRightToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationAnteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationPosteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationDorsalToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationVentralToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationResetToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewOneToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewTwoToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewSelectToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewSelectToolButtonMenu);

    QWidget* orientWidget = this->createToolWidget("Orientation", w, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    orientWidget->setVisible(false);
    
    return orientWidget;
}

/**
 * Update the orientation widget.
 */
void 
BrainBrowserWindowToolBar::updateOrientationWidget()
{
    if (this->orientationWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->orientationWidgetGroup->blockSignals(true);
    
    this->orientationWidgetGroup->blockSignals(false);
        
    this->decrementUpdateCounter(__func__);
}

/**
 * Create the whole brain surface options widget.
 *
 * @return The whole brain surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createWholeBrainSurfaceOptionsWidget()
{
    
    this->wholeBrainSurfaceTypeComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceTypeComboBox,
                                          "Select the geometric type of surface for display");
    QObject::connect(this->wholeBrainSurfaceTypeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(wholeBrainSurfaceTypeComboBoxIndexChanged(int)));
    
    this->wholeBrainSurfaceLeftCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceLeftMenu = new QMenu("Left", this);
    QObject::connect(this->wholeBrainSurfaceLeftMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceLeftMenuTriggered(QAction*)));
    QToolButton* wholeBrainLeftSurfaceToolButton = new QToolButton();
    wholeBrainLeftSurfaceToolButton->setText("Left");
    wholeBrainLeftSurfaceToolButton->setMenu(this->wholeBrainSurfaceLeftMenu);
        WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceLeftCheckBox,
                                          "Enable/Disable display of the left cortical surface");
    QObject::connect(this->wholeBrainSurfaceLeftCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceLeftCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceRightCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceRightMenu = new QMenu("Right", this);
    QObject::connect(this->wholeBrainSurfaceRightMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceRightMenuTriggered(QAction*)));
    QToolButton* wholeBrainRightSurfaceToolButton = new QToolButton();
    wholeBrainRightSurfaceToolButton->setText("Right");
    wholeBrainRightSurfaceToolButton->setMenu(this->wholeBrainSurfaceRightMenu);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceRightCheckBox,
                                          "Enable/Disable display of the right cortical surface");
    QObject::connect(this->wholeBrainSurfaceRightCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceRightCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceCerebellumCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceCerebellumMenu = new QMenu("Cerebellum", this);
    QObject::connect(this->wholeBrainSurfaceCerebellumMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceCerebellumMenuTriggered(QAction*)));
    QToolButton* wholeBrainCerebellumSurfaceToolButton = new QToolButton();
    wholeBrainCerebellumSurfaceToolButton->setText("Cerebellum");
    wholeBrainCerebellumSurfaceToolButton->setMenu(this->wholeBrainSurfaceCerebellumMenu);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceCerebellumCheckBox,
                                          "Enable/Disable display of the cerebellum surface");
    QObject::connect(this->wholeBrainSurfaceCerebellumCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceCerebellumCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceSeparationLeftRightSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationLeftRightSpinBox,
                                          "Adjust the separation of the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationLeftRightSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double)));
    
    this->wholeBrainSurfaceSeparationCerebellumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationCerebellumSpinBox,
                                          "Adjust the separation of the cerebellum from the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationCerebellumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double)));
    
    
    QLabel* separationLabel = new QLabel("Separation");
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(4);
    gridLayout->setHorizontalSpacing(4);
    gridLayout->addWidget(this->wholeBrainSurfaceTypeComboBox, 0, 0, 1, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceLeftCheckBox, 1, 0);
    gridLayout->addWidget(wholeBrainLeftSurfaceToolButton, 1, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceRightCheckBox, 1, 2);
    gridLayout->addWidget(wholeBrainRightSurfaceToolButton, 1, 3);
    gridLayout->addWidget(this->wholeBrainSurfaceCerebellumCheckBox, 2, 0);
    gridLayout->addWidget(wholeBrainCerebellumSurfaceToolButton, 2, 1, 1, 3);
    gridLayout->addWidget(separationLabel, 0, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationLeftRightSpinBox, 1, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationCerebellumSpinBox, 2, 4);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    
    this->wholeBrainSurfaceOptionsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceTypeComboBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceLeftCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceLeftMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainLeftSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceRightCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceRightMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainRightSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceCerebellumCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceCerebellumMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainCerebellumSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationLeftRightSpinBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationCerebellumSpinBox);
    
    QWidget* w = this->createToolWidget("Surface Options", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    w->setVisible(false);
    return w;
}

/**
 * Update the whole brain surface options widget.
 */
void 
BrainBrowserWindowToolBar::updateWholeBrainSurfaceOptionsWidget()
{
    if (this->wholeBrainSurfaceOptionsWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->wholeBrainSurfaceOptionsWidgetGroup->blockSignals(true);
    
    this->wholeBrainSurfaceOptionsWidgetGroup->blockSignals(false);
    
    this->decrementUpdateCounter(__func__);
}

/**
 * Create the volume indices widget.
 *
 * @return  The volume indices widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeIndicesWidget()
{
    QLabel* parasagittalLabel = new QLabel("P:");
    QLabel* coronalLabel = new QLabel("C:");
    QLabel* axialLabel = new QLabel("A:");
    
    this->volumeIndicesParasagittalCheckBox = new QCheckBox("");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesParasagittalCheckBox,
                                          "Enable/Disable display of PARASAGITTAL slice");
    QObject::connect(this->volumeIndicesParasagittalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesParasagittalCheckBoxStateChanged(int)));
    
    this->volumeIndicesCoronalCheckBox = new QCheckBox("");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesCoronalCheckBox,
                                          "Enable/Disable display of CORONAL slice");
    QObject::connect(this->volumeIndicesCoronalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesCoronalCheckBoxStateChanged(int)));
    
    this->volumeIndicesAxialCheckBox = new QCheckBox("");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesAxialCheckBox,
                                          "Enable/Disable display of AXIAL slice");
    
    QObject::connect(this->volumeIndicesAxialCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesAxialCheckBoxStateChanged(int)));
    
    this->volumeIndicesParasagittalSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesParasagittalSpinBox,
                                          "Change the selected PARASAGITTAL slice");
    QObject::connect(this->volumeIndicesParasagittalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesParasagittalSpinBoxValueChanged(int)));
    
    this->volumeIndicesCoronalSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesCoronalSpinBox,
                                          "Change the selected CORONAL slice");
    QObject::connect(this->volumeIndicesCoronalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesCoronalSpinBoxValueChanged(int)));
    
    this->volumeIndicesAxialSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesAxialSpinBox,
                                          "Change the selected AXIAL slice");
    QObject::connect(this->volumeIndicesAxialSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesAxialSpinBoxValueChanged(int)));
    
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(this->volumeIndicesParasagittalCheckBox, 0, 0);
    gridLayout->addWidget(parasagittalLabel, 0, 1);
    gridLayout->addWidget(this->volumeIndicesParasagittalSpinBox, 0, 2);
    gridLayout->addWidget(this->volumeIndicesCoronalCheckBox, 1, 0);
    gridLayout->addWidget(coronalLabel, 1, 1);
    gridLayout->addWidget(this->volumeIndicesCoronalSpinBox, 1, 2);
    gridLayout->addWidget(this->volumeIndicesAxialCheckBox, 2, 0);
    gridLayout->addWidget(axialLabel, 2, 1);
    gridLayout->addWidget(this->volumeIndicesAxialSpinBox, 2, 2);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    
    this->volumeIndicesWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesParasagittalCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesParasagittalSpinBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesCoronalCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesCoronalSpinBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesAxialCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesAxialSpinBox);
    
    QWidget* w = this->createToolWidget("Volume Indices", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume indices widget.
 */
void 
BrainBrowserWindowToolBar::updateVolumeIndicesWidget()
{
    if (this->volumeIndicesWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->volumeIndicesWidgetGroup->blockSignals(true);
    
    this->volumeIndicesWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the tools widget.
 *
 * @return The tools widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createToolsWidget()
{
    
    this->toolsToolBoxToolButtonAction = WuQtUtilities::createAction("Toolbox", 
                                                                     "Show/Hide the Toolbox", 
                                                                     this, 
                                                                     this, 
                                                                     SLOT(toolsToolBoxToolButtonTriggered(bool)));
    QToolButton* toolBoxToolButton = new QToolButton();
    toolBoxToolButton->setDefaultAction(this->toolsToolBoxToolButtonAction);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addStretch();
    layout->addWidget(toolBoxToolButton);
    
    this->toolsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->toolsWidgetGroup->add(this->toolsToolBoxToolButtonAction);
    
    QWidget* w = this->createToolWidget("Tools", widget, WIDGET_PLACEMENT_LEFT, WIDGET_PLACEMENT_BOTTOM);
    return w;
}

/**
 * Update the tools widget.
 */
void 
BrainBrowserWindowToolBar::updateToolsWidget()
{
    if (this->toolsWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->toolsWidgetGroup->blockSignals(true);
    
    this->toolsWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the window (yoking) widget.
 *
 * @return  The window (yoking) widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createWindowWidget()
{
    QLabel* yokeToLabel = new QLabel("Yoke to:");
    this->windowYokeToTabComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->windowYokeToTabComboBox,
                                          "Select the tab to which the current tab is yoked (view linked)");
    QObject::connect(this->windowYokeToTabComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(windowYokeToTabComboBoxIndexChanged(int)));
    
    this->windowYokeMirroredCheckBox = new QCheckBox("Mirrored");
    WuQtUtilities::setToolTipAndStatusTip(this->windowYokeMirroredCheckBox,
                                          "Enable/Disable left/right mirroring when yoked");
    QObject::connect(this->windowYokeMirroredCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(windowYokeMirroredCheckBoxStateChanged(int)));
    

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 4, 2, 0);
    layout->addWidget(yokeToLabel);
    layout->addWidget(this->windowYokeToTabComboBox);
    layout->addWidget(this->windowYokeMirroredCheckBox);
    
    this->windowWidgetGroup = new WuQWidgetObjectGroup(this);
    this->windowWidgetGroup->add(this->windowYokeToTabComboBox);
    this->windowWidgetGroup->add(this->windowYokeMirroredCheckBox);
    
    QWidget* w = this->createToolWidget("Window", widget, WIDGET_PLACEMENT_LEFT, WIDGET_PLACEMENT_TOP);
    return w;
}

/**
 * Update the window widget.
 */
void 
BrainBrowserWindowToolBar::updateWindowWidget()
{
    if (this->windowWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->windowWidgetGroup->blockSignals(true);
    
    this->windowWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the single surface options widget.
 *
 * @return  The single surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createSingleSurfaceOptionsWidget()
{
    QLabel* structureSurfaceLabel = new QLabel("Brain Structure and Surface: ");
    
    this->surfaceSurfaceSelectionComboBox = new QComboBox();
    this->surfaceSurfaceSelectionComboBox->addItem("Surface Anatomical");
    this->surfaceSurfaceSelectionComboBox->addItem("Surface Inflated");
    this->surfaceSurfaceSelectionComboBox->addItem("Surface Very Inflated");
    
    WuQtUtilities::setToolTipAndStatusTip(this->surfaceSurfaceSelectionComboBox,
                                          "Select the displayed surface");
    QObject::connect(this->surfaceSurfaceSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(surfaceSurfaceSelectionComboBoxIndexChanged(int)));

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 6, 2, 2);
    layout->addWidget(structureSurfaceLabel);
    layout->addWidget(this->surfaceSurfaceSelectionComboBox);
    layout->addStretch();
    
    
    this->singleSurfaceSelectionWidgetGroup = new WuQWidgetObjectGroup(this);
    this->singleSurfaceSelectionWidgetGroup->add(this->surfaceSurfaceSelectionComboBox);

    QWidget* w = this->createToolWidget("Selection", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    w->setVisible(false);
    return w;
}

/**
 * Update the single surface options widget.
 */
void 
BrainBrowserWindowToolBar::updateSingleSurfaceOptionsWidget()
{
    if (this->singleSurfaceSelectionWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->singleSurfaceSelectionWidgetGroup->blockSignals(true);
    
    this->singleSurfaceSelectionWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the volume montage widget.
 *
 * @return The volume montage widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeMontageWidget()
{
    QLabel* rowsLabel = new QLabel("Rows:");
    QLabel* columnsLabel = new QLabel("Columns:");
    QLabel* spacingLabel = new QLabel("Spacing:");
    
    this->montageRowsSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageRowsSpinBox,
                                          "Select the number of rows in montage of volume slices");
    QObject::connect(this->montageRowsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageRowsSpinBoxValueChanged(int)));
    
    this->montageColumnsSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageColumnsSpinBox,
                                          "Select the number of columns in montage of volume slices");
    QObject::connect(this->montageColumnsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageColumnsSpinBoxValueChanged(int)));

    this->montageSpacingSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageSpacingSpinBox,
                                          "Select the number of slices skipped between displayed montage slices");
    QObject::connect(this->montageSpacingSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageSpacingSpinBoxValueChanged(int)));
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(rowsLabel, 0, 0);
    gridLayout->addWidget(this->montageRowsSpinBox, 0, 1);
    gridLayout->addWidget(columnsLabel, 1, 0);
    gridLayout->addWidget(this->montageColumnsSpinBox, 1, 1);
    gridLayout->addWidget(spacingLabel, 2, 0);
    gridLayout->addWidget(this->montageSpacingSpinBox, 2, 1);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    
    this->volumeMontageWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumeMontageWidgetGroup->add(this->montageRowsSpinBox);
    this->volumeMontageWidgetGroup->add(this->montageColumnsSpinBox);
    this->volumeMontageWidgetGroup->add(this->montageSpacingSpinBox);
    
    QWidget* w = this->createToolWidget("Montage", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume montage widget.
 */
void 
BrainBrowserWindowToolBar::updateVolumeMontageWidget()
{
    if (this->volumeMontageWidget->isHidden()) {
        return;
    }

    this->incrementUpdateCounter(__func__);
    
    this->volumeMontageWidgetGroup->blockSignals(true);
    
    this->volumeMontageWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create the volume plane widget.
 *
 * @return The volume plane widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumePlaneWidget()
{
    QIcon parasagittalIcon;
    const bool parasagittalIconValid =
    WuQtUtilities::loadIcon(":/view-plane-parasagittal.png", 
                            parasagittalIcon);

    QIcon coronalIcon;
    const bool coronalIconValid =
    WuQtUtilities::loadIcon(":/view-plane-coronal.png", 
                            coronalIcon);

    QIcon axialIcon;
    const bool axialIconValid =
    WuQtUtilities::loadIcon(":/view-plane-axial.png", 
                            axialIcon);
    
    this->volumePlaneParasagittalToolButtonAction = WuQtUtilities::createAction("P", 
                                                                                      "View the PARASAGITTAL slice", 
                                                                                      this);
    this->volumePlaneParasagittalToolButtonAction->setCheckable(true);
    if (parasagittalIconValid) {
        this->volumePlaneParasagittalToolButtonAction->setIcon(parasagittalIcon);
    }
    
    this->volumePlaneCoronalToolButtonAction = WuQtUtilities::createAction("C", 
                                                                                 "View the CORONAL slice", 
                                                                                 this);
    this->volumePlaneCoronalToolButtonAction->setCheckable(true);
    if (coronalIconValid) {
        this->volumePlaneCoronalToolButtonAction->setIcon(coronalIcon);
    }

    this->volumePlaneAxialToolButtonAction = WuQtUtilities::createAction("A", 
                                                                               "View the AXIAL slice", 
                                                                               this);
    this->volumePlaneAxialToolButtonAction->setCheckable(true);
    if (axialIconValid) {
        this->volumePlaneAxialToolButtonAction->setIcon(axialIcon);
    }

    this->volumePlaneAllToolButtonAction = WuQtUtilities::createAction("All", 
                                                                             "View the PARASAGITTAL, CORONAL, and AXIAL slices", 
                                                                             this);
    this->volumePlaneAllToolButtonAction->setCheckable(true);
    

    this->volumePlaneObliqueToolButtonAction = WuQtUtilities::createAction("O",
                                                                           "View the volume obliquely",                                                                           
                                                                           this);
    this->volumePlaneObliqueToolButtonAction->setCheckable(true);
    
    this->volumePlaneMontageToolButtonAction = WuQtUtilities::createAction("M",
                                                                           "View a montage of slices",                                                                           
                                                                           this);
    this->volumePlaneMontageToolButtonAction->setCheckable(true);
    
    this->volumePlaneResetToolButtonAction = WuQtUtilities::createAction("Reset", 
                                                                               "Reset to the slices to those with stereotaxic coordinate (0, 0, 0) and remove panning/zooming", 
                                                                               this, 
                                                                               this, 
                                                                               SLOT(volumePlaneResetToolButtonTriggered(bool)));
    
    this->volumePlaneActionGroup = new QActionGroup(this);
    this->volumePlaneActionGroup->addAction(this->volumePlaneParasagittalToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneCoronalToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneAxialToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneAllToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneMontageToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneObliqueToolButtonAction);
    this->volumePlaneActionGroup->setExclusive(true);
    QObject::connect(this->volumePlaneActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneActionGroupTriggered(QAction*)));
    
    
    QToolButton* volumePlaneParasagittalToolButton = new QToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(this->volumePlaneParasagittalToolButtonAction);
    
    QToolButton* volumePlaneCoronalToolButton = new QToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(this->volumePlaneCoronalToolButtonAction);
    
    QToolButton* volumePlaneAxialToolButton = new QToolButton();
    volumePlaneAxialToolButton->setDefaultAction(this->volumePlaneAxialToolButtonAction);
    
    QToolButton* volumePlaneAllToolButton = new QToolButton();
    volumePlaneAllToolButton->setDefaultAction(this->volumePlaneAllToolButtonAction);
    
    QToolButton* volumePlaneMontageToolButton = new QToolButton();
    volumePlaneMontageToolButton->setDefaultAction(this->volumePlaneMontageToolButtonAction);
    
    QToolButton* volumePlaneObliqueToolButton = new QToolButton();
    volumePlaneObliqueToolButton->setDefaultAction(this->volumePlaneObliqueToolButtonAction);
    
    QToolButton* volumePlaneResetToolButton = new QToolButton();
    volumePlaneResetToolButton->setDefaultAction(this->volumePlaneResetToolButtonAction);
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton,
                                      volumePlaneAllToolButton);
    
    
    QHBoxLayout* planeLayout1 = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(planeLayout1, 0, 2, 0);
    planeLayout1->addWidget(volumePlaneParasagittalToolButton);
    planeLayout1->addWidget(volumePlaneCoronalToolButton);
    planeLayout1->addWidget(volumePlaneAxialToolButton);
    planeLayout1->addWidget(volumePlaneAllToolButton);
    planeLayout1->addStretch();

    QHBoxLayout* planeLayout2 = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(planeLayout2, 0, 2, 0);
    planeLayout2->addWidget(volumePlaneMontageToolButton);
    planeLayout2->addWidget(volumePlaneObliqueToolButton);
    planeLayout2->addWidget(volumePlaneResetToolButton);
    planeLayout2->addStretch();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(planeLayout1);
    layout->addLayout(planeLayout2);
    
    this->volumePlaneWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumePlaneWidgetGroup->add(this->volumePlaneActionGroup);
    this->volumePlaneWidgetGroup->add(this->volumePlaneResetToolButtonAction);
    
    QWidget* w = this->createToolWidget("Plane", widget, WIDGET_PLACEMENT_RIGHT, WIDGET_PLACEMENT_TOP);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume plane orientation widget.
 */
void 
BrainBrowserWindowToolBar::updateVolumePlaneWidget()
{
    if (this->volumePlaneWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__func__);
    
    this->volumePlaneWidgetGroup->blockSignals(true);
    
    this->volumePlaneWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__func__);
}

/**
 * Create a tool widget which is a group of widgets with 
 * a descriptive label added.
 *
 * @param name
 *    Name for the descriptive labe.
 * @param childWidget
 *    Child widget that is in the tool widget.
 * @param verticalBarPlacement
 *    Where to place a vertical bar.  Values other than right or 
 *    left are ignored in which case no vertical bar is displayed.
 * @param contentPlacement
 *    Where to place widget which must be top or bottom.
 * @return The tool widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createToolWidget(const QString& name,
                                            QWidget* childWidget,
                                            const WidgetPlacement verticalBarPlacement,
                                            const WidgetPlacement contentPlacement)
{
    //QLabel* nameLabel = new QLabel("<html><b>" + name + "<b></html>");
    QLabel* nameLabel = new QLabel("<html>" + name + "</html>");
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 2, 2, 0);
    switch (contentPlacement) {
        case WIDGET_PLACEMENT_BOTTOM:
            layout->addStretch();
            layout->addWidget(childWidget);
            break;
        case WIDGET_PLACEMENT_TOP:
            layout->addWidget(childWidget);
            layout->addStretch();
            break;
        default:
            assert(0);
    }
    layout->addWidget(nameLabel, 0, Qt::AlignHCenter);
    
    const bool addVerticalBarOnLeftSide = (verticalBarPlacement == WIDGET_PLACEMENT_LEFT);
    const bool addVerticalBarOnRightSide = (verticalBarPlacement == WIDGET_PLACEMENT_RIGHT);
    
    if (addVerticalBarOnLeftSide
        || addVerticalBarOnRightSide) {
        QWidget* w2 = new QWidget();
        QHBoxLayout* horizLayout = new QHBoxLayout(w2);
        WuQtUtilities::setLayoutMargins(horizLayout, 0, 0, 0);
        if (addVerticalBarOnLeftSide) {
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
            horizLayout->addSpacing(3);
        }
        horizLayout->addWidget(w);
        if (addVerticalBarOnRightSide) {
            horizLayout->addSpacing(3);
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
        }
        w = w2;
    }
    
    return w;
}


/**
 * Called when the selected tab is changed.
 */
void 
BrainBrowserWindowToolBar::tabBarIndexChanged(int indx)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when a view mode is selected.
 */
void 
BrainBrowserWindowToolBar::viewModeRadioButtonClicked(QAbstractButton*)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
    this->updateToolBar();    
}

/**
 * Called when orientation left button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationLeftToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation right button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationRightToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation anterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationAnteriorToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation posterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationPosteriorToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation dorsal button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationDorsalToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation ventral button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationVentralToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation reset button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationResetToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view one button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewOneToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view one button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewTwoToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view menu is about to display.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewSelectToolButtonMenuAboutToShow()
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view selection is made from the menu.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewSelectToolButtonMenuTriggered(QAction* action)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when the whole brain surface type combo box is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceTypeComboBoxIndexChanged(int indx)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when whole brain surface left check box is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceLeftCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceLeftMenuTriggered(QAction*)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

void 
BrainBrowserWindowToolBar::wholeBrainSurfaceRightMenuTriggered(QAction*)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

void 
BrainBrowserWindowToolBar::wholeBrainSurfaceCerebellumMenuTriggered(QAction*)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when whole brain surface right checkbox is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceRightCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when whole brain cerebellum check box is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceCerebellumCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when whole brain separation left/right spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double d)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when whole brain left&right/cerebellum spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double d)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices parasagittal check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesParasagittalCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices coronal check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesCoronalCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices axial check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesAxialCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices parasagittal spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesParasagittalSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices coronal spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesCoronalSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume indices axial spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesAxialSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when window yoke to tab combo box is selected.
 */
void 
BrainBrowserWindowToolBar::windowYokeToTabComboBoxIndexChanged(int indx)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when yoke mirrored checkbox is toggled.
 */
void 
BrainBrowserWindowToolBar::windowYokeMirroredCheckBoxStateChanged(int state)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when single surface selection combo box is changed.
 */
void 
BrainBrowserWindowToolBar::surfaceSurfaceSelectionComboBoxIndexChanged(int indx)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when volume slice plane button is clicked.
 */
void 
BrainBrowserWindowToolBar::volumePlaneActionGroupTriggered(QAction* action)
{
    std::cout << __func__ << std::endl;
    
    if (action == this->volumePlaneAllToolButtonAction) {
        
    }
    else if (action == this->volumePlaneAxialToolButtonAction) {
        
    }
    else if (action == this->volumePlaneCoronalToolButtonAction) {
        
    }
    else if (action == this->volumePlaneParasagittalToolButtonAction) {
    
    }
    else if (action == this->volumePlaneMontageToolButtonAction) {
        
    }
    else if (action == this->volumePlaneObliqueToolButtonAction) {
        
    }
    else {
        std::cerr << "Invalid volume plane action: " 
        << qPrintable(action->text())
        << std::endl;
    }
    this->checkUpdateCounter();
}

/**
 * Called when volume reset slice view button is pressed.
 */
void 
BrainBrowserWindowToolBar::volumePlaneResetToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when montage rows spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageRowsSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when montage columns spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageColumnsSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when montage spacing spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageSpacingSpinBoxValueChanged(int i)
{
    std::cout << __func__ << std::endl;
    this->checkUpdateCounter();
}

/**
 * Called when tools toolbox button is toggled.
 */
void 
BrainBrowserWindowToolBar::toolsToolBoxToolButtonTriggered(bool checked)
{
    std::cout << __func__ << std::endl; 
    this->checkUpdateCounter();
}

void 
BrainBrowserWindowToolBar::checkUpdateCounter()
{
    if (this->updateCounter != 0) {
        std::cout << "Update counter is non-zero, signal needs to be blocked during update." << std::endl;        
        std::cout << "Value=" << updateCounter << std::endl;
    }
}

void 
BrainBrowserWindowToolBar::incrementUpdateCounter(const char* methodName)
{
    this->updateCounter++;
    //std::cout << methodName << " enter: " << this->updateCounter << std::endl;    
}

void 
BrainBrowserWindowToolBar::decrementUpdateCounter(const char* methodName)
{
    //std::cout << methodName << " exit: " << this->updateCounter << std::endl;    
    this->updateCounter--;
}

