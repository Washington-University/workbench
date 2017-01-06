
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

#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainOpenGL.h"
#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
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
AnnotationColorWidget::AnnotationColorWidget(const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                             const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{
    
    QLabel* backFillLabel      = new QLabel("Fill");
    QLabel* backFillColorLabel = new QLabel("Color");
    QLabel* foreLineLabel      = new QLabel("Line");
    QLabel* foreLineColorLabel = new QLabel("Color");
    
    QLabel* lineWidthLabel = NULL;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            lineWidthLabel = new QLabel("Width");
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    
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
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_backgroundToolButton);
    
    /*
     * Widget/object group for background widgets
     */
    m_backgroundWidgetGroup = new WuQWidgetObjectGroup(this);
    m_backgroundWidgetGroup->add(backFillLabel);
    m_backgroundWidgetGroup->add(backFillColorLabel);
    m_backgroundWidgetGroup->add(m_backgroundToolButton);
    
    /*
     * Line color menu
     */
    m_lineColorMenu = new CaretColorEnumMenu((CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR
                                                    | CaretColorEnum::OPTION_INCLUDE_NONE_COLOR));
    QObject::connect(m_lineColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(lineColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Line color action and toolbutton
     */
    m_lineColorAction = new QAction("F",
                                          this);
    m_lineColorAction->setToolTip("Adjust the line color");
    m_lineColorAction->setMenu(m_lineColorMenu);
    m_lineToolButton = new QToolButton();
    m_lineToolButton->setDefaultAction(m_lineColorAction);
    m_lineToolButton->setIconSize(toolButtonSize);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_lineToolButton);
    
    /*
     * Line thickness
     */
    float minimumLineWidth = 0.0;
    float maximumLineWidth = 1.0;
    
    m_lineThicknessSpinBox = NULL;
    if (lineWidthLabel != NULL) {
        BrainOpenGL::getMinMaxLineWidth(minimumLineWidth,
                                        maximumLineWidth);
        minimumLineWidth = std::max(minimumLineWidth, 1.0f);
        m_lineThicknessSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(minimumLineWidth,
                                                                                                      maximumLineWidth,
                                                                                                      1.0,
                                                                                                      0,
                                                                                                      this,
                                                                                                      SLOT(lineThicknessSpinBoxValueChanged(double)));
        WuQtUtilities::setWordWrappedToolTip(m_lineThicknessSpinBox,
                                             "Adjust the line thickness");
        m_lineThicknessSpinBox->setFixedWidth(45);
    }
    

    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            CaretAssert(lineWidthLabel);
            gridLayout->addWidget(foreLineLabel,
                                  0, 0,
                                  1, 2,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(lineWidthLabel,
                                  1, 0,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(foreLineColorLabel,
                                  1, 1,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(m_lineThicknessSpinBox,
                                  2, 0,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(m_lineToolButton,
                                  2, 1,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(backFillLabel,
                                  0, 2,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(backFillColorLabel,
                                  1, 2,
                                  Qt::AlignHCenter);
            gridLayout->addWidget(m_backgroundToolButton,
                                  2, 2,
                                  Qt::AlignHCenter);
        }
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    /*
     * Layout widgets
     */
    backgroundColorSelected(CaretColorEnum::WHITE);
    lineColorSelected(CaretColorEnum::BLACK);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
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
 * @param annotations
 */
void
AnnotationColorWidget::updateContent(std::vector<Annotation*>& annotations)
{
    m_annotations = annotations;
    
    if ( ! m_annotations.empty()) {
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
    
    updateBackgroundColorButton();
    updateLineColorButton();
    updateLineThicknessSpinBox();
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
    if (! m_annotations.empty()) {
        float rgba[4];
        m_annotations[0]->getCustomBackgroundColor(rgba);
        
        if (caretColor == CaretColorEnum::CUSTOM) {
            QColor color;
            color.setRgbF(rgba[0], rgba[1], rgba[2]);
            
            QColor newColor = QColorDialog::getColor(color,
                                                     m_backgroundToolButton,
                                                     "Background Color");
            if (newColor.isValid()) {
                rgba[0] = newColor.redF();
                rgba[1] = newColor.greenF();
                rgba[2] = newColor.blueF();
                

                switch (m_parentWidgetType) {
                    case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                        Annotation::setUserDefaultCustomBackgroundColor(rgba);
                        break;
                    case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                        CaretAssert(0);
                        break;
                }
            }
        }
        
        if ( ! isBothColorsSetToNoneAllowed(this,
                                            caretColor,
                                            m_lineColorMenu->getSelectedColor(),
                                            m_annotations)) {
            return;
        }
        
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeColorBackground(caretColor,
                                            rgba,
                                            m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        
        Annotation::setUserDefaultBackgroundColor(caretColor);
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
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
    CaretColorEnum::toRGBAFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    const int32_t numAnnotations = static_cast<int32_t>(m_annotations.size());
    if (numAnnotations > 0) {
        bool firstColorSupportFlag = true;
        bool enableBackgroundFlag = false;
        bool allSameColorFlag = true;
        
        for (int32_t i = 0; i < numAnnotations; i++) {
            if (m_annotations[0]->isBackgroundColorSupported()) {
                if (firstColorSupportFlag) {
                    m_annotations[i]->getBackgroundColorRGBA(rgba);
                    firstColorSupportFlag = false;
                    enableBackgroundFlag = true;
                }
                else {
                    float colorRGBA[4];
                    m_annotations[i]->getBackgroundColorRGBA(colorRGBA);
                    for (int32_t iColor = 0; iColor < 4; iColor++) {
                        if (rgba[iColor] != colorRGBA[iColor]) {
                            allSameColorFlag = false;
                            break;
                        }
                    }
                    
                    if ( ! allSameColorFlag) {
                        break;
                    }
                }
            }
        }
        
        if (allSameColorFlag) {
            colorEnum = m_annotations[0]->getBackgroundColor();
            m_annotations[0]->getBackgroundColorRGBA(rgba);
            
            float customRGBA[4];
            m_annotations[0]->getCustomBackgroundColor(customRGBA);
            m_backgroundColorMenu->setCustomIconColor(customRGBA);
            
            switch (m_parentWidgetType) {
                case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                    Annotation::setUserDefaultBackgroundColor(colorEnum);
                    Annotation::setUserDefaultCustomBackgroundColor(customRGBA);
                    break;
                case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                    CaretAssert(0);
                    break;
            }
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
 * Update the line color.
 */
void
AnnotationColorWidget::updateLineColorButton()
{
    CaretColorEnum::Enum colorEnum = CaretColorEnum::NONE;
    float rgba[4];
    CaretColorEnum::toRGBAFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    const int32_t numAnnotations = static_cast<int32_t>(m_annotations.size());
    if (numAnnotations > 0) {
        bool firstColorSupportFlag = true;
        bool enableLineFlag = false;
        bool allSameColorFlag = true;
        
        for (int32_t i = 0; i < numAnnotations; i++) {
                if (firstColorSupportFlag) {
                    m_annotations[i]->getLineColorRGBA(rgba);
                    firstColorSupportFlag = false;
                    enableLineFlag = true;
                }
                else {
                    float colorRGBA[4];
                    m_annotations[i]->getLineColorRGBA(colorRGBA);
                    for (int32_t iColor = 0; iColor < 4; iColor++) {
                        if (rgba[iColor] != colorRGBA[iColor]) {
                            allSameColorFlag = false;
                            break;
                        }
                    }
                    
                    if ( ! allSameColorFlag) {
                        break;
                    }
                }
        }
        
        if (allSameColorFlag) {
            colorEnum = m_annotations[0]->getLineColor();
            m_annotations[0]->getLineColorRGBA(rgba);
            
            float customRGBA[4];
            m_annotations[0]->getCustomLineColor(customRGBA);
            m_lineColorMenu->setCustomIconColor(customRGBA);

            switch (m_parentWidgetType) {
                case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                    setUserDefaultLineColor(colorEnum,
                                            customRGBA);
                    break;
                case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                    CaretAssert(0);
                    break;
            }
            
        }
        
        
        if ( ! enableLineFlag) {
            colorEnum = CaretColorEnum::NONE;
        }
    }
    
    QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(m_lineToolButton, 24, 24, colorEnum, rgba, true);
    m_lineColorAction->setIcon(QIcon(pm));
    m_lineColorMenu->setSelectedColor(colorEnum);
}

/**
 * Tests for both background and line colors both set to none.  This is allowed
 * for some annotations types such as text, which uses a text color, or images.
 *
 * @param widget
 *     Widget on which error dialog is displayed.
 * @param colorOne
 *     One of the background or line colors.
 * @param colorTwo
 *     The other one of the background or line colors.
 * @param annotations
 *     The selected annotations.
 * @return
 *     True if the colors are acceptable, else false (and a an error
 *     message dialog is displayed).
 */
bool
AnnotationColorWidget::isBothColorsSetToNoneAllowed(QWidget* widget,
                                                    const CaretColorEnum::Enum colorOne,
                                                    const CaretColorEnum::Enum colorTwo,
                                                    const std::vector<Annotation*>& annotations)
{
    if ((colorOne == CaretColorEnum::NONE)
        && (colorTwo == CaretColorEnum::NONE)) {
        
        bool allowBothColorsNoneFlag = true;
        
        for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
             iter != annotations.end();
             iter++) {
            const Annotation* ann = *iter;
            CaretAssert(ann);
            switch (ann->getType()) {
                case AnnotationTypeEnum::BOX:
                    allowBothColorsNoneFlag = false;
                    break;
                case AnnotationTypeEnum::COLOR_BAR:
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    allowBothColorsNoneFlag = false;
                    break;
                case AnnotationTypeEnum::OVAL:
                    allowBothColorsNoneFlag = false;
                    break;
                case AnnotationTypeEnum::TEXT:
                    break;
            }
        }
        
        if ( ! allowBothColorsNoneFlag) {
            const AString message("Setting both Line and Fill colors to NONE is not allowed for the selected annotation(s).");
            WuQMessageBox::errorOk(widget,
                                   message);
            return false;
        }
    }

    return true;
}


/**
 * Gets called when the line color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationColorWidget::lineColorSelected(const CaretColorEnum::Enum caretColor)
{
    if ( ! m_annotations.empty()) {
        CaretAssertVectorIndex(m_annotations, 0);
        
        float rgba[4];
        m_annotations[0]->getCustomLineColor(rgba);
        
        if (caretColor == CaretColorEnum::CUSTOM) {
            QColor color;
            color.setRgbF(rgba[0], rgba[1], rgba[2]);
            
            QColor newColor = QColorDialog::getColor(color,
                                                     m_backgroundToolButton,
                                                     "Line Color");
            if (newColor.isValid()) {
                rgba[0] = newColor.redF();
                rgba[1] = newColor.greenF();
                rgba[2] = newColor.blueF();
            }
        }
        
        if ( ! isBothColorsSetToNoneAllowed(this,
                                            caretColor,
                                            m_backgroundColorMenu->getSelectedColor(),
                                            m_annotations)) {
            return;
        }
        
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeColorLine(caretColor,
                                      rgba,
                                      m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        setUserDefaultLineColor(caretColor,
                                rgba);
    }
    
    updateLineColorButton();
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Set the user default line color.
 *
 * @param caretColor
 *     The line color.
 * @param customRGBA
 *     The custom line color RGBA components.
 */
void
AnnotationColorWidget::setUserDefaultLineColor(const CaretColorEnum::Enum caretColor,
                                               const float customRGBA[4])
{
    if ( ! m_annotations.empty()) {
        bool allTextFlag  = true;
        bool someTextFlag = false;
        
        for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
             iter != m_annotations.end();
             iter++) {
            const Annotation* ann = *iter;
            if (ann->getType() == AnnotationTypeEnum::TEXT) {
                someTextFlag = true;
            }
            else {
                allTextFlag = false;
            }
        }
        
        /*
         * Note: Text has its own default line color.  Without it, if the
         * user creates a text annotation it will get a box around it since
         * other annotations frequently use a line color.
         */
        if (allTextFlag
            || someTextFlag) {
            Annotation::setUserDefaultForTextLineColor(caretColor);
            if (caretColor == CaretColorEnum::CUSTOM) {
                Annotation::setUserDefaultForTextCustomLineColor(customRGBA);
            }
        }
        
        if (! allTextFlag) {
            Annotation::setUserDefaultLineColor(caretColor);
            if (caretColor == CaretColorEnum::CUSTOM) {
                Annotation::setUserDefaultCustomLineColor(customRGBA);
            }
        }
    }
}


/**
 * Gets called when the line thickness value changes.
 *
 * @param value
 *     New value for line thickness.
 */
void
AnnotationColorWidget::lineThicknessSpinBoxValueChanged(double value)
{
    if ( ! m_lineThicknessSpinBox->specialValueText().isEmpty()) {
        if (m_lineThicknessSpinBox->specialValueText()
            == m_lineThicknessSpinBox->text()) {
            /*
             * Ignore special text which is available when 
             * there are multiple annotations with different
             * line thicknesses.
             */
            std::cout << "Ignoring special text " << std::endl;
            return;
        }
    }
    
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeLineWidth(value,
                                  m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    Annotation::setUserDefaultLineWidth(value);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the line thickness spin box.
 */
void
AnnotationColorWidget::updateLineThicknessSpinBox()
{
    if (m_lineThicknessSpinBox == NULL) {
        return;
    }
    
    float lineWidthValue = 1.0;
    bool  lineWidthValid = false;
    bool  haveMultipleLineWidthValues = false;
    
    const int32_t numAnnotations = static_cast<int32_t>(m_annotations.size());
    if (numAnnotations > 0) {
        for (int32_t i = 0; i < numAnnotations; i++) {
            if (m_annotations[i]->isLineWidthSupported()) {
                const float annLineWidth = m_annotations[i]->getLineWidth();
                if (lineWidthValid) {
                    if (annLineWidth != lineWidthValue) {
                        haveMultipleLineWidthValues = true;
                    }
                    lineWidthValue = std::min(lineWidthValue,
                                              annLineWidth);
                }
                else {
                    lineWidthValue = annLineWidth;
                    lineWidthValid = true;
                }
            }
        }
        
        if (lineWidthValid) {
            switch (m_parentWidgetType) {
                case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                    Annotation::setUserDefaultLineWidth(lineWidthValue);
                    break;
                case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                    CaretAssert(0);
                    break;
            }
        }
    }
    
    /*
     * When the selected annotations have different line
     * widths, the valid displayed is the minimum line
     * width with a suffix consisting of a plus symbol.
     */
    m_lineThicknessSpinBox->blockSignals(true);
    m_lineThicknessSpinBox->setValue(lineWidthValue);
    m_lineThicknessSpinBox->setEnabled(lineWidthValid);
    if (haveMultipleLineWidthValues) {
        m_lineThicknessSpinBox->setSuffix("+");
    }
    else {
        m_lineThicknessSpinBox->setSuffix("");
    }
    m_lineThicknessSpinBox->blockSignals(false);
}

