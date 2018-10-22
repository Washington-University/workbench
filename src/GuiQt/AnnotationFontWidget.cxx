
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
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ModelSurfaceMontage.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::AnnotationFontWidget
 * \brief Widget for annotation font selection
 * \ingroup GuiQt
 */

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
    const AString fontSizeToolTop("<html>"
                                  "Adjusts font height (size), as a percentage of the viewport height, that is  "
                                  "converted to a pixel height when the text is drawn.  "
                                  "<p>"
                                  "The numeric value in this control will be <font color=\"red\">RED</font> "
                                  "when the pixel height is estimated to be <i>too small</i> and some or all "
                                  "characters may not be drawn.  "
                                  "Reducing the height of the text and/or the height of the window may cause "
                                  "<i>too small</i> text.  "
                                  "</html>");
    m_fontSizeSpinBox = new WuQDoubleSpinBox(this);
    m_fontSizeSpinBox->setRangePercentage(0.0, 100.0);
    QObject::connect(m_fontSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(fontSizeChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontSizeSpinBox->getWidget(),
                                          fontSizeToolTop);
    
    /*
     * Default palette for size spin box
     */
    m_fontSizeSpinBoxDefaultPalette = m_fontSizeSpinBox->getWidget()->palette();
    
    /*
     * Palette for spin box that colors text in red when the font height is "too small"
     */
    m_fontSizeSpinBoxRedTextPalette = m_fontSizeSpinBoxDefaultPalette;
    QBrush brush = m_fontSizeSpinBoxRedTextPalette.brush(QPalette::Active, QPalette::Text);
    brush.setColor(Qt::red);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::Text, brush);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    m_fontSizeSpinBoxRedTextPalette.setBrush(QPalette::Active, QPalette::HighlightedText, brush);

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
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_textColorToolButton);
    
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
            m_boldFontAction = WuQtUtilities::createAction("B",
                                                           "Enable/disable bold styling",
                                                           this, this, SLOT(fontBoldChanged()));
            m_boldFontAction->setCheckable(true);
            boldFontToolButton = new QToolButton();
            boldFontToolButton->setDefaultAction(m_boldFontAction);
            WuQtUtilities::setToolButtonStyleForQt5Mac(boldFontToolButton);
            
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
            WuQtUtilities::setToolButtonStyleForQt5Mac(italicFontToolButton);
            
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
            WuQtUtilities::setToolButtonStyleForQt5Mac(underlineFontToolButton);
            
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
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            QLabel* sizeLabel  = new QLabel("Size");
            QLabel* styleLabel = new QLabel("Style");
            
            QHBoxLayout* stylesLayout = new QHBoxLayout();
            WuQtUtilities::setLayoutSpacingAndMargins(stylesLayout, 0, 0);
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
            fontNameSizeLayout->addWidget(m_fontSizeSpinBox->getWidget(),
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

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
}

/**
 * Destructor.
 */
AnnotationFontWidget::~AnnotationFontWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
AnnotationFontWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* redrawEvent = dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(redrawEvent);
        
        if (m_browserWindowIndex == redrawEvent->getBrowserWindowIndex()) {
            if (isVisible()) {
                updateFontSizeControls();
            }
        }
    }
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
    m_annotationsFontColor.clear();
    m_annotationsFontColor.reserve(annotations.size());
    m_annotationsFontName.clear();
    m_annotationsFontName.reserve(annotations.size());
    m_annotationsFontSize.clear();
    m_annotationsFontSize.reserve(annotations.size());
    m_annotationsFontStyle.clear();
    m_annotationsFontStyle.reserve(annotations.size());

    for (auto a : annotations) {
        Annotation* ann = dynamic_cast<Annotation*>(a);
        if (ann->testProperty(Annotation::Property::TEXT_FONT_NAME)) {
            m_annotationsFontName.push_back(a);
        }
        if (ann->testProperty(Annotation::Property::TEXT_FONT_SIZE)) {
            m_annotationsFontSize.push_back(a);
        }
        if (ann->testProperty(Annotation::Property::TEXT_FONT_STYLE)) {
            m_annotationsFontStyle.push_back(a);
        }
        if (ann->testProperty(Annotation::Property::TEXT_COLOR)) {
            m_annotationsFontColor.push_back(a);
        }
    }
    
    updateFontNameControls();
    updateFontSizeControls();
    updateFontStyleControls();
    updateTextColorButton();
    
    setEnabled((m_annotationsFontColor.size()
                + m_annotationsFontName.size()
                + m_annotationsFontSize.size()
                + m_annotationsFontStyle.size()) > 0);
}

/**
 * Update the font name controls.
 */
void
AnnotationFontWidget::updateFontNameControls()
{
    if ( ! m_annotationsFontName.empty()) {
        AnnotationTextFontNameEnum::Enum fontName = AnnotationTextFontNameEnum::VERA;
        //bool fontNameValid = true;
        
        const int32_t numAnn = static_cast<int32_t>(m_annotationsFontName.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(m_annotationsFontName, i);
            AnnotationFontAttributesInterface* annText = m_annotationsFontName[i];
            CaretAssert(annText);
            
            if (i == 0) {
                fontName = annText->getFont();
            }
            else {
                if (annText->getFont() != fontName) {
                    //fontNameValid = false;
                }
            }
        }
        
        m_fontNameComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(fontName);
        
        AnnotationText::setUserDefaultFont(fontName);
    }
}

/**
 * Update the font size controls.
 */
void
AnnotationFontWidget::updateFontSizeControls()
{
    if ( ! m_annotationsFontSize.empty()) {
        float fontSizeValue = 5.0;
        bool haveMultipleFontSizeValues = false;
        
        const float surfaceMontageRowCount = getSurfaceMontageRowCount();
        
        bool tooSmallFlag = false;
        const int32_t numAnn = static_cast<int32_t>(m_annotationsFontSize.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(m_annotationsFontSize, i);
            AnnotationFontAttributesInterface* annText = m_annotationsFontSize[i];
            CaretAssert(annText);
            
            float sizeValue = annText->getFontPercentViewportSize();
            
            Annotation* ann = dynamic_cast<Annotation*>(annText);
            CaretAssert(ann);
            switch (ann->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    sizeValue /= surfaceMontageRowCount;
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    sizeValue /= surfaceMontageRowCount;
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case  AnnotationCoordinateSpaceEnum::WINDOW:
                    break;
            }
            
            if (i == 0) {
                fontSizeValue = sizeValue;
            }
            else {
                if (fontSizeValue != sizeValue) {
                    haveMultipleFontSizeValues = true;
                    fontSizeValue = std::min(fontSizeValue,
                                             sizeValue);
                }
            }
            
            if (annText->isFontTooSmallWhenLastDrawn()) {
                tooSmallFlag = true;
            }
//            const AnnotationText* textAnnotation = dynamic_cast<AnnotationText*>(annText);
//            if (textAnnotation != NULL) {
//                if (textAnnotation->isFontTooSmallWhenLastDrawn()) {
//                    tooSmallFlag = true;
//                }
//            }
        }
        
        updateFontSizeSpinBox(fontSizeValue,
                              haveMultipleFontSizeValues,
                              tooSmallFlag);
        
        AnnotationText::setUserDefaultFontPercentViewportSize(fontSizeValue);
    }
}

/**
 * Update the font style controls.
 */
void
AnnotationFontWidget::updateFontStyleControls()
{
    if ( ! m_annotationsFontStyle.empty()) {
        bool boldOnFlag        = true;
        bool italicOnFlag      = true;
        bool underlineOnFlag   = true;
        int32_t stylesEnabledCount = 0;
        
        const int32_t numAnn = static_cast<int32_t>(m_annotationsFontStyle.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(m_annotationsFontStyle, i);
            AnnotationFontAttributesInterface* annText = m_annotationsFontStyle[i];
            CaretAssert(annText);
            
            Annotation* annotation = dynamic_cast<Annotation*>(annText);
            CaretAssert(annotation);
            if (annotation->testProperty(Annotation::Property::TEXT_FONT_STYLE)) {
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
        
        if (stylesEnabledFlag) {
            AnnotationText::setUserDefaultBoldEnabled(boldOnFlag);
            AnnotationText::setUserDefaultItalicEnabled(italicOnFlag);
            AnnotationText::setUserDefaultUnderlineEnabled(underlineOnFlag);
        }
    }
}


/**
 * Update the font size spin box.
 *
 * @param value
 *     New value for font size spin box.
 * @param haveMultipleValuesFlag
 *     If true, there are multiple font size values so indicate
 *     this with a '+' sign as a suffix
 * @param tooSmallFontFlag
 *     If true, the font may be too small as detected by the
 *     graphics drawing code.
 */
void
AnnotationFontWidget::updateFontSizeSpinBox(const float value,
                                            const bool haveMultipleValuesFlag,
                                            const bool tooSmallFontFlag)
{
    QSignalBlocker blocker(m_fontSizeSpinBox->getWidget());
    m_fontSizeSpinBox->setValue(value);

    if (tooSmallFontFlag) {
        m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxRedTextPalette);
    }
    else {
        m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxDefaultPalette);
    }

    QString fontSizeSuffix("%");
    if (haveMultipleValuesFlag) {
        fontSizeSuffix = "%+";
    }
    m_fontSizeSpinBox->setValue(value);
    m_fontSizeSpinBox->setSuffix(fontSizeSuffix);
    
//    if (tooSmallFontFlag) {
//        m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxRedTextPalette);
//    }
//    else {
//        m_fontSizeSpinBox->getWidget()->setPalette(m_fontSizeSpinBoxDefaultPalette);
//    }
//    m_fontSizeSpinBox->setValue(value);
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
    if ( ! m_annotationsFontColor.empty()) {
        float rgba[4];
        m_annotationsFontColor[0]->getTextColorRGBA(rgba);
        
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
                                              convertToAnnotations(m_annotationsFontColor));
                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                AString errorMessage;
                if ( ! annMan->applyCommand(undoCommand,
                                            errorMessage)) {
                    WuQMessageBox::errorOk(this,
                                           errorMessage);
                }
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
    CaretColorEnum::toRGBAFloat(colorEnum, rgba);
    rgba[3] = 1.0;
    
    bool colorButtonValidFlag = false;
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            const int32_t numAnnotations = static_cast<int32_t>(m_annotationsFontColor.size());
            if (numAnnotations > 0) {
                bool firstColorSupportFlag = true;
                bool allSameColorFlag = true;
                
                for (int32_t i = 0; i < numAnnotations; i++) {
                    if (firstColorSupportFlag) {
                        m_annotationsFontColor[i]->getTextColorRGBA(rgba);
                        firstColorSupportFlag = false;
                        colorButtonValidFlag = true;
                    }
                    else {
                        float colorRGBA[4];
                        m_annotationsFontColor[i]->getTextColorRGBA(colorRGBA);
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
                    colorEnum = m_annotationsFontColor[0]->getTextColor();
                    m_annotationsFontColor[0]->getTextColorRGBA(rgba);
                    
                    float customRGBA[4];
                    m_annotationsFontColor[0]->getCustomTextColor(customRGBA);
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
    
    m_textColorToolButton->setEnabled(colorButtonValidFlag);
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
                                 convertToAnnotations(m_annotationsFontStyle));
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(command,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
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
                                   convertToAnnotations(m_annotationsFontStyle));
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(command,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
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
                                 convertToAnnotations(m_annotationsFontName));
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(command,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
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
                                        convertToAnnotations(m_annotationsFontSize),
                                        getSurfaceMontageRowCount());
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(command,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
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
    
    /*
     * "Font too small" status is set while drawing so need
     * to update the size spin box AFTER the graphics update
     */
    updateFontSizeControls();
}

/**
 * @return Number of rows in a surface montage.
 *         If a surface montage is not displayed, one is returned.
 */
float
AnnotationFontWidget::getSurfaceMontageRowCount() const
{
    const BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    CaretAssert(bbw);
    int32_t surfaceMontageRowCount = 1;
    const BrowserTabContent* btc = bbw->getBrowserTabContent();
    if (btc != NULL) {
        const ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        if (msm != NULL) {
            int32_t columnCount = 1;
            msm->getSurfaceMontageNumberOfRowsAndColumns(btc->getTabNumber(),
                                                         surfaceMontageRowCount,
                                                         columnCount);
        }
    }
    
    return surfaceMontageRowCount;
}

/**
 * Gets called when font underline changed.
 */
void
AnnotationFontWidget::fontUnderlineChanged()
{
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontUnderline(m_underlineFontAction->isChecked(),
                                      convertToAnnotations(m_annotationsFontStyle));
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(command,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
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

/**
 * Convert the font style interfade annotations to annotations
 *
 * @param fontInterfaces
 *     Input font interface annotations.
 * @return
 *     Vector with input converted to Annotation class.
 */
std::vector<Annotation*>
AnnotationFontWidget::convertToAnnotations(const std::vector<AnnotationFontAttributesInterface*>& fontInterfaces)
{
    std::vector<Annotation*> annotationsOut;
    
    for (auto f : fontInterfaces) {
        Annotation* a = dynamic_cast<Annotation*>(f);
        CaretAssert(a);
        annotationsOut.push_back(a);
    }
    
    return annotationsOut;
}


