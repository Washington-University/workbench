
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ANNOTATION_SAMPLES_META_DATA_DIALOG_DECLARE__
#include "AnnotationSamplesMetaDataDialog.h"
#undef __ANNOTATION_SAMPLES_META_DATA_DIALOG_DECLARE__

#include <QAction>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>

#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationSampleMetaData.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "ChooseBorderFocusFromFileDialog.h"
#include "DingOntologyTermsDialog.h"
#include "DisplayPropertiesSamples.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiMetaData.h"
#include "GuiManager.h"
#include "HemisphereEnum.h"
#include "LabelSelectionDialog.h"
#include "SamplesFile.h"
#include "SamplesMetaDataManager.h"
#include "WuQMessageBoxTwo.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSamplesMetaDataDialog 
 * \brief Dialog for creating new sample (polyhedron) or editing metadata
 * \ingroup GuiQt
 */

/**
 * Delete all static members to eliminate reported memory leaks.
 */
void
AnnotationSamplesMetaDataDialog::deleteStaticMembers()
{
    s_previousCreateMetaData.reset();
    s_previousCreateSampleMetaData.reset();
}

/**
 * Dialog constructor when creating a new polyhedron.
 * This dialog will add the annotation to the file if OK is clicked or destroy the annotation if
 * Cancel is clicked.
 *
 * @param userInputMode
 *     The current user input mode
 * @param browserWindowIndex
 *     Index of browser window
 * @param browserTabIndex
 *     Index of tab
 * @param annotationFile
 *     The annotation file.
 * @param polyhedron
 *     The new polyhedron.
 * @param viewportHeight
 *      Height of viewport
 * @param volumeSliceThickness
 *     Thickness of volume slice
 * @param parent
 *     Optional parent for this dialog.
 */
AnnotationSamplesMetaDataDialog::AnnotationSamplesMetaDataDialog(const UserInputModeEnum::Enum userInputMode,
                                                                 const int32_t browserWindowIndex,
                                                                 const int32_t browserTabIndex,
                                                                 AnnotationFile* annotationFile,
                                                                 AnnotationPolyhedron* polyhedron,
                                                                 const int32_t viewportHeight,
                                                                 const float volumeSliceThickness,
                                                                 QWidget* parent)
: WuQDialogModal("New Sample",
                 parent),
m_dialogMode(DialogMode::CREATE_NEW_SAMPLE),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex),
m_browserTabIndex(browserTabIndex),
m_annotationFile(annotationFile),
m_polyhedron(polyhedron),
m_viewportHeight(viewportHeight),
m_volumeSliceThickness(volumeSliceThickness)
{
    CaretAssert(m_polyhedron);

    if (s_previousCreateSampleMetaData) {
        AnnotationSampleMetaData* polyMetaData(m_polyhedron->getSampleMetaData());
        CaretAssert(polyMetaData);
        polyMetaData->copyMetaDataForNewAnnotation(*s_previousCreateSampleMetaData);
    }
        
    /*
     * Directly edit polyhedron's metadata since creating a new polyhedron
     */
    m_sampleMetaData = m_polyhedron->getSampleMetaData();
    CaretAssert(m_sampleMetaData);
    m_sampleMetaData->updateMetaDataWithNameChanges();
    
    m_sampleMetaData->setActualSampleEditDate(QDate::currentDate().toString(AnnotationSampleMetaData::getDateFormat()));
    
    createDialog();

    createSampleNumber();
}

/**
 * Dialog constructor when editing metadata in an existing polyhedron.
 *
 * @param polyhedron
 *     The polyhedron with metadata being edited
 * @param parent
 *     Optional parent for this dialog.
 */
AnnotationSamplesMetaDataDialog::AnnotationSamplesMetaDataDialog(AnnotationPolyhedron* polyhedron,
                                                                 QWidget* parent)
: WuQDialogModal("Edit Metadata",
                 parent),
m_dialogMode(DialogMode::EDIT_EXISTING_SAMPLE),
m_userInputMode(UserInputModeEnum::Enum::INVALID),
m_browserWindowIndex(-1),
m_browserTabIndex(-1),
m_annotationFile(NULL),
m_polyhedron(polyhedron),
m_viewportHeight(-1),
m_volumeSliceThickness(0.0)
{
    CaretAssert(m_polyhedron);
    CaretAssert(m_polyhedron->getSampleMetaData());
    
    /*
     * Make a COPY of the annotation's metadata so that user edit's a copy of the metadata.
     * This allows user to cancel editing without any changes to the metadata.
     */
    m_editModeMetaDataCopy.reset(new GiftiMetaData());
    m_editModeSampleMetaDataCopy.reset(new AnnotationSampleMetaData(m_editModeMetaDataCopy.get()));
    m_editModeSampleMetaDataCopy->copyMetaData(*m_polyhedron->getSampleMetaData());
    m_editModeSampleMetaDataCopy->updateMetaDataWithNameChanges();
    m_editModeMetaDataCopy->clearModified();

    /*
     * Edit's copy of metadata
     */
    m_sampleMetaData = m_editModeSampleMetaDataCopy.get();
    
    createDialog();
}

/**
 * Destructor.
 */
AnnotationSamplesMetaDataDialog::~AnnotationSamplesMetaDataDialog()
{
}

/**
 * Create the dialog
 */
void
AnnotationSamplesMetaDataDialog::createDialog()
{
    QTabWidget* tabWidget(new QTabWidget());
    tabWidget->addTab(createPrimaryTabWidget(), "Primary");
    tabWidget->addTab(createBorderFociTabWidget(), "Border / Foci");
    tabWidget->addTab(createSpecimenPortalTabWidget(), "Specimen Portal");
    tabWidget->addTab(createAdvancedTabWidget(), "Advanced");
    
    setSizePolicy(tabWidget->sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
    
    m_requireMetaDataCheckBox = new QCheckBox("Require Metadata (* indicates required metadata elements)");
    m_requireMetaDataCheckBox->setChecked(s_previousRequireMetaDataCheckedFlag);
    
    QWidget* dialogWidget(new QWidget());
    QVBoxLayout* dialogLayout(new QVBoxLayout(dialogWidget));
    dialogLayout->addWidget(tabWidget);
    dialogLayout->addWidget(m_requireMetaDataCheckBox, 0, Qt::AlignLeft);
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    loadMetaDataIntoDialog();
}

/**
 * @return Annotation that was created by dialog (NULL if annotation NOT created).
 */
Annotation*
AnnotationSamplesMetaDataDialog::getAnnotationThatWasCreated()
{
    return m_polyhedron;
}

/**
 * Add a label and widget to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the widget
 * @param gridLayout
 *    Grid layout label and widget are added to
 * @param widget
 *    Widget that is added to the grid layout and has tooltip set
 */
void
AnnotationSamplesMetaDataDialog::addWidget(const AString& labelText,
                                           const AString& toolTip,
                                           QGridLayout* gridLayout,
                                           QWidget* widget)
{
    QLabel*    label(new QLabel(labelText + ": "));
    widget->setToolTip(toolTip);
    const int row(gridLayout->rowCount());
    gridLayout->addWidget(label, row, 0);
    gridLayout->addWidget(widget, row, 1);
    
}

/**
 * Add a label and line edit to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the line edit
 * @param gridLayout
 *    Grid layout label and line edit are added to
 * @return
 *    Pointer to the line edit that was created
 */
QLineEdit*
AnnotationSamplesMetaDataDialog::addLineEdit(const AString& labelText,
                                             const AString& toolTip,
                                             QGridLayout* gridLayout)
{
    QLineEdit* lineEdit(new QLineEdit());
    addWidget(labelText,
              toolTip,
              gridLayout,
              lineEdit);
    return lineEdit;
}

/**
 * Add a label and line edit to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the line edit
 * @param chooseButton
 *    Enum identifying metadata for choose button
 * @param gridLayout
 *    Grid layout label and line edit are added to
 * @return
 *    Pointer to the line edit that was created
 */
QLineEdit*
AnnotationSamplesMetaDataDialog::addLineEdit(const AString& labelText,
                                             const AString& toolTip,
                                             const ChooseButtonEnum chooseButton,
                                             QGridLayout* gridLayout)
{
    const int32_t row(gridLayout->rowCount());
    QLineEdit* lineEdit = addLineEdit(labelText,
                                      toolTip,
                                      gridLayout);
    QAction* action(new QAction("Choose..."));
    QObject::connect(action, &QAction::triggered,
                     this, [=] { chooseButtonClicked(chooseButton); });
    QToolButton* toolButton(new QToolButton());
    toolButton->setDefaultAction(action);
    gridLayout->addWidget(toolButton, row, 2);
    return lineEdit;
}

/**
 * Add a label and line edit, and toolbutton connected to the given slot to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the line edit
 * @param slot
 *    Slot connected to toolbutton
 * @param gridLayout
 *    Grid layout label and line edit are added to
 * @return
 *    Pointer to the line edit that was created
 */
template <typename PointerToMemberFunction> QLineEdit*
AnnotationSamplesMetaDataDialog::addLineEdit(const AString& labelText,
                                             const AString& toolTip,
                                             PointerToMemberFunction slot,
                                             QGridLayout* gridLayout)
{
    const int32_t row(gridLayout->rowCount());
    QLineEdit* lineEdit = addLineEdit(labelText,
                toolTip,
                gridLayout);
    QAction* action(new QAction("Choose..."));
    QObject::connect(action, &QAction::triggered,
                     this, slot);
    QToolButton* toolButton(new QToolButton());
    toolButton->setDefaultAction(action);
    gridLayout->addWidget(toolButton, row, 2);
    return lineEdit;
}

/**
 * Add a combo box and line edit to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the combo box
 * @param comboBoxTextSelections
 *    Text selections added to combo box
 * @param gridLayout
 *    Grid layout label and combo box are added to
 * @return
 *    Pointer to the combo box that was created
 */
QComboBox*
AnnotationSamplesMetaDataDialog::addComboBox(const AString& labelText,
                                             const AString& toolTip,
                                             const std::vector<AString>& comboBoxTextSelections,
                                             QGridLayout* gridLayout)
{
    QComboBox* comboBox(new QComboBox());
    for (const AString& text : comboBoxTextSelections) {
        comboBox->addItem(text);
    }
    addWidget(labelText,
              toolTip,
              gridLayout,
              comboBox);
    return comboBox;
}

/**
 * Add a label and date edit to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the date edit
 * @param gridLayout
 *    Grid layout label and date edit are added to
 * @return
 *    Pointer to the date edit that was created
 */
QDateEdit* 
AnnotationSamplesMetaDataDialog::addDateEdit(const AString& labelText,
                                             const AString& toolTip,
                                             QGridLayout* gridLayout)
{
    QDateEdit* dateEdit(new QDateEdit());
    dateEdit->setDisplayFormat(AnnotationSampleMetaData::getDateFormat());
    dateEdit->setCalendarPopup(true);
    dateEdit->setToolTip(toolTip);
    addWidget(labelText,
              toolTip,
              gridLayout,
              dateEdit);
    return dateEdit;
}

/**
 * Add a label and spin box to the grid layout
 * @param labelText
 *    Text for the label in column zero
 * @param toolTip
 *    Tooltip text for the date edit
 * @param minimumValue
 *    Minimum value for spin box
 * @param maximumValue
 *    Maximum value for spin box
 * @param gridLayout
 *    Grid layout label and date edit are added to
 * @return
 *    Pointer to the date edit that was created
 */
QSpinBox*
AnnotationSamplesMetaDataDialog::addSpinBox(const AString& labelText,
                                            const AString& toolTip,
                                            const int32_t minimumValue,
                                            const int32_t maximumValue,
                                            QGridLayout* gridLayout)
{
    QSpinBox* spinBox(new QSpinBox());
    spinBox->setRange(minimumValue, maximumValue);
    addWidget(labelText,
              toolTip,
              gridLayout,
              spinBox);
    return spinBox;
}


/**
 * @return Instance of the Primary tab widget
 */
QWidget*
AnnotationSamplesMetaDataDialog::createPrimaryTabWidget()
{
    QGridLayout* gridLayout(new QGridLayout());
    gridLayout->setContentsMargins(0, 0, 0, 0);

    m_subjectNameLineEdit = addLineEdit(AnnotationSampleMetaData::getSubjectNameLabelText(),
                                        "",
                                        gridLayout);
    QObject::connect(m_subjectNameLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setSubjectName(text); });
    
    m_allenLocalNameLineEdit = addLineEdit(AnnotationSampleMetaData::getAllenLocalNameLabelText() + "*",
                                           "Example: QM23.50.003",
                                           gridLayout);
    QObject::connect(m_allenLocalNameLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setAllenLocalName(text); loadLocalSlabID(); loadLocalSampleID(); });

    m_hemisphereComboBox = addComboBox(AnnotationSampleMetaData::getHemisphereLabelText() + "*",
                                       "",
                                       AnnotationSampleMetaData::getAllValidHemisphereValues(),
                                       gridLayout);
    QObject::connect(m_hemisphereComboBox, &QComboBox::currentTextChanged,
                     [=](const QString& text) { m_sampleMetaData->setHemisphere(text); });
    
    m_allenTissueTypeComboBox = addComboBox(AnnotationSampleMetaData::getAllenTissueTypeLabelText() + "*",
                                            "",
                                            AnnotationSampleMetaData::getAllValidAllenTissueTypeValues(),
                                            gridLayout);
    QObject::connect(m_allenTissueTypeComboBox, &QComboBox::currentTextChanged,
                     [=](const QString& text) { m_sampleMetaData->setAllenTissueType(text); loadLocalSlabID(); loadLocalSampleID(); });

    m_allenSlabNumberLineEdit = addLineEdit(AnnotationSampleMetaData::getAllenSlabNumberLabelText() + "*",
                                            "<html>CB: Left Hemisphere=00-39, <br>"
                                            "Right Hemisphere=40-60+",
                                            gridLayout);
    QObject::connect(m_allenSlabNumberLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { 
        m_sampleMetaData->setAllenSlabNumber(text);
        createSampleNumber();
        loadLocalSlabID();
        loadLocalSampleID(); });

    m_localSlabIdLineEdit = addLineEdit(AnnotationSampleMetaData::getLocalSlabIdLabelText(),
                                        "",
                                        gridLayout);
    m_localSlabIdLineEdit->setReadOnly(true);
    
    m_slabFaceComboBox = addComboBox(AnnotationSampleMetaData::getSlabFaceLabelText(),
                                     "",
                                     AnnotationSampleMetaData::getAllValidSlabFaceValues(),
                                     gridLayout);
    QObject::connect(m_slabFaceComboBox, &QComboBox::currentTextChanged,
                     [=](const QString& text) { m_sampleMetaData->setSlabFace(text); });

    m_sampleTypeComboBox = addComboBox(AnnotationSampleMetaData::getSampleTypeLabelText() + "*",
                                       "",
                                       AnnotationSampleMetaData::getAllValidSampleTypeValues(),
                                       gridLayout);
    QObject::connect(m_sampleTypeComboBox, &QComboBox::currentTextChanged,
                     [=](const QString& text) { m_sampleMetaData->setSampleType(text); });

    m_sampleLocationActualRadioButton  = new QRadioButton(AnnotationSampleMetaData::getSampleLocationText(AnnotationSampleMetaData::LocationEnum::ACTUAL));
    m_sampleLocationDesiredRadioButton = new QRadioButton(AnnotationSampleMetaData::getSampleLocationText(AnnotationSampleMetaData::LocationEnum::DESIRED));
    QButtonGroup* sampleLocationButtonGroup(new QButtonGroup(this));
    sampleLocationButtonGroup->addButton(m_sampleLocationActualRadioButton);
    sampleLocationButtonGroup->addButton(m_sampleLocationDesiredRadioButton);
    QObject::connect(sampleLocationButtonGroup, &QButtonGroup::buttonClicked,
                     [=](QAbstractButton* button) { 
        if (button == m_sampleLocationActualRadioButton) {
            m_sampleMetaData->setSampleLocation(AnnotationSampleMetaData::LocationEnum::ACTUAL);
        }
        else if (button == m_sampleLocationDesiredRadioButton) {
            m_sampleMetaData->setSampleLocation(AnnotationSampleMetaData::LocationEnum::DESIRED);
        }
    });
    QWidget* sampleLocationWidget(new QWidget());
    QHBoxLayout* sampleLocationLayout(new QHBoxLayout(sampleLocationWidget));
    sampleLocationLayout->setContentsMargins(0, 0, 0, 0);
    sampleLocationLayout->addWidget(m_sampleLocationActualRadioButton);
    sampleLocationLayout->addWidget(m_sampleLocationDesiredRadioButton);
    addWidget("Location", "", gridLayout, sampleLocationWidget);

    m_desiredSampleEntryDateEdit = addDateEdit(AnnotationSampleMetaData::getDesiredSampleEditDateLabelText(),
                                               "Format is dd/mm/yyyy",
                                               gridLayout);
    QObject::connect(m_desiredSampleEntryDateEdit, &QDateEdit::dateChanged,
                     [=](QDate date) {
        m_sampleMetaData->setDesiredSampleEditDate(date.toString(AnnotationSampleMetaData::getDateFormat()));
        std::cout << "Date" << std::endl;
    });
    
    m_actualSampleEntryDateEdit = addDateEdit(AnnotationSampleMetaData::getActualSampleEditDateLabelText(),
                                              "Format is dd/mm/yyyy",
                                              gridLayout);
    QObject::connect(m_actualSampleEntryDateEdit, &QDateEdit::dateChanged,
                     [=](QDate date) {
        m_sampleMetaData->setActualSampleEditDate(date.toString(AnnotationSampleMetaData::getDateFormat()));
        std::cout << "Date2" << std::endl;
    });

    
    m_hmbaParcelDingAbbreviationLineEdit = addLineEdit(AnnotationSampleMetaData::getHmbaParcelDingAbbreviationLabelText(),
                                                       "",
                                                       ChooseButtonEnum::DING_ABBREVIATION,
                                                       gridLayout);
    QObject::connect(m_hmbaParcelDingAbbreviationLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setHmbaParcelDingAbbreviation(text); });

    m_hmbaParcelDingFullNameLineEdit = addLineEdit(AnnotationSampleMetaData::getHmbaParcelDingFullNameLabelText(),
                                                   "",
                                                   gridLayout);
    QObject::connect(m_hmbaParcelDingFullNameLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setHmbaParcelDingFullName(text); });

    m_sampleNameLineEdit = addLineEdit(AnnotationSampleMetaData::getSampleNameLabelText(),
                                       "",
                                       gridLayout);
    QObject::connect(m_sampleNameLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setSampleName(text); });

    m_alternativeSampleNameLineEdit = addLineEdit(AnnotationSampleMetaData::getAlternativeSampleNameLabelText(),
                                                  "",
                                                  gridLayout);
    QObject::connect(m_alternativeSampleNameLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setAlternateSampleName(text); });

    m_sampleNumberLineEdit = addLineEdit(AnnotationSampleMetaData::getSampleNumberLabelText() + "*",
                                         "",
                                         gridLayout);
    QObject::connect(m_sampleNumberLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setSampleNumber(text); loadLocalSampleID(); });

    m_localSampleIdLineEdit = addLineEdit(AnnotationSampleMetaData::getLocalSampleIdLabelText(),
                                          "",
                                          gridLayout);
    m_localSampleIdLineEdit->setReadOnly(true);
    
    m_primaryParcellationLineEdit = addLineEdit(AnnotationSampleMetaData::getPrimaryParcellationLabelText(),
                                                "",
                                                ChooseButtonEnum::PRIMARY_PARCELLATION,
                                                gridLayout);
    QObject::connect(m_primaryParcellationLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setPrimaryParcellation(text); });

    m_alternativeParcellationLineEdit = addLineEdit(AnnotationSampleMetaData::getAlternativeParcellationLabelText(),
                                                    "",
                                                    ChooseButtonEnum::ALTERNATE_PARCELLATION,
                                                    gridLayout);
    QObject::connect(m_alternativeParcellationLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setAlternateParcellation(text); });

    m_commentTextEdit = new QTextEdit();
    addWidget(AnnotationSampleMetaData::getCommentLabelText(),
              "",
              gridLayout,
              m_commentTextEdit);
    QObject::connect(m_commentTextEdit, &QTextEdit::textChanged,
                     [=]() { m_sampleMetaData->setComment(m_commentTextEdit->toPlainText()); });
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * @return Instance of the Border / Foci  tab widget
 */
QWidget*
AnnotationSamplesMetaDataDialog::createBorderFociTabWidget()
{
    QLabel* borderFileLabel(new QLabel(AnnotationSampleMetaData::getBorderFileNameLabelText() + ":"));
    QLabel* borderClassLabel(new QLabel(AnnotationSampleMetaData::getBorderClassLabelText() + ":"));
    QLabel* borderNameLabel(new QLabel(AnnotationSampleMetaData::getBorderNameLabelText() + ":"));

    m_borderFileNameLineEdit = new QLineEdit();
    m_borderFileNameLineEdit->setReadOnly(true);
    
    m_borderClassLineEdit = new QLineEdit();
    m_borderClassLineEdit->setReadOnly(true);
    
    m_borderNameLineEdit = new QLineEdit();
    m_borderNameLineEdit->setReadOnly(true);
    
    QAction* selectBorderAction(new QAction("Choose..."));
    QObject::connect(selectBorderAction, &QAction::triggered,
                     this, [=] { chooseButtonClicked(ChooseButtonEnum::BORDER); });
    QToolButton* selectBorderToolButton(new QToolButton());
    selectBorderToolButton->setDefaultAction(selectBorderAction);
    
    QLabel* focusFileLabel(new QLabel(AnnotationSampleMetaData::getFocusFileNameLabelText() + ":"));
    QLabel* focusClassLabel(new QLabel(AnnotationSampleMetaData::getFocusClassLabelText() + ":"));
    QLabel* focusNameLabel(new QLabel(AnnotationSampleMetaData::getFocusNameLabelText() + ":"));
    
    m_focusFileNameLineEdit = new QLineEdit();
    m_focusFileNameLineEdit->setReadOnly(true);
    
    m_focusClassLineEdit = new QLineEdit();
    m_focusClassLineEdit->setReadOnly(true);
    
    m_focusNameLineEdit = new QLineEdit();
    m_focusNameLineEdit->setReadOnly(true);
    
    QAction* selectFocusAction(new QAction("Choose..."));
    QObject::connect(selectFocusAction, &QAction::triggered,
                     this, [=] { chooseButtonClicked(ChooseButtonEnum::FOCUS); });
    QToolButton* selectFocusToolButton(new QToolButton());
    selectFocusToolButton->setDefaultAction(selectFocusAction);
    

    QGridLayout* gridLayout(new QGridLayout());
    gridLayout->setContentsMargins(0, 0, 0, 0);
    int row(gridLayout->rowCount());
    gridLayout->addWidget(borderFileLabel, row, 0);
    gridLayout->addWidget(m_borderFileNameLineEdit, row, 1, 1, 3);
    row = gridLayout->rowCount();
    gridLayout->addWidget(borderNameLabel, row, 0);
    gridLayout->addWidget(m_borderNameLineEdit, row, 1);
    gridLayout->addWidget(borderClassLabel, row, 2);
    gridLayout->addWidget(m_borderClassLineEdit, row, 3);
    gridLayout->addWidget(selectBorderToolButton, row - 1, 4, 2, 1);
    
    row = gridLayout->rowCount();
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 4);
    
    row = gridLayout->rowCount();
    gridLayout->addWidget(focusFileLabel, row, 0);
    gridLayout->addWidget(m_focusFileNameLineEdit, row, 1, 1, 3);
    row = gridLayout->rowCount();
    gridLayout->addWidget(focusNameLabel, row, 0);
    gridLayout->addWidget(m_focusNameLineEdit, row, 1);
    gridLayout->addWidget(focusClassLabel, row, 2);
    gridLayout->addWidget(m_focusClassLineEdit, row, 3);
    gridLayout->addWidget(selectFocusToolButton, row - 1, 4, 2, 1);

    gridLayout->setColumnStretch(gridLayout->columnCount(), 100); /* Push to left */

    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * @return Instance of the Specimen Portal tab widget
 */
QWidget*
AnnotationSamplesMetaDataDialog::createSpecimenPortalTabWidget()
{
    QGridLayout* gridLayout(new QGridLayout());
    gridLayout->setContentsMargins(0, 0, 0, 0);

    m_bicanDonorIdLineEdit = addLineEdit(AnnotationSampleMetaData::getBicanDonorIdLabelText(),
                                         "Example: DO-TWUW4075",
                                         gridLayout);
    QObject::connect(m_bicanDonorIdLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setBicanDonorID(text); });

    m_nhashSlabIdLineEdit = addLineEdit(AnnotationSampleMetaData::getNhashSlabIdLabelText(),
                                        "",
                                        gridLayout);
    QObject::connect(m_nhashSlabIdLineEdit, &QLineEdit::textEdited,
                     [=](const QString& text) { m_sampleMetaData->setNHashSlabID(text); });

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * @return Instance of the Advanced tab widget
 */
QWidget*
AnnotationSamplesMetaDataDialog::createAdvancedTabWidget()
{
    QGridLayout* gridLayout(new QGridLayout());

    m_firstLeftHemisphereAllenSlabNumberSpinBox = addSpinBox("First Left Hemisphere Allen Slab Number",
                                                             "",
                                                             1, 1000,
                                                             gridLayout);
    m_firstLeftHemisphereAllenSlabNumberSpinBox->setValue(AnnotationSampleMetaData::getFirstLeftHemisphereAllenSlabNumber());
    QObject::connect(m_firstLeftHemisphereAllenSlabNumberSpinBox,  QOverload<int>::of(&QSpinBox::valueChanged),
                     [=](int value) { AnnotationSampleMetaData::setFirstLeftHemisphereAllenSlabNumber(value); });
    
    m_firstRightHemisphereAllenSlabNumberSpinBox = addSpinBox("First Right Hemisphere Allen Slab Number",
                                                              "",
                                                              1, 1000,
                                                              gridLayout);
    m_firstRightHemisphereAllenSlabNumberSpinBox->setValue(AnnotationSampleMetaData::getFirstRightHemisphereAllenSlabNumber());
    QObject::connect(m_firstRightHemisphereAllenSlabNumberSpinBox,  QOverload<int>::of(&QSpinBox::valueChanged),
                     [=](int value) { AnnotationSampleMetaData::setFirstRightHemisphereAllenSlabNumber(value); });

    m_lastRightHemisphereAllenSlabNumberSpinBox = addSpinBox("Last Right Hemisphere Allen Slab Number",
                                                             "",
                                                             1, 1000,
                                                             gridLayout);
    m_lastRightHemisphereAllenSlabNumberSpinBox->setValue(AnnotationSampleMetaData::getLastRightHemisphereAllenSlabNumber());
    QObject::connect(m_lastRightHemisphereAllenSlabNumberSpinBox,  QOverload<int>::of(&QSpinBox::valueChanged),
                     [=](int value) { AnnotationSampleMetaData::setLastRightHemisphereAllenSlabNumber(value); });

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * Set the combo box to the given text
 * @param comboBox
 *    The combo box
 * @param text
 *    Text that must match an item in the combo box
 */
void
AnnotationSamplesMetaDataDialog::setComboBoxSelection(QComboBox* comboBox,
                                                      const AString& text)
{
    int32_t index(0);
    const int32_t numItems(comboBox->count());
    for (int32_t i = 0; i < numItems; i++) {
        if (comboBox->itemText(i).toLower() == text.toLower()) {
            index = i;
            break;
        }
    }
    if ((index >= 0)
        && (index < numItems)) {
        comboBox->setCurrentIndex(index);
    }
}

/**
 * Set the date edit widget to date in the given text
 * @param dateEdit
 *    The date edit
 * @param text
 *    Text containing date
 */
void
AnnotationSamplesMetaDataDialog::setDateEditSelection(QDateEdit* dateEdit,
                                                      const AString& text)
{
    dateEdit->setDate(QDate::fromString(text,
                                        AnnotationSampleMetaData::getDateFormat()));
}

/**
 * @return The date a text from the QDateEdit
 * @param dateEdit
 *    The date edit
 */
AString
AnnotationSamplesMetaDataDialog::getDateAsText(QDateEdit* dateEdit)
{
    const QDate date(dateEdit->date());
    const AString text = date.toString(AnnotationSampleMetaData::getDateFormat());
    return text;
}


/**
 * Load metadata from polyhedron into dialog
 */
void
AnnotationSamplesMetaDataDialog::loadMetaDataIntoDialog()
{
    CaretAssert(m_sampleMetaData);
    
    /*
     * Note QSignalBlockers are needed on some items as the
     * signal being used issue both then the user modifies the
     * item and when the item is modified programatically.
     */
    m_subjectNameLineEdit->setText(m_sampleMetaData->getSubjectName());
    m_allenLocalNameLineEdit->setText(m_sampleMetaData->getAllenLocalName());
    QSignalBlocker hemisphereBlocker(m_hemisphereComboBox);
    setComboBoxSelection(m_hemisphereComboBox,
                         m_sampleMetaData->getHemisphere());
    QSignalBlocker allTissueTypeBlocker(m_allenTissueTypeComboBox);
    setComboBoxSelection(m_allenTissueTypeComboBox,
                         m_sampleMetaData->getAllenTissueType());
    m_allenSlabNumberLineEdit->setText(m_sampleMetaData->getAllenSlabNumber());
    loadLocalSlabID();
    QSignalBlocker slabFaceBlocker(m_slabFaceComboBox);
    setComboBoxSelection(m_slabFaceComboBox, m_sampleMetaData->getSlabFace());
    QSignalBlocker sampleTypeBlocker(m_sampleTypeComboBox);
    setComboBoxSelection(m_sampleTypeComboBox, m_sampleMetaData->getSampleType());
    switch (m_sampleMetaData->getSampleLocation()) {
        case AnnotationSampleMetaData::LocationEnum::ACTUAL:
            m_sampleLocationActualRadioButton->setChecked(true);
            break;
        case AnnotationSampleMetaData::LocationEnum::DESIRED:
            m_sampleLocationDesiredRadioButton->setChecked(true);
            break;
        case AnnotationSampleMetaData::LocationEnum::UNKNOWN:
            break;
    }
    QSignalBlocker desiredDateBlocker(m_desiredSampleEntryDateEdit);
    setDateEditSelection(m_desiredSampleEntryDateEdit, m_sampleMetaData->getDesiredSampleEditDate());
    QSignalBlocker actualDateBlocker(m_actualSampleEntryDateEdit);
    setDateEditSelection(m_actualSampleEntryDateEdit, m_sampleMetaData->getActualSampleEditDate());
    m_hmbaParcelDingAbbreviationLineEdit->setText(m_sampleMetaData->getHmbaParcelDingAbbreviation());
    m_hmbaParcelDingFullNameLineEdit->setText(m_sampleMetaData->getHmbaParcelDingFullName());
    
    m_sampleNameLineEdit->setText(m_sampleMetaData->getSampleName());
    m_alternativeSampleNameLineEdit->setText(m_sampleMetaData->getAlternateSampleName());
    m_sampleNumberLineEdit->setText(m_sampleMetaData->getSampleNumber());
    m_localSampleIdLineEdit->setText(m_sampleMetaData->getLocalSampleID());
    m_primaryParcellationLineEdit->setText(m_sampleMetaData->getPrimaryParcellation());
    m_alternativeParcellationLineEdit->setText(m_sampleMetaData->getAlternateParcellation());
    
    QSignalBlocker commentBlocker(m_commentTextEdit);
    m_commentTextEdit->setText(m_sampleMetaData->getComment());
    
    
    /*
     * Border / Foci Tab
     */
    m_borderFileNameLineEdit->setText(m_sampleMetaData->getBorderFileName());
    m_borderClassLineEdit->setText(m_sampleMetaData->getBorderClass());
    m_borderNameLineEdit->setText(m_sampleMetaData->getBorderName());
    m_focusFileNameLineEdit->setText(m_sampleMetaData->getFocusFileName());
    m_focusClassLineEdit->setText(m_sampleMetaData->getFocusClass());
    m_focusNameLineEdit->setText(m_sampleMetaData->getFocusName());
    
    /*
     * Speciman Portal Tab
     */
    m_bicanDonorIdLineEdit->setText(m_sampleMetaData->getBicanDonorID());
    m_nhashSlabIdLineEdit->setText(m_sampleMetaData->getNHashSlabID());
}

/**
 * Load the local slab ID
 */
void
AnnotationSamplesMetaDataDialog::loadLocalSlabID()
{
    m_localSlabIdLineEdit->setText(m_sampleMetaData->getLocalSlabID());

}

/**
 * Load the local sample ID
 */
void
AnnotationSamplesMetaDataDialog::loadLocalSampleID()
{
    m_localSampleIdLineEdit->setText(m_sampleMetaData->getLocalSampleID());
}

/**
 * Read metadata from dialog into polyhedron
 */
void
AnnotationSamplesMetaDataDialog::readMetaDataFromDialog()
{
    CaretAssertToDoFatal();  /* No longer need this since when items change they update the metadata*/
    CaretAssert(m_sampleMetaData);
    
    m_sampleMetaData->setSubjectName(m_subjectNameLineEdit->text().trimmed());
    m_sampleMetaData->setAllenLocalName(m_allenLocalNameLineEdit->text().trimmed());
    m_sampleMetaData->setHemisphere(m_hemisphereComboBox->currentText());
    m_sampleMetaData->setAllenTissueType(m_allenTissueTypeComboBox->currentText());
    m_sampleMetaData->setAllenSlabNumber(m_allenSlabNumberLineEdit->text().trimmed());
    m_sampleMetaData->setSlabFace(m_slabFaceComboBox->currentText());
    m_sampleMetaData->setSampleType(m_sampleTypeComboBox->currentText());
    m_sampleMetaData->setDesiredSampleEditDate(getDateAsText(m_desiredSampleEntryDateEdit));
    m_sampleMetaData->setActualSampleEditDate(getDateAsText(m_actualSampleEntryDateEdit));
    m_sampleMetaData->setHmbaParcelDingAbbreviation(m_hmbaParcelDingAbbreviationLineEdit->text().trimmed());
    m_sampleMetaData->setHmbaParcelDingFullName(m_hmbaParcelDingFullNameLineEdit->text().trimmed());
    
    m_sampleMetaData->setSampleName(m_sampleNameLineEdit->text().trimmed());
    m_sampleMetaData->setAlternateSampleName(m_alternativeSampleNameLineEdit->text().trimmed());
    m_sampleMetaData->setSampleNumber(m_sampleNumberLineEdit->text().trimmed());
    m_sampleMetaData->setPrimaryParcellation(m_primaryParcellationLineEdit->text().trimmed());
    m_sampleMetaData->setAlternateParcellation(m_alternativeParcellationLineEdit->text().trimmed());
    
    m_sampleMetaData->setComment(m_commentTextEdit->toPlainText());
    
    m_sampleMetaData->setBorderFileName(m_borderFileNameLineEdit->text().trimmed());
    m_sampleMetaData->setBorderClass(m_borderClassLineEdit->text().trimmed());
    m_sampleMetaData->setBorderName(m_borderNameLineEdit->text().trimmed());
    m_sampleMetaData->setFocusFileName(m_focusFileNameLineEdit->text().trimmed());
    m_sampleMetaData->setFocusClass(m_focusClassLineEdit->text().trimmed());
    m_sampleMetaData->setFocusName(m_focusNameLineEdit->text().trimmed());

    /*
     * Speciman Portal Tab
     */
    m_sampleMetaData->setBicanDonorID(m_bicanDonorIdLineEdit->text().trimmed());
    m_sampleMetaData->setNHashSlabID(m_nhashSlabIdLineEdit->text().trimmed());
}

AString
AnnotationSamplesMetaDataDialog::getLabelFileAndMap()
{
    AString textOut;
    
    LabelSelectionDialog labelDialog(LabelSelectionDialog::Mode::FILE_AND_MAP,
                                     "AnnotationSamplesMetaDataDialog",
                                     this);
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
            textOut = text;
        }
    }
    
    return textOut;
}
/**
 * Called when a choose button is clicked
 * @param metaDataName
 *    Name of metadata for button
 */
void
AnnotationSamplesMetaDataDialog::chooseButtonClicked(const ChooseButtonEnum chooseButton)
{
    switch (chooseButton) {
        case ChooseButtonEnum::ALTERNATE_PARCELLATION:
            {
                const AString text(getLabelFileAndMap());
                if ( ! text.isEmpty()) {
                    m_sampleMetaData->setAlternateParcellation(text);
                    m_alternativeParcellationLineEdit->setText(text);
                }
            }
            break;
        case ChooseButtonEnum::BORDER:
        {
            ChooseBorderFocusFromFileDialog chooseDialog(ChooseBorderFocusFromFileDialog::FileMode::BORDER,
                                                         this);
            chooseDialog.setSelections(m_borderFileNameLineEdit->text(),
                                       m_borderClassLineEdit->text(),
                                       m_borderNameLineEdit->text());
            if (chooseDialog.exec() == ChooseBorderFocusFromFileDialog::Accepted) {
                m_sampleMetaData->setBorderFileName(chooseDialog.getSelectedFileName());
                m_sampleMetaData->setBorderClass(chooseDialog.getSelectedClass());
                m_sampleMetaData->setBorderName(chooseDialog.getSelectedName());
                
                m_borderFileNameLineEdit->setText(chooseDialog.getSelectedFileName());
                m_borderClassLineEdit->setText(chooseDialog.getSelectedClass());
                m_borderNameLineEdit->setText(chooseDialog.getSelectedName());
            }
        }
            break;
        case ChooseButtonEnum::DING_ABBREVIATION:
        {
            const SamplesMetaDataManager* smdm(GuiManager::get()->getBrain()->getSamplesMetaDataManager());
            CaretAssert(smdm);
            DingOntologyTermsDialog dotd(smdm->getDingOntologyTermsFile(),
                                         this);
            if (dotd.exec() == DingOntologyTermsDialog::Accepted) {
                const QString shorthandID(dotd.getAbbreviatedName());
                const QString description(dotd.getDescriptiveName());
                
                m_sampleMetaData->setHmbaParcelDingAbbreviation(shorthandID);
                m_sampleMetaData->setHmbaParcelDingFullName(description);
                
                m_hmbaParcelDingAbbreviationLineEdit->setText(shorthandID);
                m_hmbaParcelDingFullNameLineEdit->setText(description);
            }
        }
            break;
        case ChooseButtonEnum::FOCUS:
        {
            ChooseBorderFocusFromFileDialog chooseDialog(ChooseBorderFocusFromFileDialog::FileMode::FOCUS,
                                                         this);
            chooseDialog.setSelections(m_focusFileNameLineEdit->text(),
                                       m_focusClassLineEdit->text(),
                                       m_focusNameLineEdit->text());
            if (chooseDialog.exec() == ChooseBorderFocusFromFileDialog::Accepted) {
                m_sampleMetaData->setFocusFileName(chooseDialog.getSelectedFileName());
                m_sampleMetaData->setFocusClass(chooseDialog.getSelectedClass());
                m_sampleMetaData->setFocusName(chooseDialog.getSelectedName());
                
                m_focusFileNameLineEdit->setText(chooseDialog.getSelectedFileName());
                m_focusClassLineEdit->setText(chooseDialog.getSelectedClass());
                m_focusNameLineEdit->setText(chooseDialog.getSelectedName());
            }
        }
            break;
        case ChooseButtonEnum::PRIMARY_PARCELLATION:
        {
            const AString text(getLabelFileAndMap());
            if ( ! text.isEmpty()) {
                m_sampleMetaData->setPrimaryParcellation(text);
                m_primaryParcellationLineEdit->setText(text);
            }
        }
            break;
    }
}

/**
 * Gets called when the OK button is clicked.
 */
void
AnnotationSamplesMetaDataDialog::okButtonClicked()
{
    s_previousRequireMetaDataCheckedFlag = m_requireMetaDataCheckBox->isChecked();
    
    switch (m_dialogMode) {
        case DialogMode::CREATE_NEW_SAMPLE:
            if ( ! finishCreatingNewSample()) {
                return;
            }
            break;
        case DialogMode::EDIT_EXISTING_SAMPLE:
            if ( ! finishEditingExistingSample()) {
                return;
            }
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialogModal::okButtonClicked();
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialogModal::okButtonClicked();
}

/**
 * Save the new polyhedron's metadata for the next time this dialog
 * is launched in create mode
 */
void
AnnotationSamplesMetaDataDialog::saveCreatingAnnotationMetaDataForNextTime()
{
    if ( ! s_previousCreateMetaData) {
        /*
         * Create storage for metadata
         * 'sample metadata' wraps a 'gifti metadata' instance
         */
        s_previousCreateMetaData.reset(new GiftiMetaData());
        s_previousCreateSampleMetaData.reset(new AnnotationSampleMetaData(s_previousCreateMetaData.get()));
    }

    /*
     * Copy the metadata for next time
     */
    s_previousCreateSampleMetaData->copyMetaData(*m_polyhedron->getSampleMetaData());
}

/**
 * Finish creating a new sample
 * @return True if new sample is valid and dialog can be closed
 */
bool 
AnnotationSamplesMetaDataDialog::finishCreatingNewSample()
{
    /*
     * Save metadata for next time an annotation is created
     */
    saveCreatingAnnotationMetaDataForNextTime();
    
    AString metaDataErrorMessage;
    if (m_requireMetaDataCheckBox->isChecked()) {
        if ( ! m_polyhedron->getSampleMetaData()->validateMetaData(metaDataErrorMessage)) {
            WuQMessageBoxTwo::critical(this,
                                       "Invalid Metadata",
                                       metaDataErrorMessage);
            return false;
        }
    }
    
    m_polyhedron->resetPlaneOneTwoNameStereotaxicXYZ();
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    
    CaretAssert(m_polyhedron);
    m_polyhedron->setDrawingNewAnnotationStatus(false);
    
    /*
     * Add annotation to its file
     */
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeCreateAnnotation(m_annotationFile,
                                         m_polyhedron);
    
    AString errorMessage;
    if ( ! annotationManager->applyCommand(undoCommand,
                                           errorMessage)) {
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   errorMessage);
        return false;
    }
    
    annotationManager->selectAnnotationForEditing(m_browserWindowIndex,
                                                  AnnotationManager::SELECTION_MODE_SINGLE,
                                                  false,
                                                  m_polyhedron);
    
    DisplayPropertiesSamples* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSamples();
    dps->updateForNewSample(m_polyhedron);
    
    return true;
}

/**
 * Finish editing an existing sample
 * @return True if existing sample is valid and dialog can be closed
 */
bool
AnnotationSamplesMetaDataDialog::finishEditingExistingSample()
{
    AString metaDataErrorMessage;
    if ( ! m_sampleMetaData->validateMetaData(metaDataErrorMessage)) {
        WuQMessageBoxTwo::critical(this,
                                   "Invalid Metadata",
                                   metaDataErrorMessage);
        return false;
    }

    /*
     * Copy dialog's metadata to the polyhedron
     */
    m_polyhedron->getSampleMetaData()->copyMetaData(*m_sampleMetaData);
    
    return true;
}

/**
 * Called if user clicks the cancel button
 */
void
AnnotationSamplesMetaDataDialog::cancelButtonClicked()
{
    s_previousRequireMetaDataCheckedFlag = m_requireMetaDataCheckBox->isChecked();

    switch (m_dialogMode) {
        case DialogMode::CREATE_NEW_SAMPLE:
            if (WuQMessageBoxTwo::warning(this,
                                          "Create Polyhedron",
                                          "Cancel finishing and return to drawing?",
                                          ((int32_t)WuQMessageBoxTwo::StandardButton::Yes | (int32_t)WuQMessageBoxTwo::StandardButton::No),
                                          WuQMessageBoxTwo::StandardButton::No) == WuQMessageBoxTwo::StandardButton::No) {
                /* User wants to continue editing metadata*/
                return;
            }
            else {
                /*
                 * Saves any metadata the user has entered for next time
                 */
                saveCreatingAnnotationMetaDataForNextTime();
            }
            break;
        case DialogMode::EDIT_EXISTING_SAMPLE:
            CaretAssert(m_editModeMetaDataCopy);
            if (m_editModeMetaDataCopy->isModified()) {
                if (WuQMessageBoxTwo::warning(this,
                                              "Metadata",
                                              "Metadata is modified, discard changes?",
                                              ((int32_t)WuQMessageBoxTwo::StandardButton::Yes | (int32_t)WuQMessageBoxTwo::StandardButton::No),
                                              WuQMessageBoxTwo::StandardButton::No) == WuQMessageBoxTwo::StandardButton::No) {
                    return;
                }
            }
            break;
    }
    
    WuQDialogModal::cancelButtonClicked();
}

/**
 * If the slab number is changed and not empty AND the sample number is empty,
 * generate a sample number.
 */
void
AnnotationSamplesMetaDataDialog::createSampleNumber()
{
    switch (m_dialogMode) {
        case DialogMode::CREATE_NEW_SAMPLE:
            break;
        case DialogMode::EDIT_EXISTING_SAMPLE:
            return;
            break;
    }
    
    const AString slabNumber(m_sampleMetaData->getAllenSlabNumber());
    if ( ! slabNumber.isEmpty()) {
        AString sampleNumber(m_sampleMetaData->getSampleNumber());
        if (sampleNumber.isEmpty()) {
            std::vector<SamplesFile*> samplesFiles(GuiManager::get()->getBrain()->getAllSamplesFiles());
            sampleNumber = SamplesFile::generateSampleNumberFromSlabID(samplesFiles,
                                                                       slabNumber);
            m_sampleMetaData->setSampleNumber(sampleNumber);
            m_sampleNumberLineEdit->setText(sampleNumber);
            loadLocalSampleID();
        }
    }
}


