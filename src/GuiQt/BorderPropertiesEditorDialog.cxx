
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#define __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
#include "BorderPropertiesEditorDialog.h"
#undef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

#include "Brain.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretColorEnumSelectionControl.h"
#include "ClassAndNameHierarchyModel.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "GuiManager.h"
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
    this->finishBorderSurfaceFile = finishBorderSurfaceFile;
    this->editModeBorderFile = editModeBorderFile;
    this->mode   = mode;
    this->border = border;
    this->classNameComboBox = NULL;
    
    QString borderName = "";
    CaretColorEnum::Enum borderColor = CaretColorEnum::BLACK;
    QString className = "";
    switch (this->mode) {
        case MODE_EDIT:
            borderName = border->getName();
            borderColor = border->getColor();
            className = border->getClassName();
            break;
        case MODE_FINISH_DRAWING:
            borderName = BorderPropertiesEditorDialog::previousName;
            borderColor = BorderPropertiesEditorDialog::previousCaretColor;
            className = BorderPropertiesEditorDialog::previousClassName;
            break;
    }
    
    /*
     * File selection combo box
     */
    QLabel* borderFileLabel = new QLabel("Border File");
    this->borderFileSelectionComboBox = new QComboBox();
    this->loadBorderFileComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->borderFileSelectionComboBox, 
                                          "Selects a new or existing border file\n"
                                          "to which new borders are added.");
    QObject::connect(this->borderFileSelectionComboBox, SIGNAL(activated(int)),
                     this, SLOT(borderFileSelected()));
    
    /*
     * Name
     */
    QLabel* nameLabel = new QLabel("Name");
    this->nameLineEdit = new QLineEdit();
    this->nameLineEdit->setText(borderName);
    
    /*
     * Color
     */
    QLabel* colorLabel = new QLabel("Color");
    this->colorSelectionControl = new CaretColorEnumSelectionControl(this,
                                                                     CaretColorEnum::OPTION_INCLUDE_CLASS);
    this->colorSelectionControl->setSelectedColor(borderColor);
    WuQtUtilities::setToolTipAndStatusTip(this->colorSelectionControl->getWidget(), 
                                          "If the color is set to \"CLASS\", the border is colored\n"
                                          "using the color associated with the border's class.\n"
                                          "Otherwise, if a color name is selected, it is used\n"
                                          "to color the border.");

    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    this->classNameComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->classNameComboBox, 
                                          "The class is used to group borders with similar\n"
                                          "characteristics.  Controls are available to\n"
                                          "display borders by their class attributes.");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    this->loadClassNameComboBox(className);
    
    /*
     * Open/Closed
     */
    this->closedCheckBox = new QCheckBox("Closed Border");
    WuQtUtilities::setToolTipAndStatusTip(this->closedCheckBox, 
                                          "If checked, additional points will be added\n"
                                          "to the border so that the border forms a loop\n"
                                          "with the last point adjacent to the first point.");
    
    QButtonGroup* openClosedButtonGroup = new QButtonGroup(this);
    openClosedButtonGroup->addButton(this->closedCheckBox);
    if (BorderPropertiesEditorDialog::previousClosedSelected) {
        this->closedCheckBox->setChecked(true);
    }
    else {
        this->closedCheckBox->setChecked(false);
    }
    
    /*
     * Reverse point order
     */
    this->reversePointOrderCheckBox = new QCheckBox("Reverse Point Order");
    WuQtUtilities::setToolTipAndStatusTip(this->reversePointOrderCheckBox, 
                                          "If checked, the order of the points in the \n"
                                          "border are reversed when the OK button is pressed.");
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = 0;
    gridLayout->addWidget(borderFileLabel, row, 0);
    gridLayout->addWidget(this->borderFileSelectionComboBox, row, 1);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(this->nameLineEdit, row, 1);
    row++;
    gridLayout->addWidget(colorLabel, row, 0);
    gridLayout->addWidget(this->colorSelectionControl->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(this->classNameComboBox, row, 1);
    gridLayout->addWidget(displayClassEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(this->closedCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(this->reversePointOrderCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    
    /*
     * Show/Hide options based upon mode
     */
    bool showFileOptionFlag   = false;
    bool showClosedOptionFlag = false;
    bool showReverseOptionFlag = false;
    switch (this->mode) {
        case MODE_EDIT:
            showReverseOptionFlag = true;
            break;
        case MODE_FINISH_DRAWING:
            showFileOptionFlag = true;
            showClosedOptionFlag = true;
            break;
    }
    
    borderFileLabel->setVisible(showClosedOptionFlag);
    this->borderFileSelectionComboBox->setVisible(showFileOptionFlag);
    this->closedCheckBox->setVisible(showClosedOptionFlag);
    this->reversePointOrderCheckBox->setVisible(showReverseOptionFlag);
    
    /*
     * Set the widget for the dialog.
     */
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
BorderPropertiesEditorDialog::~BorderPropertiesEditorDialog()
{
    
}

/**
 * Get the selected border file.  If New Border
 * File is selected, create a border file and update
 * the border file combo box.
 * @param createIfNoValidBorderFiles
 *    If there are no valid border files, create one.
 * @return BorderFile or NULL if no border file.
 */
BorderFile* 
BorderPropertiesEditorDialog::getSelectedBorderFile(bool createIfNoValidBorderFiles)
{
    if (this->editModeBorderFile != NULL) {
        return this->editModeBorderFile;
    }
    const int fileComboBoxIndex = this->borderFileSelectionComboBox->currentIndex();
    void* filePointer = this->borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    if (borderFile == NULL) {
        if (createIfNoValidBorderFiles) {
            borderFile = GuiManager::get()->getBrain()->addBorderFile();
        }
    }
    BorderPropertiesEditorDialog::previousBorderFile = borderFile;
    
    this->loadBorderFileComboBox();
    
    return borderFile;
}

/**
 * Load the border files into the border file combo box.
 */
void 
BorderPropertiesEditorDialog::loadBorderFileComboBox()
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    this->borderFileSelectionComboBox->clear();
    
    int defaultFileComboIndex = 0;
    this->borderFileSelectionComboBox->addItem("New File",
                                               qVariantFromValue((void*)NULL));
    for (int32_t i = 0; i < numBorderFiles; i++) {
        BorderFile* borderFile = brain->getBorderFile(i);
        const AString name = borderFile->getFileNameNoPath();
        this->borderFileSelectionComboBox->addItem(name,
                                                   qVariantFromValue((void*)borderFile));
        if (BorderPropertiesEditorDialog::previousBorderFile == borderFile) {
            defaultFileComboIndex = this->borderFileSelectionComboBox->count() - 1;
        }
    }
    this->borderFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
}

/**
 * Called when a border file is selected.
 */
void 
BorderPropertiesEditorDialog::borderFileSelected()
{
    if (this->classNameComboBox != NULL) {
        this->loadClassNameComboBox();
    }
}

/**
 * Load the class name combo box.
 */
void 
BorderPropertiesEditorDialog::loadClassNameComboBox(const QString& className)
{
    AString selectedClassName = this->classNameComboBox->currentText();
    if (className.isEmpty() == false) {
        selectedClassName = className;
    }
    if (selectedClassName.isEmpty()) {
        selectedClassName = BorderPropertiesEditorDialog::previousClassName;
    }
    
    this->classNameComboBox->clear();
    
    BorderFile* borderFile = this->getSelectedBorderFile(false);
    if (borderFile != NULL) {
        const GiftiLabelTable* classLabelTable = borderFile->getClassColorTable();
        std::vector<int32_t> keys = classLabelTable->getLabelKeysSortedByName();
        for (std::vector<int32_t>::iterator keyIterator = keys.begin();
             keyIterator != keys.end();
             keyIterator++) {
            const int32_t key = *keyIterator;
            const GiftiLabel* gl = classLabelTable->getLabel(key);
            
            this->classNameComboBox->addItem(gl->getName());
        }
    }
    const int previousClassIndex = this->classNameComboBox->findText(selectedClassName);
    if (previousClassIndex >= 0) {
        this->classNameComboBox->setCurrentIndex(previousClassIndex);
    }
}

/**
 * Called when the OK button is pressed.
 */
void 
BorderPropertiesEditorDialog::okButtonPressed()
{
    AString errorMessage;

    /*
     * Get data entered by the user.
     */
    const AString name = this->nameLineEdit->text();
    if (name.isEmpty()) {
        errorMessage += ("Name is invalid.\n");
    }
    const QString className = this->classNameComboBox->currentText().trimmed();
    const CaretColorEnum::Enum color = this->colorSelectionControl->getSelectedColor();
    
    if (color == CaretColorEnum::CLASS) {
        if (className.isEmpty()) {
            errorMessage += ("Color is set to class but no class is selected.  "
                             "Either change the color or add a class using "
                             "the Edit button to the right of the class control.\n");
        }
    }
    
    /*
     * Error?
     */
    if (errorMessage.isEmpty() == false) {
            WuQMessageBox::errorOk(this, 
                                   errorMessage);
        return;
    }
    
    /*
     * Get/Create border file.
     */
    BorderFile* borderFile = this->getSelectedBorderFile(true);
    
    Border* borderBeingEdited = NULL;
    bool finishModeFlag = false;
    switch (this->mode) {
        case MODE_EDIT:
            borderBeingEdited = this->border;
            break;
        case MODE_FINISH_DRAWING:
            borderBeingEdited = new Border(*this->border);
            finishModeFlag = true;
            break;
    }
    
    /*
     * Make a copy of the border being drawn
     */
    borderBeingEdited->setName(name);
    borderBeingEdited->setClassName(className);
    borderBeingEdited->setColor(color);
    
    if (finishModeFlag) {
        /*
         * Close border
         */
        if (this->closedCheckBox->isChecked()) {
            borderBeingEdited->addPointsToCloseBorderWithGeodesic(this->finishBorderSurfaceFile);    
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
        BorderPropertiesEditorDialog::previousName = name;
        BorderPropertiesEditorDialog::previousClassName = className;
        BorderPropertiesEditorDialog::previousClosedSelected = this->closedCheckBox->isChecked();
        BorderPropertiesEditorDialog::previousBorderFile = borderFile;
        BorderPropertiesEditorDialog::previousCaretColor = color;
    }
    else {
        if (this->reversePointOrderCheckBox->isChecked()) {
            this->border->reverse();
        }
    }
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonPressed();
}

/**
 * Display the class editor
 */
void 
BorderPropertiesEditorDialog::displayClassEditor()
{
    BorderFile* borderFile = this->getSelectedBorderFile(true);
    GiftiLabelTable* classLabelTable = borderFile->getClassColorTable();
    GiftiLabelTableEditor editor(classLabelTable,
                                 "Edit Class Attributes",
                                 this);
    const QString className = this->classNameComboBox->currentText();
    if (className.isEmpty() == false) {
        editor.selectLabelWithName(className);
    }
    editor.exec();
    
    this->loadClassNameComboBox();

    const QString selectedClassName = editor.getLastSelectedLabelName();
    if (selectedClassName.isEmpty() == false) {
        const int indx = this->classNameComboBox->findText(selectedClassName);
        if (indx >= 0) {
            this->classNameComboBox->setCurrentIndex(indx);
        }
    }
}



