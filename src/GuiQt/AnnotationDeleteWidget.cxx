
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
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
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
 */
AnnotationDeleteWidget::AnnotationDeleteWidget(const int32_t browserWindowIndex,
                                               QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* deleteLabel = new QLabel("Delete");
    m_deleteToolButton = createDeleteToolButton();

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(deleteLabel);
    layout->addWidget(m_deleteToolButton);
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
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    m_deleteToolButtonAction->setEnabled(annotationManager->isAnnotationSelectedForEditingDeletable(m_browserWindowIndex));
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
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_browserWindowIndex);
    std::vector<Annotation*> deleteAnnotations;
    for (auto a : selectedAnnotations) {
        if (a->testProperty(Annotation::Property::DELETION)) {
            switch (a->getType()) {
                case AnnotationTypeEnum::BOX:
                    break;
                case AnnotationTypeEnum::BROWSER_TAB:
                    CaretAssert(0);
                    break;
                case AnnotationTypeEnum::COLOR_BAR:
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    break;
                case AnnotationTypeEnum::OVAL:
                    break;
                case AnnotationTypeEnum::SCALE_BAR:
                    break;
                case AnnotationTypeEnum::TEXT:
                    break;
            }
            
            deleteAnnotations.push_back(a);
        }
    }
    
    if ( ! deleteAnnotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeDeleteAnnotations(deleteAnnotations);

        AString errorMessage;
        if ( ! annotationManager->applyCommand(UserInputModeEnum::Enum::ANNOTATIONS,
                                               undoCommand,
                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

