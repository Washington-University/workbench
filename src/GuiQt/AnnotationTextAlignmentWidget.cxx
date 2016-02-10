
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
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
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
 * @param browserWindowIndex
 *     Index of browser window.
 * @param parent
 *     The parent widget.
 */
AnnotationTextAlignmentWidget::AnnotationTextAlignmentWidget(const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_smallLayoutFlag = true;
    
    QToolButton* leftAlignToolButton   = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::LEFT);
    QToolButton* centerAlignToolButton = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::CENTER);
    QToolButton* rightAlignToolButton  = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::RIGHT);
    
    m_horizontalAlignActionGroup = new QActionGroup(this);
    m_horizontalAlignActionGroup->setExclusive(false); // not exclusive as may need to turn all off
    m_horizontalAlignActionGroup->addAction(leftAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(centerAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(rightAlignToolButton->defaultAction());
    QObject::connect(m_horizontalAlignActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(horizontalAlignmentActionSelected(QAction*)));
    
    
    QToolButton* topAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::TOP);
    QToolButton* middleAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::MIDDLE);
    QToolButton* bottomAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::BOTTOM);
    
    m_verticalAlignActionGroup = new QActionGroup(this);
    m_verticalAlignActionGroup->setExclusive(false); // not exclusive as may need to turn all off
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
    m_annotations.insert(m_annotations.end(),
                         annotationTexts.begin(),
                         annotationTexts.end());
    
    {
        /*
         * Update horizontal alignment
         */
        m_horizontalAlignActionGroup->blockSignals(true);
        
        /*
         * If multiple annotations are selected, the may have different alignments.
         */
        std::set<AnnotationTextAlignHorizontalEnum::Enum> selectedAlignments;
        for (std::vector<AnnotationText*>::iterator iter = annotationTexts.begin();
             iter != annotationTexts.end();
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
        for (std::vector<AnnotationText*>::iterator iter = annotationTexts.begin();
             iter != annotationTexts.end();
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
    
    setEnabled( ! annotationTexts.empty());
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
        undoCommand->setModeTextAlignmentHorizontal(actionAlign,
                                                    m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        annMan->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        
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
        undoCommand->setModeTextAlignmentVertical(actionAlign,
                                                  m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        annMan->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        
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
    QString toolTipText;
    switch (horizontalAlignment) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            toolTipText = "Align Text Center";
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            toolTipText = "Align Text Left";
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            toolTipText = "Align Text Right";
            break;
    }
    
    QToolButton* toolButton = new QToolButton();
    QPixmap pixmap = createHorizontalAlignmentPixmap(toolButton,
                                                     horizontalAlignment);
    
    QAction* action = new QAction(this);
    action->setCheckable(true);
    action->setData((int)AnnotationTextAlignHorizontalEnum::toIntegerCode(horizontalAlignment));
    action->setToolTip(toolTipText);
    action->setIcon(QIcon(pixmap));
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(pixmap.size());
    
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
    QString toolTipText;
    switch (verticalAlignment) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            toolTipText = "Align Text Bottom";
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            toolTipText = "Align Text Middle";
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            toolTipText = "Align Text Top";
            break;
    }
    
    QToolButton* toolButton = new QToolButton();
    QPixmap pixmap = createVerticalAlignmentPixmap(toolButton,
                                                   verticalAlignment);
    
    QAction* action = new QAction(this);
    action->setCheckable(true);
    action->setData((int)AnnotationTextAlignVerticalEnum::toIntegerCode(verticalAlignment));
    action->setToolTip(toolTipText);
    action->setIcon(QIcon(pixmap));
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(pixmap.size());
    
    return toolButton;
}


/**
 * Create a horizontal alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground, 
 *    the palette from the given widget is used.
 * @param horizontalAlignment
 *    The horizontal alignment.
 * @return
 *    Pixmap with icon for the given horizontal alignment.
 */
QPixmap
AnnotationTextAlignmentWidget::createHorizontalAlignmentPixmap(const QWidget* widget,
                                                     const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24.0;
    float height = 24.0;
    int32_t numLines = 5;
    
    if (m_smallLayoutFlag) {
        width    = 12.0;
        height   = 12.0;
        numLines = 3;
    }
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    const qreal margin          = width * 0.05;
    const qreal longLineLength  = width - (margin * 2.0);
    const qreal shortLineLength = width / 2.0;
    const qreal yStep = MathFunctions::round(height / (numLines + 1));  //6.0);
    
    for (int32_t i = 1; i <= numLines; i++) {
        const qreal lineLength = (((i % 2) == 0)
                                  ? shortLineLength
                                  : longLineLength);
        const qreal y = yStep * i;
        
        qreal xStart = 0.0;
        qreal xEnd   = width;
        
        switch (horizontalAlignment) {
            case AnnotationTextAlignHorizontalEnum::CENTER:
                xStart = (width - lineLength) / 2.0;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::LEFT:
                xStart = margin;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::RIGHT:
                xEnd   = width - margin;
                xStart = xEnd - lineLength;
                break;
        }

        painter->drawLine(QLineF(xStart,
                                y,
                                xEnd,
                                y));
    }
    
    return pixmap;
}

/**
 * Create a vertical alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param verticalAlignment
 *    The vertical alignment.
 * @return
 *    Pixmap with icon for the given vertical alignment.
 */
QPixmap
AnnotationTextAlignmentWidget::createVerticalAlignmentPixmap(const QWidget* widget,
                                                         const AnnotationTextAlignVerticalEnum::Enum verticalAlignment)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24.0;
    float height = 24.0;
    int32_t numLines = 5;
    
    if (m_smallLayoutFlag) {
        width    = 12.0;
        height   = 12.0;
        numLines = 3;
    }
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    const qreal margin          = width * 0.05;
    
    if (m_smallLayoutFlag) {
        float yStep = 3.0;
        float y1 = 0.0;
        float y2 = 0.0;
        switch (verticalAlignment) {
            case AnnotationTextAlignVerticalEnum::BOTTOM:
                y1 = (height - 1 - yStep);
                y2 = y1 + yStep;
                break;
            case AnnotationTextAlignVerticalEnum::MIDDLE:
                y1 = MathFunctions::round((height / 2.0) - (yStep / 2.0));
                y2 = y1 + yStep;
                break;
            case AnnotationTextAlignVerticalEnum::TOP:
                y1 = yStep;
                y2 = y1 + yStep;
                break;
        }
        
        const float xStart = margin;
        const float xEnd   = width - (margin * 2.0);
        
        painter->drawLine(QLineF(xStart, y1,
                                xEnd,   y1));
        painter->drawLine(QLineF(xStart, y2,
                                xEnd,   y2));
    }
    else {
        const qreal longLineLength  = width - (margin * 2.0);
        const qreal shortLineLength = width / 2.0;
        const qreal yStep = MathFunctions::round(height / 6.0);
        for (int32_t i = 1; i <= numLines; i++) {
            const qreal lineLength = (((i % 2) == 0)
                                      ? shortLineLength
                                      : longLineLength);
            
            int32_t iOffset = i;
            switch (verticalAlignment) {
                case AnnotationTextAlignVerticalEnum::BOTTOM:
                    iOffset += 2;
                    break;
                case AnnotationTextAlignVerticalEnum::MIDDLE:
                    iOffset += 1;
                    break;
                case AnnotationTextAlignVerticalEnum::TOP:
                    break;
            }
            const qreal y = yStep * iOffset;
            
            const qreal xStart = margin;
            const qreal xEnd   = xStart + lineLength;
            
            
            painter->drawLine(QLineF(xStart,
                                    y,
                                    xEnd,
                                    y));
        }
    }
    
    return pixmap;
}

