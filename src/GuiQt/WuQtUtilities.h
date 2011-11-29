
#ifndef __WU_QT_UTILITIES_H__
#define __WU_QT_UTILITIES_H__

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

#include <iosfwd>

class QAction;
class QBoxLayout;
class QDialog;
class QIcon;
class QKeySequence;
class QObject;
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
        
    private:
        WuQtUtilities();
        ~WuQtUtilities();
        WuQtUtilities(const WuQtUtilities&);
        WuQtUtilities& operator=(const WuQtUtilities&);
    };
}

#endif // __WU_QT_UTILITIES_H__
