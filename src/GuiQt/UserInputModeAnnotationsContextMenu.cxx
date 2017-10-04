
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

#include "AnnotationCoordinate.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "AnnotationTextEditorDialog.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventBrowserTabGetAll.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "UserInputModeAnnotations.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"

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
 *    Content of browser tab.
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
    
    const int32_t browserWindexIndex = m_mouseEvent.getBrowserWindowIndex();
    std::vector<std::pair<Annotation*, AnnotationFile*> > selectedAnnotations;
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->getAnnotationsSelectedForEditingIncludingLabels(browserWindexIndex,
                                                                       selectedAnnotations);
    
    m_annotationFile = NULL;
    m_annotation     = NULL;
    
    bool allSelectedAnnotationsDeletableFlag = true;
    if (selectedAnnotations.empty()) {
        allSelectedAnnotationsDeletableFlag = false;
    }
    
    m_threeDimCoordAnnotations.clear();
    for (std::vector<std::pair<Annotation*, AnnotationFile*> >::iterator iter = selectedAnnotations.begin();
         iter != selectedAnnotations.end();
         iter++) {
        Annotation* ann = iter->first;
        CaretAssert(ann);
        
        bool threeDimCoordFlag = false;
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                threeDimCoordFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
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
    }
    const bool haveThreeDimCoordAnnotationsFlag = ( ! m_threeDimCoordAnnotations.empty());

    bool oneAnnotationSelectedFlag = false;
    bool oneDeletableAnnotationSelectedFlag = false;
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        m_annotationFile = selectedAnnotations[0].second;
        m_annotation     = selectedAnnotations[0].first;
        oneAnnotationSelectedFlag = true;
        if (m_annotation->testProperty(Annotation::Property::DELETION)) {
            oneDeletableAnnotationSelectedFlag = true;
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
    cutAction->setEnabled(oneDeletableAnnotationSelectedFlag);
    
    /*
     * Copy
     */
    QAction* copyAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::COPY),
                                    this, SLOT(copyAnnotationToAnnotationClipboard()));
    copyAction->setEnabled(oneDeletableAnnotationSelectedFlag);

    /*
     * Delete
     */
    QAction* deleteAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::DELETER),
                                      this, SLOT(deleteAnnotations()));
    deleteAction->setEnabled(allSelectedAnnotationsDeletableFlag);
    
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
    QAction* pasteAction = addAction(pasteText,
                                     this, SLOT(pasteAnnotationFromAnnotationClipboard()));
    pasteAction->setEnabled(annotationManager->isAnnotationOnClipboardValid());

    /*
     * Paste Special
     */
    QAction* pasteSpecialAction = addAction(pasteSpecialText,
                                           this, SLOT(pasteSpecialAnnotationFromAnnotationClipboard()));
    pasteSpecialAction->setEnabled(annotationManager->isAnnotationOnClipboardValid());

    /*
     * Separator
     */
    addSeparator();

    /*
     * Select All annotations
     */
    addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::SELECT_ALL),
              this, SLOT(selectAllAnnotations()));
    
    /*
     * Separator
     */
    addSeparator();
    
    /*
     * Edit Text
     */
    QAction* editTextAction = addAction("Edit Text...",
                                        this, SLOT(setAnnotationText()));
    editTextAction->setEnabled(m_textAnnotation != NULL);
    
    
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
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->copyAnnotationToClipboard(m_annotationFile,
                                                 m_annotation);
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
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
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
    
    AnnotationTextEditorDialog ted(m_textAnnotation,
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
 * Turn off display of annotation in other tabs.
 */
void
UserInputModeAnnotationsContextMenu::turnOffDisplayInOtherTabs()
{
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
        CaretAssert(0);  // TAB NOT ALLOWED
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
    if (m_annotation == NULL) {
        return NULL;
    }
    
    EventBrowserTabGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    const std::vector<BrowserTabContent*> allTabs = tabIndicesEvent.getAllBrowserTabs();
    
    bool menuValidFlag = false;
    if (m_annotation->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
        if (allTabs.size() > 1) {
            menuValidFlag = true;
        }
    }
    
    QMenu* menu = new QMenu("Duplicate to Tab");
    QObject::connect(menu, SIGNAL(triggered(QAction*)),
                     this, SLOT(duplicateAnnotationSelected(QAction*)));
    
    if (menuValidFlag) {
        for (BrowserTabContent* tabContent : allTabs) {
            if (tabContent->getTabNumber() != m_annotation->getTabIndex()) {
                QAction* action = menu->addAction(tabContent->getName());
                action->setData((int)tabContent->getTabNumber());
            }
        }
    }
    else {
        menu->setEnabled(false);
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
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    
    const int32_t tabIndex = action->data().toInt();
    
    Annotation* annCopy = m_annotation->clone();
    annCopy->setTabIndex(tabIndex);
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->updateForNewAnnotation(annCopy);

    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeDuplicateAnnotation(m_annotationFile,
                                            annCopy);
    AString errorMessage;
    if ( ! annotationManager->applyCommand(undoCommand,
                                           errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    annotationManager->selectAnnotationForEditing(m_mouseEvent.getBrowserWindowIndex(),
                                                  AnnotationManager::SELECTION_MODE_SINGLE,
                                                  false,
                                                  annCopy);
    
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    
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


