
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__
#include "UserInputModeAnnotationsContextMenu.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__

#include <cmath>

#include <QLineEdit>

#include "AnnotationClipboard.h"
#include "AnnotationCoordinate.h"
#include "AnnotationCoordinateInformation.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationPolygon.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationPolyLine.h"
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "AnnotationTextEditorDialog.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabGetAtWindowXY.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "MetaDataCustomEditorDialog.h"
#include "MetaDataEditorDialog.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "UserInputModeAnnotations.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQTextEditorDialog.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeAnnotationsContextMenu 
 * \brief Context (pop-up) menu for User Input Annotations Mode.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param mouseEvent
 *    The mouse event that caused display of this menu.
 * @param selectionManager
 *    The selection manager, provides data under the cursor.
 * @param browserTabContent
 *    Content of browser tab.  May be NULL.
 * @param parentOpenGLWidget
 *    Parent OpenGL Widget on which the menu is displayed.
 */
UserInputModeAnnotationsContextMenu::UserInputModeAnnotationsContextMenu(UserInputModeAnnotations* userInputModeAnnotations,
                                                                         const MouseEvent& mouseEvent,
                                                                         SelectionManager* selectionManager,
                                                                         BrowserTabContent* browserTabContent,
                                                                         BrainOpenGLWidget* parentOpenGLWidget)
: QMenu(parentOpenGLWidget),
m_userInputModeAnnotations(userInputModeAnnotations),
m_mouseEvent(mouseEvent),
m_selectionManager(selectionManager),
m_browserTabContent(browserTabContent),
m_parentOpenGLWidget(parentOpenGLWidget),
m_newAnnotationCreatedByContextMenu(NULL)
{
    CaretAssert(m_userInputModeAnnotations);
    CaretAssert(selectionManager);
    
    bool samplesModeFlag(false);
    switch (m_userInputModeAnnotations->getUserInputMode()) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            break;
        case UserInputModeEnum::Enum::BORDERS:
            break;
        case UserInputModeEnum::Enum::FOCI:
            break;
        case UserInputModeEnum::Enum::IMAGE:
            break;
        case UserInputModeEnum::Enum::INVALID:
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            samplesModeFlag = true;
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            break;
        case UserInputModeEnum::Enum::VIEW:
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            break;
    }
    
    const int32_t browserWindexIndex = m_mouseEvent.getBrowserWindowIndex();
    std::vector<AnnotationAndFile> selectedAnnotations;
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    annotationManager->getAnnotationsAndFilesSelectedForEditingIncludingLabels(browserWindexIndex,
                                                                       selectedAnnotations);
    
    m_annotationFile = NULL;
    m_annotation     = NULL;
    m_polyAnnCoordinateSelected = -1;
    
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputModeAnnotations->getUserInputMode(),
                                                      m_userInputModeAnnotations->getBrowserWindowIndex());
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const bool drawingAnnotationFlag(annDrawEvent.isAnnotationDrawingInProgress());

    bool allSelectedAnnotationsDeletableFlag = true;
    if (selectedAnnotations.empty()) {
        allSelectedAnnotationsDeletableFlag = false;
    }
    
    bool cutCopyValidFlag(true);
    int32_t tabIndex(-1);
    m_tabSpaceFileAndAnnotations.clear();
    m_threeDimCoordAnnotations.clear();
    for (std::vector<AnnotationAndFile>::iterator iter = selectedAnnotations.begin();
         iter != selectedAnnotations.end();
         iter++) {
        Annotation* ann = iter->getAnnotation();
        CaretAssert(ann);
        
        bool threeDimCoordFlag = false;
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                if (tabIndex < 0) {
                    tabIndex = ann->getTabIndex();
                }
                if (tabIndex == ann->getTabIndex()) {
                    m_tabSpaceFileAndAnnotations.push_back(std::make_pair(iter->getFile(),
                                                                          ann));
                }
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        if (threeDimCoordFlag) {
            m_threeDimCoordAnnotations.push_back(ann);
        }
        
        if ( ! ann->testProperty(Annotation::Property::DELETION)) {
            allSelectedAnnotationsDeletableFlag = false;
        }
        
        if ( !ann->testProperty(Annotation::Property::COPY_CUT_PASTE)) {
            cutCopyValidFlag = false;
        }
    }
    const bool haveThreeDimCoordAnnotationsFlag = ( ! m_threeDimCoordAnnotations.empty());

    /*
     * For tab space annotations, all selected annotations MUST be in the same tab
     */
    if (m_tabSpaceFileAndAnnotations.size() != selectedAnnotations.size()) {
        m_tabSpaceFileAndAnnotations.clear();
    }
    
    const AnnotationPolygon*  polygon(NULL);
    const AnnotationPolyLine* polyline(NULL);
    m_polyhedronAnnotation = NULL;
    AString polyAnnTypeName("ERROR");
    int32_t polyAnnNumberOfCoordinates(-1);
    bool polyAnnInsertAllowedFlag(false);
    bool oneAnnotationSelectedFlag(false);
    bool oneDeletableAnnotationSelectedFlag = false;
    bool polyAnnRemoveCoordinateAllowedFlag(false);

    if (selectedAnnotations.size() == 1) {
        oneAnnotationSelectedFlag = true;
        
        CaretAssertVectorIndex(selectedAnnotations, 0);
        m_annotationFile = selectedAnnotations[0].getFile();
        m_annotation     = selectedAnnotations[0].getAnnotation();
        CaretAssert(m_annotation);
        if (m_annotation->testProperty(Annotation::Property::DELETION)) {
            oneDeletableAnnotationSelectedFlag = true;
        }
        
        polygon = m_annotation->castToPolygon();
        polyline = m_annotation->castToPolyline();
        m_polyhedronAnnotation = m_annotation->castToPolyhedron();
        if (drawingAnnotationFlag) {
            m_polyhedronAnnotation = NULL;
        }

        polyAnnTypeName = AnnotationTypeEnum::toGuiName(m_annotation->getType());

        const SelectionItemAnnotation* annSel(getSelectionItem(m_userInputModeAnnotations));
        if (annSel->isValid()) {
            bool sizeHandleSelectedFlag(false);
            switch (annSel->getSizingHandle()) {
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
                    sizeHandleSelectedFlag = true;
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
                    break;
                case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
                    break;
            }
            
            m_polyAnnCoordinateSelected = annSel->getPolyLineCoordinateIndex();
            polyAnnNumberOfCoordinates   = m_annotation->getNumberOfCoordinates();

            if (m_polyAnnCoordinateSelected >= 0) {
                polyAnnInsertAllowedFlag = true;
                if (sizeHandleSelectedFlag) {
                    polyAnnInsertAllowedFlag = false;
                }
                if (polyAnnNumberOfCoordinates >= 3) {
                    if (polygon != NULL) {
                        if (polyAnnNumberOfCoordinates >= 4) {
                            if (sizeHandleSelectedFlag) {
                                polyAnnRemoveCoordinateAllowedFlag = true;
                            }
                        }
                    }
                    else if (polyline != NULL) {
                        if (sizeHandleSelectedFlag) {
                            polyAnnRemoveCoordinateAllowedFlag = true;
                        }
                    }
                    else if (m_polyhedronAnnotation != NULL) {
                        /*
                         * Note: Polyhedron contains pairs of coordinates so 8 is really 4 coordinates
                         */
                        if (polyAnnNumberOfCoordinates >= 8) {
                            if (sizeHandleSelectedFlag) {
                                polyAnnRemoveCoordinateAllowedFlag = true;
                            }
                        }
                    }
                    else {
                        CaretAssert(0);
                    }
                }
            }
        }
    }
    
    m_textAnnotation = NULL;
    if (m_annotation != NULL) {
        m_textAnnotation = dynamic_cast<AnnotationText*>(m_annotation);
    }
    
    std::vector<BrainBrowserWindowEditMenuItemEnum::Enum> editMenuItemsEnabled;
    AString redoMenuItemSuffix;
    AString undoMenuItemSuffix;
    AString pasteText;
    AString pasteSpecialText;
    userInputModeAnnotations->getEnabledEditMenuItems(editMenuItemsEnabled,
                                                      redoMenuItemSuffix,
                                                      undoMenuItemSuffix,
                                                      pasteText,
                                                      pasteSpecialText);
    
    /*
     * Cut
     */
    QAction* cutAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::CUT),
                                   this, SLOT(cutAnnnotation()));
    cutAction->setEnabled(oneDeletableAnnotationSelectedFlag
                          && cutCopyValidFlag);
    cutAction->setEnabled(std::find(editMenuItemsEnabled.begin(),
                                    editMenuItemsEnabled.end(),
                                    BrainBrowserWindowEditMenuItemEnum::CUT)
                          != editMenuItemsEnabled.end());
    
    /*
     * Copy
     */
    QAction* copyAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::COPY),
                                    this, SLOT(copyAnnotationToAnnotationClipboard()));
    copyAction->setEnabled(oneDeletableAnnotationSelectedFlag
                           && cutCopyValidFlag);
    copyAction->setEnabled(std::find(editMenuItemsEnabled.begin(),
                                    editMenuItemsEnabled.end(),
                                    BrainBrowserWindowEditMenuItemEnum::COPY)
                           != editMenuItemsEnabled.end());

    /*
     * Delete
     */
    QAction* deleteAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::DELETER),
                                      this, SLOT(deleteAnnotations()));
    deleteAction->setEnabled(allSelectedAnnotationsDeletableFlag);
    deleteAction->setEnabled(std::find(editMenuItemsEnabled.begin(),
                                       editMenuItemsEnabled.end(),
                                       BrainBrowserWindowEditMenuItemEnum::DELETER)
                             != editMenuItemsEnabled.end());

    /*
     * Duplicate tab space annotation
     */
    QMenu* duplicateMenu = createDuplicateTabSpaceAnnotationMenu();
    if (duplicateMenu != NULL) {
        addMenu(duplicateMenu);
    }
    
    /*
     * Paste
     */
    const AnnotationClipboard* clipboard(annotationManager->getClipboard());
    QAction* pasteAction = addAction(pasteText,
                                     this, SLOT(pasteAnnotationFromAnnotationClipboard()));
    pasteAction->setEnabled( ! clipboard->isEmpty());

    /*
     * Paste Special
     */
    QAction* pasteSpecialAction = addAction(pasteSpecialText,
                                           this, SLOT(pasteSpecialAnnotationFromAnnotationClipboard()));
    pasteSpecialAction->setEnabled( ! clipboard->isEmpty());

    /*
     * Separator
     */
    addSeparator();
    
    /*
     * De/Select All annotations
     */
    QAction* deselectAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::DESELECT_ALL),
                                        this, SLOT(deselectAllAnnotations()));
    deselectAction->setEnabled( ! selectedAnnotations.empty());
    addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::SELECT_ALL),
              this, SLOT(selectAllAnnotations()));
    
    /*
     * Separator
     */
    addSeparator();
    
    /*
     * Insert poly line coordinate at mouse
     */
    QAction* insertPolylineCoordinateAction = addAction("Insert "
                                                        + polyAnnTypeName
                                                        + " Coordinate",
                                                        this,
                                                        &UserInputModeAnnotationsContextMenu::insertPolylineCoordinate);
    bool insertValidFlag(false);
    if (polyAnnInsertAllowedFlag
        && (m_polyAnnCoordinateSelected >= 0)) {
        if (polygon != NULL) {
            if (m_polyAnnCoordinateSelected < polyAnnNumberOfCoordinates) {
                insertValidFlag = true;
            }
        }
        else if (polyline != NULL) {
            if (m_polyAnnCoordinateSelected < (polyAnnNumberOfCoordinates - 1)) {
                insertValidFlag = true;
            }
        }
        else if (m_polyhedronAnnotation != NULL) {
            if (m_polyAnnCoordinateSelected < polyAnnNumberOfCoordinates) {
                insertValidFlag = true;
            }
        }
    }
    insertPolylineCoordinateAction->setEnabled(insertValidFlag);
    
    /*
     * Remove poly line coordinate
     */
    QAction* removePolylineCoordinateAction = addAction("Remove "
                                                        + polyAnnTypeName
                                                        + " Coordinate",
                                                    this, &UserInputModeAnnotationsContextMenu::removePolylineCoordinateSelected);
    removePolylineCoordinateAction->setEnabled(polyAnnRemoveCoordinateAllowedFlag);
    
    if (samplesModeFlag) {
        QAction* resetSliceRangeAction = addAction("Reset Slice Range...",
                                                   this,
                                                   &UserInputModeAnnotationsContextMenu::resetPolyhedronSliceRangeSelected);
        resetSliceRangeAction->setEnabled(m_polyhedronAnnotation != NULL);
    }
    
    /*
     * Separator
     */
    addSeparator();

    if ( ! samplesModeFlag) {
        /*
         * Order Operations
         */
        QAction* bringToFrontAction = addAction("Bring to Front");
        QObject::connect(bringToFrontAction, &QAction::triggered,
                         this, [=]() { processAnnotationOrderOperation(AnnotationStackingOrderTypeEnum::BRING_TO_FRONT); });
        QAction* bringForwardAction = addAction("Bring Forward");
        QObject::connect(bringForwardAction, &QAction::triggered,
                         this, [=]() { processAnnotationOrderOperation(AnnotationStackingOrderTypeEnum::BRING_FORWARD); });
        QAction* sendToBackAction = addAction("Send to Back");
        QObject::connect(sendToBackAction, &QAction::triggered,
                         this, [=]() { processAnnotationOrderOperation(AnnotationStackingOrderTypeEnum::SEND_TO_BACK); });
        QAction* sendBackwardAction = addAction("Send Backward");
        QObject::connect(sendBackwardAction, &QAction::triggered,
                         this, [=]() { processAnnotationOrderOperation(AnnotationStackingOrderTypeEnum::SEND_BACKWARD); });
        
        bringToFrontAction->setEnabled(oneAnnotationSelectedFlag);
        bringForwardAction->setEnabled(oneAnnotationSelectedFlag);
        sendToBackAction->setEnabled(oneAnnotationSelectedFlag);
        sendBackwardAction->setEnabled(oneAnnotationSelectedFlag);
        
        /*
         * Separator
         */
        addSeparator();
    }
    
    if ( ! samplesModeFlag) {
        /*
         * Edit Text
         */
        QAction* editTextAction = addAction("Edit Text...",
                                            this, SLOT(setAnnotationText()));
        editTextAction->setEnabled(m_textAnnotation != NULL);
    }
    
    
    if (samplesModeFlag) {
        QAction* polyhedronInformationAction(addAction("Information...",
                                                 this,
                                                 &UserInputModeAnnotationsContextMenu::polyhedronInformationSelected));
        polyhedronInformationAction->setEnabled(m_polyhedronAnnotation != NULL);
    }
    
    /*
     * Edit Metadata
     */
    QAction* editMetaDataAction = addAction("Edit Metadata...",
                                            this,
                                            &UserInputModeAnnotationsContextMenu::editMetaDataDialog);
    editMetaDataAction->setEnabled(oneAnnotationSelectedFlag);
    
    if ( ! samplesModeFlag) {
        /*
         * Separator
         */
        addSeparator();
        
        /*
         * Turn off display in tabs
         */
        QAction* turnOffTabDisplayAction = addAction("Turn Off Chart/Stereotaxic/Surface Annotation Display in Other Tabs",
                                                     this, SLOT(turnOffDisplayInOtherTabs()));
        
        /*
         * Separator
         */
        addSeparator();
        
        /*
         * Turn on display in tabs
         */
        QAction* turnOnTabDisplayAction = addAction("Turn On Chart/Stereotaxic/Surface Annotation Display in All Tabs",
                                                    this, SLOT(turnOnDisplayInAllTabs()));
        turnOffTabDisplayAction->setEnabled(haveThreeDimCoordAnnotationsFlag);
        turnOnTabDisplayAction->setEnabled(haveThreeDimCoordAnnotationsFlag);
        
        /*
         * Turn on/off display in groups
         */
        QAction* turnOnGroupDisplayAction = addAction("Turn On Chart/Stereotaxic/Surface Annotation Display in All Groups",
                                                      this, SLOT(turnOnDisplayInAllGroups()));
        turnOnGroupDisplayAction->setEnabled(haveThreeDimCoordAnnotationsFlag);
        QMenu* turnOnInDisplayGroupMenu = createTurnOnInDisplayGroupMenu();
        turnOnInDisplayGroupMenu->setEnabled(haveThreeDimCoordAnnotationsFlag);
        addMenu(turnOnInDisplayGroupMenu);
    }
    
    if ( ! samplesModeFlag) {
        /*
         * Separator
         */
        addSeparator();
        
        /*
         * Group annotations
         */
        QAction* groupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::GROUP),
                                         this, SLOT(applyGroupingGroup()));
        groupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                       AnnotationGroupingModeEnum::GROUP));
        
        /*
         * Ungroup annotations
         */
        QAction* ungroupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::UNGROUP),
                                           this, SLOT(applyGroupingUngroup()));
        ungroupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                         AnnotationGroupingModeEnum::UNGROUP));
        
        /*
         * Regroup annotations
         */
        QAction* regroupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::REGROUP),
                                           this, SLOT(applyGroupingRegroup()));
        regroupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                         AnnotationGroupingModeEnum::REGROUP));
    }
    
    if (samplesModeFlag) {
//        EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputModeAnnotations->getUserInputMode(),
//                                                          m_userInputModeAnnotations->getBrowserWindowIndex());
//        EventManager::get()->sendEvent(annDrawEvent.getPointer());
//        const bool drawingAnnotationFlag(annDrawEvent.isAnnotationDrawingInProgress());
//
//        if ( ! drawingAnnotationFlag) {
//            if (polyhedron != NULL) {
////                CaretAssertVectorIndex(annotations, 0);
////                CaretAssert(annotations[0]);
////                m_polyhedronSelected = annotations[0]->castToPolyhedron();
//            }
//        }
        
        const Annotation* lockedAnnotation(Annotation::getSelectionLockedPolyhedronInWindow(m_userInputModeAnnotations->getBrowserWindowIndex()));
        addSeparator();
        
        QAction* lockAction(addAction("Lock",
                                      this,
                                      &UserInputModeAnnotationsContextMenu::lockPolyhedronSelected));
        lockAction->setEnabled((m_polyhedronAnnotation != NULL)
                               && (m_polyhedronAnnotation != lockedAnnotation));
        
        QAction* unlockAction(addAction("Unlock",
                                        this,
                                        &UserInputModeAnnotationsContextMenu::unlockPolyhedronSelected));
        unlockAction->setEnabled(lockedAnnotation != NULL);
    }
}

/**
 * Destructor.
 */
UserInputModeAnnotationsContextMenu::~UserInputModeAnnotationsContextMenu()
{
}

Annotation*
UserInputModeAnnotationsContextMenu::getNewAnnotationCreatedByContextMenu()
{
    return m_newAnnotationCreatedByContextMenu;
}

/**
 * Copy the annotation to the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::copyAnnotationToAnnotationClipboard()
{
    m_userInputModeAnnotations->processEditMenuItemSelection(BrainBrowserWindowEditMenuItemEnum::COPY);
}

/**
 * Cut the selected annotation.
 */
void
UserInputModeAnnotationsContextMenu::cutAnnnotation()
{
    m_userInputModeAnnotations->cutAnnotation();
}

/**
 * Delete the annotation.
 */
void
UserInputModeAnnotationsContextMenu::deleteAnnotations()
{
    /*
     * Delete the annotation that is under the mouse
     */
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_mouseEvent.getBrowserWindowIndex());
    if ( ! selectedAnnotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeDeleteAnnotations(selectedAnnotations);
        AString errorMessage;
        if ( ! annotationManager->applyCommand(undoCommand,
                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Paste the annotation from the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::pasteAnnotationFromAnnotationClipboard()
{
    m_userInputModeAnnotations->pasteAnnotationFromAnnotationClipboard(m_mouseEvent);
}

/**
 * Paste special the annotation from the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::pasteSpecialAnnotationFromAnnotationClipboard()
{
    m_userInputModeAnnotations->pasteAnnotationFromAnnotationClipboardAndChangeSpace(m_mouseEvent);
}

/**
 * Deselect all annotations in the window.
 */
void
UserInputModeAnnotationsContextMenu::deselectAllAnnotations()
{
    m_userInputModeAnnotations->processDeselectAllAnnotations();
}

/**
 * Select all annotations in the window.
 */
void
UserInputModeAnnotationsContextMenu::selectAllAnnotations()
{
    m_userInputModeAnnotations->processSelectAllAnnotations();
}

/**
 * Set the text for an annotation.
 */
void
UserInputModeAnnotationsContextMenu::setAnnotationText()
{
    CaretAssert(m_textAnnotation);
    
    AnnotationTextEditorDialog ted(m_userInputModeAnnotations->getUserInputMode(),
                                   m_textAnnotation,
                                   this);
    /*
     * Note: Y==0 is at top for widget.
     *       Y==0 is at bottom for OpenGL mouse x,y
     */
    QPoint diaglogPos(this->pos().x(),
                      this->pos().y() + 20);
    ted.move(diaglogPos);
    ted.exec();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Edit an annotation's metadata
 */
void
UserInputModeAnnotationsContextMenu::editMetaDataDialog()
{
    if (m_userInputModeAnnotations->getUserInputMode() == UserInputModeEnum::Enum::SAMPLES_EDITING) {
        CaretAssert(m_annotation);
        MetaDataCustomEditorDialog dialog(m_annotation,
                                          this);
        dialog.exec();
    }
    else {
        CaretAssert(m_annotation);
        MetaDataEditorDialog dialog(m_annotation,
                                    this);
        dialog.exec();
    }
}

/**
 * Turn off display of annotation in other tabs.
 */
void
UserInputModeAnnotationsContextMenu::turnOffDisplayInOtherTabs()
{
    if (m_browserTabContent == NULL) {
        return;
    }
    for (std::vector<Annotation*>::iterator iter = m_threeDimCoordAnnotations.begin();
         iter != m_threeDimCoordAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInOneTab(m_browserTabContent->getTabNumber());
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Turn on display of annotation in all tabs.
 */
void
UserInputModeAnnotationsContextMenu::turnOnDisplayInAllTabs()
{
    for (std::vector<Annotation*>::iterator iter = m_threeDimCoordAnnotations.begin();
         iter != m_threeDimCoordAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInAllTabs();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Turn on display of annotation in all groups.
 */
void
UserInputModeAnnotationsContextMenu::turnOnDisplayInAllGroups()
{
    for (std::vector<Annotation*>::iterator iter = m_threeDimCoordAnnotations.begin();
         iter != m_threeDimCoordAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInAllGroups();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Turn on display of annotation in a group and off in other groups
 *
 * @param action
 *    Menu action that was selected.
 */
void
UserInputModeAnnotationsContextMenu::turnOnDisplayInGroup(QAction* action)
{
    const int intValue = action->data().toInt();
    bool validFlag = false;
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::fromIntegerCode(intValue,
                                                                            &validFlag);
    if ( ! validFlag) {
        CaretAssert(0);
        return;
    }
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssert(0);  /* TAB NOT ALLOWED */
        return;
    }
    
    for (std::vector<Annotation*>::iterator iter = m_threeDimCoordAnnotations.begin();
         iter != m_threeDimCoordAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInOneGroup(displayGroup);
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * @return New menu for turning on in display group
 */
QMenu*
UserInputModeAnnotationsContextMenu::createTurnOnInDisplayGroupMenu()
{
    QMenu* menu = new QMenu("Turn On Chart/Stereotaxic/Surface Annotation Only in Group");
    QObject::connect(menu, SIGNAL(triggered(QAction*)),
                     this, SLOT(turnOnDisplayInGroup(QAction*)));
    
    std::vector<DisplayGroupEnum::Enum> groupEnums;
    DisplayGroupEnum::getAllEnumsExceptTab(groupEnums);
    
    for (std::vector<DisplayGroupEnum::Enum>::iterator iter = groupEnums.begin();
         iter != groupEnums.end();
         iter++) {
        const DisplayGroupEnum::Enum dg = *iter;
        QAction* action = menu->addAction(DisplayGroupEnum::toGuiName(dg));
        action->setData((int)DisplayGroupEnum::toIntegerCode(dg));
    }
    
    return menu;
}

/**
 * @return New menu for duplicating a tab annotation.
 *         NULL is returned if no other tabs.
 */
QMenu*
UserInputModeAnnotationsContextMenu::createDuplicateTabSpaceAnnotationMenu()
{
    if (m_tabSpaceFileAndAnnotations.empty()) {
        return NULL;
    }
    
    EventBrowserTabGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    const std::vector<BrowserTabContent*> allTabs = tabIndicesEvent.getAllBrowserTabs();
    
    if (allTabs.size() < 1) {
        return NULL;
    }
    
    QMenu* menu = new QMenu("Duplicate to Tab");
    QObject::connect(menu, SIGNAL(triggered(QAction*)),
                     this, SLOT(duplicateAnnotationSelected(QAction*)));
    
    CaretAssertVectorIndex(m_tabSpaceFileAndAnnotations, 0);
    CaretAssert(m_tabSpaceFileAndAnnotations[0].second->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB);
    const int32_t tabIndex = m_tabSpaceFileAndAnnotations[0].second->getTabIndex();
    for (BrowserTabContent* tabContent : allTabs) {
        if (tabContent->getTabNumber() != tabIndex) {
            QAction* action = menu->addAction(tabContent->getTabName());
            action->setData((int)tabContent->getTabNumber());
        }
    }

    return menu;
}

/**
 * Gets called when a selection is made from Duplicate Tab Annotation menu
 *
 * @param action
 *     Action selected from Duplicate menu
 */
void
UserInputModeAnnotationsContextMenu::duplicateAnnotationSelected(QAction* action)
{
    CaretAssert(action);
    CaretAssert(m_tabSpaceFileAndAnnotations.size() > 0);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    
    const int32_t tabIndex = action->data().toInt();
    
    std::vector<AnnotationAndFile> fileAnnCopies;
    for (const auto& tabAnn : m_tabSpaceFileAndAnnotations) {
        Annotation* annCopy = tabAnn.second->clone();
        annCopy->setTabIndex(tabIndex);
        
        DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
        dpa->updateForNewAnnotation(annCopy);
        
        fileAnnCopies.emplace_back(annCopy,
                                   tabAnn.first,
                                   tabAnn.second->getAnnotationGroupKey());
    }

    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeDuplicateAnnotations(fileAnnCopies);
    AString errorMessage;
    if ( ! annotationManager->applyCommand(undoCommand,
                                           errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }

    bool firstTimeFlag(true);
    for (auto& annCopy : fileAnnCopies) {
        if (firstTimeFlag) {
            firstTimeFlag = false;
            annotationManager->selectAnnotationForEditing(m_mouseEvent.getBrowserWindowIndex(),
                                                          AnnotationManager::SELECTION_MODE_SINGLE,
                                                          false, /* shift key down */
                                                          annCopy.getAnnotation());
        }
        else {
            annotationManager->selectAnnotationForEditing(m_mouseEvent.getBrowserWindowIndex(),
                                                          AnnotationManager::SELECTION_MODE_EXTENDED,
                                                          true, /* shift key down */
                                                          annCopy.getAnnotation());
        }
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Group annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingGroup()
{
    applyGrouping(AnnotationGroupingModeEnum::GROUP);
}

/**
 * Ungroup annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingRegroup()
{
    applyGrouping(AnnotationGroupingModeEnum::REGROUP);
}

/**
 * Regroup annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingUngroup()
{
    applyGrouping(AnnotationGroupingModeEnum::UNGROUP);
}

/**
 * Apply grouping selection.
 *
 * @param grouping
 *     Selected grouping.
 */
void
UserInputModeAnnotationsContextMenu::applyGrouping(const AnnotationGroupingModeEnum::Enum grouping)
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    
    AString errorMessage;
    if ( ! annMan->applyGroupingMode(m_mouseEvent.getBrowserWindowIndex(),
                                     grouping,
                                     errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to process an annotation order operation
 *
 * @param orderType
 *     The ordering type
 */
void
UserInputModeAnnotationsContextMenu::processAnnotationOrderOperation(const AnnotationStackingOrderTypeEnum::Enum orderType)
{
    const int32_t browserWindowIndex = m_mouseEvent.getBrowserWindowIndex();
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    std::vector<Annotation*> selectedAnnotations = annMan->getAnnotationsSelectedForEditing(browserWindowIndex);
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        Annotation* selectedAnn = selectedAnnotations[0];
        std::vector<Annotation*> sameSpaceAnnotations = annMan->getAnnotationsDrawnInSameWindowAndSpace(selectedAnn,
                                                                                                        browserWindowIndex);
        if ( ! sameSpaceAnnotations.empty()) {
            sameSpaceAnnotations.push_back(selectedAnn);
            
            AString errorMessage;
            if ( ! annMan->applyStackingOrder(sameSpaceAnnotations,
                                              selectedAnn,
                                              orderType,
                                              browserWindowIndex,
                                              errorMessage)) {
                WuQMessageBox::errorOk(this,
                                       errorMessage);
            }
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        }
    }
}

/**
 * Called when remove polyline coordinate is selected
 */
void
UserInputModeAnnotationsContextMenu::removePolylineCoordinateSelected()
{
    /*
     * Remove coordinate from annotation
     */
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputModeAnnotations->getUserInputMode());
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_mouseEvent.getBrowserWindowIndex());
    if ( ! selectedAnnotations.empty()) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeMultiCoordAnnRemoveCoordinate(m_polyAnnCoordinateSelected,
                                                          selectedAnnotations[0]);
        AString errorMessage;
        if ( ! annotationManager->applyCommand(undoCommand,
                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Reset the slice range of a polyhedron.  This method is static so that it can be used by other parts of the GUI.
 * @param polyhedron
 *    The polyhedron
 * @param browserWindowIndex
 *    Index of window containing the polyhedron
 * @param mouseXY
 *    Location of mouse (used to find browser tab containing the polyhedron)
 * @param extraMessageInfo
 *    Text added to warning message
 * @param parentWidgetForErrorMessage
 *    Widget used for error messages
 */
bool
UserInputModeAnnotationsContextMenu::processPolyhedronResetSliceRange(AnnotationPolyhedron* polyhedron,
                                                                      const int32_t browserWindowIndex,
                                                                      const Vector3D& mouseXY,
                                                                      const AString& extraMessageInfo,
                                                                      QWidget* parentWidgetForErrorMessage)
{
    CaretAssert(polyhedron);
    EventBrowserTabGetAtWindowXY tabEvent(browserWindowIndex,
                                          mouseXY);
    EventManager::get()->sendEvent(tabEvent.getPointer());
    
    std::vector<std::shared_ptr<DrawingViewportContent>> drawingSlices(tabEvent.getSamplesResetExtentViewportContents());
    
    if ((tabEvent.getBrowserTabContent() != NULL)
        && (drawingSlices.size() == 2)) {
        const AString msg("This operation will update the range of the selected sample "
                          "to match the selected volume slices. "
                          + extraMessageInfo
                          + " \nDo you want to continue?");
        if (WuQMessageBox::warningOkCancel(parentWidgetForErrorMessage,
                                           msg)) {
            CaretAssertVectorIndex(drawingSlices, 1);
            /*
             * The two slices are:
             * [0] First Slice Drawn in Montage that allows drawing
             * [1] Last Slice drawn in Montage that allows drawing
             */
            auto firstViewportContent(drawingSlices[0]);
            auto lastViewportContent(drawingSlices[1]);
            
            const DrawingViewportContentVolumeSlice& firstSlice(firstViewportContent->getVolumeSlice());
            const DrawingViewportContentVolumeSlice& lastSlice(lastViewportContent->getVolumeSlice());
            
            const Plane firstPlane(firstSlice.getPlane());
            const Plane lastPlane(lastSlice.getPlane());
            
            AnnotationManager* annotationManager(GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::SAMPLES_EDITING));
            
            AString errorMessage;
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            if (undoCommand->setModePolyhedronResetRangeToPlane(firstPlane,
                                                                lastPlane,
                                                                polyhedron,
                                                                errorMessage)) {
                if ( ! annotationManager->applyCommand(undoCommand,
                                                       errorMessage)) {
                    WuQMessageBox::errorOk(parentWidgetForErrorMessage,
                                           errorMessage);
                    return false;
                }
            }
            else {
                WuQMessageBox::errorOk(parentWidgetForErrorMessage,
                                       errorMessage);
                return false;
            }
        }
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    return true;
}
/**
 * Reset the slice range of a polyhedron
 */
void
UserInputModeAnnotationsContextMenu::resetPolyhedronSliceRangeSelected()
{
    if (m_polyhedronAnnotation != NULL) {
        const AString extraMessageInfo;
        processPolyhedronResetSliceRange(m_polyhedronAnnotation,
                                         m_userInputModeAnnotations->getBrowserWindowIndex(),
                                         m_mouseEvent.getXY(),
                                         extraMessageInfo,
                                         m_parentOpenGLWidget);
    }
}

/**
 * Polyhedron information selected
 */
void
UserInputModeAnnotationsContextMenu::polyhedronInformationSelected()
{
    if (m_polyhedronAnnotation != NULL) {
        const AString html(m_polyhedronAnnotation->getPolyhedronInformationHtml());
        WuQTextEditorDialog::runNonModal("Sample Information",
                                         html,
                                         WuQTextEditorDialog::TextMode::HTML,
                                         WuQTextEditorDialog::WrapMode::NO,
                                         m_parentOpenGLWidget);
    }
}

/**
 * Lock polyhedron
 */
void
UserInputModeAnnotationsContextMenu::lockPolyhedronSelected()
{
    if (m_polyhedronAnnotation != NULL) {
        Annotation::setSelectionLockedPolyhedronInWindow(m_userInputModeAnnotations->getBrowserWindowIndex(),
                                                         m_polyhedronAnnotation);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Unlock polyhedron
 */
void
UserInputModeAnnotationsContextMenu::unlockPolyhedronSelected()
{
    Annotation::setSelectionLockedPolyhedronInWindow(m_userInputModeAnnotations->getBrowserWindowIndex(),
                                                     NULL);
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Insert a coordinate into a polyline
 */
void
UserInputModeAnnotationsContextMenu::insertPolylineCoordinate()
{
    insertPolylineCoordinateAtMouse(m_userInputModeAnnotations,
                                    m_mouseEvent);    
}

/**
 * @return Selection item appropriate for input mode
 */
const SelectionItemAnnotation*
UserInputModeAnnotationsContextMenu::getSelectionItem(UserInputModeAnnotations* userInputModeAnnotations)
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    SelectionManager* selectionManager(brain->getSelectionManager());
    CaretAssert(selectionManager);
    SelectionItemAnnotation* selectionItem(selectionManager->getAnnotationIdentification());
    switch (userInputModeAnnotations->getUserInputMode()) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            break;
        case UserInputModeEnum::Enum::BORDERS:
            break;
        case UserInputModeEnum::Enum::FOCI:
            break;
        case UserInputModeEnum::Enum::IMAGE:
            break;
        case UserInputModeEnum::Enum::INVALID:
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            selectionItem = selectionManager->getSamplesIdentification();
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            break;
        case UserInputModeEnum::Enum::VIEW:
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            break;
    }
    return selectionItem;
}

/**
 * Insert a coordinate into a polyline at the given mouse event
 * @param userInputModeAnnotations
 *    Annotations input mode processor
 * @param mouseEvent
 *    The mouse event.
 */
void
UserInputModeAnnotationsContextMenu::insertPolylineCoordinateAtMouse(UserInputModeAnnotations* userInputModeAnnotations,
                                                                     const MouseEvent& mouseEvent)
{
    const SelectionItemAnnotation* annSel(getSelectionItem(userInputModeAnnotations));
    if (annSel->isValid()) {
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(userInputModeAnnotations->getUserInputMode());
        std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(mouseEvent.getBrowserWindowIndex());
        AnnotationMultiCoordinateShape* multiCoordAnn(NULL);
        AnnotationMultiPairedCoordinateShape* multiPairedCoordAnn(NULL);
        if ( ! selectedAnnotations.empty()) {
            CaretAssertVectorIndex(selectedAnnotations, 0);
            multiCoordAnn = selectedAnnotations[0]->castToMultiCoordinateShape();
            multiPairedCoordAnn = selectedAnnotations[0]->castToMultiPairedCoordinateShape();
        }
        if (multiCoordAnn != NULL) {
            const int32_t numAnnCoords = multiCoordAnn->getNumberOfCoordinates();
            if (numAnnCoords <= 2) {
                return;
            }
            
            /*
             * Verify selected coordinate index is valid for shape type
             */
            if (multiCoordAnn->castToPolygon() != NULL) {
                if (annSel->getPolyLineCoordinateIndex() >= numAnnCoords) {
                    return;
                }
            }
            else if (multiCoordAnn->castToPolyline() != NULL) {
                if (annSel->getPolyLineCoordinateIndex() >= (numAnnCoords - 1)) {
                    return;
                }
            }
            else {
                CaretAssertMessage(0, "Multi coord shape is neither polyline nor polygon.  Has new shape been added?");
                return;
            }
            int32_t surfaceSpaceVertexIndex(-1);
            if (multiCoordAnn->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE) {
                AnnotationCoordinateInformation coordInfo;
                AnnotationCoordinateInformation::createCoordinateInformationFromXY(mouseEvent,
                                                                                   coordInfo);
                if (coordInfo.m_surfaceSpaceInfo.m_validFlag) {
                    const AnnotationCoordinate* firstCoord(NULL);
                    if (multiCoordAnn->getNumberOfCoordinates() > 0) {
                        /*
                         * Get structure and number of nodes from first coordinate
                         * for surface space annotation
                         */
                        firstCoord = multiCoordAnn->getCoordinate(0);
                        StructureEnum::Enum structure = StructureEnum::INVALID;
                        int32_t numberOfVertices(-1);
                        int32_t vertexIndex(-1);
                        firstCoord->getSurfaceSpace(structure, numberOfVertices, vertexIndex);
                        if ((coordInfo.m_surfaceSpaceInfo.m_structure == structure)
                            && (coordInfo.m_surfaceSpaceInfo.m_numberOfNodes == numberOfVertices)) {
                            surfaceSpaceVertexIndex = coordInfo.m_surfaceSpaceInfo.m_nodeIndex;
                        }
                    }
                }
                
                if (surfaceSpaceVertexIndex < 0) {
                    WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                           "No surface vertex found at mouse location");
                    return;
                }
            }
            
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            undoCommand->setModeMultiCoordAnnInsertCoordinate(annSel->getPolyLineCoordinateIndex(),
                                                              annSel->getNormalizedRangeFromCoordIndexToNextCoordIndex(),
                                                              surfaceSpaceVertexIndex,
                                                              selectedAnnotations[0]);
            AString errorMessage;
            if ( ! annotationManager->applyCommand(undoCommand,
                                                   errorMessage)) {
                WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                       errorMessage);
            }
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        }

        if (multiPairedCoordAnn != NULL) {
            const int32_t numAnnCoords = multiPairedCoordAnn->getNumberOfCoordinates();
            if (numAnnCoords <= 2) {
                return;
            }
            
            /*
             * Verify selected coordinate index is valid for shape type
             */
            if (multiPairedCoordAnn->castToPolyhedron() != NULL) {
                if (annSel->getPolyLineCoordinateIndex() >= numAnnCoords) {
                    return;
                }
            }
            else {
                CaretAssertMessage(0, "Multi paired coord shape is neither polyline nor polygon.  Has new shape been added?");
                return;
            }
            int32_t surfaceSpaceVertexIndex(-1);
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            undoCommand->setModeMultiCoordAnnInsertCoordinate(annSel->getPolyLineCoordinateIndex(),
                                                              annSel->getNormalizedRangeFromCoordIndexToNextCoordIndex(),
                                                              surfaceSpaceVertexIndex,
                                                              selectedAnnotations[0]);
            AString errorMessage;
            if ( ! annotationManager->applyCommand(undoCommand,
                                                   errorMessage)) {
                WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                       errorMessage);
            }
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        }
    }
}

