
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
 */
BugReportDialog::BugReportDialog(QWidget* parent)
: WuQDialogNonModal("Report Workbench Bug",
                    parent)
{
    const AString newLines("\n\n");
    
    AString bugInfo;
    
    bugInfo.appendWithNewLine("Please address each of the numbered items and then submit your bug report:\n");
    
    bugInfo.appendWithNewLine("(1) SUMMARY_OF_PROBLEM"
                              + newLines);
    
    bugInfo.appendWithNewLine("(2) HOW TO REPRODUCE PROBLEM"
                              + newLines);
    
    bugInfo.appendWithNewLine("(3) WHAT HAPPENS"
                              + newLines);
    
    bugInfo.appendWithNewLine("(4) WHAT SHOULD HAPPEN"
                              + newLines);
    
    bugInfo.appendWithNewLine("(5) NAME OF UPDLOADE DATA ZIP FILE"
                              + newLines);
    
    bugInfo.appendWithNewLine("DO NOT CHANGE TEXT BELOW THIS LINE:\n");
    bugInfo.appendWithNewLine(ApplicationInformation().getAllInformationInString("\n\n"));
    
    
    
    
    m_textEdit = new QTextEdit();
    m_textEdit->setMinimumSize(600, 600);
    m_textEdit->setText(bugInfo);
    
    QPushButton* copyToClipboardPushButton = new QPushButton("Copy to Clipboard");
    QObject::connect(copyToClipboardPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToClipboard()));
    
    QPushButton* copyToEmailPushButton = new QPushButton("Copy to Email");
    QObject::connect(copyToEmailPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToEmail()));
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(copyToClipboardPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(copyToEmailPushButton);
    buttonsLayout->addStretch();
    
    QWidget* contentWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(contentWidget);
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


void
BugReportDialog::copyToClipboard()
{
    QApplication::clipboard()->setText(m_textEdit->toPlainText().trimmed(),
                                        QClipboard::Clipboard);
}

void
BugReportDialog::copyToEmail()
{
    const AString mailURL = ("mailto:john@brainvis.wustl.edu"
                             "?subject=Bug Report"
                             "&body=" + m_textEdit->toPlainText().trimmed());
    QUrl url(mailURL);
    QDesktopServices::openUrl(url);
}

void
BugReportDialog::updateDialog()
{
    /* nothing to do */
}



