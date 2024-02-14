
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
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QStackedWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "AnnotationMetaDataNames.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DingOntologyTermsDialog.h"
#include "GiftiMetaData.h"
#include "GiftiMetaDataXmlElements.h"
#include "GuiManager.h"
#include "LabelSelectionDialog.h"
#include "SamplesFile.h"
#include "SamplesMetaDataManager.h"
#include "StructureEnum.h"
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
 * @param mode
 *    Mode of metadata editing
 * @param metaDataNames
 *    Names of metadata shown in editor
 * @param requiredMetaDataNames
 *    Names of required metadata names
 * @param metaData
 *    Metadata instance
 * @param parent
 *    Parent widget.
 */
MetaDataCustomEditorWidget::MetaDataCustomEditorWidget(const Mode mode,
                                                       const std::vector<AString>& metaDataNames,
                                                       const std::vector<AString>& requiredMetaDataNames,
                                                       GiftiMetaData* userMetaData,
                                                       QWidget* parent)
: QWidget(parent),
m_mode(mode),
m_userMetaData(userMetaData)
{
    CaretAssert(m_userMetaData);
    
    /*
     * Clone metadata since it may be a subclass of GiftiMetaData
     * and virtual methods may be called.  We also clone the metadata
     * since user may cancel editing after making modifications.
     */
    m_editorMetaData.reset(m_userMetaData->clone());
    m_editorMetaData->clearModified();
    
    const int32_t COLUMN_LABEL(0);
    const int32_t COLUMN_VALUE(1);
    const int32_t COLUMN_BUTTON(2);
    int32_t rowIndex(0);
    QGridLayout* gridLayout(new QGridLayout(this));
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_VALUE, 100);
    for (AString name : metaDataNames) {
        const bool requiredMetaDataFlag(std::find(requiredMetaDataNames.begin(),
                                                  requiredMetaDataNames.end(),
                                                  name) != requiredMetaDataNames.end());
        if ((name == GiftiMetaDataXmlElements::METADATA_NAME_COMMENT)
            || (name == AnnotationMetaDataNames::SAMPLES_COMMENT)) {
            /* Comment uses a text editor, below */
            m_commentMetaDataName = name;
        }
        else {
            MetaDataWidgetRow* mdwr(new MetaDataWidgetRow(this,
                                                          gridLayout,
                                                          rowIndex,
                                                          COLUMN_LABEL,
                                                          COLUMN_VALUE,
                                                          COLUMN_BUTTON,
                                                          name,
                                                          m_editorMetaData.get(),
                                                          requiredMetaDataFlag));
            mdwr->setSavingEnabled(false);
            mdwr->updateValueWidget();
            mdwr->setSavingEnabled(true);
            m_metaDataWidgetRows.push_back(mdwr);
            ++rowIndex;
        }
    }
    
    m_commentTextEditor = NULL;
    if ( ! m_commentMetaDataName.isEmpty()) {
        QLabel* commentLabel(new QLabel(m_commentMetaDataName + ":"));
        m_commentTextEditor = new QTextEdit();
        m_commentTextEditor->setText(m_editorMetaData->get(m_commentMetaDataName));
        gridLayout->addWidget(commentLabel,
                              rowIndex, COLUMN_LABEL);
        gridLayout->addWidget(m_commentTextEditor,
                              rowIndex, COLUMN_VALUE, 1, 2);
        ++rowIndex;
    }
    
    /*
     * Generates Sample Number if it is empty and there is an allen slab number
     */
    if (m_editorMetaData->get(AnnotationMetaDataNames::SAMPLES_SAMPLE_NUMBER).isEmpty()) {
        calledByMetaDataWidgetRowWhenValueChanges(AnnotationMetaDataNames::SAMPLES_ALLEN_SLAB_NUMBER);
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
        m_editorMetaData->set(m_commentMetaDataName,
                        m_commentTextEditor->toPlainText());
    }
    
    /*
     * Copy metadata from editor copy to the user's metadata
     * Need to save mod stats since metadata is copied
     */
    const bool modStatus(m_userMetaData->isModified()
                         || m_editorMetaData->isModified());
    *m_userMetaData = *m_editorMetaData;
    if (modStatus) {
        m_userMetaData->setModified();
    }
}

/**
 * Gets called by a metadata row when its value has changed
 * @param metaDataName
 *    Name of metadata that changed
 */
void
MetaDataCustomEditorWidget::calledByMetaDataWidgetRowWhenValueChanges(const AString& metaDataName)
{
    switch (m_mode) {
        case NEW_SAMPLE_EDITING:
            if (metaDataName == AnnotationMetaDataNames::SAMPLES_ALLEN_SLAB_NUMBER) {
                const AString slabNumber(m_editorMetaData->get(AnnotationMetaDataNames::SAMPLES_ALLEN_SLAB_NUMBER));
                if ( ! slabNumber.isEmpty()) {
                    AString sampleNumber(m_editorMetaData->get(AnnotationMetaDataNames::SAMPLES_SAMPLE_NUMBER));
                    if (sampleNumber.isEmpty()) {
                        std::vector<SamplesFile*> samplesFiles(GuiManager::get()->getBrain()->getAllSamplesFiles());
                        sampleNumber = SamplesFile::generateSampleNumberFromSlabID(samplesFiles,
                                                                                   slabNumber);
                        m_editorMetaData->set(AnnotationMetaDataNames::SAMPLES_SAMPLE_NUMBER,
                                              sampleNumber);
                    }
                }
            }
            break;
        case NORMAL_EDITING:
            break;
    }

    /*
     * Update all rows since some may be composite elements
     */
    reloadAllMetaDataWidgetRows();
}

/**
 * Update all rows
 */
void
MetaDataCustomEditorWidget::reloadAllMetaDataWidgetRows()
{
    for (auto& mdwr : m_metaDataWidgetRows) {
        mdwr->updateValueWidget();
    }
}

/**
 * @return true if the names and values been modified, else false.
 */
bool
MetaDataCustomEditorWidget::isMetaDataModified() const
{
    CaretAssert(m_editorMetaData);
    return m_editorMetaData->isModified();
}

/**
 * Called when the user clicks a meta data button
 * @param metaDataName
 *    Name of metadata element
 * @param parentDialogWidget
 *    Parent for any dialogs
 */
void
MetaDataCustomEditorWidget::metaDataButtonClicked(const AString& metaDataName,
                                                  QWidget* parentDialogWidget)
{
    const QString metaDataValue(m_editorMetaData->get(metaDataName));
    
    bool dingFlag(false);
    bool labelFilePopupFlag(false);
    bool labelPopupFlag(false);
    bool listPopupFlag(false);
    switch (m_editorMetaData->getDataTypeForMetaDataName(metaDataName)) {
        case GiftiMetaDataElementDataTypeEnum::COMMENT:
            break;
        case GiftiMetaDataElementDataTypeEnum::DATE:
            break;
        case GiftiMetaDataElementDataTypeEnum::DING_ONTOLOGY_TERM:
            dingFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LABEL_FILE_AND_MAP:
            labelFilePopupFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LABEL_ID_NAME:
            labelPopupFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LIST:
            listPopupFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::TEXT:
            break;
    }
    
    if (dingFlag) {
        const SamplesMetaDataManager* smdm(GuiManager::get()->getBrain()->getSamplesMetaDataManager());
        CaretAssert(smdm);
        DingOntologyTermsDialog dotd(smdm->getDingOntologyTermsFile(),
                                     this);
        if (dotd.exec() == DingOntologyTermsDialog::Accepted) {
            const QString shorthandID(dotd.getAbbreviatedName());
            const QString description(dotd.getDescriptiveName());
            
            if (metaDataName == AnnotationMetaDataNames::SAMPLES_DING_ABBREVIATION) {
                m_editorMetaData->set(AnnotationMetaDataNames::SAMPLES_DING_ABBREVIATION,
                                shorthandID);
                m_editorMetaData->set(AnnotationMetaDataNames::SAMPLES_DING_FULL_NAME,
                                description);
                
                updateValueInMetaDataWidgetRow(AnnotationMetaDataNames::SAMPLES_DING_ABBREVIATION);
                updateValueInMetaDataWidgetRow(AnnotationMetaDataNames::SAMPLES_DING_FULL_NAME);
            }
        }
    }
    else if (labelFilePopupFlag) {
        LabelSelectionDialog labelDialog(LabelSelectionDialog::Mode::FILE_AND_MAP,
                                         "MetaDataCustomEditorWidget",
                                         parentDialogWidget);
        if (labelDialog.exec() == LabelSelectionDialog::Accepted) {
            const AString fileName(labelDialog.getSelectedFileNameNoPath());
            const AString mapName(labelDialog.getSelectedMapName());
            const int32_t mapIndex(labelDialog.getSelectedMapIndex());
            if ( ! fileName.isEmpty()) {
                AString text(fileName + " ");
                if ( ! mapName.isEmpty()) {
                    text.append(mapName);
                }
                else {
                    text.append("Map " + AString::number(mapIndex + 1));
                }
                m_editorMetaData->set(metaDataName,
                                      text);
                updateValueInMetaDataWidgetRow(metaDataName);
            }
        }
    }
    else if (labelPopupFlag) {
        LabelSelectionDialog labelDialog(LabelSelectionDialog::Mode::FILE_MAP_AND_LABEL,
                                         "MetaDataCustomEditorWidgetLabel",
                                         parentDialogWidget);
        if (labelDialog.exec() == LabelSelectionDialog::Accepted) {
            const AString labelText(labelDialog.getSelectedLabel().trimmed());
            if ( ! labelText.isEmpty()) {
                if (metaDataName == AnnotationMetaDataNames::SAMPLES_ALT_SHORTHAND_ID) {
                    AString id, description;
                    processLabelForIdDescription(labelText,
                                                 id,
                                                 description);
                    m_editorMetaData->set(AnnotationMetaDataNames::SAMPLES_ALT_SHORTHAND_ID,
                                    id);
                    
                    updateValueInMetaDataWidgetRow(AnnotationMetaDataNames::SAMPLES_ALT_SHORTHAND_ID);
                }
                  else {
                    m_editorMetaData->set(metaDataName,
                                          labelText);
                    updateValueInMetaDataWidgetRow(metaDataName);
                }
            }
        }
    }
    else if (listPopupFlag) {
        QStringList dataSelectionValues;
        
        dataSelectionValues = m_editorMetaData->getValidValuesListForMetaDataName(metaDataName);
        if (dataSelectionValues.empty()) {
            CaretLogSevere("Metadata not supported for selection: "
                           + metaDataName);
            CaretAssert(0);
        }
        
        if (dataSelectionValues.size() > 10) {
            WuQDataEntryDialog ded("Choose " + metaDataName,
                                   parentDialogWidget);
            QComboBox* comboBox(ded.addComboBox("Choose", dataSelectionValues));
            
            /*
             * Initialize combo box with metadata value selected
             */
            if ( ! metaDataValue.isEmpty()) {
                const int32_t index(comboBox->findText(metaDataValue,
                                                       Qt::MatchFixedString));
                if (index >= 0) {
                    comboBox->setCurrentIndex(index);
                }
            }
            
            if (ded.exec() == WuQDataEntryDialog::Accepted) {
                const QString value(comboBox->currentText().trimmed());
                m_editorMetaData->set(metaDataName,
                                value);
                updateValueInMetaDataWidgetRow(metaDataName);
            }
        }
        else if (dataSelectionValues.size() > 0) {
            WuQDataEntryDialog ded("Choose " + metaDataName,
                                   parentDialogWidget);
            for (const QString& text : dataSelectionValues) {
                const bool defaultButtonFlag(text == metaDataValue);
                ded.addRadioButton(text,
                                   defaultButtonFlag);
            }
            if (ded.exec() == WuQDataEntryDialog::Accepted) {
                const int32_t buttonIndex(ded.getRadioButtonSelected());
                if (buttonIndex >= 0) {
                    CaretAssert(buttonIndex < dataSelectionValues.size());
                    const QString value(dataSelectionValues[buttonIndex]);
                    m_editorMetaData->set(metaDataName,
                                          value);
                    updateValueInMetaDataWidgetRow(metaDataName);
                }
            }
        }
    }
}

/**
 * Update the value in the meta data widget row with the given name
 * @param metaDataName
 *    The metadata name
 */
void
MetaDataCustomEditorWidget::updateValueInMetaDataWidgetRow(const QString& metaDataName)
{
    MetaDataWidgetRow* mdwr(getMetaDataWidgetRow(metaDataName));
    if (mdwr != NULL) {
        mdwr->updateValueWidget();
    }
    else {
        CaretLogSevere("Failed to find metadata widget row with name: "
                       + metaDataName);
    }
}

/**
 * @return Metadata widget row with the given name
 * @param metaDataName
 *
 */
MetaDataCustomEditorWidget::MetaDataWidgetRow*
MetaDataCustomEditorWidget::getMetaDataWidgetRow(const QString& metaDataName)
{
    for (MetaDataWidgetRow* mdwr : m_metaDataWidgetRows) {
        if (mdwr->m_metaDataName == metaDataName) {
            return mdwr;
        }
    }
    return NULL;
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
    CaretAssert(m_editorMetaData);
    saveMetaData();
    return m_userMetaData->validateRequiredMetaData(requiredMetaDataNames,
                                                      errorMessageOut);
}

/**
 * Process label's text to find the id and description
 * @param text
 *    The label's text
 * @param idOut
 *    Output with ID
 * @param descriptionOut
 *    Output with description
 *
 *
 From David's email (Nov 3, 2023):
 Unfortunately, we don’t currently have consistency across different labeling schemes. For example, the following four examples are each distinct
 For Ding ontology: <abbreviation>: <name>
 CaH: Head of Caudate
 For SARM-6: <abbreviation>
 CdH
 CdT
 For Saleem: <abbreviation> <name>
 cd caudate nucleus
 For Yerkes19_Parcellations: <shorthand_id>_<parcellation_id>
 3b_LV00
 
 Here’s my suggestion for handling such cases:
 - Have a ‘Choose’ option for just the ‘id’
 - If the id field contains only one character string without spaces,
 have the label fill in both the id and the name/description
 - If the id field contains multiple character strings with spaces:
 - Use the first character string for the shorthand_id but exclude any final character that is
 NOT an alphanumeric OR an underscore ‘_’
 - Use all subsequent text for the name/description
 *
 We may encounter label tables in the future that don’t behave well following these rules, but we can cross that bridge later.
 */
void
MetaDataCustomEditorWidget::processLabelForIdDescription(const AString& labelText,
                                                         AString& idOut,
                                                         AString& descriptionOut) const
{
    idOut          = "";
    descriptionOut = "";
    
    const AString text(labelText.trimmed());
    if ( ! labelText.isEmpty()) {
        const int32_t firstSpaceIndex(text.indexOf(" "));
        if (firstSpaceIndex > 0) {
            idOut = text.left(firstSpaceIndex);
            descriptionOut = text.mid(firstSpaceIndex + 1);
            
            const int32_t lastCharIndex(descriptionOut.length() - 1);
            if (lastCharIndex > 0) {
                const QChar lastChar(descriptionOut[lastCharIndex]);
                if (lastChar.isDigit()
                    || lastChar.isLetter()
                    || (lastChar == '_')) {
                    /* ok */
                }
                else {
                    descriptionOut.resize(lastCharIndex);
                }
            }
        }
        else {
            /*
             * No spaces so both id and description are the same
             */
            idOut = text;
            descriptionOut = text;
        }
    }
}

/**
 * Constructor.
 * @param editorWidget
 *    Editor widget that contains this metadata row
 * @param gridLayout
 *    The grid layout for widgets
 * @param gridLayoutRow
 *    Row in the grid layout
 * @param gridLayoutNameColumn
 *    Column for the name
 * @param gridLayoutValueColumn
 *    Column for the value
 * @param gridLayoutButtonColumn
 *    Column for button
 * @param metaDataName
 *   The  name of the metadata
 * @param metaData
 *   The metadata.
 * @param requiredMetaDataFlag
 *    True if meta data name is required
 */
MetaDataCustomEditorWidget::MetaDataWidgetRow::MetaDataWidgetRow(MetaDataCustomEditorWidget* editorWidget,
                                                                 QGridLayout* gridLayout,
                                                                 const int32_t gridLayoutRow,
                                                                 const int32_t gridLayoutNameColumn,
                                                                 const int32_t gridLayoutValueColumn,
                                                                 const int32_t gridLayoutButtonColumn,
                                                                 const AString& metaDataName,
                                                                 GiftiMetaData* metaData,
                                                                 const bool requiredMetaDataFlag)
: m_editorWidget(editorWidget),
m_metaDataName(metaDataName),
m_metaData(metaData)
{
    CaretAssert(m_metaData);
    
    m_compositeMetaDataItemFlag = m_metaData->isCompositeMetaDataName(metaDataName);
    
    const QString tooltip(m_metaData->getToolTipForMetaDataName(metaDataName));
    const QString value(metaData->get(metaDataName));
    
    QLabel* nameLabel(new QLabel(metaDataName
                                 + (requiredMetaDataFlag ? "*" : "")
                                 + ":"));
    nameLabel->setToolTip(tooltip);
    
    m_valueComboBox = NULL;
    m_valueDateEdit = NULL;
    m_valueLineEdit = NULL;
    
    m_toolButton = NULL;
    
    const bool useComboBoxForListsFlag(true);
    bool useComboBoxFlag(false);
    bool useDateEditFlag(false);
    bool useLineEditFlag(false);
    bool useToolButtonFlag(false);
    /* Need to use the user's metadata for data types */
    switch (metaData->getDataTypeForMetaDataName(metaDataName)) {
        case GiftiMetaDataElementDataTypeEnum::COMMENT:
            CaretAssert(0); /* Should never get here */
            break;
        case GiftiMetaDataElementDataTypeEnum::DATE:
            useDateEditFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::DING_ONTOLOGY_TERM:
            useLineEditFlag   = true;
            useToolButtonFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LABEL_FILE_AND_MAP:
            useLineEditFlag   = true;
            useToolButtonFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LABEL_ID_NAME:
            useLineEditFlag   = true;
            useToolButtonFlag = true;
            break;
        case GiftiMetaDataElementDataTypeEnum::LIST:
            if (useComboBoxForListsFlag) {
                useComboBoxFlag = true;
            }
            else {
                useLineEditFlag   = true;
                useToolButtonFlag = true;
            }
            break;
        case GiftiMetaDataElementDataTypeEnum::TEXT:
            useLineEditFlag   = true;
            break;
    }
    
    if (useComboBoxFlag) {
        const QStringList comboBoxValuesList(m_metaData->getValidValuesListForMetaDataName(metaDataName));
        m_valueComboBox = new QComboBox();
        m_valueComboBox->addItem(""); /* empty item for 'no value' */
        m_valueComboBox->addItems(comboBoxValuesList);
        m_valueComboBox->setToolTip(tooltip);
        QObject::connect(m_valueComboBox, &QComboBox::currentIndexChanged,
                         [=]() { saveAfterDataChangedInGUI(); });
    }
    if (useDateEditFlag) {
        m_valueDateEdit = new QDateEdit();
        m_valueDateEdit->setDisplayFormat(AnnotationMetaDataNames::SAMPLES_QT_DATE_FORMAT);
        m_valueDateEdit->setCalendarPopup(true);
        m_valueDateEdit->setToolTip(tooltip);
        QObject::connect(m_valueDateEdit, &QDateEdit::dateChanged,
                         [=]() { saveAfterDataChangedInGUI(); });
    }
    if (useLineEditFlag) {
        m_valueLineEdit = new QLineEdit();
        m_valueLineEdit->setToolTip(tooltip);
        if (m_compositeMetaDataItemFlag) {
            /*
             * Do not allow editing of composite elements
             */
            m_valueLineEdit->setReadOnly(true);
        }
        else {
            QObject::connect(m_valueLineEdit, &QLineEdit::editingFinished,
                             [=]() { saveAfterDataChangedInGUI(); } );
        }
    }
    if (useToolButtonFlag) {
        m_toolButton = new QToolButton();
        m_toolButton->setText("Choose...");
        QObject::connect(m_toolButton, &QToolButton::clicked,
                         [=]() { toolButtonClicked(); });
        m_toolButton->setToolTip(tooltip);
    }
    
    gridLayout->addWidget(nameLabel,
                          gridLayoutRow, gridLayoutNameColumn);
    if (m_valueComboBox != NULL) {
        gridLayout->addWidget(m_valueComboBox,
                              gridLayoutRow, gridLayoutValueColumn);
    }
    if (m_valueDateEdit != NULL) {
        gridLayout->addWidget(m_valueDateEdit,
                              gridLayoutRow, gridLayoutValueColumn);
    }
    if (m_valueLineEdit != NULL) {
        gridLayout->addWidget(m_valueLineEdit,
                              gridLayoutRow, gridLayoutValueColumn);
    }
    if (m_toolButton != NULL) {
        gridLayout->addWidget(m_toolButton,
                              gridLayoutRow, gridLayoutButtonColumn);
    }
}

/**
 * Destructor
 */
MetaDataCustomEditorWidget::MetaDataWidgetRow::~MetaDataWidgetRow()
{
}

/**
 * Update the value widget with the value from the metadata
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::updateValueWidget()
{
    CaretAssert(m_metaData);
    const QString value(m_metaData->get(m_metaDataName).trimmed());
    if (m_valueComboBox != NULL) {
        int32_t itemIndex(m_valueComboBox->findText(value));
        if (itemIndex < 0) {
            itemIndex = 0;
        }
        CaretAssert(itemIndex >= 0);
        if (itemIndex < m_valueComboBox->count()) {
            m_valueComboBox->setCurrentIndex(itemIndex);
        }
    }
    if (m_valueDateEdit != NULL) {
        m_valueDateEdit->setDate(QDate::fromString(value,
                                                   AnnotationMetaDataNames::SAMPLES_QT_DATE_FORMAT));
    }
    if (m_valueLineEdit != NULL) {
        m_valueLineEdit->setText(value);
    }
}

/**
 * Update the value widget with the value from the metadata
 * IF the row's element is a composite metadata item
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::updateCompositeMetaDataValueWidget()
{
    if (m_compositeMetaDataItemFlag) {
        updateValueWidget();
    }
}

/**
 * Called when the tool button is clicked
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::toolButtonClicked()
{
    CaretAssert(m_editorWidget);
    m_editorWidget->metaDataButtonClicked(m_metaDataName,
                                          m_toolButton);
    saveAfterDataChangedInGUI();
}

/**
 * @return value from gui as text
 */
QString
MetaDataCustomEditorWidget::MetaDataWidgetRow::getAsText() const
{
    QString text;
    if (m_valueComboBox != NULL) {
        text = m_valueComboBox->currentText().trimmed();
    }
    else if (m_valueDateEdit != NULL) {
        const QDate date(m_valueDateEdit->date());
        text = date.toString(AnnotationMetaDataNames::SAMPLES_QT_DATE_FORMAT);
    }
    else if (m_valueLineEdit != NULL) {
        text = m_valueLineEdit->text().trimmed();
    }
    else {
        CaretAssertMessage(0, "PROGRAM ERROR: Has new widget type been added?");
    }
    return text;
}

/**
 * Save the value to the metadata
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::saveToMetaData()
{
    /*
     * Only save elements that are not composite elements
     */
    if ( ! m_metaData->isCompositeMetaDataName(m_metaDataName)) {
        m_metaData->set(m_metaDataName,
                        getAsText());
    }
}

/**
 * Save the value to the metadata
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::saveAfterDataChangedInGUI()
{
    if (m_savingEnabled) {
        saveToMetaData();
        m_editorWidget->calledByMetaDataWidgetRowWhenValueChanges(m_metaDataName);
    }
}

/**
 * Set saving enabled when item changed by user
 */
void
MetaDataCustomEditorWidget::MetaDataWidgetRow::setSavingEnabled(const bool enabled)
{
    m_savingEnabled = enabled;
}


