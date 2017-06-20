
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

#define __ANNOTATION_TEXT_ORIENTATION_WIDGET_DECLARE__
#include "AnnotationTextOrientationWidget.h"
#undef __ANNOTATION_TEXT_ORIENTATION_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QToolButton>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationTextOrientationWidget 
 * \brief Widget for selection of text orientation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationTextOrientationWidget::AnnotationTextOrientationWidget(const int32_t browserWindowIndex,
                                                                 QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* orientationLabel = new QLabel("Orientation");
    QToolButton* horizontalOrientationToolButton = createOrientationToolButton(AnnotationTextOrientationEnum::HORIZONTAL);
    QToolButton* verticalOrientationToolButton   = createOrientationToolButton(AnnotationTextOrientationEnum::STACKED);

    m_orientationActionGroup = new QActionGroup(this);
    m_orientationActionGroup->setExclusive(false);
    m_orientationActionGroup->addAction(horizontalOrientationToolButton->defaultAction());
    m_orientationActionGroup->addAction(verticalOrientationToolButton->defaultAction());
    QObject::connect(m_orientationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(orientationActionSelected(QAction*)));

    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(orientationLabel,
                          row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(horizontalOrientationToolButton,
                          row, 0);
    gridLayout->addWidget(verticalOrientationToolButton,
                          row, 1);
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationTextOrientationWidget::~AnnotationTextOrientationWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotationTextsIn.
 */
void
AnnotationTextOrientationWidget::updateContent(std::vector<AnnotationText*>& annotationTextsIn)
{
    m_annotations.clear();
    m_annotations.insert(m_annotations.end(),
                         annotationTextsIn.begin(),
                         annotationTextsIn.end());
    
    {
        /*
         * Update orientation
         */
        m_orientationActionGroup->blockSignals(true);
        
        /*
         * If multiple annotations are selected, the may have different orientation.
         */
        std::set<AnnotationTextOrientationEnum::Enum> selectedOrientations;
        for (std::vector<AnnotationText*>::iterator iter = m_annotations.begin();
             iter != m_annotations.end();
             iter++) {
            const AnnotationText* annText = *iter;
            CaretAssert(annText);
            selectedOrientations.insert(annText->getOrientation());
        }
        
        AnnotationTextOrientationEnum::Enum orientation = AnnotationTextOrientationEnum::HORIZONTAL;
        bool orientationValid = false;
        if (selectedOrientations.size() == 1) {
            orientation = *(selectedOrientations.begin());
            orientationValid = true;
        }
        
        /*
         * Update the status of each action
         *
         * An action is "checked" if an only if all selected annotations
         * have the same orientation.
         */
        QList<QAction*> allActions = m_orientationActionGroup->actions();
        QListIterator<QAction*> iter(allActions);
        while (iter.hasNext()) {
            QAction* action = iter.next();
            const int intValue = action->data().toInt();
            bool valid = false;
            AnnotationTextOrientationEnum::Enum actionOrient = AnnotationTextOrientationEnum::fromIntegerCode(intValue,
                                                                                                                 &valid);
            bool actionChecked = false;
            if (valid) {
                if (orientationValid) {
                    if (actionOrient == orientation) {
                        actionChecked = true;
                    }
                }
            }
            action->setChecked(actionChecked);
        }
        
        if (orientationValid) {
            AnnotationText::setUserDefaultOrientation(orientation);
        }
        
        m_orientationActionGroup->blockSignals(false);
    }
    
    setEnabled( ! m_annotations.empty());
}

/**
 * Gets called when a orientation selection is made.
 *
 * @param action
 *     Action that was selected.
 */
void
AnnotationTextOrientationWidget::orientationActionSelected(QAction* action)
{
    CaretAssert(action);
    const int intValue = action->data().toInt();
    bool valid = false;
    AnnotationTextOrientationEnum::Enum actionOrientation = AnnotationTextOrientationEnum::fromIntegerCode(intValue,
                                                                                                         &valid);
    if (valid) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotations.begin(),
                                             m_annotations.end());
        undoCommand->setModeTextOrientation(actionOrientation,
                                            annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        
        AnnotationText::setUserDefaultOrientation(actionOrientation);
    }
}

/**
 * Create a tool button for the given horizontal alignment.
 * The tool button will contain an action with the appropriate
 * icon and tooltip.
 *
 * @param orientation
 *     The horizontal alignment.
 */
QToolButton*
AnnotationTextOrientationWidget::createOrientationToolButton(const AnnotationTextOrientationEnum::Enum orientation)
{
    QString toolTipText;
    switch (orientation) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            toolTipText = "Horizontal (left-to-right) text";
            break;
        case AnnotationTextOrientationEnum::STACKED:
            toolTipText = "Stacked (top-to-bottom) text";
            break;
    }
    
    QToolButton* toolButton = new QToolButton();
    QPixmap pixmap = createHorizontalAlignmentPixmap(toolButton,
                                                     orientation);
    
    QAction* action = new QAction(this);
    action->setCheckable(true);
    action->setData((int)AnnotationTextOrientationEnum::toIntegerCode(orientation));
    action->setToolTip(toolTipText);
    action->setIcon(QIcon(pixmap));
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(pixmap.size());
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    
    return toolButton;
}

/**
 * Create a horizontal alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param orientation
 *    The horizontal alignment.
 * @return
 *    Pixmap with icon for the given horizontal alignment.
 */
QPixmap
AnnotationTextOrientationWidget::createHorizontalAlignmentPixmap(const QWidget* widget,
                                                               const AnnotationTextOrientationEnum::Enum orientation)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24.0;
    float height = 30.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    switch (orientation) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            painter->drawText(pixmap.rect(),
                             (Qt::AlignCenter),
                             "ab");
                             
            break;
        case AnnotationTextOrientationEnum::STACKED:
            painter->drawText(pixmap.rect(),
                             (Qt::AlignCenter),
                             "a\nb");
            break;
    }

    
    
    return pixmap;
}

