
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

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QToolButton>

#define __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
#include "BorderPropertiesEditorDialog.h"
#undef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

#include "Brain.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "GroupAndNameHierarchyModel.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "GiftiLabelTableSelectionComboBox.h"
#include "GuiManager.h"
#include "SurfaceFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderPropertiesEditorDialog 
 * \brief Dialog that queries user to finish drawing of a border.
 *
 * This dialog is displayed when the user finishes drawing a 
 * border.  It allows the user to select the border file, 
 * enter the border name, select the color, enter the class,
 * set the type of border (open/closed), and
 * possibly other attributes of the border.
 */

/**
 * Create a new instance of the border properties editor for finishing
 * a border using a drawing mode.
 *
 * @param border
 *    Border that was drawn.
 * @param parent
 *    Parent on which this dialog is shown.
 * @return
 *    Dialog that will finish the border.
 *    Users MUST DELETE the returned dialog.
 */
BorderPropertiesEditorDialog*
BorderPropertiesEditorDialog::newInstanceFinishBorder(Border* border,
                                                      SurfaceFile* surfaceFile,
                                                      QWidget* parent)
{
    CaretAssert(surfaceFile);
    BorderPropertiesEditorDialog* dialog =
    new BorderPropertiesEditorDialog("Finish Border",
                                     surfaceFile,
                                     BorderPropertiesEditorDialog::MODE_FINISH_DRAWING,
                                     NULL,
                                     border,
                                     parent);
    return dialog;
}

/**
 * Create a new instance of the border properties editor for editing
 * a border properties.
 *
 * @param editModeBorderFile
 *    Border file containing the border that is being edited.
 * @param border
 *    Border that is to be edited.
 * @param parent
 *    Parent on which this dialog is shown.
 * @return
 *    Dialog that will finish the border.
 *    Users MUST DELETE the returned dialog.
 */
BorderPropertiesEditorDialog*
BorderPropertiesEditorDialog::newInstanceEditBorder(BorderFile* editModeBorderFile,
                                                    Border* border,
                                                    QWidget* parent)
{
    BorderPropertiesEditorDialog* dialog =
    new BorderPropertiesEditorDialog("Edit Border Properties",
                                     NULL,
                                     BorderPropertiesEditorDialog::MODE_EDIT,
                                     editModeBorderFile,
                                     border,
                                     parent);
    return dialog;
}

/**
 * Constructor.
 */
BorderPropertiesEditorDialog::BorderPropertiesEditorDialog(const QString& title,
                                                           SurfaceFile* finishBorderSurfaceFile,
                                                           Mode mode,
                                                           BorderFile* editModeBorderFile,
                                                           Border* border,
                                                           QWidget* parent)
: WuQDialogModal(title,
                 parent)
{
    CaretAssert(border);
    m_finishBorderSurfaceFile = finishBorderSurfaceFile;
    m_editModeBorderFile = editModeBorderFile;
    m_mode   = mode;
    m_border = border;
    m_classComboBox = NULL;
    
    QString borderName = border->getName();
    QString className = border->getClassName();
    switch (m_mode) {
        case MODE_EDIT:
            break;
        case MODE_FINISH_DRAWING:
            if (s_previousDataValid) {
                borderName = s_previousName;
                className = s_previousClassName;
            }
            break;
    }
    
    /*
     * File selection combo box
     */
    QLabel* borderFileLabel = new QLabel("Border File");
    m_borderFileSelectionComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(m_borderFileSelectionComboBox, 
                                          "Selects an existing border file\n"
                                          "to which new borders are added.");
    QObject::connect(m_borderFileSelectionComboBox, SIGNAL(activated(int)),
                     this, SLOT(borderFileSelected()));
    QAction* newFileAction = WuQtUtilities::createAction("New",
                                                         "Create a new border file", 
                                                         this, 
                                                         this,
                                                         SLOT(newBorderFileButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(newFileToolButton);
    
    /*
     * Completer for name
     */
    m_nameCompleterStringListModel = new QStringListModel(this);
    
    /*
     * Name
     */
    QLabel* nameLabel = new QLabel("Name");
    m_nameComboBox = new GiftiLabelTableSelectionComboBox(this);
    m_nameComboBox->setUnassignedLabelTextOverride("Select Name");
//    m_nameLineEdit->setText(borderName);
    QAction* displayNameColorEditorAction = WuQtUtilities::createAction("Add/Edit...",
                                                                    "Add and/or edit name colors",
                                                                    this,
                                                                    this,
                                                                    SLOT(displayNameEditor()));
    QToolButton* displayNameColorEditorToolButton = new QToolButton();
    displayNameColorEditorToolButton->setDefaultAction(displayNameColorEditorAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(displayNameColorEditorToolButton);
    
    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    m_classComboBox = new GiftiLabelTableSelectionComboBox(this);
    WuQtUtilities::setToolTipAndStatusTip(m_classComboBox->getWidget(),
                                          "The class is used to group borders with similar\n"
                                          "characteristics.  Controls are available to\n"
                                          "display borders by their class attributes.");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Add/Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(displayClassEditorToolButton);
    
    /*
     * Closed
     */
    m_closedCheckBox = new QCheckBox("Closed Border");
    WuQtUtilities::setToolTipAndStatusTip(m_closedCheckBox, 
                                          "If checked, additional points will be added\n"
                                          "to the border so that the border forms a loop\n"
                                          "with the last point adjacent to the first point.");
    if (s_previousClosedSelected) {
        m_closedCheckBox->setChecked(true);
    }
    else {
        m_closedCheckBox->setChecked(false);
    }
    
    /*
     * Reverse point order
     */
    m_reversePointOrderCheckBox = new QCheckBox("Reverse Point Order");
    WuQtUtilities::setToolTipAndStatusTip(m_reversePointOrderCheckBox, 
                                          "If checked, the order of the points in the \n"
                                          "border are reversed when the OK button is pressed.");
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = 0;
    gridLayout->addWidget(borderFileLabel, row, 0);
    gridLayout->addWidget(m_borderFileSelectionComboBox, row, 1);
    gridLayout->addWidget(newFileToolButton, row, 2);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_nameComboBox->getWidget(), row, 1);
    gridLayout->addWidget(displayNameColorEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(m_classComboBox->getWidget(), row, 1);
    gridLayout->addWidget(displayClassEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_closedCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_reversePointOrderCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    
    /*
     * Show/Hide options based upon mode
     */
    bool showFileOptionFlag   = false;
    bool showClosedOptionFlag = false;
    bool showReverseOptionFlag = false;
    switch (m_mode) {
        case MODE_EDIT:
            showReverseOptionFlag = true;
            break;
        case MODE_FINISH_DRAWING:
            showFileOptionFlag = true;
            showClosedOptionFlag = true;
            break;
    }
    
    borderFileLabel->setVisible(showFileOptionFlag);
    m_borderFileSelectionComboBox->setVisible(showFileOptionFlag);
    newFileToolButton->setVisible(showFileOptionFlag);
    m_closedCheckBox->setVisible(showClosedOptionFlag);
    m_reversePointOrderCheckBox->setVisible(showReverseOptionFlag);
    
    loadBorderFileComboBox();
    loadNameComboBox(borderName);
    loadClassComboBox(className);
    
    /*
     * Set the widget for the dialog.
     */
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
BorderPropertiesEditorDialog::~BorderPropertiesEditorDialog()
{
    
}

/**
 * Get the selected border file.
 * @return BorderFile or NULL if no border file.
 */
BorderFile* 
BorderPropertiesEditorDialog::getSelectedBorderFile()
{
    if (m_editModeBorderFile != NULL) {
        return m_editModeBorderFile;
    }
    
    const int fileComboBoxIndex = m_borderFileSelectionComboBox->currentIndex();
    void* filePointer = m_borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    s_previousBorderFile = borderFile;
    
    return borderFile;
}

/**
 * Load the SINGLE STRUCTURE border files into the border file combo box.
 */
void 
BorderPropertiesEditorDialog::loadBorderFileComboBox()
{
    CaretAssert(m_border);
    const StructureEnum::Enum borderStructure = m_border->getStructure();
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    m_borderFileSelectionComboBox->clear();
    
    int defaultFileComboIndex = 0;
    for (int32_t i = 0; i < numBorderFiles; i++) {
        BorderFile* borderFile = brain->getBorderFile(i);
        if (borderFile->isSingleStructure()) {
            if (borderFile->getStructure() == borderStructure) {
                const AString name = borderFile->getFileNameNoPath();
                m_borderFileSelectionComboBox->addItem(name,
                                                       QVariant::fromValue((void*)borderFile));
                if (s_previousBorderFile == borderFile) {
                    defaultFileComboIndex = m_borderFileSelectionComboBox->count() - 1;
                }
            }
        }
    }
    
    if (m_borderFileSelectionComboBox->count() > 0) {
        m_borderFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
        
        const BorderFile* borderFile = getSelectedBorderFile();
        if (borderFile != NULL) {
            m_nameCompleterStringList = borderFile->getAllBorderNamesSorted();
            m_nameCompleterStringListModel->setStringList(m_nameCompleterStringList);
        }
    }
}

/**
 * Called to create a new border file.
 */
void 
BorderPropertiesEditorDialog::newBorderFileButtonClicked()
{
    CaretAssert(m_border);
    const StructureEnum::Enum borderStructure = m_border->getStructure();
    
    if (StructureEnum::isSingleStructure(borderStructure)) {
        CaretAssert(m_finishBorderSurfaceFile);
        
        /*
         * Let user choose a different path/name
         */
        BorderFile* newBorderFile = new BorderFile();
        GuiManager::get()->getBrain()->convertDataFilePathNameToAbsolutePathName(newBorderFile);
        AString newBorderFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::BORDER,
                                                                           this,
                                                                           "Choose Border File Name",
                                                                           newBorderFile->getFileName());
        /*
         * If user cancels, delete the new border file and return
         */
        if (newBorderFileName.isEmpty()) {
            delete newBorderFile;
            return;
        }
        
        /*
         * Set name of new border file along with structure and number of nodes
         */
        newBorderFile->setFileName(newBorderFileName);
        newBorderFile->setStructure(borderStructure);
        newBorderFile->setNumberOfNodes(m_finishBorderSurfaceFile->getNumberOfNodes());
        EventManager::get()->sendEvent(EventDataFileAdd(newBorderFile).getPointer());
        s_previousBorderFile = newBorderFile;
        loadBorderFileComboBox();
        borderFileSelected();
    }
    else {
        WuQMessageBox::errorOk(this, ("Border must be for a single structure but it is for "
                                      + StructureEnum::toGuiName(borderStructure)));
    }
}


/**
 * Called when a border file is selected.
 */
void 
BorderPropertiesEditorDialog::borderFileSelected()
{
    loadNameComboBox();
    if (m_classComboBox != NULL) {
        loadClassComboBox();
    }
}

/**
 * Load the class combo box.
 *
 * @param name
 *   If not empty, make this name the selected name.
 */
void 
BorderPropertiesEditorDialog::loadClassComboBox(const QString& name)
{
    BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile != NULL) {
        m_classComboBox->updateContent(borderFile->getClassColorTable());
        if (name.isEmpty() == false) {
            m_classComboBox->setSelectedLabelName(name);
        }
    }
    else {
        m_classComboBox->updateContent(NULL);
    }
}

/**
 * Load the name combo box.
 *
 * @param name
 *   If not empty, make this name the selected name.
 */
void
BorderPropertiesEditorDialog::loadNameComboBox(const QString& name)
{
    BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile != NULL) {
        m_nameComboBox->updateContent(borderFile->getNameColorTable());
        if (name.isEmpty() == false) {
            m_nameComboBox->setSelectedLabelName(name);
        }
    }
    else {
        m_nameComboBox->updateContent(NULL);
    }    
}

/**
 * Called when the OK button is pressed.
 */
void 
BorderPropertiesEditorDialog::okButtonClicked()
{
    AString errorMessage;

    /*
     * Get border file.
     */
    BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    /*
     * Get data entered by the user.
     */
    const AString name = m_nameComboBox->getSelectedLabelName();
    if (name.isEmpty()) {
        errorMessage += ("Name is invalid.\n");
    }
    else {
        const int32_t unassignedNameKey = borderFile->getNameColorTable()->getUnassignedLabelKey();
        const int32_t selectedNameKey = m_nameComboBox->getSelectedLabelKey();
        if (selectedNameKey == unassignedNameKey) {
            errorMessage += "Choose or create a name for the border";
        }
    }
    
    const QString className = m_classComboBox->getSelectedLabelName();
    
    /*
     * Error?
     */
    if (errorMessage.isEmpty() == false) {
            WuQMessageBox::errorOk(this, 
                                   errorMessage);
        return;
    }
    
    
    Border* borderBeingEdited = NULL;
    bool finishModeFlag = false;
    switch (m_mode) {
        case MODE_EDIT:
            borderBeingEdited = m_border;
            break;
        case MODE_FINISH_DRAWING:
            borderBeingEdited = new Border(*m_border);
            finishModeFlag = true;
            break;
    }
    
    /*
     * Make a copy of the border being drawn
     */
    borderBeingEdited->setName(name);
    borderBeingEdited->setClassName(className);
    
    if (finishModeFlag) {
        /*
         * Close border
         */
        if (m_closedCheckBox->isChecked()) {
            borderBeingEdited->addPointsToCloseBorderWithGeodesic(m_finishBorderSurfaceFile);
            borderBeingEdited->setClosed(true);
        }
        else {
            borderBeingEdited->setClosed(false);
        }
        
        /*
         * Add border to border file
         */
        CaretAssert(borderFile);
        borderFile->addBorder(borderBeingEdited);
        
        /*
         * Save values entered by the user and
         * use them to initialize the dialog next
         * time it is displayed.
         */
        s_previousDataValid = true;
        s_previousName = name;
        s_previousClassName = className;
        s_previousClosedSelected = m_closedCheckBox->isChecked();
        s_previousBorderFile = borderFile;
    }
    else {
        if (m_reversePointOrderCheckBox->isChecked()) {
            m_border->reverse();
        }
    }
    
    if (m_nameCompleterStringList.contains(name) == false) {
        m_nameCompleterStringList.append(name);
        m_nameCompleterStringList.sort();
        m_nameCompleterStringListModel->setStringList(m_nameCompleterStringList);
    }
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonClicked();
}

/**
 * Display the class editor
 */
void 
BorderPropertiesEditorDialog::displayClassEditor()
{
    BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    /*
     * Need to detect a change in the name class table.
     * So:
     *  (1) Save the modified status
     *  (2) Clear the modified status
     *  (3) After editing, see if the modified status is on indicating
     *      that the user has made a change to the table
     *  (4) If user modified table, invalidate border colors so that
     *      the borders get the new color(s).
     *  (5) If user DID NOT modify table, restore the modification
     *      status of the table.
     */
    GiftiLabelTable* classTable = borderFile->getClassColorTable();
    const bool modifiedStatus = classTable->isModified();
    classTable->clearModified();
    
    GiftiLabelTableEditor editor(borderFile,
                                 classTable,
                                 "Edit Class Attributes",
                                 GiftiLabelTableEditor::OPTION_NONE,
                                 this);
    const QString className = m_classComboBox->getSelectedLabelName();
    if (className.isEmpty() == false) {
        editor.selectLabelWithName(className);
    }
    
    const int dialogResult = editor.exec();
    
    loadClassComboBox();

    if (dialogResult == GiftiLabelTableEditor::Accepted) {
        const QString selectedClassName = editor.getLastSelectedLabelName();
        if (selectedClassName.isEmpty() == false) {
            m_classComboBox->setSelectedLabelName(selectedClassName);
        }
    }
    
    if (classTable->isModified()) {
        /*
         * User changed something in the table.
         */
        borderFile->invalidateAllAssignedColors();
    }
    else {
        if (modifiedStatus) {
            /*
             * User did not change anything but need to restore
             * modified status.
             */
            classTable->setModified();
        }
    }
}

/**
 * Display the name editor
 */
void
BorderPropertiesEditorDialog::displayNameEditor()
{
    BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    /*
     * Need to detect a change in the name color table.
     * So:
     *  (1) Save the modified status
     *  (2) Clear the modified status
     *  (3) After editing, see if the modified status is on indicating
     *      that the user has made a change to the table
     *  (4) If user modified table, invalidate border colors so that
     *      the borders get the new color(s).
     *  (5) If user DID NOT modify table, restore the modification
     *      status of the table.
     */
    GiftiLabelTable* nameTable = borderFile->getNameColorTable();
    const bool modifiedStatus = nameTable->isModified();
    nameTable->clearModified();
    
    GiftiLabelTableEditor editor(borderFile,
                                 nameTable,
                                 "Edit Name Attributes",
                                 GiftiLabelTableEditor::OPTION_UNASSIGNED_LABEL_HIDDEN,
                                 this);
    const QString name = m_nameComboBox->getSelectedLabelName();
    if (name.isEmpty() == false) {
        editor.selectLabelWithName(name);
    }
    
    const int dialogResult = editor.exec();
    
    loadNameComboBox();
    
    if (dialogResult == GiftiLabelTableEditor::Accepted) {
        const QString selectedName = editor.getLastSelectedLabelName();
        if (selectedName.isEmpty() == false) {
            m_nameComboBox->setSelectedLabelName(selectedName);
        }
    }
    
    if (nameTable->isModified()) {
        /*
         * User changed something in the table.
         */
        borderFile->invalidateAllAssignedColors();
    }
    else {
        if (modifiedStatus) {
            /*
             * User did not change anything but need to restore
             * modified status.
             */
            nameTable->setModified();
        }
    }
}




