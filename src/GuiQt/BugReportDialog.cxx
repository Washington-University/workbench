
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
    
    bugInfo.appendWithNewLine("(5) NAME OF UPLOADED DATA ZIP FILE (Press \"Upload Data\" button to go to the upload website)"
                              + newLines);
    
    bugInfo.appendWithNewLine("(6) SCREEN CAPTURES - After this information is copied into your email client, "
                              "image captures (File Menu->Capture Image) that help describe the bug may be "
                              "copied into the email message."
                              + newLines);
    
    bugInfo.appendWithNewLine("------------------------------------------------------------------");
    bugInfo.appendWithNewLine("PLEASE DO NOT CHANGE TEXT BELOW THIS LINE.\n"
                              "It is used to help us understand the context of your reported bug.\n");
    
    bugInfo.appendWithNewLine(ApplicationInformation().getAllInformationInString("\n\n"));
    
    bugInfo.appendWithNewLine(openGLInformation);
    
    const AString hcpURL("http://humanconnectome.org/connectome/get-connectome-workbench.html");
    const AString infoMessage = ("<html>Verify that your bug occurs in the latest version of Workbench: "
                                 "<a href=\""
                                 + hcpURL
                                 + "\">"
                                 + hcpURL
                                 + "</a>.  "
                                 + "You are using version " + ApplicationInformation().getVersion()
                                 + "."
                                 + "<br><br>"
                                 + "Do not submit bug reports for these problems:<ol>"
                                 + "<li>wb_command, if run from the GUI by double-clicking (it will start and immediately "
                                 + "exit).  wb_command must be run inside a terminal window (DOS Prompt on Windows).</li>"
                                 + "<li>wb_view may crash if run with a remote desktop due to XCB and OpenGL problems in "
                                 + "the remote desktop.  Some users have had success with NX and/or VirtualGL.</li>"
                                 + "</ol>"
                                 + "Please address each of the numbered items in the field below."
                                 + "  Use the \"Copy to Email\" button to submit your bug report to "
                                 + emailAddress
                                 + "<br><br>"
                                 + "Data files that cause the bug may be uploaded (as a ZIP file) at "
                                 + "<a href=\"" + m_uploadWebSite + "\">" + m_uploadWebSite + "</a>."
                                 + "  wb_command -zip-spec-file can be used to zip a data set."
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
    
    setCentralWidget(contentWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
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



