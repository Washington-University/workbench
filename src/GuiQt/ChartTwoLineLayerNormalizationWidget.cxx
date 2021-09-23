
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_DECLARE__
#include "ChartTwoLineLayerNormalizationWidget.h"
#undef __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_DECLARE__

#include <limits>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>

#include "CaretAssert.h"
#include "ChartTwoDataCartesian.h"
#include "ChartTwoOverlay.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GraphicsPrimitiveV3f.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ChartTwoLineLayerNormalizationWidget
 * \brief Widget for editing chart line layer normalization parameters
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartTwoLineLayerNormalizationWidget::ChartTwoLineLayerNormalizationWidget()
: QWidget()
{
    /*
     * Absolute values checkbox
     */
    m_absoluteValueEnabledCheckBox = new QCheckBox("Absolute Values");
    QObject::connect(m_absoluteValueEnabledCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoLineLayerNormalizationWidget::absoluteValueEnabledCheckBoxClicked);
    
    /*
     * New Mean
     */
    m_newMeanEnabledCheckBox = new QCheckBox("New Mean");
    QObject::connect(m_newMeanEnabledCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoLineLayerNormalizationWidget::newMeanEnabledCheckBoxClicked);
    m_newMeanSpinBox = new QDoubleSpinBox();
    m_newMeanSpinBox->setDecimals(4);
    m_newMeanSpinBox->setSingleStep(0.1);
    m_newMeanSpinBox->setRange(-std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max());
    m_newMeanSpinBox->setMaximumWidth(100);
    QObject::connect(m_newMeanSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ChartTwoLineLayerNormalizationWidget::newMeanValueChanged);

    /*
     * New Deviation
     */
    m_newDeviationEnabledCheckBox = new QCheckBox("New Deviation");
    QObject::connect(m_newDeviationEnabledCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoLineLayerNormalizationWidget::newDeviationEnabledCheckBoxClicked);
    m_newDeviationSpinBox = new QDoubleSpinBox();
    m_newDeviationSpinBox->setDecimals(4);
    m_newDeviationSpinBox->setSingleStep(0.1);
    m_newDeviationSpinBox->setRange(-std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max());
    m_newDeviationSpinBox->setMaximumWidth(100);
    QObject::connect(m_newDeviationSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ChartTwoLineLayerNormalizationWidget::newDeviationValueChanged);

    /*
     * Data Offset
     */
    m_dataOffsetCheckBox = new QCheckBox("Data Offset");
    QObject::connect(m_dataOffsetCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoLineLayerNormalizationWidget::dataOffsetCheckBoxClicked);
    m_dataOffsetSpinBox = new QDoubleSpinBox();
    m_dataOffsetSpinBox->setDecimals(4);
    m_dataOffsetSpinBox->setSingleStep(0.1);
    m_dataOffsetSpinBox->setRange(-std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max());
    m_dataOffsetSpinBox->setMaximumWidth(100);
    QObject::connect(m_dataOffsetSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ChartTwoLineLayerNormalizationWidget::dataOffsetValueChanged);
    
    /*
     * Multiply Deviation
     */
    m_multiplyDeviationCheckBox = new QCheckBox("Multiply Deviation");
    QObject::connect(m_multiplyDeviationCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoLineLayerNormalizationWidget::multiplyDeviationCheckBoxClicked);
    m_multiplyDeviationSpinBox = new QDoubleSpinBox();
    m_multiplyDeviationSpinBox->setDecimals(4);
    m_multiplyDeviationSpinBox->setSingleStep(0.1);
    m_multiplyDeviationSpinBox->setRange(-std::numeric_limits<float>::max(),
                                         std::numeric_limits<float>::max());
    m_multiplyDeviationSpinBox->setMaximumWidth(100);
    QObject::connect(m_multiplyDeviationSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ChartTwoLineLayerNormalizationWidget::multiplyDeviationValueChanged);

    /*
     * Label that shows mean and deviation
     */
    m_meanDevLabel = new QLabel("");
    
    /*
     * Label explaining GUI controls
     */
    QLabel* descripionLabel = new QLabel(GraphicsPrimitive::getNewMeanDeviationOperationDescriptionInHtml());
    descripionLabel->setWordWrap(true);
    descripionLabel->setMaximumWidth(400);
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    int32_t row(0);
    layout->addWidget(m_absoluteValueEnabledCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(m_newMeanEnabledCheckBox, row, 0);
    layout->addWidget(m_newMeanSpinBox, row, 1, Qt::AlignLeft);
    row++;
    layout->addWidget(m_newDeviationEnabledCheckBox, row, 0);
    layout->addWidget(m_newDeviationSpinBox, row, 1, Qt::AlignLeft);
    row++;
    layout->addWidget(m_multiplyDeviationCheckBox, row, 0);
    layout->addWidget(m_multiplyDeviationSpinBox, row, 1, Qt::AlignLeft);
    row++;
    layout->addWidget(m_dataOffsetCheckBox, row, 0);
    layout->addWidget(m_dataOffsetSpinBox, row, 1, Qt::AlignLeft);
    row++;
    layout->addWidget(m_meanDevLabel, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    layout->addWidget(descripionLabel, row, 0, 1, 2, Qt::AlignLeft);
    row++;

    m_blockUpdatesFlag = false;
}

/**
 * Destructor.
 */
ChartTwoLineLayerNormalizationWidget::~ChartTwoLineLayerNormalizationWidget()
{
}

/**
 * Update the content of the widget
 * @param chartTwoOverlay
 */
void
ChartTwoLineLayerNormalizationWidget::updateContent(ChartTwoOverlay* chartTwoOverlay)
{
    if (m_blockUpdatesFlag) {
        return;
    }
    
    m_chartTwoOverlay = chartTwoOverlay;
    
    QString meanDevText;
    
    bool validFlag(false);
    if (m_chartTwoOverlay != NULL) {
        m_absoluteValueEnabledCheckBox->setChecked(m_chartTwoOverlay->isLineChartNormalizationAbsoluteValueEnabled());
        
        m_newMeanEnabledCheckBox->setChecked(m_chartTwoOverlay->isLineChartNewMeanEnabled());
        QSignalBlocker meanBlocker(m_newMeanSpinBox);
        m_newMeanSpinBox->setValue(m_chartTwoOverlay->getLineChartNewMeanValue());
        
        m_newDeviationEnabledCheckBox->setChecked(m_chartTwoOverlay->isLineChartNewDeviationEnabled());
        QSignalBlocker devBlocker(m_newDeviationSpinBox);
        m_newDeviationSpinBox->setValue(m_chartTwoOverlay->getLineChartNewDeviationValue());
        
        m_dataOffsetCheckBox->setChecked(m_chartTwoOverlay->isLineChartDataOffsetEnabled());
        QSignalBlocker addMeanBlocker(m_dataOffsetSpinBox);
        m_dataOffsetSpinBox->setValue(m_chartTwoOverlay->getLineChartDataOffsetValue());
        
        m_multiplyDeviationCheckBox->setChecked(m_chartTwoOverlay->isLineChartMultiplyDeviationEnabled());
        QSignalBlocker multDevBlocker(m_multiplyDeviationSpinBox);
        m_multiplyDeviationSpinBox->setValue(m_chartTwoOverlay->getLineChartMultiplyDeviationValue());
        
        float mean(0.0), dev(0.0);
        const ChartTwoDataCartesian* cartData = m_chartTwoOverlay->getLineLayerChartMapFileCartesianData();
        CaretAssert(cartData);
        const GraphicsPrimitiveV3f* primitive = cartData->getGraphicsPrimitive();
        primitive->getMeanAndStandardDeviationForY(mean, dev);
        
        const QString muCharacter("Data Mean"); //QChar(0x03BC));
        const QString sigmaCharacter("Data Deviation"); //QChar(0x03C3));
        meanDevText = (muCharacter + ": "
                       + QString::number(mean, 'f', 4)
                       + ", " + sigmaCharacter + ": "
                       + QString::number(dev, 'f', 4));
        validFlag = true;
    }
    
    this->setEnabled(validFlag);

    m_meanDevLabel->setText(meanDevText);
}

/**
 * Called when new mean checkbox is clicked
 * @param clicked
 *    New clicked status
 */
void
ChartTwoLineLayerNormalizationWidget::newMeanEnabledCheckBoxClicked(bool clicked)
{
    m_chartTwoOverlay->setLineChartNewMeanEnabled(clicked);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when new deviation checkbox is clicked
 * @param clicked
 *    New clicked status
 */
void
ChartTwoLineLayerNormalizationWidget::newDeviationEnabledCheckBoxClicked(bool clicked)
{
    m_chartTwoOverlay->setLineChartNewDeviationEnabled(clicked);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when new deviation checkbox is clicked
 * @param clicked
 *    New clicked status
 */
void
ChartTwoLineLayerNormalizationWidget::absoluteValueEnabledCheckBoxClicked(bool clicked)
{
    m_chartTwoOverlay->setLineChartNormalizationAbsoluteValueEnabled(clicked);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when new mean value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::newMeanValueChanged(double value)
{
    m_chartTwoOverlay->setLineChartNewMeanValue(value);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when new deviation value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::newDeviationValueChanged(double value)
{
    m_chartTwoOverlay->setLineChartNewDeviationValue(value);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when add to mean checkbox is clicked
 * @param clicked
 *    New clicked status
 */
void
ChartTwoLineLayerNormalizationWidget::dataOffsetCheckBoxClicked(bool clicked)
{
    m_chartTwoOverlay->setLineChartDataOffsetEnabled(clicked);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when add to mean value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::dataOffsetValueChanged(double value)
{
    m_chartTwoOverlay->setLineChartDataOffsetValue(value);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when multiply deviation checkbox is clicked
 * @param clicked
 *    New clicked status
 */
void
ChartTwoLineLayerNormalizationWidget::multiplyDeviationCheckBoxClicked(bool clicked)
{
    m_chartTwoOverlay->setLineChartMultiplyDeviationEnabled(clicked);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when multiply deviation value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::multiplyDeviationValueChanged(double value)
{
    m_chartTwoOverlay->setLineChartMultiplyDeviationValue(value);
    updateGraphics();
    updateToolBarChartAxes();
}

/**
 * Called when new mean value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::updateGraphics()
{
    
    m_blockUpdatesFlag = true;
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    m_blockUpdatesFlag = false;
}

/**
 * Called when new mean value is changed
 * @param value
 *    New value
 */
void
ChartTwoLineLayerNormalizationWidget::updateToolBarChartAxes()
{
    m_blockUpdatesFlag = true;
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_TOOLBAR_CHART_ORIENTED_AXES_UPDATE);
    m_blockUpdatesFlag = false;
    
}



