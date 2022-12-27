
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__
#include "BrainBrowserWindowToolBarHistology.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_HISTOLOGY_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowToolBarSliceSelection.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "CursorDisplayScoped.h"
#include "CziNonLinearTransform.h"
#include "DisplayPropertiesCziImages.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventBrowserTabValidate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GuiManager.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "HistologyOverlay.h"
#include "HistologyOverlaySet.h"
#include "ModelHistology.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQSpinBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarHistology
 * \brief Histology Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarHistology::BrainBrowserWindowToolBarHistology(BrainBrowserWindowToolBar* parentToolBar,
                                                                       const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    const AString objectNamePrefix(parentObjectName
                                   + ":BrainBrowserWindowToolBarHistology");
    WuQMacroManager* macroManager(WuQMacroManager::instance());
    
    /*
     * Slice controls
     */
    const int32_t sliceIndexNumberWidth(60);
    
    QLabel* sliceLabel(new QLabel("Slice"));
    QLabel* sliceIndexLabel(new QLabel("Index"));
    m_sliceIndexSpinBox = new WuQSpinBox();
    m_sliceIndexSpinBox->setSingleStep(1);
    m_sliceIndexSpinBox->setFixedWidth(sliceIndexNumberWidth);
    QObject::connect(m_sliceIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarHistology::sliceIndexValueChanged);
    
    QLabel* sliceNumberLabel(new QLabel("Number"));
    m_sliceNumberSpinBox = new WuQSpinBox();
    m_sliceNumberSpinBox->setSingleStep(1);
    m_sliceNumberSpinBox->setFixedWidth(sliceIndexNumberWidth);
    QObject::connect(m_sliceNumberSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarHistology::sliceNumberValueChanged);

    /*
     * Plane and stereotaxic coordinates
     */
    const int numberSpinBoxWidth(80);
    QLabel* planeLabel(new QLabel("Plane"));
    QLabel* stereotaxicLabel(new QLabel("XYZ"));
    for (int32_t i = 0; i < 3; i++) {
        m_planeXyzSpinBox[i] = new WuQDoubleSpinBox(this);
        m_planeXyzSpinBox[i]->setFixedWidth(numberSpinBoxWidth);
        m_planeXyzSpinBox[i]->setDecimalsModeAuto();
        m_planeXyzSpinBox[i]->setSingleStepPercentage(1.0);
        QObject::connect(m_planeXyzSpinBox[i], &WuQDoubleSpinBox::valueChanged,
                         this, &BrainBrowserWindowToolBarHistology::planeXyzSpinBoxValueChanged);

        m_stereotaxicXyzSpinBox[i] = new WuQDoubleSpinBox(this);
        m_stereotaxicXyzSpinBox[i]->setFixedWidth(numberSpinBoxWidth);
        m_stereotaxicXyzSpinBox[i]->setDecimalsModeAuto();
        m_stereotaxicXyzSpinBox[i]->setSingleStepPercentage(1.0);
        QObject::connect(m_stereotaxicXyzSpinBox[i], &WuQDoubleSpinBox::valueChanged,
                         this, &BrainBrowserWindowToolBarHistology::stereotaxicXyzSpinBoxValueChanged);
    }
    
    /*
     * Identification moves slices button
     */
    const AString idToolTipText = ("When selected: If there is an identification operation "
                                   "in ths tab or any other tab with the same yoking status "
                                   "(not Off), the volume slices will move to the location "
                                   "of the identified brainordinate.");
    m_identificationMovesSlicesAction = new QAction(this);
    m_identificationMovesSlicesAction->setCheckable(true);
    m_identificationMovesSlicesAction->setText("");
    WuQtUtilities::setWordWrappedToolTip(m_identificationMovesSlicesAction,
                                         idToolTipText);
    QAction::connect(m_identificationMovesSlicesAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarHistology::identificationMovesSlicesActionTriggered);
    QIcon volumeCrossHairIcon;
    const bool volumeCrossHairIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/volume-crosshair-pointer.png",
                            volumeCrossHairIcon);
    QToolButton* identificationMovesSlicesToolButton = new QToolButton;
    if (volumeCrossHairIconValid) {
        m_identificationMovesSlicesAction->setIcon(volumeCrossHairIcon);
        m_identificationMovesSlicesAction->setIcon(BrainBrowserWindowToolBarSliceSelection::createVolumeIdentificationUpdatesSlicesIcon(identificationMovesSlicesToolButton));
    }
    else {
        m_identificationMovesSlicesAction->setText("ID");
    }
    identificationMovesSlicesToolButton->setDefaultAction(m_identificationMovesSlicesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(identificationMovesSlicesToolButton);
    m_identificationMovesSlicesAction->setObjectName(objectNamePrefix
                                                             + "MoveSliceToID");
    macroManager->addMacroSupportToObject(m_identificationMovesSlicesAction,
                                          "Enable move volume slice to ID location");
    
    /*
     * Move to center action button
     */
    m_moveToCenterAction = new QAction(this);
    m_moveToCenterAction->setText("Center");
    m_moveToCenterAction->setToolTip("Move to center of slices");
    QObject::connect(m_moveToCenterAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarHistology::moveToCenterActionTriggered);
    m_moveToCenterAction->setObjectName(objectNamePrefix
                                                     + "MoveSliceToCenter");
    macroManager->addMacroSupportToObject(m_moveToCenterAction,
                                          "Moves to center of slices");
    QToolButton* moveToCenterToolButton = new QToolButton();
    moveToCenterToolButton->setDefaultAction(m_moveToCenterAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(moveToCenterToolButton);
    
    m_nonLinearTransformEnabledCheckBox = new QCheckBox("Non-Linear");
    m_nonLinearTransformEnabledCheckBox->setToolTip("<html>"
                                                    "Non-linear transformations enabled.  Affects all tabs "
                                                    "and is not saved to scenes"
                                                    "</html>");
    QObject::connect(m_nonLinearTransformEnabledCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarHistology::nonLinearTransformEnabledCheckBoxClicked);
    
    m_overlapCheckBox = new QCheckBox("Overlap");
    m_overlapCheckBox->setEnabled(false);
    QObject::connect(m_overlapCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarHistology::overlapCheckBoxClicked);
    
    /*
     * Layout widgets
     */
    int columnIndex(0);
    const int columnSliceLabels(columnIndex++);
    const int columnSliceSpinBoxes(columnIndex++);
    const int columnPlaneSpinBoxes(columnIndex++);
    const int columnStereotaxicSpinBoxes(columnIndex++);
    
    QGridLayout* controlsLayout(new QGridLayout());
    int row(0);
    controlsLayout->addWidget(sliceLabel,
                              row, columnSliceLabels, 1, 2, Qt::AlignHCenter);
    controlsLayout->addWidget(planeLabel,
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(stereotaxicLabel,
                              row, columnStereotaxicSpinBoxes);
    ++row;
    controlsLayout->addWidget(sliceIndexLabel,
                              row, columnSliceLabels);
    controlsLayout->addWidget(m_sliceIndexSpinBox,
                              row, columnSliceSpinBoxes);
    controlsLayout->addWidget(m_planeXyzSpinBox[0]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[0]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    ++row;
    controlsLayout->addWidget(sliceNumberLabel,
                              row, columnSliceLabels);
    controlsLayout->addWidget(m_sliceNumberSpinBox,
                              row, columnSliceSpinBoxes);
    controlsLayout->addWidget(m_planeXyzSpinBox[1]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[1]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    ++row;
    controlsLayout->addWidget(m_nonLinearTransformEnabledCheckBox,
                              row, columnSliceLabels, 1, 2, Qt::AlignLeft);
    controlsLayout->addWidget(m_planeXyzSpinBox[2]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[2]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    ++row;
    controlsLayout->addWidget(identificationMovesSlicesToolButton,
                              row, columnSliceLabels, Qt::AlignLeft);
    controlsLayout->addWidget(m_overlapCheckBox,
                              row, columnSliceSpinBoxes, 1, 2, Qt::AlignHCenter);
    controlsLayout->addWidget(moveToCenterToolButton,
                              row, columnStereotaxicSpinBoxes, Qt::AlignHCenter);
    ++row;

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addLayout(controlsLayout);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);

}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarHistology::~BrainBrowserWindowToolBarHistology()
{
}


/**
 * @return The selected histology file (NULL if none selected)
 */
HistologySlicesFile*
BrainBrowserWindowToolBarHistology::getHistologySlicesFile(BrowserTabContent* browserTabContent)
{
    HistologySlicesFile* histologySlicesFile(NULL);
    
    if (browserTabContent != NULL) {
        ModelHistology* histologyModel = browserTabContent->getDisplayedHistologyModel();
        if (histologyModel != NULL) {
            HistologyOverlaySet* histologyOverlaySet = browserTabContent->getHistologyOverlaySet();
            HistologyOverlay* underlay = histologyOverlaySet->getBottomMostEnabledOverlay();
            if (underlay != NULL) {
                const HistologyOverlay::SelectionData selectionData(underlay->getSelectionData());

                if (selectionData.m_selectedFile != NULL) {
                    histologySlicesFile = selectionData.m_selectedFile->castToHistologySlicesFile();
                }
            }
        }
    }
    
    return histologySlicesFile;
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarHistology::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(browserTabContent);
    if (histologySlicesFile != NULL) {
        const HistologyCoordinate histologyCoordinate(m_browserTabContent->getHistologySelectedCoordinate(histologySlicesFile));
        QSignalBlocker indexBlocker(m_sliceIndexSpinBox);
        m_sliceIndexSpinBox->setRange(0, histologySlicesFile->getNumberOfHistologySlices() - 1);
        m_sliceIndexSpinBox->setValue(histologyCoordinate.getSliceIndex());

        QSignalBlocker numberBlocker(m_sliceNumberSpinBox);
        m_sliceNumberSpinBox->setRange(0, 100000);
        m_sliceNumberSpinBox->setValue(histologyCoordinate.getSliceNumber());
        
        const BoundingBox planeBB(histologySlicesFile->getPlaneXyzBoundingBox());
        const Vector3D planeXYZ(histologyCoordinate.getPlaneXYZ());
        for (int32_t i = 0; i < 3; i++) {
            if (histologyCoordinate.isPlaneXYValid()) {
                double minValue(0.0);
                double maxValue(0.0);
                switch (i) {
                    case 0:
                        minValue = planeBB.getMinX();
                        maxValue = planeBB.getMaxX();
                        break;
                    case 1:
                        minValue = planeBB.getMinY();
                        maxValue = planeBB.getMaxY();
                        break;
                    case 2:
                        minValue = planeBB.getMinZ();
                        maxValue = planeBB.getMaxZ();
                        break;
                }
                QSignalBlocker blocker(m_planeXyzSpinBox[i]);
                m_planeXyzSpinBox[i]->setRangeExceedable(minValue, maxValue);
                m_planeXyzSpinBox[i]->setValue(planeXYZ[i]);
                m_planeXyzSpinBox[i]->getWidget()->setEnabled(true);
            }
            else {
                m_planeXyzSpinBox[i]->getWidget()->setEnabled(false);
            }
            
            /*
             * Plane Z always disabled since it should be the same for all slices.
             * Plane coordinates are 2D.
             */
            m_planeXyzSpinBox[2]->getWidget()->setEnabled(false);
        }
        
        const BoundingBox stereotaxicBB(histologySlicesFile->getStereotaxicXyzBoundingBox());
        const Vector3D stereotaxicXYZ(histologyCoordinate.getStereotaxicXYZ());
        for (int32_t i = 0; i < 3; i++) {
            if (histologyCoordinate.isStereotaxicXYZValid()) {
                double minValue(0.0);
                double maxValue(0.0);
                switch (i) {
                    case 0:
                        minValue = stereotaxicBB.getMinX();
                        maxValue = stereotaxicBB.getMaxX();
                        break;
                    case 1:
                        minValue = stereotaxicBB.getMinY();
                        maxValue = stereotaxicBB.getMaxY();
                        break;
                    case 2:
                        minValue = stereotaxicBB.getMinZ();
                        maxValue = stereotaxicBB.getMaxZ();
                        break;
                }
                QSignalBlocker blocker(m_stereotaxicXyzSpinBox[i]);
                m_stereotaxicXyzSpinBox[i]->setRangeExceedable(minValue, maxValue);
                m_stereotaxicXyzSpinBox[i]->setValue(stereotaxicXYZ[i]);
                m_stereotaxicXyzSpinBox[i]->getWidget()->setEnabled(true);
            }
            else {
                m_stereotaxicXyzSpinBox[i]->getWidget()->setEnabled(true);
            }
        }
    }
    
    if (m_browserTabContent != NULL) {
        m_identificationMovesSlicesAction->setChecked(m_browserTabContent->isIdentificationUpdateHistologySlices());
    }
    
    m_nonLinearTransformEnabledCheckBox->setChecked(CziNonLinearTransform::isNonLinearTransformEnabled());
    
    m_overlapCheckBox->setChecked(HistologySlicesFile::isOverlapTestingEnabled());
    
    setEnabled(histologySlicesFile != NULL);
}

/**
 * @return Viewport content for the selected tab (NULL if not available)
 */
const BrainOpenGLViewportContent*
BrainBrowserWindowToolBarHistology::getBrainOpenGLViewportContent() const
{
    std::vector<const BrainOpenGLViewportContent*> viewportContent;
    getParentToolBar()->m_parentBrainBrowserWindow->getAllBrainOpenGLViewportContent(viewportContent);
    for (auto v : viewportContent) {
        if (v->getBrowserTabContent() == m_browserTabContent) {
            return v;
        }
    }
    
    return NULL;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
BrainBrowserWindowToolBarHistology::receiveEvent(Event* event)
{
    BrainBrowserWindowToolBarComponent::receiveEvent(event);
    
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* redrawnEvent =
        dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(redrawnEvent);
        redrawnEvent->setEventProcessed();
        
        EventBrowserTabValidate tabEvent(m_browserTabContent);
        EventManager::get()->sendEvent(tabEvent.getPointer());
        
        if ( ! tabEvent.isValid()) {
            m_browserTabContent = NULL;
        }

        updateContent(m_browserTabContent);
    }
}

/**
 * Get the plane coordinate at the center of the viewport
 * @param planeXyzOut
 *    Output with plane coordinate
 * @return True if  coordinate is valid
 */
bool
BrainBrowserWindowToolBarHistology::getPlaneCoordinateAtViewportCenter(Vector3D& planeXyzOut) const
{
    const BrainOpenGLViewportContent* vpContent(getBrainOpenGLViewportContent());
    const GraphicsObjectToWindowTransform* xform(vpContent->getHistologyGraphicsObjectToWindowTransform());
    CaretAssert(xform);
    if (xform->isValid()) {
        CaretAssert(vpContent);
        int32_t viewport[4];
        vpContent->getModelViewport(viewport);
        const Vector3D vpCenter(viewport[0] + (viewport[2] / 2),
                                viewport[1] + (viewport[3] / 2),
                                0.0);   /* 0.0 or 1.0 ??? */
        xform->inverseTransformPoint(vpCenter,
                                     planeXyzOut);
        return true;
    }
    
    planeXyzOut.fill(0.0);
    
    return false;
}

/**
 * Get the stereotaxic coordinate at the center of the viewport
 * @param histologySlice
 *    Histology slice used to convert plane to stereotaxic
 * @param stereotaxicXyzOut
 *   Output with stereotaxic coordinate
 * @return True if  coordinate is valid
 */
bool
BrainBrowserWindowToolBarHistology::getStereotaxicCoordinateAtViewportCenter(const HistologySlice* histologySlice,
                                                                             Vector3D& stereotaxicXyzOut) const
{
    const BrainOpenGLViewportContent* vpContent(getBrainOpenGLViewportContent());
    const GraphicsObjectToWindowTransform* xform(vpContent->getHistologyGraphicsObjectToWindowTransform());
    CaretAssert(xform);
    if (xform->isValid()) {
        CaretAssert(vpContent);
        int32_t viewport[4];
        vpContent->getModelViewport(viewport);
        const Vector3D vpCenter(viewport[0] + (viewport[2] / 2),
                                viewport[1] + (viewport[3] / 2),
                                0.0);   /* 0.0 or 1.0 ??? */
        Vector3D planeXYZ;
        xform->inverseTransformPoint(vpCenter,
                                     planeXYZ);
        if (histologySlice->planeXyzToStereotaxicXyz(planeXYZ,
                                                     stereotaxicXyzOut)) {
            return true;
        }
    }
    
    stereotaxicXyzOut.fill(0.0);
    
    return false;
}

/**
 * Called when slice index is changed
 * @param sliceIndex
 *    New slice index
 */
void
BrainBrowserWindowToolBarHistology::sliceIndexValueChanged(int sliceIndex)
{
    const bool debugFlag(false);
    
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            CursorDisplayScoped cursor;
            cursor.showWaitCursor();
            
            /*
             * Get current selection information
             */
            const HistologyCoordinate previousHistologyCoord(m_browserTabContent->getHistologySelectedCoordinate(histologySlicesFile));
            
            const int32_t previousSliceIndex(previousHistologyCoord.getSliceIndex());
            const HistologySlice* previousSlice(histologySlicesFile->getHistologySliceByIndex(previousSliceIndex));
            
            /*
             * Get stereotaxic XYZ at center of viewport BEFORE changing slices
             */
            Vector3D centerStereotaxicXYZ;
            bool previousValidFlag(getStereotaxicCoordinateAtViewportCenter(previousSlice,
                                                                            centerStereotaxicXYZ));
            if (debugFlag) std::cout << "Before stereotaxic center: " << centerStereotaxicXYZ.toString(5) << std::endl;

            /*
             * Create new histology coordinate using new slice index
             */
            HistologyCoordinate hc(HistologyCoordinate::newInstanceSliceIndexChanged(histologySlicesFile,
                                                                                     previousHistologyCoord,
                                                                                     sliceIndex));
            
            if (hc.isValid()) {
                if (debugFlag) std::cout << "histology coord stereotaxic: " << hc.getStereotaxicXYZ().toString(5) << std::endl;
                /*
                 * Update with new histology coordinate in browser tab
                 */
                m_browserTabContent->setHistologySelectedCoordinate(histologySlicesFile,
                                                                    hc,
                                                                    BrowserTabContent::MoveYokedVolumeSlices::MOVE_YES);
                
                /*
                 * Changing to an adjacent slice
                 */
                const bool anyStepFlag(true);
                const int32_t sliceStep(std::abs(sliceIndex -  previousSliceIndex));
                if ((sliceStep == 1)
                    || anyStepFlag) {
                    if (previousValidFlag) {
                        /*
                         * Get the new slice
                         */
                        const HistologySlice* histologySlice(histologySlicesFile->getHistologySliceByIndex(sliceIndex));
                        if (histologySlice != NULL) {
                            /*
                             * MUST redraw and wait until done
                             */
                            const bool waitForRedrawFlag(true);
                            EventGraphicsUpdateOneWindow graphicsUpdateOneWindow(m_parentToolBar->browserWindowIndex,
                                                                                 waitForRedrawFlag);
                            EventManager::get()->sendEvent(graphicsUpdateOneWindow.getPointer());
                            
                            /*
                             * Project stereotaxic coord to new slice to get plane coordinate at
                             * same location as stereotaxic coordinate
                             */
                            Vector3D planeXYZ;
                            Vector3D newCenterStereotaxicXYZ;
                            float distanceToSlice(0.0);
                            if (histologySlice->projectStereotaxicXyzToSlice(centerStereotaxicXYZ,
                                                                             newCenterStereotaxicXYZ,
                                                                             distanceToSlice,
                                                                             planeXYZ)) {
                                if (debugFlag) std::cout << "  New stereotaxic: " << newCenterStereotaxicXYZ.toString(5) << std::endl;
                                if (debugFlag) std::cout << "   New plane center should be: " << planeXYZ.toString(5) << std::endl;

                                /*
                                 * Get the plane coordinate at the center of the viewport
                                 */
                                Vector3D centerPlaneXYZ;
                                if (getPlaneCoordinateAtViewportCenter(centerPlaneXYZ)) {
                                    if (debugFlag) std::cout << "   Plane at center of screen: " << centerPlaneXYZ.toString(5) << std::endl;
                                    
                                    /*
                                     * Difference in the plane coordinates is amount to use for translation with zoom.
                                     * This moves the point in the new slice that corresponds to the
                                     * point in the previous slice that was at the center of the viewport
                                     */
                                    const Vector3D diffPlaneXYZ(centerPlaneXYZ - planeXYZ);
                                    if (debugFlag) std::cout << "   Diff Plane XYZ: " << diffPlaneXYZ.toString(5) << std::endl;
                                    
                                    const float zoom(m_browserTabContent->getScaling());
                                    float translation[3];
                                    m_browserTabContent->getTranslation(translation);
                                    translation[0] += (diffPlaneXYZ[0] * zoom);
                                    translation[1] -= (diffPlaneXYZ[1] * zoom);
                                    m_browserTabContent->setTranslation(translation);
                                }
                            }
                        }
                    }
                }
                
                updateGraphicsWindowAndYokedWindows();
                
                updateUserInterface();
            }
        }
    }
}


/**
 * Called when slice number is changed
 * @param sliceNumber
 *    New slice number
 */
void
BrainBrowserWindowToolBarHistology::sliceNumberValueChanged(int sliceNumber)
{
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            const int32_t sliceIndex(histologySlicesFile->getSliceIndexFromSliceNumber(sliceNumber));
            if (sliceIndex >= 0) {
                sliceIndexValueChanged(sliceIndex);
            }
        }
    }
}

/**
 * Called when a plane XYZ spin box value is changed
 */
void
BrainBrowserWindowToolBarHistology::planeXyzSpinBoxValueChanged()
{
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            CursorDisplayScoped cursor;
            cursor.showWaitCursor();

            Vector3D planeXYZ(m_planeXyzSpinBox[0]->value(),
                              m_planeXyzSpinBox[1]->value(),
                              m_planeXyzSpinBox[2]->value());

            const int32_t sliceIndex(m_sliceIndexSpinBox->value());
            HistologyCoordinate hc(HistologyCoordinate::newInstancePlaneXYZChanged(histologySlicesFile,
                                                                                    sliceIndex,
                                                                                    planeXYZ));
            m_browserTabContent->setHistologySelectedCoordinate(histologySlicesFile,
                                                                hc,
                                                                BrowserTabContent::MoveYokedVolumeSlices::MOVE_YES);
            updateGraphicsWindowAndYokedWindows();
            updateUserInterface();
        }
    }
}

/**
 * Called when a stereotaxic XYZ spin box value is changed
 */
void
BrainBrowserWindowToolBarHistology::stereotaxicXyzSpinBoxValueChanged()
{
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            CursorDisplayScoped cursor;
            cursor.showWaitCursor();

            Vector3D xyz(m_stereotaxicXyzSpinBox[0]->value(),
                         m_stereotaxicXyzSpinBox[1]->value(),
                         m_stereotaxicXyzSpinBox[2]->value());

            HistologyCoordinate hc(HistologyCoordinate::newInstanceStereotaxicXYZ(histologySlicesFile,
                                                                                  xyz));
            m_browserTabContent->setHistologySelectedCoordinate(histologySlicesFile,
                                                                hc,
                                                                BrowserTabContent::MoveYokedVolumeSlices::MOVE_YES);
            updateGraphicsWindowAndYokedWindows();
            updateUserInterface();
        }
    }
}

/**
 * Called when identification moves slices action is toggledf
 * @param checked
 *    New 'checked' status
 */
void
BrainBrowserWindowToolBarHistology::identificationMovesSlicesActionTriggered(bool checked)
{
    if (m_browserTabContent != NULL) {
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();

        m_browserTabContent->setIdentificationUpdatesHistologySlices(checked);
        updateUserInterface();
    }
}


/**
 * Called when identification move to center action is toggledf
 */
void
BrainBrowserWindowToolBarHistology::moveToCenterActionTriggered()
{
    if (m_browserTabContent != NULL) {
        if (m_browserTabContent != NULL) {
            CursorDisplayScoped cursor;
            cursor.showWaitCursor();

            HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
            if (histologySlicesFile != NULL) {
                m_browserTabContent->selectHistologySlicesAtOrigin(histologySlicesFile);
                updateGraphicsWindowAndYokedWindows();
                updateUserInterface();
            }
        }
    }
}

/**
 * Called when non-linear checkbox checked
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarHistology::nonLinearTransformEnabledCheckBoxClicked(bool checked)
{
    CziNonLinearTransform::setNonLinearTransformEnabled(checked);
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    updateGraphicsWindowAndYokedWindows();
    updateUserInterface();
}

/**
 * Called when overlap checkbox checked
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarHistology::overlapCheckBoxClicked(bool checked)
{
    HistologySlicesFile::setOverlapTestingEnabled(checked);
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    updateGraphicsWindowAndYokedWindows();
    updateUserInterface();
}

