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

#include "CaretColorEnumComboBox.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "StructureEnumComboBox.h"
#include "SurfaceSelectionViewController.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQImageLabel.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * constructor.
 *
 * @param title
 *     Title for dialog.
 * @param parent
 *     Parent of the dialog.
 * @param addScrollBarsFlag
 *     If true, add scrollbars to the dialog (typically true when the dialog
 *     will contain many items.
 * @param f
 *     Qt's Window flags.
 *
 */
WuQDataEntryDialog::WuQDataEntryDialog(const QString& title,
                                       QWidget* parent,
                                       const bool addScrollBarsFlag,
                                       Qt::WindowFlags f)
   : WuQDialogModal(title, parent, f)
{
    WuQDialog::ScrollAreaStatus scrollBarStatus = WuQDialog::SCROLL_AREA_NEVER;
    if (addScrollBarsFlag) {
        scrollBarStatus = WuQDialog::SCROLL_AREA_ALWAYS;
    }
    
    constructDialog(scrollBarStatus);
}

/**
 * constructor.
 *
 * @param title
 *     Title for dialog.
 * @param parent
 *     Parent of the dialog.
 * @param scrollBarStatus
 *     Add scrollbars (never, as needed, always).
 * @param f
 *     Qt's Window flags.
 *
 */
WuQDataEntryDialog::WuQDataEntryDialog(const QString& title,
                   QWidget* parent,
                   const WuQDialog::ScrollAreaStatus scrollBarStatus,
                   Qt::WindowFlags f)
: WuQDialogModal(title, parent, f)
{
    constructDialog(scrollBarStatus);    
}

/**
 * destructor.
 */
WuQDataEntryDialog::~WuQDataEntryDialog()
{
}

/**
 * Hide the cancel button.
 */
void
WuQDataEntryDialog::hideCancelButton()
{
    setCancelButtonText("");
}


/**
 * Finish construction of the dialog.
 *
 * @param scrollBarStatus
 *     Status for scroll bars.
 */
void
WuQDataEntryDialog::constructDialog(const WuQDialog::ScrollAreaStatus scrollBarStatus)
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
    const int NO_STRETCH  = 0;
    const int BIG_STRETCH = 100;
    QWidget* widget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(widget);
    dialogLayout->setSpacing(0);
    dialogLayout->addWidget(textAtTopLabel, NO_STRETCH);
    dialogLayout->addWidget(widgetForGridLayout, BIG_STRETCH);
    
    this->setCentralWidget(widget,
                           scrollBarStatus);
}


/**
 * Called when the OK button is pressed.
 * If needed should override this to process
 * data when the OK button is pressed and then
 * call this to issue the accept signal.
 */
void 
WuQDataEntryDialog::okButtonClicked()
{
    m_isDataValid = true;
    m_dataInvalidErrorMessage = "";
    
    emit validateData(this);
    
    if (m_isDataValid) {
//        if (dataEnteredIsValid()) {
//            accept();
//        }
        WuQDialogModal::okButtonClicked();
    }
    else {
        if (m_dataInvalidErrorMessage.isEmpty()) {
            m_dataInvalidErrorMessage = "Data is not valid.";
        }
        WuQMessageBox::errorOk(this,
                               m_dataInvalidErrorMessage);
    }
}

/**
 * This method is used to set the validity of the data
 * widgets that were added by the user.  To use this,
 * connect to the validateData() signal and then call
 * this to indicate the validity of the data widgets.
 * getDataWidgetWithName() can be used to get widgets
 * but only will work if the user sets the name of 
 * the widgets.
 * @param isValid
 *    True if the data is valid, else false.
 * @param dataInvalidErrorMessage
 *    If data is invalid, message describing why data is invalid.
 */
void 
WuQDataEntryDialog::setDataValid(const bool isValid,
                                 const QString& dataInvalidErrorMessage)
{
    m_isDataValid = isValid;
    m_dataInvalidErrorMessage = dataInvalidErrorMessage;
}

/**
 * Get the data widget that was added by user with the
 * given name.  For this method to return the correct
 * widget, the user must call setObjectName() on a 
 * widget after it is added to the dialog.
 * 
 * @param name
 *    Name of the widget.
 * @return
 *    Pointer to widget with the given name or NULL
 *    if the widget is not found.
 */
QWidget* 
WuQDataEntryDialog::getDataWidgetWithName(const QString& name)
{
    const int numWidgets = widgets.size();
    for (int i = 0; i < numWidgets; i++) {
        if (widgets[i]->objectName() == name) {
            return widgets[i];
        }
    }
    return NULL;
}

///**
// * override to verify data after OK button pressed.
// */
//bool 
//WuQDataEntryDialog::dataEnteredIsValid()
//{
//   return true;
//}

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
 * add widget that spans both the label and widget columns to next available row in the dialog.
 */
QWidget*
WuQDataEntryDialog::addWidget(QWidget* widget)
{
    //
    // Keep pointer to widget
    //
    widgets.push_back(widget);
    
    //
    // add widget to layout
    //
    const int rowNumber = widgetGridLayout->rowCount();
    widgetGridLayout->addWidget(widget,
                                rowNumber, 0, 1, 2);
    
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
    QComboBox* comboBox = WuQFactory::newComboBox();
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
 * Add file selection combo box for given data file types loaded in wb_view
 * @param labelText
 *     Text for label
 * @param dataFileTypes
 *     Data file types for selection
 */
CaretDataFileSelectionComboBox*
WuQDataEntryDialog::addFileSelectionComboBox(const QString& labelText,
                                             const std::vector<DataFileTypeEnum::Enum> dataFileTypes)
{
    CaretDataFileSelectionComboBox* comboBox(new CaretDataFileSelectionComboBox(this));
    comboBox->updateComboBox(CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(dataFileTypes));
    
    addWidget(labelText, comboBox->getWidget());
    
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
    QSpinBox* sb = WuQFactory::newSpinBox();
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
    QDoubleSpinBox* sb = WuQFactory::newDoubleSpinBox();
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
 * Add a caret color selection control.
 */
CaretColorEnumComboBox* 
WuQDataEntryDialog::addCaretColorEnumComboBox(const QString& labelText,
                                              const CaretColorEnum::Enum defaultColor)
{
    CaretColorEnumComboBox* caretColorComboBox =
    new CaretColorEnumComboBox(this);
    caretColorComboBox->setSelectedColor(defaultColor);
    
    this->addWidget(labelText,
                    caretColorComboBox->getWidget());
    
    return caretColorComboBox;
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
                                brainStructure,
                                       "DataEntryDialogSurfaceComboBox",
                                       "Data Entry");
    
    this->addWidget(labelText,
                    surfaceSelectionViewController->getWidget());
    
    return surfaceSelectionViewController;
}

/**
 * Add an image
 * @param labelText
 *    Text for label
 * @param image
 *    Image for display
 */
void
WuQDataEntryDialog::addImage(const QString labelText,
                             const QImage& image)
{
    WuQImageLabel* imageLabel = new WuQImageLabel(&image, "");
    addWidget(labelText, imageLabel);
}


