
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

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretPreferences.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "SessionManager.h"
#include "UserView.h"
#include "WuQListWidget.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

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
: WuQDialogNonModal("Custom View",
                    parent)
{
    /*
     * Remove apply button by using an empty name
     */
    setApplyButtonText("");
    
    /*
     * View controls
     */
    QLabel* viewLabel = new QLabel("Views");
    
    /*
     * Add new view button
     */
    m_addNewViewPushButton = new QPushButton("Add...");
    QObject::connect(m_addNewViewPushButton, SIGNAL(clicked()),
                     this, SLOT(addNewViewPushButtonClicked()));
    
    /*
     * Delete view button
     */
    m_deleteViewPushButton = new QPushButton("Delete...");
    QObject::connect(m_deleteViewPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteViewPushButtonClicked()));
    
    /*
     * Replace view button
     */
    m_replaceViewPushButton = new QPushButton("Replace...");
    QObject::connect(m_replaceViewPushButton, SIGNAL(clicked()),
                     this, SLOT(replaceViewPushButtonClicked()));
    
    /*
     * Apply view button
     */
    m_applyViewPushButton = new QPushButton("Apply");
    QObject::connect(m_applyViewPushButton, SIGNAL(clicked()),
                     this, SLOT(applyViewPushButtonClicked()));
    
    /*
     * Layout for view buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(m_applyViewPushButton);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_addNewViewPushButton);
    sceneButtonLayout->addWidget(m_replaceViewPushButton);
    sceneButtonLayout->addWidget(m_deleteViewPushButton);
    
    /*
     * View selection list widget
     */
    m_viewSelectionListWidget = new WuQListWidget();
    
    QObject::connect(m_viewSelectionListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(viewSelected()));
    QObject::connect(m_viewSelectionListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                     this, SLOT(applyViewPushButtonClicked()));  // show the scene
    QObject::connect(m_viewSelectionListWidget, SIGNAL(itemWasDropped()),
                     this, SLOT(viewWasDropped()));
    
    /*------------------------------------------------------------------------*/
    /*
     * Transformations
     */
    
    const int spinBoxWidth = 100;
    
    /*
     * Column names
     */
    QLabel* xLabel = new QLabel("X");
    QLabel* yLabel = new QLabel("Y");
    QLabel* zLabel = new QLabel("Z");
    
    /*
     * Translation
     */
    QLabel* translateLabel = new QLabel("Translation:");
    m_xTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_xTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_xTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_xTranslateDoubleSpinBox->setSingleStep(5.0);
    m_xTranslateDoubleSpinBox->setDecimals(1);
    m_xTranslateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_yTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_yTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_yTranslateDoubleSpinBox->setSingleStep(5.0);
    m_yTranslateDoubleSpinBox->setDecimals(1);
    m_yTranslateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_zTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_zTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_zTranslateDoubleSpinBox->setSingleStep(5.0);
    m_zTranslateDoubleSpinBox->setDecimals(1);
    m_zTranslateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Rotation
     */
    QLabel* rotateLabel = new QLabel("Rotation:");
    m_xRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xRotateDoubleSpinBox->setMinimum(-180.0);
    m_xRotateDoubleSpinBox->setMaximum(180.0);
    m_xRotateDoubleSpinBox->setSingleStep(5.0);
    m_xRotateDoubleSpinBox->setDecimals(2);
    m_xRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yRotateDoubleSpinBox->setMinimum(-180.0);
    m_yRotateDoubleSpinBox->setMaximum(180.0);
    m_yRotateDoubleSpinBox->setSingleStep(5.0);
    m_yRotateDoubleSpinBox->setDecimals(2);
    m_yRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zRotateDoubleSpinBox->setMinimum(-180.0);
    m_zRotateDoubleSpinBox->setMaximum(180.0);
    m_zRotateDoubleSpinBox->setSingleStep(5.0);
    m_zRotateDoubleSpinBox->setDecimals(2);
    m_zRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Scale
     */
    QLabel* scaleLabel = new QLabel("Scaling:");
    m_scaleDoubleSpinBox = new QDoubleSpinBox;
    m_scaleDoubleSpinBox->setMinimum(0.001);
    m_scaleDoubleSpinBox->setMaximum(10000.0);
    m_scaleDoubleSpinBox->setSingleStep(0.1);
    m_scaleDoubleSpinBox->setDecimals(3);
    m_scaleDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_scaleDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Columns for grid layout
     */
    int column = 0;
    const int COLUMN_LABEL  = column++;
    const int COLUMN_X      = column++;
    const int COLUMN_Y      = column++;
    const int COLUMN_Z      = column++;
    const int COLUMN_BUTTON = column++;
    const int COLUMN_COUNT  = column++;
    
    /*------------------------------------------------------------------------*/
    /*
     * Layout widgets
     */
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 4);
    int row = 0;
    gridLayout->addWidget(viewLabel, row, COLUMN_LABEL, Qt::AlignTop);
    gridLayout->addWidget(m_viewSelectionListWidget, row, 1, 1, 3);
    gridLayout->addLayout(sceneButtonLayout, row, COLUMN_BUTTON);
    row++;
    QWidget* lineWidget = WuQtUtilities::createHorizontalLineWidget();
    lineWidget->setFixedHeight(12);
    gridLayout->addWidget(lineWidget, row, 0, 1, COLUMN_COUNT);
    row++;
    gridLayout->addWidget(xLabel,
                          row,
                          COLUMN_X,
                          Qt::AlignCenter);
    gridLayout->addWidget(yLabel,
                          row,
                          COLUMN_Y,
                          Qt::AlignCenter);
    gridLayout->addWidget(zLabel,
                          row,
                          COLUMN_Z,
                          Qt::AlignCenter);
    row++;
    
    gridLayout->addWidget(translateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xTranslateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yTranslateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zTranslateDoubleSpinBox,
                          row,
                          COLUMN_Z);
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
    
    gridLayout->addWidget(scaleLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_scaleDoubleSpinBox,
                          row,
                          COLUMN_X);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(gridWidget);
    setCentralWidget(widget);
    
    setAutoDefaultButtonProcessing(false);
}

/**
 * Destructor.
 */
CustomViewDialog::~CustomViewDialog()
{
    
}

/**
 * Called when a transform value is changed.
 */
void
CustomViewDialog::transformValueChanged()
{
    const float t[3] = {
        m_xTranslateDoubleSpinBox->value(),
        m_yTranslateDoubleSpinBox->value(),
        m_zTranslateDoubleSpinBox->value()
    };

    const float r[3] = {
        m_xRotateDoubleSpinBox->value(),
        m_yRotateDoubleSpinBox->value(),
        m_zRotateDoubleSpinBox->value()
    };
    Matrix4x4 rotationMatrix;
    rotationMatrix.setRotation(r[0], r[1], r[2]);
    float rotation[4][4];
    rotationMatrix.getMatrix(rotation);
    
    const float s = m_scaleDoubleSpinBox->value();
    
    UserView* userView = getSelectedUserView();
    if (userView != NULL) {
        userView->setTranslation(t);
        userView->setRotation(rotation);
        userView->setScaling(s);
    }
}

/**
 * Update the dialog.
 */
void
CustomViewDialog::updateDialog()
{
    UserView* previouslySelectedUserView = getSelectedUserView();
    
    m_viewSelectionListWidget->clear();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->readUserViews();
    std::vector<UserView*> allUserViews = prefs->getAllUserViews();
    
    int defaultSelectedIndex = 0;
    const int32_t numViews = static_cast<int32_t>(allUserViews.size());
    for (int32_t i = 0; i < numViews; i++) {
        UserView* userView = allUserViews[i];
        
        if (userView == previouslySelectedUserView) {
            defaultSelectedIndex = i;
        }
        
        const QString viewName = userView->getName();
        
        QListWidgetItem* lwi = new QListWidgetItem(viewName);
        lwi->setData(Qt::UserRole,
                     qVariantFromValue(reinterpret_cast<quintptr>(userView)));
        m_viewSelectionListWidget->addItem(lwi);
    }
    
    if (defaultSelectedIndex < m_viewSelectionListWidget->count()) {
        m_viewSelectionListWidget->setCurrentRow(defaultSelectedIndex);
        viewSelected();
    }
}

void
CustomViewDialog::setBrowserWindowIndex(const int32_t browserWindowIndex)
{
    
}

/**
 * @return The selected user view.  Returns NULL if no views.
 */
UserView*
CustomViewDialog::getSelectedUserView()
{
    UserView* userView = NULL;
    QListWidgetItem* lwi = m_viewSelectionListWidget->currentItem();
    if (lwi != NULL) {
        userView = reinterpret_cast<UserView*>(qVariantValue<quintptr>(lwi->data(Qt::UserRole)));
    }
    return userView;
}

/**
 * Called when add new view push button clicked.
 */
void
CustomViewDialog::addNewViewPushButtonClicked()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    bool ok = false;
    bool exitLoop = false;
    AString newViewName;
    while (exitLoop == false) {
        newViewName = QInputDialog::getText(m_addNewViewPushButton,
                                            "",
                                            "Name of New View",
                                            QLineEdit::Normal,
                                            newViewName,
                                            &ok);
        if (ok) {
            bool overwriteFlag = false;
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
                if (WuQMessageBox::warningYesNo(m_addNewViewPushButton,
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
        const int32_t windowIndex = 0;  // NEED TO DETERMINE WINDOWS
        
        BrowserTabContent* btc = GuiManager::get()->getBrowserTabContentForBrowserWindow(windowIndex,
                                                                                         true);
        if (btc != NULL) {
            Model* model = btc->getModelControllerForTransformation();
            if (model != NULL) {
                const int32_t tabIndex = btc->getTabNumber();

                UserView uv;
                model->getTransformationsInUserView(tabIndex,
                                                  uv);
                uv.setName(newViewName);
                prefs->addUserView(uv);
                
                updateDialog();
            }
        }        
    }
    
}

/**
 * Called when apply view push button clicked.
 */
void
CustomViewDialog::applyViewPushButtonClicked()
{
    const int32_t windowIndex = 0;  // NEED TO DETERMINE WINDOWS
    
    UserView* userView = getSelectedUserView();
    if (userView != NULL) {
        BrowserTabContent* btc = GuiManager::get()->getBrowserTabContentForBrowserWindow(windowIndex,
                                                                                         true);
        if (btc != NULL) {
            Model* model = btc->getModelControllerForTransformation();
            if (model != NULL) {
                const int32_t tabIndex = btc->getTabNumber();
                model->setTransformationsFromUserView(tabIndex, *userView);
                EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(windowIndex).getPointer());
            }
        }
    }
}

/**
 * Called when delete view push button clicked.
 */
void
CustomViewDialog::deleteViewPushButtonClicked()
{
//    const std::vector<UserView*> userViews = prefs->getAllUserViews();
//    const int32_t numViews = static_cast<int32_t>(userViews.size());
//    if (numViews > 0) {
//        WuQDataEntryDialog dialog("Edit Views",
//                                  this->orientationCustomViewSelectToolButtonMenu,
//                                  (numViews > 10));
//        dialog.setTextAtTop("Unchecked views will be deleted", false);
//        std::vector<QCheckBox*> viewNameCheckBoxes;
//        for (int32_t i = 0; i < numViews; i++) {
//            const QString viewName = userViews[i]->getName();
//            viewNameCheckBoxes.push_back(dialog.addCheckBox(viewName,
//                                                            true));
//        }
//        
//        if (dialog.exec() == QDialog::Accepted) {
//            for (int32_t i = 0; i < numViews; i++) {
//                QCheckBox* cb = viewNameCheckBoxes[i];
//                if (cb->isChecked() == false) {
//                    prefs->removeUserView(cb->text());
//                }
//            }
//        }
//    }
}

/**
 * Called when replace view push button clicked.
 */
void
CustomViewDialog::replaceViewPushButtonClicked()
{
    
}

/**
 * Called when a view is selected.
 */
void
CustomViewDialog::viewSelected()
{
    UserView* userView = getSelectedUserView();
    if (userView != NULL) {
        float rotation[4][4];
        userView->getRotation(rotation);
        Matrix4x4 rotationMatrix;
        rotationMatrix.setMatrix(rotation);
        
        double rotationX, rotationY, rotationZ;
        rotationMatrix.getRotation(rotationX, rotationY, rotationZ);
        
        float translation[3];
        userView->getTranslation(translation);
        
        const float scaling = userView->getScaling();
        
        m_xTranslateDoubleSpinBox->setValue(translation[0]);
        m_yTranslateDoubleSpinBox->setValue(translation[1]);
        m_zTranslateDoubleSpinBox->setValue(translation[2]);
        
        m_xRotateDoubleSpinBox->setValue(rotationX);
        m_yRotateDoubleSpinBox->setValue(rotationY);
        m_zRotateDoubleSpinBox->setValue(rotationZ);
        
        m_scaleDoubleSpinBox->setValue(scaling);
    }
}

/**
 * Called when user drags and drops a view in the list box.
 */
void
CustomViewDialog::viewWasDropped()
{
    std::vector<UserView*> newlyOrderedUserViews;
    
    /*
     * Get the scenes from this list widget to obtain the new scene ordering.
     */
    const int32_t numItems = m_viewSelectionListWidget->count();
    for (int32_t i = 0; i < numItems; i++) {
        QListWidgetItem* lwi = m_viewSelectionListWidget->item(i);
        if (lwi != NULL) {
            if (lwi != NULL) {
                UserView* view = reinterpret_cast<UserView*>(qVariantValue<quintptr>(lwi->data(Qt::UserRole)));
                newlyOrderedUserViews.push_back(view);
            }
        }
    }

    if (newlyOrderedUserViews.empty() == false) {
        /*
         * Update the order of the views in the preferences.
         */
    }
    
    viewSelected();
}

