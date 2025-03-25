
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

#define __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_DECLARE__
#include "WorkbenchInstallationAssistantDialog.h"
#undef __WORKBENCH_INSTALLATION_ASSISTANT_DIALOG_DECLARE__

#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WorkbenchInstallationAssistant.h"

using namespace caret;
    
/**
 * \class caret::WorkbenchInstallationAssistantDialog 
 * \brief Dialog that provides assistance with installation of Workbench
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget
 */
WorkbenchInstallationAssistantDialog::WorkbenchInstallationAssistantDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Installation Assistant");
    
    m_pathTextEdit = new QTextEdit();
    m_pathTextEdit->setReadOnly(true);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pathTextEdit, 100);
    layout->addWidget(buttonBox, 0);

    loadPathInformation();
}

/**
 * Destructor.
 */
WorkbenchInstallationAssistantDialog::~WorkbenchInstallationAssistantDialog()
{
}

/**
 * Load the path information
 */
void
WorkbenchInstallationAssistantDialog::loadPathInformation()
{
    AString text;
    
    WorkbenchInstallationAssistant assistant;
    const FunctionResultString result(assistant.findBinDirectory());
    if (result.isOk()) {
        text.appendWithNewLine("Bin Directory: " + result.getValue());
        
        text.append("\n");
        if (assistant.isBinDirectoryInUsersPath(result.getValue())) {
            text.appendWithNewLine("bin directory is in user's path");
        }
        else {
            text.appendWithNewLine("bin directory is NOT in user's path");
        }
    }
    else {
        text.appendWithNewLine("ERROR: " + result.getErrorMessage());
    }
    
    m_pathTextEdit->setText(text);
}
