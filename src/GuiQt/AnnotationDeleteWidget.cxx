
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

#define __ANNOTATION_DELETE_WIDGET_DECLARE__
#include "AnnotationDeleteWidget.h"
#undef __ANNOTATION_DELETE_WIDGET_DECLARE__

#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "UserInputModeAnnotations.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationDeleteWidget 
 * \brief Widget for deleting annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The input mode
 * @param browserWindowIndex
 *    Index of browser window
 * @param parent
 *    The parent widget
 */
AnnotationDeleteWidget::AnnotationDeleteWidget(const UserInputModeEnum::Enum userInputMode,
                                               const int32_t browserWindowIndex,
                                               QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* deleteLabel = new QLabel("Delete");
    m_deleteToolButton = createDeleteToolButton();

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(deleteLabel);
    layout->addWidget(m_deleteToolButton, 0, Qt::AlignHCenter);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationDeleteWidget::~AnnotationDeleteWidget()
{
}

/**
 * Update the content.
 */
void
AnnotationDeleteWidget::updateContent()
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    m_deleteToolButtonAction->setEnabled(annotationManager->isAnnotationSelectedForEditingDeletable(m_browserWindowIndex));
    
    setEnabled(m_deleteToolButtonAction->isEnabled());
}

/**
 * @return The delete tool button.
 */
QToolButton*
AnnotationDeleteWidget::createDeleteToolButton()
{
    m_deleteToolButtonAction = WuQtUtilities::createAction("",
                                                           ("Delete the selected annotation\n"
                                                            "\n"
                                                            "Pressing the Delete key while an annotation\n"
                                                            "is selected will also delete an annotation\n"
                                                            "\n"
                                                            "Pressing the arrow will show a menu for\n"
                                                            "undeleting annotations"),
                                                           this,
                                                           this,
                                                           SLOT(deleteActionTriggered()));
    QToolButton* toolButton = new QToolButton();
    
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(toolButton,
                                                                                pixmap);
    
    
    /* trash can */
    painter->drawLine(4, 6, 4, 22);
    painter->drawLine(4, 22, 20, 22);
    painter->drawLine(20, 22, 20, 6);
    
    /* trash can lines */
    painter->drawLine(12, 8, 12, 20);
    painter->drawLine(8,  8,  8, 20);
    painter->drawLine(16, 8, 16, 20);
    
    /* trash can lid and handle */
    painter->drawLine(2, 6, 22, 6);
    painter->drawLine(8, 6, 8, 2);
    painter->drawLine(8, 2, 16, 2);
    painter->drawLine(16, 2, 16, 6);
    
    
    m_deleteToolButtonAction->setIcon(QIcon(pixmap));
    
    toolButton->setIconSize(pixmap.size());
    toolButton->setDefaultAction(m_deleteToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    
    return toolButton;
}

/**
 * Gets called when the delete action is triggered.
 */
void
AnnotationDeleteWidget::deleteActionTriggered()
{
    EventGetOrSetUserInputModeProcessor inputProcessorEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(inputProcessorEvent.getPointer());
    UserInputModeAbstract* inputProcessor(inputProcessorEvent.getUserInputProcessor());
    if (inputProcessor != NULL) {
        UserInputModeAnnotations* annotationsProcessor(dynamic_cast<UserInputModeAnnotations*>(inputProcessor));
        if (annotationsProcessor != NULL) {
            annotationsProcessor->deleteSelectedAnnotations();
        }
    }
}

