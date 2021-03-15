
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

#define __META_DATA_EDITOR_WIDGET_DECLARE__
#include "MetaDataEditorWidget.h"
#undef __META_DATA_EDITOR_WIDGET_DECLARE__

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
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::MetaDataEditorWidget 
 * \brief Widget for editing GIFTI MetaData.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *   Parent widget.
 */
MetaDataEditorWidget::MetaDataEditorWidget(QWidget* parent)
: QWidget(parent)
{
    m_metaDataBeingEdited = NULL;

    m_deleteActionSignalMapper = new QSignalMapper();
#if QT_VERSION >= 0x060000
    QObject::connect(m_deleteActionSignalMapper, &QSignalMapper::mappedInt,
                     this, &MetaDataEditorWidget::deleteActionTriggered);
#else
    QObject::connect(m_deleteActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(deleteActionTriggered(int)));
#endif

    m_newPushButton = new QPushButton("New...");
    QObject::connect(m_newPushButton, SIGNAL(clicked()),
                     this, SLOT(newPushButtonClicked()));
    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_newPushButton);
    buttonsLayout->addSpacing(10);
    
    m_metaGridLayout = new QGridLayout();
    m_metaGridLayout->addWidget(new QLabel("Delete"),
                                      0, COLUMN_DELETE,
                                      Qt::AlignCenter);
    m_metaGridLayout->addWidget(new QLabel("Name"),
                                      0, COLUMN_NAME,
                                      Qt::AlignCenter);
    m_metaGridLayout->addWidget(new QLabel("Value"),
                                      0, COLUMN_VALUE,
                                      Qt::AlignCenter);
    
    QWidget* metaDataWidget = new QWidget();
    QVBoxLayout* metaDataWidgetLayout = new QVBoxLayout(metaDataWidget);
    metaDataWidgetLayout->addLayout(m_metaGridLayout);
    metaDataWidgetLayout->addStretch();
    
    m_metaDataNameValueScrollArea = new QScrollArea();
    m_metaDataNameValueScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_metaDataNameValueScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_metaDataNameValueScrollArea->setWidget(metaDataWidget);
    m_metaDataNameValueScrollArea->setWidgetResizable(true);
    m_metaDataNameValueScrollArea->setFrameShape(QFrame::NoFrame);
    
    QHBoxLayout* dialogLayout = new QHBoxLayout(this);
    dialogLayout->addWidget(m_metaDataNameValueScrollArea, 100);
    dialogLayout->addLayout(buttonsLayout, 0);
    
    setMinimumWidth(600);
    setMinimumHeight(260);
}

/**
 * Destructor.
 */
MetaDataEditorWidget::~MetaDataEditorWidget()
{
}

/**
 * Called when Add push button is clicked.
 */
void
MetaDataEditorWidget::newPushButtonClicked()
{
    WuQDataEntryDialog ded("New Metadata Name",
                           m_newPushButton);
    m_newNameDialogLineEdit = ded.addLineEditWidget("New MetaData Name");
    
    QObject::connect(&ded, SIGNAL(validateData(WuQDataEntryDialog*)),
                     this, SLOT(validateNewName(WuQDataEntryDialog*)));
    
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        readNamesAndValues();
        
        m_namesAndValues.push_back(std::make_pair(m_newNameDialogLineEdit->text().trimmed(),
                                                  ""));
        
        displayNamesAndValues();
        
        const int32_t numNames = static_cast<int32_t>(m_namesAndValues.size());
        m_metaDataWidgetRows[numNames - 1]->m_valueLineEdit->setFocus();
        
        m_metaDataNameValueScrollArea->ensureWidgetVisible(m_metaDataWidgetRows[numNames-1]->m_nameLineEdit);
    }
}

/**
 * Validate the new name from new name dialog.
 * @param dataEntryDialog
 *    The dialog used for entry of new name.
 */
void
MetaDataEditorWidget::validateNewName(WuQDataEntryDialog* dataEntryDialog)
{
    const AString newName = m_newNameDialogLineEdit->text().trimmed();
    if (newName.isEmpty()) {
        dataEntryDialog->setDataValid(false, "Name may not be empty.");
        return;
    }
    
    std::set<AString> allNames;
    
    getNamesInDialog(allNames,
                     NULL,
                     NULL);
    
    if (std::find(allNames.begin(),
                  allNames.end(),
                  newName) != allNames.end()) {
        const AString msg = ("Name \""
                             + newName
                             + "\" already exists.  Duplicate names are not allowed.");
        dataEntryDialog->setDataValid(false,
                                      Qt::convertFromPlainText(msg));
        return;
    }
    
    dataEntryDialog->setDataValid(true,
                                  "");
}

/**
 * Load the given metadata in this widget.
 *
 * @param metaData
 *     Metadata that displayed in widget.
 */
void
MetaDataEditorWidget::loadMetaData(GiftiMetaData* metaData)
{
    CaretAssert(metaData);
    m_metaDataBeingEdited = metaData;
    
    std::vector<AString> metaDataNames = m_metaDataBeingEdited->getAllMetaDataNames();
    const int32_t numMetaData = static_cast<int32_t>(metaDataNames.size());
    
    /*
     * Get names and values
     */
    m_unmodifiedNamesAndValues.clear();
    m_namesAndValues.clear();
    for (int32_t iRow = 0; iRow < numMetaData; iRow++) {
        const AString name = metaDataNames[iRow].trimmed();
        const AString value = m_metaDataBeingEdited->get(name).trimmed();
        m_namesAndValues.push_back(std::make_pair(name,
                                                  value));
        m_unmodifiedNamesAndValues.insert(std::make_pair(name,
                                                         value));
    }
    
    displayNamesAndValues();
}

/**
 * Display the names and values.
 */
void
MetaDataEditorWidget::displayNamesAndValues()
{
    const int32_t numNamesAndValues = static_cast<int32_t>(m_namesAndValues.size());
    
    int32_t numWidgetRows = static_cast<int32_t>(m_metaDataWidgetRows.size());
    
    /*
     * Update existing rows and add new rows as needed.
     */
    for (int32_t iRow = 0; iRow < numNamesAndValues; iRow++) {
        const AString name  = m_namesAndValues[iRow].first;
        const AString value = m_namesAndValues[iRow].second;
        
        MetaDataWidgetRow* widgetsRow = NULL;
        if (iRow < numWidgetRows) {
            widgetsRow = m_metaDataWidgetRows[iRow];
        }
        else {
            widgetsRow = new MetaDataWidgetRow(this,
                                               m_deleteActionSignalMapper,
                                               iRow);
            
            const int layoutRow = m_metaGridLayout->rowCount();
            m_metaGridLayout->addWidget(widgetsRow->m_deleteToolButton,
                                              layoutRow,
                                              COLUMN_DELETE,
                                              Qt::AlignCenter);
            m_metaGridLayout->addWidget(widgetsRow->m_nameLineEdit,
                                              layoutRow,
                                              COLUMN_NAME);
            m_metaGridLayout->addWidget(widgetsRow->m_valueLineEdit,
                                              layoutRow,
                                              COLUMN_VALUE);
            m_metaDataWidgetRows.push_back(widgetsRow);
        }
        widgetsRow->m_nameLineEdit->setText(name);
        widgetsRow->m_valueLineEdit->setText(value);
        widgetsRow->m_widgetGroup->setVisible(true);
    }
    
    /*
     * Hide rows that are no longer used.
     */
    numWidgetRows = static_cast<int32_t>(m_metaDataWidgetRows.size());
    for (int32_t iRow = numNamesAndValues; iRow < numWidgetRows; iRow++) {
        m_metaDataWidgetRows[iRow]->m_widgetGroup->setVisible(false);
    }
}

/**
 * Read the names and values from the GUI.
 */
void
MetaDataEditorWidget::readNamesAndValues()
{
    m_namesAndValues.clear();
    
    /*
     * Read from the rows.
     */
    int32_t numWidgetRows = static_cast<int32_t>(m_metaDataWidgetRows.size());
    for (int32_t iRow = 0; iRow < numWidgetRows; iRow++) {
        MetaDataWidgetRow* widgetRow = m_metaDataWidgetRows[iRow];
        if (widgetRow->m_widgetGroup->isVisible()) {
            m_namesAndValues.push_back(std::make_pair(widgetRow->m_nameLineEdit->text().trimmed(),
                                                      widgetRow->m_valueLineEdit->text().trimmed()));
        }
    }
}

/**
 * Get the names that are listed in the editor.
 *
 * @param namesOut
 *    Output will contain all unique names.
 * @param duplicateNamesOut
 *    If not NULL, any duplicate names will be placed here.
 * @param haveEmptyNamesOut
 *    If not NULL, will be true if any empty names were found.
 * @return true if all names valid, else false.
 */
bool
MetaDataEditorWidget::getNamesInDialog(std::set<AString>& namesOut,
                                       std::set<AString>* duplicateNamesOut,
                                       bool* haveEmptyNamesOut)
{
    readNamesAndValues();

    namesOut.clear();
    
    if (duplicateNamesOut != NULL) {
        duplicateNamesOut->clear();
    }
    if (haveEmptyNamesOut != NULL) {
        *haveEmptyNamesOut = false;
    }
    
    bool allValidNames = true;
    
    const int32_t numItems = static_cast<int32_t>(m_namesAndValues.size());
    for (int32_t i = 0; i < numItems; i++) {
        const AString name = m_namesAndValues[i].first;
        if (name.isEmpty()) {
            if (haveEmptyNamesOut != NULL) {
                *haveEmptyNamesOut = true;
            }
            allValidNames = false;
        }
        else if (std::find(namesOut.begin(),
                           namesOut.end(),
                           name) != namesOut.end()) {
            if (duplicateNamesOut != NULL) {
                duplicateNamesOut->insert(name);
            }
            allValidNames = false;
        }
        else {
            namesOut.insert(name);
        }
    }

    return allValidNames;
}

/**
 * Transfer values in dialog into metadata.
 *
 * @return Empty string if no errors, otherwise error message.
 */
AString
MetaDataEditorWidget::saveMetaData()
{
    if (isMetaDataModified() == false) {
        return "";
    }
    
    readNamesAndValues();

    std::set<AString> allNames;
    std::set<AString> duplicateNames;
    bool haveEmptyNames;
    
    const bool valid = getNamesInDialog(allNames,
                                        &duplicateNames,
                                        &haveEmptyNames);
    
    if (valid) {
        m_metaDataBeingEdited->clear();
        const int32_t numItems = static_cast<int32_t>(m_namesAndValues.size());
        for (int32_t i = 0; i < numItems; i++) {
            const AString name = m_namesAndValues[i].first;
            const AString value = m_namesAndValues[i].second;
            m_metaDataBeingEdited->set(name,
                                       value);
        }
        return "";
    }

    AString errorMessage = "";
    
    if (haveEmptyNames) {
        errorMessage.appendWithNewLine("Empty names are not allowed.");
    }
    
    if (duplicateNames.empty() == false) {
        errorMessage.appendWithNewLine("Duplicate names are not allowed:");
        for (std::set<AString>::iterator iter = duplicateNames.begin();
             iter != duplicateNames.end();
             iter++) {
            errorMessage.appendWithNewLine("    " + *iter);
        }
    }
    
    return errorMessage;
}

/**
 * @return true if the names and values been modified, else false.
 */
bool
MetaDataEditorWidget::isMetaDataModified()
{
    readNamesAndValues();
    
    const int32_t numItems = static_cast<int32_t>(m_namesAndValues.size());
    if (numItems != static_cast<int32_t>(m_unmodifiedNamesAndValues.size())) {
        return true;
    }
    
    std::map<AString, AString> nameValueMap;
    for (int32_t i = 0; i < numItems; i++) {
        nameValueMap.insert(std::make_pair(m_namesAndValues[i].first,
                                           m_namesAndValues[i].second));
    }
    
    
    const bool theSame = std::equal(m_unmodifiedNamesAndValues.begin(),
                                    m_unmodifiedNamesAndValues.end(),
                                    nameValueMap.begin());
    
    return (theSame == false);
}


/**
 * Called when a delete tool button is clicked.
 */
void
MetaDataEditorWidget::deleteActionTriggered(int indx)
{
    readNamesAndValues();
    
    CaretAssertVectorIndex(m_namesAndValues, indx);
    m_namesAndValues.erase(m_namesAndValues.begin() + indx);
    
    displayNamesAndValues();
}

/**
 * Constructor.
 * @param parent
 *   The parent widget.
 * @param deleteActionSignalMapper
 *   The signal mapper for delete action.
 * @param rowIndex
 *   Row index of widgets and used by signal mapper. 
 */
MetaDataEditorWidget::MetaDataWidgetRow::MetaDataWidgetRow(QWidget* parent,
                                                           QSignalMapper* deleteActionSignalMapper,
                                                           const int32_t rowIndex)
{
    QAction* deleteAction = new QAction("X",
                                        parent);
    QObject::connect(deleteAction, SIGNAL(triggered(bool)),
                     deleteActionSignalMapper, SLOT(map()));
    deleteActionSignalMapper->setMapping(deleteAction, rowIndex);
    
    m_deleteToolButton = new QToolButton();
    m_deleteToolButton->setDefaultAction(deleteAction);

    const int minWidth = 150;
    
    m_nameLineEdit = new QLineEdit();
    m_nameLineEdit->setMinimumWidth(minWidth);
    
    m_valueLineEdit = new QLineEdit();
    m_valueLineEdit->setMinimumWidth(minWidth);
    
    m_widgetGroup = new WuQWidgetObjectGroup(parent);
    m_widgetGroup->add(deleteAction);
    m_widgetGroup->add(m_deleteToolButton);
    m_widgetGroup->add(m_nameLineEdit);
    m_widgetGroup->add(m_valueLineEdit);
}

MetaDataEditorWidget::MetaDataWidgetRow::~MetaDataWidgetRow()
{
    
}

