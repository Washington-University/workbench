
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __VOLUME_MONTAGE_SETUP_DIALOG_DECLARE__
#include "VolumeMontageSetupDialog.h"
#undef __VOLUME_MONTAGE_SETUP_DIALOG_DECLARE__

#include <cmath>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QQuaternion>
#include <QSpinBox>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "EnumComboBoxTemplate.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "Matrix4x4.h"
#include "MetaVolumeFile.h"
#include "OverlaySet.h"
#include "VolumeFile.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::VolumeMontageSetupDialog 
 * \brief Dialog for setting up volume montages
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
VolumeMontageSetupDialog::VolumeMontageSetupDialog(BrowserTabContent* browserTabContent,
                                                   QWidget* parent)
: WuQDialogModal("Setup Volume Montage",
                 parent),
m_browserTabContent(browserTabContent)
{
    CaretAssert(m_browserTabContent);
    
    m_dialogCreationInProgressFlag = true;

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    m_volumeSliceInputWidget = createVolumeSliceInputWidget();
    m_histologyWidget = createHistologyWidget();
    layout->addWidget(createMontageInputWidget());
    layout->addWidget(m_volumeSliceInputWidget);
    layout->addWidget(m_histologyWidget);
    layout->addWidget(createMontageOutputWidget());
    setCentralWidget(widget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    VolumeMappableInterface* vmi(browserTabContent->getOverlaySet()->getUnderlayVolume());
    if (vmi != NULL) {
        CaretDataFile* caretDataFile(vmi->castToVolumeMappableDataFile());
        if (caretDataFile != NULL) {
            m_volumeFileSelectionComboBox->setSelectedFile(caretDataFile);
        }
    }
    
    /*
     * Loads data and sets up dialog
     */
    histologyFileSelected(m_histologyFileSelectionComboBox->getSelectedFile());
    volumeFileSelected(m_volumeFileSelectionComboBox->getSelectedFile());
    QSignalBlocker rowsBlocker(m_montageInputRowsSpinBox);
    m_montageInputRowsSpinBox->setValue(5);
    QSignalBlocker columnsBlocker(m_montageInputColumnsSpinBox);
    m_montageInputColumnsSpinBox->setValue(5);
    montageInputVolumeSliceAxisComboBoxValueChanged(m_montageInputVolumeSliceAxisComboBox->currentIndex());
    m_dialogCreationInProgressFlag = false;
    montageInputRowOrColumnValueChanged();
    
    /*
     * Disable OK button until finished
     */
    setOkButtonEnabled(false);
}

/**
 * Destructor.
 */
VolumeMontageSetupDialog::~VolumeMontageSetupDialog()
{

}

/**
 * @return New instance of the input section
 */
QWidget*
VolumeMontageSetupDialog::createMontageInputWidget()
{
    QLabel* slicePlaneLabel(new QLabel("Volume Montage Axis:"));
    m_montageInputVolumeSliceAxisComboBox = new QComboBox();
    m_montageInputVolumeSliceAxisComboBox->addItem("Histology File's Axis");
    m_montageInputVolumeSliceAxisComboBox->setItemData(m_montageInputVolumeSliceAxisComboBox->count() - 1,
                                                 static_cast<int32_t>(VolumeSliceViewPlaneEnum::ALL));
    m_montageInputVolumeSliceAxisComboBox->addItem("Volume's Axial Axis");
    m_montageInputVolumeSliceAxisComboBox->setItemData(m_montageInputVolumeSliceAxisComboBox->count() - 1,
                                                 static_cast<int32_t>(VolumeSliceViewPlaneEnum::AXIAL));
    m_montageInputVolumeSliceAxisComboBox->addItem("Volume's Coronal Axis");
    m_montageInputVolumeSliceAxisComboBox->setItemData(m_montageInputVolumeSliceAxisComboBox->count() - 1,
                                                 static_cast<int32_t>(VolumeSliceViewPlaneEnum::CORONAL));
    m_montageInputVolumeSliceAxisComboBox->addItem("Volume's Parasagittal Axis");
    m_montageInputVolumeSliceAxisComboBox->setItemData(m_montageInputVolumeSliceAxisComboBox->count() - 1,
                                                 static_cast<int32_t>(VolumeSliceViewPlaneEnum::PARASAGITTAL));
    QObject::connect(m_montageInputVolumeSliceAxisComboBox, &QComboBox::activated,
                     this, &VolumeMontageSetupDialog::montageInputVolumeSliceAxisComboBoxValueChanged);
    

    QLabel* volumeFileLabel(new QLabel("Volume File:"));
    std::vector<DataFileTypeEnum::Enum> volumeFileTypes { DataFileTypeEnum::VOLUME, DataFileTypeEnum::META_VOLUME };
    std::pair<CaretDataFileSelectionModel*, CaretDataFileSelectionComboBox*> volumeModelAndFileCombo
    = CaretDataFileSelectionComboBox::newInstanceForFileType(DataFileTypeEnum::VOLUME, this);
    m_volumeFileSelectionComboBox = volumeModelAndFileCombo.second;
    QObject::connect(m_volumeFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &VolumeMontageSetupDialog::volumeFileSelected);
    

    const int32_t maxVal(100);
    
    QLabel* rowsLabel(new QLabel("Montage Rows:"));
    m_montageInputRowsSpinBox = new QSpinBox();
    m_montageInputRowsSpinBox->setRange(1, maxVal);
    QObject::connect(m_montageInputRowsSpinBox,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::montageInputRowsValueChanged);
    
    QLabel* columnsLabel(new QLabel(" Columns:"));
    m_montageInputColumnsSpinBox = new QSpinBox();
    m_montageInputColumnsSpinBox->setRange(1, maxVal);
    QObject::connect(m_montageInputColumnsSpinBox,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::montageInputColumnsValueChanged);

    QLabel* numberOfSlicesLabel(new QLabel("Number of Slices:"));
    m_montageInputNumberOfSlicesLabel = new QLabel();

    QGridLayout* fileAndPlaneLayout(new QGridLayout());
    fileAndPlaneLayout->addWidget(slicePlaneLabel,
                                  0, 0);
    fileAndPlaneLayout->addWidget(m_montageInputVolumeSliceAxisComboBox,
                                  0, 1, 1, 5, Qt::AlignLeft);
    fileAndPlaneLayout->addWidget(volumeFileLabel,
                                  1, 0);
    fileAndPlaneLayout->addWidget(m_volumeFileSelectionComboBox->getWidget(),
                                  1, 1);

    
    const int32_t STRETCH_YES(100);
    QGridLayout* rowColLayout(new QGridLayout());
    rowColLayout->setColumnStretch(6, STRETCH_YES);
    int row(rowColLayout->rowCount());
    rowColLayout->addWidget(rowsLabel,
                            row, 0);
    rowColLayout->addWidget(m_montageInputRowsSpinBox,
                            row, 1);
    rowColLayout->addWidget(columnsLabel,
                            row, 2);
    rowColLayout->addWidget(m_montageInputColumnsSpinBox,
                            row, 3);
    rowColLayout->addWidget(numberOfSlicesLabel,
                            row, 4);
    rowColLayout->addWidget(m_montageInputNumberOfSlicesLabel,
                            row, 5);;
    ++row;

    QGroupBox* widget(new QGroupBox("Montage Inputs"));
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(fileAndPlaneLayout);
    layout->addLayout(rowColLayout);

    return widget;
}

/**
 * @return New instance of volume slice input widget
 */
QWidget*
VolumeMontageSetupDialog::createVolumeSliceInputWidget()
{
    QLabel* startLabel(new QLabel("Start:"));
    QLabel* endLabel(new QLabel("End:"));
    QLabel* sliceIndexLabel(new QLabel("Slice Index"));
    QLabel* coordinateLabel(new QLabel("Coordinate"));
    
    m_volumeStartSliceIndexSpinBox = new QSpinBox();
    m_volumeStartSliceIndexSpinBox->setMinimumWidth(s_minimumSpinBoxWidth);
    QObject::connect(m_volumeStartSliceIndexSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::volumeStartSliceIndexSpinBoxValueChanged);
    
    m_volumeEndSliceIndexSpinBox = new QSpinBox();
    m_volumeEndSliceIndexSpinBox->setMinimumWidth(s_minimumSpinBoxWidth);
    QObject::connect(m_volumeEndSliceIndexSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::volumeEndSliceIndexSpinBoxValueChanged);
    
    m_volumeStartSliceCoordinateLabel = new QLabel();
    m_volumeEndSliceCoordinateLabel   = new QLabel();
    
    const int32_t COL_LABEL(0);
    const int32_t COL_INDEX(1);
    const int32_t COL_COORD(2);
    const int32_t STRETCH_YES(100);
    
    QWidget* widget(new QGroupBox("Volume Axis Slices"));
    QGridLayout* sliceCoordLayout(new QGridLayout(widget));
    sliceCoordLayout->setColumnStretch(3, STRETCH_YES);
    int row(sliceCoordLayout->rowCount());
    sliceCoordLayout->addWidget(sliceIndexLabel,
                                row, COL_INDEX, Qt::AlignHCenter);
    sliceCoordLayout->addWidget(coordinateLabel,
                                row, COL_COORD, Qt::AlignHCenter);
    ++row;
    sliceCoordLayout->addWidget(endLabel,
                                row, COL_LABEL);
    sliceCoordLayout->addWidget(m_volumeEndSliceIndexSpinBox,
                                row, COL_INDEX);
    sliceCoordLayout->addWidget(m_volumeEndSliceCoordinateLabel,
                                row, COL_COORD);
    ++row;
    sliceCoordLayout->addWidget(startLabel,
                                row, COL_LABEL);
    sliceCoordLayout->addWidget(m_volumeStartSliceIndexSpinBox,
                                row, COL_INDEX);
    sliceCoordLayout->addWidget(m_volumeStartSliceCoordinateLabel,
                                row, COL_COORD);
    ++row;

    return widget;
}


/**
 * @return Selected montage volume axis (ALL implies axis of histology slices)
 */
VolumeSliceViewPlaneEnum::Enum
VolumeMontageSetupDialog::getSelectedVolumeMontageAxis() const
{
    const int32_t axisInteger(m_montageInputVolumeSliceAxisComboBox->currentData().toInt());
    bool validFlag(false);
    VolumeSliceViewPlaneEnum::Enum axis(VolumeSliceViewPlaneEnum::fromIntegerCode(axisInteger,
                                                                                  &validFlag));
    if ( ! validFlag) {
        axis = VolumeSliceViewPlaneEnum::AXIAL;
    }
    return axis;
}

/**
 * @return New instance of the histology section
 */
QWidget*
VolumeMontageSetupDialog::createHistologyWidget()
{
    QLabel* histologyFileLabel(new QLabel("Histology Slices File:"));
    std::pair<CaretDataFileSelectionModel*, CaretDataFileSelectionComboBox*> histModelAndFileCombo
    = CaretDataFileSelectionComboBox::newInstanceForFileType(DataFileTypeEnum::HISTOLOGY_SLICES, this);
    m_histologyFileSelectionComboBox = histModelAndFileCombo.second;
    QObject::connect(m_histologyFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &VolumeMontageSetupDialog::histologyFileSelected);
    
    QLabel* startLabel(new QLabel("Start:"));
    QLabel* endLabel(new QLabel("End:"));
    QLabel* sliceIndexLabel(new QLabel("Slice Index"));
    QLabel* coordinateLabel(new QLabel("Coordinate"));

    m_histologyStartSliceIndexSpinBox = new QSpinBox();
    m_histologyStartSliceIndexSpinBox->setMinimumWidth(s_minimumSpinBoxWidth);
    QObject::connect(m_histologyStartSliceIndexSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::histologyStartSliceIndexSpinBoxValueChanged);

    m_histologyEndSliceIndexSpinBox = new QSpinBox();
    m_histologyEndSliceIndexSpinBox->setMinimumWidth(s_minimumSpinBoxWidth);
    QObject::connect(m_histologyEndSliceIndexSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &VolumeMontageSetupDialog::histologyEndSliceIndexSpinBoxValueChanged);

    m_histologyStartSliceCoordinateLabel = new QLabel();
    m_histologyEndSliceCoordinateLabel   = new QLabel();
    
    const int32_t COL_LABEL(0);
    const int32_t COL_INDEX(1);
    const int32_t COL_COORD(2);
    const int32_t STRETCH_NO(0);
    const int32_t STRETCH_YES(100);
    
    QHBoxLayout* fileLayout(new QHBoxLayout());
    fileLayout->addWidget(histologyFileLabel, STRETCH_NO);
    fileLayout->addWidget(m_histologyFileSelectionComboBox->getWidget(), STRETCH_YES);
    
    QWidget* widget(new QGroupBox("Histology File"));
    QGridLayout* layout(new QGridLayout(widget));
    layout->setColumnStretch(3, STRETCH_YES);
    int row(layout->rowCount());
    layout->addLayout(fileLayout,
                      row, COL_LABEL, 1, 4);
    ++row;
    layout->addWidget(sliceIndexLabel,
                      row, COL_INDEX, Qt::AlignHCenter);
    layout->addWidget(coordinateLabel,
                      row, COL_COORD, Qt::AlignHCenter);
    ++row;
    layout->addWidget(endLabel,
                      row, COL_LABEL);
    layout->addWidget(m_histologyEndSliceIndexSpinBox,
                      row, COL_INDEX);
    layout->addWidget(m_histologyEndSliceCoordinateLabel,
                      row, COL_COORD);
    ++row;
    layout->addWidget(startLabel,
                      row, COL_LABEL);
    layout->addWidget(m_histologyStartSliceIndexSpinBox,
                      row, COL_INDEX);
    layout->addWidget(m_histologyStartSliceCoordinateLabel,
                      row, COL_COORD);
    ++row;

    return widget;
}

/**
 * @return New instance of the montage section
 */
QWidget*
VolumeMontageSetupDialog::createMontageOutputWidget()
{
    QLabel* parasagittalLabel = new QLabel("Parasagittal:");
    QLabel* coronalLabel      = new QLabel("Coronal:");
    QLabel* axialLabel        = new QLabel("Axial:");
    QLabel* sliceAxisLabel    = new QLabel("Slice Axis");
    QLabel* sliceIndexLabel(new QLabel("Index"));
    QLabel* coordinateLabel(new QLabel("Coordinate"));

    m_montageOutputParagittalSliceIndexSpinBox = new QDoubleSpinBox();
    
    m_montageOutputCoronalSliceIndexSpinBox = new QDoubleSpinBox();
    
    m_montageOutputAxialSliceIndexSpinBox = new QDoubleSpinBox();
    
    m_montageOutputParagittalSliceCoordinateSpinBox = new QDoubleSpinBox();
    
    m_montageOutputCoronalSliceCoordinateSpinBox = new QDoubleSpinBox();
    
    m_montageOutputAxialSliceCoordinateSpinBox = new QDoubleSpinBox();

    const int32_t COL_LABEL(0);
    const int32_t COL_INDEX(1);
    const int32_t COL_COORD(2);
    const int32_t COL_STRETCH(3);
    const int32_t STRETCH_YES(100);
    QGridLayout* pcaLayout(new QGridLayout());
    pcaLayout->setColumnStretch(COL_STRETCH,
                                STRETCH_YES);
    int32_t row(pcaLayout->rowCount());
    pcaLayout->addWidget(sliceAxisLabel,
                         row, COL_LABEL);
    pcaLayout->addWidget(sliceIndexLabel,
                         row, COL_INDEX);
    pcaLayout->addWidget(coordinateLabel,
                         row, COL_COORD);
    ++row;
    pcaLayout->addWidget(parasagittalLabel,
                         row, COL_LABEL);
    pcaLayout->addWidget(m_montageOutputParagittalSliceIndexSpinBox,
                         row, COL_INDEX);
    pcaLayout->addWidget(m_montageOutputParagittalSliceCoordinateSpinBox,
                         row, COL_COORD);
    ++row;
    pcaLayout->addWidget(coronalLabel,
                         row, COL_LABEL);
    pcaLayout->addWidget(m_montageOutputCoronalSliceIndexSpinBox,
                         row, COL_INDEX);
    pcaLayout->addWidget(m_montageOutputCoronalSliceCoordinateSpinBox,
                         row, COL_COORD);
    ++row;
    pcaLayout->addWidget(axialLabel,
                         row, COL_LABEL);
    pcaLayout->addWidget(m_montageOutputAxialSliceIndexSpinBox,
                         row, COL_INDEX);
    pcaLayout->addWidget(m_montageOutputAxialSliceCoordinateSpinBox,
                         row, COL_COORD);
    ++row;

    QLabel* stepLabel(new QLabel("Montage Step:"));
    m_montageOutputSliceStepLabel = new QLabel();

    QLabel* thicknessLabel(new QLabel("Slice Thickness:"));
    m_montageOutputSliceSpacingLabel = new QLabel();
    
    QLabel* rotationAnglesLabel(new QLabel("MPR Rotation Angles:"));
    m_montageOutputRotationAnglesLabel = new QLabel();
    
    QLabel* sliceViewPlaneLabel(new QLabel("Slice Plane:"));
    m_montageOutputSliceViewPlaneLabel = new QLabel();
    
    QGridLayout* stepLayout(new QGridLayout());
    stepLayout->setColumnStretch(2, STRETCH_YES);
    int32_t stepLayoutRow(stepLayout->rowCount());
    stepLayout->addWidget(thicknessLabel,
                          stepLayoutRow, 0);
    stepLayout->addWidget(m_montageOutputSliceSpacingLabel,
                          stepLayoutRow, 1);
    ++stepLayoutRow;
    stepLayout->addWidget(stepLabel,
                          stepLayoutRow, 0);
    stepLayout->addWidget(m_montageOutputSliceStepLabel,
                          stepLayoutRow, 1);
    ++stepLayoutRow;
    stepLayout->addWidget(sliceViewPlaneLabel,
                          stepLayoutRow, 0);
    stepLayout->addWidget(m_montageOutputSliceViewPlaneLabel,
                          stepLayoutRow, 1);
    ++stepLayoutRow;
    stepLayout->addWidget(rotationAnglesLabel,
                          stepLayoutRow, 0);
    stepLayout->addWidget(m_montageOutputRotationAnglesLabel,
                          stepLayoutRow, 1);
    ++stepLayoutRow;
    
    QWidget* widget(new QGroupBox("Montage Output"));
    QHBoxLayout* layout(new QHBoxLayout(widget));
    layout->addLayout(pcaLayout);
    layout->addSpacing(10);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addSpacing(10);
    layout->addLayout(stepLayout);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when montage rows value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::montageInputRowsValueChanged(int /*value*/)
{
    montageInputRowOrColumnValueChanged();
}

/**
 * Called when montage columns value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::montageInputColumnsValueChanged(int /*value*/)
{
    montageInputRowOrColumnValueChanged();
}

/**
 * Called when a montage row or column value changed
 */
void
VolumeMontageSetupDialog::montageInputRowOrColumnValueChanged()
{
    const int32_t numRows(m_montageInputRowsSpinBox->value());
    const int32_t numCols(m_montageInputColumnsSpinBox->value());
    const int32_t numberOfSlices(numCols * numRows);
    m_montageInputNumberOfSlicesLabel->setText(AString::number(numberOfSlices));

    updateMontageOutputWidget();
}

/**
 * Called when histology file changed
 * @param caretDataFile
 *    File selected by user
 */
void
VolumeMontageSetupDialog::histologyFileSelected(CaretDataFile* caretDataFile)
{
    bool validFlag(false);
    if (caretDataFile != NULL) {
        HistologySlicesFile* hsf(caretDataFile->castToHistologySlicesFile());
        if (hsf != NULL) {
            const int32_t lastSliceIndex(hsf->getNumberOfHistologySlices() - 1);
            if (lastSliceIndex >= 0) {
                QSignalBlocker startBlocker(m_histologyStartSliceIndexSpinBox);
                m_histologyStartSliceIndexSpinBox->setRange(0, lastSliceIndex);
                m_histologyStartSliceIndexSpinBox->setValue(0);
                histologyStartSliceIndexSpinBoxValueChanged(m_histologyStartSliceIndexSpinBox->value());
                QSignalBlocker endBlocker(m_histologyEndSliceIndexSpinBox);
                m_histologyEndSliceIndexSpinBox->setRange(0, lastSliceIndex);
                m_histologyEndSliceIndexSpinBox->setValue(lastSliceIndex);
                histologyEndSliceIndexSpinBoxValueChanged(m_histologyEndSliceIndexSpinBox->value());
                validFlag = true;
            }
        }
    }
    m_histologyStartSliceIndexSpinBox->setEnabled(validFlag);
    m_histologyEndSliceIndexSpinBox->setEnabled(validFlag);
    
    updateMontageOutputWidget();
}

/**
 * Called when value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::histologyStartSliceIndexSpinBoxValueChanged(int value)
{
    FunctionResultValue<Vector3D> result(histologySliceIndexToCoordinate(value));
    if (result.isOk()) {
        m_histologyStartSliceCoordinateLabel->setText(result.getValue().toString());
    }
    else {
        m_histologyStartSliceCoordinateLabel->setText("Invalid");
    }
    
    updateMontageOutputWidget();
}

/**
 * Called when value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::histologyEndSliceIndexSpinBoxValueChanged(int value)
{
    FunctionResultValue<Vector3D> result(histologySliceIndexToCoordinate(value));
    if (result.isOk()) {
        m_histologyEndSliceCoordinateLabel->setText(result.getValue().toString());
    }
    else {
        m_histologyEndSliceCoordinateLabel->setText("Invalid");
    }

    updateMontageOutputWidget();
}

/**
 * Convert a histology slice index to a coordinate
 * @param sliceIndex
 *   The slice index
 * @return
 *   The slice coordinate center
 */
FunctionResultValue<Vector3D>
VolumeMontageSetupDialog::histologySliceIndexToCoordinate(const int32_t sliceIndex) const
{
    Vector3D xyz;
    bool validFlag(false);
    CaretDataFile* cdf(m_histologyFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        HistologySlicesFile* hsi(dynamic_cast<HistologySlicesFile*>(cdf));
        if (hsi != NULL) {
            const HistologySlice* slice(hsi->getHistologySliceByIndex(sliceIndex));
            if (slice != NULL) {
                const BoundingBox boundingBox(slice->getStereotaxicXyzBoundingBox());
                if (boundingBox.isValid()) {
                    boundingBox.getCenter(xyz);
                    validFlag = true;
                }
            }
        }
    }
    
    return FunctionResultValue<Vector3D>(xyz, "", validFlag);
}

/**
 * @return Normal vector formed by histology slices
 */
FunctionResultValue<Vector3D>
VolumeMontageSetupDialog::getHistologySlicesNormalVector() const
{
    FunctionResultValue<Vector3D> resultStart(histologySliceIndexToCoordinate(m_histologyStartSliceIndexSpinBox->value()));
    FunctionResultValue<Vector3D> resultEnd(histologySliceIndexToCoordinate(m_histologyEndSliceIndexSpinBox->value()));
    Vector3D normalVector(0.0, 0.0, 1.0);
    AString msg;
    if (resultStart.isOk()
        && resultEnd.isOk()) {
        normalVector = (resultEnd.getValue() - resultStart.getValue()).normal();
    }
    else {
        msg = "Histology slices normal vector is invalid (default to (0, 0, 1))";
    }
    return FunctionResultValue<Vector3D>(normalVector,
                                         msg,
                                         msg.isEmpty());
}

/**
 * @return Slice view plane that best matches the histology slices normal vector
 */
FunctionResultValue<std::pair<VolumeSliceViewPlaneEnum::Enum,bool>>
VolumeMontageSetupDialog::histologyOrientationToVolumeSliceViewPlane() const
{
    bool flipFlag(false);
    VolumeSliceViewPlaneEnum::Enum sliceViewPlane(VolumeSliceViewPlaneEnum::AXIAL);
    const FunctionResultValue<Vector3D> normalVectorResult(getHistologySlicesNormalVector());
    if (normalVectorResult.isOk()) {
        const Vector3D normalVector(normalVectorResult.getValue());
        const float nx(std::fabs(normalVector[0]));
        const float ny(std::fabs(normalVector[1]));
        const float nz(std::fabs(normalVector[2]));
        if ((nx >= ny)
            && (nx >= nz)) {
            sliceViewPlane = VolumeSliceViewPlaneEnum::PARASAGITTAL;
            flipFlag = ((nx * normalVector[0]) < 0.0);
        }
        else if ((ny >= nx)
                 && (ny >= nz)) {
            sliceViewPlane = VolumeSliceViewPlaneEnum::CORONAL;
            flipFlag = ((ny * normalVector[1]) < 0.0);
        }
        else {
            sliceViewPlane = VolumeSliceViewPlaneEnum::AXIAL;
            flipFlag = ((nz * normalVector[2]) < 0.0);
        }
    }
    return FunctionResultValue<std::pair<VolumeSliceViewPlaneEnum::Enum,bool>>(std::make_pair(sliceViewPlane, flipFlag),
                                                                               normalVectorResult.getErrorMessage(),
                                                                               normalVectorResult.isOk());
}


/**
 * Called when volume file changed
 * @param caretDataFile
 *    File selected by user
 */
void
VolumeMontageSetupDialog::volumeFileSelected(CaretDataFile* caretDataFile)
{
    int32_t minSlice(0);
    int32_t maxSlice(-1);
    bool validFlag(false);
    if (caretDataFile != NULL) {
        VolumeMappableInterface* vmi(dynamic_cast<VolumeMappableInterface*>(caretDataFile));
        if (vmi != NULL) {
            int64_t dimI, dimJ, dimK, dimTime, dimCompontents;
            vmi->getDimensions(dimI, dimJ, dimK, dimTime, dimCompontents);
            
            const int32_t viewPlaneInteger(m_montageInputVolumeSliceAxisComboBox->currentData().toInt());
            const VolumeSliceViewPlaneEnum::Enum slicePlane(VolumeSliceViewPlaneEnum::fromIntegerCode(viewPlaneInteger, NULL));
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    minSlice = 0;
                    maxSlice = std::max(dimI, std::max(dimJ, dimK));
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    minSlice = 0;
                    maxSlice = dimK - 1;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    minSlice = 0;
                    maxSlice = dimJ - 1;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    minSlice = 0;
                    maxSlice = dimI - 1;
                    break;
            }
            
            QSignalBlocker startBlocker(m_volumeStartSliceIndexSpinBox);
            m_volumeStartSliceIndexSpinBox->setRange(minSlice, maxSlice);
            m_volumeStartSliceIndexSpinBox->setValue(0);
            volumeStartSliceIndexSpinBoxValueChanged(m_volumeStartSliceIndexSpinBox->value());
            QSignalBlocker endBlocker(m_volumeEndSliceIndexSpinBox);
            m_volumeEndSliceIndexSpinBox->setRange(minSlice, maxSlice);
            m_volumeEndSliceIndexSpinBox->setValue(maxSlice);
            volumeEndSliceIndexSpinBoxValueChanged(m_volumeEndSliceIndexSpinBox->value());

            m_montageOutputParagittalSliceIndexSpinBox->setRange(0, dimI - 1);
            m_montageOutputCoronalSliceIndexSpinBox->setRange(0, dimJ - 1);
            m_montageOutputAxialSliceIndexSpinBox->setRange(0, dimK - 1);

            BoundingBox bb;
            vmi->getVoxelSpaceBoundingBox(bb);
            if (bb.isValid()) {
                m_montageOutputParagittalSliceCoordinateSpinBox->setRange(bb.getMinX(), bb.getMaxX());
                m_montageOutputCoronalSliceCoordinateSpinBox->setRange(bb.getMinY(), bb.getMaxY());
                m_montageOutputAxialSliceCoordinateSpinBox->setRange(bb.getMinZ(), bb.getMaxZ());
            }
            validFlag = true;
        }
    }
    
    m_volumeStartSliceIndexSpinBox->setEnabled(validFlag);
    m_volumeEndSliceIndexSpinBox->setEnabled(validFlag);

    updateMontageOutputWidget();
}

/**
 * Called when value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::volumeStartSliceIndexSpinBoxValueChanged(int value)
{
    FunctionResultValue<Vector3D> result(volumeSliceIndexToCoordinate(value));
    if (result.isOk()) {
        m_volumeStartSliceCoordinateLabel->setText(result.getValue().toString());
    }
    else {
        m_volumeStartSliceCoordinateLabel->setText("Invalid");
    }
    updateMontageOutputWidget();
}

/**
 * Called when value changed
 * @param value
 *    New value
 */
void
VolumeMontageSetupDialog::volumeEndSliceIndexSpinBoxValueChanged(int value)
{
    FunctionResultValue<Vector3D> result(volumeSliceIndexToCoordinate(value));
    if (result.isOk()) {
        m_volumeEndSliceCoordinateLabel->setText(result.getValue().toString());
    }
    else {
        m_volumeEndSliceCoordinateLabel->setText("Invalid");
    }
    updateMontageOutputWidget();
}


/**
 * Convert a volume slice index to a coordinate for the currently selected axis
 * @param sliceIndex
 *   The slice index
 * @return
 *   The slice coordinate
 */
FunctionResultValue<Vector3D>
VolumeMontageSetupDialog::volumeSliceIndexToCoordinate(const int32_t sliceIndex) const
{
    Vector3D xyz;
    bool validFlag(false);
    
    CaretDataFile* cdf(m_volumeFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        VolumeMappableInterface* vmi(dynamic_cast<VolumeMappableInterface*>(cdf));
        if (vmi != NULL) {
            int64_t dimI, dimJ, dimK, dimTime, dimCompontents;
            vmi->getDimensions(dimI, dimJ, dimK, dimTime, dimCompontents);
            const int64_t middleI((dimI > 1) ? (dimI / 2) : dimI);
            const int64_t middleJ((dimJ > 1) ? (dimJ / 2) : dimJ);
            const int64_t middleK((dimK > 1) ? (dimK / 2) : dimK);

            const int32_t selectedIndex(m_montageInputVolumeSliceAxisComboBox->currentIndex());
            const int32_t viewPlaneInteger(m_montageInputVolumeSliceAxisComboBox->itemData(selectedIndex).toInt());
            const VolumeSliceViewPlaneEnum::Enum slicePlane(VolumeSliceViewPlaneEnum::fromIntegerCode(viewPlaneInteger, NULL));
            float x(0.0), y(0.0), z(0.0);
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    vmi->indexToSpace(middleI, middleJ, sliceIndex, x, y, z);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    vmi->indexToSpace(middleI, sliceIndex, middleK, x, y, z);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    vmi->indexToSpace(sliceIndex, middleJ, middleK, x, y, z);
                    break;
            }
            xyz.set(x, y, z);
            validFlag = true;
        }
    }
    
    return FunctionResultValue<Vector3D>(xyz, "", validFlag);
}

/**
 * Convert a volume slice index to a coordinate for the currently selected axis
 * @param sliceIndex
 *   The slice index
 * @return
 *   The slice coordinate
 */
FunctionResultValue<int32_t>
VolumeMontageSetupDialog::volumeCoordinateToSliceIndex(const Vector3D& xyz) const
{
    int32_t sliceIndex(0);
    bool validFlag(false);
    
    CaretDataFile* cdf(m_volumeFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        VolumeMappableInterface* vmi(dynamic_cast<VolumeMappableInterface*>(cdf));
        if (vmi != NULL) {
            int64_t ijk[3] { 0, 0, 0 };
            vmi->enclosingVoxel(xyz, ijk);
            
            const int32_t viewPlaneInteger(m_montageInputVolumeSliceAxisComboBox->currentData().toInt());
            const VolumeSliceViewPlaneEnum::Enum slicePlane(VolumeSliceViewPlaneEnum::fromIntegerCode(viewPlaneInteger, NULL));
            float x(0.0), y(0.0), z(0.0);
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceIndex = ijk[2];
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceIndex = ijk[1];
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceIndex = ijk[0];
                    break;
            }
            validFlag = true;
        }
    }
    
    return FunctionResultValue<int32_t>(sliceIndex, "", validFlag);
}

/**
 * Called when the volume slice view plane is changed
 */
void
VolumeMontageSetupDialog::montageInputVolumeSliceAxisComboBoxValueChanged(int itemIndex)
{
    const VolumeSliceViewPlaneEnum::Enum slicePlane(VolumeSliceViewPlaneEnum::fromIntegerCode(itemIndex, NULL));
    
    const bool enableHistologyWidgetFlag(slicePlane == VolumeSliceViewPlaneEnum::ALL);
    m_volumeSliceInputWidget->setEnabled( ! enableHistologyWidgetFlag);
    m_histologyWidget->setEnabled(enableHistologyWidgetFlag);

    volumeFileSelected(m_volumeFileSelectionComboBox->getSelectedFile());
}

/**
 * Update content of montage output widget
 */
void
VolumeMontageSetupDialog::updateMontageOutputWidget()
{
    if (m_dialogCreationInProgressFlag) {
        return;
    }
    
    int64_t indexI(0);
    int64_t indexJ(0);
    int64_t indexK(0);
    Vector3D stereoXYZ(0.0, 0.0, 0.0);
    float stepThickness(0.0);
    float numberOfSlicesPerStep(0.0);
    Vector3D rotationAngles(0.0, 0.0, 0.0);
    AString sliceViewPlaneName;
    
    VolumeSliceViewPlaneEnum::Enum slicePlane(VolumeMontageSetupDialog::getSelectedVolumeMontageAxis());
    bool histologyFlag(slicePlane == VolumeSliceViewPlaneEnum::ALL);
    FunctionResultValue<Vector3D> resultStartCoord(Vector3D(), "", false);
    FunctionResultValue<Vector3D> resultEndCoord(Vector3D(), "", false);
    if (histologyFlag) {
        resultStartCoord = histologySliceIndexToCoordinate(m_histologyStartSliceIndexSpinBox->value());
        resultEndCoord   = histologySliceIndexToCoordinate(m_histologyEndSliceIndexSpinBox->value());
        if (resultStartCoord.isOk()
            && resultEndCoord.isOk()) {
        }
        
    }
    else {
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                break;
        }
        
        const int32_t startSliceIndex(m_volumeStartSliceIndexSpinBox->value());
        const int32_t endSliceIndex(m_volumeEndSliceIndexSpinBox->value());
        
        resultStartCoord = volumeSliceIndexToCoordinate(startSliceIndex);
        resultEndCoord   = volumeSliceIndexToCoordinate(endSliceIndex);
    }
    
    if (resultStartCoord.isOk()
        && resultEndCoord.isOk()) {
        const Vector3D startCoord(resultStartCoord.getValue());
        const Vector3D endCoord(resultEndCoord.getValue());
        const Vector3D middleXYZ((endCoord
                                  + startCoord)
                                 / 2.0);
        CaretDataFile* cdf(m_volumeFileSelectionComboBox->getSelectedFile());
        if (cdf != NULL) {
            VolumeMappableInterface* vmi(dynamic_cast<VolumeMappableInterface*>(cdf));
            if (vmi != NULL) {
                const float distance((endCoord - startCoord).length());
                const float numMontageSlices(m_montageInputRowsSpinBox->value()
                                             * m_montageInputColumnsSpinBox->value());
                if (numMontageSlices > 1.0) {
                    stepThickness = distance / numMontageSlices;
                }
                
                int64_t ijk[3] { 0, 0, 0 };
                vmi->enclosingVoxel(middleXYZ, ijk);
                indexI = ijk[0];
                indexJ = ijk[1];
                indexK = ijk[2];
                stereoXYZ = middleXYZ;
                
                int64_t sliceZero[3] { 0, 0, 0 };
                int64_t sliceOne[3] { 1, 1, 1 };
                Vector3D xyzZero;
                Vector3D xyzOne;
                vmi->indexToSpace(sliceZero, xyzZero);
                vmi->indexToSpace(sliceOne,  xyzOne);
                
                /* ALL is histology axis */
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                    {
                        FunctionResultValue<std::pair<VolumeSliceViewPlaneEnum::Enum,bool>> slicePlaneResult(histologyOrientationToVolumeSliceViewPlane());
                        bool flipFlag(false);
                        if (slicePlaneResult.isOk()) {
                            slicePlane = slicePlaneResult.getValue().first;
                            flipFlag   = slicePlaneResult.getValue().second;
                        }
                        const Vector3D histologyVector((endCoord - startCoord).normal());
                        
                        Vector3D sliceVector(0.0, 0.0, 1.0);
                        switch (slicePlane) {
                            case VolumeSliceViewPlaneEnum::ALL:
                                break;
                            case VolumeSliceViewPlaneEnum::AXIAL:
                                sliceVector.set(0.0, 0.0, -1.0);
                                break;
                            case VolumeSliceViewPlaneEnum::CORONAL:
                                sliceVector.set(0.0, 1.0, 0.0);
                                break;
                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                sliceVector.set(1.0, 0.0, 0.0);
                                break;
                        }
                        if (flipFlag) {
                            sliceVector = -sliceVector;
                        }
                        
                        const QVector3D hv(histologyVector[0],
                                           histologyVector[1],
                                           histologyVector[2]);
                        const QVector3D sv(sliceVector[0],
                                           sliceVector[1],
                                           sliceVector[2]);
                        const QQuaternion q(QQuaternion::rotationTo(sv, hv));
                        QVector3D eulerAngles(q.toEulerAngles());
                        rotationAngles.set(eulerAngles[0],
                                           eulerAngles[1],
                                           -eulerAngles[2]); /* look down Z-axis (positive to negative) */
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                    case VolumeSliceViewPlaneEnum::CORONAL:
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        rotationAngles.set(0.0, 0.0, 0.0);
                        break;
                }
                float sliceThickness(0.0);
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        sliceViewPlaneName = "Axial";
                        sliceThickness = sliceOne[2] - sliceZero[2];
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        sliceViewPlaneName = "Coronal";
                        sliceThickness = sliceOne[1] - sliceZero[1];
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        sliceViewPlaneName = "Parasagittal";
                        sliceThickness = sliceOne[0] - sliceZero[0];
                        break;
                }
                
                if (sliceThickness != 0.0) {
                    numberOfSlicesPerStep = stepThickness / sliceThickness;
                }
            }
        }
    }
    
    QSignalBlocker paraIndexBlocker(m_montageOutputParagittalSliceIndexSpinBox);
    m_montageOutputParagittalSliceIndexSpinBox->setValue(indexI);
    QSignalBlocker coronalIndexBlocker(m_montageOutputCoronalSliceIndexSpinBox);
    m_montageOutputCoronalSliceIndexSpinBox->setValue(indexJ);
    QSignalBlocker axialIndexBlocker(m_montageOutputAxialSliceIndexSpinBox);
    m_montageOutputAxialSliceIndexSpinBox->setValue(indexK);
    
    QSignalBlocker paraCoordBlocker(m_montageOutputParagittalSliceCoordinateSpinBox);
    m_montageOutputParagittalSliceCoordinateSpinBox->setValue(stereoXYZ[0]);
    QSignalBlocker coronalCoordBlocker(m_montageOutputCoronalSliceCoordinateSpinBox);
    m_montageOutputCoronalSliceCoordinateSpinBox->setValue(stereoXYZ[1]);
    QSignalBlocker axialCoordBlocker(m_montageOutputAxialSliceCoordinateSpinBox);
    m_montageOutputAxialSliceCoordinateSpinBox->setValue(stereoXYZ[2]);
    
    m_montageOutputSliceStepLabel->setText(QString::number(numberOfSlicesPerStep, 'f', 2));
    m_montageOutputSliceSpacingLabel->setText(QString::number(stepThickness, 'f', 3));
    
    m_montageOutputRotationAnglesLabel->setText(rotationAngles.toString());
    m_montageOutputSliceViewPlaneLabel->setText(sliceViewPlaneName);
}

/**
 * Called when the ok button is clicked
 */
void
VolumeMontageSetupDialog::okButtonClicked()
{
}
