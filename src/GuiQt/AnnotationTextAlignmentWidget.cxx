
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

#include <cmath>

#define __ANNOTATION_TEXT_ALIGNMENT_WIDGET_DECLARE__
#include "AnnotationTextAlignmentWidget.h"
#undef __ANNOTATION_TEXT_ALIGNMENT_WIDGET_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QVBoxLayout>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "WorkbenchAction.h"
#include "WorkbenchToolButton.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


/**
 * \class caret::AnnotationTextAlignmentWidget 
 * \brief Widget for adjusting annotation alignment.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *     The user input mode
 * @param browserWindowIndex
 *     Index of browser window.
 * @param parent
 *     The parent widget.
 */
AnnotationTextAlignmentWidget::AnnotationTextAlignmentWidget(const UserInputModeEnum::Enum userInputMode,
                                                             const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    m_smallLayoutFlag = true;
    
    QToolButton* leftAlignToolButton   = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::LEFT);
    QToolButton* centerAlignToolButton = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::CENTER);
    QToolButton* rightAlignToolButton  = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::RIGHT);
    
    m_horizontalAlignActionGroup = new QActionGroup(this);
    m_horizontalAlignActionGroup->setExclusive(false); /**  not exclusive as may need to turn all off */
    m_horizontalAlignActionGroup->addAction(leftAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(centerAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(rightAlignToolButton->defaultAction());
    QObject::connect(m_horizontalAlignActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(horizontalAlignmentActionSelected(QAction*)));
    
    
    QToolButton* topAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::TOP);
    QToolButton* middleAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::MIDDLE);
    QToolButton* bottomAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::BOTTOM);
    
    m_verticalAlignActionGroup = new QActionGroup(this);
    m_verticalAlignActionGroup->setExclusive(false); /** not exclusive as may need to turn all off */
    m_verticalAlignActionGroup->addAction(topAlignToolButton->defaultAction());
    m_verticalAlignActionGroup->addAction(middleAlignToolButton->defaultAction());
    m_verticalAlignActionGroup->addAction(bottomAlignToolButton->defaultAction());
    QObject::connect(m_verticalAlignActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(verticalAlignmentActionSelected(QAction*)));
    

    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    if (m_smallLayoutFlag) {
        QLabel* alignmentLabel = new QLabel("Alignment");
        int row = gridLayout->rowCount();
        gridLayout->addWidget(alignmentLabel, row, 0, 1, 3, Qt::AlignHCenter);
        row++;
        gridLayout->addWidget(leftAlignToolButton, row, 0);
        gridLayout->addWidget(centerAlignToolButton, row, 1);
        gridLayout->addWidget(rightAlignToolButton, row, 2);
        row++;
        gridLayout->addWidget(topAlignToolButton, row, 0);
        gridLayout->addWidget(middleAlignToolButton, row, 1);
        gridLayout->addWidget(bottomAlignToolButton, row, 2);
    }
    else {
        QLabel* horizontalLabel = new QLabel("Text Horizontal");
        QHBoxLayout* horizontalAlignLayout = new QHBoxLayout();
        WuQtUtilities::setLayoutSpacingAndMargins(horizontalAlignLayout, 2, 0);
        horizontalAlignLayout->addWidget(leftAlignToolButton);
        horizontalAlignLayout->addWidget(centerAlignToolButton);
        horizontalAlignLayout->addWidget(rightAlignToolButton);
        
        QLabel* verticalLabel = new QLabel("Text Vertical");
        QHBoxLayout* verticalAlignLayout = new QHBoxLayout();
        WuQtUtilities::setLayoutSpacingAndMargins(verticalAlignLayout, 2, 0);
        verticalAlignLayout->addWidget(topAlignToolButton);
        verticalAlignLayout->addWidget(middleAlignToolButton);
        verticalAlignLayout->addWidget(bottomAlignToolButton);
        
        gridLayout->addWidget(horizontalLabel,
                              0, 0,
                              Qt::AlignHCenter);
        gridLayout->addLayout(horizontalAlignLayout,
                              1, 0);
        gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                              0, 1,
                              2, 1);
        gridLayout->addWidget(verticalLabel,
                              0, 2,
                              Qt::AlignHCenter);
        gridLayout->addLayout(verticalAlignLayout,
                              1, 2);
    }
}

/**
 * Destructor.
 */
AnnotationTextAlignmentWidget::~AnnotationTextAlignmentWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotationTexts.
 */
void
AnnotationTextAlignmentWidget::updateContent(std::vector<AnnotationText*>& annotationTexts)
{
    m_annotations.clear();
    m_annotations.reserve(annotationTexts.size());
    for (auto a : annotationTexts) {
        if (a->testProperty(Annotation::Property::TEXT_ORIENTATION)) {
            m_annotations.push_back(a);
        }
    }
    
    {
        /*
         * Update horizontal alignment
         */
        m_horizontalAlignActionGroup->blockSignals(true);
        
        /*
         * If multiple annotations are selected, the may have different alignments.
         */
        std::set<AnnotationTextAlignHorizontalEnum::Enum> selectedAlignments;
        for (std::vector<AnnotationText*>::iterator iter = m_annotations.begin();
             iter != m_annotations.end();
             iter++) {
            const AnnotationText* annText = *iter;
            CaretAssert(annText);
            selectedAlignments.insert(annText->getHorizontalAlignment());
        }
        
        AnnotationTextAlignHorizontalEnum::Enum alignment = AnnotationTextAlignHorizontalEnum::LEFT;
        bool alignmentValid = false;
        if (selectedAlignments.size() == 1) {
            alignment = *(selectedAlignments.begin());
            alignmentValid = true;
        }
        
        /*
         * Update the status of each action
         *
         * An action is "checked" if an only if all selected annotations
         * have the same alignment.
         */
        QList<QAction*> allActions = m_horizontalAlignActionGroup->actions();
        QListIterator<QAction*> iter(allActions);
        while (iter.hasNext()) {
            QAction* action = iter.next();
            const int intValue = action->data().toInt();
            bool valid = false;
            AnnotationTextAlignHorizontalEnum::Enum actionAlign = AnnotationTextAlignHorizontalEnum::fromIntegerCode(intValue,
                                                                                                                     &valid);
            bool actionChecked = false;
            if (valid) {
                if (alignmentValid) {
                    if (actionAlign == alignment) {
                        actionChecked = true;
                    }
                }
            }
            action->setChecked(actionChecked);
        }
        
        if (alignmentValid) {
            AnnotationText::setUserDefaultHorizontalAlignment(alignment);
        }
        m_horizontalAlignActionGroup->blockSignals(false);
    }
    
    {
        /*
         * Update vertical alignment
         */
        m_verticalAlignActionGroup->blockSignals(true);
        
        /*
         * If multiple annotations are selected, the may have different alignments.
         */
        std::set<AnnotationTextAlignVerticalEnum::Enum> selectedAlignments;
        for (std::vector<AnnotationText*>::iterator iter = m_annotations.begin();
             iter != m_annotations.end();
             iter++) {
            const AnnotationText* annText = *iter;
            CaretAssert(annText);
            selectedAlignments.insert(annText->getVerticalAlignment());
        }
        
        AnnotationTextAlignVerticalEnum::Enum alignment = AnnotationTextAlignVerticalEnum::TOP;
        bool alignmentValid = false;
        if (selectedAlignments.size() == 1) {
            alignment = *(selectedAlignments.begin());
            alignmentValid = true;
        }
        
        /*
         * Update the status of each action
         *
         * An action is "checked" if an only if all selected annotations
         * have the same alignment.
         */
        QList<QAction*> allActions = m_verticalAlignActionGroup->actions();
        QListIterator<QAction*> iter(allActions);
        while (iter.hasNext()) {
            QAction* action = iter.next();
            const int intValue = action->data().toInt();
            bool valid = false;
            AnnotationTextAlignVerticalEnum::Enum actionAlign = AnnotationTextAlignVerticalEnum::fromIntegerCode(intValue,
                                                                                                                 &valid);
            bool actionChecked = false;
            if (valid) {
                if (alignmentValid) {
                    if (actionAlign == alignment) {
                        actionChecked = true;
                    }
                }
            }
            action->setChecked(actionChecked);
        }
        
        if (alignmentValid) {
            AnnotationText::setUserDefaultVerticalAlignment(alignment);
        }
        
        m_verticalAlignActionGroup->blockSignals(false);
    }
    
    setEnabled( ! m_annotations.empty());
}

/**
 * Gets called when a horizontal alignment selection is made.
 *
 * @param action
 *     Action that was selected.
 */
void
AnnotationTextAlignmentWidget::horizontalAlignmentActionSelected(QAction* action)
{
    CaretAssert(action);
    const int intValue = action->data().toInt();
    bool valid = false;
    AnnotationTextAlignHorizontalEnum::Enum actionAlign = AnnotationTextAlignHorizontalEnum::fromIntegerCode(intValue,
                                                                                                             &valid);
    if (valid) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotations.begin(),
                                             m_annotations.end());
        undoCommand->setModeTextAlignmentHorizontal(actionAlign,
                                                    annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        
        AnnotationText::setUserDefaultHorizontalAlignment(actionAlign);
    }
}

/**
 * Gets called when a vertical alignment selection is made.
 *
 * @param action
 *     Action that was selected.
 */
void
AnnotationTextAlignmentWidget::verticalAlignmentActionSelected(QAction* action)
{
    CaretAssert(action);
    const int intValue = action->data().toInt();
    bool valid = false;
    AnnotationTextAlignVerticalEnum::Enum actionAlign = AnnotationTextAlignVerticalEnum::fromIntegerCode(intValue,
                                                                                                             &valid);
    if (valid) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotations.begin(),
                                             m_annotations.end());
        undoCommand->setModeTextAlignmentVertical(actionAlign,
                                                  annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        
        AnnotationText::setUserDefaultVerticalAlignment(actionAlign);
    }
}

/**
 * Create a tool button for the given horizontal alignment.
 * The tool button will contain an action with the appropriate
 * icon and tooltip.
 *
 * @param horizontalAlignment
 *     The horizontal alignment.
 */
QToolButton*
AnnotationTextAlignmentWidget::createHorizontalAlignmentToolButton(const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment)
{
    WorkbenchIconTypeEnum::Enum iconType(WorkbenchIconTypeEnum::NO_ICON);
    QString toolTipText;
    switch (horizontalAlignment) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER;
            toolTipText = "Align Text Center";
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT;
            toolTipText = "Align Text Left";
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT;
            toolTipText = "Align Text Right";
            break;
    }
    
    QAction* action = new WorkbenchAction(iconType,
                                          this);
    action->setCheckable(true);
    action->setData((int)AnnotationTextAlignHorizontalEnum::toIntegerCode(horizontalAlignment));
    action->setToolTip(toolTipText);
    
    QToolButton* toolButton = new WorkbenchToolButton();
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(QSize(12, 12));
    
    return toolButton;
}

/**
 * Create a tool button for the given vertical alignment.
 * The tool button will contain an action with the appropriate
 * icon and tooltip.
 *
 * @param verticalAlignment
 *     The vertical alignment.
 */
QToolButton*
AnnotationTextAlignmentWidget::createVerticalAlignmentToolButton(const AnnotationTextAlignVerticalEnum::Enum verticalAlignment)
{
    WorkbenchIconTypeEnum::Enum iconType(WorkbenchIconTypeEnum::NO_ICON);
    QString toolTipText;
    switch (verticalAlignment) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM;
            toolTipText = "Align Text Bottom";
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE;
            toolTipText = "Align Text Middle";
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            iconType    = WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP;
            toolTipText = "Align Text Top";
            break;
    }
    
    QAction* action = new WorkbenchAction(iconType,
                                          this);
    action->setCheckable(true);
    action->setData((int)AnnotationTextAlignVerticalEnum::toIntegerCode(verticalAlignment));
    action->setToolTip(toolTipText);

    QToolButton* toolButton = new WorkbenchToolButton();
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(QSize(12, 12));
    
    return toolButton;
}
