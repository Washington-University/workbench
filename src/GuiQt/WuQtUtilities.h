
#ifndef __WU_QT_UTILITIES_H__
#define __WU_QT_UTILITIES_H__

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

#include <stdint.h>
#include <iosfwd>

#include <QSize>
#include <QSharedPointer>
#include <QString>

#include "CaretColorEnum.h"

class QAction;
class QBoxLayout;
class QDialog;
class QIcon;
class QKeySequence;
class QLayout;
class QObject;
class QPixmap;
class QPushButton;
class QString;
class QTableWidget;
class QToolButton;
class QWidget;

namespace caret {
    
    /**
     * Utilities for use with Qt.
     */
    class WuQtUtilities {
        
    public:
        /*
         * Options for creating pixmaps
         */
        enum PixMapCreationOptions {
            /*
             * Create pixmap with transparent background.
             * Useful for pixmaps used in toolbar toggle buttons so that selection is shaded
             * in the entire button not just around the pixmap
             */
            TransparentBackground = 1
        };
        
        static QToolButton* createToolButtonWithIcon(const QString& text,
                                                     const QString& iconFileName,
                                                     const QString& toolTip,
                                                     QObject* receiver,
                                                     const char* method);
        
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
        
        static QPixmap createCaretColorEnumPixmap(const QWidget* widget,
                                                  const int32_t  pixmapWidth,
                                                  const int32_t  pixmapHeight,
                                                  const CaretColorEnum::Enum caretColor,
                                                  const float    customColorRGBA[4],
                                                  const bool     outlineFlag);
        
        static QSharedPointer<QPainter> createPixmapWidgetPainter(const QWidget* widget,
                                                                  QPixmap& pixmap,
                                                                  const uint32_t pixmapOptions = 0);
        
        static QSharedPointer<QPainter> createPixmapWidgetPainterOriginBottomLeft(const QWidget* widget,
                                                                                  QPixmap& pixmap,
                                                                                  const uint32_t pixmapOptions = 0);
                
        static QSharedPointer<QPainter> createPixmapWidgetPainterOriginCenter(const QWidget* widget,
                                                                              QPixmap& pixmap,
                                                                              const uint32_t pixmapOptions = 0);
        
        static QSharedPointer<QPainter> createPixmapWidgetPainterOriginCenter100x100(const QWidget* widget,
                                                                                     QPixmap& pixmap,
                                                                                     const uint32_t pixmapOptions = 0);
        
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
        
        static void moveAndSizeWindow(QWidget* window,
                                      const int32_t x,
                                      const int32_t y,
                                      const int32_t w,
                                      const int32_t h,
                                      int32_t* xywhOut);
        
        static void resizeWindow(QWidget* window,
                                 const int32_t width,
                                 const int32_t height);
        
        static void limitWindowSizePercentageOfMaximum(QWidget* window,
                                                       const float widthMaximumPercentage,
                                                       const float heightMaximumPercentage);
        
        static int getMaximumWidgetHeight(QWidget* w1,
                                          QWidget* w2,
                                          QWidget* w3 = 0,
                                          QWidget* w4 = 0,
                                          QWidget* w5 = 0,
                                          QWidget* w6 = 0,
                                          QWidget* w7 = 0,
                                          QWidget* w8 = 0,
                                          QWidget* w9 = 0,
                                          QWidget* w10 = 0);
        
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
        
        static void matchWidgetWidths(QWidget* w1,
                                       QWidget* w2,
                                       QWidget* w3 = 0,
                                       QWidget* w4 = 0,
                                       QWidget* w5 = 0,
                                       QWidget* w6 = 0,
                                       QWidget* w7 = 0,
                                       QWidget* w8 = 0,
                                       QWidget* w9 = 0,
                                       QWidget* w10 = 0);
        
        static void setLayoutSpacingAndMargins(QLayout* layout,
                                     const int spacing,
                                     const int contentsMargin);
        
        static void setLayoutMargins(QLayout* layout,
                                     const int contentsMargin);
        
        static QSize estimateTableWidgetSize(QTableWidget* tableWidget);
        
        static QSize getMinimumScreenSize();
        
        static bool isSmallDisplay();
        
        static QString getLayoutContentDescription(QLayout* layout);
        
        //static void playSound(const QString& soundFileName);
        
        static QString createWordWrappedToolTipText(const QString& tooltipText);
        
        static void setWordWrappedToolTip(QWidget* widget,
                                          const QString& tooltipText);
        
        static void setWordWrappedToolTip(QAction* action,
                                          const QString& tooltipText);
        
        static bool checkStateToBool(const Qt::CheckState checkState);
        
        static Qt::CheckState boolToCheckState(const bool value);
        
        static void setToolButtonStyleForQt5Mac(QToolButton* toolButton);
        
    private:
        static QSharedPointer<QPainter> createPixmapWidgetPainterPrivate(const QWidget* widget,
                                                                         QPixmap& pixmap,
                                                                         const uint32_t pixmapOptions = 0);
        
        WuQtUtilities();
        ~WuQtUtilities();
        WuQtUtilities(const WuQtUtilities&);
        WuQtUtilities& operator=(const WuQtUtilities&);
    };
}

#endif // __WU_QT_UTILITIES_H__
