
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __CZI_IMAGE_EXPORT_DIALOG_DECLARE__
#include "CziImageExportDialog.h"
#undef __CZI_IMAGE_EXPORT_DIALOG_DECLARE__

#include "CaretAssert.h"
using namespace caret;

#include <QBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include "CaretFileDialog.h"
#include "CursorDisplayScoped.h"
#include "CziImageFile.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

/**
 * \class caret::CziImageExportDialog 
 * \brief Dialog for exporting CZI file to image file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param exportType
 *   Type of image export
 * @param cziImageFile
 *    The CZI image file
 * @param parent
 *    Parent widget
 */
CziImageExportDialog::CziImageExportDialog(const ExportType exportType,
                                           CziImageFile* cziImageFile,
                                           QWidget* parent)
: WuQDialogModal("Export CZI File to Image File",
                 parent),
m_exportType(exportType),
m_cziImageFile(cziImageFile)
{
    CaretAssert(cziImageFile);
    
    if (s_firstTimeFlag) {
        s_firstTimeFlag = false;
        s_lastImageDimensionValue = GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension();
    }
    
    QPushButton* fileNamePushButton = new QPushButton("Image File...");
    fileNamePushButton->setToolTip("Choose name of image file");
    QObject::connect(fileNamePushButton, &QPushButton::clicked,
                     this, &CziImageExportDialog::fileSelectionButtonClicked);

    m_filenameLineEdit = new QLineEdit();
    m_filenameLineEdit->setMinimumWidth(300);
    m_filenameLineEdit->setReadOnly(true);
    m_filenameLineEdit->setText(s_lastFileName);
    
    m_alphaCheckBox = new QCheckBox("Include Alpha");
    m_alphaCheckBox->setToolTip("Include alpha component in pixels");
    m_alphaCheckBox->setChecked(s_lastAlphaSelectedFlag);
    
    m_matricesCheckBox = new QCheckBox("Include Support for Plane Coordinate Viewing Mode");
    m_matricesCheckBox->setToolTip("Plane coordinates may rotate and/or scale image when viewed");
    
    Vector3D dummyStereotaxicXYZ;
    const bool enableMatricesCheckboxFlag(m_cziImageFile->pixelIndexToStereotaxicXYZ(PixelIndex(0, 0, 0), dummyStereotaxicXYZ));
    m_matricesCheckBox->setEnabled(enableMatricesCheckboxFlag);
    m_matricesCheckBox->setChecked(s_lastMatricesSelectedFlag);
    
    const AString dimensionToolTip("Limit width or height to this value (aspect preserved)");
    m_maximumWidthHeightSpinBox = new QSpinBox();
    m_maximumWidthHeightSpinBox->setRange(1, 99999999);
    m_maximumWidthHeightSpinBox->setValue(s_lastImageDimensionValue); /* DO BEFORE connecting signal */
    m_maximumWidthHeightSpinBox->setToolTip(dimensionToolTip);
    QObject::connect(m_maximumWidthHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &CziImageExportDialog::updateExportImageDimensionsLabel);

    
    m_maximumWidthHeightCheckBox = new QCheckBox("Maximum width or height ");
    m_maximumWidthHeightCheckBox->setToolTip(dimensionToolTip);
    QObject::connect(m_maximumWidthHeightCheckBox, &QCheckBox::clicked,
                     m_maximumWidthHeightSpinBox, &QSpinBox::setEnabled);
    QObject::connect(m_maximumWidthHeightCheckBox, &QCheckBox::clicked,
                     this, &CziImageExportDialog::updateExportImageDimensionsLabel);
    m_maximumWidthHeightCheckBox->setChecked(s_lastImageDimensionSelectedFlag);
    
    m_maximumWidthHeightSpinBox->setEnabled(m_maximumWidthHeightCheckBox->isChecked());
    
    m_cziImageDimensionsLabel = new QLabel();
    if (m_cziImageFile != NULL) {
        m_cziImageDimensionsLabel->setText("CZI Image Width x Height: "
                                           + AString::number(m_cziImageFile->getWidth())
                                           + " x "
                                           + AString::number(m_cziImageFile->getHeight()));
    }
    
    m_exportImageDimensionsLabel = new QLabel();
    
    QBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->addWidget(fileNamePushButton);
    fileLayout->addWidget(m_filenameLineEdit);
    
    QHBoxLayout* dimensionLayout = new QHBoxLayout();
    dimensionLayout->addWidget(m_maximumWidthHeightCheckBox);
    dimensionLayout->addWidget(m_maximumWidthHeightSpinBox);
    dimensionLayout->addStretch();
    
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addLayout(fileLayout);
    layout->addLayout(dimensionLayout);
    layout->addWidget(m_alphaCheckBox);
    layout->addWidget(m_matricesCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(m_cziImageDimensionsLabel);
    layout->addWidget(m_exportImageDimensionsLabel);

    setCentralWidget(w, ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
    updateExportImageDimensionsLabel();
}

/**
 * Destructor.
 */
CziImageExportDialog::~CziImageExportDialog()
{
}

/**
 * Update the dimensions label for the exported image.
 */
void
CziImageExportDialog::updateExportImageDimensionsLabel()
{
    m_exportImageDimensionsLabel->setText("");

    if (m_cziImageFile == NULL) {
        return;
    }
    
    float imageWidth(m_cziImageFile->getWidth());
    float imageHeight(m_cziImageFile->getHeight());
    if ((imageWidth <= 1.0)
        || (imageHeight <= 1.0)) {
        return;
    }

    if (m_maximumWidthHeightCheckBox->isChecked()) {
        const float maxDim(m_maximumWidthHeightSpinBox->value());
        const float aspect(imageHeight / imageWidth);
        if (imageHeight > imageWidth) {
            imageHeight = maxDim;
            imageWidth  = maxDim / aspect;
        }
        else {
            imageWidth  = maxDim;
            imageHeight = maxDim * aspect;
        }
    }

    m_exportImageDimensionsLabel->setText("Export Image Width x Height: "
                                          + AString::number((int64_t)imageWidth)
                                          + " x "
                                          + AString::number((int64_t)imageHeight));
}

/**
 * Called when OK button clicked
 */
void
CziImageExportDialog::okButtonClicked()
{
    if (m_cziImageFile == NULL) {
        WuQMessageBox::errorOk(this, "CZI Image File is invalid");
        return;
    }
    
    /*
     * Negative value for maximum dimension is unlimited
     */
    const int32_t maximumDimension(m_maximumWidthHeightCheckBox->isChecked()
                                   ? m_maximumWidthHeightSpinBox->value()
                                   : -1);
    
    AString filename(m_filenameLineEdit->text().trimmed());
    if (filename.isEmpty()) {
        WuQMessageBox::errorOk(this, "Image filename is invalid");
    }
    AString errorMessage;
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    switch (m_exportType) {
        case ExportType::ANY_IMAGE:
            if (m_cziImageFile->exportToImageFile(filename,
                                                  maximumDimension,
                                                  m_matricesCheckBox->isChecked(),
                                                  m_alphaCheckBox->isChecked(),
                                                  errorMessage)) {
                cursor.restoreCursor();
                WuQMessageBox::informationOk(this,
                                             "Image export complete");
            }
            else {
                cursor.restoreCursor();
                WuQMessageBox::errorOk(this,
                                       errorMessage);
            }
            break;
    }
    
    WuQDialogModal::okButtonClicked();
    
    s_lastFileName                   = filename;
    s_lastAlphaSelectedFlag          = m_alphaCheckBox->isChecked();
    s_lastMatricesSelectedFlag       = m_matricesCheckBox->isChecked();
    s_lastImageDimensionValue        = m_maximumWidthHeightSpinBox->value();
    s_lastImageDimensionSelectedFlag = m_maximumWidthHeightCheckBox->isChecked();
}

/**
 * Called to select image file name
 */
void
CziImageExportDialog::fileSelectionButtonClicked()
{
    QString filename;
    
    switch (m_exportType) {
        case ExportType::ANY_IMAGE:
            filename = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::IMAGE);
            break;
    }

    if ( ! filename.isEmpty()) {
        m_filenameLineEdit->setText(filename);
    }
}

