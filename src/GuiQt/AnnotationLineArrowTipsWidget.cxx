
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

#define __ANNOTATION_LINE_ARROW_TIPS_WIDGET_DECLARE__
#include "AnnotationLineArrowTipsWidget.h"
#undef __ANNOTATION_LINE_ARROW_TIPS_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>

#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WorkbenchAction.h"
#include "WorkbenchToolButton.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationLineArrowTipsWidget 
 * \brief Widget for enabling/disabling line arrow tips
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *    Index of browser window
 * @param parent
 *    Parent widget
 */
AnnotationLineArrowTipsWidget::AnnotationLineArrowTipsWidget(const UserInputModeEnum::Enum userInputMode,
                                                             const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* label = new QLabel("Line");

    QToolButton* endArrowToolButton = new WorkbenchToolButton();
    m_endArrowAction = new WorkbenchAction(WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP,
                                           this);
    m_endArrowAction->setCheckable(true);
    m_endArrowAction->setToolTip("Show arrow at line's end coordinate");
    QObject::connect(m_endArrowAction, &QAction::triggered,
                     this, &AnnotationLineArrowTipsWidget::endArrowTipActionToggled);
    endArrowToolButton->setDefaultAction(m_endArrowAction);
    
    
    QToolButton* startArrowToolButton = new WorkbenchToolButton();
    m_startArrowAction = new WorkbenchAction(WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN,
                                             this);
    m_startArrowAction->setCheckable(true);
    m_startArrowAction->setToolTip("Show arrow at line's start coordinate");
    QObject::connect(m_startArrowAction, &QAction::triggered,
                     this, &AnnotationLineArrowTipsWidget::startArrowTipActionToggled);
    startArrowToolButton->setDefaultAction(m_startArrowAction);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(label,
                          0, 0, Qt::AlignHCenter);
    gridLayout->addWidget(startArrowToolButton,
                          1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(endArrowToolButton,
                          2, 0, Qt::AlignHCenter);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationLineArrowTipsWidget::~AnnotationLineArrowTipsWidget()
{
}

/**
 * Update with the given line annotation.
 *
 * @param annotationLines
 */
void
AnnotationLineArrowTipsWidget::updateContent(std::vector<AnnotationLine*>& annotationLines)
{
    m_annotations.clear();
    m_annotations.reserve(annotationLines.size());
    for (auto a : annotationLines) {
        if (a->testProperty(Annotation::Property::LINE_ARROWS)) {
            m_annotations.push_back(a);
        }
    }
    
    bool allStartOnFlag = true;
    bool allEndOnFlag   = true;
    
    const int32_t numLines = static_cast<int32_t>(annotationLines.size());
    for (int32_t i = 0; i < numLines; i++) {
        CaretAssertVectorIndex(annotationLines, i);
        if ( ! annotationLines[i]->isDisplayStartArrow()) {
            allStartOnFlag = false;
        }
        if ( ! annotationLines[i]->isDisplayEndArrow()) {
            allEndOnFlag = false;
        }
    }

    if (numLines <= 0) {
        allStartOnFlag = false;
        allEndOnFlag   = false;
    }
    
    m_startArrowAction->setChecked(allStartOnFlag);
    m_endArrowAction->setChecked(allEndOnFlag);
    
    if (numLines > 0) {
        setEnabled(true);
        
        AnnotationLine::setUserDefaultDisplayStartArrow(m_startArrowAction->isChecked());
        AnnotationLine::setUserDefaultDisplayEndArrow(m_endArrowAction->isChecked());
    }
    else {
        setEnabled(false);
    }
}


/**
 * Gets called when the line arrow start buttons is toggled.
 */
void
AnnotationLineArrowTipsWidget::startArrowTipActionToggled()
{
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowStart(m_startArrowAction->isChecked(),
                                           m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
        AnnotationLine::setUserDefaultDisplayStartArrow(m_startArrowAction->isChecked());
}

/**
 * Gets called when the line arrow end buttons is toggled.
 */
void
AnnotationLineArrowTipsWidget::endArrowTipActionToggled()
{
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowEnd(m_endArrowAction->isChecked(),
                                         m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);

    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
        AnnotationLine::setUserDefaultDisplayEndArrow(m_endArrowAction->isChecked());
}

