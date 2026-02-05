
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __NAMES_ON_OFF_LINE_EDIT_WIDGET_DECLARE__
#include "NamesOnOffLineEditWidget.h"
#undef __NAMES_ON_OFF_LINE_EDIT_WIDGET_DECLARE__

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::NamesOnOffLineEditWidget 
 * \brief Line Edit with on and off buttons
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget
 */
NamesOnOffLineEditWidget::NamesOnOffLineEditWidget(QWidget* parent)
: QWidget(parent)
{
    const AString nameTT("<html>"
                         "To simplify turning more than one item with the same name on or off, "
                         "enter the names here and the press the On or Off button.  Names must be separated by "
                         "the separator character(s) at the far right of the On and Off buttons.  "
                         "</html>");
    m_namesLineEdit = new QLineEdit();
    m_namesLineEdit->setToolTip(nameTT);
    
    QLabel* separatorLabel(new QLabel("Sep:"));
    m_separatorLineEdit = new QLineEdit();
    m_separatorLineEdit->setFixedWidth(24);
    m_separatorLineEdit->setText(";");
    m_separatorLineEdit->setToolTip("<html>"
                                    "Separator for use when multiple label names are in Names text box"
                                    "</html>");
    
    AString onOffTT("Click button to turn label name(s) in the Names text box ");
    m_onAction = new QAction("Names On");
    m_onAction->setToolTip(onOffTT + "on");
    QObject::connect(m_onAction, &QAction::triggered,
                     this, &NamesOnOffLineEditWidget::onButtonClicked);
    m_offAction = new QAction("Names Off");
    m_offAction->setToolTip(onOffTT + "off");
    QObject::connect(m_offAction, &QAction::triggered,
                     this, &NamesOnOffLineEditWidget::offButtonClicked);
    
    QToolButton* onToolButton(new QToolButton());
    onToolButton->setDefaultAction(m_onAction);

    QToolButton* offToolButton(new QToolButton());
    offToolButton->setDefaultAction(m_offAction);
    
    const int32_t STRETCH_YES(100);
    const int32_t STRETCH_NO(0);
    QHBoxLayout* layout(new QHBoxLayout(this));
    
    const bool altLayoutFlag(true);
    if (altLayoutFlag) {
        QLabel* namesLabel(new QLabel("Names: "));
        layout->setSpacing(4);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->addWidget(onToolButton, STRETCH_NO);
        layout->addWidget(offToolButton, STRETCH_NO);
        layout->addWidget(separatorLabel, STRETCH_NO);
        layout->addWidget(m_separatorLineEdit, STRETCH_NO);
        layout->addWidget(namesLabel, STRETCH_NO);
        layout->addWidget(m_namesLineEdit, STRETCH_YES);
    }
    else {
        QLabel* namesLabel(new QLabel("Names: "));
        layout->setContentsMargins(2, 2, 2, 2);
        layout->addWidget(namesLabel, STRETCH_NO);
        layout->addWidget(m_namesLineEdit, STRETCH_YES);
        layout->addWidget(onToolButton, STRETCH_NO);
        layout->addWidget(offToolButton, STRETCH_NO);
        layout->addWidget(separatorLabel, STRETCH_NO);
        layout->addWidget(m_separatorLineEdit, STRETCH_NO);
    }
}

/**
 * Destructor.
 */
NamesOnOffLineEditWidget::~NamesOnOffLineEditWidget()
{
}

/**
 * Called when On button is clicked
 */
void
NamesOnOffLineEditWidget::onButtonClicked()
{
    const std::vector<AString> names(getNames());
    if ( ! names.empty()) {
        emit namesOnTriggered(names);
    }
}

/**
 * Called when Off button is clicked
 */
void
NamesOnOffLineEditWidget::offButtonClicked()
{
    const std::vector<AString> names(getNames());
    if ( ! names.empty()) {
        emit namesOffTriggered(names);
    }
}

/**
 * @return Names after splitting with separator
 */
std::vector<AString>
NamesOnOffLineEditWidget::getNames() const
{
    QString separator(m_separatorLineEdit->text().trimmed());
    if (separator.isEmpty()) {
        separator = " ";
    }
#if QT_VERSION >= 0x060000
    QStringList namesList(m_namesLineEdit->text().trimmed().split(separator,
                                                                  Qt::SkipEmptyParts,
                                                                  Qt::CaseInsensitive)); /* of separator */
#else
    QStringList namesList(m_namesLineEdit->text().trimmed().split(separator,
                                                                  QString::SkipEmptyParts,
                                                                  Qt::CaseInsensitive)); /* of separator */
#endif
    std::vector<AString> namesOut;
    for (const QString& name : namesList) {
        const AString n = name.trimmed();
        if (n.isNotEmpty()) {
            namesOut.push_back(n);
        }
    }
    
    return namesOut;
}


