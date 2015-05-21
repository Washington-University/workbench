
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
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(textLabel, 0, Qt::AlignHCenter);
    layout->addWidget(editToolButton, 0, Qt::AlignHCenter);
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
    
    setEnabled(m_annotationText != NULL);
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

