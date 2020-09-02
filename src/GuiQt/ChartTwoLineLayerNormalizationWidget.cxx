
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
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

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
    const QString toolTipText("Normalize: (y - mean + demean) / stddev");
 
    m_normalizeCheckBox = new QCheckBox("Normalize");
    m_normalizeCheckBox->setToolTip(toolTipText);
    QObject::connect(m_normalizeCheckBox, &QCheckBox::clicked,
                     this, [=] { this->valueChanged(); });
    
    QLabel* demeanLabel = new QLabel("Demean");
    m_demeanSpinBox = new QDoubleSpinBox();
    m_demeanSpinBox->setDecimals(4);
    m_demeanSpinBox->setSingleStep(1.0);
    m_demeanSpinBox->setMinimum(-std::numeric_limits<float>::max());
    m_demeanSpinBox->setMaximum( std::numeric_limits<float>::max());
    m_demeanSpinBox->setToolTip(toolTipText);
    m_demeanSpinBox->setMaximumWidth(100);
    QObject::connect(m_demeanSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, [=] { this->valueChanged(); });

    m_meanDevLabel = new QLabel("");
    
    QGridLayout* layout = new QGridLayout(this);
    int32_t row(0);
    layout->addWidget(m_normalizeCheckBox, row, 0, 1, 2);
    row++;
    layout->addWidget(demeanLabel, row, 0);
    layout->addWidget(m_demeanSpinBox, row, 1);
    row++;
    layout->addWidget(m_meanDevLabel, row, 0, 1, 2, Qt::AlignLeft);
    row++;
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
    m_chartTwoOverlay = chartTwoOverlay;
    
    QString meanDevText;
    
    bool validFlag(false);
    if (m_chartTwoOverlay != NULL) {
        m_normalizeCheckBox->setChecked(m_chartTwoOverlay->isLineChartNormalizationEnabled());
        QSignalBlocker blocker(m_demeanSpinBox);
        m_demeanSpinBox->setValue(m_chartTwoOverlay->getLineChartNormalizationDemeanValue());
        
        float mean(0.0), dev(0.0);
        const ChartTwoDataCartesian* cartData = m_chartTwoOverlay->getLineLayerChartCartesianData();
        CaretAssert(cartData);
        const GraphicsPrimitiveV3f* primitive = cartData->getGraphicsPrimitive();
        primitive->getMeanAndStandardDeviationForY(mean, dev);
        
        const QString muCharacter(QChar(0x03BC));
        const QString sigmaCharacter(QChar(0x03C3));
        meanDevText = (muCharacter + ": "
                       + QString::number(mean, 'f', 4)
                       + " " + sigmaCharacter + ": "
                       + QString::number(dev, 'f', 4));
        validFlag = true;
    }
    
    m_normalizeCheckBox->setEnabled(validFlag);
    m_demeanSpinBox->setEnabled(validFlag);
    m_meanDevLabel->setEnabled(validFlag);
    m_meanDevLabel->setText(meanDevText);
}

/**
 * Called when a value is changed
 */
void
ChartTwoLineLayerNormalizationWidget::valueChanged()
{
    if (m_chartTwoOverlay != NULL) {
        m_chartTwoOverlay->setLineChartNormalizationEnabled(m_normalizeCheckBox->isChecked());
        m_chartTwoOverlay->setLineChartNormalizationDemeanValue(m_demeanSpinBox->value());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}
