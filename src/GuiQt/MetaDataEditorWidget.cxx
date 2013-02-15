
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
    QObject::connect(m_deleteActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(deleteActionTriggered(int)));

    m_newPushButton = new QPushButton("New...");
    QObject::connect(m_newPushButton, SIGNAL(clicked()),
                     this, SLOT(newPushButtonClicked()));
    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addWidget(m_newPushButton);
    buttonsLayout->addStretch();
    
    m_metaDataWidgetLayout = new QGridLayout();
    m_metaDataWidgetLayout->addWidget(new QLabel("Delete"),
                                      0, COLUMN_DELETE);
    m_metaDataWidgetLayout->addWidget(new QLabel("Name"),
                                      0, COLUMN_NAME);
    m_metaDataWidgetLayout->addWidget(new QLabel("Value"),
                                      0, COLUMN_VALUE);
    
    QHBoxLayout* dialogLayout = new QHBoxLayout(this);
    dialogLayout->addLayout(m_metaDataWidgetLayout, 100);
    dialogLayout->addLayout(buttonsLayout, 0);
    
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
        const AString name = metaDataNames[iRow];
        const AString value = m_metaDataBeingEdited->get(name);
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
            
            const int layoutRow = m_metaDataWidgetLayout->rowCount();
            m_metaDataWidgetLayout->addWidget(widgetsRow->m_deleteToolButton,
                                              layoutRow,
                                              COLUMN_DELETE);
            m_metaDataWidgetLayout->addWidget(widgetsRow->m_nameLineEdit,
                                              layoutRow,
                                              COLUMN_NAME);
            m_metaDataWidgetLayout->addWidget(widgetsRow->m_valueLineEdit,
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
    
    const int numItems = static_cast<int32_t>(m_namesAndValues.size());
    if (numItems != m_unmodifiedNamesAndValues.size()) {
        std::cout << "MetaData has changed." << std::endl;
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
    
    std::cout << "MetaData modfied: " << (theSame ? "No" : "Yes") << std::endl;
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

    m_nameLineEdit = new QLineEdit();
    
    m_valueLineEdit = new QLineEdit();
    
    m_widgetGroup = new WuQWidgetObjectGroup(parent);
    m_widgetGroup->add(deleteAction);
    m_widgetGroup->add(m_deleteToolButton);
    m_widgetGroup->add(m_nameLineEdit);
    m_widgetGroup->add(m_valueLineEdit);
}

MetaDataEditorWidget::MetaDataWidgetRow::~MetaDataWidgetRow()
{
    
}

