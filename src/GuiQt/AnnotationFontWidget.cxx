
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
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#define __ANNOTATION_FONT_WIDGET_DECLARE__
#include "AnnotationFontWidget.h"
#undef __ANNOTATION_FONT_WIDGET_DECLARE__

#include "AnnotationColorBar.h"
#include "AnnotationFontAttributesInterface.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationTextFontNameEnum.h"
#include "AnnotationTextFontPointSizeEnum.h"
#include "AnnotationPercentSizeText.h"
#include "Brain.h"
#include "CaretAssert.h"
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
        //const double stepAmount = currentValue * 0.10;
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
    m_annotationColorBar = NULL;
    
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
    
    QToolButton* boldFontToolButton      = NULL;
    QToolButton* italicFontToolButton    = NULL;
    QToolButton* underlineFontToolButton = NULL;
    QToolButton* outlineFontToolButton   = NULL;
    m_boldFontAction      = NULL;
    m_italicFontAction    = NULL;
    m_underlineFontAction = NULL;
    m_outlineFontAction   = NULL;
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
           
            /*
             * Outline font toolbutton
             */
            outlineFontToolButton = new QToolButton();
            outlineFontToolButton->setFixedSize(underlineFontToolButton->sizeHint());
            QPixmap outlinePixmap = createOutlineButtonPixmap(underlineFontToolButton);
            m_outlineFontAction =  WuQtUtilities::createAction("O", "Enable/disable font outlining",
                                                                 this, this, SLOT(fontOutlineChanged()));
            m_outlineFontAction->setIcon(QIcon(outlinePixmap));
            m_outlineFontAction->setCheckable(true);
            outlineFontToolButton->setDefaultAction(m_outlineFontAction);
            
//            /*
//             * Change the underline toolbutton's font to underline.
//             */
//            QFont outlineFont = outlineFontToolButton->font();
//            outlineFont.setOutline(true);
//            outlineFontToolButton->setFont(outlineFont);
        }
            break;
        case AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET:
            break;
    }
    
    
    /*
     * Layout the widgets
     */
    
    const bool newLayoutFlag = true;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            if (newLayoutFlag) {
                QLabel* sizeLabel  = new QLabel("Size");
                QLabel* styleLabel = new QLabel("Style");
                
                QHBoxLayout* stylesLayout = new QHBoxLayout();
                WuQtUtilities::setLayoutSpacingAndMargins(stylesLayout, 0, 0);
                //stylesLayout->addStretch();
                stylesLayout->addWidget(boldFontToolButton);
                stylesLayout->addWidget(italicFontToolButton);
                stylesLayout->addWidget(underlineFontToolButton);
                stylesLayout->addWidget(outlineFontToolButton);
                stylesLayout->addStretch();
                
                QGridLayout* fontNameSizeLayout = new QGridLayout(this);
                WuQtUtilities::setLayoutSpacingAndMargins(fontNameSizeLayout, 2, 0);
                fontNameSizeLayout->setColumnStretch(0, 0);
                fontNameSizeLayout->setColumnStretch(0, 1);
                fontNameSizeLayout->addWidget(fontLabel, 0, 0);
                fontNameSizeLayout->addWidget(m_fontNameComboBox->getWidget(), 0, 1);
                fontNameSizeLayout->addWidget(sizeLabel, 1, 0);
                fontNameSizeLayout->addWidget(m_fontSizeSpinBox, 1, 1, Qt::AlignLeft);
                fontNameSizeLayout->addWidget(styleLabel, 2, 0);
                fontNameSizeLayout->addLayout(stylesLayout, 2, 1);
                
                
//                QVBoxLayout* layout = new QVBoxLayout(this);
//                WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
//                layout->addLayout(fontNameSizeLayout);
//                layout->addLayout(stylesLayout);
            }
            else {
                QHBoxLayout* topRowLayout = new QHBoxLayout();
                WuQtUtilities::setLayoutSpacingAndMargins(topRowLayout, 2, 0);
                topRowLayout->addWidget(fontLabel, 0);
                topRowLayout->addWidget(m_fontNameComboBox->getWidget(), 100);
                QHBoxLayout* bottomRowLayout = new QHBoxLayout();
                WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 0);
                
                bottomRowLayout->addWidget(boldFontToolButton);
                bottomRowLayout->addWidget(italicFontToolButton);
                bottomRowLayout->addWidget(underlineFontToolButton);
                bottomRowLayout->addWidget(outlineFontToolButton);
                bottomRowLayout->addStretch();
                bottomRowLayout->addWidget(m_fontSizeSpinBox);
                
                QVBoxLayout* layout = new QVBoxLayout(this);
                WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
                layout->addLayout(topRowLayout);
                layout->addLayout(bottomRowLayout);
                //            layout->addStretch();
            }
            break;
        case AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET:
        {
            QLabel* sizeLabel = new QLabel("Size");
            
            QGridLayout* gridLayout = new QGridLayout(this);
            WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
            gridLayout->addWidget(fontLabel, 0, 0);
            gridLayout->addWidget(m_fontNameComboBox->getWidget(), 0, 1);
            gridLayout->addWidget(sizeLabel, 1, 0);
            gridLayout->addWidget(m_fontSizeSpinBox, 1, 1);
        }
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
    if ( ! annotations.empty()) {
        bool boldOnFlag        = true;
        bool italicOnFlag      = true;
        bool underlineOnFlag   = true;
        bool outlineOnFlag     = true;
        int32_t stylesEnabledCount = 0;
        
        AnnotationTextFontNameEnum::Enum fontName = AnnotationTextFontNameEnum::VERA;
        bool fontNameValid = true;
        float fontSizeValue = 5.0;
        bool haveMultipleFontSizeValues = false;
        
        const int32_t numAnn = static_cast<int32_t>(annotations.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(annotations, i);
            const AnnotationFontAttributesInterface* annText = annotations[i];
            CaretAssert(annText);
            
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
                if ( ! annText->isOutlineStyleEnabled()) {
                    outlineOnFlag = false;
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
        
        m_outlineFontAction->setEnabled(stylesEnabledFlag);
        m_outlineFontAction->setChecked(outlineOnFlag && stylesEnabledFlag);
        
        AnnotationText::setUserDefaultFont(fontName);
        AnnotationText::setUserDefaultFontPercentViewportSize(fontSizeValue);
        if (stylesEnabledFlag) {
            AnnotationText::setUserDefaultBoldEnabled(boldOnFlag);
            AnnotationText::setUserDefaultItalicEnabled(italicOnFlag);
            AnnotationText::setUserDefaultUnderlineEnabled(underlineOnFlag);
            AnnotationText::setUserDefaultOutlineEnabled(outlineOnFlag);
        }
    }
    
    setEnabled( ! annotations.empty());
}

/**
 * Update the content of this widget with the given text annotation.
 *
 * @param annotationColorBar
 *     Color bar for display (may be NULL).
 */
void
AnnotationFontWidget::updateAnnotationColorBarContent(AnnotationColorBar* annotationColorBar)
{
    m_annotationColorBar = annotationColorBar;
    
    if (m_annotationColorBar != NULL) {
        m_fontNameComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(m_annotationColorBar->getFont());
        updateFontSizeSpinBox(m_annotationColorBar->getFontPercentViewportSize(),
                              false);
    }
}

///**
// * Update the content of this widget with the given text annotation.
// *
// * @param annotationTexts
// *     Text annotations for display (may be NULL).
// */
//void
//AnnotationFontWidget::updateAnnotationTextContent(std::vector<AnnotationText*>& annotationTexts)
//{
//    if ( ! annotationTexts.empty()) {
//        bool boldOnFlag      = true;
//        bool italicOnFlag    = true;
//        bool underlineOnFlag = true;
//        
//        AnnotationTextFontNameEnum::Enum fontName = AnnotationTextFontNameEnum::VERA;
//        bool fontNameValid = true;
//        float fontSizeValue = 5.0;
//        bool haveMultipleFontSizeValues = false;
//        
//        const int32_t numAnn = static_cast<int32_t>(annotationTexts.size());
//        for (int32_t i = 0; i < numAnn; i++) {
//            CaretAssertVectorIndex(annotationTexts, i);
//            const AnnotationText* annText = annotationTexts[i];
//            const AnnotationPercentSizeText* annPercentText = dynamic_cast<const AnnotationPercentSizeText*>(annText);
//            
//            const float sizeValue = annPercentText->getFontPercentViewportSize();
//            if (i == 0) {
//                fontName = annText->getFont();
//                fontSizeValue = sizeValue;
//            }
//            else {
//                if (annText->getFont() != fontName) {
//                    fontNameValid = false;
//                }
//                if (fontSizeValue != sizeValue) {
//                    haveMultipleFontSizeValues = true;
//                    fontSizeValue = std::min(fontSizeValue,
//                                             sizeValue);
//                }
//            }
//            
//            if ( ! annText->isBoldStyleEnabled()) {
//                boldOnFlag = false;
//            }
//            if ( ! annText->isItalicStyleEnabled()) {
//                italicOnFlag = false;
//            }
//            if ( ! annText->isUnderlineStyleEnabled()) {
//                underlineOnFlag = false;
//            }
//        }
//        
//        m_fontNameComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(fontName);
//        
//        updateFontSizeSpinBox(fontSizeValue,
//                              haveMultipleFontSizeValues);
//        
//        /*
//         * Font styles are ON only if all selected
//         * text annotations have the style enabled
//         */
//        m_boldFontAction->setChecked(boldOnFlag);
//        m_italicFontAction->setChecked(italicOnFlag);
//        m_underlineFontAction->setChecked(underlineOnFlag);
//        
//        AnnotationText::setUserDefaultFont(fontName);
//        AnnotationText::setUserDefaultFontPercentViewportSize(fontSizeValue);
//        AnnotationText::setUserDefaultBoldEnabled(boldOnFlag);
//        AnnotationText::setUserDefaultItalicEnabled(italicOnFlag);
//        AnnotationText::setUserDefaultUnderlineEnabled(underlineOnFlag);
//    }
//    
//    setEnabled( ! annotationTexts.empty());
//}

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
 * Gets called when font bold changed.
 */
void
AnnotationFontWidget::fontBoldChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontBold(m_boldFontAction->isChecked(),
                                 annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    AnnotationText::setUserDefaultBoldEnabled(m_boldFontAction->isChecked());
}

/**
 * Gets called when font italic changed.
 */
void
AnnotationFontWidget::fontItalicChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontItalic(m_italicFontAction->isChecked(),
                                   annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    AnnotationText::setUserDefaultItalicEnabled(m_italicFontAction->isChecked());
}

/**
 * Gets called when font name changed.
 */
void
AnnotationFontWidget::fontNameChanged()
{
    const AnnotationTextFontNameEnum::Enum fontName = m_fontNameComboBox->getSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            command->setModeTextFontName(fontName,
                                         annMan->getSelectedAnnotations());
            annMan->applyCommand(command);
            
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            AnnotationText::setUserDefaultFont(fontName);
        }
            break;
        case AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET:
            if (m_annotationColorBar != NULL) {
                m_annotationColorBar->setFont(fontName);
            }
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
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
        {
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            command->setModeTextFontPercentSize(fontPercentSize,
                                                annMan->getSelectedAnnotations());
            annMan->applyCommand(command);
            
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            AnnotationText::setUserDefaultFontPercentViewportSize(fontPercentSize);
        }
            break;
        case AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET:
            if (m_annotationColorBar != NULL) {
                m_annotationColorBar->setFontPercentViewportSize(fontPercentSize);
            }
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontUnderline(m_underlineFontAction->isChecked(),
                                      annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    AnnotationText::setUserDefaultUnderlineEnabled(m_underlineFontAction->isChecked());
}

/**
 * Gets called when font outline changed.
 */
void
AnnotationFontWidget::fontOutlineChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontOutline(m_outlineFontAction->isChecked(),
                                      annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    AnnotationText::setUserDefaultOutlineEnabled(m_outlineFontAction->isChecked());
}

/**
 * Create a horizontal alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    Pixmap with icon for the given horizontal alignment.
 */
QPixmap
AnnotationFontWidget::createOutlineButtonPixmap(const QWidget* widget)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24;
    float height = 24;
//
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    QPen pen = painter->pen();
    pen.setWidthF(pen.widthF() * 2.0);
    painter->setPen(pen);
    QFont font = painter->font();
    const int fontHeight = 16;
    font.setPointSize(fontHeight);
    painter->setFont(font);
    
    const int boxMargin = 2;
    const int boxWidth  = width - (boxMargin * 2);
    const int boxHeight = height - (boxMargin * 2);
    const int boxX = boxMargin;
    const int boxY = boxMargin;
    painter->drawRect(boxX, boxY, boxWidth, boxHeight);
    
    painter->drawText(boxX, boxY + 2,
                      boxWidth, boxHeight - 2,
                      (Qt::AlignCenter),
                      "O");
    
    return pixmap;
}

