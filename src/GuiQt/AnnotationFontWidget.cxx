
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

#include <QAction>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#define __ANNOTATION_FONT_WIDGET_DECLARE__
#include "AnnotationFontWidget.h"
#undef __ANNOTATION_FONT_WIDGET_DECLARE__

#include "AnnotationFontAttributesInterface.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationTextFontNameEnum.h"
#include "AnnotationTextFontPointSizeEnum.h"
#include "AnnotationPercentSizeText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQSpecialIncrementDoubleSpinBox.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::AnnotationFontWidget
 * \brief Widget for annotation font selection
 * \ingroup GuiQt
 */

/**
 * Processes increment and decrements for double spin box.
 */
class FontSizeFunctionObject : public WuQSpecialIncrementDoubleSpinBox::StepFunctionObject {
public:
    double getNewValue(const double currentValue,
                       const int steps) const {
        const double stepAmount = 0.1;
        const double outputValue = currentValue + (stepAmount * steps);
        return outputValue;
    }
};

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of window in which this instance is displayed
 * @param parent
 *     Parent for this widget.
 */
AnnotationFontWidget::AnnotationFontWidget(const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                           const int32_t browserWindowIndex,
                                           QWidget* parent)
: QWidget(parent),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{
    /*
     * "Font" label
     */
    QLabel* fontLabel = new QLabel("Font ");
    
    /*
     * Combo box for font name selection
     */
    m_fontNameComboBox = new EnumComboBoxTemplate(this);
    m_fontNameComboBox->setup<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
    QObject::connect(m_fontNameComboBox, SIGNAL(itemActivated()),
                     this, SLOT(fontNameChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontNameComboBox->getWidget(),
                                          "Change font");
    
    /*
     * Combo box for font size
     */
    m_fontSizeSpinBox = new WuQSpecialIncrementDoubleSpinBox(new FontSizeFunctionObject);
    m_fontSizeSpinBox->setRange(0.0, 100.0);
    m_fontSizeSpinBox->setDecimals(1);
    m_fontSizeSpinBox->setSingleStep(0.1);
    m_fontSizeSpinBox->setSuffix("%");
    QObject::connect(m_fontSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(fontSizeChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontSizeSpinBox,
                                          "Change font size (height) as percentage, zero to one-hundred, of viewport height");
    

    /*
     * Text color menu
     */
    m_textColorMenu = new CaretColorEnumMenu((CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR));
    QObject::connect(m_textColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(textColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Text color action and toolbutton
     */
    QLabel* textColorLabel = new QLabel("Color");
    const QSize toolButtonSize(16, 16);
    m_textColorAction = new QAction("C",
                                    this);
    m_textColorAction->setToolTip("Adjust the text color");
    m_textColorAction->setMenu(m_textColorMenu);
    m_textColorToolButton = new QToolButton();
    m_textColorToolButton->setDefaultAction(m_textColorAction);
    m_textColorToolButton->setIconSize(toolButtonSize);
    
    QToolButton* boldFontToolButton      = NULL;
    QToolButton* italicFontToolButton    = NULL;
    QToolButton* underlineFontToolButton = NULL;
    m_boldFontAction      = NULL;
    m_italicFontAction    = NULL;
    m_underlineFontAction = NULL;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            /*
             * Bold Font
             */
            m_boldFontAction = WuQtUtilities::createAction("B", //boldFontText.toStringWithHtmlBody(),
                                                           "Enable/disable bold styling",
                                                           this, this, SLOT(fontBoldChanged()));
            m_boldFontAction->setCheckable(true);
            boldFontToolButton = new QToolButton();
            boldFontToolButton->setDefaultAction(m_boldFontAction);
            
            /*
             * Change the bold toolbutton's font to bold.
             */
            QFont boldFont = boldFontToolButton->font();
            boldFont.setBold(true);
            boldFontToolButton->setFont(boldFont);
            
            /*
             * Italic font toolbutton
             */
            m_italicFontAction = WuQtUtilities::createAction("i", "Enable/disable italic styling",
                                                             this, this, SLOT(fontItalicChanged()));
            m_italicFontAction->setCheckable(true);
            italicFontToolButton = new QToolButton();
            italicFontToolButton->setDefaultAction(m_italicFontAction);
            
            /*
             * Change the italic toolbutton's font to italic.
             */
            QFont italicFont = italicFontToolButton->font();
            italicFont.setItalic(true);
            italicFontToolButton->setFont(italicFont);
            
            /*
             * Underline font toolbutton
             */
            m_underlineFontAction =  WuQtUtilities::createAction("U", "Enable/disable font underlining",
                                                                 this, this, SLOT(fontUnderlineChanged()));
            m_underlineFontAction->setCheckable(true);
            underlineFontToolButton = new QToolButton();
            underlineFontToolButton->setDefaultAction(m_underlineFontAction);
            
            /*
             * Change the underline toolbutton's font to underline.
             */
            QFont underlineFont = underlineFontToolButton->font();
            underlineFont.setUnderline(true);
            underlineFontToolButton->setFont(underlineFont);
        }
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    
    /*
     * Layout the widgets
     */
    
    const bool newLayoutFlag = true;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            QLabel* sizeLabel  = new QLabel("Size");
            QLabel* styleLabel = new QLabel("Style");
            
            QHBoxLayout* stylesLayout = new QHBoxLayout();
            WuQtUtilities::setLayoutSpacingAndMargins(stylesLayout, 0, 0);
            //stylesLayout->addStretch();
            stylesLayout->addWidget(boldFontToolButton);
            stylesLayout->addWidget(italicFontToolButton);
            stylesLayout->addWidget(underlineFontToolButton);
            stylesLayout->addStretch();
            
            QGridLayout* fontNameSizeLayout = new QGridLayout(this);
            WuQtUtilities::setLayoutSpacingAndMargins(fontNameSizeLayout, 2, 0);
            fontNameSizeLayout->setColumnStretch(0, 0);
            fontNameSizeLayout->setColumnStretch(1, 0);
            fontNameSizeLayout->setColumnStretch(2, 0);
            fontNameSizeLayout->setColumnStretch(3, 100);
            fontNameSizeLayout->addWidget(fontLabel, 0, 0);
            fontNameSizeLayout->addWidget(m_fontNameComboBox->getWidget(),
                                          0, 1, 1, 3);
            fontNameSizeLayout->addWidget(sizeLabel, 1, 0);
            fontNameSizeLayout->addWidget(m_fontSizeSpinBox,
                                          1, 1);
            fontNameSizeLayout->addWidget(styleLabel, 2, 0);
            fontNameSizeLayout->addLayout(stylesLayout, 2, 1);
            fontNameSizeLayout->addWidget(textColorLabel, 1, 2);
            fontNameSizeLayout->addWidget(m_textColorToolButton, 2, 2);
        }
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationFontWidget::~AnnotationFontWidget()
{
}

/**
 * Update the content of this widget with the given annotations.
 *
 * @param annotations
 *     The selected annotations.
 */
void
AnnotationFontWidget::updateContent(std::vector<AnnotationFontAttributesInterface*>& annotations)
{
    m_annotationsFontStyle = annotations;
    m_annotations.clear();
    
    if ( ! m_annotationsFontStyle.empty()) {
        bool boldOnFlag        = true;
        bool italicOnFlag      = true;
        bool underlineOnFlag   = true;
        int32_t stylesEnabledCount = 0;
        
        AnnotationTextFontNameEnum::Enum fontName = AnnotationTextFontNameEnum::VERA;
        bool fontNameValid = true;
        float fontSizeValue = 5.0;
        bool haveMultipleFontSizeValues = false;
        
        const int32_t numAnn = static_cast<int32_t>(m_annotationsFontStyle.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(m_annotationsFontStyle, i);
            AnnotationFontAttributesInterface* annText = m_annotationsFontStyle[i];
            CaretAssert(annText);
            
            Annotation* annotation = dynamic_cast<Annotation*>(annText);
            CaretAssert(annotation);
            m_annotations.push_back(annotation);
            
            const float sizeValue = annText->getFontPercentViewportSize();
            if (i == 0) {
                fontName = annText->getFont();
                fontSizeValue = sizeValue;
            }
            else {
                if (annText->getFont() != fontName) {
                    fontNameValid = false;
                }
                if (fontSizeValue != sizeValue) {
                    haveMultipleFontSizeValues = true;
                    fontSizeValue = std::min(fontSizeValue,
                                             sizeValue);
                }
            }
            
            if (annText->isStylesSupported()) {
                if ( ! annText->isBoldStyleEnabled()) {
                    boldOnFlag = false;
                }
                if ( ! annText->isItalicStyleEnabled()) {
                    italicOnFlag = false;
                }
                if ( ! annText->isUnderlineStyleEnabled()) {
                    underlineOnFlag = false;
                }
                
                ++stylesEnabledCount;
            }
        }
        
        m_fontNameComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(fontName);
        
        updateFontSizeSpinBox(fontSizeValue,
                              haveMultipleFontSizeValues);
        
        /*
         * Font styles are ON only if all selected
         * text annotations have the style enabled
         */
        const bool stylesEnabledFlag = (stylesEnabledCount > 0);
        
        m_boldFontAction->setEnabled(stylesEnabledFlag);
        m_boldFontAction->setChecked(boldOnFlag && stylesEnabledFlag);

        m_italicFontAction->setEnabled(stylesEnabledFlag);
        m_italicFontAction->setChecked(italicOnFlag && stylesEnabledFlag);
        
        m_underlineFontAction->setEnabled(stylesEnabledFlag);
        m_underlineFontAction->setChecked(underlineOnFlag && stylesEnabledFlag);
        
        AnnotationText::setUserDefaultFont(fontName);
        AnnotationText::setUserDefaultFontPercentViewportSize(fontSizeValue);
        if (stylesEnabledFlag) {
            AnnotationText::setUserDefaultBoldEnabled(boldOnFlag);
            AnnotationText::setUserDefaultItalicEnabled(italicOnFlag);
            AnnotationText::setUserDefaultUnderlineEnabled(underlineOnFlag);
        }
    }
    
    CaretAssert(m_annotations.size() == m_annotationsFontStyle.size());
    
    updateTextColorButton();
    
    setEnabled( ! m_annotations.empty());
}

/**
 * Update the font size spin box.
 *
 * @param value
 *     New value for font size spin box.
 * @param haveMultipleValuesFlag
 *     If true, there are multiple font size values so indicate
 *     this with a '+' sign as a suffix
 */
void
AnnotationFontWidget::updateFontSizeSpinBox(const float value,
                                            const bool haveMultipleValuesFlag)
{
    m_fontSizeSpinBox->blockSignals(true);
    m_fontSizeSpinBox->setValue(value);
    m_fontSizeSpinBox->blockSignals(false);
    QString fontSizeSuffix("%");
    if (haveMultipleValuesFlag) {
        fontSizeSuffix = "%+";
    }
    m_fontSizeSpinBox->setSuffix(fontSizeSuffix);
}

/**
 * Gets called when the text color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationFontWidget::textColorSelected(const CaretColorEnum::Enum caretColor)
{
    if ( ! m_annotationsFontStyle.empty()) {
        float rgba[4];
        m_annotationsFontStyle[0]->getTextColorRGBA(rgba);
        
        if (caretColor == CaretColorEnum::CUSTOM) {
            QColor color;
            color.setRgbF(rgba[0], rgba[1], rgba[2]);
            
            QColor newColor = QColorDialog::getColor(color,
                                                     m_textColorToolButton,
                                                     "Text Color");
            if (newColor.isValid()) {
                rgba[0] = newColor.redF();
                rgba[1] = newColor.greenF();
                rgba[2] = newColor.blueF();
                
                
                switch (m_parentWidgetType) {
                    case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                        AnnotationText::setUserDefaultCustomTextColor(rgba);
                        break;
                    case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                        CaretAssert(0);
                        break;
                }
            }
        }
        
        
        switch (m_parentWidgetType) {
            case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            {
                AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                undoCommand->setModeTextColor(caretColor,
                                              rgba,
                                              m_annotations);
                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                annMan->applyCommand(undoCommand);
                
                AnnotationText::setUserDefaultTextColor(caretColor);
            }
                break;
            case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                CaretAssert(0);
                break;
        }
    }
    
    updateTextColorButton();
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the text color button.
 */
void
AnnotationFontWidget::updateTextColorButton()
{
    CaretColorEnum::Enum colorEnum = CaretColorEnum::NONE;
    float rgba[4];
    CaretColorEnum::toRGBFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    bool colorButtonValidFlag = false;
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            const int32_t numAnnotations = static_cast<int32_t>(m_annotationsFontStyle.size());
            if (numAnnotations > 0) {
                bool firstColorSupportFlag = true;
                bool allSameColorFlag = true;
                
                for (int32_t i = 0; i < numAnnotations; i++) {
                    if (firstColorSupportFlag) {
                        m_annotationsFontStyle[i]->getTextColorRGBA(rgba);
                        firstColorSupportFlag = false;
                        colorButtonValidFlag = true;
                    }
                    else {
                        float colorRGBA[4];
                        m_annotationsFontStyle[i]->getTextColorRGBA(colorRGBA);
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
                    colorEnum = m_annotationsFontStyle[0]->getTextColor();
                    m_annotationsFontStyle[0]->getTextColorRGBA(rgba);
                    
                    float customRGBA[4];
                    m_annotationsFontStyle[0]->getCustomTextColor(customRGBA);
                    m_textColorMenu->setCustomIconColor(customRGBA);
                    
                    switch (m_parentWidgetType) {
                        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                            AnnotationText::setUserDefaultTextColor(colorEnum);
                            AnnotationText::setUserDefaultCustomTextColor(customRGBA);
                            break;
                        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                            CaretAssert(0);
                            break;
                    }
                    
                }
            }
        }
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    
    QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(m_textColorToolButton, 24, 24, colorEnum, rgba, true);
    m_textColorAction->setIcon(QIcon(pm));
    m_textColorMenu->setSelectedColor(colorEnum);
    
    if (colorButtonValidFlag) {
        
    }
}

/**
 * Gets called when font bold changed.
 */
void
AnnotationFontWidget::fontBoldChanged()
{
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontBold(m_boldFontAction->isChecked(),
                                 m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationText::setUserDefaultBoldEnabled(m_boldFontAction->isChecked());
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            break;
    }
}

/**
 * Gets called when font italic changed.
 */
void
AnnotationFontWidget::fontItalicChanged()
{
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontItalic(m_italicFontAction->isChecked(),
                                   m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationText::setUserDefaultItalicEnabled(m_italicFontAction->isChecked());
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            break;
    }
}

/**
 * Gets called when font name changed.
 */
void
AnnotationFontWidget::fontNameChanged()
{
    const AnnotationTextFontNameEnum::Enum fontName = m_fontNameComboBox->getSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
    
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontName(fontName,
                                 m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationText::setUserDefaultFont(fontName);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
}

/**
 * Gets called when font size changed.
 */
void
AnnotationFontWidget::fontSizeChanged()
{
    const float fontPercentSize = m_fontSizeSpinBox->value();
    
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontPercentSize(fontPercentSize,
                                        m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationText::setUserDefaultFontPercentViewportSize(fontPercentSize);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
}

/**
 * Gets called when font underline changed.
 */
void
AnnotationFontWidget::fontUnderlineChanged()
{
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontUnderline(m_underlineFontAction->isChecked(),
                                      m_annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationText::setUserDefaultUnderlineEnabled(m_underlineFontAction->isChecked());
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
}

