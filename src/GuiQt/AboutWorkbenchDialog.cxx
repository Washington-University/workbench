
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
    
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
AboutWorkbenchDialog::~AboutWorkbenchDialog()
{
    
}

AboutWorkbenchDialog::ModalDialogUserButtonResult
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


