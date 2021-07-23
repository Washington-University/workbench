
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ANNOTATION_PASTE_DIALOG_DECLARE__
#include "AnnotationPasteDialog.h"
#undef __ANNOTATION_PASTE_DIALOG_DECLARE__

#include <cmath>

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationClipboard.h"
#include "AnnotationCoordinateInformation.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationPastingInformation.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "ModelSurfaceMontage.h"
#include "MouseEvent.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationPasteDialog 
 * \brief Dialog for pasting annotations.
 * \ingroup GuiQt
 */
/**
 * Paste the annotation on the clipboard relative to the mouse event location in
 * the same space as the annotation on the clipboard.  If the annotation cannot
 * be pasted in the annotation's current space, the user is allowed to choose
 * from any other valid spaces containing the annotation
 *
 * @param mouseEvent
 *     Information about where to paste the annotation.
 * @return
 *     Pointer to annotation that was pasted or NULL if the annotation
 *     on the clipboard was not pasted.
 */
Annotation*
AnnotationPasteDialog::pasteAnnotationOnClipboard(const MouseEvent& mouseEvent)
{
    Annotation* newPastedAnnotation = NULL;
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    const AnnotationClipboard* clipboard(annotationManager->getClipboard());
    if (clipboard->isAnnotationValid()) {
        AnnotationFile* annotationFile = clipboard->getAnnotationFile();
        Annotation* annotation(clipboard->getCopyOfAnnotation());
        CaretAssert(annotation);
        
        /*
         * Create information that finds valid spaces for all
         * of the annotations coordinates
         */
        AnnotationPastingInformation pastingInformation(mouseEvent,
                                                        clipboard);
        if ( ! pastingInformation.isValid()) {
            WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                   pastingInformation.getInvalidDescription());
            return NULL;
        }
        
        /*
         * Try pasting the annotation
         */
        const bool pasteSuccessFlag = pasteAnnotationInSpace(annotationFile,
                                                             annotation,
                                                             annotation->getCoordinateSpace(),
                                                             pastingInformation);

        if ( ! pasteSuccessFlag) {
            /*
             * Pasting annotation in its coordinate failed (user may have tried to paste
             * an annotation in surface space where there is no surface).
             */
            delete annotation;
            annotation = NULL;
            
            const QString message("The location for pasting the annotation is incompatible with the "
                                  "coordinate space "
                                  "used by the annotation on the clipboard.  Choose one of the coordinate "
                                  "spaces below to paste the annotation or press Cancel to cancel pasting "
                                  "of the annotation.");
            
            /*
             * Use dialog to allow user to choose a space for pasting
             */
            AnnotationPasteDialog pasteDialog(mouseEvent,
                                              pastingInformation,
                                              annotationFile,
                                              clipboard->getAnnotation(),
                                              message,
                                              mouseEvent.getOpenGLWidget());
            if (pasteDialog.exec() == AnnotationPasteDialog::Accepted) {
                newPastedAnnotation = pasteDialog.getAnnotationThatWasCreated();
            }
        }
    }
    
    return newPastedAnnotation;
}

/**
 * Paste the annotation on the clipboard using the mouse information
 * and allow the user to change the coordinate space.
 *
 * @param mouseEvent
 *     Information about where to paste the annotation.
 * @return
 *     Pointer to annotation that was pasted or NULL if the annotation
 *     on the clipboard was not pasted.
 */
Annotation*
AnnotationPasteDialog::pasteAnnotationOnClipboardChangeSpace(const MouseEvent& mouseEvent)
{
    Annotation* newPastedAnnotation = NULL;
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationClipboard* clipboard = annotationManager->getClipboard();
    if (clipboard->isAnnotationValid()) {
        AnnotationFile* annotationFile = clipboard->getAnnotationFile();
        
        /*
         * Create information that finds valid spaces for all
         * of the annotations coordinates
         */
        AnnotationPastingInformation pastingInformation(mouseEvent,
                                                        clipboard);
        if ( ! pastingInformation.isValid()) {
            WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                   pastingInformation.getInvalidDescription());
            return NULL;
        }
        AString message("Choose one of the coordinate "
                        "spaces below to paste the annotation or press Cancel to cancel pasting "
                        "of the annotation.");

        /*
         * Use dialog to allow user to choose a space for pasting
         */
        AnnotationPasteDialog pasteDialog(mouseEvent,
                                          pastingInformation,
                                          annotationFile,
                                          clipboard->getAnnotation(),
                                          message,
                                          mouseEvent.getOpenGLWidget());
        if (pasteDialog.exec() == AnnotationPasteDialog::Accepted) {
            newPastedAnnotation = pasteDialog.getAnnotationThatWasCreated();
        }
    }
    
    return newPastedAnnotation;
}

/**
 * Constructor.
 *
 * @param mouseEvent
 *     Information about where mouse was clicked.
 * @param annCoordPastingCoordInfo,
 *     Valid spaces and coordinates for annotation that is being pasted
 * @param annotationFile
 *     File that contains the annotation.
 * @param annotation
 *     Annotation that is copied and pasted.
 * @param informationMessage
 *     Message shown on dialog.
 * @param parent
 *     Parent widget of dialog.
 */
AnnotationPasteDialog::AnnotationPasteDialog(const MouseEvent& mouseEvent,
                                             const AnnotationPastingInformation& annotationPastingInformation,
                                             AnnotationFile* annotationFile,
                                             const Annotation* annotation,
                                             const AString& informationMessage,
                                             QWidget* parent)
: WuQDialogModal("Paste Annotation",
                 parent),
m_mouseEvent(mouseEvent),
m_annotationPastingInformation(annotationPastingInformation),
m_annotationFile(annotationFile),
m_annotation(annotation),
m_annotationThatWasCreated(NULL)
{
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    m_spaceRadioButtonsSpaces = m_annotationPastingInformation.getPasteableSpaces();
    CaretAssert( ! m_spaceRadioButtonsSpaces.empty());
    
    /*
     * Radio buttons for selecting coordinate space
     */
    QRadioButton* defaultButton(NULL);
    QButtonGroup* radioButtonGroup = new QButtonGroup(this);
    QGroupBox* coordGroupBox = new QGroupBox("Coordinate Space");
    QVBoxLayout* radioButtonLayout = new QVBoxLayout(coordGroupBox);
    for (const auto& space : m_spaceRadioButtonsSpaces) {
        QRadioButton* rb = new QRadioButton(AnnotationCoordinateSpaceEnum::toGuiName(space));
        m_spaceRadioButtons.push_back(rb);
        radioButtonLayout->addWidget(rb);
        radioButtonGroup->addButton(rb);
        if (space == annotation->getCoordinateSpace()) {
            defaultButton = rb;
        }
    }
    
    if (defaultButton == NULL) {
        CaretAssertVectorIndex(m_spaceRadioButtons, 0);
        defaultButton = m_spaceRadioButtons[0];
    }
    defaultButton->setChecked(true);
        
    QLabel* messageLabel = new QLabel(informationMessage);
    messageLabel->setWordWrap(true);
    
    QLabel* spaceLabel = new QLabel("Space of Annotation on Clipboard: "
                                    + AnnotationCoordinateSpaceEnum::toGuiName(m_annotation->getCoordinateSpace()));
    spaceLabel->setWordWrap(false);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    layout->addWidget(spaceLabel);
    layout->addSpacing(10);
    layout->addWidget(messageLabel);
    layout->addSpacing(10);
    layout->addWidget(coordGroupBox);
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
AnnotationPasteDialog::~AnnotationPasteDialog()
{
}

/**
 * @return Get the annotation that was created.
 */
Annotation*
AnnotationPasteDialog::getAnnotationThatWasCreated()
{
    return m_annotationThatWasCreated;
}

/**
 * Paste the given annotation in the given space using the coordinate information for the annotation's coordinates
 * @param annotationFile
 *    File for the annotation
 * @param annotation
 *    A copy of this annotation will be created
 * @param annotationSpace
 *    Space for pasting
 * @param annotationPastingInformation
 *    Information for pasting the annotation's coordinates
 * @return New annotation that was pasted or NULL if failed to paste the annotation
 */
bool
AnnotationPasteDialog::pasteAnnotationInSpace(AnnotationFile* annotationFile,
                                              Annotation* annotation,
                                              const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                              const AnnotationPastingInformation& annotationPastingInformation)
{
    CaretAssert(annotation);
    
    if ( ! annotationPastingInformation.isValid()) {
        return false;
    }
    
    /*
     * Can annotation be pasted in the given space?
     */
    if ( ! annotationPastingInformation.isPasteableInSpace(annotationSpace)) {
        return false;
    }
    
    const AnnotationCoordinateSpaceEnum::Enum previousSpace(annotation->getCoordinateSpace());
    std::vector<std::unique_ptr<AnnotationCoordinate>> coords(annotation->getCopyOfAllCoordinates());
    const auto& pasteCoords(annotationPastingInformation.getAllCoordinatesInformation());
    CaretAssert(coords.size() == pasteCoords.size());
    bool pasteValidFlag(false);
    
    /*
     * Verify new coordinates are valid for the space
     */
    const int32_t numCoords(coords.size());
    for (int32_t i = 0; i < numCoords; i++) {
        CaretAssertVectorIndex(coords, i);
        CaretAssertVectorIndex(pasteCoords, i);
        auto& ac = coords[i];
        const auto& coordInfo = pasteCoords[i];
        
        switch (annotationSpace) {
            case AnnotationCoordinateSpaceEnum::CHART:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::CHART);
                }
                ac->setXYZ(coordInfo->m_chartSpaceInfo.m_xyz);
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
                }
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SPACER);
                    annotation->setSpacerTabIndex(coordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex);
                }
                ac->setXYZ(coordInfo->m_spacerTabSpaceInfo.m_xyz);
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                }
                ac->setXYZ(coordInfo->m_modelSpaceInfo.m_xyz);
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
            {
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                }
                const auto& ssi = coordInfo->m_surfaceSpaceInfo;
                ac->setSurfaceSpace(ssi.m_structure,
                                    ssi.m_numberOfNodes,
                                    ssi.m_nodeIndex);
            }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                    annotation->setTabIndex(coordInfo->m_tabSpaceInfo.m_index);
                }
                ac->setXYZ(coordInfo->m_tabSpaceInfo.m_xyz);
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssertMessage(0, "Viewport space not supported");
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                if (i == 0) {
                    pasteValidFlag = true;
                    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                    annotation->setWindowIndex(coordInfo->m_windowSpaceInfo.m_index);
                }
                ac->setXYZ(coordInfo->m_windowSpaceInfo.m_xyz);
                break;
        }
    }

    if (pasteValidFlag) {
        annotation->replaceAllCoordinatesNotConst(coords);
        
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModePasteAnnotation(annotationFile,
                                            annotation);
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                CaretAssert(0);
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                CaretAssert(0);
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                break;
            case AnnotationTypeEnum::OVAL:
                break;
            case AnnotationTypeEnum::POLYGON:
                break;
            case AnnotationTypeEnum::POLYLINE:
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                adjustTextAnnotationFontHeight(annotationPastingInformation.getMouseEvent(),
                                               previousSpace,
                                               annotation);
                break;
        }
        
        const MouseEvent& mouseEvent(annotationPastingInformation.getMouseEvent());
        AString errorMessage;
        AnnotationManager* annotationManager(GuiManager::get()->getBrain()->getAnnotationManager());
        if ( ! annotationManager->applyCommand(UserInputModeEnum::Enum::ANNOTATIONS,
                                               undoCommand,
                                               errorMessage)) {
            WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                   errorMessage);
            pasteValidFlag = false;
        }

        if (pasteValidFlag) {
            annotationManager->selectAnnotationForEditing(mouseEvent.getBrowserWindowIndex(),
                                                          AnnotationManager::SELECTION_MODE_SINGLE,
                                                          false,
                                                          annotation);
        }

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    
    return pasteValidFlag;
}

/**
 * Gets called when the OK button is clicked.
 */
void
AnnotationPasteDialog::okButtonClicked()
{
    AString errorMessage;
    
    AnnotationCoordinateSpaceEnum::Enum selectedCoordinateSpace = AnnotationCoordinateSpaceEnum::VIEWPORT;
    bool validSpaceFlag(false);;
    const int32_t numSpaceButtons(m_spaceRadioButtons.size());
    for (int32_t i = 0; i < numSpaceButtons; i++) {
        CaretAssertVectorIndex(m_spaceRadioButtons, i);
        if (m_spaceRadioButtons[i]->isChecked()) {
            CaretAssertVectorIndex(m_spaceRadioButtonsSpaces, i);
            selectedCoordinateSpace = m_spaceRadioButtonsSpaces[i];
            validSpaceFlag = true;
        }
    }
    if ( ! validSpaceFlag) {
        const QString msg("A coordinate space has not been selected.");
        WuQMessageBox::errorOk(this,
                               msg);
        return;
    }
    
    Annotation* newAnnotation(m_annotation->clone());
    const bool pasteValidFlag = pasteAnnotationInSpace(m_annotationFile,
                                                       newAnnotation,
                                                       selectedCoordinateSpace,
                                                       m_annotationPastingInformation);
    if ( ! pasteValidFlag) {
        delete newAnnotation;
        WuQMessageBox::errorOk(this, "Failed to paste annotation");
        return;
    }

    m_annotationThatWasCreated = newAnnotation;

    WuQDialog::okButtonClicked();
}

/**
 * If the given annotation is a text annotation and the space is changed to 
 * surface, we may need to adjust the height if in surface montage.  This 
 * results in the surface space pixel height being the same as the tab 
 * space text pixel height when in surface montage.
 *
 * Inverse logic is need when converting text annotation from surface
 * space to another space.
 *
 * @param mouseEvent
 *   Mouse event for pasting the annotation
 * @param previousSpace
 *      Space of annotation that was on the clipboard.
 * @param annotation
 *      Annotation that is being pasted.
 */
void
AnnotationPasteDialog::adjustTextAnnotationFontHeight(const MouseEvent& mouseEvent,
                                                      const AnnotationCoordinateSpaceEnum::Enum previousSpace,
                                                      Annotation* annotation)
{
    CaretAssert(annotation);
    
    BrainOpenGLViewportContent* vpContent = mouseEvent.getViewportContent();
    CaretAssert(vpContent);
    
    int32_t surfaceMontageRowCount = 1;
    BrowserTabContent* btc = vpContent->getBrowserTabContent();
    if (btc != NULL) {
        const ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        if (msm != NULL) {
            int32_t columnCount = 1;
            msm->getSurfaceMontageNumberOfRowsAndColumns(btc->getTabNumber(),
                                                         surfaceMontageRowCount,
                                                         columnCount);
        }
    }
    
    const AnnotationCoordinateSpaceEnum::Enum newSpace = annotation->getCoordinateSpace();
    
    const bool previousSpaceStereoOrSurfaceFlag = ((previousSpace == AnnotationCoordinateSpaceEnum::STEREOTAXIC)
                                                   || (previousSpace == AnnotationCoordinateSpaceEnum::SURFACE));
    const bool newSpaceStereoOrSurfaceFlag = ((newSpace == AnnotationCoordinateSpaceEnum::STEREOTAXIC)
                                                   || (newSpace == AnnotationCoordinateSpaceEnum::SURFACE));
    if (surfaceMontageRowCount > 1) {
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            
            float heightMultiplier = 0.0;
            
            if ( ! previousSpaceStereoOrSurfaceFlag) {
                if (newSpaceStereoOrSurfaceFlag) {
                    /*
                     * Converting to surface
                     */
                    heightMultiplier = surfaceMontageRowCount;
                }
            }
            else {
                if ( ! newSpaceStereoOrSurfaceFlag) {
                    /*
                     * Converting from surface
                     */
                    heightMultiplier = 1.0 / surfaceMontageRowCount;
                }
            }
            
            if (heightMultiplier != 0.0) {
                AnnotationPercentSizeText* textAnn = dynamic_cast<AnnotationPercentSizeText*>(annotation);
                if (textAnn != NULL) {
                    float percentHeight = textAnn->getFontPercentViewportSize();
                    percentHeight *= heightMultiplier;
                    textAnn->setFontPercentViewportSize(percentHeight);
                }
            }
        }
    }
}


