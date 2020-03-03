
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

#define __WU_Q_COLOR_EDITOR_WIDGET_DECLARE__
#include "WuQColorEditorWidget.h"
#undef __WU_Q_COLOR_EDITOR_WIDGET_DECLARE__

#include <iostream>

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "MathFunctions.h"
#include "WuQImageLabel.h"
using namespace caret;


    
/**
 * \class caret::WuQColorEditorWidget
 * \brief Widget for editing color
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQColorEditorWidget::WuQColorEditorWidget(QWidget* parent)
: QWidget(parent)
{
    QWidget* hueSaturationWidget = createHueSaturationColorLabel();
    QWidget* valueWidget         = createValueColorLabel();
    
    QWidget* controlsWidget = createControlsWidget();
    
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(hueSaturationWidget, 0, 0);
    layout->addWidget(valueWidget, 0, 1);
    layout->addWidget(controlsWidget, 1, 0, 1, 2);
    
    m_currentColor.setRgb(255, 0, 0);
    updateControls();

    setSizePolicy(sizePolicy().horizontalPolicy(),
                          QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
WuQColorEditorWidget::~WuQColorEditorWidget()
{
}

/**
 * Set the current color for editing
 * @param color
 * New color for editing
 */
void
WuQColorEditorWidget::setCurrentColor(const QColor& color)
{
    m_currentColor = color;
    
    updateControls();
}

/**
 * Set the current color for editing
 * @param red
 *     New red component for editing
 * @param green
 *     New green component for editing
 * @param blue
 *     New blue component for editing
 */
void
WuQColorEditorWidget::setCurrentColor(const uint8_t red,
                                const uint8_t green,
                                const uint8_t blue)
{
    setCurrentColor(QColor(red, green, blue));
}

/**
 * @return New instance of the controls widget
 */
QWidget*
WuQColorEditorWidget::createControlsWidget()
{
    m_colorSwatchWidget = new QWidget();
    m_colorSwatchWidget->setMinimumWidth(50);
    
    m_hueSlider = new QSlider();
    m_hueSlider->setOrientation(Qt::Horizontal);
    m_hueSlider->setRange(0, 359);
    QObject::connect(m_hueSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::hueChanged);
    
    m_hueSpinBox = new QSpinBox();
    m_hueSpinBox->setRange(0, 359);
    m_hueSpinBox->setWrapping(true);
    QObject::connect(m_hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::hueChanged);

    m_saturationSlider = new QSlider();
    m_saturationSlider->setOrientation(Qt::Horizontal);
    m_saturationSlider->setRange(0, 255);
    QObject::connect(m_saturationSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::saturationChanged);

    m_saturationSpinBox = new QSpinBox();
    m_saturationSpinBox->setRange(0, 255);
    QObject::connect(m_saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::saturationChanged);

    m_valueSlider = new QSlider();
    m_valueSlider->setOrientation(Qt::Horizontal);
    m_valueSlider->setRange(0, 255);
    QObject::connect(m_valueSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::valueChanged);

    m_valueSpinBox = new QSpinBox();
    m_valueSpinBox->setRange(0, 255);
    QObject::connect(m_valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::valueChanged);

    m_redSlider = new QSlider();
    m_redSlider->setOrientation(Qt::Horizontal);
    m_redSlider->setRange(0, 255);
    QObject::connect(m_redSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::redChanged);

    m_redSpinBox = new QSpinBox();
    m_redSpinBox->setRange(0, 255);
    QObject::connect(m_redSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::redChanged);

    m_greenSlider = new QSlider();
    m_greenSlider->setOrientation(Qt::Horizontal);
    m_greenSlider->setRange(0, 255);
    QObject::connect(m_greenSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::greenChanged);

    m_greenSpinBox = new QSpinBox();
    m_greenSpinBox->setRange(0, 255);
    QObject::connect(m_greenSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::greenChanged);

    m_blueSlider = new QSlider();
    m_blueSlider->setOrientation(Qt::Horizontal);
    m_blueSlider->setRange(0, 255);
    QObject::connect(m_blueSlider, &QSlider::valueChanged,
                     this, &WuQColorEditorWidget::blueChanged);

    m_blueSpinBox = new QSpinBox();
    m_blueSpinBox->setRange(0, 255);
    QObject::connect(m_blueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQColorEditorWidget::blueChanged);

    const int32_t COL_SWATCH(0);
    const int32_t COL_LABEL(COL_SWATCH + 1);
    const int32_t COL_SLIDER(COL_LABEL + 1);
    const int32_t COL_SPIN_BOX(COL_SLIDER + 1);
    
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    layout->setColumnStretch(COL_SWATCH,   0);
    layout->setColumnStretch(COL_LABEL,    0);
    layout->setColumnStretch(COL_SLIDER, 100);
    layout->setColumnStretch(COL_SPIN_BOX, 0);
    
    int32_t row(0);
    layout->addWidget(m_colorSwatchWidget,
                      row, COL_SWATCH,
                      6, 1);
    
    layout->addWidget(new QLabel("Red:"),
                      row, COL_LABEL);
    layout->addWidget(m_redSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_redSpinBox,
                      row, COL_SPIN_BOX);
    row++;

    layout->addWidget(new QLabel("Green:"),
                      row, COL_LABEL);
    layout->addWidget(m_greenSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_greenSpinBox,
                      row, COL_SPIN_BOX);
    row++;

    layout->addWidget(new QLabel("Blue:"),
                      row, COL_LABEL);
    layout->addWidget(m_blueSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_blueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    layout->addWidget(new QLabel("Hue:"),
                      row, COL_LABEL);
    layout->addWidget(m_hueSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_hueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    layout->addWidget(new QLabel("Sat:"),
                      row, COL_LABEL);
    layout->addWidget(m_saturationSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_saturationSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    layout->addWidget(new QLabel("Value:"),
                      row, COL_LABEL);
    layout->addWidget(m_valueSlider,
                      row, COL_SLIDER);
    layout->addWidget(m_valueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    return widget;
}

/**
 * Called when hue changed
 * @param hue
 *    New hue
 */
void
WuQColorEditorWidget::hueChanged(int hue)
{
    m_currentColor.setHsv(hue,
                          m_currentColor.saturation(),
                          m_currentColor.value());
    
    updateControls();
}

/**
 * Called when saturation changed
 * @param saturation
 *    New saturation
 */
void
WuQColorEditorWidget::saturationChanged(int saturation)
{
    m_currentColor.setHsv(m_currentColor.hue(),
                          saturation,
                          m_currentColor.value());
    
    updateControls();
}

/**
 * Called when changed
 * @param value
 *    New value
 */
void
WuQColorEditorWidget::valueChanged(int value)
{
    m_currentColor.setHsv(m_currentColor.hue(),
                          m_currentColor.saturation(),
                          value);
    
    updateControls();
}

/**
 * Called when red changed
 * @param red
 *    New red
 */
void
WuQColorEditorWidget::redChanged(int red)
{
    m_currentColor.setRed(red);
    
    updateControls();
}

/**
 * Called when green changed
 * @param green
 *    New green
 */
void
WuQColorEditorWidget::greenChanged(int green)
{
    m_currentColor.setGreen(green);
    
    updateControls();
}

/**
 * Called when blue changed
 * @param blue
 *    New blue
 */
void
WuQColorEditorWidget::blueChanged(int blue)
{
    m_currentColor.setBlue(blue);
    
    updateControls();
}

/**
 * Update the controls with the current color
 */
void
WuQColorEditorWidget::updateControls()
{
    updateSliderAndSpinBox(m_hueSlider,
                           m_hueSpinBox,
                           m_currentColor.hue());

    updateSliderAndSpinBox(m_saturationSlider,
                           m_saturationSpinBox,
                           m_currentColor.saturation());

    updateSliderAndSpinBox(m_valueSlider,
                           m_valueSpinBox,
                           m_currentColor.value());

    updateSliderAndSpinBox(m_redSlider,
                           m_redSpinBox,
                           m_currentColor.red());

    updateSliderAndSpinBox(m_greenSlider,
                           m_greenSpinBox,
                           m_currentColor.green());

    updateSliderAndSpinBox(m_blueSlider,
                           m_blueSpinBox,
                           m_currentColor.blue());
    
    m_colorSwatchWidget->setStyleSheet("background-color: rgb("
                                       + QString::number(m_currentColor.red())
                                       + ", " + QString::number(m_currentColor.green())
                                       + ", " + QString::number(m_currentColor.blue())
                                       + ");");
    
    updateValueColorLabel();
    
    updateHueSaturationLabel();
}

/**
 * Update the slider and the spin box with the given value
 * @param slider
 *  Slider that is update
 * @param spinBox
 *  Spin box that is updated
 * @param value
 *  New value for slider and spin box
 */
void
WuQColorEditorWidget::updateSliderAndSpinBox(QSlider* slider,
                                       QSpinBox* spinBox,
                                       const int value)
{
    QSignalBlocker sliderBlocker(slider);
    slider->setValue(value);
    
    QSignalBlocker spinBoxBlocker(spinBox);
    spinBox->setValue(value);
}

QWidget*
WuQColorEditorWidget::createHueSaturationColorLabel()
{
    const int32_t xWidth(360);
    const int32_t yHeight(256);
    
    m_hueSaturationColorLabel = new WuQImageLabel();
    m_hueSaturationColorLabel->setFixedSize(xWidth, yHeight);
    m_hueSaturationPixmap = QPixmap(xWidth,
                                    yHeight);
    QPainter painter(&m_hueSaturationPixmap);
    QPen pen = painter.pen();
    
    updateHueSaturationToLabelTransforms();
    for (int32_t x = 0; x < xWidth; x++) {
        const float hue = m_hueToLabelLinearTransform->inverseTransformValue(x);
        
        for (int32_t y = 0; y < yHeight; y++) {
            const float sat = m_saturationToLabelLinearTransform->inverseTransformValue(y);
            pen.setColor(QColor::fromHsv(hue, sat, 255));
            painter.setPen(pen);
            painter.drawPoint(x, y);
        }
    }
    
    m_hueSaturationColorLabel->setPixmap(m_hueSaturationPixmap);
    
    QObject::connect(m_hueSaturationColorLabel, &WuQImageLabel::clickedXY,
                     this, &WuQColorEditorWidget::hueSaturationLabelClicked);
    
    return m_hueSaturationColorLabel;
}

/**
 * Update with cursor showing hue and saturation values
 */
void
WuQColorEditorWidget::updateHueSaturationLabel()
{
    const float hue(m_currentColor.hue());
    const float saturation(m_currentColor.saturation());

    updateHueSaturationToLabelTransforms();
    const float cursorX = m_hueToLabelLinearTransform->transformValue(hue);
    const float cursorY = m_saturationToLabelLinearTransform->transformValue(saturation);
    QPixmap pixmap = m_hueSaturationPixmap;

    QPainter painter(&pixmap);
    painter.translate(cursorX, cursorY);
    QPen pen = painter.pen();
    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    
    const int32_t cursorSize(10);
    painter.drawLine(-cursorSize, 0, cursorSize, 0);
    painter.drawLine(0, -cursorSize, 0, cursorSize);
    
    m_hueSaturationColorLabel->setPixmap(pixmap);
}

void
WuQColorEditorWidget::hueSaturationLabelClicked(int x, int y)
{
    updateHueSaturationToLabelTransforms();
    const float hue = m_hueToLabelLinearTransform->transformValue(x);
    const float saturation = m_saturationToLabelLinearTransform->transformValue(y);

    m_currentColor.setHsv(hue, saturation, m_currentColor.value());
    updateControls();
}


QWidget*
WuQColorEditorWidget::createValueColorLabel()
{
    m_valueColorLabel = new WuQImageLabel();
    m_valueColorLabel->setFixedSize(25, 256);
    
    
    QObject::connect(m_valueColorLabel, &WuQImageLabel::clickedXY,
                     this, &WuQColorEditorWidget::valueLabelClicked);
    
    return m_valueColorLabel;
}

void
WuQColorEditorWidget::updateValueColorLabel()
{
    const int32_t xWidth  = m_valueColorLabel->width();
    const int32_t yHeight = m_valueColorLabel->height();
    
    QPixmap pixmap(xWidth,
                   yHeight);
    QPainter painter(&pixmap);
    
    painter.fillRect(0, 0, xWidth, yHeight, QColor(255, 255, 255));
    
    QPen pen = painter.pen();
    const int32_t saturation = m_saturationSpinBox->value();
    const int32_t hue        = m_hueSpinBox->value();
    
    updateValueToLabelXyTransform();
    
    /*
     * Max saturation is at top
     */
    const int32_t halfWidth(xWidth / 2);
    
    for (int32_t value = 0; value < 255; value++) {
        const float yValue = m_valueToLabelLinearTransform->transformValue(value);
        pen.setColor(QColor::fromHsv(hue, saturation, value));
        painter.setPen(pen);
        painter.drawRect(0, yValue, halfWidth, 1);
    }
    
    const int32_t quarterWidth(halfWidth / 2);
    QPolygon triangle;
    triangle.append(QPoint(halfWidth, 0));
    triangle.append(QPoint(xWidth, -quarterWidth));
    triangle.append(QPoint(xWidth,  quarterWidth));
    
    const int32_t valueY = (yHeight - m_currentColor.value());
    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.translate(0, valueY);
    QBrush brush = painter.brush();
    brush.setColor(QColor(0, 0, 0));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawPolygon(triangle);
    
    m_valueColorLabel->setPixmap(pixmap);
}

void
WuQColorEditorWidget::valueLabelClicked(int /*x*/, int y)
{
    updateValueToLabelXyTransform();
    
    const float newValue = m_valueToLabelLinearTransform->inverseTransformValue(y);

    m_currentColor.setHsv(m_currentColor.hue(),
                          m_currentColor.saturation(),
                          newValue);
    updateControls();
}

void
WuQColorEditorWidget::updateValueToLabelXyTransform()
{
    /*
     * Only need to update if Label size has changed
     * or first time
     */
    if (m_valueColorLabel->size() != m_valueLabelSize) {
        m_valueToLabelLinearTransform.reset();
    }
    
    if ( ! m_valueToLabelLinearTransform) {
        m_valueLabelSize = m_valueColorLabel->size();
        
        /*
         * Qt's widgets have (0, 0) in the upper left corner
         * The transform maps an HSV's "value" to a widgets Y-coordinate (pixel)
         */
        const float labelHeight(m_valueColorLabel->height() - 1);
        const float valueMaximum(255); /* ranges [0, 255] */
        
        m_valueToLabelLinearTransform.reset(new LinearEquationTransform(0, valueMaximum,
                                                                        0, labelHeight,
                                                                        0, labelHeight));
    }
}

void
WuQColorEditorWidget::updateHueSaturationToLabelTransforms()
{
    if (m_hueSaturationColorLabel->size() != m_hueSaturationLabelSize) {
        m_hueToLabelLinearTransform.reset();
        m_saturationToLabelLinearTransform.reset();
    }
    
    if ( ! m_hueToLabelLinearTransform) {
        /*
         * Qt's widgets have (0, 0) in the upper left corner
         * The transform maps an HSV's "hue" to a widgets X-coordinate (pixel)
         * Hue=359 is at left, Hue=0 is at right
         */
        const float labelWidth(m_hueSaturationColorLabel->width() - 1);
        const float hueMaximum(359); /* ranges [0, 359] */
        const float hueMinimum(0);

        m_hueToLabelLinearTransform.reset(new LinearEquationTransform(hueMinimum, hueMaximum,
                                                                      0, labelWidth,
                                                                      hueMinimum, labelWidth));
        
        const float labelHeight(m_hueSaturationColorLabel->height() - 1);
        const float saturationMaximum(255);
        const float saturationMinimum(0);
        m_saturationToLabelLinearTransform.reset(new LinearEquationTransform(saturationMinimum, saturationMaximum,
                                                                             0, labelHeight,
                                                                             saturationMinimum, labelHeight));
    }
}







/* ======================================================================== */
LinearEquationTransform::LinearEquationTransform(const float xMin,
                                                 const float xMax,
                                                 const float yMin,
                                                 const float yMax,
                                                 const float x0,
                                                 const float y0)
{
    const float dy(yMax - yMin);
    const float dx(xMax - xMin);
    if (dx == 0) {
        return;
    }
    
    const float slope(dy / dx);
    const float intercept = y0 - (slope * x0);
    
    m_transform.scale(1.0, -slope);
    m_transform.translate(0.0, -intercept);
    
    if (m_transform.isInvertible()) {
        m_inverseTransform = m_transform.inverted();
    }
}

LinearEquationTransform::~LinearEquationTransform()
{
    
}

float
LinearEquationTransform::transformValue(const float value) const
{
    QPointF pIn(0, value);
    QPointF pOut = m_transform.map(QPointF(0.0, value));
    
    const float valueOut = pOut.y();
    return valueOut;
}

float
LinearEquationTransform::inverseTransformValue(const float value) const
{
    QPointF pIn(0, value);
    QPointF pOut = m_inverseTransform.map(QPointF(0.0, value));
    
    const float valueOut = pOut.y();
    return valueOut;
}
