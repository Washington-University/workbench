
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

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "Brain.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
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
    
    FileInformation fileInfo(imageFile->getFileName());
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
    

    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    int row = layout->rowCount();
    layout->addWidget(volumeFileNameLabel, row, 0);
    layout->addWidget(m_volumeFileNameLineEdit, row, 1);
    row++;
    layout->addWidget(m_sliceViewPlaneLabel, row, 0);
    layout->addWidget(m_sliceViewPlaneComboBox->getWidget(), row, 1);
    row++;
    
    setCentralWidget(widget, WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
ImageFileConvertToVolumeFileDialog::~ImageFileConvertToVolumeFileDialog()
{
}

/**
 * Gets called when the OK button is clicked.
 */
void
ImageFileConvertToVolumeFileDialog::okButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    CaretAssert(m_imageFile);
   
    Matrix4x4 sformMatrix;
    AString errorMessage;
    VolumeFile* volumeFile = m_imageFile->convertToVolumeFile(sformMatrix,
                                                              brain->getPaletteFile(),
                                                              errorMessage);
    
    if (volumeFile == NULL) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    volumeFile->setFileName(m_volumeFileNameLineEdit->text().trimmed());
    EventManager::get()->sendEvent(EventDataFileAdd(volumeFile).getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    WuQDialogModal::okButtonClicked();
}


