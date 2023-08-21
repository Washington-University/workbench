
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__
#include "MetaDataCustomEditorWidget.h"
#undef __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__

#include <set>

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "GiftiMetaDataXmlElements.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::MetaDataCustomEditorWidget
 * \brief Widget for editing GIFTI MetaData.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param metaDataNames
 *    Names of metadata shown in editor
 * @param commentEditorStatus
 *    Whether or not to show comment editor
 * @param metaData
 *    Metadata instance
 * @param parent
 *    Parent widget.
 */
MetaDataCustomEditorWidget::MetaDataCustomEditorWidget(const std::vector<AString>& metaDataNames,
                                                       GiftiMetaData* metaData,
                                                       QWidget* parent)
: QWidget(parent),
m_metaData(metaData)
{
    bool hasCommentMetaDataFlag(false);
    const int32_t COLUMN_LABEL(0);
    const int32_t COLUMN_VALUE(1);
    int32_t rowIndex(0);
    QGridLayout* gridLayout(new QGridLayout(this));
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_VALUE, 100);
    for (const auto& name : metaDataNames) {
        if (name == GiftiMetaDataXmlElements::METADATA_NAME_COMMENT) {
            /* Comment uses a text editor, below */
            hasCommentMetaDataFlag = true;
        }
        else {
            MetaDataWidgetRow* mdwr(new MetaDataWidgetRow(name,
                                                          m_metaData));
            m_metaDataWidgetRows.push_back(mdwr);
            
            gridLayout->addWidget(mdwr->m_nameLabel,
                                  rowIndex, COLUMN_LABEL);
            gridLayout->addWidget(mdwr->m_valueLineEdit,
                                  rowIndex, COLUMN_VALUE);
            ++rowIndex;
        }
    }
    
    m_commentTextEditor = NULL;
    if (hasCommentMetaDataFlag) {
        QLabel* commentLabel(new QLabel(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT));
        m_commentTextEditor = new QTextEdit();
        m_commentTextEditor->setText(m_metaData->get(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT));
        gridLayout->addWidget(commentLabel,
                              rowIndex, COLUMN_LABEL);
        gridLayout->addWidget(m_commentTextEditor,
                              rowIndex, COLUMN_VALUE);
        ++rowIndex;
    }
}

/**
 * Destructor.
 */
MetaDataCustomEditorWidget::~MetaDataCustomEditorWidget()
{
}

/**
 * Transfer values in dialog into metadata.
 */
void
MetaDataCustomEditorWidget::saveMetaData()
{
    for (auto& mdwr : m_metaDataWidgetRows) {
        mdwr->saveToMetaData();
    }

    if (m_commentTextEditor != NULL) {
        m_metaData->set(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT,
                        m_commentTextEditor->toPlainText());
    }
}

/**
 * Validate the the required metadata (values must not be empty)
 * @param requiredMetaDataNames
 *    Names of required metadata
 * @param errorMessageOut
 *    Contains error message if validation fails
 * @return
 *    True if metadata is valid, else false.
 */
bool
MetaDataCustomEditorWidget::validateAndSaveRequiredMetaData(const std::vector<AString>& requiredMetaDataNames,
                                                            AString& errorMessageOut)
{
    CaretAssert(m_metaData);
    saveMetaData();
    return m_metaData->validateRequiredMetaData(requiredMetaDataNames,
                                                errorMessageOut);
}

/**
 * Constructor.
 * @param metaDataName
 *   The  name of the metadata
 * @param metaData
 *   The metadata.
 */
MetaDataCustomEditorWidget::MetaDataWidgetRow::MetaDataWidgetRow(const AString& metaDataName,
                                                                 GiftiMetaData* metaData)
: m_metaDataName(metaDataName),
m_metaData(metaData)
{
    CaretAssert(m_metaData);
    
    m_nameLabel = new QLabel(metaDataName + ":");
    
    m_valueLineEdit = new QLineEdit();
    m_valueLineEdit->setText(metaData->get(metaDataName));
}

MetaDataCustomEditorWidget::MetaDataWidgetRow::~MetaDataWidgetRow()
{
    
}

/**
 * Save the value to the metadata
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::saveToMetaData()
{
    m_metaData->set(m_metaDataName,
                    m_valueLineEdit->text().trimmed());
}

