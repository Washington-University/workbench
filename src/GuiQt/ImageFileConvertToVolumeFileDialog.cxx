
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

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ControlPointFile.h"
#include "ControlPoint3D.h"
#include "DisplayPropertiesImages.h"
#include "EnumComboBoxTemplate.h"
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
    
    loadAllControlPoints();
    
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
    
    QLabel* m_sliceViewPlaneLabel = new QLabel("Slice Axis:");
    
    m_sliceViewPlaneComboBox = new EnumComboBoxTemplate(this);
    m_sliceViewPlaneComboBox->setupWithItems<VolumeSliceViewPlaneEnum,VolumeSliceViewPlaneEnum::Enum>(slicePlanes);
    m_sliceViewPlaneComboBox->getComboBox()->setItemText(0, "Current View"); // replaces ALL
    m_sliceViewPlaneComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    WuQtUtilities::setToolTipAndStatusTip(m_sliceViewPlaneComboBox->getWidget(),
                                          "Axis of slice to which image is aligned");
    
    
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
    layout->addWidget(m_sliceViewPlaneLabel, row, 0);
    layout->addWidget(m_sliceViewPlaneComboBox->getWidget(), row, 1);
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
    QGroupBox* widget = new QGroupBox("Control Points");
    QGridLayout* gridLayout = new QGridLayout(widget);
    
    int row = 0;
    gridLayout->addWidget(new QLabel("Pixel I"), row, 0, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Pixel J"), row, 1, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("X"), row, 2, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Y"), row, 3, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Z"), row, 4, Qt::AlignHCenter);
    row++;
    
    const ControlPointFile* controlPointFile = m_imageFile->getControlPointFile();
    const int32_t numberOfControlPoints = controlPointFile->getNumberOfControlPoints();
    
    for (int32_t icp = 0; icp < numberOfControlPoints; icp++) {
        QSpinBox* spinBoxSourceX = new QSpinBox();
        spinBoxSourceX->setRange(0, m_imageFile->getWidth() - 1);
        QSpinBox* spinBoxSourceY = new QSpinBox();
        spinBoxSourceY->setRange(0, m_imageFile->getHeight() - 1);
        
        const double maxValue = 9999999.0;
        QDoubleSpinBox* spinBoxTargetX = new QDoubleSpinBox();
        spinBoxTargetX->setRange(-maxValue, maxValue);
        spinBoxTargetX->setDecimals(3);
        QDoubleSpinBox* spinBoxTargetY = new QDoubleSpinBox();
        spinBoxTargetY->setRange(-maxValue, maxValue);
        spinBoxTargetY->setDecimals(3);
        QDoubleSpinBox* spinBoxTargetZ = new QDoubleSpinBox();
        spinBoxTargetZ->setRange(-maxValue, maxValue);
        spinBoxTargetZ->setDecimals(3);

        m_sourceXSpinBox.push_back(spinBoxSourceX);
        m_sourceYSpinBox.push_back(spinBoxSourceY);
        m_targetXSpinBox.push_back(spinBoxTargetX);
        m_targetYSpinBox.push_back(spinBoxTargetY);
        m_targetZSpinBox.push_back(spinBoxTargetZ);
        
        gridLayout->addWidget(spinBoxSourceX, row, 0);
        gridLayout->addWidget(spinBoxSourceY, row, 1);
        gridLayout->addWidget(spinBoxTargetX, row, 2);
        gridLayout->addWidget(spinBoxTargetY, row, 3);
        gridLayout->addWidget(spinBoxTargetZ, row, 4);
        row++;
    }
    
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
    if (tabContent != NULL) {
        wholeBrainModel = tabContent->getDisplayedWholeBrainModel();
    }
    if (wholeBrainModel == NULL) {
        WuQMessageBox::errorOk(this,
                               "Conversion must be performed in ALL view.");
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
    
    const int32_t numberOfControlPoints = static_cast<int32_t>(m_sourceXSpinBox.size());
    for (int32_t icp = 0; icp < numberOfControlPoints; icp++) {
        const float sourceXYZ[3] = {
            m_sourceXSpinBox[icp]->value(),
            m_sourceYSpinBox[icp]->value(),
            0.0
        };
        const float targetXYZ[3] = {
            m_targetXSpinBox[icp]->value(),
            m_targetYSpinBox[icp]->value(),
            m_targetZSpinBox[icp]->value()
        };
        controlPoints.push_back(ControlPoint3D(sourceXYZ,
                                               targetXYZ));
    }
    
    const int colorConversionIndex = m_colorConversionComboBox->itemData(m_colorConversionComboBox->currentIndex()).toInt();
    const ImageFile::CONVERT_TO_VOLUME_COLOR_MODE colorMode = static_cast<ImageFile::CONVERT_TO_VOLUME_COLOR_MODE>(colorConversionIndex);
    AString errorMessage;
    VolumeFile* volumeFile = m_imageFile->convertToVolumeFile(colorMode,
                                                              controlPoints,
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

/**
 * Load the control points.
 */
void
ImageFileConvertToVolumeFileDialog::loadAllControlPoints()
{
    /*
     * Coronal View
     */
    ControlPoint3D cp1(48, 105, 0, -58, -13, 11);
    ControlPoint3D cp2(140, 106, 0, -12, -13, 13);
    ControlPoint3D cp3(133, 196, 0, -17, -13, 58);

//    /*
//     * Axial View
//     */
//    ControlPoint3D cp1(60, 36, 0, -41, -79, 6);
//    ControlPoint3D cp2(98, 35, 0, -13, -80, 6);
//    ControlPoint3D cp3(97, 151, 0, -14, 4, 6);

    const ControlPointFile* controlPointFile = m_imageFile->getControlPointFile();
    const int32_t numCP = controlPointFile->getNumberOfControlPoints();
    for (int32_t i = 0; i < numCP; i++) {
        loadControlPoint(i,
                         controlPointFile->getControlPointAtIndex(i));
    }
}

/**
 * Load a control point into the spin boxes for the given index.
 *
 * @param index
 *     Index of the spin boxes.
 * @param cp
 *     The control point.
 */
void
ImageFileConvertToVolumeFileDialog::loadControlPoint(const int32_t index,
                                                     const ControlPoint3D* cp)
{
    CaretAssertVectorIndex(m_sourceXSpinBox, index);
    CaretAssertVectorIndex(m_sourceYSpinBox, index);
    CaretAssertVectorIndex(m_targetXSpinBox, index);
    CaretAssertVectorIndex(m_targetYSpinBox, index);
    CaretAssertVectorIndex(m_targetZSpinBox, index);
    
    m_sourceXSpinBox[index]->setValue(cp->getSourceX());
    m_sourceYSpinBox[index]->setValue(cp->getSourceY());
    m_targetXSpinBox[index]->setValue(cp->getTargetX());
    m_targetYSpinBox[index]->setValue(cp->getTargetY());
    m_targetZSpinBox[index]->setValue(cp->getTargetZ());
    
}
