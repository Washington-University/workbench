
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
 * 
 * @param parent
 * @param mode
 *    Mode of the dialog
 * @param saveRestoreStateName
 *    Used to save the state of the widget so that state is restored next time dialog is displayed
 * @param parent
 *    Optional parent dialog
 */
LabelSelectionDialog::LabelSelectionDialog(const Mode mode,
                                           const QString& saveRestoreStateName,
                                           QWidget* parent)
: WuQDialogModal("Choose Label",
                 parent)
{
    LabelSelectionWidget::Mode labelWidgetMode(LabelSelectionWidget::Mode::FILE_AND_MAP);
    switch (mode) {
        case Mode::FILE_AND_MAP:
            labelWidgetMode = LabelSelectionWidget::Mode::FILE_AND_MAP;
            break;
        case Mode::FILE_MAP_AND_LABEL:
            labelWidgetMode = LabelSelectionWidget::Mode::FILE_MAP_AND_LABEL;
            break;
    }
    m_labelSelectionWidget = new LabelSelectionWidget(labelWidgetMode,
                                                      saveRestoreStateName);
    
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

/**
 * @return The selected file
 */
CaretMappableDataFile*
LabelSelectionDialog::getSelectedFile() const
{
    return m_labelSelectionWidget->getSelectedFile();
}


/**
 * @return The selected file name
 */
AString
LabelSelectionDialog::getSelectedFileName() const
{
    return m_labelSelectionWidget->getSelectedFileName();
}

/**
 * @return The selected file name without a path
 */
AString
LabelSelectionDialog::getSelectedFileNameNoPath() const
{
    return m_labelSelectionWidget->getSelectedFileNameNoPath();
}

/**
 * @return The selected map name
 */
AString
LabelSelectionDialog::getSelectedMapName() const
{
    return m_labelSelectionWidget->getSelectedMapName();
}

/**
 * @return The selected map index
 */
int32_t
LabelSelectionDialog::getSelectedMapIndex() const
{
    return m_labelSelectionWidget->getSelectedMapIndex();
}
