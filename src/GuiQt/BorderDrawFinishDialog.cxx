
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

#define __BORDER_DRAW_FINISH_DIALOG_DECLARE__
#include "BorderDrawFinishDialog.h"
#undef __BORDER_DRAW_FINISH_DIALOG_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretColorEnumSelectionControl.h"
#include "Brain.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderDrawFinishDialog 
 * \brief Dialog that queries user to finish drawing of a border.
 *
 * This dialog is displayed when the user finishes drawing a 
 * border.  It allows the user to select the border file, 
 * enter the border name, select the color, enter the class,
 * set the type of border (open/closed), and
 * possibly other attributes of the border.
 */
/**
 * Constructor.
 */
BorderDrawFinishDialog::BorderDrawFinishDialog(Border* border,
                                               QWidget* parent)
: WuQDialogModal("Finish Border",
                 parent)
{
    CaretAssert(border);
    this->border = border;
    this->classNameComboBox = NULL;
    
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
    this->nameLineEdit->setText(BorderDrawFinishDialog::previousName);
    
    /*
     * Color
     */
    QLabel* colorLabel = new QLabel("Color");
    this->colorSelectionControl = new CaretColorEnumSelectionControl(CaretColorEnum::OPTION_INCLUDE_CLASS);
    this->colorSelectionControl->setSelectedColor(CaretColorEnum::CLASS);
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
    this->loadClassNameComboBox();
    
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
    if (BorderDrawFinishDialog::previousClosedSelected) {
        this->closedCheckBox->setChecked(true);
    }
    else {
        this->closedCheckBox->setChecked(false);
    }
    
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
    
    /*
     * Set the widget for the dialog.
     */
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
BorderDrawFinishDialog::~BorderDrawFinishDialog()
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
BorderDrawFinishDialog::getSelectedBorderFile(bool createIfNoValidBorderFiles)
{
    const int fileComboBoxIndex = this->borderFileSelectionComboBox->currentIndex();
    void* filePointer = this->borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    if (borderFile == NULL) {
        if (createIfNoValidBorderFiles) {
            borderFile = GuiManager::get()->getBrain()->addBorderFile();
        }
    }
    BorderDrawFinishDialog::previousBorderFile = borderFile;
    
    this->loadBorderFileComboBox();
    
    return borderFile;
}

/**
 * Load the border files into the border file combo box.
 */
void 
BorderDrawFinishDialog::loadBorderFileComboBox()
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
        if (BorderDrawFinishDialog::previousBorderFile == borderFile) {
            defaultFileComboIndex = this->borderFileSelectionComboBox->count() - 1;
        }
    }
    this->borderFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
}

/**
 * Called when a border file is selected.
 */
void 
BorderDrawFinishDialog::borderFileSelected()
{
    if (this->classNameComboBox != NULL) {
        this->loadClassNameComboBox();
    }
}

/**
 * Load the class name combo box.
 */
void 
BorderDrawFinishDialog::loadClassNameComboBox()
{
    AString selectedClassName = this->classNameComboBox->currentText();
    if (selectedClassName.isEmpty()) {
        selectedClassName = BorderDrawFinishDialog::previousClassName;
    }
    
    this->classNameComboBox->clear();
    
    BorderFile* borderFile = this->getSelectedBorderFile(false);
    if (borderFile != NULL) {
        GiftiLabelTable* glt = borderFile->getClassNamesTable();
        std::set<int32_t> keys = glt->getKeys();
        for (std::set<int32_t>::iterator keyIterator = keys.begin();
             keyIterator != keys.end();
             keyIterator++) {
            const int32_t key = *keyIterator;
            const GiftiLabel* gl = glt->getLabel(key);
            
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
BorderDrawFinishDialog::okButtonPressed()
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
    
    /*
     * Make a copy of the border being drawn
     */
    Border* borderBeingCreated = new Border(*this->border);
    borderBeingCreated->setName(name);
    borderBeingCreated->setClassName(className);
    borderBeingCreated->setColor(color);
    
    /*
     * Add border to border file
     */
    CaretAssert(borderFile);
    borderFile->addBorder(borderBeingCreated);
    
    /*
     * Save values entered by the user and
     * use them to initialize the dialog next
     * time it is displayed.
     */
    BorderDrawFinishDialog::previousName = name;
    BorderDrawFinishDialog::previousClassName = className;
    BorderDrawFinishDialog::previousClosedSelected = this->closedCheckBox->isChecked();
    BorderDrawFinishDialog::previousBorderFile = borderFile;
    BorderDrawFinishDialog::previousCaretColor = color;
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonPressed();
}

/**
 * Display the class editor
 */
void 
BorderDrawFinishDialog::displayClassEditor()
{
    BorderFile* borderFile = this->getSelectedBorderFile(true);
    GiftiLabelTableEditor editor(borderFile->getClassNamesTable(),
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


