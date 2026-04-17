
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

#include <QAction>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QSlider>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "LinearEquationTransform.h"
#include "MathFunctions.h"
#include "WuQImageLabel.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQColorEditorWidget
 * \brief Widget for editing color
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget
 */
WuQColorEditorWidget::WuQColorEditorWidget(QWidget* parent)
: QWidget(parent)
{
    const int32_t labelHeight(180);
    QWidget* hueSaturationWidget = createHueSaturationColorLabel(labelHeight);
    QWidget* valueWidget         = createValueColorLabel(labelHeight);
    
    QWidget* controlsWidget = createControlsWidget();
    QWidget* caretColorWidget = createCaretColorNoNamesSelectionButtonsWidget();
    
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setVerticalSpacing(4);
    int row(layout->rowCount());
    layout->addWidget(hueSaturationWidget, row, 0);
    layout->addWidget(valueWidget, row, 1);
    layout->addWidget(caretColorWidget, row, 2);
    ++row;
    layout->addWidget(controlsWidget, row, 0, 1, 3);
    
    setCurrentColor(QColor());
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
 * @return The color in the editor
 */
QColor
WuQColorEditorWidget::getColor() const
{
    return m_currentColor;
}


/**
 * Set the current color for editing
 * @param color
 * New color for editing
 */
void
WuQColorEditorWidget::setCurrentColor(const QColor& color)
{
    m_currentColor  = color;
    m_originalColor = m_currentColor;
    
    updateControls();
}

/**
 * @return New instance of the controls widget
 */
QWidget*
WuQColorEditorWidget::createControlsWidget()
{
    QLabel* currentLabel = new QLabel("Color");
    currentLabel->setAlignment(Qt::AlignHCenter);
    m_currentColorSwatchWidget = new QWidget();
    m_currentColorSwatchWidget->setMinimumWidth(20);
    m_currentColorSwatchWidget->setMinimumHeight(10);
    
    QLabel* originalLabel = new QLabel("Original");
    m_originalColorSwatchWidget = new QWidget();
    m_originalColorSwatchWidget->setMinimumWidth(20);
    m_originalColorSwatchWidget->setMinimumHeight(10);
    
    QToolButton* revertToolButton = new QToolButton();
    revertToolButton->setText("Revert");
    revertToolButton->setToolTip("Revert to Original Color");
    QObject::connect(revertToolButton, &QToolButton::clicked,
                     this, &WuQColorEditorWidget::revertToOriginalColorToolButtonClicked);
    
    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->setContentsMargins(0, 0, 0, 0);
    colorLayout->addWidget(currentLabel);
    colorLayout->addWidget(m_currentColorSwatchWidget);
    colorLayout->addWidget(originalLabel);
    colorLayout->addWidget(m_originalColorSwatchWidget);
    colorLayout->addWidget(revertToolButton);
    colorLayout->addStretch();
    
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

    const int32_t COL_LABEL(0);
    const int32_t COL_SLIDER(COL_LABEL + 1);
    const int32_t COL_SPIN_BOX(COL_SLIDER + 1);
    
    QGridLayout* adjustLayout = new QGridLayout();
    adjustLayout->setContentsMargins(0, 0, 0, adjustLayout->contentsMargins().bottom());
    adjustLayout->setVerticalSpacing(4);
    adjustLayout->setColumnStretch(COL_LABEL,    0);
    adjustLayout->setColumnStretch(COL_SLIDER, 100);
    adjustLayout->setColumnStretch(COL_SPIN_BOX, 0);
    
    int32_t row(0);
    adjustLayout->addWidget(new QLabel("Red:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_redSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_redSpinBox,
                      row, COL_SPIN_BOX);
    row++;

    adjustLayout->addWidget(new QLabel("Green:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_greenSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_greenSpinBox,
                      row, COL_SPIN_BOX);
    row++;

    adjustLayout->addWidget(new QLabel("Blue:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_blueSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_blueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    adjustLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                            row, COL_LABEL, 1, 3, Qt::AlignVCenter);
    adjustLayout->setRowMinimumHeight(row, 10);
    
    row++;
    
    adjustLayout->addWidget(new QLabel("Hue:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_hueSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_hueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    adjustLayout->addWidget(new QLabel("Sat:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_saturationSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_saturationSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    adjustLayout->addWidget(new QLabel("Value:"),
                      row, COL_LABEL);
    adjustLayout->addWidget(m_valueSlider,
                      row, COL_SLIDER);
    adjustLayout->addWidget(m_valueSpinBox,
                      row, COL_SPIN_BOX);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(colorLayout, 0);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget(), 0);
    layout->addLayout(adjustLayout, 0);
    
    return widget;
}

/**
 * Called to revert to the original color
 */
void
WuQColorEditorWidget::revertToOriginalColorToolButtonClicked()
{
    m_currentColor = m_originalColor;
    updateControls();
    emitColorChangedSignal();
}

/**
 * Emit the color changed signal when the color changes
 */
void
WuQColorEditorWidget::emitColorChangedSignal()
{
    emit colorChanged(m_currentColor);
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
                          m_currentColor.hsvSaturation(),
                          m_currentColor.value());
    
    updateControls();
    emitColorChangedSignal();
}

/**
 * Called when saturation changed
 * @param saturation
 *    New saturation
 */
void
WuQColorEditorWidget::saturationChanged(int saturation)
{
    m_currentColor.setHsv(m_currentColor.hsvHue(),
                          saturation,
                          m_currentColor.value());
    
    updateControls();
    emitColorChangedSignal();
}

/**
 * Called when changed
 * @param value
 *    New value
 */
void
WuQColorEditorWidget::valueChanged(int value)
{
    m_currentColor.setHsv(m_currentColor.hsvHue(),
                          m_currentColor.hsvSaturation(),
                          value);
    
    updateControls();
    emitColorChangedSignal();
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
    emitColorChangedSignal();
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
    emitColorChangedSignal();
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
    emitColorChangedSignal();
}

/**
 * Update the controls with the current color
 */
void
WuQColorEditorWidget::updateControls()
{
    updateSliderAndSpinBox(m_hueSlider,
                           m_hueSpinBox,
                           m_currentColor.hsvHue());

    updateSliderAndSpinBox(m_saturationSlider,
                           m_saturationSpinBox,
                           m_currentColor.hsvSaturation());

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
    
    m_currentColorSwatchWidget->setStyleSheet("background-color: rgb("
                                              + QString::number(m_currentColor.red())
                                              + ", " + QString::number(m_currentColor.green())
                                              + ", " + QString::number(m_currentColor.blue())
                                              + ");");
    
    m_originalColorSwatchWidget->setStyleSheet("background-color: rgb("
                                               + QString::number(m_originalColor.red())
                                               + ", " + QString::number(m_originalColor.green())
                                               + ", " + QString::number(m_originalColor.blue())
                                               + ");");

    updateValueColorLabel();
    
    updateHueSaturationLabel();
    
    setEnabled(m_currentColor.isValid());
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
WuQColorEditorWidget::createHueSaturationColorLabel(const int32_t labelHeight)
{
    const int32_t xWidth(labelHeight);
    const int32_t yHeight(labelHeight);
    
    m_hueSaturationColorLabel = new WuQImageLabel();
    m_hueSaturationColorLabel->setFixedSize(xWidth, yHeight);
    m_hueSaturationPixmap = QPixmap(xWidth,
                                    yHeight);
    QPainter painter(&m_hueSaturationPixmap);
    QPen pen = painter.pen();
    
    updateHueSaturationToLabelTransforms();
    for (int32_t x = 0; x < xWidth; x++) {
        const float hue = m_hueToLabelLinearTransform->inverseTransform(x);
        
        for (int32_t y = 0; y < yHeight; y++) {
            const float sat = m_saturationToLabelLinearTransform->inverseTransform(y);
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
    const float hue(m_currentColor.hsvHue());
    const float saturation(m_currentColor.hsvSaturation());

    updateHueSaturationToLabelTransforms();
    const float cursorX = m_hueToLabelLinearTransform->transform(hue);
    const float cursorY = m_saturationToLabelLinearTransform->transform(saturation);
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
    const float hue = m_hueToLabelLinearTransform->inverseTransform(x);
    const float saturation = m_saturationToLabelLinearTransform->inverseTransform(y);

    m_currentColor.setHsv(hue, saturation, m_currentColor.value());
    updateControls();
    emitColorChangedSignal();
}


QWidget*
WuQColorEditorWidget::createValueColorLabel(const int32_t labelHeight)
{
    m_valueColorLabel = new WuQImageLabel();
    m_valueColorLabel->setFixedSize(25, labelHeight);
    
    
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
    
    for (int32_t y = 0; y < m_valueColorLabel->height(); y++) {
        const float value = m_valueToLabelLinearTransform->inverseTransform(y);
        pen.setColor(QColor::fromHsv(hue, saturation, value));
        painter.setPen(pen);
        painter.drawRect(0, y, halfWidth, 1);
    }
    
    const int32_t quarterWidth(halfWidth / 2);
    QPolygon triangle;
    triangle.append(QPoint(halfWidth, 0));
    triangle.append(QPoint(xWidth, -quarterWidth));
    triangle.append(QPoint(xWidth,  quarterWidth));
    
    const int32_t triangleY(m_valueToLabelLinearTransform->transform(m_currentColor.value()));
    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.translate(0, triangleY);
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
    
    const float newValue = m_valueToLabelLinearTransform->inverseTransform(y);

    m_currentColor.setHsv(m_currentColor.hsvHue(),
                          m_currentColor.hsvSaturation(),
                          newValue);
    updateControls();
    emitColorChangedSignal();
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
        
        AString errorMessage;
        m_valueToLabelLinearTransform = LinearEquationTransform::newInstance(0, valueMaximum,
                                                                             0, labelHeight,
                                                                             0, labelHeight,
                                                                             errorMessage);
        m_valueToLabelLinearTransform = LinearEquationTransform::newInstance(0, valueMaximum,
                                                                             labelHeight, 0,
                                                                             0, labelHeight,
                                                                             errorMessage);
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

        AString errorMessage;
        m_hueToLabelLinearTransform = LinearEquationTransform::newInstance(hueMaximum, hueMinimum,
                                                                           0, labelWidth,
                                                                           hueMaximum, 0,
                                                                           errorMessage);

        const bool testFlag(false);
        if (testFlag) {
            std::cout << "Hue to label: " << m_hueToLabelLinearTransform->toString() << std::endl;
            for (int32_t hue = 0; hue <= hueMaximum; hue++) {
                const float labelX( m_hueToLabelLinearTransform->transform(hue));
                const float hue2(m_hueToLabelLinearTransform->inverseTransform(labelX));
                std::cout << "   hue=" << hue << ", labelX=" << labelX << ", hue2=" << hue2 << std::endl;
            }
        }
        
        const float labelHeight(m_hueSaturationColorLabel->height() - 1);
        const float saturationMaximum(255);
        const float saturationMinimum(0);
        m_saturationToLabelLinearTransform = LinearEquationTransform::newInstance(saturationMaximum, saturationMinimum,
                                                                                  0, labelHeight,
                                                                                  saturationMaximum, 0,
                                                                                  errorMessage);
        if (testFlag) {
            std::cout << "Saturation to label: " << m_hueToLabelLinearTransform->toString() << std::endl;
        }
        for (int32_t sat = 0; sat <= saturationMaximum; sat++) {
            const float labelY( m_saturationToLabelLinearTransform->transform(sat));
            const float sat2(m_saturationToLabelLinearTransform->inverseTransform(labelY));
            if (testFlag) {
                std::cout << "   sat=" << sat << ", labelY=" << labelY << ", sat2=" << sat2 << std::endl;
            }
        }
    }
}

QWidget*
WuQColorEditorWidget::createCaretColorNoNamesSelectionButtonsWidget()
{
    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout(widget);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(0);
    
    std::vector<CaretColorEnum::Enum> colorEnums;
    CaretColorEnum::getColorEnums(colorEnums);
    
    const int32_t maxRows(8);
    int32_t row(0);
    int32_t col(0);
    std::vector<QWidget*> tbWidgets;
    for (auto cc : colorEnums) {
        QToolButton* tb = new QToolButton();
        
        QSize iconSize(12, 12);
        float rgbaFloat[4];
        CaretColorEnum::toRGBAFloat(cc, rgbaFloat);
        QPixmap pm(WuQtUtilities::createCaretColorEnumPixmap(tb, iconSize.width(), iconSize.height(),
                                                             CaretColorEnum::CUSTOM, rgbaFloat, false));
        
        QAction* a = new QAction(CaretColorEnum::toGuiName(cc));
        a->setData(CaretColorEnum::toIntegerCode(cc));
        a->setIcon(pm);
        QObject::connect(a, &QAction::triggered,
                         this, [=]() { caretColorActionClicked(a); } );
        
        tb->setDefaultAction(a);
        tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
        tb->setIconSize(iconSize);
        
        layout->addWidget(tb, row, col);

        row++;
        if (row >= maxRows) {
            row = 0;
            col++;
        }
        
        tbWidgets.push_back(tb);
    }
    
    WuQtUtilities::matchWidgetWidths(tbWidgets);
    
    widget->setFixedSize(widget->sizeHint());
    
    return widget;
}

void
WuQColorEditorWidget::caretColorActionClicked(QAction* action)
{
    CaretAssert(action);
    
    const int intData = action->data().toInt();
    bool validFlag(false);
    const CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(intData,
                                                                       &validFlag);
    if (validFlag) {
        uint8_t rgba[4];
        CaretColorEnum::toRGBAByte(color, rgba);
        m_currentColor.setRgb(rgba[0], rgba[1], rgba[2]);

        updateControls();
        emitColorChangedSignal();
    }
}

