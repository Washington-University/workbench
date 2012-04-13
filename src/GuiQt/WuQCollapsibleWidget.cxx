
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
    
    
    QWidget* widget = new QWidget();
    this->collapsibleLayout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(this->collapsibleLayout, 2, 2);
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(widget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);
    //layout->addStretch();
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
    QSize minSize = this->minimumSize();
    std::cout << "Collapse Min Size: "
    << minSize.width() << ", " << minSize.width() << std::endl;
    
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
    showHideAction->setData(qVariantFromValue((void*)widget));
    QToolButton* showHideToolButton = new QToolButton();
    showHideToolButton->setDefaultAction(showHideAction);
    
    QHBoxLayout* controlLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(controlLayout, 2, 2);
    controlLayout->addWidget(label);
    controlLayout->addStretch();
    controlLayout->addWidget(showHideToolButton);
    controlLayout->addSpacing(15);
  
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


