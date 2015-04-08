
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
#include <QTooLButton>
#include <QVBoxLayout>

#define __ANNOTATION_FONT_WIDGET_DECLARE__
#include "AnnotationFontWidget.h"
#undef __ANNOTATION_FONT_WIDGET_DECLARE__

#include "AnnotationFontNameEnum.h"
#include "AnnotationFontSizeEnum.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
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
 * @param parent
 *     Parent for this widget.
 */
AnnotationFontWidget::AnnotationFontWidget(QWidget* parent)
: QWidget(parent)
{
    /*
     * "Font" label
     */
    QLabel* fontLabel = new QLabel("Font ");
    m_annotationText = NULL;
    
    /*
     * Combo box for font name selection
     */
    m_fontNameComboBox = new EnumComboBoxTemplate(this);
    m_fontNameComboBox->setup<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>();
    QObject::connect(m_fontNameComboBox, SIGNAL(itemActivated()),
                     this, SLOT(slotItemValueChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontNameComboBox->getWidget(),
                                          "Change font");
    
    /*
     * Combo box for font size
     */
    m_fontSizeComboBox = new EnumComboBoxTemplate(this);
    m_fontSizeComboBox->setup<AnnotationFontSizeEnum,AnnotationFontSizeEnum::Enum>();
    QObject::connect(m_fontSizeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(slotItemValueChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontSizeComboBox->getWidget(),
                                          "Change font size (height)");
    
    /*
     * Bold Font
     */
    m_boldFontAction = WuQtUtilities::createAction("B", //boldFontText.toStringWithHtmlBody(),
                                                   "Enable/disable bold styling",
                                                   this, this, SLOT(slotItemValueChanged()));
    m_boldFontAction->setCheckable(true);
    QToolButton* boldFontToolButton = new QToolButton();
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
                                                     this, this, SLOT(slotItemValueChanged()));
    m_italicFontAction->setCheckable(true);
    QToolButton* italicFontToolButton = new QToolButton();
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
                                                         this, this, SLOT(slotItemValueChanged()));
    m_underlineFontAction->setCheckable(true);
    QToolButton* underlineFontToolButton = new QToolButton();
    underlineFontToolButton->setDefaultAction(m_underlineFontAction);
    
    /*
     * Change the underline toolbutton's font to underline.
     */
    QFont underlineFont = underlineFontToolButton->font();
    underlineFont.setUnderline(true);
    underlineFontToolButton->setFont(underlineFont);
    
    /*
     * Layout the widgets
     */
    QHBoxLayout* topRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(topRowLayout, 2, 0);
    topRowLayout->addWidget(fontLabel, 0);
    topRowLayout->addWidget(m_fontNameComboBox->getWidget(), 100);
    
    
    
    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 0);
    bottomRowLayout->addWidget(boldFontToolButton);
    bottomRowLayout->addWidget(italicFontToolButton);
    bottomRowLayout->addWidget(underlineFontToolButton);
    bottomRowLayout->addStretch();
    bottomRowLayout->addWidget(m_fontSizeComboBox->getWidget());
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(topRowLayout);
    layout->addLayout(bottomRowLayout);
//    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
//    WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 0);
//    bottomRowLayout->addWidget(boldFontToolButton);
//    bottomRowLayout->addWidget(italicFontToolButton);
//    bottomRowLayout->addWidget(underlineFontToolButton);
//    bottomRowLayout->addStretch();
//    
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
//    layout->addLayout(topRowLayout);
//    layout->addLayout(bottomRowLayout);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);

    
    //m_foregroundColorAction;
    
    //QAction* m_backgroundColorAction;

    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
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
 *    An event for which this instance is listening.
 */
void
AnnotationFontWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Update the content of this widget with the given text annotation.
 * 
 * @param annotationText
 *     Text annotation for display (may be NULL).
 */
void
AnnotationFontWidget::updateContent(AnnotationText* annotationText)
{
    m_annotationText = annotationText;
    
    if (m_annotationText != NULL) {
        m_fontNameComboBox->setSelectedItem<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>(m_annotationText->getFont());

        m_fontSizeComboBox->setSelectedItem<AnnotationFontSizeEnum,AnnotationFontSizeEnum::Enum>(m_annotationText->getFontSize());
        
        m_boldFontAction->setChecked(m_annotationText->isBoldEnabled());
        m_italicFontAction->setChecked(m_annotationText->isItalicEnabled());
        m_underlineFontAction->setChecked(m_annotationText->isUnderlineEnabled());
    }
    
    setEnabled(m_annotationText != NULL);
}

/**
 * Gets called when the user changes a GUI controls value.
 */
void
AnnotationFontWidget::slotItemValueChanged()
{
    if (m_annotationText != NULL) {
        const AnnotationFontNameEnum::Enum fontName = m_fontNameComboBox->getSelectedItem<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>();
        m_annotationText->setFont(fontName);
        
        const AnnotationFontSizeEnum::Enum fontSize = m_fontSizeComboBox->getSelectedItem<AnnotationFontSizeEnum, AnnotationFontSizeEnum::Enum>();
        m_annotationText->setFontSize(fontSize);
        
        m_annotationText->setBoldEnabled(m_boldFontAction->isChecked());
        m_annotationText->setItalicEnabled(m_italicFontAction->isChecked());
        m_annotationText->setUnderlineEnabled(m_underlineFontAction->isChecked());
    }
}


