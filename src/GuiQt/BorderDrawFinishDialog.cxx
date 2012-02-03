
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
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
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
     * Get border files
     */
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    
    /*
     * File selection combo box
     */
    int defaultFileComboIndex = 0;
    QLabel* borderFileLabel = new QLabel("Border File");
    this->borderFileSelectionComboBox = new QComboBox();
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
    
    QAction* displayColorEditorAction = WuQtUtilities::createAction("Edit...", 
                                                                    "Add and/or edit colors", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayColorEditor()));
    QToolButton* displayColorEditorToolButton = new QToolButton();
    displayColorEditorToolButton->setDefaultAction(displayColorEditorAction);
    
    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    this->classNameLineEdit = new QLineEdit();
    this->classNameLineEdit->setText(BorderDrawFinishDialog::previousClassName);
    
    /*
     * Open/Closed
     */
    QLabel* typeLabel = new QLabel("Type");
    this->closedRadioButton = new QRadioButton("Closed");
    this->openRadioButton = new QRadioButton("Open");
    
    QButtonGroup* openClosedButtonGroup = new QButtonGroup(this);
    openClosedButtonGroup->addButton(this->closedRadioButton);
    openClosedButtonGroup->addButton(this->openRadioButton);
    if (BorderDrawFinishDialog::previousOpenTypeSelected) {
        this->openRadioButton->setChecked(true);
    }
    else {
        this->closedRadioButton->setChecked(true);
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
    gridLayout->addWidget(displayColorEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(this->classNameLineEdit, row, 1);
    row++;
    gridLayout->addWidget(typeLabel, row, 0);
    gridLayout->addWidget(this->closedRadioButton, row, 1);
    row++;
    gridLayout->addWidget(this->openRadioButton, row, 1);
    
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
    const QString className = this->classNameLineEdit->text().trimmed();
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
    const int fileComboBoxIndex = this->borderFileSelectionComboBox->currentIndex();
    void* filePointer = this->borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    if (borderFile == NULL) {
        borderFile = GuiManager::get()->getBrain()->addBorderFile();
    }
    
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
    BorderDrawFinishDialog::previousOpenTypeSelected = this->openRadioButton->isChecked();
    BorderDrawFinishDialog::previousBorderFile = borderFile;
    BorderDrawFinishDialog::previousCaretColor = color;
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonPressed();
}

void 
BorderDrawFinishDialog::displayColorEditor()
{
    GiftiLabelTableEditor editor(NULL,
                                 "Edit Border Colors",
                                 this);
    editor.exec();
}


