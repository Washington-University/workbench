
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

#define __ABOUT_WORKBENCH_DIALOG_DECLARE__
#include "AboutWorkbenchDialog.h"
#undef __ABOUT_WORKBENCH_DIALOG_DECLARE__

#include <QLabel>
#include <QVBoxLayout>

#include "ApplicationInformation.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "WuQDataEntryDialog.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AboutWorkbenchDialog 
 * \brief Dialog that displays information about workbench.
 */

/**
 * Constructor.
 *
 * @param openGLParentWidget
 *    The parent OpenGL Widget for which information is provided.
 */
AboutWorkbenchDialog::AboutWorkbenchDialog(BrainOpenGLWidget* openGLParentWidget)
: WuQDialogModal("About Workbench",
                 openGLParentWidget)
{
    m_openGLParentWidget = openGLParentWidget;
    
    this->setCancelButtonText("");
    
    std::vector<AString> informationData;
    
    ApplicationInformation appInfo;
    appInfo.getAllInformation(informationData);
    
    m_openGLPushButton = addUserPushButton("OpenGL...",
                                           QDialogButtonBox::NoRole);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 
                                    4, 
                                    2);
    
    const int32_t numInfo = static_cast<int32_t>(informationData.size());
    for (int32_t i = 0; i < numInfo; i++) {
        layout->addWidget(new QLabel(informationData[i]));
    }
    
    this->setCentralWidget(widget,
                           WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
AboutWorkbenchDialog::~AboutWorkbenchDialog()
{
    
}

AboutWorkbenchDialog::DialogUserButtonResult
AboutWorkbenchDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_openGLPushButton) {
        displayOpenGLInformation();
    }
    else {
        CaretAssert(0);
    }
    
    return AboutWorkbenchDialog::RESULT_NONE;
}

void
AboutWorkbenchDialog::displayOpenGLInformation()
{
    WuQDataEntryDialog ded("OpenGL Information",
                           this,
                           true);
    ded.addTextEdit("",
                    m_openGLParentWidget->getOpenGLInformation(),
                    true);
    ded.resize(600, 600);
    ded.setCancelButtonText("");
    ded.exec();
}


