
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
#include <QToolButton>

#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationLineArrowTipsWidget 
 * \brief Widget for enabling/disabling line arrow tips
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationLineArrowTipsWidget::AnnotationLineArrowTipsWidget(const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotationLine = NULL;
    
    QLabel* label = new QLabel("Line");
    
    const QSize toolButtonSize(18, 18);
    
    m_endArrowToolButton = new QToolButton();
    m_endArrowToolButton->setArrowType(Qt::DownArrow);
    m_endArrowToolButton->setCheckable(true);
    m_endArrowToolButton->setToolTip("Show arrow at line's end coordinate");
    m_endArrowToolButton->setFixedSize(toolButtonSize);
    QObject::connect(m_endArrowToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(endArrowTipActionToggled()));
    
    m_startArrowToolButton = new QToolButton();
    m_startArrowToolButton->setArrowType(Qt::UpArrow);
    m_startArrowToolButton->setCheckable(true);
    m_startArrowToolButton->setToolTip("Show arrow at line's start coordinate");
    m_startArrowToolButton->setFixedSize(toolButtonSize);
    QObject::connect(m_startArrowToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(startArrowTipActionToggled()));
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(label,
                          0, 0, Qt::AlignHCenter);
    gridLayout->addWidget(m_startArrowToolButton,
                          1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(m_endArrowToolButton,
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
    AnnotationLine* line = NULL;
    if ( ! annotationLines.empty()) {
        line = annotationLines[0];
    }
    
    if (line != NULL) {
        m_startArrowToolButton->setChecked(line->isDisplayStartArrow());
        m_endArrowToolButton->setChecked(line->isDisplayEndArrow());
        setEnabled(true);
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
//    if (m_annotationLine != NULL) {
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowStart(m_startArrowToolButton->isChecked(),
                                           annMan->getSelectedAnnotations());
        annMan->applyCommand(undoCommand);
        
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//    }
}

/**
 * Gets called when the line arrow end buttons is toggled.
 */
void
AnnotationLineArrowTipsWidget::endArrowTipActionToggled()
{
//    if (m_annotationLine != NULL) {
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowEnd(m_endArrowToolButton->isChecked(),
                                         annMan->getSelectedAnnotations());
        annMan->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//    }
}
