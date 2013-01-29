
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __CUSTOM_VIEW_DIALOG_DECLARE__
#include "CustomViewDialog.h"
#undef __CUSTOM_VIEW_DIALOG_DECLARE__

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrowserTabContent.h"
#include "CaretPreferences.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelYokingGroup.h"
#include "SessionManager.h"
#include "UserView.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::CustomViewDialog 
 * \brief Dialog for creation of custom views (orientations).
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
CustomViewDialog::CustomViewDialog(QWidget* parent)
: WuQDialogNonModal("Custom Orientation",
                    parent)
{
    m_blockDialogUpdate = true;
    
    /*
     * Remove apply button by using an empty name
     */
    setApplyButtonText("");
    
    QWidget* customViewWidget = createCustomViewWidget();
    QWidget* transformWidget = createTransformsWidget();
    WuQtUtilities::matchWidgetHeights(customViewWidget,
                                      transformWidget);
    
    /*
     * Layout for dialog
     */
    QWidget* widget = new QWidget();
    QHBoxLayout* sceneButtonLayout = new QHBoxLayout(widget);
    sceneButtonLayout->addWidget(transformWidget);
    sceneButtonLayout->addWidget(customViewWidget);
    setCentralWidget(widget);
    
    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
 
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    
    m_blockDialogUpdate = false;
}

/**
 * Destructor.
 */
CustomViewDialog::~CustomViewDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return The Custom View widget.
 */
QWidget*
CustomViewDialog::createCustomViewWidget()
{
    m_loadCustomViewPushButton = new QPushButton("Load...");
    QObject::connect(m_loadCustomViewPushButton, SIGNAL(clicked()),
                     this, SLOT(loadCustomViewPushButtonClicked()));
    
    m_saveCustomViewPushButton = new QPushButton("Save...");
    QObject::connect(m_saveCustomViewPushButton, SIGNAL(clicked()),
                     this, SLOT(saveCustomViewPushButtonClicked()));
    
    QGroupBox* groupBox = new QGroupBox("Custom");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_loadCustomViewPushButton);
    layout->addWidget(m_saveCustomViewPushButton);
    layout->addStretch();
    
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);

    return groupBox;
}

/**
 * @return The Transform widget.
 */
QWidget*
CustomViewDialog::createTransformsWidget()
{
    const int spinBoxWidth = 70;
    
    /*
     * Window number
     */
    QLabel* windowLabel = new QLabel("Window: ");
    m_browserWindowComboBox = new BrainBrowserWindowComboBox(this);
    m_browserWindowComboBox->getWidget()->setFixedWidth(spinBoxWidth);
    QObject::connect(m_browserWindowComboBox, SIGNAL(browserWindowSelected(BrainBrowserWindow*)),
                     this, SLOT(browserWindowComboBoxValueChanged(BrainBrowserWindow*)));
    
    /*
     * Panning
     */
    const double panStep = 1.0;
    QLabel* panLabel = new QLabel("Pan (X,Y):");
    m_xPanDoubleSpinBox = new QDoubleSpinBox;
    m_xPanDoubleSpinBox->setMinimum(-100000.0);
    m_xPanDoubleSpinBox->setMaximum( 100000.0);
    m_xPanDoubleSpinBox->setSingleStep(panStep);
    m_xPanDoubleSpinBox->setDecimals(2);
    m_xPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yPanDoubleSpinBox = new QDoubleSpinBox;
    m_yPanDoubleSpinBox->setMinimum(-100000.0);
    m_yPanDoubleSpinBox->setMaximum( 100000.0);
    m_yPanDoubleSpinBox->setSingleStep(panStep);
    m_yPanDoubleSpinBox->setDecimals(2);
    m_yPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Rotation
     */
    const double rotationMinimum = -360.0;
    const double rotationMaximum =  360.0;
    const double rotateStep = 1.0;
    QLabel* rotateLabel = new QLabel("Rotate (X,Y,Z): ");
    m_xRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xRotateDoubleSpinBox->setWrapping(true);
    m_xRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_xRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_xRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_xRotateDoubleSpinBox->setDecimals(2);
    m_xRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yRotateDoubleSpinBox->setWrapping(true);
    m_yRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_yRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_yRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_yRotateDoubleSpinBox->setDecimals(2);
    m_yRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zRotateDoubleSpinBox->setWrapping(true);
    m_zRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_zRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_zRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_zRotateDoubleSpinBox->setDecimals(2);
    m_zRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Zoom
     */
    const double zoomStep = 0.1;
    QLabel* zoomLabel = new QLabel("Zoom: ");
    m_zoomDoubleSpinBox = new QDoubleSpinBox;
    m_zoomDoubleSpinBox->setMinimum(0.001);
    m_zoomDoubleSpinBox->setMaximum(10000.0);
    m_zoomDoubleSpinBox->setSingleStep(zoomStep);
    m_zoomDoubleSpinBox->setDecimals(3);
    m_zoomDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zoomDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    m_transformWidgetGroup = new WuQWidgetObjectGroup(this);
    m_transformWidgetGroup->add(m_xPanDoubleSpinBox);
    m_transformWidgetGroup->add(m_yPanDoubleSpinBox);
    m_transformWidgetGroup->add(m_xRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_yRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_zRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_zoomDoubleSpinBox);
    
    /*------------------------------------------------------------------------*/
    /*
     * Layout widgets
     */
    /*
     * Columns for grid layout
     */
    int column = 0;
    const int COLUMN_LABEL  = column++;
    const int COLUMN_X      = column++;
    const int COLUMN_Y      = column++;
    const int COLUMN_Z      = column++;
    const int COLUMN_COUNT  = column++;
    
    QGroupBox* groupBox = new QGroupBox("Transform");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 4);
    int row = 0;
    gridLayout->addWidget(windowLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_browserWindowComboBox->getWidget(),
                          row,
                          COLUMN_X);
    row++;
    
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                          row,
                          COLUMN_LABEL,
                          1,
                          COLUMN_COUNT);
    gridLayout->setRowMinimumHeight(row,
                                    10.0);
    row++;
    
    gridLayout->addWidget(panLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xPanDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yPanDoubleSpinBox,
                          row,
                          COLUMN_Y);
    row++;
    
    gridLayout->addWidget(rotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xRotateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yRotateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zRotateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(zoomLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_zoomDoubleSpinBox,
                          row,
                          COLUMN_X);
    row++;
    
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    
    return groupBox;
}

/**
 * Called when window number combo box value changed.
 */
void
CustomViewDialog::browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow)
{
    int32_t windowIndex = -1;
    if (browserWindow != NULL) {
        std::cout << "Selected " << browserWindow->getBrowserWindowIndex() << std::endl;
        windowIndex = browserWindow->getBrowserWindowIndex();
    }
    
    updateContent(windowIndex);
}

/**
 * Called when a transform value is changed.
 */
void
CustomViewDialog::transformValueChanged()
{
    double panX, panY, rotX, rotY, rotZ, zoom;
    getTransformationControlValues(panX,
                                   panY,
                                   rotX,
                                   rotY,
                                   rotZ,
                                   zoom);
    const float panning[3] = { panX, panY, 0.0 };
    
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        const int32_t tabIndex = btc->getTabNumber();
        if (btc != NULL) {
            Model* model = btc->getModelControllerForTransformation();
            if (model != NULL) {
                model->setTranslation(tabIndex, panning);
                Matrix4x4* rotationMatrix = model->getViewingRotationMatrix(tabIndex, Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->setRotation(rotX, rotY, rotZ);
                model->setScaling(tabIndex, zoom);
                
                updateGraphicsWindow();
            }
        }
    }
}

/**
 * Update the selected graphics window.
 */
void
CustomViewDialog::updateGraphicsWindow()
{
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        const int32_t windowIndex = bbw->getBrowserWindowIndex();
        
        m_blockDialogUpdate = true;
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(windowIndex).getPointer());
        m_blockDialogUpdate = false;
    }
}

/**
 * Update the dialog.
 */
void
CustomViewDialog::updateDialog()
{
    m_browserWindowComboBox->updateComboBox();
    updateContent(m_browserWindowComboBox->getSelectedBrowserWindowIndex());
}

/**
 * Update the dialog.
 */
void
CustomViewDialog::updateContent(const int32_t browserWindowIndexIn)
{
    /*
     * May get updates when graphics are redrawn by this dialog
     * and not doing this could result in infinite loop
     */
    if (m_blockDialogUpdate) {
        return;
    }
    
    /*
     * Update, set, and validate selected browser window
     */
    m_browserWindowComboBox->updateComboBox();
    m_browserWindowComboBox->setBrowserWindowByIndex(browserWindowIndexIn);
    const int32_t browserWindowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        const int32_t tabIndex = btc->getTabNumber();
        if (btc != NULL) {
            Model* model = btc->getModelControllerForTransformation();
            if (model != NULL) {
                const float* panning = model->getTranslation(tabIndex, Model::VIEWING_TRANSFORM_NORMAL);
                const Matrix4x4* rotationMatrix = model->getViewingRotationMatrix(tabIndex, Model::VIEWING_TRANSFORM_NORMAL);
                const float zooming = model->getScaling(tabIndex);
                
                double rotX, rotY, rotZ;
                rotationMatrix->getRotation(rotX, rotY, rotZ);
                
                setTransformationControlValues(panning[0],
                                               panning[1],
                                               rotX,
                                               rotY,
                                               rotZ,
                                               zooming);
            }
        }
        
        m_transformWidgetGroup->setEnabled(true);
    }
    else {
        m_transformWidgetGroup->setEnabled(false);
    }
}

/**
 * Get the transformation values.
 *
 * @param panX
 *    X pannning
 * @param panX
 *    X pannning
 * @param rotX
 *    X rotation
 * @param rotY
 *    Y rotation
 * @param rotZ
 *    Z rotation
 * @param zoom
 *    Zooming
 */
void
CustomViewDialog::getTransformationControlValues(double& panX,
                        double& panY,
                        double& rotX,
                        double& rotY,
                        double& rotZ,
                        double& zoom) const
{
    panX = m_xPanDoubleSpinBox->value();
    panY = m_yPanDoubleSpinBox->value();
    
    rotX = m_xRotateDoubleSpinBox->value();
    rotY = m_yRotateDoubleSpinBox->value();
    rotZ = m_zRotateDoubleSpinBox->value();
    
    zoom = m_zoomDoubleSpinBox->value();
}

/**
 * Set the transformation values.
 *
 * @param panX
 *    X pannning
 * @param panX
 *    X pannning
 * @param rotX
 *    X rotation
 * @param rotY
 *    Y rotation
 * @param rotZ
 *    Z rotation
 * @param zoom
 *    Zooming
 */
void
CustomViewDialog::setTransformationControlValues(const double panX,
                                    const double panY,
                                    const double rotX,
                                    const double rotY,
                                    const double rotZ,
                                    const double zoom) const
{
    m_transformWidgetGroup->blockAllSignals(true);
    
    m_xPanDoubleSpinBox->setValue(panX);
    m_yPanDoubleSpinBox->setValue(panY);
    
    m_xRotateDoubleSpinBox->setValue(rotX);
    m_yRotateDoubleSpinBox->setValue(rotY);
    m_zRotateDoubleSpinBox->setValue(rotZ);
    
    m_zoomDoubleSpinBox->setValue(zoom);
    
    m_transformWidgetGroup->blockAllSignals(false);
}

/**
 * Called when load custom view push button clicked.
 */
void
CustomViewDialog::loadCustomViewPushButtonClicked()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const std::vector<UserView*> userViews = prefs->getAllUserViews();

    QStringList userViewNames;
    for (std::vector<UserView*>::const_iterator iter = userViews.begin();
         iter != userViews.end();
         iter++) {
        const UserView* uv = *iter;
        userViewNames.append(uv->getName());
    }
    WuQDataEntryDialog ded("Load Custom View",
                           m_loadCustomViewPushButton);
    QListWidget* viewListWidget = ded.addListWidget("",
                                                    userViewNames);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const int viewIndex = viewListWidget->currentRow();
        if (viewIndex >= 0) {
            const UserView* uv = userViews[viewIndex];
            
            const int32_t browserWindowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
            
            BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
            if (bbw != NULL) {
                BrowserTabContent* btc = bbw->getBrowserTabContent();
                const int32_t tabIndex = btc->getTabNumber();
                if (btc != NULL) {
                    Model* model = btc->getModelControllerForTransformation();
                    if (model != NULL) {
                        model->setTransformationsFromUserView(tabIndex, *uv);
                        updateGraphicsWindow();
                    }
                }
            }
            
            updateDialog();
        }
    }
}

/**
 * Called when save custom view push button clicked.
 */
void
CustomViewDialog::saveCustomViewPushButtonClicked()
{
    bool ok = false;
    bool exitLoop = false;
    AString newViewName;
    while (exitLoop == false) {
        newViewName = QInputDialog::getText(m_saveCustomViewPushButton,
                                            "",
                                            "Name of New View",
                                            QLineEdit::Normal,
                                            newViewName,
                                            &ok);
        if (ok) {
            bool overwriteFlag = false;
            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            const std::vector<UserView*> userViews = prefs->getAllUserViews();
            const int32_t numViews = static_cast<int32_t>(userViews.size());
            if (numViews > 0) {
                for (int32_t i = 0; i < numViews; i++) {
                    const QString viewName = userViews[i]->getName();
                    if (viewName == newViewName) {
                        overwriteFlag = true;
                    }
                }
            }
            if (overwriteFlag) {
                const QString msg = ("View named \""
                                     + newViewName
                                     + "\" already exits.  Replace?");
                if (WuQMessageBox::warningYesNo(m_saveCustomViewPushButton,
                                                msg)) {
                    exitLoop = true;
                }
            }
            else {
                exitLoop = true;
            }
            
        }
        else {
            exitLoop = true;
        }
    }
    if (ok && (newViewName.isEmpty() == false)) {
        double panX, panY, rotX, rotY, rotZ, zoom;
        getTransformationControlValues(panX,
                                       panY,
                                       rotX,
                                       rotY,
                                       rotZ,
                                       zoom);
        const float panning[3] = { panX, panY, 0.0 };

        UserView uv;
        
        uv.setTranslation(panning);
        
        Matrix4x4 rotationMatrix;
        rotationMatrix.setRotation(rotX,
                                   rotY,
                                   rotZ);
        float rotationMatrixArray[4][4];
        rotationMatrix.getMatrix(rotationMatrixArray);
        uv.setRotation(rotationMatrixArray);
        
        uv.setScaling(zoom);
        
        uv.setName(newViewName);
        
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addUserView(uv);
    }
    
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
CustomViewDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* redrawnEvent =
        dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(redrawnEvent);
        
        redrawnEvent->setEventProcessed();

        const int32_t selectedBrowserWindowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
        if (redrawnEvent->getBrowserWindowIndex() == selectedBrowserWindowIndex) {
            updateContent(selectedBrowserWindowIndex);
        }
    }
}

