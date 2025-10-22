
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

#define __WU_Q_MESSAGE_BOX_TWO_DECLARE__
#include "WuQMessageBoxTwo.h"
#undef __WU_Q_MESSAGE_BOX_TWO_DECLARE__

#include <iostream>

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::WuQMessageBoxTwo 
 * \brief Similar to QMessageBox but may put text in a scrollable region
 * \ingroup GuiQt
 */

/**
 * @return A button mask for use with the dialog methods
 * @param button1
 *    First standard button for dialog
 * @param button2
 *    Optional second standard button for dialog
 * @param button3
 *    Optional third standard button for dialog
 * @param button4
 *    Optional four standard button for dialog
 */
int32_t
WuQMessageBoxTwo::createButtonMask(const StandardButton button1,
                                   const StandardButton button2,
                                   const StandardButton button3,
                                   const StandardButton button4)
{
    const int32_t mask(static_cast<int32_t>(button1)
                       | static_cast<int32_t>(button2)
                       | static_cast<int32_t>(button3)
                       | static_cast<int32_t>(button4));
    return mask;
}


/**
 * Displays a critical message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons displayed
 * @param defaultButton
 *    Default button
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::critical(QWidget *parent,
                           const QString &title,
                           const QString &text,
                           const int32_t buttonMask,
                           StandardButton defaultButton)
{
    return runStaticDialog(parent,
                           IconType::Critical,
                           title,
                           text,
                           buttonMask,
                           defaultButton);
}

/**
 * Displays a critical message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 */
void
WuQMessageBoxTwo::criticalOk(QWidget *parent,
                             const QString &title,
                             const QString &text)
{
    WuQMessageBoxTwo::critical(parent,
                               title,
                               text,
                               createButtonMask(StandardButton::Ok),
                               StandardButton::Ok);
}

/**
 * Displays a warning message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons displayed
 * @param defaultButton
 *    Default button
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::warning(QWidget *parent,
                           const QString &title,
                           const QString &text,
                           const int32_t buttonMask,
                           StandardButton defaultButton)
{
    return runStaticDialog(parent,
                           IconType::Warning,
                           title,
                           text,
                           buttonMask,
                           defaultButton);
}

/**
 * Displays a warning message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 */
void
WuQMessageBoxTwo::warningOk(QWidget *parent,
                                const QString &title,
                                const QString &text)
{
    WuQMessageBoxTwo::warning(parent,
                              title,
                              text,
                              createButtonMask(StandardButton::Ok),
                              StandardButton::Ok);
}

/**
 * Displays a question message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons displayed
 * @param defaultButton
 *    Default button
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::question(QWidget *parent,
                           const QString &title,
                           const QString &text,
                           const int32_t buttonMask,
                           StandardButton defaultButton)
{
    return runStaticDialog(parent,
                           IconType::Question,
                           title,
                           text,
                           buttonMask,
                           defaultButton);
}

/**
 * Displays an information message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons displayed
 * @param defaultButton
 *    Default button
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::information(QWidget *parent,
                           const QString &title,
                           const QString &text,
                           const int32_t buttonMask,
                           StandardButton defaultButton)
{
    return runStaticDialog(parent,
                           IconType::Information,
                           title,
                           text,
                           buttonMask,
                           defaultButton);
}

/**
 * Displays a information message box with the given text and title.
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 */
void
WuQMessageBoxTwo::informationOk(QWidget *parent,
                             const QString &title,
                             const QString &text)
{
    WuQMessageBoxTwo::information(parent,
                                  title,
                                  text,
                                  createButtonMask(StandardButton::Ok),
                                  StandardButton::Ok);
}

/**
 * Run one of the static dialogs.
 *
 * @param parent
 *    Parent widget of dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons displayed
 * @param defaultButton
 *    Default button
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::runStaticDialog(QWidget *parent,
                                  const IconType iconType,
                                  const QString &title,
                                  const QString &text,
                                  const int32_t buttonMask,
                                  StandardButton defaultButton)
{
    WuQMessageBoxTwo msgBox(iconType,
                            title,
                            text,
                            buttonMask,
                            parent);
    msgBox.setDefaultButton(defaultButton);
    
    msgBox.exec();
    
    StandardButton clickedButton = msgBox.clickedStandardButton();
    if (clickedButton == StandardButton::NoButton) {
        clickedButton = msgBox.findEscapeButton();
        msgBox.m_clickedStandardButton = clickedButton;
    }
    CaretAssert(msgBox.clickedButton());
    CaretAssert(clickedButton != StandardButton::NoButton);
    
    return clickedButton;
}

/**
 * Constructor.
 * @param icon
 *    Icon displayed in dialog
 * @param title
 *    Title of dialog
 * @param text
 *    Text displayed in dialog
 * @param buttonMask
 *    Mask for buttons
 * @param parent
 *    Parent widget
 * @param f
 *    Window flags
 */
WuQMessageBoxTwo::WuQMessageBoxTwo(const IconType icon,
                                   const QString& title,
                                   const QString& text,
                                   const int32_t buttonMask,
                                   QWidget* parent,
                                   Qt::WindowFlags f)
: QDialog(parent,
          f)
{

    QString iconImageName;
    switch (icon) {
        case IconType::Critical:
            iconImageName = ":/MessageDialog/BrainCriticalImage.png";
            break;
        case IconType::Information:
            iconImageName = ":/MessageDialog/BrainInformationImage.png";
            break;
        case IconType::NoIcon:
            break;
        case IconType::Question:
            iconImageName = ":/MessageDialog/BrainQuestionImage.png";
            break;
        case IconType::Warning:
            iconImageName = ":/MessageDialog/BrainWarningImage.png";
            break;
    }

    QLabel* iconLabel(NULL);
    if ( ! iconImageName.isEmpty()) {
        QPixmap pixmap;
        const bool pixmapValid(WuQtUtilities::loadPixmap(iconImageName,
                                                         pixmap));
        if (pixmapValid) {
            iconLabel = new QLabel();
            iconLabel->setPixmap(pixmap);
        }
    }

    setWindowTitle(title);
    
    /*
     * Label for storing message
     */
    m_textLabel = new QLabel(text);
    m_textLabel->setWordWrap(true);
    m_textLabel->setMinimumWidth(400);
    m_textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    
    const QSize textLabelSize(m_textLabel->sizeHint());
    const int32_t textHeight(textLabelSize.height());
    const int32_t textWidth(textLabelSize.width());
    
    /*
     * Put label in a scroll area when there is
     * a large amount of text.  Otherwise, the
     * dialog may be too large (taller than the
     * screen) and unusable.
     */
    QWidget* textWidget(m_textLabel);
    if ((textWidth > s_scrollWidth)
        || (textHeight > s_scrollHeight)) {
        m_haveScrollAreaFlag = true;
        QScrollArea* scrollArea(new QScrollArea());
        scrollArea->setWidget(m_textLabel);
        textWidget = scrollArea;
    }
    /*
     * Buttons at bottom of dialog
     */
    m_buttonBox = new QDialogButtonBox();
    for (int32_t i = 0; i < 32; i++) {
        const int32_t bit(1 << i);
        if (bit & buttonMask) {
            const StandardButton button(static_cast<StandardButton>(bit));
            addButton(button);
        }
    }
    
    QObject::connect(m_buttonBox, &QDialogButtonBox::clicked,
                     this, &WuQMessageBoxTwo::buttonClicked);
    
    QGridLayout* layout = new QGridLayout(this);
    if (iconLabel != NULL) {
        layout->addWidget(iconLabel, 0, 0, Qt::AlignTop);
    }
    layout->addWidget(textWidget, 0, 1);
    layout->addWidget(m_buttonBox, 1, 0, 1, 2);
}

/**
 * Destructor.
 */
WuQMessageBoxTwo::~WuQMessageBoxTwo()
{
}

/**
 * Add a standard button to the dialog
 * @param button
 *    Button to add
 */
QPushButton*
WuQMessageBoxTwo::addButton(StandardButton button)
{
    QPushButton* pushButton(NULL);
    
    QDialogButtonBox::StandardButton bbButton = toDialogButtonBoxStandardButton(button);
    if (bbButton != QDialogButtonBox::NoButton) {
        pushButton = m_buttonBox->addButton(bbButton);
        
        switch (button) {
            case StandardButton::NoButton:
                break;
            case StandardButton::Yes:
                m_yesPushButton = pushButton;
                break;
            case StandardButton::No:
                m_noPushButton = pushButton;
                break;
            case StandardButton::Cancel:
                m_cancelPushButton = pushButton;
                break;
            case StandardButton::Ok:
                m_okPushButton = pushButton;
                break;
        }
    }

    return pushButton;
}

/**
 * @return Button clicked by user or NULL if no button was clicked
 */
QPushButton*
WuQMessageBoxTwo::clickedButton() const
{
    if (m_clickedPushButton == NULL) {
        /*
         * Use ESC button if there is one
         */
        StandardButton stdButton(findEscapeButton());
        QDialogButtonBox::StandardButton bbStdButton(toDialogButtonBoxStandardButton(stdButton));
        m_clickedPushButton = m_buttonBox->button(bbStdButton);
    }
    return m_clickedPushButton;
}

/**
 * @return Standard button clicked by user or NoButton is no button was clicked
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::clickedStandardButton() const
{
    return m_clickedStandardButton;
}

/**
 * Set's the message box's default button to the given button
 * @param button
 *    The default button
 */
void
WuQMessageBoxTwo::setDefaultButton(const StandardButton button)
{
    QDialogButtonBox::StandardButton bbButton = toDialogButtonBoxStandardButton(button);
    QAbstractButton* defButton = m_buttonBox->button(bbButton);
    if (defButton != NULL) {
        QPushButton* pushButton = dynamic_cast<QPushButton*>(defButton);
        if (pushButton != NULL) {
            pushButton->setDefault(true);
        }
    }
}

/**
 * Set the text displayed in the message box
 * @param text
 *    The text.
 */
void
WuQMessageBoxTwo::setText(const QString& text)
{
    m_textLabel->setText(text);
}


/**
 * @return Size hint for this dialog
 */
QSize
WuQMessageBoxTwo::sizeHint() const
{
    /*
     * The size hint will be the default size
     * of the dialog.
     */
    QSize s(QDialog::sizeHint());
    if (m_haveScrollAreaFlag) {
        if (s.width() > s_scrollWidth) {
            s.setWidth(s_scrollWidth);
        }
        if (s.height() > s_scrollHeight) {
            s.setHeight(s_scrollHeight);
        }
    }
    return s;
}

/**
 * Convert from standard button to a QDialogButtonBox button
 */
QDialogButtonBox::StandardButton
WuQMessageBoxTwo::toDialogButtonBoxStandardButton(const StandardButton button) const
{
    QDialogButtonBox::StandardButton bbButton = QDialogButtonBox::NoButton;
    
    switch (button) {
        case StandardButton::NoButton:
            break;
        case StandardButton::Cancel:
            bbButton = QDialogButtonBox::Cancel;
            break;
        case StandardButton::No:
            bbButton = QDialogButtonBox::No;
            break;
        case StandardButton::Ok:
            bbButton = QDialogButtonBox::Ok;
            break;
        case StandardButton::Yes:
            bbButton = QDialogButtonBox::Yes;
            break;
    }

    return bbButton;
}

/**
 * @return button that should be processed when the ESC button is clicked
 * If there is only one button, it is the button activated when Esc is pressed.
 * If there is a Cancel button, it is the button activated when Esc is pressed.
 * If there is exactly one button having either the Reject role or the the No role, it is the button activated when Esc is pressed.
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::findEscapeButton() const
{
    QDialogButtonBox::StandardButton dialogButton(QDialogButtonBox::NoButton);
    QList<QAbstractButton*> allButtons = m_buttonBox->buttons();
    const int32_t numButtons(allButtons.size());
    if (numButtons == 1) {
        dialogButton = m_buttonBox->standardButton(allButtons[0]);
    }
    else if (numButtons > 1) {
        for (int32_t i = 0; i < numButtons; i++) {
            QDialogButtonBox::StandardButton bi(m_buttonBox->standardButton(allButtons[i]));
            if (bi == QDialogButtonBox::Cancel) {
                dialogButton = bi;
                break;
            }
            else if (bi == QDialogButtonBox::No) {
                dialogButton = bi;
                break;
            }
        }
    }
    
    StandardButton button(fromDialogButtonBoxStandardButton(dialogButton));
    
    return button;
}

/**
 * @return StandardButton corresponding to a QDialogButtonBox::StandardButton
 * @param button
 *    QDialogButtonBox::StandardButton
 */
WuQMessageBoxTwo::StandardButton
WuQMessageBoxTwo::fromDialogButtonBoxStandardButton(QDialogButtonBox::StandardButton button) const
{
    StandardButton stdButton = StandardButton::NoButton;
    
    switch (button) {
        case QDialogButtonBox::NoButton:
            stdButton = StandardButton::NoButton;
            break;
        case QDialogButtonBox::Cancel:
            stdButton = StandardButton::Cancel;
            break;
        case QDialogButtonBox::Yes:
            stdButton = StandardButton::Yes;
            break;
        case QDialogButtonBox::No:
            stdButton = StandardButton::No;
            break;
        case QDialogButtonBox::Ok:
            stdButton = StandardButton::Ok;
            break;
        case QDialogButtonBox::Abort:
        case QDialogButtonBox::Apply:
        case QDialogButtonBox::Close:
        case QDialogButtonBox::Discard:
        case QDialogButtonBox::Help:
        case QDialogButtonBox::Ignore:
        case QDialogButtonBox::NoToAll:
        case QDialogButtonBox::Open:
        case QDialogButtonBox::Reset:
        case QDialogButtonBox::RestoreDefaults:
        case QDialogButtonBox::Retry:
        case QDialogButtonBox::Save:
        case QDialogButtonBox::SaveAll:
        case QDialogButtonBox::YesToAll:
            CaretAssertMessage(0, "Has new standard button been added?");
            break;
    }
    
    return stdButton;
}

/**
 * Called a button is clicked
 * @param button
 *    Button that was clicked
 */
void
WuQMessageBoxTwo::buttonClicked(QAbstractButton *button)
{
    m_clickedPushButton     = NULL;
    m_clickedStandardButton = StandardButton::NoButton;
    
    if (button != NULL) {
        QPushButton* pushButton(dynamic_cast<QPushButton*>(button));
        if (pushButton != NULL) {
            m_clickedPushButton = pushButton;
            
            if (m_clickedPushButton == m_okPushButton) {
                m_clickedStandardButton = StandardButton::Ok;
                accept();
            }
            else if (m_clickedPushButton == m_cancelPushButton) {
                m_clickedStandardButton = StandardButton::Cancel;
                reject();
            }
            else if (m_clickedPushButton == m_yesPushButton) {
                m_clickedStandardButton = StandardButton::Yes;
                accept();
            }
            else if (m_clickedPushButton == m_noPushButton) {
                m_clickedStandardButton = StandardButton::No;
                reject();
            }
            else {
                const QString msg("Unrecognized button with text \""
                                  + pushButton->text()
                                  + "\" was clicked.  Has  a new button been added?");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
            }
        }
    }
    
    if (button != NULL) {
        switch (m_buttonBox->standardButton(button)) {
            case QDialogButtonBox::NoButton:
                break;
            case QDialogButtonBox::Cancel:
                break;
            case QDialogButtonBox::Yes:
                break;
            case QDialogButtonBox::No:
                break;
            case QDialogButtonBox::Ok:
                break;
            case QDialogButtonBox::Abort:
                break;
            case QDialogButtonBox::Apply:
                break;
            case QDialogButtonBox::Close:
                break;
            case QDialogButtonBox::Discard:
                break;
            case QDialogButtonBox::Help:
                break;
            case QDialogButtonBox::Ignore:
                break;
            case QDialogButtonBox::NoToAll:
                break;
            case QDialogButtonBox::Open:
                break;
            case QDialogButtonBox::Reset:
                break;
            case QDialogButtonBox::RestoreDefaults:
                break;
            case QDialogButtonBox::Retry:
                break;
            case QDialogButtonBox::Save:
                break;
            case QDialogButtonBox::SaveAll:
                break;
            case QDialogButtonBox::YesToAll:
                break;
        }
    }
}
