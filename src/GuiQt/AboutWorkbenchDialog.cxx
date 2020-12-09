
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

#include <QApplication>
#include <QDate>
#include <QDesktopServices>
#include <QLabel>
#include <QStyle>
#include <QUrl>
#include <QVBoxLayout>

#include "ApplicationInformation.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "ImageFile.h"
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
    
    ApplicationInformation appInfo;
    
    QLabel* imageLabel = NULL;
    QPixmap pixmap;
    if (WuQtUtilities::loadPixmap(":/About/hcp-logo.png", pixmap)) {
        imageLabel = new QLabel();
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    }
    
    QLabel* workbenchLabel = new QLabel(appInfo.getNameForGuiLabel());
    QFont workbenchFont = workbenchLabel->font();
    workbenchFont.setBold(true);
    workbenchFont.setPointSize(32);
    workbenchLabel->setFont(workbenchFont);
    
    const QString labelStyle = ("QLabel { "
                                " font: 20px bold "
                                "}");
    QLabel* hcpWebsiteLabel = new QLabel("<html>"
                                         "<bold><a href=\"http://www.humanconnectome.org\">Visit the Human Connectome Project</a></bold>"
                                         "</html>");
    hcpWebsiteLabel->setStyleSheet(labelStyle);
    hcpWebsiteLabel->setAlignment(Qt::AlignCenter);
    QObject::connect(hcpWebsiteLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(websiteLinkActivated(const QString&)));
    
    QLabel* versionLabel = new QLabel("Version "
                                      + appInfo.getVersion());
    
    QLabel* copyrightLabel = new QLabel("Copyright "
                                        + QString::number(QDate::currentDate().year())
                                        + " Washington University");
    
    m_morePushButton   = addUserPushButton("More...",
                                           QDialogButtonBox::NoRole);
    m_openGLPushButton = addUserPushButton("OpenGL...",
                                           QDialogButtonBox::NoRole);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout,
                                              4,
                                              2);
    if (imageLabel != NULL) {
        layout->addWidget(imageLabel, 0, Qt::AlignHCenter);
        layout->addSpacing(15);
    }
    layout->addWidget(workbenchLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(hcpWebsiteLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(versionLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(copyrightLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    
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
    else if (userPushButton == m_morePushButton) {
        displayMoreInformation();
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

void
AboutWorkbenchDialog::displayMoreInformation()
{
    ApplicationInformation appInfo;
    std::vector<AString> informationData;
    appInfo.getAllInformation(informationData);
    
    QString styleName("Undefined");
    QStyle* appStyle = QApplication::style();
    if (appStyle != NULL) {
        styleName = appStyle->objectName();
    }
    informationData.push_back(QString("Style Name: " + styleName));
    
    std::vector<AString> imageReadExtensions, imageWriteExtensions;
    ImageFile::getQtSupportedImageFileExtensions(imageReadExtensions, imageWriteExtensions);
    informationData.push_back("Qt Readable Images: "
                              + AString::join(imageReadExtensions, ", "));
    informationData.push_back("Qt Writable Images: "
                              + AString::join(imageWriteExtensions, ", "));

    ImageFile::getWorkbenchSupportedImageFileExtensions(imageReadExtensions, imageWriteExtensions);
    informationData.push_back("Workbench Readable Images: "
                              + AString::join(imageReadExtensions, ", "));
    informationData.push_back("Workbench Writable Images: "
                              + AString::join(imageWriteExtensions, ", "));

    WuQDataEntryDialog ded("More " + appInfo.getName() + " Information",
                           this,
                           true);
    const int32_t numInfo = static_cast<int32_t>(informationData.size());
    for (int32_t i = 0; i < numInfo; i++) {
        ded.addWidget("", new QLabel(informationData[i]));
    }
    ded.setCancelButtonText("");
    ded.exec();
}

/**
 * Called when a label's hyperlink is selected.
 * @param link
 *   The URL.
 */
void
AboutWorkbenchDialog::websiteLinkActivated(const QString& link)
{
    if (link.isEmpty() == false) {
        QDesktopServices::openUrl(QUrl(link));
    }
}


