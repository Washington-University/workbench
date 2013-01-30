#ifndef __WU_QDIALOG_H__
#define __WU_QDIALOG_H__

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

#include <QDialog>
#include <QDialogButtonBox>

#include "AString.h"

class QFocusEvent;
class QHBoxLayout;
class QKeyEvent;
class QMenu;
class QVBoxLayout;

namespace caret  {

    class WuQDialog : public QDialog {
        Q_OBJECT
        
    protected:
        WuQDialog(const AString& dialogTitle,
                  QWidget* parent,
                  Qt::WindowFlags f = 0);
        
    public:
        virtual ~WuQDialog();
        
        void setCentralWidget(QWidget* centralWidget,
                              const bool allowInsertingIntoScrollArea = true);
        
        void setTopBottomAndCentralWidgets(QWidget* topWidget,
                              QWidget* centralWidget,
                                           QWidget* bottomWidget,
                              const bool allowInsertingIntoScrollArea = true);
        
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
        
    private:
        void setTopBottomAndCentralWidgetsInternal(QWidget* topWidget,
                                                   QWidget* centralWidget,
                                                   QWidget* bottomWidget,
                                                   const bool allowInsertingIntoScrollArea);
        
        QList<QWidget*> m_userWidgets;
        
        QVBoxLayout* userWidgetLayout;
        
        QDialogButtonBox* buttonBox;
        
        QHBoxLayout* m_layoutLeftOfButtonBox;
        
        bool autoDefaultProcessingEnabledFlag;
    };

}

#endif // __WU_QDIALOG_H__

