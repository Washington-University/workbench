
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

    m_meanDevLabel = new QLabel("");
    
    QGridLayout* layout = new QGridLayout(this);
    int32_t row(0);
    layout->addWidget(m_newMeanEnabledCheckBox, row, 0);
    layout->addWidget(m_newMeanSpinBox, row, 1);
    row++;
    layout->addWidget(m_newDeviationEnabledCheckBox, row, 0);
    layout->addWidget(m_newDeviationSpinBox, row, 1);
    row++;
    layout->addWidget(m_meanDevLabel, row, 0, 1, 2, Qt::AlignLeft);
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
        m_newMeanEnabledCheckBox->setChecked(m_chartTwoOverlay->isLineChartNewMeanEnabled());
        QSignalBlocker meanBlocker(m_newMeanSpinBox);
        m_newMeanSpinBox->setValue(m_chartTwoOverlay->getLineChartNewMeanValue());
        
        m_newDeviationEnabledCheckBox->setChecked(m_chartTwoOverlay->isLineChartNewDeviationEnabled());
        QSignalBlocker devBlocker(m_newDeviationSpinBox);
        m_newDeviationSpinBox->setValue(m_chartTwoOverlay->getLineChartNewDeviationValue());
        
        float mean(0.0), dev(0.0);
        const ChartTwoDataCartesian* cartData = m_chartTwoOverlay->getLineLayerChartMapFileCartesianData();
        CaretAssert(cartData);
        const GraphicsPrimitiveV3f* primitive = cartData->getGraphicsPrimitive();
        primitive->getMeanAndStandardDeviationForY(mean, dev);
        
        const QString muCharacter(QChar(0x03BC));
        const QString sigmaCharacter(QChar(0x03C3));
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
 * Called when a value is changed
 */
void
ChartTwoLineLayerNormalizationWidget::valueChanged()
{
    if (m_chartTwoOverlay != NULL) {
        m_chartTwoOverlay->setLineChartNewMeanEnabled(m_newMeanEnabledCheckBox->isChecked());
        m_chartTwoOverlay->setLineChartNewMeanValue(m_newMeanSpinBox->value());
        m_chartTwoOverlay->setLineChartNewDeviationEnabled(m_newDeviationEnabledCheckBox->isChecked());
        m_chartTwoOverlay->setLineChartNewDeviationValue(m_newDeviationSpinBox->value());
        
        updateGraphics();
//        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//        m_blockUpdatesFlag = true;
////        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//        m_blockUpdatesFlag = false;
    }
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



