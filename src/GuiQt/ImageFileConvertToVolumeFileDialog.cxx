
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_DECLARE__
#include "ImageFileConvertToVolumeFileDialog.h"
#undef __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_DECLARE__

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ControlPointFile.h"
#include "ControlPoint3D.h"
#include "DisplayPropertiesImages.h"
#include "EventBrowserTabGet.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "Matrix4x4.h"
#include "VolumeFile.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ImageFileConvertToVolumeFileDialog 
 * \brief Dialog for converting an image file to a volume file.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ImageFileConvertToVolumeFileDialog::ImageFileConvertToVolumeFileDialog(QWidget* parent,
                                                   const int32_t tabIndex,
                                                   ImageFile* imageFile)
: WuQDialogModal("Convert Image File to Volume File",
                 parent),
m_tabIndex(tabIndex),
m_imageFile(imageFile)
{
    CaretAssert(m_imageFile);
    
    QWidget* volumeWidget = createVolumeSelectionWidget();
    QWidget* controlPointWidget = createControlPointWidget();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(volumeWidget);
    layout->addWidget(controlPointWidget);
    
    setCentralWidget(widget, WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
ImageFileConvertToVolumeFileDialog::~ImageFileConvertToVolumeFileDialog()
{
}

/**
 * @return New instance of the volume selection widget.
 */
QWidget*
ImageFileConvertToVolumeFileDialog::createVolumeSelectionWidget()
{
    FileInformation fileInfo(m_imageFile->getFileName());
    const AString volumeFileName = FileInformation::assembleFileComponents(fileInfo.getAbsolutePath(),
                                                                           fileInfo.getFileNameNoExtension(),
                                                                           DataFileTypeEnum::toFileExtension(DataFileTypeEnum::VOLUME));
    
    QLabel* volumeFileNameLabel = new QLabel("Volume File Name:");
    m_volumeFileNameLineEdit = new QLineEdit;
    m_volumeFileNameLineEdit->setText(volumeFileName);
    
    std::vector<VolumeSliceViewPlaneEnum::Enum> slicePlanes;
    slicePlanes.push_back(VolumeSliceViewPlaneEnum::ALL);
    slicePlanes.push_back(VolumeSliceViewPlaneEnum::AXIAL);
    slicePlanes.push_back(VolumeSliceViewPlaneEnum::CORONAL);
    slicePlanes.push_back(VolumeSliceViewPlaneEnum::PARASAGITTAL);
    
    QLabel* colorConversionLabel = new QLabel("Color Conversion:");
    m_colorConversionComboBox = new QComboBox();
    m_colorConversionComboBox->addItem("Grayscale");
    m_colorConversionComboBox->setItemData(0, ImageFile::CONVERT_TO_VOLUME_COLOR_GRAYSCALE);
    m_colorConversionComboBox->addItem("RGB");
    m_colorConversionComboBox->setItemData(1, ImageFile::CONVERT_TO_VOLUME_COLOR_RGB);
    
    QGroupBox* groupBox = new QGroupBox("Volume Selection");
    QGridLayout* layout = new QGridLayout(groupBox);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    layout->setColumnMinimumWidth(1, 250);
    int row = layout->rowCount();
    layout->addWidget(volumeFileNameLabel, row, 0);
    layout->addWidget(m_volumeFileNameLineEdit, row, 1);
    row++;
    layout->addWidget(colorConversionLabel, row, 0);
    layout->addWidget(m_colorConversionComboBox, row, 1);
    row++;
    
    return groupBox;
}

/**
 * @return New instance of the control point widget.
 */
QWidget*
ImageFileConvertToVolumeFileDialog::createControlPointWidget()
{
    const ControlPointFile* controlPointFile = m_imageFile->getControlPointFile();
    const int32_t numberOfControlPoints = controlPointFile->getNumberOfControlPoints();
    
    int32_t columnCounter = 0;
    const int32_t COLUMN_PIXEL_I       = columnCounter++;
    const int32_t COLUMN_PIXEL_J       = columnCounter++;
    const int32_t COLUMN_SEPARATOR_1   = columnCounter++;
    const int32_t COLUMN_VOLUME_X      = columnCounter++;
    const int32_t COLUMN_VOLUME_Y      = columnCounter++;
    const int32_t COLUMN_VOLUME_Z      = columnCounter++;
    const int32_t COLUMN_SEPARATOR_2   = columnCounter++;
    const int32_t COLUMN_TRANSFORMED_X = columnCounter++;
    const int32_t COLUMN_TRANSFORMED_Y = columnCounter++;
    const int32_t COLUMN_TRANSFORMED_Z = columnCounter++;
    const int32_t COLUMN_SEPARATOR_3   = columnCounter++;
    const int32_t COLUMN_ERROR_X       = columnCounter++;
    const int32_t COLUMN_ERROR_Y       = columnCounter++;
    const int32_t COLUMN_ERROR_Z       = columnCounter++;
    const int32_t COLUMN_ERROR_TOTAL   = columnCounter++;
    
    const int32_t FLOAT_PRECISION = 1;
    
    QGroupBox* widget = new QGroupBox("Control Points");
    QGridLayout* gridLayout = new QGridLayout(widget);
    int32_t row = gridLayout->rowCount();
    
    gridLayout->addWidget(new QLabel("Image"),
                          row, COLUMN_PIXEL_I, 1, 2, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Volume"),
                          row, COLUMN_VOLUME_X, 1, 3, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Transformed"),
                          row, COLUMN_TRANSFORMED_X, 1, 3, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Error"),
                          row, COLUMN_ERROR_X, 1, 4, Qt::AlignHCenter);
    row++;
    
    gridLayout->addWidget(new QLabel("I"),
                          row, COLUMN_PIXEL_I, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("J"),
                          row, COLUMN_PIXEL_J, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("X"),
                          row, COLUMN_VOLUME_X, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Y"),
                          row, COLUMN_VOLUME_Y, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Z"),
                          row, COLUMN_VOLUME_Z, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("X"),
                          row, COLUMN_TRANSFORMED_X, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Y"),
                          row, COLUMN_TRANSFORMED_Y, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Z"),
                          row, COLUMN_TRANSFORMED_Z, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("X"),
                          row, COLUMN_ERROR_X, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Y"),
                          row, COLUMN_ERROR_Y, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Z"),
                          row, COLUMN_ERROR_Z, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Total"),
                          row, COLUMN_ERROR_TOTAL, Qt::AlignHCenter);
    row++;
    
    for (int32_t icp = 0; icp < numberOfControlPoints; icp++) {
        const ControlPoint3D* cp = controlPointFile->getControlPointAtIndex(icp);
        
        float sourceXYZ[3];
        cp->getSourceXYZ(sourceXYZ);
        gridLayout->addWidget(new QLabel(QString::number(sourceXYZ[0], 'f', 0)),
                              row,
                              COLUMN_PIXEL_I,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(sourceXYZ[1], 'f', 0)),
                              row,
                              COLUMN_PIXEL_J,
                              Qt::AlignRight);
        
        float targetXYZ[3];
        cp->getTargetXYZ(targetXYZ);
        gridLayout->addWidget(new QLabel(QString::number(targetXYZ[0], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_VOLUME_X,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(targetXYZ[1], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_VOLUME_Y,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(targetXYZ[2], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_VOLUME_Z,
                              Qt::AlignRight);
        
        float transformedXYZ[3];
        cp->getTransformedXYZ(transformedXYZ);
        gridLayout->addWidget(new QLabel(QString::number(transformedXYZ[0], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_TRANSFORMED_X,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(transformedXYZ[1], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_TRANSFORMED_Y,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(transformedXYZ[2], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_TRANSFORMED_Z,
                              Qt::AlignRight);
        
        float errorXYZTotal[4];
        cp->getErrorMeasurements(errorXYZTotal);
        gridLayout->addWidget(new QLabel(QString::number(errorXYZTotal[0], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_ERROR_X,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(errorXYZTotal[1], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_ERROR_Y,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(errorXYZTotal[2], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_ERROR_Z,
                              Qt::AlignRight);
        gridLayout->addWidget(new QLabel(QString::number(errorXYZTotal[3], 'f', FLOAT_PRECISION)),
                              row,
                              COLUMN_ERROR_TOTAL,
                              Qt::AlignRight);
        
        row++;
    }
    
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, COLUMN_SEPARATOR_1,
                          row, 1);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, COLUMN_SEPARATOR_2,
                          row, 1);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, COLUMN_SEPARATOR_3,
                          row, 1);
    
    return widget;
}


/**
 * Gets called when the OK button is clicked.
 */
void
ImageFileConvertToVolumeFileDialog::okButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    EventBrowserTabGet tabEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabEvent.getPointer());
    BrowserTabContent* tabContent = tabEvent.getBrowserTab();
    ModelWholeBrain* wholeBrainModel = NULL;
    ModelVolume* volumeModel = NULL;
    if (tabContent != NULL) {
        volumeModel     = tabContent->getDisplayedVolumeModel();
        wholeBrainModel = tabContent->getDisplayedWholeBrainModel();
    }
    if ((volumeModel == NULL)
        && (wholeBrainModel == NULL)) {
        WuQMessageBox::errorOk(this,
                               "Conversion must be performed in ALL or VOLUME view.");
        return;
    }
    
    float translation[3];
    tabContent->getTranslation(translation);
    Matrix4x4 rotMatrix = tabContent->getRotationMatrix();
    const float scaling = tabContent->getScaling();
    
    Matrix4x4 sformMatrix;
    sformMatrix.translate(translation[0], translation[1], translation[2]);
    sformMatrix.postmultiply(rotMatrix);
    sformMatrix.scale(scaling, scaling, scaling);
    
    
    CaretAssert(m_imageFile);
   
    std::vector<ControlPoint3D> controlPoints;
    
    const ControlPointFile* controlPointFile = m_imageFile->getControlPointFile();
    const int32_t numberOfControlPoints = controlPointFile->getNumberOfControlPoints();
    for (int32_t icp = 0; icp < numberOfControlPoints; icp++) {
        controlPoints.push_back(*controlPointFile->getControlPointAtIndex(icp));
    }
    
    const int colorConversionIndex = m_colorConversionComboBox->itemData(m_colorConversionComboBox->currentIndex()).toInt();
    const ImageFile::CONVERT_TO_VOLUME_COLOR_MODE colorMode = static_cast<ImageFile::CONVERT_TO_VOLUME_COLOR_MODE>(colorConversionIndex);
    AString errorMessage;
    VolumeFile* volumeFile = m_imageFile->convertToVolumeFile(colorMode,
                                                              brain->getPaletteFile(),
                                                              errorMessage);
    
    if (volumeFile == NULL) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    volumeFile->setFileName(m_volumeFileNameLineEdit->text().trimmed());
    EventManager::get()->sendEvent(EventDataFileAdd(volumeFile).getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    WuQDialogModal::okButtonClicked();
}

