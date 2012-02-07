
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
    
    /*
     * File selection combo box
     */
    QLabel* borderFileLabel = new QLabel("Border File");
    this->borderFileSelectionComboBox = new QComboBox();
    this->loadBorderFileComboBox();
    
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

    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    this->classNameComboBox = new QComboBox();
    this->classNameComboBox->addItem("Class 1");
    this->classNameComboBox->addItem("Class 2");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    
    const int previousClassIndex = this->classNameComboBox->findText(BorderDrawFinishDialog::previousClassName);
    if (previousClassIndex >= 0) {
        this->classNameComboBox->setCurrentIndex(previousClassIndex);
    }
    
    /*
     * Open/Closed
     */
    this->closedCheckBox = new QCheckBox("Closed Border");
    
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
 * @return Get the selected border file.  If New Border
 * File is selected, create a border file and update
 * the border file combo box.
 */
BorderFile* 
BorderDrawFinishDialog::getSelectedBorderFile()
{
    const int fileComboBoxIndex = this->borderFileSelectionComboBox->currentIndex();
    void* filePointer = this->borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    if (borderFile == NULL) {
        borderFile = GuiManager::get()->getBrain()->addBorderFile();
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
    
    /*
     * Error?
     */
    if (errorMessage.isEmpty() == false) {
            WuQMessageBox::errorOk(this, 
                                   errorMessage);
    }
    
    /*
     * Get/Create border file.
     */
    BorderFile* borderFile = this->getSelectedBorderFile();
    
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
    BorderFile* borderFile = this->getSelectedBorderFile();
    GiftiLabelTableEditor editor(borderFile->getClassNamesTable(),
                                 "Edit Class Attributes",
                                 this);
    editor.exec();
}


