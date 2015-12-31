
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

#define __ANNOTATION_MENU_ARRANGE_DECLARE__
#include "AnnotationMenuArrange.h"
#undef __ANNOTATION_MENU_ARRANGE_DECLARE__

#include "Annotation.h"
#include "AnnotationArrangerInputs.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventGetBrainOpenGLTextRenderer.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuArrange 
 * \brief Menu for arranging annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget.
 * @param browserWindowIndex
 *     Index of the browser window.
 */
AnnotationMenuArrange::AnnotationMenuArrange(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QMenu(parent),
m_browserWindowIndex(browserWindowIndex)
{
    addAlignmentSelections();
    
    addSeparator();
    
    addDistributeSelections();

    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionTriggered(QAction*)));
}

/**
 * Destructor.
 */
AnnotationMenuArrange::~AnnotationMenuArrange()
{
}

/**
 * Add alignment options to the menu.
 */
void
AnnotationMenuArrange::addAlignmentSelections()
{
    std::vector<AnnotationAlignmentEnum::Enum> alignments;
    AnnotationAlignmentEnum::getAllEnums(alignments);
    
    for (std::vector<AnnotationAlignmentEnum::Enum>::iterator iter = alignments.begin();
         iter != alignments.end();
         iter++) {
        const AnnotationAlignmentEnum::Enum annAlign = *iter;
        const QString enumText = AnnotationAlignmentEnum::toGuiName(annAlign);
        const QString enumName = AnnotationAlignmentEnum::toName(annAlign);
        
        QAction* action = addAction(enumText);
        action->setData(enumName);
    }
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addDistributeSelections()
{
    std::vector<AnnotationDistributeEnum::Enum> distributes;
    AnnotationDistributeEnum::getAllEnums(distributes);
    
    for (std::vector<AnnotationDistributeEnum::Enum>::iterator iter = distributes.begin();
         iter != distributes.end();
         iter++) {
        const AnnotationDistributeEnum::Enum annDist = *iter;
        const QString enumText = AnnotationDistributeEnum::toGuiName(annDist);
        const QString enumName = AnnotationDistributeEnum::toName(annDist);
        
        QAction* action = addAction(enumText);
        action->setData(enumName);
        
        action->setEnabled(false);
    }
}

/**
 * Gets called when the user selects a menu item.
 */
void
AnnotationMenuArrange::menuActionTriggered(QAction* action)
{
    CaretAssert(action);
    
    const QString enumName = action->data().toString();
    
    bool validAlignmentFlag = false;
    const AnnotationAlignmentEnum::Enum annAlign = AnnotationAlignmentEnum::fromName(enumName,
                                                                                     &validAlignmentFlag);

    bool validDistributeFlag = false;
    const AnnotationDistributeEnum::Enum annDist = AnnotationDistributeEnum::fromName(enumName,
                                                                                       &validDistributeFlag);
    if (validAlignmentFlag) {
        applyAlignment(annAlign);
    }
    else if (validDistributeFlag) {
        std::cout << "Distribute not implemented " << AnnotationDistributeEnum::toGuiName(annDist) << std::endl;
    }
    else {
        const AString msg("Unrecognized Enum name in Annotation Align Menu \""
                          + enumName
                          + "\"");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Apply alignment selection.
 *
 * @param alignment
 *     Selected alignment.
 */
void
AnnotationMenuArrange::applyAlignment(const AnnotationAlignmentEnum::Enum alignment)
{
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    CaretAssert(bbw);
    
    EventGetBrainOpenGLTextRenderer textRendererEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(textRendererEvent.getPointer());
    BrainOpenGLTextRenderInterface* textRenderer = textRendererEvent.getTextRenderer();
    if (textRenderer == NULL) {
        WuQMessageBox::errorOk(this, "Failed to get text renderer for window "
                               + QString::number(m_browserWindowIndex));
        return;
    }
    
    AnnotationArrangerInputs alignMod(textRenderer,
                                     alignment,
                                     m_browserWindowIndex);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->alignAnnotations(alignMod, errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
//    std::vector<Annotation*> selectedAnnotations = annMan->getSelectedAnnotations(m_browserWindowIndex);
//    const int32_t numSelectedAnnotations = static_cast<int32_t>(selectedAnnotations.size());
//    
//    std::vector<Annotation*> annotationsBeforeMoveAndResize;
//    std::vector<Annotation*> annotationsAfterMoveAndResize;
//    
//    for (int32_t i = 0; i < numSelectedAnnotations; i++) {
//        Annotation* annotationModified(selectedAnnotations[i]->clone());
//        if (annotationModified->applyAlignmentModification(
//        if (annotationModified->applySpatialModification(annSpatialMod)) {
//            annotationsBeforeMoveAndResize.push_back(selectedAnnotations[i]);
//            annotationsAfterMoveAndResize.push_back(annotationModified);
//        }
//        else {
//            delete annotationModified;
//            annotationModified = NULL;
//        }
//    }
//    CaretAssert(annotationsAfterMoveAndResize.size() == annotationsBeforeMoveAndResize.size());
//    
//    if ( ! annotationsAfterMoveAndResize.empty()) {
//        AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
//        command->setModeLocationAndSize(annotationsBeforeMoveAndResize,
//                                        annotationsAfterMoveAndResize);
//        
//        if ( ! mouseEvent.isFirstDragging()) {
//            command->setMergeEnabled(true);
//        }
//        
//        annotationManager->applyCommand(command);
//    }
//    
//    for (std::vector<Annotation*>::iterator iter = annotationsAfterMoveAndResize.begin();
//         iter != annotationsAfterMoveAndResize.end();
//         iter++) {
//        delete *iter;
//    }
//    annotationsAfterMoveAndResize.clear();
//
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Apply distribute selection.
 *
 * @param distribute
 *     Selected distribute.
 */
void
AnnotationMenuArrange::applyDistribute(const AnnotationDistributeEnum::Enum distribute)
{
    
}


