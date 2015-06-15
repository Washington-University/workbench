
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_COLOR_WIDGET_DECLARE__
#include "AnnotationColorWidget.h"
#undef __ANNOTATION_COLOR_WIDGET_DECLARE__

#include <QAction>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "BrainOpenGL.h"
#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationColorWidget 
 * \brief Widget for annotation color selection.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     Parent for this widget.
 */
AnnotationColorWidget::AnnotationColorWidget(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotation = NULL;
    
    QLabel* backLabel      = new QLabel("Fill");
    QLabel* backColorLabel = new QLabel("Color");
    QLabel* lineLabel      = new QLabel("Line");
    QLabel* lineColorLabel = new QLabel("Color");
    QLabel* lineWidthLabel = new QLabel("Width");
    
    const QSize toolButtonSize(16, 16);
    
    /*
     * Background color menu
     */
    m_backgroundColorMenu = new CaretColorEnumMenu((CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR
                                                    | CaretColorEnum::OPTION_INCLUDE_NONE_COLOR));
    QObject::connect(m_backgroundColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(backgroundColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Background action and tool button
     */
    m_backgroundColorAction = new QAction("B",
                                          this);
    m_backgroundColorAction->setToolTip("Adjust the fill color");
    m_backgroundColorAction->setMenu(m_backgroundColorMenu);
    m_backgroundToolButton = new QToolButton();
    m_backgroundToolButton->setDefaultAction(m_backgroundColorAction);
    m_backgroundToolButton->setIconSize(toolButtonSize);
    
    /*
     * Widget/object group for background widgets
     */
    m_backgroundWidgetGroup = new WuQWidgetObjectGroup(this);
    m_backgroundWidgetGroup->add(backLabel);
    m_backgroundWidgetGroup->add(backColorLabel);
    m_backgroundWidgetGroup->add(m_backgroundToolButton);
    
    /*
     * Foreground color menu
     */
    m_foregroundColorMenu = new CaretColorEnumMenu((CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR
                                                    | CaretColorEnum::OPTION_INCLUDE_NONE_COLOR));
    QObject::connect(m_foregroundColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(foregroundColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Foreground color action and toolbutton
     */
    m_foregroundColorAction = new QAction("F",
                                          this);
    m_foregroundColorAction->setToolTip("Adjust the line/text color");
    m_foregroundColorAction->setMenu(m_foregroundColorMenu);
    m_foregroundToolButton = new QToolButton();
    m_foregroundToolButton->setDefaultAction(m_foregroundColorAction);
    m_foregroundToolButton->setIconSize(toolButtonSize);
    
    /*
     * Foreground thickness
     */
    float minimumLineWidth = 0.0;
    float maximumLineWidth = 1.0;
    
    BrainOpenGL::getMinMaxLineWidth(minimumLineWidth,
                                    maximumLineWidth);
    minimumLineWidth = std::max(minimumLineWidth, 1.0f);
    m_foregroundThicknessSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(minimumLineWidth,
                                                                                                  maximumLineWidth,
                                                                                                  1.0,
                                                                                                  0,
                                                                                                  this,
                                                                                                  SLOT(foregroundThicknessSpinBoxValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_foregroundThicknessSpinBox,
                                         "Adjust the line thickness");
    m_foregroundThicknessSpinBox->setFixedWidth(45);
    
    /*
     * Layout widgets
     */
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(lineLabel,
                          0, 0,
                          1, 2,
                          Qt::AlignHCenter);
    gridLayout->addWidget(lineWidthLabel,
                          1, 0,
                          Qt::AlignHCenter);
    gridLayout->addWidget(lineColorLabel,
                          1, 1,
                          Qt::AlignHCenter);
    gridLayout->addWidget(m_foregroundThicknessSpinBox,
                          2, 0,
                          Qt::AlignHCenter);
    gridLayout->addWidget(m_foregroundToolButton,
                          2, 1,
                          Qt::AlignHCenter);
    gridLayout->addWidget(backLabel,
                          0, 2,
                          Qt::AlignHCenter);
    gridLayout->addWidget(backColorLabel,
                          1, 2,
                          Qt::AlignHCenter);
    gridLayout->addWidget(m_backgroundToolButton,
                          2, 2,
                          Qt::AlignHCenter);

    backgroundColorSelected(CaretColorEnum::WHITE);
    foregroundColorSelected(CaretColorEnum::BLACK);
}

/**
 * Destructor.
 */
AnnotationColorWidget::~AnnotationColorWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationColorWidget::updateContent(Annotation* annotation)
{
    m_annotation = annotation;
    
    if (m_annotation != NULL) {
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
    
    updateBackgroundColorButton();
    updateForegroundColorButton();
    updateForegroundThicknessSpinBox();
}

/**
 * Gets called when the background color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationColorWidget::backgroundColorSelected(const CaretColorEnum::Enum caretColor)
{
    if (m_annotation != NULL) {
        m_annotation->setBackgroundColor(caretColor);
        
        if (caretColor == CaretColorEnum::CUSTOM) {
            float rgba[4];
            m_annotation->getCustomBackgroundColor(rgba);
            
            QColor color;
            color.setRgbF(rgba[0], rgba[1], rgba[2]);
            
            QColor newColor = QColorDialog::getColor(color,
                                                     m_backgroundToolButton,
                                                     "Background Color");
            if (newColor.isValid()) {
                rgba[0] = newColor.redF();
                rgba[1] = newColor.greenF();
                rgba[2] = newColor.blueF();
                m_annotation->setCustomBackgroundColor(rgba);
            }
        }
    }

    updateBackgroundColorButton();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the background color.
 */
void
AnnotationColorWidget::updateBackgroundColorButton()
{
    CaretColorEnum::Enum colorEnum = CaretColorEnum::NONE;
    float rgba[4];
    CaretColorEnum::toRGBFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    if (m_annotation != NULL) {
        colorEnum = m_annotation->getBackgroundColor();
        m_annotation->getBackgroundColorRGBA(rgba);
        
        float customRGBA[4];
        m_annotation->getCustomBackgroundColor(customRGBA);
        m_backgroundColorMenu->setCustomIconColor(customRGBA);
        
        bool enableBackgroundFlag = false;
        switch (m_annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                enableBackgroundFlag = true;
                break;
            case AnnotationTypeEnum::IMAGE:
                enableBackgroundFlag = false;
                break;
            case AnnotationTypeEnum::LINE:
                enableBackgroundFlag = false;
                break;
            case AnnotationTypeEnum::OVAL:
                enableBackgroundFlag = true;
                break;
            case AnnotationTypeEnum::TEXT:
                enableBackgroundFlag = true;
                break;
        }
        
        m_backgroundWidgetGroup->setEnabled(enableBackgroundFlag);
        if ( ! enableBackgroundFlag) {
            colorEnum = CaretColorEnum::NONE;
        }
    }
    
    QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(m_backgroundToolButton,
                                                           24, 24,
                                                           colorEnum,
                                                           rgba,
                                                           false);
    QIcon icon(pm);
    
    m_backgroundColorAction->setIcon(icon);
    m_backgroundColorMenu->setSelectedColor(colorEnum);
}


/**
 * Update the foreground color.
 */
void
AnnotationColorWidget::updateForegroundColorButton()
{
    CaretColorEnum::Enum colorEnum = CaretColorEnum::WHITE;
    float rgba[4];
    CaretColorEnum::toRGBFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    if (m_annotation != NULL) {
        colorEnum = m_annotation->getForegroundColor();
        m_annotation->getForegroundColorRGBA(rgba);

        float customRGBA[4];
        m_annotation->getCustomForegroundColor(customRGBA);
        m_foregroundColorMenu->setCustomIconColor(customRGBA);
    }
    
    QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(m_foregroundToolButton, 24, 24, colorEnum, rgba, true);
    m_foregroundColorAction->setIcon(QIcon(pm));
    m_foregroundColorMenu->setSelectedColor(colorEnum);
}

/**
 * Gets called when the foreground color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationColorWidget::foregroundColorSelected(const CaretColorEnum::Enum caretColor)
{
    if (m_annotation != NULL) {
        m_annotation->setForegroundColor(caretColor);
        
        if (caretColor == CaretColorEnum::CUSTOM) {
            float rgba[4];
            m_annotation->getCustomForegroundColor(rgba);
            
            QColor color;
            color.setRgbF(rgba[0], rgba[1], rgba[2]);
            
            QColor newColor = QColorDialog::getColor(color,
                                                     m_foregroundToolButton,
                                                     "Foreground Color");
            if (newColor.isValid()) {
                rgba[0] = newColor.redF();
                rgba[1] = newColor.greenF();
                rgba[2] = newColor.blueF();
                m_annotation->setCustomForegroundColor(rgba);
            }
        }
    }
    updateForegroundColorButton();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the foreground thickness value changes.
 *
 * @param value
 *     New value for foreground thickness.
 */
void
AnnotationColorWidget::foregroundThicknessSpinBoxValueChanged(double value)
{
    if (m_annotation != NULL) {
        m_annotation->setForegroundLineWidth(value);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Update the foreground thickness spin box.
 */
void
AnnotationColorWidget::updateForegroundThicknessSpinBox()
{
    float value = 0.0;
    bool widgetEnabled = false;
    if (m_annotation != NULL) {
        if (m_annotation->isForegroundLineWidthSupported()) {
            value = m_annotation->getForegroundLineWidth();
            widgetEnabled = true;
        }
    }
    
    m_foregroundThicknessSpinBox->blockSignals(true);
    m_foregroundThicknessSpinBox->setValue(value);
    m_foregroundThicknessSpinBox->blockSignals(false);
    m_foregroundThicknessSpinBox->setEnabled(widgetEnabled);
}
