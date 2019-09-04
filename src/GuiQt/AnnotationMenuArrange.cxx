
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

#include <QPainter>

#include "Annotation.h"
#include "AnnotationArrangerInputs.h"
#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesAnnotationTextSubstitution.h"
#include "EventAnnotationGrouping.h"
#include "EventBrowserWindowContent.h"
#include "EventGetBrainOpenGLTextRenderer.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuArrange 
 * \brief Menu for arranging annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *     The user input mode
 * @param parent
 *     The parent widget.
 * @param browserWindowIndex
 *     Index of the browser window.
 */
AnnotationMenuArrange::AnnotationMenuArrange(const UserInputModeEnum::Enum userInputMode,
                                             const int32_t browserWindowIndex,
                                             QWidget* parent)
: QMenu(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    switch (m_userInputMode) {
        case UserInputModeEnum::ANNOTATIONS:
            m_menuMode = MenuMode::ANNOTATIONS;
            break;
        case UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING:
            m_menuMode = MenuMode::TILE_TABS;
            break;
        case UserInputModeEnum::BORDERS:
        case UserInputModeEnum::FOCI:
        case UserInputModeEnum::IMAGE:
        case UserInputModeEnum::INVALID:
        case UserInputModeEnum::VIEW:
        case UserInputModeEnum::VOLUME_EDIT:
            CaretAssert(0);
            return;
            break;
    }

    addAlignmentSelections();
    
    addDistributeSelections();

    addOrderingSelections();
    
    addGroupingSelections();
    
    addTileTabsSelections();

    QObject::connect(this, SIGNAL(aboutToShow()),
                     this, SLOT(menuAboutToShow()));
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
    if ( ! actions().isEmpty()) {
        addSeparator();
    }
    
    std::vector<AnnotationAlignmentEnum::Enum> alignments;
    AnnotationAlignmentEnum::getAllEnums(alignments);
    
    for (std::vector<AnnotationAlignmentEnum::Enum>::iterator iter = alignments.begin();
         iter != alignments.end();
         iter++) {
        const AnnotationAlignmentEnum::Enum annAlign = *iter;
        const QString enumText = AnnotationAlignmentEnum::toGuiName(annAlign);
        const QString enumName = AnnotationAlignmentEnum::toName(annAlign);
        
        QPixmap pixmap = createAlignmentPixmap(this, annAlign);

        QAction* action = addAction(enumText);
        action->setIcon(pixmap);
        action->setData(enumName);
    }
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addDistributeSelections()
{
    if ( ! actions().isEmpty()) {
        addSeparator();
    }
    
    std::vector<AnnotationDistributeEnum::Enum> distributes;
    AnnotationDistributeEnum::getAllEnums(distributes);
    
    for (std::vector<AnnotationDistributeEnum::Enum>::iterator iter = distributes.begin();
         iter != distributes.end();
         iter++) {
        const AnnotationDistributeEnum::Enum annDist = *iter;
        const QString enumText = AnnotationDistributeEnum::toGuiName(annDist);
        const QString enumName = AnnotationDistributeEnum::toName(annDist);
        
        QPixmap pixmap = createDistributePixmap(this, annDist);
        
        QAction* action = addAction(enumText);
        action->setIcon(pixmap);
        action->setData(enumName);
    }
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addGroupingSelections()
{
    if ( ! actions().isEmpty()) {
        addSeparator();
    }
    
    m_groupAction = NULL;
    m_regroupAction  = NULL;
    m_ungroupAction  = NULL;
    
    std::vector<AnnotationGroupingModeEnum::Enum> groupings;
    AnnotationGroupingModeEnum::getAllEnums(groupings);
    
    for (std::vector<AnnotationGroupingModeEnum::Enum>::iterator iter = groupings.begin();
         iter != groupings.end();
         iter++) {
        const AnnotationGroupingModeEnum::Enum groupingMode = *iter;
        const QString enumText = AnnotationGroupingModeEnum::toGuiName(groupingMode);
        const QString enumName = AnnotationGroupingModeEnum::toName(groupingMode);
        
        QPixmap pixmap = createGroupingPixmap(this, groupingMode);
        
        QAction* action = addAction(enumText);
        action->setIcon(pixmap);
        action->setData(enumName);
        
        switch (groupingMode) {
            case  AnnotationGroupingModeEnum::GROUP:
                m_groupAction = action;
                break;
            case AnnotationGroupingModeEnum::REGROUP:
                m_regroupAction = action;
                break;
            case AnnotationGroupingModeEnum::UNGROUP:
                m_ungroupAction = action;
                break;
        }
    }

    CaretAssert(m_groupAction);
    CaretAssert(m_regroupAction);
    CaretAssert(m_ungroupAction);
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addTileTabsSelections()
{
    switch (m_menuMode) {
        case MenuMode::ANNOTATIONS:
            break;
        case MenuMode::TILE_TABS:
        {
            if ( ! actions().isEmpty()) {
                addSeparator();
            }
            
            m_tileTabsExpandToFillAction = addAction("Expand Tab to Fill Empty Space");
        }
            break;
    }
}

/**
 * Add ordering for tile tabs mode
 */
void
AnnotationMenuArrange::addOrderingSelections()
{
    switch (m_menuMode) {
        case MenuMode::ANNOTATIONS:
            break;
        case MenuMode::TILE_TABS:
        {
            if ( ! actions().isEmpty()) {
                addSeparator();
            }
            
            m_orderingBringToFrontAction = addAction("Bring to Front");
            
            m_orderingBringForwardAction = addAction("Bring Forward");
            
            m_orderingSendToBackAction = addAction("Send to Back");
            
            m_orderingSendBackwardAction = addAction("Send Backward");
        }
            break;
    }
}


/*
 * Gets called when the menu is about to show
 * so that its menu items can be enabled/disabled.
 */
void
AnnotationMenuArrange::menuAboutToShow()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    
    m_groupAction->setEnabled(annMan->isGroupingModeValid(m_browserWindowIndex,
                                                          AnnotationGroupingModeEnum::GROUP));
    m_regroupAction->setEnabled(annMan->isGroupingModeValid(m_browserWindowIndex,
                                                            AnnotationGroupingModeEnum::REGROUP));
    m_ungroupAction->setEnabled(annMan->isGroupingModeValid(m_browserWindowIndex,
                                                            AnnotationGroupingModeEnum::UNGROUP));
    
    switch (m_menuMode) {
        case MenuMode::ANNOTATIONS:
            break;
        case MenuMode::TILE_TABS:
        {
            std::unique_ptr<EventBrowserWindowContent> windowEvent = EventBrowserWindowContent::getWindowContent(m_browserWindowIndex);
            EventManager::get()->sendEvent(windowEvent->getPointer());
            BrowserWindowContent* bwc = windowEvent->getBrowserWindowContent();
            CaretAssert(bwc);
            int32_t numSelected = 0;
            if (bwc->isManualModeTileTabsConfigurationEnabled()) {
                numSelected = annMan->getAnnotationsSelectedForEditing(m_browserWindowIndex).size();
            }
            const bool oneSelectedFlag(numSelected == 1);
            
            CaretAssert(m_tileTabsExpandToFillAction);
            m_tileTabsExpandToFillAction->setEnabled(oneSelectedFlag);
            
            CaretAssert(m_orderingSendToBackAction);
            m_orderingSendToBackAction->setEnabled(oneSelectedFlag);
            
            CaretAssert(m_orderingSendBackwardAction);
            m_orderingSendBackwardAction->setEnabled(oneSelectedFlag);
            
            CaretAssert(m_orderingBringForwardAction);
            m_orderingBringForwardAction->setEnabled(oneSelectedFlag);
            
            CaretAssert(m_orderingBringToFrontAction);
            m_orderingBringToFrontAction->setEnabled(oneSelectedFlag);
        }
            break;
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
    bool validGroupingFlag = false;
    const AnnotationGroupingModeEnum::Enum annGroup = AnnotationGroupingModeEnum::fromName(enumName,
                                                                                           &validGroupingFlag);

    if (validAlignmentFlag) {
        applyAlignment(annAlign);
    }
    else if (validDistributeFlag) {
        applyDistribute(annDist);
    }
    else if (validGroupingFlag) {
        applyGrouping(annGroup);
    }
    else if (processTileTabsMenu(action)) {
        /* If true returned, a tile tabs menu item was selected */
    }
    else if (processOrderingMenuItem(action)) {
        /* If true returned, an ordering menu item was selected */
    }
    else {
        const AString msg("Unrecognized Enum name in Annotation Align Menu \""
                          + enumName
                          + "\"");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }

    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Process the action if it is on the Tile Tabs Menu
 *
 * @param actionSelected
 *     Action selected by user
 * @return
 *     True if the action was on this menu, else false.
 */
bool
AnnotationMenuArrange::processTileTabsMenu(QAction* actionSelected)
{
    bool menuSelectedFlag(false);
    
    if ((m_tileTabsExpandToFillAction != NULL)
        && (actionSelected == m_tileTabsExpandToFillAction)) {
        processExpandTabMenuItem();
        menuSelectedFlag = true;
    }
    
    return menuSelectedFlag;
}

/**
 * Process the action if it is on the Tile Tabs Menu
 *
 * @param actionSelected
 *     Action selected by user
 * @return
 *     True if the action was on this menu, else false.
 */
bool
AnnotationMenuArrange::processOrderingMenuItem(QAction* actionSelected)
{
    bool menuSelectedFlag(false);
    
    switch (m_menuMode) {
        case MenuMode::ANNOTATIONS:
            break;
        case MenuMode::TILE_TABS:
        {
            CaretAssert(m_orderingBringToFrontAction);
            CaretAssert(m_orderingBringForwardAction);
            CaretAssert(m_orderingSendBackwardAction);
            CaretAssert(m_orderingSendToBackAction);
            
            if (actionSelected == m_orderingBringToFrontAction) {
                processWindowTileTabOperation(EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_TO_FRONT);
                menuSelectedFlag = true;
            }
            else if (actionSelected == m_orderingBringForwardAction) {
                processWindowTileTabOperation(EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_FORWARD);
                menuSelectedFlag = true;
            }
            else if (actionSelected == m_orderingSendBackwardAction) {
                processWindowTileTabOperation(EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_BACKWARD);
                menuSelectedFlag = true;
            }
            else if (actionSelected == m_orderingSendToBackAction) {
                processWindowTileTabOperation(EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_TO_BACK);
                menuSelectedFlag = true;
            }
        }
            break;
    }
    
    return menuSelectedFlag;
}

/**
 * Called to process a tile tab operation
 *
 * @param operation
 *     The operation
 */
void
AnnotationMenuArrange::processWindowTileTabOperation(const EventBrowserWindowTileTabOperation::Operation operation)
{
    std::vector<BrowserTabContent*> emptyBrowserTabs;

    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    CaretAssert(bbw);
    BrowserWindowContent* bwc = bbw->getBrowerWindowContent();
    CaretAssert(bwc);
    
    int32_t tabIndex(-1);
    if (bwc->isManualModeTileTabsConfigurationEnabled()) {
        std::vector<Annotation*> selectedAnnotations = annMan->getAnnotationsSelectedForEditing(m_browserWindowIndex);
        if (selectedAnnotations.size() == 1) {
            CaretAssertVectorIndex(selectedAnnotations, 0);
            Annotation* ann = selectedAnnotations[0];
            CaretAssert(ann);
            AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(ann);
            if (abt != NULL) {
                tabIndex = abt->getTabIndex();
            }
            else {
                CaretLogSevere("Program Error: Selected annotation is not a browser tab");
            }
        }
        else {
            CaretLogSevere("Program Error: Only one tab should be selected for an ordering operation");
            return;
        }
    }

    int windowViewport[4] { 0, 0, 10, 10 };
    const int32_t mouseX(50);
    const int32_t mouseY(50);
    EventBrowserWindowTileTabOperation tileTabOperation(operation,
                                                        bbw,
                                                        m_browserWindowIndex,
                                                        tabIndex,
                                                        windowViewport,
                                                        mouseX,
                                                        mouseY,
                                                        emptyBrowserTabs);
    EventManager::get()->sendEvent(tileTabOperation.getPointer());
}

void
AnnotationMenuArrange::processExpandTabMenuItem()
{
    BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    CaretAssert(window);
    std::vector<BrowserTabContent*> allTabContent;
    window->getAllTabContent(allTabContent);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    const bool result = annMan->expandSelectedBrowserTabAnnotation(allTabContent,
                                                                   m_browserWindowIndex,
                                                                   m_userInputMode,
                                                                   errorMessage);
    if ( ! result) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
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
    EventGetBrainOpenGLTextRenderer textRendererEvent;
    EventManager::get()->sendEvent(textRendererEvent.getPointer());
    BrainOpenGLTextRenderInterface* textRenderer = textRendererEvent.getTextRenderer();
    if (textRenderer == NULL) {
        WuQMessageBox::errorOk(this, "Failed to get text renderer for window "
                               + QString::number(m_browserWindowIndex));
        return;
    }
    
    DisplayPropertiesAnnotationTextSubstitution* dpats = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotationTextSubstitution();
    BrainOpenGLTextRenderInterface::DrawingFlags drawingFlags;
    drawingFlags.setDrawSubstitutedText(dpats->isEnableSubstitutions());
    AnnotationArrangerInputs alignMod(textRenderer,
                                      drawingFlags,
                                     m_browserWindowIndex);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->alignAnnotations(m_userInputMode,
                                    alignMod,
                                    alignment,
                                    errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }

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
    EventGetBrainOpenGLTextRenderer textRendererEvent;
    EventManager::get()->sendEvent(textRendererEvent.getPointer());
    BrainOpenGLTextRenderInterface* textRenderer = textRendererEvent.getTextRenderer();
    if (textRenderer == NULL) {
        WuQMessageBox::errorOk(this, "Failed to get text renderer for window "
                               + QString::number(m_browserWindowIndex));
        return;
    }
    
    DisplayPropertiesAnnotationTextSubstitution* dpats = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotationTextSubstitution();
    BrainOpenGLTextRenderInterface::DrawingFlags drawingFlags;
    drawingFlags.setDrawSubstitutedText(dpats->isEnableSubstitutions());
    AnnotationArrangerInputs distributeMod(textRenderer,
                                           drawingFlags,
                                           m_browserWindowIndex);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->distributeAnnotations(m_userInputMode,
                                         distributeMod,
                                         distribute,
                                         errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Apply grouping selection.
 *
 * @param grouping
 *     Selected grouping.
 */
void
AnnotationMenuArrange::applyGrouping(const AnnotationGroupingModeEnum::Enum grouping)
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();

    AString errorMessage;
    if ( ! annMan->applyGroupingMode(m_userInputMode,
                                     m_browserWindowIndex,
                                     grouping,
                                     errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Create an alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param alignment
 *    The alignment.
 * @return
 *    Pixmap with icon for the given alignment.
 */
QPixmap
AnnotationMenuArrange::createAlignmentPixmap(const QWidget* widget,
                                             const AnnotationAlignmentEnum::Enum alignment)
{
    CaretAssert(widget);
    
    const float pixmapSize = 24.0;
    const float halfPixmapSize  = pixmapSize / 2.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    const qreal minInset = 2;
    const qreal minInset2 = minInset * 2;
    const qreal minInset3 = minInset * 3;
    const qreal maxInset = pixmapSize - minInset;
    
    const QLineF horizontalLine(minInset, 0.0, maxInset, 0.0);
    const QLineF verticalLine(0.0, minInset, 0.0, maxInset);
    
    const qreal rectangleThickness = pixmapSize * 0.25;
    const qreal longRectangleLength = pixmapSize - (minInset * 4);
    const qreal shortRectangleLength = longRectangleLength * 0.60;
    const QRectF longVerticalRectangle(QPointF(0.0, 0.0),
                                      QPointF(rectangleThickness, longRectangleLength));
    const QRectF shortVerticalRectangle(QPointF(0.0, 0.0),
                                       QPointF(rectangleThickness, shortRectangleLength));
    const qreal shortVerticalRectangleOffset = pixmapSize - minInset2 - rectangleThickness;

    const QRectF longHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(longRectangleLength, rectangleThickness));
    const QRectF shortHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(shortRectangleLength, rectangleThickness));
    
    const qreal longRectangleSpace = pixmapSize - longRectangleLength;
    const qreal shortRectangleSpace = pixmapSize - shortRectangleLength;
    
    QBrush foregroundBrush = widget->palette().brush(widget->foregroundRole());
    QPen foregroundPen = painter->pen();
    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
    
    switch (alignment) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            drawLine(painter, horizontalLine, 0.0, minInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, minInset3);
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, minInset3);
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            drawLine(painter, verticalLine, halfPixmapSize, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, (longRectangleSpace / 2.0), minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace / 2.0), shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            drawLine(painter, verticalLine, minInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, minInset3, minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, minInset3, shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            drawLine(painter, horizontalLine, 0.0, halfPixmapSize);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, (longRectangleSpace / 2.0));
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace / 2.0));
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            drawLine(painter, verticalLine, maxInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, (longRectangleSpace - minInset3), minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace - minInset3), shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            drawLine(painter, horizontalLine, 0.0, maxInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, (longRectangleSpace - minInset3));
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace - minInset3));
            break;
    }
    
    return pixmap;
}

/**
 * Create a distribute pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param distribute
 *    The distribute type.
 * @return
 *    Pixmap with icon for the given distribute.
 */
QPixmap
AnnotationMenuArrange::createDistributePixmap(const QWidget* widget,
                                             const AnnotationDistributeEnum::Enum distribute)
{
    CaretAssert(widget);
    
    const float pixmapSize = 24.0;
    const float halfPixmapSize  = pixmapSize / 2.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    const qreal minInset = 2;
    const qreal minInset2 = minInset * 2;
    const qreal minInset3 = minInset * 3;
    const qreal maxInset = pixmapSize - minInset;
    
    const QLineF horizontalLine(minInset, 0.0, maxInset, 0.0);
    const QLineF verticalLine(0.0, minInset, 0.0, maxInset);
    
    const qreal rectangleThickness = pixmapSize * 0.15;
    const qreal longRectangleLength = pixmapSize - (minInset3 * 2);
    const qreal shortRectangleLength = longRectangleLength * 0.50;
    const qreal midSizeRectangleLength = longRectangleLength * 0.75;
    const QRectF longVerticalRectangle(QPointF(0.0, 0.0),
                                       QPointF(rectangleThickness, longRectangleLength));
    const QRectF midSizeVerticalRectangle(QPointF(0.0, 0.0),
                                        QPointF(rectangleThickness, midSizeRectangleLength));
    const QRectF shortVerticalRectangle(QPointF(0.0, 0.0),
                                        QPointF(rectangleThickness, shortRectangleLength));
    const qreal shortVerticalRectangleOffset = halfPixmapSize - (rectangleThickness / 2.0);
    const qreal midSizeVerticalRectangleOffset = pixmapSize - minInset2 - rectangleThickness;
    
    const QRectF longHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(longRectangleLength, rectangleThickness));
    const QRectF midSizeHorizontalRectangle(QPointF(0.0, 0.0),
                                          QPointF(midSizeRectangleLength, rectangleThickness));
    const QRectF shortHorizontalRectangle(QPointF(0.0, 0.0),
                                          QPointF(shortRectangleLength, rectangleThickness));
    
    const qreal shortRectangleSpace = pixmapSize - shortRectangleLength;
    const qreal midSizeRectangleSpace = pixmapSize - midSizeRectangleLength;
    
    QBrush foregroundBrush = widget->palette().brush(widget->foregroundRole());
    QPen foregroundPen = painter->pen();
    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
    
    switch (distribute) {
        case AnnotationDistributeEnum::HORIZONTALLY:
            drawLine(painter, horizontalLine, 0.0, maxInset);
            drawLine(painter, horizontalLine, 0.0, minInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, minInset3);
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace / 2.0));
            drawRect(painter, foregroundColor, midSizeVerticalRectangle, midSizeVerticalRectangleOffset, (midSizeRectangleSpace / 2.0));
            break;
        case AnnotationDistributeEnum::VERTICALLY:
            drawLine(painter, verticalLine, minInset, 0.0);
            drawLine(painter, verticalLine, maxInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, minInset3, minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace / 2.0), shortVerticalRectangleOffset);
            drawRect(painter, foregroundColor, midSizeHorizontalRectangle, (midSizeRectangleSpace / 2.0), midSizeVerticalRectangleOffset);
            break;
    }
    
    return pixmap;
}

/**
 * Create a grouping pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param grouping
 *    The grouping type.
 * @return
 *    Pixmap with icon for the given grouping.
 */
QPixmap
AnnotationMenuArrange::createGroupingPixmap(const QWidget* widget,
                                            const AnnotationGroupingModeEnum::Enum grouping)
{
    CaretAssert(widget);
    
    const float pixmapSize = 24.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter100x100(widget,
                                                                                                   pixmap);
    
    const float xyOne = 75.0;
    const float xyTwo = 25.0;
    const float boxSize = 100.0;
    
    painter->drawRect(-xyOne, -xyTwo, boxSize, boxSize);
    painter->drawRect(-xyTwo, -xyOne, boxSize, boxSize);
    
    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
    
    painter->setBrush(foregroundColor);
    const float dotSize = 20.0;
    switch (grouping) {
        case AnnotationGroupingModeEnum::GROUP:
            painter->drawEllipse(QPointF(-xyOne, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF( xyOne, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF( xyOne,  xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne,  xyOne), dotSize, dotSize);
            break;
        case AnnotationGroupingModeEnum::REGROUP:
            painter->drawEllipse(QPointF(-xyOne, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF( xyOne, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF( xyOne,  xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne,  xyOne), dotSize, dotSize);
            if (grouping == AnnotationGroupingModeEnum::REGROUP) {
                QVector<QPoint> points;
                points.push_back(QPoint(-100, -10));
                points.push_back(QPoint( 10, -10));
                points.push_back(QPoint( 10, -50));
                points.push_back(QPoint( 80,   0));
                points.push_back(QPoint( 10,  50));
                points.push_back(QPoint( 10,  10));
                points.push_back(QPoint(-100,  10));
                points.push_back(QPoint(-100, -10));
                
                painter->drawPolygon(QPolygon(points));
            }
            break;
        case AnnotationGroupingModeEnum::UNGROUP:
            painter->drawEllipse(QPointF(-xyTwo, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyTwo + boxSize, -xyOne), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyTwo + boxSize, -xyOne + boxSize), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyTwo, -xyOne + boxSize), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne, -xyTwo), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne + boxSize, -xyTwo), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne + boxSize, -xyTwo + boxSize), dotSize, dotSize);
            painter->drawEllipse(QPointF(-xyOne, -xyTwo + boxSize), dotSize, dotSize);
            break;
    }
    
    return pixmap;
}


/**
 * Draw a line at the given X,Y using the painter.
 *
 * @param painter
 *     The painter.
 * @param line
 *     The line
 * @param x
 *     Translate to X.
 * @param y
 *     Translate to Y.
 */
void
AnnotationMenuArrange::drawLine(QSharedPointer<QPainter>& painter,
                                const QLineF& line,
                                const qreal x,
                                const qreal y)
{
    painter->save();
    painter->translate(x, y);
    painter->drawLine(line);
    painter->restore();
}

/**
 * Draw a filled rectangle at the given X,Y using the painter and in the given color.
 *
 * @param painter
 *     The painter.
 * @param color
 *     Color of filled rectangle.
 * @param rectangle
 *     The rectangle
 * @param x
 *     Translate to X.
 * @param y
 *     Translate to Y.
 */
void
AnnotationMenuArrange::drawRect(QSharedPointer<QPainter>& painter,
                                const QColor& color,
                                const QRectF& rectangle,
                                const qreal x,
                                const qreal y)
{
    painter->save();
    painter->translate(x, y);
    painter->fillRect(rectangle,
                      color);
    painter->restore();
}

