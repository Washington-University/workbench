
#ifndef __WU_QT_UTILITIES_H__
#define __WU_QT_UTILITIES_H__

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iosfwd>

#include <QSize>

class QAction;
class QBoxLayout;
class QDialog;
class QIcon;
class QKeySequence;
class QObject;
class QPushButton;
class QString;
class QWidget;

namespace caret {
    
    /**
     * Utilities for use with Qt.
     */
    class WuQtUtilities {
        
    public:
        static QAction* createAction(const QString& text,
                                     const QString& toolAndStatusTipText,
                                     const QKeySequence& shortcut,
                                     QObject* parent,
                                     QObject* receiver,
                                     const char* method);
        
        static QAction* createAction(const QString& text,
                                     const QString& toolAndStatusTipText,
                                     QObject* parent,
                                     QObject* receiver,
                                     const char* method);

        static QAction* createAction(const QString& text,
                                     const QString& toolAndStatusTipText,
                                     QObject* parent);
        
        static QAction* createAction(const QString& text,
                                     const QString& toolAndStatusTipText,
                                     const QKeySequence& shortcut,
                                     QObject* parent);
        
        static QPushButton* createPushButton(const QString& text,
                                             const QString& toolAndStatusTipText,
                                             QObject* receiver,
                                             const char* method);
                                             
        static QWidget* createVerticalLineWidget();
        
        static QWidget* createHorizontalLineWidget();
        
        static void moveWindowToOffset(QWidget* parentWidget,
                                       QWidget* window,
                                       const int xOffset, 
                                       const int yOffset);
        
        static void setToolTipAndStatusTip(QWidget* widget,
                                           const QString& text);
        
        static void setToolTipAndStatusTip(QAction* action,
                                           const QString& text);
        
        static void sendListOfResourcesToCaretLogger();
        
        static bool loadIcon(const QString& filename,
                             QIcon& iconOut);
        
        static QIcon* loadIcon(const QString& filename);
        
        static bool loadPixmap(const QString& filename,
                               QPixmap& pixmapOut);
        
        static void moveWindowToSideOfParent(QWidget* parent,
                                             QWidget* window);
        
        static void matchWidgetHeights(QWidget* w1,
                                       QWidget* w2,
                                       QWidget* w3 = 0,
                                       QWidget* w4 = 0,
                                       QWidget* w5 = 0,
                                       QWidget* w6 = 0,
                                       QWidget* w7 = 0,
                                       QWidget* w8 = 0,
                                       QWidget* w9 = 0,
                                       QWidget* w10 = 0);
        
        static void setLayoutMargins(QLayout* layout,
                                     const int spacing,
                                     const int margin,
                                     const int contentsMargin);
        
        static QSize getMinimumScreenSize();
        
    private:
        WuQtUtilities();
        ~WuQtUtilities();
        WuQtUtilities(const WuQtUtilities&);
        WuQtUtilities& operator=(const WuQtUtilities&);
    };
}

#endif // __WU_QT_UTILITIES_H__
