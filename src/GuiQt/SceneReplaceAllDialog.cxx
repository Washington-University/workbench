
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __SCENE_REPLACE_ALL_DIALOG_DECLARE__
#include "SceneReplaceAllDialog.h"
#undef __SCENE_REPLACE_ALL_DIALOG_DECLARE__

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SceneReplaceAllDialog 
 * \brief Dialog for replacing scenes
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
SceneReplaceAllDialog::SceneReplaceAllDialog(const AString& replaceDescription,
                                             QWidget* parent)
: WuQDialogModal("Replace All Scenes",
                 parent)
{
    QLabel* slowLabel = new QLabel(replaceDescription);
    slowLabel->setWordWrap(true);
    
    m_changeSurfaceAnntotationOffsetCheckBox = new QCheckBox("Change offset of all surface annotations to TANGENT");

    QGroupBox* optionsGroupBox = new QGroupBox("Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroupBox);
    optionsLayout->addWidget(m_changeSurfaceAnntotationOffsetCheckBox);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(slowLabel);
    layout->addWidget(optionsGroupBox);
    
    setCentralWidget(widget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
SceneReplaceAllDialog::~SceneReplaceAllDialog()
{
}

/**
 * Is true if the offset for all surface annotations should be changed
 * to TANGENT.
 */
bool
SceneReplaceAllDialog::isChangeSurfaceAnnotationOffsetToOffset() const
{
    return m_changeSurfaceAnntotationOffsetCheckBox->isChecked();
}

/**
 * Called when OK button is clicked.
 */
void
SceneReplaceAllDialog::okButtonClicked()
{
    WuQDialogModal::okButtonClicked();
}

