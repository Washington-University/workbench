/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QIcon>

#include "CaretLogger.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Create an action with the specified text.
 *
 * @param text
 *    Text for the action.
 * @param toolAndStatusTipText
 *    Text for both tool and status tips.
 * @param parent
 *    Owner of the created action.
 * @return
 *    Action that was created.
 */
QAction*
WuQtUtilities::createAction(const QString& text,
                            const QString& toolAndStatusTipText,
                            QObject* parent)
{
    QAction* action = new QAction(parent);
    action->setText(text);
    if (toolAndStatusTipText.isEmpty() == false) {
        action->setStatusTip(toolAndStatusTipText);
        action->setToolTip(toolAndStatusTipText);
    }
    
    return action;
}

/**
 * Create an action with the specified text, shortcut,
 * and calls the specified slot.
 *
 * @param text
 *    Text for the action.
 * @param toolAndStatusTipText
 *    Text for both tool and status tips.
 * @param shortcut
 *    Keyboard shortcut.
 * @param parent
 *    Owner of the created action.
 * @param receiver
 *    Owner of method that is called when action is triggered.
 * @param method
 *    method in receiver that is called when action is triggered.
 * @return
 *    Action that was created.
 */
QAction*
WuQtUtilities::createAction(const QString& text,
                            const QString& toolAndStatusTipText,
                            const QKeySequence& shortcut,
                            QObject* parent,
                            QObject* receiver,
                            const char* method)
{
    QAction* action = WuQtUtilities::createAction(text,
                                                  toolAndStatusTipText,
                                                  parent,
                                                  receiver,
                                                  method);
    action->setShortcut(shortcut);
    return action;
}

/**
 * Create an action with the specified text and calls
 * the specified slot.
 *
 * @param text
 *    Text for the action.
 * @param toolAndStatusTipText
 *    Text for both tool and status tips.
 * @param parent
 *    Owner of the created action.
 * @param receiver
 *    Owner of method that is called when action is triggered.
 * @param method
 *    method in receiver that is called when action is triggered.
 * @return
 *    Action that was created.
 */
QAction*
WuQtUtilities::createAction(const QString& text,
                            const QString& toolAndStatusTipText,
                            QObject* parent,
                            QObject* receiver,
                            const char* method)
{
    QAction* action = WuQtUtilities::createAction(text,
                                                  toolAndStatusTipText,
                                                  parent);
//    QAction* action = new QAction(parent);
//    action->setText(text);
//    if (toolAndStatusTipText.isEmpty() == false) {
//        action->setStatusTip(toolAndStatusTipText);
//        action->setToolTip(toolAndStatusTipText);
//    }
    QObject::connect(action,
                     SIGNAL(triggered(bool)),
                     receiver,
                     method);
    return action;
}

/**
 * Create a vertical line widget used as a separator.
 *
 * @return  A vertical line widget used as a separator.
 */
QWidget* 
WuQtUtilities::createVerticalLineWidget()
{
    QFrame* frame = new QFrame();
    frame->setMidLineWidth(0);
    frame->setLineWidth(2);
    frame->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    return frame;
}

/**
 * Move a window relative to its parent window
 * but do not let the window move off the screen.
 * X is left to right, Y is top to bottom.
 *
 * @param parentWindow
 *    The parent window of the window being moved.
 * @param window
 *    The window.
 * @param xOffset
 *    Offset widget from parent by this X amount.
 * @param yOffset
 *    Offset widget from parent by this Y amount.
 */
void 
WuQtUtilities::moveWindowToOffset(QWidget* parentWindow,
                                  QWidget* window,
                                  const int xOffset, 
                                  const int yOffset)
{
    int x = parentWindow->x() + xOffset;
    int y = parentWindow->y() + yOffset;
    
    QDesktopWidget* dw = QApplication::desktop();
    const QRect geometry = dw->availableGeometry(parentWindow);
    
    const int margin = 20;
    const int maxX = geometry.width()  - margin;
    const int maxY = geometry.height() - margin;
    
    if (x > maxX) x = maxX;
    if (x < margin) x = margin;
    if (y > maxY) y = maxY;
    if (y < margin) y = margin;
    
    window->move(x, y);
}

/**
 * Set the tool tip and status tip for a widget.
 * 
 * @param widget
 *    Widget that has its tool and status tip set.
 * @param text
 *    Text for the tool and status tip.
 */
void
WuQtUtilities::setToolTipAndStatusTip(QWidget* widget,
                                      const QString& text)
{
    widget->setToolTip(text);
    widget->setStatusTip(text);
}

/**
 * Print a list of resources to the Caret Logger.
 */
void 
WuQtUtilities::sendListOfResourcesToCaretLogger()
{
    QString msg = "Resources loaded:\n";
    QDir dir(":/");
    QFileInfoList infoList = dir.entryInfoList();
    for (int i = 0; i < infoList.count(); i++) {
        msg += "   ";
        msg += infoList.at(i).filePath();
    }
    CaretLogInfo(msg);
}

/**
 * Load an icon.  
 * 
 * @param filename
 *    Name of file (or resource) containing the icon.
 * @param iconOut
 *    Output that will contain the desired icon.
 * @return
 *    True if the icon is valid, else false.
 */
bool 
WuQtUtilities::loadIcon(const QString& filename,
                        QIcon& iconOut)
{
    QPixmap pixmap(filename);
    if (pixmap.isNull()) {
        QString msg = (QString("Failed to load icon named \"")
                       + filename
                       + "\"");
        CaretLogSevere(msg);
        return false;
    }

    if ((pixmap.width() <= 0) || (pixmap.height() <= 0)) {
        QString msg = "Pixmap " + filename + " has invalid size";
        CaretLogSevere(msg);
        return false;
    }
    
    iconOut.addPixmap(pixmap);
    
    return true;
}

/**
 * Find the widget with the maximum height in its
 * size hint.  Apply this height to all of the widgets.
 * 
 * @param w1   Required widget.
 * @param w2   Required widget.
 * @param w3   Optional widget.
 * @param w4   Optional widget.
 * @param w5   Optional widget.
 * @param w6   Optional widget.
 * @param w7   Optional widget.
 * @param w8   Optional widget.
 * @param w9   Optional widget.
 * @param w10  Optional widget.
 */
void 
WuQtUtilities::matchWidgetHeights(QWidget* w1,
                                  QWidget* w2,
                                  QWidget* w3,
                                  QWidget* w4,
                                  QWidget* w5,
                                  QWidget* w6,
                                  QWidget* w7,
                                  QWidget* w8,
                                  QWidget* w9,
                                  QWidget* w10)
{
    QVector<QWidget*> widgets;
    
    if (w1 != NULL) widgets.push_back(w1);
    if (w2 != NULL) widgets.push_back(w2);
    if (w3 != NULL) widgets.push_back(w3);
    if (w4 != NULL) widgets.push_back(w4);
    if (w5 != NULL) widgets.push_back(w5);
    if (w6 != NULL) widgets.push_back(w6);
    if (w7 != NULL) widgets.push_back(w7);
    if (w8 != NULL) widgets.push_back(w8);
    if (w9 != NULL) widgets.push_back(w9);
    if (w10 != NULL) widgets.push_back(w10);
    
    int maxHeight = 0;
    const int num = widgets.size();
    for (int i = 0; i < num; i++) {
        const int h = widgets[i]->sizeHint().height();
        if (h > maxHeight) {
            maxHeight = h;
        }
    }
    
    if (maxHeight > 0) {
        for (int i = 0; i < num; i++) {
            widgets[i]->setFixedHeight(maxHeight);
        }
    }
}

void 
WuQtUtilities::setLayoutMargins(QBoxLayout* layout,
                                const int spacing,
                                const int margin,
                                const int contentsMargin)
{
    layout->setSpacing(spacing);
    layout->setMargin(margin);
    layout->setContentsMargins(contentsMargin,
                               contentsMargin,
                               contentsMargin,
                               contentsMargin);
}
