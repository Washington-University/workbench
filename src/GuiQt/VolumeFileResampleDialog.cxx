
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __VOLUME_FILE_RESAMPLE_DIALOG_DECLARE__
#include "VolumeFileResampleDialog.h"
#undef __VOLUME_FILE_RESAMPLE_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "VolumeFile.h"
#include "WuQFactory.h"

using namespace caret;


    
/**
 * \class caret::VolumeFileResampleDialog 
 * \brief Dialog for resampling a volume file to a different spacing
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dimensions
 *    Dimensions of original volume
 * @param origin
 *    Origin of original volume
 * @param spacing
 *    Spacing of original volume
 * @param spacingSingleStep
 *    Single step for spacing
 * @param spacingDecimals
 *    Decimals for spacing
 */
VolumeFileResampleDialog::VolumeFileResampleDialog(const Vector3D& dimensions,
                                                   const Vector3D& origin,
                                                   const Vector3D& spacing,
                                                   const float spacingSingleStep,
                                                   const int32_t spacingDecimals,
                                                   QWidget* parent)
: WuQDialogModal("Resample Volume",
                 parent),
m_inputDimensions(dimensions),
m_inputOrigin(origin),
m_inputSpacing(spacing),
m_newDimensions(dimensions),
m_newOrigin(origin),
m_newSpacing(spacing)
{
    m_blockVoxelEdgeLabelUpdateFlag = true;
    const double bigDouble = 100000000.0;
    const int SPIN_BOX_WIDTH = 80;

    QLabel* dimensionLabel(new QLabel("New Dimensions: "));
    m_newDimensionXLabel = new QLabel();
    m_newDimensionYLabel = new QLabel();
    m_newDimensionZLabel = new QLabel();

    QLabel* originLabel(new QLabel("New Origin: "));
    m_newOriginXLabel = new QLabel();
    m_newOriginYLabel = new QLabel();
    m_newOriginZLabel = new QLabel();

    QLabel* spacingLabel = new QLabel("New Spacing: ");
    m_spacingXSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, spacingSingleStep, spacingDecimals);
    m_spacingXSpinBox->setToolTip("Size of voxel along X-axis");
    m_spacingXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingXSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingYSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, spacingSingleStep, spacingDecimals);
    m_spacingYSpinBox->setToolTip("Size of voxel along Y-axis");
    m_spacingYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingYSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingZSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, spacingSingleStep, spacingDecimals);
    m_spacingZSpinBox->setToolTip("Size of voxel along Z-axis");
    m_spacingZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_spacingZSpinBox->setKeyboardTracking(true);
    QObject::connect(m_spacingZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=] { this->updateVoxelEdgeLabels(); } );
    m_spacingXSpinBox->setValue(spacing[0]);
    m_spacingYSpinBox->setValue(spacing[1]);
    m_spacingZSpinBox->setValue(spacing[2]);
    
    QLabel* xLabel = new QLabel("X");
    QLabel* yLabel = new QLabel("Y");
    QLabel* zLabel = new QLabel("Z");
    
    QLabel* firstEdgeLabel(new QLabel("First Voxel Edge: "));
    QLabel* lastEdgeLabel(new QLabel("Last Voxel Edge: "));
    m_xFirstVoxelEdgeLabel = new QLabel();
    m_xLastVoxelEdgeLabel  = new QLabel();
    m_yFirstVoxelEdgeLabel = new QLabel();
    m_yLastVoxelEdgeLabel  = new QLabel();
    m_zFirstVoxelEdgeLabel = new QLabel();
    m_zLastVoxelEdgeLabel  = new QLabel();

    const QString helpText("Enter new spacing (voxel size) values.  All other values will update "
                           "as the spacing values are changed.");
    QLabel* helpLabel(new QLabel(helpText));
    helpLabel->setWordWrap(true);
    
    const int COL_LABEL = 0;
    const int COL_X = COL_LABEL + 1;
    const int COL_Y = COL_X + 1;
    const int COL_Z = COL_Y + 1;
    
    QWidget* dialogWidget(new QWidget());
    QGridLayout* paramsLayout = new QGridLayout(dialogWidget);
    paramsLayout->setVerticalSpacing(5);
    
    int paramsRow = 0;
    paramsLayout->addWidget(xLabel, paramsRow, COL_X, Qt::AlignHCenter);
    paramsLayout->addWidget(yLabel, paramsRow, COL_Y, Qt::AlignHCenter);
    paramsLayout->addWidget(zLabel, paramsRow, COL_Z, Qt::AlignHCenter);
    paramsRow++;
    
    paramsLayout->addWidget(dimensionLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_newDimensionXLabel, paramsRow, COL_X, Qt::AlignRight);
    paramsLayout->addWidget(m_newDimensionYLabel, paramsRow, COL_Y, Qt::AlignRight);
    paramsLayout->addWidget(m_newDimensionZLabel, paramsRow, COL_Z, Qt::AlignRight);
    paramsRow++;

    paramsLayout->addWidget(originLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_newOriginXLabel, paramsRow, COL_X, Qt::AlignRight);
    paramsLayout->addWidget(m_newOriginYLabel, paramsRow, COL_Y, Qt::AlignRight);
    paramsLayout->addWidget(m_newOriginZLabel, paramsRow, COL_Z, Qt::AlignRight);
    paramsRow++;
    
    paramsLayout->addWidget(spacingLabel, paramsRow, COL_LABEL);
    paramsLayout->addWidget(m_spacingXSpinBox, paramsRow, COL_X);
    paramsLayout->addWidget(m_spacingYSpinBox, paramsRow, COL_Y);
    paramsLayout->addWidget(m_spacingZSpinBox, paramsRow, COL_Z);
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
    
    paramsLayout->addWidget(new QLabel(""), paramsRow, COL_LABEL); /* space */
    paramsRow++;
    paramsLayout->addWidget(helpLabel, paramsRow, COL_LABEL, 1, 4);
    paramsRow++;
    
    setCentralWidget(dialogWidget, ScrollAreaStatus::SCROLL_AREA_NEVER);
    m_blockVoxelEdgeLabelUpdateFlag = false;
    updateVoxelEdgeLabels();
}

/**
 * Destructor.
 */
VolumeFileResampleDialog::~VolumeFileResampleDialog()
{
}

/**
 * Update the voxel edges as values are changed
 */
void
VolumeFileResampleDialog::updateVoxelEdgeLabels()
{
    if (m_blockVoxelEdgeLabelUpdateFlag) {
        return;
    }
    
    m_newSpacing.set(m_spacingXSpinBox->value(),
                     m_spacingYSpinBox->value(),
                     m_spacingZSpinBox->value());
    if ((m_newSpacing[0] == 0.0)
        || (m_newSpacing[1] == 0.0)
        || (m_newSpacing[2] == 0.0)) {
        return;
    }
    
    Vector3D oldFirstVoxelEdge;
    Vector3D oldLastVoxelEdge;
    VolumeFile::dimensionOriginSpacingXyzToVoxelEdges(m_inputDimensions,
                                                      m_inputOrigin,
                                                      m_inputSpacing,
                                                      oldFirstVoxelEdge,
                                                      oldLastVoxelEdge);
    
    const Vector3D newHalfSpacing(m_newSpacing / 2.0);
    m_newOrigin = (oldFirstVoxelEdge + newHalfSpacing);
    const Vector3D spacingRatio(m_inputSpacing[0] / m_newSpacing[0],
                                m_inputSpacing[1] / m_newSpacing[1],
                                m_inputSpacing[2] / m_newSpacing[2]);
    
    /*
     * REOUND OR CEIL !!!!
     */
    m_newDimensions.set(std::round(m_inputDimensions[0] * spacingRatio[0]),
                        std::round(m_inputDimensions[1] * spacingRatio[1]),
                        std::round(m_inputDimensions[2] * spacingRatio[2]));

    
    Vector3D firstVoxelEdge;
    Vector3D lastVoxelEdge;
    VolumeFile::dimensionOriginSpacingXyzToVoxelEdges(m_newDimensions,
                                                      m_newOrigin,
                                                      m_newSpacing,
                                                      firstVoxelEdge,
                                                      lastVoxelEdge);
    const int32_t decimals(3);
    const AString shiftLeftText("    ");
    m_xFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[0], 'f', decimals) + shiftLeftText);
    m_yFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[1], 'f', decimals) + shiftLeftText);
    m_zFirstVoxelEdgeLabel->setText(AString::number(firstVoxelEdge[2], 'f', decimals) + shiftLeftText);
    m_xLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[0], 'f', decimals) + shiftLeftText);
    m_yLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[1], 'f', decimals) + shiftLeftText);
    m_zLastVoxelEdgeLabel->setText(AString::number(lastVoxelEdge[2], 'f', decimals) + shiftLeftText);
    
    m_newDimensionXLabel->setText(AString::number(m_newDimensions[0], 'f', 0) + shiftLeftText);
    m_newDimensionYLabel->setText(AString::number(m_newDimensions[1], 'f', 0) + shiftLeftText);
    m_newDimensionZLabel->setText(AString::number(m_newDimensions[2], 'f', 0) + shiftLeftText);
    
    const int32_t spacingDecimals(m_spacingXSpinBox->decimals());
    m_newOriginXLabel->setText(AString::number(m_newOrigin[0], 'f', spacingDecimals) + shiftLeftText);
    m_newOriginYLabel->setText(AString::number(m_newOrigin[1], 'f', spacingDecimals) + shiftLeftText);
    m_newOriginZLabel->setText(AString::number(m_newOrigin[2], 'f', spacingDecimals) + shiftLeftText);
}

/**
 * @return The new dimensions
 */
Vector3D
VolumeFileResampleDialog::getDimensions() const
{
    return m_newDimensions;
}

/**
 * @return The new origin
 */
Vector3D
VolumeFileResampleDialog::getOrigin() const
{
    return m_newOrigin;
}

/**
 * @return The new spacing
 */
Vector3D
VolumeFileResampleDialog::getSpacing() const
{
    return m_newSpacing;
}

/**
 * Gets called when the ok button is clicked.
 */
void
VolumeFileResampleDialog::okButtonClicked()
{
    WuQDialog::okButtonClicked();
}
