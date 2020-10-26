
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <iostream>

#include <QAction>
#include <QActionGroup>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#define __WU_Q_COLLAPSIBLE_WIDGET_DECLARE__
#include "WuQCollapsibleWidget.h"
#undef __WU_Q_COLLAPSIBLE_WIDGET_DECLARE__

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQCollapsibleWidget 
 * \brief A container widget with multiple collapsible sections
 *
 * A container widget with multiple collapsible sections in 
 * a vertical orientation.  Each section is independent of
 * the other sections so that multiple sections may be open
 * at one time.
 */

/**
 * Constructor.
 */
WuQCollapsibleWidget::WuQCollapsibleWidget(QWidget* parent)
: QWidget(parent)
{
    this->showHideActionGroup = new QActionGroup(this);
    QObject::connect(this->showHideActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(showHideActionGroupTriggered(QAction*)));
    
    
    this->collapsibleLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(this->collapsibleLayout, 2, 2);
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(this->collapsibleLayout);
    widgetLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(widget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);
    //layout->addStretch(); Using this will result in the widget too small vertically
}

/**
 * Destructor.
 */
WuQCollapsibleWidget::~WuQCollapsibleWidget()
{
    
}

/**
 * @return The recommended size for this widget.
 */
QSize 
WuQCollapsibleWidget::sizeHint() const
{
    //QSize minSize = this->minimumSize();
    //std::cout << "Collapse Min Size: "
    //<< minSize.width() << ", " << minSize.width() << std::endl;
    
    QSize sz = QWidget::sizeHint();
    sz.setHeight(500);
    return sz;
}


/**
 * Add a widget to this collapsible widget with the
 * given label in the titlebar.
 * @param widget
 *   Widget that is added.
 * @param text
 *   Text displayed for collapsing/expanding view
 *   of widget.
 */
void 
WuQCollapsibleWidget::addItem(QWidget* widget,
                                const QString& text)
{
    CaretAssert(widget);
    
    QLabel* label = new QLabel(text);
    
    QAction* showHideAction = this->showHideActionGroup->addAction("-");
    showHideAction->setData(QVariant::fromValue((void*)widget));
    QToolButton* showHideToolButton = new QToolButton();
    showHideToolButton->setDefaultAction(showHideAction);
    
    QHBoxLayout* controlLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(controlLayout, 2, 2);
    controlLayout->addWidget(showHideToolButton);
    controlLayout->addSpacing(15);
    controlLayout->addWidget(label);
    controlLayout->addStretch();
  
    QFrame* controlFrame = new QFrame();
    controlFrame->setLineWidth(1);
    controlFrame->setMidLineWidth(2);
    controlFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    controlFrame->setLayout(controlLayout);
    controlFrame->setFixedHeight(controlFrame->sizeHint().height());

    this->collapsibleLayout->addWidget(controlFrame);
    this->collapsibleLayout->addWidget(widget);
    
    this->widgets.push_back(widget);
}

/**
 * Called when a show/hide tool button is triggered
 */
void 
WuQCollapsibleWidget::showHideActionGroupTriggered(QAction* action)
{
    if (action != NULL) {
        void* pointer = action->data().value<void*>();
        CaretAssert(pointer);
        QWidget* widget = (QWidget*)pointer;
        QString collapseExpandText = "";
        if (widget->isVisible()) {
            widget->setVisible(false);
            collapseExpandText = "+";
        }
        else {
            widget->setVisible(true);
            collapseExpandText = "-";
        }
        action->setText(collapseExpandText);
    }
}


