#ifndef __WU_QDIALOG_H__
#define __WU_QDIALOG_H__

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

#include <map>

#include <QDialog>
#include <QDialogButtonBox>

#include "AString.h"

class QFocusEvent;
class QHBoxLayout;
class QKeyEvent;
class QMenu;
class QScrollArea;
class QVBoxLayout;

namespace caret  {

    class WuQDialog : public QDialog {
        Q_OBJECT
        
    protected:
        WuQDialog(const AString& dialogTitle,
                  QWidget* parent,
                  Qt::WindowFlags f = 0);
        
    public:
        /**
         *
         */
        enum ScrollAreaStatus {
            SCROLL_AREA_ALWAYS,
            SCROLL_AREA_AS_NEEDED,
            SCROLL_AREA_NEVER
        };
        
        virtual ~WuQDialog();
        
        void setCentralWidget(QWidget* centralWidget,
                              const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus);
        
        void setTopBottomAndCentralWidgets(QWidget* topWidget,
                              QWidget* centralWidget,
                                           QWidget* bottomWidget,
                                           const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus);
        
        void setStandardButtonText(QDialogButtonBox::StandardButton button,
                                   const AString& text);
        
        virtual QPushButton* addUserPushButton(const AString& text,
                                               const QDialogButtonBox::ButtonRole buttonRole) = 0;
        
        void setDeleteWhenClosed(bool deleteFlag);
        
        void setAutoDefaultButtonProcessing(bool enabled);
        
        void disableAutoDefaultForAllPushButtons();
        
        void addWidgetToLeftOfButtons(QWidget* widget);
        
        static void beep();
        
        static void showWaitCursor();
        
        static void showNormalCursor();
        
        static void adjustSizeOfDialogWithScrollArea(QDialog* dialog,
                                                     QScrollArea* scrollArea);
        
        virtual QSize sizeHint() const;

    public slots:

        virtual bool close();
        
    protected slots:

        void slotMenuCaptureImageOfWindowToClipboard();
        
        void slotCaptureImageAfterTimeOut();
        
    protected:
        QDialogButtonBox* getDialogButtonBox();
        
        void addImageCaptureToMenu(QMenu* menu);
        
        virtual void keyPressEvent(QKeyEvent* e);
        
        virtual void contextMenuEvent(QContextMenuEvent*);
        
        virtual void focusInEvent(QFocusEvent* event);
        
        virtual void helpButtonClicked();
        
        virtual void focusGained();
        
        virtual void showEvent(QShowEvent* event);
        
        void setDialogSizeHint(const int32_t width,
                               const int32_t height);
        
    private:
        void setTopBottomAndCentralWidgetsInternal(QWidget* topWidget,
                                                   QWidget* centralWidget,
                                                   QWidget* bottomWidget,
                                                   const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus);
        
        QVBoxLayout* userWidgetLayout;
        
        QDialogButtonBox* buttonBox;
        
        QHBoxLayout* m_layoutLeftOfButtonBox;
        
        bool autoDefaultProcessingEnabledFlag;
        
        bool m_firstTimeInShowMethodFlag;
        
        ScrollAreaStatus m_placeCentralWidgetInScrollAreaStatus;
        
        int32_t m_centralWidgetLayoutIndex;
        
        QWidget* m_topWidget;
        QWidget* m_centralWidget;
        QWidget* m_bottomWidget;
        
        mutable int32_t m_sizeHintWidth;
        mutable int32_t m_sizeHintHeight;
    };

#ifdef __WU_QDIALOG_DECLARE__
#endif // __WU_QDIALOG_DECLARE__
}

#endif // __WU_QDIALOG_H__

