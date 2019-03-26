#ifndef __WU_QDIALOG_H__
#define __WU_QDIALOG_H__

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
         * Status of scroll area displayed
         */
        enum ScrollAreaStatus {
            SCROLL_AREA_ALWAYS,
            SCROLL_AREA_AS_NEEDED,
            SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ,
            SCROLL_AREA_NEVER
        };

        /**
         * Result of user button pressed.
         */
        enum DialogUserButtonResult {
            /** MODAL accept which means OK pressed and dialog closes */
            RESULT_MODAL_ACCEPT,
            /** MODAL reject which means Cancel pressed and dialog closes */
            RESULT_MODAL_REJECT,
            /** NON-MODAL close dialog. */
            RESULT_NON_MODAL_CLOSE,
            /** none which means no action is taken and dialog remains open */
            RESULT_NONE
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
        
        void setStandardButtonEnabled(QDialogButtonBox::StandardButton button,
                                      const bool enabled);
        
        QPushButton* addUserPushButton(const AString& text,
                                       const QDialogButtonBox::ButtonRole buttonRole);
        
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

        void setSizeOfDialogWhenDisplayed(const QSize& size);
        
    public slots:

        virtual bool close();
        
    protected slots:

        void slotMenuCaptureImageOfWindowToClipboard();
        
        void slotCaptureImageAfterTimeOut();
        
    protected:
        QDialogButtonBox* getDialogButtonBox();
        
        virtual DialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
        void addImageCaptureToMenu(QMenu* menu);
        
        virtual void okButtonClicked();
        
        virtual void cancelButtonClicked();
        
        virtual void applyButtonClicked();
        
        virtual void closeButtonClicked();
        
        virtual void keyPressEvent(QKeyEvent* e);
        
        virtual void contextMenuEvent(QContextMenuEvent*);
        
        virtual void focusInEvent(QFocusEvent* event);
        
        virtual void helpButtonClicked();
        
        virtual void focusGained();
        
        virtual void showEvent(QShowEvent* event);
        
    private slots:
            void clicked(QAbstractButton* button);
        
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

