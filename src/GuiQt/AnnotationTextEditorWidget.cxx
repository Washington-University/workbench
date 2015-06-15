
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

#define __ANNOTATION_TEXT_EDITOR_WIDGET_DECLARE__
#include "AnnotationTextEditorWidget.h"
#undef __ANNOTATION_TEXT_EDITOR_WIDGET_DECLARE__

#include <QLabel>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQDataEntryDialog.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationTextEditorWidget 
 * \brief Widget for editing annotation text.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of browser window.
 * @param parent
 *     The parent widget.
 */
AnnotationTextEditorWidget::AnnotationTextEditorWidget(const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* textLabel = new QLabel("Text");
    
    QAction* textEditAction = WuQtUtilities::createAction("Edit", "Edit the selected annotation's text",
                                                          this, this, SLOT(editTextActionClicked()));
    m_textEditorInDialog = NULL;

    QToolButton* editToolButton = new QToolButton();
    editToolButton->setDefaultAction(textEditAction);
    
    const QString textConnectToolTip("Connect text to brainordinate with arrow or line");
    m_annotationTextConnectTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationTextConnectTypeEnumComboBox->getWidget()->setToolTip(textConnectToolTip);
    m_annotationTextConnectTypeEnumComboBox->setup<AnnotationTextConnectTypeEnum,AnnotationTextConnectTypeEnum::Enum>();
    QObject::connect(m_annotationTextConnectTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationTextConnectTypeEnumComboBoxItemActivated()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(textLabel, 0, Qt::AlignHCenter);
    layout->addWidget(editToolButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_annotationTextConnectTypeEnumComboBox->getWidget(), 0, Qt::AlignHCenter);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationTextEditorWidget::~AnnotationTextEditorWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationTextEditorWidget::updateContent(AnnotationText* annotationText)
{
    m_annotationText = annotationText;

    
    AnnotationTextConnectTypeEnum::Enum connectValue = AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE;
    if (m_annotationText != NULL) {
        connectValue = m_annotationText->getConnectToBrainordinate();
    }
    m_annotationTextConnectTypeEnumComboBox->setSelectedItem<AnnotationTextConnectTypeEnum,AnnotationTextConnectTypeEnum::Enum>(connectValue);
    
    if (m_annotationText != NULL) {
        setEnabled(true);
        
        if (m_annotationText->isConnectToBrainordinateValid()) {
            m_annotationTextConnectTypeEnumComboBox->getComboBox()->setEnabled(true);
        }
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when the edit button is clicked.
 */
void
AnnotationTextEditorWidget::editTextActionClicked()
{
    if (m_annotationText != NULL) {
        AString savedText = m_annotationText->getText();
        
        WuQDataEntryDialog ded("Edit Annotation Text",
                               this,
                               false);
        m_textEditorInDialog = ded.addTextEdit("",
                                               savedText,
                                               false);
        QObject::connect(m_textEditorInDialog, SIGNAL(textChanged()),
                         this, SLOT(annotationTextChanged()));
        
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            annotationTextChanged();
        }
        else {
            m_textEditorInDialog->setText(savedText);
            annotationTextChanged();
        }
    }
    
    m_textEditorInDialog = NULL;
}

/**
 * Gets called when the annotation text is changed.
 */
void
AnnotationTextEditorWidget::annotationTextChanged()
{
    if (m_textEditorInDialog != NULL) {
        if (m_annotationText != NULL) {
            m_annotationText->setText(m_textEditorInDialog->toPlainText().trimmed());
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        }
    }
}

/**
 * Gets called when the annotation connect to brainordinate is changed.
 */
void
AnnotationTextEditorWidget::annotationTextConnectTypeEnumComboBoxItemActivated()
{
    if (m_annotationText != NULL) {
        const AnnotationTextConnectTypeEnum::Enum connectType = m_annotationTextConnectTypeEnumComboBox->getSelectedItem<AnnotationTextConnectTypeEnum,AnnotationTextConnectTypeEnum::Enum>();
        m_annotationText->setConnectToBrainordinate(connectType);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    
}

