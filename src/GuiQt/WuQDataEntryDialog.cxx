/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>
#include "StructureEnumComboBox.h"
#include "SurfaceSelectionViewController.h"
#include "WuQDataEntryDialog.h"

using namespace caret;

/**
 * constructor.
 */
WuQDataEntryDialog::WuQDataEntryDialog(const QString& title,
                                       QWidget* parent,
                                       const bool addScrollBarsFlag,
                                       Qt::WindowFlags f)
   : WuQDialogModal(title, parent, f)
{
   //
   // Widget and Layout for user's widgets
   //
   QWidget* widgetForGridLayout = new QWidget;
   widgetGridLayout = new QGridLayout(widgetForGridLayout);
   
   //
   // Labels for text at top, hidden until set by user
   //
   textAtTopLabel = new QLabel;
   textAtTopLabel->setHidden(true);
   
   //
   // ButtonGroup for radio buttons
   //
   radioButtonGroup = new QButtonGroup(this);
   
   //
   // Layout for dialog
   //
   QWidget* widget = new QWidget();
   QVBoxLayout* dialogLayout = new QVBoxLayout(widget);
   dialogLayout->addWidget(textAtTopLabel);
   dialogLayout->addWidget(widgetForGridLayout);

   this->setCentralWidget(widget,
                          addScrollBarsFlag);
}
                   
/**
 * destructor.
 */
WuQDataEntryDialog::~WuQDataEntryDialog()
{
    for (std::vector<StructureEnumComboBox*>::iterator iter = this->structureEnumComboBoxesToDelete.begin();
         iter != this->structureEnumComboBoxesToDelete.end();
         iter++) {
        delete *iter;
    }
    for (std::vector<SurfaceSelectionViewController*>::iterator iter = this->surfaceSelectionViewControllersToDelete.begin();
         iter != this->surfaceSelectionViewControllersToDelete.end();
         iter++) {
        delete *iter;
    }
}

/**
 * Called when the OK button is pressed.
 * If needed should override this to process
 * data when the OK button is pressed and then
 * call this to issue the accept signal.
 */
void 
WuQDataEntryDialog::okButtonPressed()
{
    if (dataEnteredIsValid()) {
        accept();
    }
}

/**
 * override to verify data after OK button pressed.
 */
bool 
WuQDataEntryDialog::dataEnteredIsValid()
{
   return true;
}

/**
 * set text at top of dialog (text is automatically wrapped).
 */
void 
WuQDataEntryDialog::setTextAtTop(const QString& s,
                                 const bool wrapTheText)
{
   textAtTopLabel->setText(s);
   textAtTopLabel->setWordWrap(wrapTheText);
   
   if (s.isEmpty()) {
      textAtTopLabel->setHidden(true);
   }
   else {
      textAtTopLabel->setHidden(false);
   }
}
      
/**
 * add widgets to the next available row in the dialog.
 */
void 
WuQDataEntryDialog::addWidgetsToNextRow(QWidget* leftColumnWidget,
                                        QWidget* rightColumnWidget)
{
   //
   // add widgets to the next row
   //
   const int rowNumber = widgetGridLayout->rowCount();
   if (leftColumnWidget != NULL) {
      widgetGridLayout->addWidget(leftColumnWidget, rowNumber, 0);
   }
   if (rightColumnWidget != NULL) {
      widgetGridLayout->addWidget(rightColumnWidget, rowNumber, 1);
   }
}

/**
 * add widget to next available row in the dialog.
 */
QWidget* 
WuQDataEntryDialog::addWidget(const QString& labelText,
                              QWidget* widget)
{
   //
   // Create the label
   //
   QLabel* label = new QLabel(labelText);
   
   //
   // Keep pointer to widget
   //
   widgets.push_back(widget);
   
   //
   // add widget to layout
   //
   addWidgetsToNextRow(label, widget);
   
   return widget;
}
                     
/**
 * add a check box.
 */
QCheckBox* 
WuQDataEntryDialog::addCheckBox(const QString& text,
                                const bool defaultValue)
{
   //
   // Create check box
   //
   QCheckBox* cb = new QCheckBox(text);
   cb->setChecked(defaultValue);

   //
   // Keep pointer to widget
   //
   widgets.push_back(cb);
   
   //
   // add widget to both columns of layout
   //
   const int rowNumber = widgetGridLayout->rowCount();
   widgetGridLayout->addWidget(cb, rowNumber, 0, 1, 2, Qt::AlignLeft);
   
   return cb;
}

/**
 * get radio button selected (-1 if none, value is sequence added).
 */
int 
WuQDataEntryDialog::getRadioButtonSelected() const
{
   return radioButtonGroup->checkedId();
}

/**
 * add a radio button (all radio buttons will be mutually exclusive).
 */
QRadioButton* 
WuQDataEntryDialog::addRadioButton(const QString& text,
                                   const bool defaultValue)
{
   //
   // Create radio button
   //
   QRadioButton* rb = new QRadioButton(text);
   rb->setChecked(defaultValue);

   //
   // Add to radio button group
   //
   const int buttNum = radioButtonGroup->buttons().count();
   radioButtonGroup->addButton(rb, buttNum);
   
   //
   // Keep pointer to widget
   //
   widgets.push_back(rb);
   
   //
   // add widget to both columns of layout
   //
   const int rowNumber = widgetGridLayout->rowCount();
   widgetGridLayout->addWidget(rb, rowNumber, 0, 1, 2, Qt::AlignLeft);
   
   return rb;
}                                   
                             
/**
 * add a combo box.
 */
QComboBox* 
WuQDataEntryDialog::addComboBox(const QString& labelText,
                                const QStringList& comboBoxItems,
                                const QList<QVariant>* comboBoxItemsUserData)
{
   //
   // Create combo box
   //
   QComboBox* comboBox = new QComboBox;
   for (int i = 0; i < comboBoxItems.size(); i++) {
      QVariant userData;
      if (comboBoxItemsUserData != NULL) {
         if (i < comboBoxItemsUserData->size()) {
            userData = (*comboBoxItemsUserData).at(i);
         }
      }
      comboBox->addItem(comboBoxItems.at(i), 
                        userData);
   }
   
   //
   // Add to dialog
   //
   addWidget(labelText, comboBox);
   
   return comboBox;
}
                       
/**
 * add line edit.
 */
QLineEdit* 
WuQDataEntryDialog::addLineEditWidget(const QString& labelText,
                                      const QString& defaultText)
{
   //
   // Create line edit
   //
   QLineEdit* le = new QLineEdit;
   le->setText(defaultText);

   //
   // Add to dialog
   //
   addWidget(labelText, le);
   
   return le;
}
      
/*
 * add list box.
 */
QListWidget* 
WuQDataEntryDialog::addListWidget(const QString& labelText,
                                  const QStringList& listBoxItems)
{
   //
   // Create and populate list widget
   //
   QListWidget* lw = new QListWidget;
   lw->addItems(listBoxItems);
   if (listBoxItems.count() > 0) {
      lw->setCurrentRow(0);
   }
   
   //
   // Add to dialog
   //
   addWidget(labelText, lw);
      
   return lw;
}

/**
 * add spin box.
 */
QSpinBox* 
WuQDataEntryDialog::addSpinBox(const QString& labelText,
                               const int defaultValue,
                               const int minimumValue,
                               const int maximumValue,
                               const int singleStep)
{
   //
   // Create spin box
   //
   QSpinBox* sb = new QSpinBox;
   sb->setMinimum(minimumValue);
   sb->setMaximum(maximumValue);
   sb->setSingleStep(singleStep);
   sb->setValue(defaultValue);
   
   //
   // Add to dialog
   //
   addWidget(labelText, sb);
   
   return sb;
}                            

/**
 * add double spin box.
 */
QDoubleSpinBox* 
WuQDataEntryDialog::addDoubleSpinBox(const QString& labelText,
                                     const float defaultValue,
                                     const float minimumValue,
                                     const float maximumValue,
                                     const float singleStep,
                                     const int numberOfDecimals)
{
   //
   // Create spin box
   //
   QDoubleSpinBox* sb = new QDoubleSpinBox;
   sb->setMinimum(minimumValue);
   sb->setMaximum(maximumValue);
   sb->setSingleStep(singleStep);
   sb->setValue(defaultValue);
   sb->setDecimals(numberOfDecimals);
   
   //
   // Add to dialog
   //
   addWidget(labelText, sb);
   
   return sb;
}                                     

/**
 * add a text edit.
 */
QTextEdit* 
WuQDataEntryDialog::addTextEdit(const QString& labelText,
                                const QString& defaultText,
                                const bool readOnlyFlag)                       
{
   //
   // Create text edit
   //
   QTextEdit* te = new QTextEdit;
   te->setReadOnly(readOnlyFlag);
   te->setPlainText(defaultText);
   
   //
   // add to dialog
   //
   addWidget(labelText, te);
   
   return te;
}

/**
 * Add a structure selection control.
 */
StructureEnumComboBox* 
WuQDataEntryDialog::addStructureEnumComboBox(const QString& labelText,
                                                 const StructureEnum::Enum defaultStructure)
{
    StructureEnumComboBox* structureEnumComboBox = 
    new StructureEnumComboBox(this);
    structureEnumComboBox->setSelectedStructure(defaultStructure);
    
    this->addWidget(labelText,
                    structureEnumComboBox->getWidget());
    this->structureEnumComboBoxesToDelete.push_back(structureEnumComboBox);
    
    return structureEnumComboBox;
}

/**
 * Add a surface selection control
 */
SurfaceSelectionViewController* 
WuQDataEntryDialog::addSurfaceSelectionViewController(const QString& labelText,
                                               BrainStructure* brainStructure)
{
    SurfaceSelectionViewController* surfaceSelectionViewController =
    new SurfaceSelectionViewController(this,
                                brainStructure);
    
    this->addWidget(labelText,
                    surfaceSelectionViewController->getWidget());
    this->surfaceSelectionViewControllersToDelete.push_back(surfaceSelectionViewController);
    
    return surfaceSelectionViewController;
}


