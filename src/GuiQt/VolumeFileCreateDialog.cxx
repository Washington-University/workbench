
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __VOLUME_FILE_CREATE_DIALOG_DECLARE__
#include "VolumeFileCreateDialog.h"
#undef __VOLUME_FILE_CREATE_DIALOG_DECLARE__

#include <QComboBox>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CiftiMappableDataFile.h"
#include "CaretVolumeExtension.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "VolumeFile.h"
#include "VolumeFileResampleDialog.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::VolumeFileCreateDialog 
 * \brief Dialog for creating volume file or adding map to volume file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param underlayVolume
 *    Current underlay volume
 * @param parent
 *    The parent dialog
 */
VolumeFileCreateDialog::VolumeFileCreateDialog(const VolumeMappableInterface* underlayVolume,
                                               QWidget* parent)
: WuQDialogModal("Create Volume",
                 parent)
{
    m_blockVoxelEdgeLabelUpdateFlag = true;
    
    if ( ! s_previousVolumeSettingsValid) {
        s_previousVolumeSettings.m_dimensions.clear();
        s_previousVolumeSettings.m_dimensions.push_back(182);
        s_previousVolumeSettings.m_dimensions.push_back(218);
        s_previousVolumeSettings.m_dimensions.push_back(182);
        s_previousVolumeSettings.m_dimensions.push_back(1);
        
        std::vector<float> row1;
        row1.push_back(-1);
        row1.push_back(0);
        row1.push_back(0);
        row1.push_back(90);

        std::vector<float> row2;
        row2.push_back(0);
        row2.push_back(1);
        row2.push_back(0);
        row2.push_back(-126);

        std::vector<float> row3;
        row3.push_back(0);
        row3.push_back(0);
        row3.push_back(1);
        row3.push_back(-72);

        std::vector<float> row4;
        row4.push_back(0);
        row4.push_back(0);
        row4.push_back(0);
        row4.push_back(1);
        
        /*
         * If available, match underlay volume
         */
        if (underlayVolume != NULL) {
            const VolumeFile* vf(dynamic_cast<const VolumeFile*>(underlayVolume));
            if (vf != NULL) {
                std::vector<int64_t> dims;
                vf->getDimensions(dims);
                if (dims.size() >= 4) {
                    s_previousVolumeSettings.m_dimensions.clear();
                    s_previousVolumeSettings.m_dimensions.push_back(dims[0]);
                    s_previousVolumeSettings.m_dimensions.push_back(dims[1]);
                    s_previousVolumeSettings.m_dimensions.push_back(dims[2]);
                    s_previousVolumeSettings.m_dimensions.push_back(1);
                    
                    std::vector<std::vector<float>> sform(vf->getSform());
                    if (sform.size() == 4) {
                        if ((sform[0].size() == 4)
                            && (sform[1].size() == 4)
                            && (sform[2].size() == 4)
                            && (sform[3].size() == 4)) {
                            row1 = sform[0];
                            row2 = sform[1];
                            row3 = sform[2];
                            row4 = sform[3];
                        }
                    }
                }
            }
        }
        s_previousVolumeSettings.m_indexToSpace.clear();
        s_previousVolumeSettings.m_indexToSpace.push_back(row1);
        s_previousVolumeSettings.m_indexToSpace.push_back(row2);
        s_previousVolumeSettings.m_indexToSpace.push_back(row3);
        s_previousVolumeSettings.m_indexToSpace.push_back(row4);
        
        s_previousVolumeSettings.m_volumeType = SubvolumeAttributes::UNKNOWN;
        
        s_previousVolumeSettingsValid = true;
    }
    
    m_volumeFile = NULL;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(createNewVolumeFileWidget());
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    m_blockVoxelEdgeLabelUpdateFlag = false;
    updateVoxelEdgeLabels();
}

/**
 * Destructor.
 */
VolumeFileCreateDialog::~VolumeFileCreateDialog()
{
}

/**
 * @return Create and return the add map to volume file widget
 */
QWidget*
VolumeFileCreateDialog::addMapToVolumeFileWidget()
{
    QWidget* widget = new QWidget();
    return widget;
}

/**
 * @return Create and return the create new volume file widget
 */
QWidget*
VolumeFileCreateDialog::createNewVolumeFileWidget()
{
    const AString defaultFileName = ("File_"
                                     + AString::number(s_fileNameCounter)
                                     + ".nii.gz");
    s_fileNameCounter++;
    
    QLabel* newFileNameLabel = new QLabel("Volume Filename:");
    m_newFileNameLineEdit = new QLineEdit();
    m_newFileNameLineEdit->setText(defaultFileName);
    QPushButton* newFileNamePushButton = new QPushButton("Select...");
    QObject::connect(newFileNamePushButton, SIGNAL(clicked()),
                     this, SLOT(newFileNamePushButtonClicked()));
    
    QLabel* newFileTypeLabel = new QLabel("Volume Type:");
    m_newFileTypeComboBox = new QComboBox();
    m_newFileTypeComboBox->addItem("Choose Type",
                                   SubvolumeAttributes::UNKNOWN);
    m_newFileTypeComboBox->addItem("Functional (Scalars)",
                                   SubvolumeAttributes::FUNCTIONAL);
    m_newFileTypeComboBox->addItem("Label (Names)",
                                   SubvolumeAttributes::LABEL);
    
    QLabel* newFileNumberOfMapsLabel = new QLabel("Number of Maps:");
    m_newFileNumberOfMapsSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, s_maximumNumberOfMaps, 1);
    m_newFileNumberOfMapsSpinBox->setFixedWidth(50);
    QObject::connect(m_newFileNumberOfMapsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(numberOfMapsSpinBoxValueChanged(int)));
    
    for (int32_t iMap = 0; iMap < s_maximumNumberOfMaps; iMap++) {
        const AString mapNumText("Map " + AString::number(iMap + 1) + " ");
        QLabel* label = new QLabel(mapNumText
                                   + "Name");
        m_mapNameLabels.push_back(label);
        
        QLineEdit* lineEdit = new QLineEdit();
        lineEdit->setText(mapNumText);
        m_mapNameLineEdits.push_back(lineEdit);
    }

    QGroupBox* fileGroupBox = new QGroupBox("File");
    QGridLayout* nameTypeLayout = new QGridLayout(fileGroupBox);
    int nameTypeRow = 0;
    nameTypeLayout->setColumnStretch(0, 0);
    nameTypeLayout->setColumnStretch(1, 0);
    nameTypeLayout->setColumnStretch(2, 100);
    nameTypeLayout->setColumnStretch(3, 0);
    nameTypeLayout->addWidget(newFileNameLabel,      nameTypeRow, 0);
    nameTypeLayout->addWidget(m_newFileNameLineEdit, nameTypeRow, 1, 1, 2);
    nameTypeLayout->addWidget(newFileNamePushButton, nameTypeRow, 4);
    nameTypeRow++;
    nameTypeLayout->addWidget(newFileTypeLabel,      nameTypeRow, 0);
    nameTypeLayout->addWidget(m_newFileTypeComboBox, nameTypeRow, 1);
    nameTypeRow++;
    nameTypeLayout->addWidget(newFileNumberOfMapsLabel,     nameTypeRow, 0);
    nameTypeLayout->addWidget(m_newFileNumberOfMapsSpinBox, nameTypeRow, 1, Qt::AlignLeft);
    nameTypeRow++;
    for (int32_t iMap = 0; iMap < s_maximumNumberOfMaps; iMap++) {
        CaretAssertVectorIndex(m_mapNameLabels, iMap);
        CaretAssertVectorIndex(m_mapNameLineEdits, iMap);
        
        nameTypeLayout->addWidget(m_mapNameLabels[iMap], nameTypeRow, 0);
        nameTypeLayout->addWidget(m_mapNameLineEdits[iMap], nameTypeRow, 1, 1, 2);
        nameTypeRow++;
    }
    
    const int SPIN_BOX_WIDTH = 80;
    
    QLabel* dimensionsLabel = new QLabel("Dimensions:");
    m_dimXSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_dimXSpinBox->setToolTip("Number of voxels along X-axis");
    m_dimXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_dimXSpinBox->setKeyboardTracking(true);
    QObject::connect(m_dimXSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_dimYSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_dimYSpinBox->setToolTip("Number of voxels along Y-axis");
    m_dimYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_dimYSpinBox->setKeyboardTracking(true);
    QObject::connect(m_dimYSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_dimZSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_dimZSpinBox->setToolTip("Number of voxels along Z-axis");
    m_dimZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_dimZSpinBox->setKeyboardTracking(true);
    QObject::connect(m_dimZSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    
    const double bigDouble = 100000000.0;
    
    QLabel* originLabel = new QLabel("Origin (Center of Voxel):");
    m_originXSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, s_spacingDecimals);
    m_originXSpinBox->setToolTip("X-coordinate at CENTER of first voxel in volume");
    m_originXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_originXSpinBox->setKeyboardTracking(true);
    QObject::connect(m_originXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_originYSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, s_spacingDecimals);
    m_originYSpinBox->setToolTip("Y-coordinate at CENTER of first voxel in volume");
    m_originYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_originYSpinBox->setKeyboardTracking(true);
    QObject::connect(m_originYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_originZSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, s_spacingDecimals);
    m_originZSpinBox->setToolTip("Z-coordinate at CENTER of first voxel in volume");
    m_originZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_originZSpinBox->setKeyboardTracking(true);
    QObject::connect(m_originZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    
    QLabel* spacingLabel = new QLabel("Spacing:");
    m_spacingXSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, s_spacingSingleStep, s_spacingDecimals);
    m_spacingXSpinBox->setToolTip("Size of voxel along X-axis");
    m_spacingXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingXSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingYSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, s_spacingSingleStep, s_spacingDecimals);
    m_spacingYSpinBox->setToolTip("Size of voxel along Y-axis");
    m_spacingYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingYSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingZSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, s_spacingSingleStep, s_spacingDecimals);
    m_spacingZSpinBox->setToolTip("Size of voxel along Z-axis");
    m_spacingZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingZSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingXSpinBox->setValue(1.0);
    m_spacingYSpinBox->setValue(1.0);
    m_spacingZSpinBox->setValue(1.0);
    
    QLabel* xLabel = new QLabel("X");
    QLabel* yLabel = new QLabel("Y");
    QLabel* zLabel = new QLabel("Z");
    
    QLabel* niftiLabel(new QLabel("These values follow the <a href=\"https://nifti.nimh.nih.gov\">NIFTI volume file specification</a>"));
    QObject::connect(niftiLabel, &QLabel::linkActivated,
                     this, &VolumeFileCreateDialog::linkActivated);
    QLabel* firstEdgeLabel(new QLabel("First Voxel Edge: "));
    QLabel* lastEdgeLabel(new QLabel("Last Voxel Edge: "));
    m_xFirstVoxelEdgeLabel = new QLabel();
    m_xLastVoxelEdgeLabel  = new QLabel();
    m_yFirstVoxelEdgeLabel = new QLabel();
    m_yLastVoxelEdgeLabel  = new QLabel();
    m_zFirstVoxelEdgeLabel = new QLabel();
    m_zLastVoxelEdgeLabel  = new QLabel();

    m_paramFromFilePushButton = new QPushButton("Match Volume...");
    m_paramFromFilePushButton->setToolTip("Set the voxel parameters to match a volume loaded in wb_view");
    QObject::connect(m_paramFromFilePushButton, &QPushButton::clicked,
                     this, &VolumeFileCreateDialog::loadVolumeParametersFromFilePushButtonClicked);
    
    m_resamplePushButton = new QPushButton("Resample...");
    m_resamplePushButton->setToolTip("Enter a different spacing that automatically updates dimensions and origin");
    QObject::connect(m_resamplePushButton, &QPushButton::clicked,
                     this, &VolumeFileCreateDialog::resamplePushButtonClicked);

    const int COL_LABEL = 0;
    const int COL_X = COL_LABEL + 1;
    const int COL_Y = COL_X + 1;
    const int COL_Z = COL_Y + 1;
    const int COL_LOAD = COL_Z + 1;
    
    QGroupBox* paramGroupBox = new QGroupBox("Voxels");
    QGridLayout* paramsLayout = new QGridLayout(paramGroupBox);
    paramsLayout->setVerticalSpacing(5);
    
    int paramsRow = 0;
    paramsLayout->addWidget(xLabel, paramsRow, COL_X, Qt::AlignHCenter);
    paramsLayout->addWidget(yLabel, paramsRow, COL_Y, Qt::AlignHCenter);
    paramsLayout->addWidget(zLabel, paramsRow, COL_Z, Qt::AlignHCenter);
    paramsRow++;
    
    paramsLayout->addWidget(dimensionsLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_dimXSpinBox, paramsRow, COL_X);
    paramsLayout->addWidget(m_dimYSpinBox, paramsRow, COL_Y);
    paramsLayout->addWidget(m_dimZSpinBox, paramsRow, COL_Z);
    paramsLayout->addWidget(m_paramFromFilePushButton, paramsRow, COL_LOAD);
    paramsRow++;
    
    paramsLayout->addWidget(originLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_originXSpinBox, paramsRow, COL_X);
    paramsLayout->addWidget(m_originYSpinBox, paramsRow, COL_Y);
    paramsLayout->addWidget(m_originZSpinBox, paramsRow, COL_Z);
    paramsRow++;
    
    paramsLayout->addWidget(spacingLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_spacingXSpinBox, paramsRow, COL_X);
    paramsLayout->addWidget(m_spacingYSpinBox, paramsRow, COL_Y);
    paramsLayout->addWidget(m_spacingZSpinBox, paramsRow, COL_Z);
    paramsLayout->addWidget(m_resamplePushButton,  paramsRow, COL_LOAD);
    paramsRow++;
    
    paramsLayout->addWidget(firstEdgeLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_xFirstVoxelEdgeLabel, paramsRow, COL_X, Qt::AlignRight);
    paramsLayout->addWidget(m_yFirstVoxelEdgeLabel, paramsRow, COL_Y, Qt::AlignRight);
    paramsLayout->addWidget(m_zFirstVoxelEdgeLabel, paramsRow, COL_Z, Qt::AlignRight);
    paramsRow++;
    
    paramsLayout->addWidget(lastEdgeLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_xLastVoxelEdgeLabel, paramsRow, COL_X, Qt::AlignRight);
    paramsLayout->addWidget(m_yLastVoxelEdgeLabel, paramsRow, COL_Y, Qt::AlignRight);
    paramsLayout->addWidget(m_zLastVoxelEdgeLabel, paramsRow, COL_Z, Qt::AlignRight);
    paramsRow++;
    
    paramsLayout->addWidget(new QLabel(" "), paramsRow, COL_LABEL);
    paramsRow++;
    paramsLayout->addWidget(niftiLabel, paramsRow, COL_LABEL, 1, 4, Qt::AlignLeft);
    paramsRow++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(fileGroupBox);
    layout->addWidget(paramGroupBox);
    
    if (s_previousVolumeSettingsValid) {
        m_dimXSpinBox->setValue(s_previousVolumeSettings.m_dimensions[0]);
        m_dimYSpinBox->setValue(s_previousVolumeSettings.m_dimensions[1]);
        m_dimZSpinBox->setValue(s_previousVolumeSettings.m_dimensions[2]);
        m_newFileNumberOfMapsSpinBox->setValue(s_previousVolumeSettings.m_dimensions[3]);

        const int typeIndex = m_newFileTypeComboBox->findData(static_cast<int>(s_previousVolumeSettings.m_volumeType));
        if (typeIndex >= 0) {
            m_newFileTypeComboBox->setCurrentIndex(typeIndex);
        }
        
        std::vector<std::vector<float> > m = s_previousVolumeSettings.m_indexToSpace;
        m_spacingXSpinBox->setValue(m[0][0]);
        m_spacingYSpinBox->setValue(m[1][1]);
        m_spacingZSpinBox->setValue(m[2][2]);

        m_originXSpinBox->setValue(m[0][3]);
        m_originYSpinBox->setValue(m[1][3]);
        m_originZSpinBox->setValue(m[2][3]);
    }
    
    m_newFileNameLineEdit->selectAll();
    
    numberOfMapsSpinBoxValueChanged(m_newFileNumberOfMapsSpinBox->value());
    
    return widget;
}

/**
 * @return Volume file that was created (NULL if error).
 */
VolumeFile*
VolumeFileCreateDialog::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * Called to create a new file name via the file selection dialog.
 */
void
VolumeFileCreateDialog::newFileNamePushButtonClicked()
{
    const AString filename = CaretFileDialog::getChooseFileNameDialog(DataFileTypeEnum::VOLUME,
                                                                      "");
    if ( ! filename.isEmpty()) {
        m_newFileNameLineEdit->setText(filename);
    }
}

/**
 * Called when the number of maps is changed.
 *
 * @param value
 *    New value for number of maps.
 */
void
VolumeFileCreateDialog::numberOfMapsSpinBoxValueChanged(int value)
{
    for (int32_t iMap = 0; iMap < s_maximumNumberOfMaps; iMap++) {
        CaretAssertVectorIndex(m_mapNameLabels, iMap);
        CaretAssertVectorIndex(m_mapNameLineEdits, iMap);
        
        const bool enabledFlag = (iMap < value);
        
        m_mapNameLabels[iMap]->setEnabled(enabledFlag);
        m_mapNameLineEdits[iMap]->setEnabled(enabledFlag);
    }
}


/**
 * Gets called when the ok button is clicked.
 */
void
VolumeFileCreateDialog::okButtonClicked()
{
    AString filename = m_newFileNameLineEdit->text().trimmed();
    if (filename.isEmpty()) {
        WuQMessageBox::errorOk(this, "Filename is empty.");
        return;
    }
    
    if (! DataFileTypeEnum::isValidFileExtension(filename,
                                               DataFileTypeEnum::VOLUME)) {
        AString validExtensions;
        const std::vector<AString> allExts = DataFileTypeEnum::getAllFileExtensions(DataFileTypeEnum::VOLUME);
        const int32_t numExts = static_cast<int32_t>(allExts.size());
        
        WuQDataEntryDialog ded("Invalid Volume File Extension",
                               m_newFileNameLineEdit);
        ded.setTextAtTop("Filename extension is invalid.  Choose one "
                         "of the extensions below and press OK.  Otherwise, "
                         "press Cancel to change the name of the file.",
                         true);
        
        std::vector<QRadioButton*> extButtons;
        for (int32_t i = 0; i < numExts; i++) {
            QRadioButton* rb = ded.addRadioButton("." + allExts[i]);
            if (allExts[i] == "nii.gz") {
                rb->setChecked(true);
            }
            extButtons.push_back(rb);
        }
        
        bool extValid = false;
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            for (int32_t i = 0; i < numExts; i++) {
                QRadioButton* rb = extButtons[i];
                if (rb->isChecked()) {
                    filename += rb->text();
                    extValid = true;
                    break;
                }
            }
            
        }
        
        if ( ! extValid) {
            return;
        }
    }

    const int typeIndex = m_newFileTypeComboBox->currentIndex();
    const SubvolumeAttributes::VolumeType volumeType = static_cast<SubvolumeAttributes::VolumeType>(m_newFileTypeComboBox->itemData(typeIndex).toInt());
    if (volumeType == SubvolumeAttributes::UNKNOWN) {
        WuQMessageBox::errorOk(this, "Select a valid Volume Type.");
        return;
    }
    
    const int32_t numMaps = m_newFileNumberOfMapsSpinBox->value();
    std::vector<int64_t> dimensions;
    dimensions.push_back(m_dimXSpinBox->value());
    dimensions.push_back(m_dimYSpinBox->value());
    dimensions.push_back(m_dimZSpinBox->value());
    dimensions.push_back(numMaps);
    
    const float xOrigin = m_originXSpinBox->value();
    const float yOrigin = m_originYSpinBox->value();
    const float zOrigin = m_originZSpinBox->value();
    
    const float xSpacing = m_spacingXSpinBox->value();
    const float ySpacing = m_spacingYSpinBox->value();
    const float zSpacing = m_spacingZSpinBox->value();
    if ((xSpacing == 0.0)
        || (ySpacing == 0.0)
        || (zSpacing == 0.0)) {
        WuQMessageBox::errorOk(this, "Spacing values must be non-zero.");
        return;
    }
    
    std::vector<float> rowOne(4, 0.0);
    rowOne[0] = xSpacing;
    rowOne[3] = xOrigin;

    std::vector<float> rowTwo(4, 0.0);
    rowTwo[1] = ySpacing;
    rowTwo[3] = yOrigin;
    
    std::vector<float> rowThree(4, 0.0);
    rowThree[2] = zSpacing;
    rowThree[3] = zOrigin;
    
    std::vector<float> rowFour(4, 0.0);
    rowFour[3] = 1.0;
    
    std::vector<std::vector<float> > indexToSpace;
    indexToSpace.push_back(rowOne);
    indexToSpace.push_back(rowTwo);
    indexToSpace.push_back(rowThree);
    indexToSpace.push_back(rowFour);
    
    m_volumeFile = new VolumeFile(dimensions,
                                  indexToSpace,
                                  1,
                                  volumeType);
    m_volumeFile->setFileName(filename);
    m_volumeFile->setType(volumeType);
    
    s_previousVolumeSettings.m_dimensions = dimensions;
    s_previousVolumeSettings.m_indexToSpace = indexToSpace;
    s_previousVolumeSettings.m_volumeType = volumeType;
    s_previousVolumeSettingsValid = true;
    
    float defaultValue = 0.0;
    if (numMaps > 0) {
        switch (volumeType) {
            case SubvolumeAttributes::ANATOMY:
                break;
            case SubvolumeAttributes::FUNCTIONAL:
                break;
            case SubvolumeAttributes::LABEL:
                defaultValue = m_volumeFile->getMapLabelTable(0)->getUnassignedLabelKey();
                break;
            case SubvolumeAttributes::RGB:
                CaretAssert(0);
                break;
            case SubvolumeAttributes::RGB_WORKBENCH:
                CaretAssert(0);
                break;
            case SubvolumeAttributes::SEGMENTATION:
                break;
            case SubvolumeAttributes::UNKNOWN:
                CaretAssert(0);
                break;
            case SubvolumeAttributes::VECTOR:
                CaretAssert(0);
                break;
        }
    }
    m_volumeFile->setValueAllVoxels(defaultValue);
    m_volumeFile->updateScalarColoringForAllMaps();
    
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        CaretAssertVectorIndex(m_mapNameLineEdits, iMap);
        m_volumeFile->setMapName(iMap,
                                 m_mapNameLineEdits[iMap]->text().trimmed());
    }
    
    WuQDialog::okButtonClicked();
}

/**
 * Set the volume parameters from a loaded file.
 */
void
VolumeFileCreateDialog::loadVolumeParametersFromFilePushButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    std::vector<CaretMappableDataFile*> allMappableFiles;
    brain->getAllMappableDataFiles(allMappableFiles);
    
    std::vector<CaretMappableDataFile*> volumeMappableFiles;
    for (std::vector<CaretMappableDataFile*>::iterator allIter = allMappableFiles.begin();
         allIter != allMappableFiles.end();
         allIter++) {
        CaretMappableDataFile* mapFile = *allIter;
        if (mapFile->isVolumeMappable()) {
            volumeMappableFiles.push_back(mapFile);
        }
    }
    
    const int32_t numberOfVolumeMappableFiles = static_cast<int32_t>(volumeMappableFiles.size());
    if (numberOfVolumeMappableFiles <= 0) {
        WuQMessageBox::errorOk(m_paramFromFilePushButton,
                               "No volume mappable files are loaded.");
    }
    
    std::vector<QRadioButton*> volumeRadioButtons;
    WuQDataEntryDialog dialog("Select File For Volume Parameters",
                              m_paramFromFilePushButton);
    for (int32_t i = 0; i < numberOfVolumeMappableFiles; i++) {
        CaretAssertVectorIndex(volumeMappableFiles, i);
        QRadioButton* rb = dialog.addRadioButton(volumeMappableFiles[i]->getFileNameNoPath());
        volumeRadioButtons.push_back(rb);
        if (i == 0) {
            rb->setChecked(true);
        }
    }
    
    
    if (dialog.exec() == WuQDataEntryDialog::Accepted) {
        for (int32_t i = 0; i < numberOfVolumeMappableFiles; i++) {
            CaretAssertVectorIndex(volumeRadioButtons, i);
            if (volumeRadioButtons[i]->isChecked()) {
                CaretAssertVectorIndex(volumeMappableFiles, i);
                CaretMappableDataFile* volMapFile = volumeMappableFiles[i];
                
                CiftiMappableDataFile* ciftiFile  = dynamic_cast<CiftiMappableDataFile*>(volMapFile);
                VolumeFile*            volumeFile = dynamic_cast<VolumeFile*>(volMapFile);
                
                
                
                int64_t dimensions[5] = { 0, 0, 0, 0, 0 };
                float origin[3]       = { 0.0, 0.0, 0.0 };
                float spacing[3]      = { 0.0, 0.0, 0.0 };
                
                int32_t numberOfMaps = 1;
                SubvolumeAttributes::VolumeType volumeType = SubvolumeAttributes::FUNCTIONAL;
                
                if (ciftiFile != NULL) {
                    int64_t dimI, dimJ, dimK, dimTime, dimComponents;
                    ciftiFile->getDimensions(dimI, dimJ, dimK, dimTime, dimComponents);
                    
                    dimensions[0] = dimI;
                    dimensions[1] = dimJ;
                    dimensions[2] = dimK;
                    
                    ciftiFile->indexToSpace(0, 0, 0, origin);
                    
                    float oneOneOneVoxelXYZ[3];
                    ciftiFile->indexToSpace(1, 1, 1, oneOneOneVoxelXYZ);
                    
                    spacing[0] = oneOneOneVoxelXYZ[0] - origin[0];
                    spacing[1] = oneOneOneVoxelXYZ[1] - origin[1];
                    spacing[2] = oneOneOneVoxelXYZ[2] - origin[2];
                    
                    numberOfMaps = volMapFile->getNumberOfMaps();
                    if (volMapFile->isMappedWithLabelTable()) {
                        volumeType = SubvolumeAttributes::LABEL;
                    }
                    else if (volMapFile->isMappedWithPalette()) {
                        volumeType = SubvolumeAttributes::FUNCTIONAL;
                    }
                }
                else if (volumeFile != NULL) {
                    const VolumeSpace volumeSpace = volumeFile->getVolumeSpace();
                    
                    const int64_t* dimArray = volumeSpace.getDims();
                    dimensions[0] = dimArray[0];
                    dimensions[1] = dimArray[1];
                    dimensions[2] = dimArray[2];
                    
                    VolumeSpace::OrientTypes orientation[3];
                    volumeSpace.getOrientAndSpacingForPlumb(orientation,
                                                            spacing,
                                                            origin);
                    
                    numberOfMaps = volumeFile->getNumberOfMaps();
                    volumeType   = volumeFile->getType();
                }
                else {
                    AString msg = ("Program Error: "
                                   + volMapFile->getFileNameNoPath()
                                   + " is volume mappable but neither a volume nor a CIFTI file.");
                    WuQMessageBox::errorOk(m_paramFromFilePushButton,
                                           msg);
                    return;
                }
                
                const bool includeTypeAndNumberOfMapsFlag = false;
                if (includeTypeAndNumberOfMapsFlag) {
                    const int typeIndex = m_newFileTypeComboBox->findData(static_cast<int>(volumeType));
                    if (typeIndex >= 0) {
                        m_newFileTypeComboBox->setCurrentIndex(typeIndex);
                    }
                    m_newFileNumberOfMapsSpinBox->setValue(numberOfMaps);
                }
                
                m_blockVoxelEdgeLabelUpdateFlag = true;
                
                m_dimXSpinBox->setValue(dimensions[0]);
                m_dimYSpinBox->setValue(dimensions[1]);
                m_dimZSpinBox->setValue(dimensions[2]);
                
                
                m_originXSpinBox->setValue(origin[0]);
                m_originYSpinBox->setValue(origin[1]);
                m_originZSpinBox->setValue(origin[2]);
                
                m_spacingXSpinBox->setValue(spacing[0]);
                m_spacingYSpinBox->setValue(spacing[1]);
                m_spacingZSpinBox->setValue(spacing[2]);
                
                m_blockVoxelEdgeLabelUpdateFlag = false;
                
                break;
            }
        }
    }
}

/**
 * Called when a hyperlink is clicked
 * @param link
 *    Link that was clicked
 */
void
VolumeFileCreateDialog::linkActivated(const QString& link)
{
    QDesktopServices::openUrl(link);
}

/**
 * Update the voxel edges as values are changed
 */
void
VolumeFileCreateDialog::updateVoxelEdgeLabels()
{
    if (m_blockVoxelEdgeLabelUpdateFlag) {
        return;
    }
    
    const Vector3D dimensions(m_dimXSpinBox->value(),
                              m_dimYSpinBox->value(),
                              m_dimZSpinBox->value());
    const Vector3D spacing(m_spacingXSpinBox->value(),
                           m_spacingYSpinBox->value(),
                           m_spacingZSpinBox->value());
    const Vector3D origin(m_originXSpinBox->value(),
                          m_originYSpinBox->value(),
                          m_originZSpinBox->value());

    Vector3D firstVoxelEdge;
    Vector3D lastVoxelEdge;
    
    VolumeFile::dimensionOriginSpacingXyzToVoxelEdges(dimensions,
                                                      origin,
                                                      spacing,
                                                      firstVoxelEdge,
                                                      lastVoxelEdge);
    const int32_t decimals(3);
    m_xFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[0], 'f', decimals));
    m_yFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[1], 'f', decimals));
    m_zFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[2], 'f', decimals));
    m_xLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[0], 'f', decimals));
    m_yLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[1], 'f', decimals));
    m_zLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[2], 'f', decimals));
}

/**
 * Called when the resample push button is clicked
 */
void
VolumeFileCreateDialog::resamplePushButtonClicked()
{
    const Vector3D dimensions(m_dimXSpinBox->value(),
                              m_dimYSpinBox->value(),
                              m_dimZSpinBox->value());
    const Vector3D spacing(m_spacingXSpinBox->value(),
                           m_spacingYSpinBox->value(),
                           m_spacingZSpinBox->value());
    const Vector3D origin(m_originXSpinBox->value(),
                          m_originYSpinBox->value(),
                          m_originZSpinBox->value());

    VolumeFileResampleDialog dialog(dimensions,
                                    origin,
                                    spacing,
                                    s_spacingSingleStep,
                                    s_spacingDecimals);
    if (dialog.exec() == VolumeFileResampleDialog::Accepted) {
        m_blockVoxelEdgeLabelUpdateFlag = true;
        
        const Vector3D dim(dialog.getDimensions());
        m_dimXSpinBox->setValue(dim[0]);
        m_dimYSpinBox->setValue(dim[1]);
        m_dimZSpinBox->setValue(dim[2]);
        
        const Vector3D origin(dialog.getOrigin());
        m_originXSpinBox->setValue(origin[0]);
        m_originYSpinBox->setValue(origin[1]);
        m_originZSpinBox->setValue(origin[2]);
        
        const Vector3D spacing(dialog.getSpacing());
        m_spacingXSpinBox->setValue(spacing[0]);
        m_spacingYSpinBox->setValue(spacing[1]);
        m_spacingZSpinBox->setValue(spacing[2]);
        
        m_blockVoxelEdgeLabelUpdateFlag = true;
        updateVoxelEdgeLabels();
    }
}

