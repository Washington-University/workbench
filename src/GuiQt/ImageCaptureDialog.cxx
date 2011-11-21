
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <limits>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QImageWriter>

#define __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#include "ImageCaptureDialog.h"
#undef __IMAGE_CAPTURE_DIALOG__H__DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "WuQFileDialog.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class ImageCaptureDialog 
 * \brief Dialog for capturing images.
 *
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
ImageCaptureDialog::ImageCaptureDialog(BrainBrowserWindow* parent)
: WuQDialogNonModal("Image Capture",
                    parent)
{
    this->setDeleteWhenClosed(false);

    /*
     * Use Apply button for image capture
     */
    this->setApplyButtonText("Capture");

    /*
     * Image Source
     */
    QLabel* windowLabel = new QLabel("Workbench Window: ");
    this->windowSelectionSpinBox = new QSpinBox();
    this->windowSelectionSpinBox->setRange(1, 1000000);
    this->windowSelectionSpinBox->setSingleStep(1);
    this->windowSelectionSpinBox->setValue(parent->getBrowserWindowIndex() + 1);
    this->windowSelectionSpinBox->setFixedWidth(60);
    
    QGroupBox* imageSourceGroupBox = new QGroupBox("Image Source");
    QGridLayout* imageSourceLayout = new QGridLayout(imageSourceGroupBox);
    imageSourceLayout->addWidget(windowLabel, 0, 0);
    imageSourceLayout->addWidget(this->windowSelectionSpinBox, 0, 1);
    imageSourceLayout->setColumnStretch(0, 0);
    imageSourceLayout->setColumnStretch(1, 0);
    imageSourceLayout->setColumnStretch(1000, 100);
    
    /*
     * Image Options
     */
    this->imageSizeWindowRadioButton = new QRadioButton("Size of Window");
    this->imageSizeCustomRadioButton = new QRadioButton("Custom");
    QButtonGroup* sizeButtonGroup = new QButtonGroup(this);
    sizeButtonGroup->addButton(this->imageSizeWindowRadioButton);
    sizeButtonGroup->addButton(this->imageSizeCustomRadioButton);
    this->imageSizeWindowRadioButton->setChecked(true);
    this->imageSizeCustomXSpinBox = new QSpinBox();
    this->imageSizeCustomXSpinBox->setFixedWidth(80);
    this->imageSizeCustomXSpinBox->setRange(1, 1000000);
    this->imageSizeCustomXSpinBox->setSingleStep(1);
    this->imageSizeCustomXSpinBox->setValue(2560);
    this->imageSizeCustomYSpinBox = new QSpinBox();
    this->imageSizeCustomYSpinBox->setFixedWidth(80);
    this->imageSizeCustomYSpinBox->setRange(1, 1000000);
    this->imageSizeCustomYSpinBox->setSingleStep(1);
    this->imageSizeCustomYSpinBox->setValue(2048);
    QGroupBox* imageOptionsGroupBox = new QGroupBox("Image Options");
    QGridLayout* imageOptionsLayout = new QGridLayout(imageOptionsGroupBox);
    imageOptionsLayout->addWidget(this->imageSizeWindowRadioButton, 0, 0, 1, 3);
    imageOptionsLayout->addWidget(this->imageSizeCustomRadioButton, 1, 0);
    imageOptionsLayout->addWidget(this->imageSizeCustomXSpinBox, 1, 1);
    imageOptionsLayout->addWidget(this->imageSizeCustomYSpinBox, 1, 2);
    imageOptionsLayout->setColumnStretch(0, 0);
    imageOptionsLayout->setColumnStretch(1, 0);
    imageOptionsLayout->setColumnStretch(2, 0);
    imageOptionsLayout->setColumnStretch(1000, 100);
    
    /*
     * Image Destination
     */
    this->copyImageToClipboardCheckBox = new QCheckBox("Copy to Clipboard");
    this->copyImageToClipboardCheckBox->setChecked(true);
    this->saveImageToFileCheckBox = new QCheckBox("Save to File: " );
    this->imageFileNameLineEdit = new QLineEdit();
    this->imageFileNameLineEdit->setText("capture.png");
    QPushButton* fileNameSelectionPushButton = new QPushButton("Choose File...");
    QObject::connect(fileNameSelectionPushButton, SIGNAL(clicked()),
                     this, SLOT(selectImagePushButtonPressed()));
    
    QGroupBox* imageDestinationGroupBox = new QGroupBox("Image Destination");
    QGridLayout* imageDestinationLayout = new QGridLayout(imageDestinationGroupBox);
    imageDestinationLayout->addWidget(this->copyImageToClipboardCheckBox, 0, 0, 1, 3);
    imageDestinationLayout->addWidget(this->saveImageToFileCheckBox, 1, 0);
    imageDestinationLayout->addWidget(this->imageFileNameLineEdit, 1, 1);
    imageDestinationLayout->addWidget(fileNameSelectionPushButton, 1, 2);
        
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(imageSourceGroupBox);
    layout->addWidget(imageOptionsGroupBox);
    layout->addWidget(imageDestinationGroupBox);
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
ImageCaptureDialog::~ImageCaptureDialog()
{
    
}

/**
 * May be called to update the dialog's content.
 */
void 
ImageCaptureDialog::updateDialog()
{
    
}

/**
 * Called when choose file pushbutton is pressed.
 */
void 
ImageCaptureDialog::selectImagePushButtonPressed()
{
    QString filterText = "Image File (";
    
    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    const int numFormats = imageFormats.count();
    for (int i = 0; i < numFormats; i++) {        
        QByteArray format = imageFormats.at(i);
        if (i > 0) {
            filterText += " ";
        }
        filterText.append(("*." + QString(format)));
    }
    filterText += ")";
    AString name = WuQFileDialog::getSaveFileName(this,
                                                  "Choose File Name",
                                                  GuiManager::get()->getBrain()->getCurrentDirectory(),
                                                  filterText,
                                                  &filterText);
    if (name.isEmpty() == false) {
        this->imageFileNameLineEdit->setText(name.trimmed());
    }
}

/**
 * Called when the apply button is pressed.
 */
void ImageCaptureDialog::applyButtonPressed()
{
    const int browserWindowIndex = this->windowSelectionSpinBox->value() - 1;
    
    int32_t imageX = 0;
    int32_t imageY = 0;
    if (this->imageSizeCustomRadioButton->isChecked()) {
        imageX = this->imageSizeCustomXSpinBox->value();
        imageY = this->imageSizeCustomYSpinBox->value();
    }
    QImage image;
    bool valid = GuiManager::get()->captureImageOfBrowserWindowGraphicsArea(browserWindowIndex,
                                                                            imageX,
                                                                            imageY,
                                                                            image);
    
    if (valid == false) {
        WuQMessageBox::errorOk(this, 
                               "Invalid window selected");
        return;
    }
    
    if (this->copyImageToClipboardCheckBox->isChecked()) {
        QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
    }

    if (this->saveImageToFileCheckBox->isChecked()) {
        AString filename = this->imageFileNameLineEdit->text().trimmed();
        FileInformation fileInfo(filename);
        AString ext = fileInfo.getFileExtension();
        
        QImageWriter writer(filename, ext.toAscii());
        if (writer.supportsOption(QImageIOHandler::Quality)) {
            if (ext.compare("png", Qt::CaseInsensitive) == 0) {
                const int quality = 1;
                writer.setQuality(quality);
            }
            else {
                const int quality = 100;
                writer.setQuality(quality);
            }
        }
        if (writer.supportsOption(QImageIOHandler::CompressionRatio)) {
            writer.setCompression(1);
        }
        if (writer.write(image) == false) {
            QString msg("Unable to save: " + filename);
            WuQMessageBox::errorOk(this, msg);
            return;
        }
    }
}


