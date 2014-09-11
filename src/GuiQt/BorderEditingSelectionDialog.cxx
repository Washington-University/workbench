
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __BORDER_EDITING_SELECTION_DIALOG_DECLARE__
#include "BorderEditingSelectionDialog.h"
#undef __BORDER_EDITING_SELECTION_DIALOG_DECLARE__

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BorderEditingSelectionDialog 
 * \brief Dialog for selecting border when borders are editing.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param modeDescription
 *    Description of the mode.
 * @param borderNames
 *    Names of the borders for selection.
 * @param parent
 *    Parent of the dialog.
 */
BorderEditingSelectionDialog::BorderEditingSelectionDialog(const AString& modeDescription,
                                                           const std::vector<AString>& borderNames,
                                                           QWidget* parent)
: WuQDialogModal("Border Editing",
                 parent)
{
    AString modeText(modeDescription
                     + ((borderNames.size() > 1)
                        ? " these borders (ordered by distance): "
                        : " this border: "));
    
    
    const QString toolTipText = ("ERASE AND REPLACE - Distance is the average of:\n"
                                 " * Distance from first segment point to nearest point in border.\n"
                                 " * Distance from last segment point to nearest point in border.\n"
                                 "\n"
                                 "EXTEND - Distance is from first segment point to any point\n"
                                 "in border.\n");
    
    QLabel* modeLabel = new QLabel(modeText);
    
    QPushButton* allOnPushButton = new QPushButton("All On");
    QObject::connect(allOnPushButton, SIGNAL(clicked()),
                     this, SLOT(allOnPushButtonClicked()));
    QPushButton* allOffPushButton = new QPushButton("All Off");
    QObject::connect(allOffPushButton, SIGNAL(clicked()),
                     this, SLOT(allOffPushButtonClicked()));
    QWidget* allOnOffWidget = new QWidget();
    QHBoxLayout* allOnOffLayout = new QHBoxLayout(allOnOffWidget);
    allOnOffLayout->addWidget(allOnPushButton);
    allOnOffLayout->addStrut(10);
    allOnOffLayout->addWidget(allOffPushButton);
    allOnOffLayout->addStretch();
    
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(modeLabel);
    
    for (std::vector<AString>::const_iterator iter = borderNames.begin();
         iter != borderNames.end();
         iter++) {
        QCheckBox* cb = new QCheckBox(*iter);
        cb->setChecked(true);
        cb->setToolTip(toolTipText);
        
        m_borderNameCheckBoxes.push_back(cb);
        layout->addWidget(cb);
    }
    
    setTopBottomAndCentralWidgets(allOnOffWidget,
                                  widget,
                                  NULL,
                                  WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
BorderEditingSelectionDialog::~BorderEditingSelectionDialog()
{
}

/**
 * Called when ALL ON push button is clicked.
 */
void
BorderEditingSelectionDialog::allOnPushButtonClicked()
{
    setAllCheckBoxesChecked(true);
}

/**
 * Called when ALL OFF push button is clicked.
 */
void
BorderEditingSelectionDialog::allOffPushButtonClicked()
{
    setAllCheckBoxesChecked(false);
}

/**
 * Set all checkbox check status.
 *
 * @param status
 *    New checked status.
 */
void
BorderEditingSelectionDialog::setAllCheckBoxesChecked(const bool status)
{
    for (std::vector<QCheckBox*>::iterator iter = m_borderNameCheckBoxes.begin();
         iter != m_borderNameCheckBoxes.end();
         iter++) {
        QCheckBox* cb = *iter;
        cb->setChecked(status);
    }
}

/**
 * Is the border name with the given index selected ?
 * 
 * @param borderNameIndex
 *    Index of the border name.
 * @return
 *    Selection status.
 */
bool
BorderEditingSelectionDialog::isBorderNameSelected(const int32_t borderNameIndex) const
{
    CaretAssertVectorIndex(m_borderNameCheckBoxes,
                           borderNameIndex);
    return m_borderNameCheckBoxes[borderNameIndex]->isChecked();
}

