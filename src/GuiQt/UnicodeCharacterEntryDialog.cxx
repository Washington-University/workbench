
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __UNICODE_CHARACTER_ENTRY_DIALOG_DECLARE__
#include "UnicodeCharacterEntryDialog.h"
#undef __UNICODE_CHARACTER_ENTRY_DIALOG_DECLARE__

#include <climits>

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QUrl>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UnicodeCharacterEntryDialog 
 * \brief Dialog for entering a unicode character.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
UnicodeCharacterEntryDialog::UnicodeCharacterEntryDialog(QWidget* parent)
: QDialog(parent)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= (Qt::CustomizeWindowHint);  // disables min/max buttons
    setWindowFlags(flags);
    
    setWindowTitle("Edit Annotation Text");
    
    QLabel* unicodeLabel(new QLabel("Unicode value (hexadecimal): "));
    m_unicodeSpinBox = new QSpinBox();
    m_unicodeSpinBox->setMinimumWidth(100);
    m_unicodeSpinBox->setMinimum(0);
    m_unicodeSpinBox->setMaximum(std::numeric_limits<short>::max());
    m_unicodeSpinBox->setDisplayIntegerBase(16);
    QObject::connect(m_unicodeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &UnicodeCharacterEntryDialog::unicodeSpinBoxValueChanged);

    QLabel* characterLabel(new QLabel("Character: "));
    m_unicodeLabel = new QLabel("     ");
    
    QLabel* unicodeMapLabel(new QLabel("<html><a href=\"https://en.wikipedia.org/wiki/List_of_Unicode_characters\">Show Unicode Character Map in Web Browser</a><br>Not all characters supported by Workbench</html>"));
    QObject::connect(unicodeMapLabel, &QLabel::linkActivated,
                     this, &UnicodeCharacterEntryDialog::displayHyperlink);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()),
                     this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));
    
    QGridLayout* gridLayout(new QGridLayout());
    gridLayout->addWidget(unicodeLabel, 0, 0);
    gridLayout->addWidget(m_unicodeSpinBox, 0, 1);
    gridLayout->addWidget(characterLabel, 1, 0);
    gridLayout->addWidget(m_unicodeLabel, 1, 1);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addLayout(gridLayout);
    layout->addSpacing(12);
    layout->addWidget(unicodeMapLabel);
    layout->addWidget(buttonBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    setFixedSize(sizeHint());
    
    m_unicodeSpinBox->setValue(0x00B1);
}

/**
 * Destructor.
 */
UnicodeCharacterEntryDialog::~UnicodeCharacterEntryDialog()
{
}

/**
 * Called when unicode spin box value is changedf
 * @param value
 *    New valule
 */
void
UnicodeCharacterEntryDialog::unicodeSpinBoxValueChanged(int value)
{
    const QChar unicodeChar(value);
    const AString text(unicodeChar);
    m_unicodeLabel->setText(text);
}

/**
 * @return The selected unicode value if OK was pressed else zero
 **/
short
UnicodeCharacterEntryDialog::getUnicodeValue() const
{
    return m_selectedUnicodeValue;
}

/**
 * Closes the dialog.
 *
 * @param resultCode
 */
void
UnicodeCharacterEntryDialog::done(int resultCode)
{
    if (resultCode == QDialog::Accepted) {
        m_selectedUnicodeValue = m_unicodeSpinBox->value();
    }
    else {
    }
    
    QDialog::done(resultCode);
}

/**
 * Called to display hyperlink in user's web browser
 * @param link
 *    URL of webpage
 */
void
UnicodeCharacterEntryDialog::displayHyperlink(const QString& link)
{
    QDesktopServices::openUrl(QUrl(link));
}

