
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
#include <QDir>
#include <QImageReader>
#include <QImageWriter>
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
    
    QLabel* hcpWebsiteLabel = new QLabel("<html>"
                                         "<a href=\"http://www.humanconnectome.org\">Visit the Human Connectome Project</a>"
                                         "</html>");
    QFont hcpWebsiteFont(hcpWebsiteLabel->font());
    hcpWebsiteFont.setPointSize(16);
    hcpWebsiteLabel->setFont(hcpWebsiteFont);
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
    ImageFile::getQtSupportedImageFileExtensions(imageReadExtensions,
                                                 imageWriteExtensions);
    informationData.push_back("Qt Readable Images: "
                              + AString::join(imageReadExtensions, ", "));
    informationData.push_back("Qt Writable Images: "
                              + AString::join(imageWriteExtensions, ", "));

    std::vector<AString> clipRectReadableExtensions;
    std::vector<AString> scaledClipRectReadableExtensions;
    std::vector<AString> metaDataReadableWritableExtensions;
    ImageFile::getImageFileQtSupportedOptionExtensions(clipRectReadableExtensions,
                                                       scaledClipRectReadableExtensions,
                                                       metaDataReadableWritableExtensions);
    informationData.push_back("Qt ClipRect (ROI) Readable Images: "
                              + AString::join(clipRectReadableExtensions, ", "));
    informationData.push_back("Qt Scaled ClipRect (ROI) Readable Images: "
                              + AString::join(scaledClipRectReadableExtensions, ", "));
    informationData.push_back("Qt Readable/Writable Metadata Support (Text Key/Value) Images: "
                              + AString::join(metaDataReadableWritableExtensions, ", "));

    AString imageWriteDefaultExtension;
    ImageFile::getWorkbenchSupportedImageFileExtensions(imageReadExtensions,
                                                        imageWriteExtensions,
                                                        imageWriteDefaultExtension);
    informationData.push_back("Workbench Readable Images: "
                              + AString::join(imageReadExtensions, ", "));
    informationData.push_back("Workbench Writable Images: "
                              + AString::join(imageWriteExtensions, ", "));
    informationData.push_back("Default Image Type: "
                              + imageWriteDefaultExtension);

    std::vector<AString> movieReadExtensions;
    DataFileTypeEnum::getQtSupportedMovieFileExtensions(movieReadExtensions);
    informationData.push_back("Qt Readable Movies (QMovie): "
                              + AString::join(movieReadExtensions, ", "));

    /*
     * Show image formats that support "clipRect" which is supposed
     * to mean reading only a portion, defined as a rectangle,
     * of an image.  To test this option, it requires an image file
     * so very small images are created in the temp directory.
     */
    const bool testClipRectSupportFlag(false);
    if (testClipRectSupportFlag) {
        std::vector<AString> clipRectExtensions;
        std::vector<AString> scaledClipRectExtensions;
        std::vector<AString> imageReadExtensions, imageWriteExtensions;
        ImageFile::getQtSupportedImageFileExtensions(imageReadExtensions,
                                                     imageWriteExtensions);
        for (auto ext : imageWriteExtensions) {
            if (std::find(imageReadExtensions.begin(),
                          imageReadExtensions.end(),
                          ext) != imageReadExtensions.end()) {
                QImage image(2, 2, QImage::Format_RGB32);
                QString name(QDir::tempPath() + "/file." + ext);
                QImageWriter writer(name);
                writer.write(image);
                
                QImageReader reader(name);
                if (reader.supportsOption(QImageIOHandler::ClipRect)) {
                    clipRectExtensions.push_back(ext);
                }
                if (reader.supportsOption(QImageIOHandler::ScaledClipRect)) {
                    scaledClipRectExtensions.push_back(ext);
                }
            }
        }
        
        informationData.push_back("Qt ClipRect Readable: "
                                  + AString::join(clipRectExtensions, ", "));
        informationData.push_back("Qt Scaled ClipRect Readable: "
                                  + AString::join(scaledClipRectExtensions, ", "));
    }
    
    informationData.push_back("Library paths:");
    QStringList libPaths(QCoreApplication::libraryPaths());
    QStringListIterator libPathsIter(libPaths);
    while (libPathsIter.hasNext()) {
        informationData.push_back("   " + libPathsIter.next());
    }
    
    informationData.push_back("File and extensions for reading and writing:");
    std::vector<DataFileTypeEnum::Enum> allDataFileTypes;
    uint32_t dataFileTypeOptions(0);
    DataFileTypeEnum::getAllEnums(allDataFileTypes,
                                  dataFileTypeOptions);
    for (const auto dft : allDataFileTypes) {
        const AString typeName("   " + DataFileTypeEnum::toGuiName(dft));
        
        const std::vector<AString> allReadExtensions(DataFileTypeEnum::getAllFileExtensionsForReading(dft));
        const AString readExts(AString::join(allReadExtensions, ", "));
        
        const std::vector<AString> allWriteExtensions(DataFileTypeEnum::getAllFileExtensionsForWriting(dft));
        const AString writeExts(AString::join(allWriteExtensions, ", "));
        
        if (readExts == writeExts) {
            informationData.push_back(typeName
                         + ": "
                         + readExts);
        }
        else {
            informationData.push_back(typeName
                         + " Read: "
                         + readExts);
            informationData.push_back(QString(typeName.length(), ' ')
                         + "Write: "
                         + writeExts);
        }
    }

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


