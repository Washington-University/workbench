
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
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "AnnotationTextEditorDialog.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
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
:
QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* textLabel = new QLabel("Text");

    m_textLineEdit = new AnnotationLineEdit();
    QObject::connect(m_textLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(annotationTextChanged()));
    QObject::connect(m_textLineEdit, SIGNAL(doubleClickInLineEdit()),
                     this, SLOT(displayTextEditor()));
    m_textLineEdit->setToolTip("Insert lines breaks with '\\n' \n"
                               "Double-click text to edit text in a multi-line text dialog");
    
    const QString textConnectToolTip("Connect text to brainordinate with arrow or line");
    m_annotationTextConnectTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationTextConnectTypeEnumComboBox->getWidget()->setToolTip(textConnectToolTip);
    m_annotationTextConnectTypeEnumComboBox->setup<AnnotationTextConnectTypeEnum,AnnotationTextConnectTypeEnum::Enum>();
    QObject::connect(m_annotationTextConnectTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationTextConnectTypeEnumComboBoxItemActivated()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(textLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_textLineEdit); //, 0, Qt::AlignHCenter);
    layout->addWidget(m_annotationTextConnectTypeEnumComboBox->getWidget(), 0, Qt::AlignHCenter);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
    setMaximumWidth(200);
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
    
    QString text;
    
    if (m_annotationText != NULL) {
        setEnabled(true);
        
        if (m_annotationText->isConnectToBrainordinateValid()) {
            m_annotationTextConnectTypeEnumComboBox->getComboBox()->setEnabled(true);
        }
        
        text = m_annotationText->getText();
    }
    else {
        setEnabled(false);
    }

    updateLineEditText(text);
}

/**
 * Gets called when the edit button is clicked.
 */
void
AnnotationTextEditorWidget::displayTextEditor()
{
    if (m_annotationText != NULL) {
        AnnotationTextEditorDialog ted(m_annotationText,
                                       this);
        QObject::connect(&ted, SIGNAL(textHasBeenChanged(const QString&)),
                         this, SLOT(textEditorDialogTextChanged(const QString&)));
        ted.exec();
    }
}

/**
 * Gets called when text is edited in the text editor dialog.
 */
void
AnnotationTextEditorWidget::textEditorDialogTextChanged(const QString& text)
{
    updateLineEditText(text);
    annotationTextChanged();
}


/**
 * Gets called when the annotation text is changed.
 */
void
AnnotationTextEditorWidget::annotationTextChanged()
{
    QString s(m_textLineEdit->text().trimmed());
    s.replace("\\n", "\n");
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeTextCharacters(s, annMan->getSelectedAnnotations());
    annMan->applyCommand(undoCommand);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the text in the line edit including
 * replacing newline characters with "\n"
 *
 * @param text
 *    Text for the line edit.
 */
void
AnnotationTextEditorWidget::updateLineEditText(const QString& text)
{
    QString s(text);
    s.replace("\n", "\\n");
    m_textLineEdit->setText(s);
}

/**
 * Gets called when the annotation connect to brainordinate is changed.
 */
void
AnnotationTextEditorWidget::annotationTextConnectTypeEnumComboBoxItemActivated()
{
    if (m_annotationText != NULL) {
        const AnnotationTextConnectTypeEnum::Enum connectType = m_annotationTextConnectTypeEnumComboBox->getSelectedItem<AnnotationTextConnectTypeEnum,AnnotationTextConnectTypeEnum::Enum>();
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeTextConnectToBrainordinate(connectType,
                                                       annMan->getSelectedAnnotations());
        annMan->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    
}

