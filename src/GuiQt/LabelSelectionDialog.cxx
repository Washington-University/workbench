
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __LABEL_SELECTION_DIALOG_DECLARE__
#include "LabelSelectionDialog.h"
#undef __LABEL_SELECTION_DIALOG_DECLARE__

#include <QVBoxLayout>

#include "CaretAssert.h"
#include "LabelSelectionWidget.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionDialog 
 * \brief Dialog for selecting a label from a label-type file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget
 */
LabelSelectionDialog::LabelSelectionDialog(const QString& saveRestoreStateName,
                                           QWidget* parent)
: WuQDialogModal("Choose Label",
                 parent)
{
    m_labelSelectionWidget = new LabelSelectionWidget(saveRestoreStateName);
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addWidget(m_labelSelectionWidget);
    
    setCentralWidget(widget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
LabelSelectionDialog::~LabelSelectionDialog()
{
}

AString
LabelSelectionDialog::getSelectedLabel() const
{
    return m_labelSelectionWidget->getSelectedLabel();
}

