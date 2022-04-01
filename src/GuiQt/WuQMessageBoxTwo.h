#ifndef __WU_Q_MESSAGE_BOX_TWO_H__
#define __WU_Q_MESSAGE_BOX_TWO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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


#include <cstdint>
#include <memory>

#include <QDialog>
#include <QDialogButtonBox>

class QAbstractButton;
class QLabel;
class QPushButton;

namespace caret {

    class WuQMessageBoxTwo : public QDialog {
        
        Q_OBJECT

    public:
        /** Type of icon displayed in dialog */
        enum class IconType {
            NoIcon,
            Question,
            Information,
            Warning,
            Critical
        };
        
        /** Buttons displayed in the dialog */
        enum class StandardButton : int32_t {
            NoButton = 1,
            Ok       = 2,
            Cancel   = 4,
            Yes      = 8,
            No       = 16
        };
        
        static StandardButton critical(QWidget *parent,
                                       const QString &title,
                                       const QString &text,
                                       const int32_t buttonMask = static_cast<int32_t>(StandardButton::Ok),
                                       StandardButton defaultButton = StandardButton::NoButton);
        
        static StandardButton information(QWidget *parent,
                                          const QString &title,
                                          const QString &text,
                                          const int32_t buttonMask = static_cast<int32_t>(StandardButton::Ok),
                                          StandardButton defaultButton = StandardButton::NoButton);
        
        static StandardButton question(QWidget *parent,
                                       const QString &title,
                                       const QString &text,
                                       const int32_t buttonMask = (static_cast<int32_t>(StandardButton::Yes)
                                                                   | static_cast<int32_t>(StandardButton::No)),
                                       StandardButton defaultButton = StandardButton::NoButton);
        
        static StandardButton warning(QWidget *parent,
                                      const QString &title,
                                      const QString &text,
                                      const int32_t buttonMask = static_cast<int32_t>(StandardButton::Ok),
                                      StandardButton defaultButton = StandardButton::NoButton);
        
        WuQMessageBoxTwo(const IconType icon,
                         const QString& title,
                         const QString& text,
                         const int32_t buttonMask = static_cast<int32_t>(StandardButton::NoButton),
                         QWidget* parent = NULL,
                         Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        
        virtual ~WuQMessageBoxTwo();
        
        WuQMessageBoxTwo(const WuQMessageBoxTwo&) = delete;

        WuQMessageBoxTwo& operator=(const WuQMessageBoxTwo&) = delete;
        
        QPushButton* addButton(StandardButton button);
        
        QPushButton* clickedButton() const;
        
        StandardButton clickedStandardButton() const;
        
        void setDefaultButton(const StandardButton button);
        
        void setText(const QString& text);
        
        virtual QSize sizeHint() const override;

        StandardButton runDialog();

        // ADD_NEW_METHODS_HERE

    private slots:
        void buttonClicked(QAbstractButton *button);
        
    private:
        static StandardButton runStaticDialog(QWidget *parent,
                                              const IconType iconType,
                                              const QString &title,
                                              const QString &text,
                                              const int32_t buttonMask,
                                              StandardButton defaultButton);

        StandardButton findEscapeButton() const;
        
        QDialogButtonBox::StandardButton toDialogButtonBoxStandardButton(const StandardButton button) const;
        
        StandardButton fromDialogButtonBoxStandardButton(QDialogButtonBox::StandardButton button) const;
        
        bool m_haveScrollAreaFlag = false;
        
        QLabel* m_textLabel;
        
        QDialogButtonBox* m_buttonBox;
        
//        StandardButton m_buttonClicked = StandardButton::NoButton;
        
        QPushButton* m_okPushButton = NULL;
        
        QPushButton* m_cancelPushButton = NULL;
        
        QPushButton* m_yesPushButton = NULL;
        
        QPushButton* m_noPushButton = NULL;
        
        mutable QPushButton* m_clickedPushButton = NULL;
        
        StandardButton m_clickedStandardButton = StandardButton::NoButton;
        
        StandardButton m_EscapeButton = StandardButton::NoButton;
        
        static constexpr int32_t s_scrollWidth  = 700;
        static constexpr int32_t s_scrollHeight = 400;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MESSAGE_BOX_TWO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MESSAGE_BOX_TWO_DECLARE__

} // namespace
#endif  //__WU_Q_MESSAGE_BOX_TWO_H__
