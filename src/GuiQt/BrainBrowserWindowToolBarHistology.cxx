
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
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWhatsThis>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowToolBarSlicePlane.h"
#include "BrainBrowserWindowToolBarSliceSelection.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "CursorDisplayScoped.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesCziImages.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventBrowserTabValidate.h"
#include "EventGraphicsPaintNowOneWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventGraphicsPaintSoonOneWindow.h"
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
    
    QLabel* sliceNameLabel(new QLabel("Name"));
    m_sliceNameComboBox = new QComboBox();
    QObject::connect(m_sliceNameComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &BrainBrowserWindowToolBarHistology::sliceNameComboBoxActivated);

    WuQtUtilities::matchWidgetWidths(m_sliceIndexSpinBox,
                                     m_sliceNameComboBox);
    
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
        
    /*
     * Yoke orientation check box
     */
    m_yokeOrientationCheckBox = new QCheckBox("Yoke Orientation to MPR");
    m_yokeOrientationCheckBox->setToolTip("<html>"
                                          "Copy slice orientation angles to yoking group MPR volume angles"
                                          "</html>");
    QObject::connect(m_yokeOrientationCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarHistology::yokeOrientationCheckBoxChecked);
    
    /*
     * Crosshair button
     */
    m_showAxisCrosshairsAction = new QAction(this);
    m_showAxisCrosshairsAction->setCheckable(true);
    m_showAxisCrosshairsAction->setToolTip("Show crosshairs on histology slice");
    QObject::connect(m_showAxisCrosshairsAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarHistology::axisCrosshairActionTriggered);
    m_showAxisCrosshairsAction->setObjectName(objectNamePrefix
                                              + "ShowHistologySliceCrosshairs");
    macroManager->addMacroSupportToObject(m_showAxisCrosshairsAction,
                                          "Show histology axis crosshairs");
    
    QToolButton* showCrosshairsToolButton = new QToolButton();
    QPixmap xhairPixmap = BrainBrowserWindowToolBarSlicePlane::createCrosshairsIcon(showCrosshairsToolButton);
    showCrosshairsToolButton->setDefaultAction(m_showAxisCrosshairsAction);
    m_showAxisCrosshairsAction->setIcon(QIcon(xhairPixmap));
    showCrosshairsToolButton->setIconSize(xhairPixmap.size());
    WuQtUtilities::setToolButtonStyleForQt5Mac(showCrosshairsToolButton);

    /*
     * Angles
     */
    QLabel* anglesLabel(new QLabel("Angles"));
    m_rotationAngleXLabel = new QLabel("-000.0");
    m_rotationAngleXLabel->setAlignment(Qt::AlignRight);
    m_rotationAngleYLabel = new QLabel("-000.0");
    m_rotationAngleYLabel->setAlignment(Qt::AlignRight);
    m_rotationAngleZLabel = new QLabel("-000.0");
    m_rotationAngleZLabel->setAlignment(Qt::AlignRight);

    /*
     * Layout widgets
     */
    int columnIndex(0);
    const int columnSliceLabels(columnIndex++);
    const int columnSliceSpinBoxes(columnIndex++);
    const int columnPlaneSpinBoxes(columnIndex++);
    const int columnStereotaxicSpinBoxes(columnIndex++);
    const int columnAngles(columnIndex++);
    
    QGridLayout* controlsLayout(new QGridLayout());
    int row(0);
    controlsLayout->addWidget(sliceLabel,
                              row, columnSliceLabels, 1, 2, Qt::AlignHCenter);
    controlsLayout->addWidget(planeLabel,
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(stereotaxicLabel,
                              row, columnStereotaxicSpinBoxes);
    controlsLayout->addWidget(anglesLabel,
                              row, columnAngles);
    ++row;
    controlsLayout->addWidget(sliceIndexLabel,
                              row, columnSliceLabels);
    controlsLayout->addWidget(m_sliceIndexSpinBox,
                              row, columnSliceSpinBoxes, Qt::AlignRight);
    controlsLayout->addWidget(m_planeXyzSpinBox[0]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[0]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    controlsLayout->addWidget(m_rotationAngleXLabel,
                              row, columnAngles);
    ++row;
    controlsLayout->addWidget(sliceNameLabel,
                              row, columnSliceLabels);
    controlsLayout->addWidget(m_sliceNameComboBox,
                              row, columnSliceSpinBoxes, Qt::AlignRight);
    controlsLayout->addWidget(m_planeXyzSpinBox[1]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[1]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    controlsLayout->addWidget(m_rotationAngleYLabel,
                              row, columnAngles);
    ++row;
    controlsLayout->addWidget(identificationMovesSlicesToolButton,
                              row, columnSliceLabels, Qt::AlignHCenter);
    controlsLayout->addWidget(m_planeXyzSpinBox[2]->getWidget(),
                              row, columnPlaneSpinBoxes);
    controlsLayout->addWidget(m_stereotaxicXyzSpinBox[2]->getWidget(),
                              row, columnStereotaxicSpinBoxes);
    controlsLayout->addWidget(m_rotationAngleZLabel,
                              row, columnAngles);
    ++row;
    controlsLayout->addWidget(showCrosshairsToolButton,
                              row, columnSliceLabels, Qt::AlignHCenter);
    controlsLayout->addWidget(m_yokeOrientationCheckBox,
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

    m_rotationAngleXLabel->setText("");
    m_rotationAngleYLabel->setText("");
    m_rotationAngleZLabel->setText("");

    HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(browserTabContent);
    if (histologySlicesFile != NULL) {
        const HistologyCoordinate histologyCoordinate(m_browserTabContent->getHistologySelectedCoordinate(histologySlicesFile));
        QSignalBlocker indexBlocker(m_sliceIndexSpinBox);
        
        /*
         * Note: Slice indices range 0 to N-1 but we display as 1 to N
         */
        m_sliceIndexSpinBox->setRange(1, histologySlicesFile->getNumberOfHistologySlices());
        m_sliceIndexSpinBox->setValue(histologyCoordinate.getSliceIndex() + 1);

        Vector3D rotationAngles;
        bool rotationAnglesValidFlag(false);
        int32_t selectedItemIndex(0);
        m_sliceNameComboBox->clear();
        const int32_t numSlices(histologySlicesFile->getNumberOfHistologySlices());
        for (int32_t i = 0; i < numSlices; i++) {
            const HistologySlice* slice(histologySlicesFile->getHistologySliceByIndex(i));
            CaretAssert(slice);
            if (slice->getSliceIndex() == histologyCoordinate.getSliceIndex()) {
                selectedItemIndex = i;
                if (slice->getSliceRotationAngles(rotationAngles)) {
                    rotationAnglesValidFlag = true;
                }
            }
            m_sliceNameComboBox->addItem(slice->getSliceName(),
                                           i);
        }
        if ((selectedItemIndex >= 0)
            && (selectedItemIndex < m_sliceNameComboBox->count())) {
            m_sliceNameComboBox->setCurrentIndex(selectedItemIndex);
        }
                
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
            
            if (rotationAnglesValidFlag) {
                m_rotationAngleXLabel->setText(AString::number(rotationAngles[0], 'f', 1));
                m_rotationAngleYLabel->setText(AString::number(rotationAngles[1], 'f', 1));
                m_rotationAngleZLabel->setText(AString::number(rotationAngles[2], 'f', 1));
            }
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
        m_yokeOrientationCheckBox->setChecked(m_browserTabContent->isHistologyOrientationAppliedToYoking());
        m_showAxisCrosshairsAction->setChecked(m_browserTabContent->isHistologyAxesCrosshairsDisplayed());
    }
    
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
 * @param sliceIndexIn
 *    New slice index
 */
void
BrainBrowserWindowToolBarHistology::sliceIndexValueChanged(int sliceIndexIn)
{
    /*
     * Note: Slice indices range 0 to N-1 but we display as 1 to N
     */
    const int sliceIndex(sliceIndexIn - 1);
    
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
                            EventGraphicsPaintNowOneWindow graphicsUpdateOneWindow(m_parentToolBar->browserWindowIndex);
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
 * Called when name combo box is activated
 * @param index
 *    Index of item selected
 */
void
BrainBrowserWindowToolBarHistology::sliceNameComboBoxActivated(int index)
{
    
    if (m_browserTabContent != NULL) {
        HistologySlicesFile* histologySlicesFile = getHistologySlicesFile(m_browserTabContent);
        if (histologySlicesFile != NULL) {
            const int32_t sliceIndex(m_sliceNameComboBox->itemData(index).toInt());
            if (sliceIndex >= 0) {
                /*
                 * Note: Slice indices range 0 to N-1 but we display as 1 to N
                 * 'sliceIndexValueChanged' is called by spin box and expects
                 * 1 to N
                 */
                sliceIndexValueChanged(sliceIndex + 1);
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

            /*
             * Note: Slice indices range 0 to N-1 but we display as 1 to N
             */
            const int32_t sliceIndex(m_sliceIndexSpinBox->value() - 1);
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
 * Called when yoking orientation checkbox clicked
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarHistology::yokeOrientationCheckBoxChecked(bool checked)
{
    if (m_browserTabContent != NULL) {
        m_browserTabContent->setHistologyOrientationAppliedToYoking(checked);
        updateGraphicsWindowAndYokedWindows();
        updateUserInterface();
    }
}

/**
 * Called when crosshairs action is triggered
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarHistology::axisCrosshairActionTriggered(bool checked)
{
    if (m_browserTabContent != NULL) {
        m_browserTabContent->setHistologyAxesCrosshairsDisplayed(checked);
        updateGraphicsWindowAndYokedWindows();
        updateUserInterface();
    }
}

