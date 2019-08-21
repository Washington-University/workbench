
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

#define __ANNOTATION_FORMAT_WIDGET_DECLARE__
#include "AnnotationFormatWidget.h"
#undef __ANNOTATION_FORMAT_WIDGET_DECLARE__

#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationMenuArrange.h"
#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationFormatWidget 
 * \brief Widget for formatting annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *     The user input mode
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     Parent of this widget.
 */
AnnotationFormatWidget::AnnotationFormatWidget(const UserInputModeEnum::Enum userInputMode,
                                               const int32_t browserWindowIndex,
                                               QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* formatLabel = new QLabel("Format");
    
    QWidget* arrangeButton = createArrangeMenuToolButton();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(formatLabel);
    layout->addWidget(arrangeButton);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationFormatWidget::~AnnotationFormatWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationFormatWidget::updateContent(Annotation* /*annotation*/)
{
}

/**
 * @return The arrange tool button.
 */
QWidget*
AnnotationFormatWidget::createArrangeMenuToolButton()
{
    AnnotationMenuArrange* arrangeMenu = new AnnotationMenuArrange(m_userInputMode,
                                                                   m_browserWindowIndex);
    
    QAction* arrangeAction = new QAction("Arrange",
                                         this);
    arrangeAction->setToolTip("Arrange (align) and group annotations");
    arrangeAction->setMenu(arrangeMenu);
    
    QToolButton* arrangeToolButton = new QToolButton();
    arrangeToolButton->setDefaultAction(arrangeAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(arrangeToolButton);
    
    return arrangeToolButton;
}

