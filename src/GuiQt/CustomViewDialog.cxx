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

#define __CUSTOM_VIEW_DIALOG_DECLARE__
#include "CustomViewDialog.h"
#undef __CUSTOM_VIEW_DIALOG_DECLARE__

#include <cmath>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelTransform.h"
#include "SessionManager.h"
#include "WuQDataEntryDialog.h"
#include "WuQListWidget.h"
#include "WuQMessageBox.h"
#include "WuQTextEditorDialog.h"
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

    /*
     * Create the controls
     */
    QWidget* customViewWidget = createCustomViewWidget();
    QWidget* transformWidget = createTransformsWidget();
    m_copyWidget = createCopyWidget();
    
    /*
     * Layout for dialog
     */
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setSpacing(layout->spacing() / 2);
    layout->addWidget(transformWidget, 0, Qt::AlignVCenter);
    layout->addWidget(m_copyWidget, 0, Qt::AlignVCenter);
    layout->addWidget(customViewWidget, 0, Qt::AlignVCenter);
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
 
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
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
    /*
     * View selection list widget
     */
    m_customViewListWidget = new WuQListWidget();
    m_customViewListWidget->setFixedHeight(100);
    m_customViewListWidget->setFixedWidth(140);
    m_customViewListWidget->setSelectionMode(QListWidget::SingleSelection);
    
    QObject::connect(m_customViewListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                     this, SLOT(customViewSelected()));
    QObject::connect(m_customViewListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                     this, SLOT(customViewSelectedAndApplied()));

    m_newCustomViewPushButton = new QPushButton("New...");
    WuQtUtilities::setWordWrappedToolTip(m_newCustomViewPushButton,
                                         "Create a new Custom View by entering its name.  The view will use the current transformation values.");
    QObject::connect(m_newCustomViewPushButton, SIGNAL(clicked()),
                     this, SLOT(newCustomViewPushButtonClicked()));
    
    m_deleteCustomViewPushButton = new QPushButton("Delete...");
    WuQtUtilities::setWordWrappedToolTip(m_deleteCustomViewPushButton,
                                         "Delete the selected Custom View.");
    QObject::connect(m_deleteCustomViewPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteCustomViewPushButtonClicked()));
    
    WuQtUtilities::matchWidgetWidths(m_newCustomViewPushButton,
                                     m_deleteCustomViewPushButton);
    
    QGroupBox* groupBox = new QGroupBox("Custom Orientation");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
    layout->addWidget(m_customViewListWidget, 100, Qt::AlignHCenter);
    layout->addWidget(m_newCustomViewPushButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_deleteCustomViewPushButton, 0, Qt::AlignHCenter);
    layout->addStretch();
    
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);

    return groupBox;
}

/**
 * Called when a custom view is selected.
 */
void
CustomViewDialog::customViewSelected()
{
    
}

/**
 * Called when a custom view name is double-clicked.
 */
void
CustomViewDialog::customViewSelectedAndApplied()
{
    copyToTransformPushButtonClicked();
}

/**
 * @return Name of selected custom view..
 */
AString
CustomViewDialog::getSelectedCustomViewName()
{
    AString viewName;
    
    QListWidgetItem* lwi = m_customViewListWidget->currentItem();
    if (lwi != NULL) {
        viewName = lwi->text();
    }
    
    return viewName;
}


/**
 * Load content of custom view list widget.
 * 
 * @param selectedName
 *    If not empty, this name will be selected.
 */
void
CustomViewDialog::loadCustomViewListWidget(const AString& selectedName)
{
    /*
     * Cannot use getSelectedUserView() since the item returned may be invalid
     * if items have been removed.
     */
    QString previousViewName = getSelectedCustomViewName();
    if (selectedName.isEmpty() == false) {
        previousViewName = selectedName;
    }
    
    int defaultSelectedIndex = m_customViewListWidget->currentRow();
    
    m_customViewListWidget->clear();
    
    CaretPreferences* prefs = getCaretPreferences();
    prefs->readCustomViews();
    const std::vector<std::pair<AString,AString> > customViewNamesAndComments = prefs->getCustomViewNamesAndComments();
    const int32_t numViews = static_cast<int32_t>(customViewNamesAndComments.size());
    
    for (int32_t i = 0; i < numViews; i++) {
        const AString viewName = customViewNamesAndComments[i].first;
        const AString comment  = customViewNamesAndComments[i].second;
        
        if (viewName == previousViewName) {
            defaultSelectedIndex = i;
        }
        
        QListWidgetItem* lwi = new QListWidgetItem(viewName);
        if (comment.isEmpty() == false) {
            lwi->setToolTip(WuQtUtilities::createWordWrappedToolTipText(comment));
        }
        
        m_customViewListWidget->addItem(lwi);
    }
    
    if (defaultSelectedIndex >= numViews) {
        defaultSelectedIndex = numViews - 1;
    }
    if (defaultSelectedIndex < 0) {
        defaultSelectedIndex = 0;
    }
    
    if ((defaultSelectedIndex >= 0)
        && (defaultSelectedIndex < m_customViewListWidget->count())) {
        m_customViewListWidget->setCurrentRow(defaultSelectedIndex);
        customViewSelected();
        
        m_customViewListWidget->scrollToItem(m_customViewListWidget->currentItem());
    }
    
    const bool haveViews = (numViews > 0);
    m_copyWidget->setEnabled(haveViews);
    
    const bool haveSelectedView = (getSelectedCustomViewName().isEmpty() == false);
    m_deleteCustomViewPushButton->setEnabled(haveSelectedView);
}

/**
 * @return Create and return the copy buttons widget.
 */
QWidget*
CustomViewDialog::createCopyWidget()
{
    QPushButton* copyToCustomViewPushButton = new QPushButton("Copy -->");
    WuQtUtilities::setWordWrappedToolTip(copyToCustomViewPushButton,
                                         "Copy the Transform's values into the selected Custom Orientation.");
    QObject::connect(copyToCustomViewPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToCustomViewPushButtonClicked()));
    
    QPushButton* copyToTransformPushButton = new QPushButton("<-- Load");
    WuQtUtilities::setWordWrappedToolTip(copyToTransformPushButton,
                                         "Load the Custom Orientation's transform values into the Transform.");
    QObject::connect(copyToTransformPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToTransformPushButtonClicked()));
    
    m_viewTransformPushButton = new QPushButton("--> View...");
    m_viewTransformPushButton->setToolTip("View the transformation values");
    QObject::connect(m_viewTransformPushButton, &QPushButton::clicked,
                     this, &CustomViewDialog::viewTransformPushButtonClicked);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(copyToCustomViewPushButton);
    layout->addWidget(copyToTransformPushButton);
    layout->addSpacing(20);
    layout->addWidget(m_viewTransformPushButton);
    
    widget->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    
    return widget;
}

/**
 * Called when Copy To Custom View push buttton is clicked.
 */
void
CustomViewDialog::copyToCustomViewPushButtonClicked()
{
    CaretPreferences* prefs = getCaretPreferences();
    
    ModelTransform modelTransform;
    if (prefs->getCustomView(getSelectedCustomViewName(), modelTransform)) {
        moveTransformValuesToModelTransform(modelTransform);
        prefs->addOrReplaceCustomView(modelTransform);
    }
}

/**
 * Move the transform values to the given mode transform
 *
 * @param modelTransform
 *     Model transform nto which transform values are moved.
 */
void
CustomViewDialog::moveTransformValuesToModelTransform(ModelTransform& modelTransform)
{
    double panX, panY, panZ, rotX, rotY, rotZ, obRotX, obRotY, obRotZ, mprRotX, mprRotY, mprRotZ, flatRotate, zoom, rightFlatX, rightFlatY, rightFlatZoom;
    getTransformationControlValues(panX,
                                   panY,
                                   panZ,
                                   rotX,
                                   rotY,
                                   rotZ,
                                   obRotX,
                                   obRotY,
                                   obRotZ,
                                   mprRotX,
                                   mprRotY,
                                   mprRotZ,
                                   flatRotate,
                                   zoom,
                                   rightFlatX,
                                   rightFlatY,
                                   rightFlatZoom);
    
    Matrix4x4 rotationMatrix;
    rotationMatrix.setRotation(rotX,
                               rotY,
                               rotZ);
    float rotationMatrixArray[4][4];
    rotationMatrix.getMatrix(rotationMatrixArray);
    
    Matrix4x4 obliqueRotationMatrix;
    obliqueRotationMatrix.setRotation(obRotX, obRotY, obRotZ);
    float obliqueRotationMatrixArray[4][4];
    obliqueRotationMatrix.getMatrix(obliqueRotationMatrixArray);
    
    const float mprRotationAngles[3] { (float)mprRotX, (float)mprRotY, (float)mprRotZ };
    
    Matrix4x4 flatRotationMatrix;
    flatRotationMatrix.setRotation(0.0, 0.00, flatRotate);
    float flatRotationMatrixArray[4][4];
    flatRotationMatrix.getMatrix(flatRotationMatrixArray);
    modelTransform.setPanningRotationMatrixAndZoom(panX,
                                                   panY,
                                                   panZ,
                                                   rotationMatrixArray,
                                                   obliqueRotationMatrixArray,
                                                   mprRotationAngles,
                                                   flatRotationMatrixArray,
                                                   zoom,
                                                   rightFlatX,
                                                   rightFlatY,
                                                   rightFlatZoom);
}


/**
 * Called when Move To Transform push buttton is clicked.
 */
void
CustomViewDialog::copyToTransformPushButtonClicked()
{
    CaretPreferences* prefs = getCaretPreferences();
    
    const AString customViewName = getSelectedCustomViewName();
    
    ModelTransform modelTransform;
    if (prefs->getCustomView(customViewName, modelTransform)) {
        float panX, panY, panZ, rotationMatrixArray[4][4],
        mprRotationAngles[3],
              obliqueRotationMatrixArray[4][4], flatRotationMatrixArray[4][4], zoom,
        rightFlatX, rightFlatY, rightFlatZoom;
        modelTransform.getPanningRotationMatrixAndZoom(panX,
                                                       panY,
                                                       panZ,
                                                       rotationMatrixArray,
                                                       obliqueRotationMatrixArray,
                                                       mprRotationAngles,
                                                       flatRotationMatrixArray,
                                                       zoom,
                                                       rightFlatX,
                                                       rightFlatY,
                                                       rightFlatZoom);
        
        Matrix4x4 rotationMatrix;
        rotationMatrix.setMatrix(rotationMatrixArray);
        
        double rotX, rotY, rotZ;
        rotationMatrix.getRotation(rotX, rotY, rotZ);
        
        Matrix4x4 obliqueRotationMatrix;
        obliqueRotationMatrix.setMatrix(obliqueRotationMatrixArray);
        
        double obRotX, obRotY, obRotZ;
        obliqueRotationMatrix.getRotation(obRotX, obRotY, obRotZ);
        
        Matrix4x4 flatRotationMatrix;
        flatRotationMatrix.setMatrix(flatRotationMatrixArray);
        double flatRotX, flatRotY, flatRotZ;
        flatRotationMatrix.getRotation(flatRotX, flatRotY, flatRotZ);
        
        const double mprRotX = mprRotationAngles[0];
        const double mprRotY = mprRotationAngles[1];
        const double mprRotZ = mprRotationAngles[2];

        setTransformationControlValues(panX, panY, panZ,
                                       rotX, rotY, rotZ,
                                       obRotX, obRotY, obRotZ,
                                       mprRotX,
                                       mprRotY,
                                       mprRotZ,
                                       flatRotZ, zoom,
                                       rightFlatX, rightFlatY, rightFlatZoom);
        
        transformValueChanged();
    }
}

/**
 * Called when view transforms button clicked
 */
void
CustomViewDialog::viewTransformPushButtonClicked()
{
    ModelTransform mt;
    moveTransformValuesToModelTransform(mt);
    
    /*
     * Get the rotation matrix (for right surface)
     */
    float rotationMatrixComponents[4][4];
    mt.getRotation(rotationMatrixComponents);
    Matrix4x4 rotationMatrix;
    rotationMatrix.setMatrix(rotationMatrixComponents);

    /*
     * Flip-X matrix
     */
    Matrix4x4 flipMatrix;
    flipMatrix.scale(-1.0, 1.0, 1.0);
    
    /*
     * Left Matrix = Flip * RotMatrix * Flip
     */
    Matrix4x4 leftRotationMatrix(flipMatrix);
    leftRotationMatrix.postmultiply(rotationMatrix);
    leftRotationMatrix.postmultiply(flipMatrix);
    
    float leftRotationMatrixComponents[4][4];
    leftRotationMatrix.getMatrix(leftRotationMatrixComponents);
    
    Matrix4x4 matrixForCalculations;
    WuQTextEditorDialog::runNonModal("View Transforms",
                                     mt.getAsPrettyString(matrixForCalculations,
                                                          leftRotationMatrixComponents),
                                     WuQTextEditorDialog::TextMode::PLAIN,
                                     WuQTextEditorDialog::WrapMode::YES,
                                     m_viewTransformPushButton);
}

/**
 * @return The Transform widget.
 */
QWidget*
CustomViewDialog::createTransformsWidget()
{
    const int spinBoxWidth = 90;
    
    /*
     * Window number
     */
    QLabel* windowLabel = new QLabel("Workbench Window: ");
    m_browserWindowComboBox = new BrainBrowserWindowComboBox(BrainBrowserWindowComboBox::STYLE_NUMBER,
                                                             this);
    m_browserWindowComboBox->getWidget()->setFixedWidth(spinBoxWidth);
    QObject::connect(m_browserWindowComboBox, SIGNAL(browserWindowSelected(BrainBrowserWindow*)),
                     this, SLOT(browserWindowComboBoxValueChanged(BrainBrowserWindow*)));
    
    /*
     * Reset View button
     */
    QToolButton* resetViewToolButton = new QToolButton();
    resetViewToolButton->setText("Reset View");
    QObject::connect(resetViewToolButton, &QToolButton::clicked,
                     this, &CustomViewDialog::resetViewToolButtonClicked);
    
    /*
     * Panning
     */
    const double panMax(100000000.0);
    const double panStep = 1.0;
    QLabel* panLabel = new QLabel("Pan (X,Y):");
    m_xPanDoubleSpinBox = new QDoubleSpinBox;
    m_xPanDoubleSpinBox->setMinimum(-panMax);
    m_xPanDoubleSpinBox->setMaximum( panMax);
    m_xPanDoubleSpinBox->setSingleStep(panStep);
    m_xPanDoubleSpinBox->setDecimals(2);
    m_xPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yPanDoubleSpinBox = new QDoubleSpinBox;
    m_yPanDoubleSpinBox->setMinimum(-panMax);
    m_yPanDoubleSpinBox->setMaximum( panMax);
    m_yPanDoubleSpinBox->setSingleStep(panStep);
    m_yPanDoubleSpinBox->setDecimals(2);
    m_yPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zPanDoubleSpinBox = new QDoubleSpinBox;
    m_zPanDoubleSpinBox->setMinimum(-panMax);
    m_zPanDoubleSpinBox->setMaximum( panMax);
    m_zPanDoubleSpinBox->setSingleStep(panStep);
    m_zPanDoubleSpinBox->setDecimals(2);
    m_zPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zPanDoubleSpinBox, SIGNAL(valueChanged(double)),
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
     * Oblique Rotation
     */
    QLabel* obliqueRotateLabel = new QLabel("Oblique Rotate (X,Y,Z): ");
    m_xObliqueRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xObliqueRotateDoubleSpinBox->setWrapping(true);
    m_xObliqueRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_xObliqueRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_xObliqueRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_xObliqueRotateDoubleSpinBox->setDecimals(2);
    m_xObliqueRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xObliqueRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yObliqueRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yObliqueRotateDoubleSpinBox->setWrapping(true);
    m_yObliqueRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_yObliqueRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_yObliqueRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_yObliqueRotateDoubleSpinBox->setDecimals(2);
    m_yObliqueRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yObliqueRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zObliqueRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zObliqueRotateDoubleSpinBox->setWrapping(true);
    m_zObliqueRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_zObliqueRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_zObliqueRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_zObliqueRotateDoubleSpinBox->setDecimals(2);
    m_zObliqueRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zObliqueRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Oblique Rotation
     */
    QLabel* mprRotateLabel = new QLabel("MPR Rotate (X,Y,Z): ");
    m_xMprRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xMprRotateDoubleSpinBox->setWrapping(true);
    m_xMprRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_xMprRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_xMprRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_xMprRotateDoubleSpinBox->setDecimals(2);
    m_xMprRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xMprRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_yMprRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yMprRotateDoubleSpinBox->setWrapping(true);
    m_yMprRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_yMprRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_yMprRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_yMprRotateDoubleSpinBox->setDecimals(2);
    m_yMprRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yMprRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    m_zMprRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zMprRotateDoubleSpinBox->setWrapping(true);
    m_zMprRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_zMprRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_zMprRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_zMprRotateDoubleSpinBox->setDecimals(2);
    m_zMprRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zMprRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));

    /*
     * Flat rotation
     */
    QLabel* flatRotateLabel = new QLabel("Flat Rotation");
    m_flatRotationDoubleSpinBox = new QDoubleSpinBox;
    m_flatRotationDoubleSpinBox->setWrapping(true);
    m_flatRotationDoubleSpinBox->setMinimum(rotationMinimum);
    m_flatRotationDoubleSpinBox->setMaximum(rotationMaximum);
    m_flatRotationDoubleSpinBox->setSingleStep(rotateStep);
    m_flatRotationDoubleSpinBox->setDecimals(2);
    m_flatRotationDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_flatRotationDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    /*
     * Zoom
     */
    const double zoomStep = 0.01;
    QLabel* zoomLabel = new QLabel("Zoom: ");
    m_zoomDoubleSpinBox = new QDoubleSpinBox;
    m_zoomDoubleSpinBox->setMinimum(0.001);
    m_zoomDoubleSpinBox->setMaximum(10000.0);
    m_zoomDoubleSpinBox->setSingleStep(zoomStep);
    m_zoomDoubleSpinBox->setDecimals(3);
    m_zoomDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zoomDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &CustomViewDialog::zoomValueChanged);
    
    /*
     * Flat offset
     */
    QLabel* rightFlatOffsetLabel = new QLabel("Right Flat Offset: ");
    m_xRightFlatMapSpinBox = new QDoubleSpinBox;
    m_xRightFlatMapSpinBox->setMinimum(-100000.0);
    m_xRightFlatMapSpinBox->setMaximum( 100000.0);
    m_xRightFlatMapSpinBox->setSingleStep(panStep);
    m_xRightFlatMapSpinBox->setDecimals(2);
    m_xRightFlatMapSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xRightFlatMapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    m_yRightFlatMapSpinBox = new QDoubleSpinBox;
    m_yRightFlatMapSpinBox->setMinimum(-100000.0);
    m_yRightFlatMapSpinBox->setMaximum( 100000.0);
    m_yRightFlatMapSpinBox->setSingleStep(panStep);
    m_yRightFlatMapSpinBox->setDecimals(2);
    m_xRightFlatMapSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yRightFlatMapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    QLabel* rightFlatZoomLabel = new QLabel("Right Flat Zoom: ");
    m_rightFlatMapZoomFactorSpinBox = new QDoubleSpinBox;
    m_rightFlatMapZoomFactorSpinBox->setMinimum(0.001);
    m_rightFlatMapZoomFactorSpinBox->setMaximum(10000.0);
    m_rightFlatMapZoomFactorSpinBox->setSingleStep(zoomStep);
    m_rightFlatMapZoomFactorSpinBox->setDecimals(3);
    m_rightFlatMapZoomFactorSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_rightFlatMapZoomFactorSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(transformValueChanged()));
    
    m_transformWidgetGroup = new WuQWidgetObjectGroup(this);
    m_transformWidgetGroup->add(m_xPanDoubleSpinBox);
    m_transformWidgetGroup->add(m_yPanDoubleSpinBox);
    m_transformWidgetGroup->add(m_zPanDoubleSpinBox);
    m_transformWidgetGroup->add(m_xRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_yRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_zRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_xObliqueRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_yObliqueRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_zObliqueRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_xMprRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_yMprRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_zMprRotateDoubleSpinBox);
    m_transformWidgetGroup->add(m_flatRotationDoubleSpinBox);
    m_transformWidgetGroup->add(m_zoomDoubleSpinBox);
    m_transformWidgetGroup->add(m_xRightFlatMapSpinBox);
    m_transformWidgetGroup->add(m_yRightFlatMapSpinBox);
    m_transformWidgetGroup->add(m_rightFlatMapZoomFactorSpinBox);
    
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
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 4);
    int row = 0;
    gridLayout->addWidget(windowLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_browserWindowComboBox->getWidget(),
                          row,
                          COLUMN_X);
    gridLayout->addWidget(resetViewToolButton,
                          row,
                          COLUMN_Y,
                          1, 2, Qt::AlignHCenter);
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
    gridLayout->addWidget(m_zPanDoubleSpinBox,
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
    
    gridLayout->addWidget(obliqueRotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xObliqueRotateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yObliqueRotateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zObliqueRotateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(mprRotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xMprRotateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yMprRotateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zMprRotateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(flatRotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_flatRotationDoubleSpinBox,
                          row,
                          COLUMN_X);
    row++;
    
    gridLayout->addWidget(zoomLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_zoomDoubleSpinBox,
                          row,
                          COLUMN_X);
    row++;
    
    gridLayout->addWidget(rightFlatOffsetLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xRightFlatMapSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yRightFlatMapSpinBox,
                          row,
                          COLUMN_Y);
    row++;
    
    gridLayout->addWidget(rightFlatZoomLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_rightFlatMapZoomFactorSpinBox,
                          row,
                          COLUMN_X);
    
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
        windowIndex = browserWindow->getBrowserWindowIndex();
    }
    
    updateContent(windowIndex);
}

/**
 * Called when reset view tool button is clicked
 */
void
CustomViewDialog::resetViewToolButtonClicked()
{
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            btc->resetView();
            updateGraphicsWindow();
            if (btc->isMediaDisplayed()) {
                const bool repaintFlag(true);
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(repaintFlag).getPointer());
            }
        }
    }
}

/**
 * Called when zoom value is changed
 * @param value
 *    New zoom value.
 */
void
CustomViewDialog::zoomValueChanged(double value)
{
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            ModelHistology* histologyModel(btc->getDisplayedHistologyModel());
            if (histologyModel != NULL) {
                const BrainOpenGLViewportContent* vpContent(bbw->getViewportContentForSelectedTab());
                if (vpContent != NULL) {
                    btc->setHistologyScalingFromGui(const_cast<BrainOpenGLViewportContent*>(vpContent),
                                                   value);
                    updateGraphicsWindow();
                }
                else {
                    CaretLogSevere("Unable to find viewport content for tab index="
                                   + AString::number(btc->getTabNumber()));
                }
                
                /*
                 * Since histology is in tab we DO NOT want to call transformValueChanged() below.
                 */
                return;
            }
            ModelMedia* mediaModel = btc->getDisplayedMediaModel();
            if (mediaModel != NULL) {
                const BrainOpenGLViewportContent* vpContent(bbw->getViewportContentForSelectedTab());
                if (vpContent != NULL) {
                    btc->setMediaScalingFromGui(const_cast<BrainOpenGLViewportContent*>(vpContent),
                                                value);
                    updateGraphicsWindow();
                }
                else {
                    CaretLogSevere("Unable to find viewport content for tab index="
                                   + AString::number(btc->getTabNumber()));
                }
                
                /*
                 * Since media is in tab we DO NOT want to call transformValueChanged() below.
                 */
                return;
            }
        }
    }
    
    transformValueChanged();
}


/**
 * Called when a transform value is changed.
 */
void
CustomViewDialog::transformValueChanged()
{
    double panX, panY, panZ, rotX, rotY, rotZ, obRotX, obRotY, obRotZ, mprRotX, mprRotY, mprRotZ, flatRotate, zoom, rightFlatX, rightFlatY, rightFlatZoom;
    getTransformationControlValues(panX,
                                   panY,
                                   panZ,
                                   rotX,
                                   rotY,
                                   rotZ,
                                   obRotX,
                                   obRotY,
                                   obRotZ,
                                   mprRotX,
                                   mprRotY,
                                   mprRotZ,
                                   flatRotate,
                                   zoom,
                                   rightFlatX,
                                   rightFlatY,
                                   rightFlatZoom);
    
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            Model* model = btc->getModelForDisplay();
            if (model != NULL) {
                Matrix4x4 rotationMatrix;
                rotationMatrix.setRotation(rotX, rotY, rotZ);
                float rotationMatrixArray[4][4];
                rotationMatrix.getMatrix(rotationMatrixArray);
                
                Matrix4x4 obliqueRotationMatrix;
                obliqueRotationMatrix.setRotation(obRotX, obRotY, obRotZ);
                float obliqueRotationMatrixArray[4][4];
                obliqueRotationMatrix.getMatrix(obliqueRotationMatrixArray);
                
                float mprRotationAngles[3] { (float)mprRotX, (float)mprRotY, (float)mprRotZ };
                Matrix4x4 flatRotationMatrix;
                flatRotationMatrix.setRotation(0.0, 0.0, flatRotate);
                float flatRotationMatrixArray[4][4];
                flatRotationMatrix.getMatrix(flatRotationMatrixArray);
                
                ModelTransform modelTransform;
                modelTransform.setPanningRotationMatrixAndZoom(panX, panY, panZ,
                                                               rotationMatrixArray, obliqueRotationMatrixArray,
                                                               mprRotationAngles,
                                                               flatRotationMatrixArray, zoom,
                                                               rightFlatX, rightFlatY, rightFlatZoom);
                btc->setTransformationsFromModelTransform(modelTransform);
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
 * Gets called when the dialog gains focus.
 */
void
CustomViewDialog::focusGained()
{
    updateDialog();
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
        if (btc != NULL) {
            Model* model = btc->getModelForDisplay();
            if (model != NULL) {
                ModelTransform modelTransform;
                btc->getTransformationsInModelTransform(modelTransform);
                
                float panX, panY, panZ, rotationMatrixArray[4][4],
                mprRotationAngles[3],
                obliqueRotationMatrixArray[4][4], flatRotationMatrixArray[4][4], zoom,
                rightFlatX, rightFlatY, rightFlatZoom;
                modelTransform.getPanningRotationMatrixAndZoom(panX,
                                                               panY,
                                                               panZ,
                                                               rotationMatrixArray,
                                                               obliqueRotationMatrixArray,
                                                               mprRotationAngles,
                                                               flatRotationMatrixArray,
                                                               zoom,
                                                               rightFlatX,
                                                               rightFlatY,
                                                               rightFlatZoom);
                
                Matrix4x4 rotationMatrix;
                rotationMatrix.setMatrix(rotationMatrixArray);
                
                double rotX, rotY, rotZ;
                rotationMatrix.getRotation(rotX, rotY, rotZ);
                
                Matrix4x4 obliqueRotationMatrix;
                obliqueRotationMatrix.setMatrix(obliqueRotationMatrixArray);
                
                double obRotX, obRotY, obRotZ;
                obliqueRotationMatrix.getRotation(obRotX, obRotY, obRotZ);
                
                Matrix4x4 flatRotationMatrix;
                flatRotationMatrix.setMatrix(flatRotationMatrixArray);
                double flatRotX, flatRotY, flatRotZ;
                flatRotationMatrix.getRotation(flatRotX, flatRotY, flatRotZ);
                
                const double mprRotX = mprRotationAngles[0];
                const double mprRotY = mprRotationAngles[1];
                const double mprRotZ = mprRotationAngles[2];
                
                setTransformationControlValues(panX, panY, panZ,
                                               rotX, rotY, rotZ,
                                               obRotX, obRotY, obRotZ,
                                               mprRotX,
                                               mprRotY,
                                               mprRotZ,
                                               flatRotZ, zoom,
                                               rightFlatX, rightFlatY, rightFlatZoom);
//                const float* panning = btc->getTranslation();
//                const Matrix4x4 rotationMatrix = btc->getRotationMatrix();
//                const float zooming = btc->getScaling();
//                const Matrix4x4 obliqueRotationMatrix = btc->getObliqueVolumeRotationMatrix();
//
//                double rotX, rotY, rotZ;
//                rotationMatrix.getRotation(rotX, rotY, rotZ);
//
//                double obRotX, obRotY, obRotZ;
//                obliqueRotationMatrix.getRotation(obRotX, obRotY, obRotZ);
//
//                Matrix4x4 flatRotationMatrix = btc->getFlatRotationMatrix();
//                double flatRotX, flatRotY, flatRotZ;
//                flatRotationMatrix.getRotation(flatRotX, flatRotY, flatRotZ);
//
//                float rightFlatX, rightFlatY;
//                btc->getRightCortexFlatMapOffset(rightFlatX, rightFlatY);
//
//                const float rightFlatZoom = btc->getRightCortexFlatMapZoomFactor();
//
//                const float mprRotX(btc->getMprRotationX());
//                const float mprRotY(btc->getMprRotationY());
//                const float mprRotZ(btc->getMprRotationZ());

//                setTransformationControlValues(panning[0],
//                                               panning[1],
//                                               panning[2],
//                                               rotX,
//                                               rotY,
//                                               rotZ,
//                                               obRotX,
//                                               obRotY,
//                                               obRotZ,
//                                               mprRotX,
//                                               mprRotY,
//                                               mprRotZ,
//                                               flatRotZ,
//                                               zooming,
//                                               rightFlatX,
//                                               rightFlatY,
//                                               rightFlatZoom);
                
                const BrainOpenGLViewportContent* vpc(bbw->getViewportContentForSelectedTab());
                float stepValue(1.0);
                if (vpc != NULL) {
                    stepValue = vpc->getTranslationStepValueForCustomViewDialog();
                    updateSpinBoxSingleStepValue(m_xPanDoubleSpinBox, stepValue);
                    updateSpinBoxSingleStepValue(m_yPanDoubleSpinBox, stepValue);
                    updateSpinBoxSingleStepValue(m_zPanDoubleSpinBox, stepValue);
                }
            }
        }
        
        m_transformWidgetGroup->setEnabled(true);
    }
    else {
        m_transformWidgetGroup->setEnabled(false);
    }
    
    loadCustomViewListWidget();
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
 * @param objRotX
 *    X rotation
 * @param objRotY
 *    Y rotation
 * @param objRotZ
 *    Z rotation
 * @param flatRotation
 *    Flat rotation
 * @param zoom
 *    Zooming
 * @param rightFlatX
 *    Offset for right flat map.
 * @param rightFlat&
 *    Offset for right flat map.
 * @param rightFlatZoom
 *    Zoom for right flat map.
 */
void
CustomViewDialog::getTransformationControlValues(double& panX,
                                                 double& panY,
                                                 double& panZ,
                                                 double& rotX,
                                                 double& rotY,
                                                 double& rotZ,
                                                 double& obRotX,
                                                 double& obRotY,
                                                 double& obRotZ,
                                                 double& mprRotX,
                                                 double& mprRotY,
                                                 double& mprRotZ,
                                                 double& flatRotation,
                                                 double& zoom,
                                                 double& rightFlatX,
                                                 double& rightFlatY,
                                                 double& rightFlatZoom) const
{
    panX = m_xPanDoubleSpinBox->value();
    panY = m_yPanDoubleSpinBox->value();
    panZ = m_zPanDoubleSpinBox->value();
    
    rotX = m_xRotateDoubleSpinBox->value();
    rotY = m_yRotateDoubleSpinBox->value();
    rotZ = m_zRotateDoubleSpinBox->value();
    
    obRotX = m_xObliqueRotateDoubleSpinBox->value();
    obRotY = m_yObliqueRotateDoubleSpinBox->value();
    obRotZ = m_zObliqueRotateDoubleSpinBox->value();
    
    mprRotX = m_xMprRotateDoubleSpinBox->value();
    mprRotY = m_yMprRotateDoubleSpinBox->value();
    mprRotZ = m_zMprRotateDoubleSpinBox->value();

    flatRotation = m_flatRotationDoubleSpinBox->value();
    
    zoom = m_zoomDoubleSpinBox->value();
    
    rightFlatX = m_xRightFlatMapSpinBox->value();
    rightFlatY = m_yRightFlatMapSpinBox->value();
    rightFlatZoom = m_rightFlatMapZoomFactorSpinBox->value();
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
 * @param objRotX
 *    X rotation
 * @param objRotY
 *    Y rotation
 * @param objRotZ
 *    Z rotation
 * @param flatRotation
 *    Flat rotation
 * @param zoom
 *    Zooming
 * @param rightFlatX
 *    Offset for right flat map.
 * @param rightFlat
 *    Offset for right flat map.
 * @param rightFlatZoom
 *    Zoom factor for right flat map.
 */
void
CustomViewDialog::setTransformationControlValues(const double panX,
                                                 const double panY,
                                                 const double panZ,
                                                 const double rotX,
                                                 const double rotY,
                                                 const double rotZ,
                                                 const double obRotX,
                                                 const double obRotY,
                                                 const double obRotZ,
                                                 const double mprRotX,
                                                 const double mprRotY,
                                                 const double mprRotZ,
                                                 const double flatRotation,
                                                 const double zoom,
                                                 const double rightFlatX,
                                                 const double rightFlatY,
                                                 const double rightFlatZoom) const
{
    m_transformWidgetGroup->blockAllSignals(true);
    
    updateSpinBoxValue(m_xPanDoubleSpinBox, panX);
    updateSpinBoxValue(m_yPanDoubleSpinBox, panY);
    updateSpinBoxValue(m_zPanDoubleSpinBox, panZ);

    updateSpinBoxValue(m_xRotateDoubleSpinBox, rotX);
    updateSpinBoxValue(m_yRotateDoubleSpinBox, rotY);
    updateSpinBoxValue(m_zRotateDoubleSpinBox, rotZ);

    updateSpinBoxValue(m_xObliqueRotateDoubleSpinBox, obRotX);
    updateSpinBoxValue(m_yObliqueRotateDoubleSpinBox, obRotY);
    updateSpinBoxValue(m_zObliqueRotateDoubleSpinBox, obRotZ);

    updateSpinBoxValue(m_xMprRotateDoubleSpinBox, mprRotX);
    updateSpinBoxValue(m_yMprRotateDoubleSpinBox, mprRotY);
    updateSpinBoxValue(m_zMprRotateDoubleSpinBox, mprRotZ);

    updateSpinBoxValue(m_flatRotationDoubleSpinBox, flatRotation);
    
    updateSpinBoxValue(m_zoomDoubleSpinBox, zoom);
    
    updateSpinBoxValue(m_xRightFlatMapSpinBox, rightFlatX);
    updateSpinBoxValue(m_yRightFlatMapSpinBox, rightFlatY);

    updateSpinBoxValue(m_rightFlatMapZoomFactorSpinBox, rightFlatZoom);
    
    m_transformWidgetGroup->blockAllSignals(false);
}

/**
 * Scale the double value to a 'scaled integer' with the given number of decimals.
 * Example createScaledInt(123.457, 2) returns 12346.
 * @param value
 *    The floating point value
 * @param decimals
 *    Number of decimals for scaling
 * @return
 *    The value scaled to an integer
 */
int64_t
CustomViewDialog::createScaledInt(const double value,
                                  const int32_t decimals) const
{
    CaretAssert(decimals >= 0);
    
    static std::vector<double> scaleVector;
    static bool firstTime(true);
    if (firstTime) {
        firstTime = false;
        scaleVector.push_back(1.0);
        scaleVector.push_back(10.0);
        scaleVector.push_back(100.0);
        scaleVector.push_back(1000.0);
        scaleVector.push_back(10000.0);
        scaleVector.push_back(100000.0);
        scaleVector.push_back(1000000.0);
        scaleVector.push_back(10000000.0);
        scaleVector.push_back(100000000.0);
    }
    
    double scaleToInt(1.0);
    if (decimals >= 0) {
        if (decimals >= static_cast<int32_t>(scaleVector.size())) {
            scaleToInt = std::pow(10.0, decimals);
        }
        else {
            CaretAssertVectorIndex(scaleVector, decimals);
            scaleToInt = scaleVector[decimals];
        }
    }
    const int64_t intValue(std::round(value * scaleToInt));
    return intValue;
}
/**
 * Update the spin box value if its value has changed.
 * @param spinBox
 *    The spin box
 * @param value
 *    New value.
 */
void
CustomViewDialog::updateSpinBoxValue(QDoubleSpinBox* spinBox,
                                     const double newValue) const
{
    /*
     * Prevent updates since the dialog can get updated
     * as the user is typing in values and messes up
     * user's typing.
     */
    CaretAssert(spinBox);
    const double value(spinBox->value());
    const int32_t decimals(spinBox->decimals());
    
    /*
     * Compare floating point numbers for given number of decimal places
     */
    if (createScaledInt(value, decimals)
        != createScaledInt(newValue, decimals)) {
        QSignalBlocker blocker(spinBox);
        spinBox->setValue(newValue);
    }
}

/**
 * Updfate the step value if it has changed to prevent signals and when user editing.
 * @param spinBox
 *    The spin box
 * @param singleStep
 *   New single step value.
 */
void
CustomViewDialog::updateSpinBoxSingleStepValue(QDoubleSpinBox* spinBox,
                                               const double singleStep)
{
    CaretAssert(spinBox);
    if (spinBox->singleStep() != singleStep) {
        QSignalBlocker blocker(spinBox);
        spinBox->setSingleStep(singleStep);
    }
}

/**
 * @return The caret preferences.
 */
CaretPreferences*
CustomViewDialog::getCaretPreferences()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    return prefs;
}

/**
 * Called when new custom view push button clicked.
 */
void
CustomViewDialog::newCustomViewPushButtonClicked()
{
    CaretPreferences* prefs = getCaretPreferences();
    const std::vector<AString> existingCustomViewNames = prefs->getCustomViewNames();
    
    bool createViewFlag = false;
    AString newViewName;
    AString newViewComment;
    
    bool exitLoop = false;
    while (exitLoop == false) {
        WuQDataEntryDialog ded("New Custom View",
                               m_newCustomViewPushButton);
        
        QLineEdit* nameLineEdit = ded.addLineEditWidget("View Name");
        QTextEdit* commentTextEdit = ded.addTextEdit("Comment", "", false);
        
        nameLineEdit->setFocus();
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            newViewName = nameLineEdit->text().trimmed();

            
            
            if (newViewName.isEmpty() == false) {
                newViewComment = commentTextEdit->toPlainText().trimmed();
                
                /*
                 * If custom view exists with name entered by user,
                 * then warn the user.
                 */
                if (std::find(existingCustomViewNames.begin(),
                              existingCustomViewNames.end(),
                              newViewName) != existingCustomViewNames.end()) {
                    const QString msg = ("View named \""
                                         + newViewName
                                         + "\" already exits.  Replace?");
                    if (WuQMessageBox::warningYesNo(m_newCustomViewPushButton,
                                                    msg)) {
                        exitLoop = true;
                        createViewFlag = true;
                    }
                }
                else {
                    exitLoop = true;
                    createViewFlag = true;
                }
            }
            
        }
        else {
            exitLoop = true;
        }
    }
    if (createViewFlag && (newViewName.isEmpty() == false)) {
        ModelTransform  mt;
        mt.setName(newViewName);
        mt.setComment(newViewComment);
        moveTransformValuesToModelTransform(mt);
        prefs->addOrReplaceCustomView(mt);
        
        loadCustomViewListWidget(newViewName);
    }
}

/**
 * Called when save custom view push button clicked.
 */
void
CustomViewDialog::deleteCustomViewPushButtonClicked()
{
    const AString viewName = getSelectedCustomViewName();
    if (viewName.isEmpty() == false) {
        const QString msg = ("Delete view named \""
                             + viewName
                             + "\" ?");
        if (WuQMessageBox::warningYesNo(m_newCustomViewPushButton,
                                        msg)) {
            CaretPreferences* prefs = getCaretPreferences();
            prefs->removeCustomView(viewName);
            loadCustomViewListWidget();
        }
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

