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

#include <limits>
#include <typeinfo>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
//#include <QSound>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QToolButton>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MacroPrototype.h"
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
 * Create an action with the specified text.
 *
 * @param text
 *    Text for the action.
 * @param toolAndStatusTipText
 *    Text for both tool and status tips.
 * @param shortcut
 *    Keyboard shortcut.
 * @param parent
 *    Owner of the created action.
 * @return
 *    Action that was created.
 */
QAction*
WuQtUtilities::createAction(const QString& text,
                            const QString& toolAndStatusTipText,
                            const QKeySequence& shortcut,
                            QObject* parent)
{
    QAction* action = new QAction(parent);
    action->setText(text);
    if (toolAndStatusTipText.isEmpty() == false) {
        action->setStatusTip(toolAndStatusTipText);
        action->setToolTip(toolAndStatusTipText);
    }
    action->setShortcut(shortcut);
    
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
 * Create a pushbutton.
 * 
 * @param text
 *    Text for the pushbutton.
 * @param toolAndStatusTipText
 *    Text for both tool and status tips.
 * @param receiver
 *    Owner of method that is called when button is clicked.
 * @param method
 *    method in receiver that is called when button is clicked.
 * @return
 *    Pushbutton that was created.
 */
QPushButton* 
WuQtUtilities::createPushButton(const QString& text,
                                const QString& toolAndStatusTipText,
                                QObject* receiver,
                                const char* method)
{
    QPushButton* pb = new QPushButton(text);
    if (toolAndStatusTipText.isEmpty() == false) {
        pb->setStatusTip(toolAndStatusTipText);
        pb->setToolTip(toolAndStatusTipText);
    }
    
    QObject::connect(pb,
                     SIGNAL(clicked()),
                     receiver,
                     method);

    return pb;
}

//QToolButton*
//WuQtUtilities::createToolButtonWithIcon(const QString& text,
//                                             const QString& iconFileName,
//                                             const QString& toolTip,
//                                             const QString& widgetName,
//                                             QObject* receiver,
//                                             WuQObject::QObjectBoolPointerToMemberFunction method)
//{
//    QIcon icon;
//    const bool iconValid = WuQtUtilities::loadIcon(iconFileName,
//                                                   icon);
//    
//    QToolButton* toolButton = new QToolButton();
//    if (iconValid) {
//        toolButton->setIcon(icon);
//    }
//    else {
//        toolButton->setText(text);
//    }
//    toolButton->setToolTip(toolTip);
//    toolButton->setObjectName(widgetName);
//    
//    WuQObject::connectFP(toolButton,
//                         static_cast<void (QObject::*)(bool)>(&QToolButton::clicked),
//                           receiver,
//                           method);
//    
//    return toolButton;
//}

QToolButton*
WuQtUtilities::createToolButtonWithIcon(const QString& text,
                                        const QString& iconFileName,
                                        const QString& toolTip,
                                        const QString& widgetName,
                                        QObject* receiver,
                                        const char* method)
{
    QIcon icon;
    const bool iconValid = WuQtUtilities::loadIcon(iconFileName,
                                                   icon);

    QToolButton* toolButton = new QToolButton();
    if (iconValid) {
        toolButton->setIcon(icon);
    }
    else {
        toolButton->setText(text);
    }
    toolButton->setToolTip(toolTip);
    toolButton->setObjectName(widgetName);

    WuQObject::connect(toolButton, SIGNAL(clicked(bool)),
                     receiver, method);
    
    return toolButton;
}

/**
 * Create a horizontal line widget used as a separator.
 *
 * @return  A horizontal line widget used as a separator.
 */
QWidget*
WuQtUtilities::createHorizontalLineWidget()
{
    QFrame* frame = new QFrame();
    frame->setMidLineWidth(1);
    frame->setLineWidth(1);
    frame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    return frame;
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
 * Place a dialog next to its parent.  May not work correctly with
 * multi-screen systems.
 *
 * MUST BE CALLED after a window is displayed since the given window
 * may not have its geometry (size) set until AFTER it is displayed.
 *
 * It will stop after the first one of these actions that is successful:
 *   1) Put window on right of parent if all of window will be visible.
 *   2) Put window on left of parent if all of window will be visible.
 *   3) Put window on right of parent if more space to right of window.
 *   4) Put window on left of parent.
 * @param parent
 *    The parent.
 * @param window
 *    The window.
 */
void 
WuQtUtilities::moveWindowToSideOfParent(QWidget* parent,
                                        QWidget* window)
{
    const QRect parentGeometry = parent->geometry();
    const int px = parentGeometry.x();
    const int py = parentGeometry.y();
    const int pw = parentGeometry.width();
    const int ph = parentGeometry.height();
    const int parentMaxX = px + pw;
    
    //int x = px + pw + 1;
    int y = py + ph - window->height() - 20;
   // int y = py;
    const int windowWidth = window->width();

    QDesktopWidget* dw = QApplication::desktop();
    const QRect geometry = dw->availableGeometry(parent);
    const int screenMinX = geometry.x();
    const int screenWidth = geometry.width();
    const int screenMaxX = screenMinX + screenWidth;
    const int screenMaxY = geometry.x() + geometry.height();
    
    const int spaceOnLeft = px -screenMinX;
    const int spaceOnRight = screenMaxX - parentMaxX;
    
    int x = screenMinX;
    if (spaceOnRight > windowWidth) {
        x = parentMaxX;
    }
    else if (spaceOnLeft > windowWidth) {
        x = px - windowWidth;
    }
    else if (spaceOnRight > spaceOnLeft) {
        x = screenMaxX - windowWidth;
    }
//    else {
//        x = screenMinX;
//    }
    
    if ((x + windowWidth) > screenMaxX) {
        x = screenMaxX - windowWidth;
    }
    if (x < screenMinX) {
        x = screenMinX;   
    }
    
    const int maxY = screenMaxY - window->height() - 50;
    if (y > maxY) {
        y = maxY;
    }
    if (y < 50) {
        y = 50;
    }
    
    window->move(x, y);
}

/**
 * Move and size a window limiting window so that 
 * it fits within the screen.
 * @param x
 *    X-coordinate of window.
 * @param y 
 *    Y-coordinate of window.
 * @param w
 *    Width of window.
 * @param h
 *    Height of window.
 * @param xywhOut
 *    On exit contains 4 values that are the actual
 *    x, y, width, and height of the window after 
 *    any needed adjustments for screen sizes.
 */
void
WuQtUtilities::moveAndSizeWindow(QWidget* window,
                                  const int32_t x,
                                  const int32_t y,
                                  const int32_t w,
                                 const int32_t h,
                                 int32_t* xywhOut)
{
    QDesktopWidget* dw = QApplication::desktop();
    
    /*
     * Get available geometry where window is to be placed
     * This geometry is all screens together as one large screen
     */
    QPoint pXY(x,
               y);
#if QT_VERSION >= 0x050000
    const QRect availableRect = dw->availableGeometry();
#else
    /*
     * Note 23 September 2016:
     *    Calling geometry is likely the WRONG method to call
     *    but since it is not causing a problem in in Qt 4.x
     *    we will continuing using to avoid the risk of 
     *    breaking scenes.
     */
    const QRect availableRect = dw->screen()->geometry();
#endif
    const int32_t screenSizeX = availableRect.width();
    const int32_t screenSizeY = availableRect.height();
    
    /*
     * Limit width/height in desktop
     */
    int32_t width  = std::min(w, screenSizeX);
    int32_t height = std::min(h, screenSizeY);
    
    /*
     * Limit window position in desktop
     */
    int32_t xPos = x;
    if (xPos < availableRect.x()) {
        xPos = availableRect.x();
    }
    const int32_t maxX = screenSizeX - 200;
    if (xPos >= maxX) {
        xPos = maxX;
    }
    int32_t yPos = y;
    if (yPos < availableRect.y()) {
        yPos = availableRect.y();
    }
    const int32_t maxY = screenSizeY - 200;
    if (yPos >= maxY) {
        yPos = maxY;
    }
        
    /*
     * Make sure visible in closest screen
     */
    pXY.setX(xPos);
    pXY.setY(yPos);
    const int32_t nearestScreen = dw->screenNumber(pXY);
    if (nearestScreen >= 0) {
        const QRect screenRect = dw->availableGeometry(nearestScreen);
        if (xPos < screenRect.x()) {
            xPos = screenRect.x();
        }
        const int32_t maxX = screenRect.right() - 200;
        if (xPos > maxX) {
            xPos = maxX;
        }
        if (yPos < screenRect.y()) {
            yPos = screenRect.y();
        }
        const int32_t maxY = screenRect.bottom() - 200;
        if (yPos > maxY) {
            yPos = maxY;
        }
        
        /*
         * ScreenRect width/height is size of screen
         * reduced by menu bars, docks and other items
         * that reduce available screen space
         */
        const int32_t maxWidth = screenRect.width();
        if (width > maxWidth) {
            width = maxWidth;
        }
        
        const int32_t maxHeight = screenRect.height();
        if (height > maxHeight) {
            height = maxHeight;
        }
        
        const QRect geom = dw->screenGeometry(nearestScreen);
        CaretLogInfo(QString("Window Available width/height: %1, %2 \n      Screen width/height: %3, %4"
                             ).arg(maxWidth).arg(maxHeight).arg(geom.width()).arg(geom.height()));
    }

    /*
     * Move and size window
     */
    window->move(xPos,
                 yPos);
    window->resize(width,
                   height);
    
    if (xywhOut != NULL) {
        xywhOut[0] = window->x();
        xywhOut[1] = window->y();
        xywhOut[2] = window->width();
        xywhOut[3] = window->height();
    }
}

/**
 * Resize a window but limit maximum size of window
 * to an area less than the full size of the display.
 * Has no effect on position of the window so part
 * of window may end up being outside the display.
 *
 * @param window
 *     Window that is resized.
 * @param width
 *     Desired width of the window.
 * @param height
 *     Desired width of the window.
 */
void
WuQtUtilities::resizeWindow(QWidget* window,
                            const int32_t width,
                            const int32_t height)
{
    QDesktopWidget* dw = QApplication::desktop();
    QPoint pXY(window->x(),
               window->y());
    const int32_t nearestScreen = dw->screenNumber(pXY);
    if (nearestScreen >= 0) {
        const QRect screenRect = dw->availableGeometry(nearestScreen);
        const int32_t screenWidth  = screenRect.width() - 100;
        const int32_t screenHeight = screenRect.height() - 100;
        
        const int windowWidth = std::min(width,
                               screenWidth);
        const int windowHeight = std::min(height,
                               screenHeight);
        
        window->resize(windowWidth,
                       windowHeight);
    }
}

/**
 * Limit a window's size to a percentage of the maximum size.
 *
 * @param window
 *     Window that is resized.
 * @param widthMaximumPercentage
 *     Maximum width percentage [0, 100]
 * @param heightMaximumPercentage
 *     Maximum height percentage [0, 100]
 */
void
WuQtUtilities::limitWindowSizePercentageOfMaximum(QWidget* window,
                                                  const float widthMaximumPercentage,
                                                  const float heightMaximumPercentage)
{
    AString message;
    if ((widthMaximumPercentage < 0.0)
        || (widthMaximumPercentage > 100.0)) {
        message.appendWithNewLine("Percentage width must range [0, 100]: value -> "
                                  + AString::number(widthMaximumPercentage));
    }
    if ((heightMaximumPercentage < 0.0)
        || (heightMaximumPercentage > 100.0)) {
        message.appendWithNewLine("Percentage height must range [0, 100]: value -> "
                                  + AString::number(heightMaximumPercentage));
    }
    
    if ( ! message.isEmpty()) {
        CaretLogSevere(message);
        return;
    }
    
    QDesktopWidget* dw = QApplication::desktop();
    QPoint pXY(window->x(),
               window->y());
    const int32_t nearestScreen = dw->screenNumber(pXY);
    if (nearestScreen >= 0) {
        const QRect screenRect = dw->availableGeometry(nearestScreen);
        const int32_t screenWidth  = screenRect.width();
        const int32_t screenHeight = screenRect.height();
    
        const float maxWidth = ((widthMaximumPercentage   / 100.0) * screenWidth);
        const float maxHeight = ((heightMaximumPercentage / 100.0) * screenHeight);
        
        window->setMaximumSize(maxWidth,
                               maxHeight);
        window->adjustSize();
    }
}


/**
 * Table widget has a default size of 640 x 480.
 * Estimate the size of the dialog with the table fully expanded.
 *
 * @param tableWidget
 *    Table widget whose size is estimated.
 */
QSize
WuQtUtilities::estimateTableWidgetSize(QTableWidget* tableWidget)
{
    QSize tableSize(0, 0);
    
    /*
     * Table widget has a default size of 640 x 480.
     * So estimate the size of the dialog with the table fully
     * expanded.
     */
    const int numRows = tableWidget->rowCount();
    const int numCols = tableWidget->columnCount();
    const int cellGap = (tableWidget->showGrid()
                         ? 3
                         : 0);
    if ((numRows > 0)
        && (numCols > 0)) {
        int tableWidth = 10; // start out with a little extra space
        int tableHeight = 0;
        
        if (tableWidget->horizontalHeader()->isHidden() == false) {
            QHeaderView* columnHeader = tableWidget->horizontalHeader();
            const int columnHeaderHeight = columnHeader->sizeHint().height();
            tableHeight += columnHeaderHeight;
        }
        
        if (tableWidget->verticalHeader()->isHidden() == false) {
            QHeaderView* rowHeader = tableWidget->verticalHeader();
            const int rowHeaderHeight = rowHeader->sizeHint().width();
            tableHeight += rowHeaderHeight;
        }
        
        std::vector<int> columnWidths(numCols, 0);
        std::vector<int> rowHeights(numRows, 0);
        
        for (int iCol = 0; iCol < numCols; iCol++) {
            columnWidths[iCol] = tableWidget->columnWidth(iCol) + cellGap;
        }
        
        for (int jRow = 0; jRow < numRows; jRow++) {
            rowHeights[jRow]= (tableWidget->rowHeight(jRow) + cellGap);
        }

        for (int iCol = 0; iCol < numCols; iCol++) {
            for (int jRow = 0; jRow < numRows; jRow++) {
                QWidget* widget = tableWidget->cellWidget(jRow, iCol);
                if (widget != NULL) {
                    const QSize widgetSizeHint = widget->sizeHint();
                    columnWidths[iCol] = std::max(columnWidths[iCol],
                                                  widgetSizeHint.width());
                    rowHeights[jRow] = std::max(rowHeights[jRow],
                                                  widgetSizeHint.height());
                }
                
                QTableWidgetItem* item = tableWidget->item(jRow, iCol);
                if (item != NULL) {
                    int itemWidth = 0;
                    int itemHeight = 0;
                    if (item->flags() && Qt::ItemIsUserCheckable) {
                        itemWidth += 12;
                    }
                    
                    QFont font = item->font();
                    const QString text = item->text();
                    if (text.isEmpty() == false) {
                        QFont font = item->font();
                        QFontMetrics fontMetrics(font);
                        const int textWidth =  fontMetrics.width(text);
                        const int textHeight = fontMetrics.height();
                        
                        itemWidth += textWidth;
                        itemHeight = std::max(itemHeight,
                                              textHeight);
                    }
                    
                    columnWidths[iCol] = std::max(columnWidths[iCol],
                                                  itemWidth);
                    rowHeights[jRow] = std::max(rowHeights[jRow],
                                                   itemHeight);
                }
            }
        }
    
        for (int iCol = 0; iCol < numCols; iCol++) {
            tableWidth += columnWidths[iCol];
        }
        
        for (int jRow = 0; jRow < numRows; jRow++) {
            tableHeight += (rowHeights[jRow] - 2);
        }
        
        tableSize.setWidth(tableWidth);
        tableSize.setHeight(tableHeight);
    }
    
    return tableSize;
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
 * Set the tool tip and status tip for an action.
 * 
 * @param action
 *    Action that has its tool and status tip set.
 * @param text
 *    Text for the tool and status tip.
 */
void
WuQtUtilities::setToolTipAndStatusTip(QAction* action,
                                      const QString& text)
{
    action->setToolTip(text);
    action->setStatusTip(text);
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
    QPixmap pixmap;
    
    const bool valid = WuQtUtilities::loadPixmap(filename, 
                                                 pixmap);
    if (valid) {
        iconOut.addPixmap(pixmap);
    }
    
    return valid;
}

/**
 * Load an icon.
 * @param filename
 *    Name of file containing the icon.
 * @return Pointer to icon (call must delete it) or NULL
 *    if there was a failure to load the icon.
 */
QIcon* 
WuQtUtilities::loadIcon(const QString& filename)
{
    QPixmap pixmap;
    
    const bool valid = WuQtUtilities::loadPixmap(filename, 
                                                 pixmap);
    
    QIcon* icon = NULL;
    if (valid) {
        icon = new QIcon(pixmap);
    }
    return icon;
}


/**
 * Load an pixmap.  
 * 
 * @param filename
 *    Name of file (or resource) containing the pixmap.
 * @param pixmapOut
 *    Output that will contain the desired pixmap.
 * @return
 *    True if the pixmap is valid, else false.
 */
bool 
WuQtUtilities::loadPixmap(const QString& filename,
                          QPixmap& pixmapOut)
{
    bool valid = pixmapOut.load(filename);
    
    if (valid == false) {
        QString msg = "Failed to load Pixmap \"" + filename + "\".";
        CaretLogSevere(msg);
    }
    else if ((pixmapOut.width() <= 0) || (pixmapOut.height() <= 0)) {
        QString msg = "Pixmap \"" + filename + "\" has invalid size.";
        CaretLogSevere(msg);
        valid = false;
    }
    
    return valid;
}

/**
 * Get the maximum height from the given widgets.
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
 * @return
 *    Maximum height of the widgets.
 */
int
WuQtUtilities::getMaximumWidgetHeight(QWidget* w1,
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
    
    return maxHeight;
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
    const int maxHeight = getMaximumWidgetHeight(w1,
                                                 w2,
                                                 w3,
                                                 w4,
                                                 w5,
                                                 w6,
                                                 w7,
                                                 w8,
                                                 w9,
                                                 w10);
    if (maxHeight > 0) {
        w1->setFixedHeight(maxHeight);
        w2->setFixedHeight(maxHeight);
        if (w3  != NULL) w3->setFixedHeight(maxHeight);
        if (w4  != NULL) w4->setFixedHeight(maxHeight);
        if (w5  != NULL) w5->setFixedHeight(maxHeight);
        if (w6  != NULL) w6->setFixedHeight(maxHeight);
        if (w7  != NULL) w7->setFixedHeight(maxHeight);
        if (w8  != NULL) w8->setFixedHeight(maxHeight);
        if (w9  != NULL) w9->setFixedHeight(maxHeight);
        if (w10 != NULL) w10->setFixedHeight(maxHeight);
    }
}

/**
 * Find the widget with the maximum width in its
 * size hint.  Apply this width to all of the widgets.
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
WuQtUtilities::matchWidgetWidths(QWidget* w1,
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
    
    int maxWidth = 0;
    const int num = widgets.size();
    for (int i = 0; i < num; i++) {
        const int w = widgets[i]->sizeHint().width();
        if (w > maxWidth) {
            maxWidth = w;
        }
    }
    
    if (maxWidth > 0) {
        for (int i = 0; i < num; i++) {
            widgets[i]->setFixedWidth(maxWidth);
        }
    }
}

/**
 * Set the margins and spacing for a layout.
 * @param layout
 *     Layout that has margins and spacings set.
 * @param spacing
 *     Spacing between widgets within layout.
 * @param contentsMargin
 *     Margin around the layout.
 */
void 
WuQtUtilities::setLayoutSpacingAndMargins(QLayout* layout,
                                const int spacing,
                                const int contentsMargin)
{
    layout->setSpacing(spacing);
    layout->setContentsMargins(contentsMargin,
                               contentsMargin,
                               contentsMargin,
                               contentsMargin);
}

/**
 * Set the content margins around a layout.
 *
 * @param contentsMargin
 *    Margin around the layout.
 */
void
WuQtUtilities::setLayoutMargins(QLayout* layout,
                             const int contentsMargin)
{
    layout->setContentsMargins(contentsMargin,
                               contentsMargin,
                               contentsMargin,
                               contentsMargin);
}


/**
 * @return The minimum size (width/height) of all screens.
 */
QSize 
WuQtUtilities::getMinimumScreenSize()
{
    int minWidth  = std::numeric_limits<int>::max();
    int minHeight = std::numeric_limits<int>::max();
    
    QDesktopWidget* dw = QApplication::desktop();
    const int numScreens = dw->screenCount();
    for (int i = 0; i < numScreens; i++) {
        const QRect rect = dw->availableGeometry(i);
        const int w = rect.width();
        const int h = rect.height();
        
        minWidth = std::min(minWidth, w);
        minHeight = std::min(minHeight, h);
    }
    
    const QSize size(minWidth, minHeight);
    return size;
}

/**
 * Is the user's display small?  This is loosely
 * defined as a vertical resolution of 800 or less.
 * @return true if resolution is 800 or less, 
 * else false.
 */
bool 
WuQtUtilities::isSmallDisplay()
{
    QDesktopWidget* dw = QApplication::desktop();
    QRect screenRect = dw->screenGeometry();
    const int verticalSize = screenRect.height();
    if (verticalSize <= 800) {
        return true;
    }
    
    return false;
}

/**
 * Get a String containing information about a layout' content.
 * @param layout
 *    The layout
 * @return 
 *    String with info.
 */
QString
WuQtUtilities::getLayoutContentDescription(QLayout* layout)
{
    QString s;
    s.reserve(25000);
    
    s += ("Layout  type : "
          + QString(typeid(*layout).name())
          + "\n");
    
    const int itemCount = layout->count();
    for (int32_t i = 0; i < itemCount; i++) {
        s += "   ";
        QLayoutItem* layoutItem = layout->itemAt(i);
        QLayout* layout = layoutItem->layout();
        if (layout != NULL) {
            s += QString(typeid(*layout).name());
        }
        QWidget* widget = layoutItem->widget();
        if (widget != NULL) {
            s += QString(typeid(*widget).name());
        }
        QSpacerItem* spacerItem = layoutItem->spacerItem();
        if (spacerItem != NULL) {
            s += "QSpacerItem";
        }
    }
    return s;
}

///**
// * Play a sound file.  The sound file MUST be in the distribution's
// * "resources/sounds" directory.
// *
// * Note that sound files, as of Qt 4.8, do not support Qt's resource
// * system.
// *
// * @param soundFileName
// *    Name of sound file (with no path, just the filename).
// */
//void
//WuQtUtilities::playSound(const QString& soundFileName)
//{
//    const QString workbenchDir = SystemUtilities::getWorkbenchHome();
//    const QString soundFilePath = (workbenchDir
//                                   + "/../resources/sounds/"
//                                   + soundFileName);
//    
//    if (QFile::exists(soundFilePath)) {
//        QSound::play(soundFilePath);
//    }
//    else {
//        CaretLogSevere("Sound file \""
//                       + soundFilePath
//                       + "\" does not exist.");
//    }
//}

/**
 * Create the text for a tooltip so that long lines are
 * wrapped and the tooltip is not one giant line
 * that is the width of the display.
 *
 * This is accomplished by placing the text into a
 * QTextDocument and then retrieving the text with
 * HTML formatting.
 *
 * @param tooltipText
 *    Text for the tooltip.
 * @return
 *    Text reformatted for display in a tool tip.
 */
QString
WuQtUtilities::createWordWrappedToolTipText(const QString& tooltipText)
{
    if (tooltipText.isEmpty()) {
        return "";
    }
    
    QTextDocument textDocument(tooltipText);
    QString html = textDocument.toHtml();
    return html;
}


/**
 * Set the text for a tooltip so that long lines are
 * wrapped and the tooltip is not one giant line
 * that is the width of the display.
 *
 * This is accomplished by placing the text into a
 * QTextDocument and then retrieving the text with
 * HTML formatting.
 *
 * @param widget
 *    Widget on which tooltip is set.
 * @param tooltipText
 *    Text for the widget's tooltip.
 */
void
WuQtUtilities::setWordWrappedToolTip(QWidget* widget,
                                     const QString& tooltipText)
{
    widget->setToolTip(createWordWrappedToolTipText(tooltipText));
}

/**
 * Set the text for a tooltip so that long lines are
 * wrapped and the tooltip is not one giant line
 * that is the width of the display.
 *
 * This is accomplished by placing the text into a
 * QTextDocument and then retrieving the text with
 * HTML formatting.
 *
 * @param action
 *    Action on which tooltip is set.
 * @param tooltipText
 *    Text for the widget's tooltip.
 */
void
WuQtUtilities::setWordWrappedToolTip(QAction* action,
                                     const QString& tooltipText)
{
    action->setToolTip(createWordWrappedToolTipText(tooltipText));
}

/**
 * Convert a Qt::CheckState to a boolean value.
 *
 * @param checkState
 *    The check state value.
 * @return 
 *    true if checked or partially checked, else false.
 */
bool
WuQtUtilities::checkStateToBool(const Qt::CheckState checkState)
{
    if (checkState == Qt::Unchecked) {
        return false;
    }
    
    return true;
}

/**
 * Convert a boolean value to a Qt::CheckState
 *
 * @param value
 *    The boolean value.
 * @return
 *    Check state indicating checked or not checked.
 */
Qt::CheckState
WuQtUtilities::boolToCheckState(const bool value)
{
    if (value) {
        return Qt::Checked;
    }
    return Qt::Unchecked;
}

/**
 * Create a pixmap with the given color.
 *
 * @param widget
 *    Widget that will contain pixmap.  It used for getting the widget's
 *    foreground and background colors.
 * @param pixmapWidth
 *    Width for the pixmap.
 * @param pixmapHeight
 *    Height for the pixmap.
 * @param caretColor
 *    The Caret Color Enum value.
 * @param rgba
 *    RGBA color for the pixmap.  If the alpha component is zero, a
 *    outline box with an 'X' symbol is drawn using the widget's
 *    foreground color.
 * @param outlineFlag
 *    If true, drawn an outline with the given color and the background
 *    using the widget's background color.
 * @return
 *    The pixmap.
 */
QPixmap
WuQtUtilities::createCaretColorEnumPixmap(const QWidget* widget,
                                          const int32_t  pixmapWidth,
                                          const int32_t  pixmapHeight,
                                          const CaretColorEnum::Enum caretColor,
                                          const float    customColorRGBA[4],
                                          const bool     outlineFlag)
{
    bool noneColorFlag      = false;
    bool validColorFlag     = false;
    float colorRGBA[4];
    
    switch (caretColor) {
        case CaretColorEnum::NONE:
            noneColorFlag = true;
            break;
        case CaretColorEnum::CUSTOM:
            if (customColorRGBA[3] > 0.0) {
                colorRGBA[0] = customColorRGBA[0];
                colorRGBA[1] = customColorRGBA[1];
                colorRGBA[2] = customColorRGBA[2];
                colorRGBA[3] = customColorRGBA[3];
                validColorFlag = true;
            }
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBAFloat(caretColor,
                                       colorRGBA);
            colorRGBA[3] = 1.0;
            validColorFlag = true;
            break;
    }

    /*
     * Create a small pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    QPixmap pixmap(pixmapWidth,
                   pixmapHeight);
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap,
                                                                                0);
    
    if (noneColorFlag) {
        /*
         * Draw lines (rectangle) around the perimeter of the pixmap
         * and an 'X' in the widget's foreground color.
         */
        for (int32_t i = 0; i < 1; i++) {
            painter->drawRect(i, i,
                             pixmapWidth - 1 - i * 2, pixmapHeight - 1 - i * 2);
        }
        painter->drawLine(0, 0, pixmapWidth - 1, pixmapHeight - 1);
        painter->drawLine(0, pixmapHeight - 1, pixmapWidth - 1, 0);
    }
    else if (validColorFlag) {
        if (outlineFlag) {
            /*
             * Draw lines (rectangle) around the perimeter of the pixmap
             */
            painter->setPen(QColor::fromRgbF(colorRGBA[0],
                                            colorRGBA[1],
                                            colorRGBA[2]));
            for (int32_t i = 0; i < 3; i++) {
                painter->drawRect(i, i,
                                 pixmapWidth - 1 - i * 2, pixmapHeight - 1 - i * 2);
            }
        }
        else {
            /*
             * Fill the pixmap with the RGBA color.
             */
            pixmap.fill(QColor::fromRgbF(colorRGBA[0],
                                     colorRGBA[1],
                                     colorRGBA[2]));
        }
    }
    
    return pixmap;
}

/**
 * Create a painter for the given pixmap that will be placed
 * into the given widget.  The pixmap's background is painted
 * with the widget's background color, the painter's pen is set
 * to the widget's foreground color, and then the painter is
 * returned.
 *
 * Origin of the painter will be in the center with the 
 * coordinates, both X and Y, ranging -100 to 100.
 *
 * @param widget
 *     Widget used for coloring.
 * @param pixmap
 *     The Pixmap must be square (width == height).
 * @param pixmapOptions
 *     Options for creation of pixmap.
 * @return
 *     Shared pointer containing QPainter for drawing to the pixmap.
 */
QSharedPointer<QPainter>
WuQtUtilities::createPixmapWidgetPainterOriginCenter100x100(const QWidget* widget,
                                                            QPixmap& pixmap,
                                                            const uint32_t pixmapOptions)
{
    CaretAssert(pixmap.width() == pixmap.height());
    
    QSharedPointer<QPainter> painter = createPixmapWidgetPainter(widget,
                                                                 pixmap,
                                                                 pixmapOptions);
    
    /*
     * Note: QPainter has its origin at the top left.
     * Using a negative for the Y-scale value will
     * move the origin to the bottom.
     */
    painter->translate(pixmap.width() / 2.0,
                       pixmap.height() / 2.0);
    painter->scale(pixmap.width() / 200.0,
                   -(pixmap.height() / 200.0));
    
    return painter;
}

/**
 * Create a painter for the given pixmap that will be placed
 * into the given widget.  The pixmap's background is painted
 * with the widget's background color, the painter's pen is set
 * to the widget's foreground color, and then the painter is
 * returned.
 *
 * Origin of the painter will be in the center with the
 * coordinates, both X and Y, ranging -100 to 100.
 *
 * @param widget
 *     Widget used for coloring.
 * @param pixmap
 *     The Pixmap must be square (width == height).
 * @param pixmapOptions
 *     Options for creation of pixmap.
 * @return
 *     Shared pointer containing QPainter for drawing to the pixmap.
 */
QSharedPointer<QPainter>
WuQtUtilities::createPixmapWidgetPainterOriginCenter(const QWidget* widget,
                                                     QPixmap& pixmap,
                                                     const uint32_t pixmapOptions)
{
    CaretAssert(pixmap.width() == pixmap.height());
    
    QSharedPointer<QPainter> painter = createPixmapWidgetPainterPrivate(widget,
                                                                        pixmap,
                                                                        pixmapOptions);
    
    /*
     * Note: QPainter has its origin at the top left.
     * Using a negative for the Y-scale value will
     * move the origin to the bottom.
     */
    painter->translate(pixmap.width() / 2.0,
                       pixmap.height() / 2.0);
    painter->scale(1.0,
                   -1.0);
    
    return painter;
}

/**
 * Create a painter for the given pixmap that will be placed
 * into the given widget.  The pixmap's background is painted
 * with the widget's background color, the painter's pen is set
 * to the widget's foreground color, and then the painter is
 * returned.
 *
 * Origin of painter will be in the BOTTOM LEFT corner.
 *
 * @param widget
 *     Widget used for coloring.
 * @param pixmap
 *     The Pixmap.
 * @param pixmapOptions
 *     Options for creation of pixmap.
 * @return
 *     Shared pointer containing QPainter for drawing to the pixmap.
 */
QSharedPointer<QPainter>
WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(const QWidget* widget,
                                                         QPixmap& pixmap,
                                                         const uint32_t pixmapOptions)
{
    QSharedPointer<QPainter> painter = createPixmapWidgetPainter(widget,
                                                                 pixmap,
                                                                 pixmapOptions);
    
    /*
     * Note: QPainter has its origin at the top left.
     * Using a negative for the Y-scale value will
     * move the origin to the bottom.
     */
    painter->translate(0.0,
                       pixmap.height() - 1);
    painter->scale(1.0,
                   -1.0);
    
    return painter;
}


/**
 * Create a painter for the given pixmap that will be placed
 * into the given widget.  The pixmap's background is painted
 * with the widget's background color, the painter's pen is set
 * to the widget's foreground color, and then the painter is
 * returned.
 *
 * Origin of painter will be in the TOP LEFT corner.
 *
 * @param widget
 *     Widget used for coloring.
 * @param pixmap
 *     The Pixmap.
 * @param pixmapOptions
 *     Options for creation of pixmap.
 * @return
 *     Shared pointer containing QPainter for drawing to the pixmap.
 */
QSharedPointer<QPainter>
WuQtUtilities::createPixmapWidgetPainter(const QWidget* widget,
                                         QPixmap& pixmap,
                                         const uint32_t pixmapOptions)
{
    return createPixmapWidgetPainterPrivate(widget,
                                            pixmap,
                                            pixmapOptions);
}

/**
 * Create a painter for the given pixmap that will be placed
 * into the given widget.  The pixmap's background is painted
 * with the widget's background color, the painter's pen is set
 * to the widget's foreground color, and then the painter is
 * returned.
 *
 * Origin of painter will be in the TOP LEFT corner.
 *
 * @param widget
 *     Widget used for coloring.
 * @param pixmap
 *     The Pixmap.
 * @param pixmapOptions
 *     Options for creation of the pixmap.
 * @return
 *     Shared pointer containing QPainter for drawing to the pixmap.
 */
QSharedPointer<QPainter>
WuQtUtilities::createPixmapWidgetPainterPrivate(const QWidget* widget,
                                                QPixmap& pixmap,
                                                const uint32_t pixmapOptions)
{
    CaretAssert(widget);
    CaretAssert(pixmap.width() > 0);
    CaretAssert(pixmap.height() > 0);
    
    /*
     * Get the widget's background and foreground color
     */
    const QPalette palette = widget->palette();
    const QPalette::ColorRole backgroundRole = widget->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QPalette::ColorRole foregroundRole = widget->foregroundRole();
    const QBrush foregroundBrush = palette.brush(foregroundRole);
    const QColor foregroundColor = foregroundBrush.color();
    
    const bool transparentBackgroundFlag = (pixmapOptions
                                            & static_cast<uint32_t>(PixMapCreationOptions::TransparentBackground));
    if (transparentBackgroundFlag) {
        /*
         * It is not possible to create a pixmap with alpha using Qt.
         * So, create a QImage filled with alpha = 0 and then
         * let the pixmap copy from the QImage.
         */
        QImage image(pixmap.width(),
                     pixmap.height(),
                     QImage::Format_RGBA8888_Premultiplied);
        image.fill(QColor(0, 0, 0, 0));
        const bool succssFlag = pixmap.convertFromImage(image);
        if ( ! succssFlag) {
            CaretLogSevere("Failed to convert image to pixmap");
        }
    }
    
    /*
     * Create a painter and fill the pixmap with
     * the background color
     */
    QSharedPointer<QPainter> painter(new QPainter(&pixmap));
    painter->setRenderHint(QPainter::Antialiasing,
                           true);
    if (transparentBackgroundFlag) {
        painter->setBackgroundMode(Qt::TransparentMode);
    }
    else {
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->fillRect(pixmap.rect(), backgroundColor);
    }
    
    painter->setPen(foregroundColor);
    
    return painter;
}

/**
 * With Qt5, a toolbutton placed into a toolbar uses the
 * background of the toolbar with no border and appears 
 * similar to a label.  Use a stylesheet so that
 * the button appears similar to Qt4.
 *
 * @param toolButton
 *     toolButton ToolButton that needs style updated.
 */
#ifdef CARET_OS_MACOSX
#if QT_VERSION >= 0x050000
void
WuQtUtilities::setToolButtonStyleForQt5Mac(QToolButton* toolButton)
{
    CaretAssert(toolButton);
    
    bool hasMenuFlag = false;
    bool hasCheckableFlag = false;
    QAction* action = toolButton->defaultAction();
    if (action != NULL) {
        if (action->menu() != NULL) {
            hasMenuFlag = true;
        }
        if (action->isCheckable()) {
            hasCheckableFlag = true;
        }
    }
    
    const QPalette palette = toolButton->palette();
    const QPalette::ColorRole backgroundRole = toolButton->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QColor lighterColor    = backgroundColor.lighter(100);
    const QColor darkerColor = backgroundColor.darker(125);
    const QColor slightlyDarkerColor = backgroundColor.darker(115);
    
    /*
     * Use a stylesheet to:
     * (1) Make the background of the button lighter
     * (2) Add a border around the button that is slightly
     *     darker than the background.
     */
    QString toolButtonStyleSheet(" QToolButton { "
                                 "   background: " + lighterColor.name() + "; ");
    if (hasMenuFlag) {
        //        toolButtonStyleSheet.append("   border-style: solid; "
        //                                    "   border-width: 1px; "
        //                                    "   border-color: " + darkerColor.name() + "; "
        //                                    "   padding-top:    6px; "
        //                                    "   padding-bottom: 6px; "
        //                                    "   padding-right:  4px; "
        //                                    "   padding-left:   3px; ");
    }
    else {
        toolButtonStyleSheet.append("   border-style: solid; "
                                    "   border-width: 1px; "
                                    "   border-color: " + darkerColor.name() + "; "
                                    "   padding-top:    2px; "
                                    "   padding-bottom: 2px; "
                                    "   padding-right:  3px; "
                                    "   padding-left:   3px; ");
    }
    toolButtonStyleSheet.append(" } ");
    
    if (hasCheckableFlag) {
        /*
         * Background color when button is "checked"
         */
        toolButtonStyleSheet.append(" QToolButton:checked { "
                                    "   background-color: " + slightlyDarkerColor.name() + "; "
                                    " } ");
    }
    else {
        /*
         * Background color when button is "pressed"
         */
        toolButtonStyleSheet.append(" QToolButton:pressed { "
                                    "   background-color: " + slightlyDarkerColor.name() + "; "
                                    " } ");
    }
    
    toolButton->setStyleSheet(toolButtonStyleSheet);
}
#else
    void WuQtUtilities::setToolButtonStyleForQt5Mac(QToolButton*) { }
#endif
#else
    void WuQtUtilities::setToolButtonStyleForQt5Mac(QToolButton*) { }
#endif



