
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>

#define __BUG_REPORT_DIALOG_DECLARE__
#include "BugReportDialog.h"
#undef __BUG_REPORT_DIALOG_DECLARE__

#include "ApplicationInformation.h"
#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BugReportDialog 
 * \brief Dialog for reporting bugs in Workbench.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent on which this dialog is displayed.
 * @param openGLInformation
 *    Information about OpenGL.
 */
BugReportDialog::BugReportDialog(QWidget* parent,
                                 const AString& openGLInformation)
: WuQDialogNonModal("Report Workbench Bug",
                    parent)
{
    const AString emailAddress("workbench_bugs@brainvis.wustl.edu");
    m_emailAddressURL = ("mailto:"
                         + emailAddress
                         + "?subject=Workbench Bug Report");
    
    m_uploadWebSite = "http://brainvis.wustl.edu/cgi-bin/upload.cgi";

    const AString newLines("\n\n");
    
    AString bugInfo;
    
    bugInfo.appendWithNewLine("");
    
    bugInfo.appendWithNewLine("(1) SUMMARY_OF_PROBLEM"
                              + newLines);
    
    bugInfo.appendWithNewLine("(2) HOW TO REPRODUCE PROBLEM"
                              + newLines);
    
    bugInfo.appendWithNewLine("(3) WHAT HAPPENS"
                              + newLines);
    
    bugInfo.appendWithNewLine("(4) WHAT SHOULD HAPPEN"
                              + newLines);
    
    bugInfo.appendWithNewLine("(5) NAME OF UPDLOADED DATA ZIP FILE (Press \"Upload Data\" button to go to the upload website)"
                              + newLines);
    
    bugInfo.appendWithNewLine("(6) SCREEN CAPTURES - After this information is copied into your email client, "
                              "image captures (File Menu->Capture Image) that help describe the bug may be "
                              "copied into the email message."
                              + newLines);
    
    bugInfo.appendWithNewLine("-----------------------------------");
    bugInfo.appendWithNewLine("DO NOT CHANGE TEXT BELOW THIS LINE:\n");
    
    bugInfo.appendWithNewLine(ApplicationInformation().getAllInformationInString("\n\n"));
    
    bugInfo.appendWithNewLine(openGLInformation);
    
    const AString hcpURL("http://humanconnectome.org/connectome/get-connectome-workbench.html");
    const AString infoMessage = ("<html>You are using Workbench version "
                                 + ApplicationInformation().getVersion()
                                 + ".  Check for a newer release of Workbench at <a href=\""
                                 + hcpURL
                                 + "\">"
                                 + hcpURL
                                 + "</a>."
                                 + "  If there is a newer version of Workbench, please download it and "
                                 + "verify that the bug has not been fixed."
                                 + "<br><br>"
                                 + "Please address each of the numbered items in the text below and then submit your bug report."
                                 + "<br><br>"
                                 + "Data files that cause the bug may be uploaded (as a ZIP file) at "
                                 + "<a href=\"" + m_uploadWebSite + "\">" + m_uploadWebSite + "</a>."
                                 + "  wb_command -zip-spec-file can be used to zip a data set."
                                 + "<br><br>"
                                 + "Email the bug report to: "
                                 + emailAddress
                                 //+ "<a href=\"mailto:" + m_emailAddressURL + "\">" + emailAddress + "</a>"
                                 + "</html>");
    
    QLabel* versionLabel = new QLabel(infoMessage);
    versionLabel->setWordWrap(true);
    versionLabel->setOpenExternalLinks(true);
    
    m_textEdit = new QTextEdit();
    m_textEdit->setMinimumSize(600, 400);
    m_textEdit->setText(bugInfo);
    
    QPushButton* copyToClipboardPushButton = new QPushButton("Copy to Clipboard");
    QObject::connect(copyToClipboardPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToClipboard()));
    
    QPushButton* copyToEmailPushButton = new QPushButton("Copy to Email");
    QObject::connect(copyToEmailPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToEmail()));
    
    QPushButton* uploadWebSitePushButton = new QPushButton("Upload Data");
    QObject::connect(uploadWebSitePushButton, SIGNAL(clicked()),
                     this, SLOT(openUploadWebsite()));
    
    copyToClipboardPushButton->setToolTip("Copies information to the computer's clipboard");
    copyToEmailPushButton->setToolTip("Sends information to the user's email client");
    uploadWebSitePushButton->setToolTip("Display upload website in user's web browser");
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(uploadWebSitePushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(copyToClipboardPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(copyToEmailPushButton);
    buttonsLayout->addStretch();
    
    QWidget* contentWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(contentWidget);
    layout->addWidget(versionLabel, 0);
    layout->addSpacing(8);
    layout->addWidget(m_textEdit, 1000);
    layout->addLayout(buttonsLayout, 0);
    
    setCentralWidget(contentWidget);
    
    setApplyButtonText("");
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
BugReportDialog::~BugReportDialog()
{
}


/**
 * Copy the text to the user's clipboard.
 */
void
BugReportDialog::copyToClipboard()
{
    QApplication::clipboard()->setText(m_textEdit->toPlainText().trimmed(),
                                        QClipboard::Clipboard);
}

/**
 * Copy the text to the user's email client.
 */
void
BugReportDialog::copyToEmail()
{
    const AString mailURL = (m_emailAddressURL
                             + "&body="
                             + m_textEdit->toPlainText().trimmed());
    QUrl url(mailURL);
    QDesktopServices::openUrl(url);
}

/**
 * Update the dialog.
 */
void
BugReportDialog::updateDialog()
{
    /* nothing to do */
}

/**
 * Open the Upload Website in the user's web browser.
 */
void
BugReportDialog::openUploadWebsite()
{
    QDesktopServices::openUrl(QUrl(m_uploadWebSite));
}



