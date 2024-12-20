
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

#define __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__
#include "ChooseBorderFocusFromFileDialog.h"
#undef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__

#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QToolButton>
#include <QStackedWidget>

#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChooseBorderFocusFromFileDialog 
 * \brief Dialog for selecting a border or focus from a file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param fileMode
 *    The file mode - border or focus
 * @param parent
 *    The parent widget
 */
ChooseBorderFocusFromFileDialog::ChooseBorderFocusFromFileDialog(const FileMode fileMode,
                                                                 QWidget* parent)
: WuQDialogModal("Choose ",
                 parent),
m_fileMode(fileMode)
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
    switch (m_fileMode) {
        case FileMode::BORDER:
            setWindowTitle("Choose Border");
            m_dataFileType = DataFileTypeEnum::BORDER;
            break;
        case FileMode::FOCUS:
            setWindowTitle("Choose Focus");
            m_dataFileType = DataFileTypeEnum::FOCI;
            break;
    }
    CaretAssert(m_dataFileType != DataFileTypeEnum::UNKNOWN);

    QLabel* fileLabel(new QLabel("File: "));
    m_fileNameLineEdit = new QLineEdit();
    m_fileNameLineEdit->setReadOnly(true);
    
    QAction* fileAction(new QAction("Choose..."));
    QObject::connect(fileAction, &QAction::triggered,
                     this, &ChooseBorderFocusFromFileDialog::chooseFileActionTriggered);
    QToolButton* fileToolButton(new QToolButton());
    fileToolButton->setDefaultAction(fileAction);
    
    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    layout->setVerticalSpacing(15);
    layout->setColumnStretch(2, 100);
    int row(layout->rowCount());
    layout->addWidget(fileLabel, row, 0);
    layout->addWidget(m_fileNameLineEdit, row, 1, 1, 2);
    layout->addWidget(fileToolButton, row, 3);
    row = layout->rowCount();
    layout->addWidget(createModeSelectionWidget(), row, 0, 1, 2, Qt::AlignTop);
    layout->addWidget(createClassAndNameSelectionWidget(), row, 2, 1, -1, (Qt::AlignLeft | Qt::AlignLeft));

    
    setCentralWidget(widget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);

    m_modeClassRadioButton->setChecked(true);
    modeButtonGroupButtonClicked(m_modeButtonGroup->checkedButton());
}

/**
 * Destructor.
 */
ChooseBorderFocusFromFileDialog::~ChooseBorderFocusFromFileDialog()
{
}

/**
 * @return New instance of widget for selecting mode
 */
QWidget*
ChooseBorderFocusFromFileDialog::createModeSelectionWidget()
{
    m_modeClassRadioButton = new QRadioButton("Class");
    setToolTipText(m_modeClassRadioButton, ToolTip::MODE_CLASS_BUTTON);
    m_modeNameRadioButton  = new QRadioButton("Name");
    setToolTipText(m_modeNameRadioButton, ToolTip::MODE_NAME_BUTTON);
    
    m_modeButtonGroup = new QButtonGroup(this);
    m_modeButtonGroup->setExclusive(true);
    m_modeButtonGroup->addButton(m_modeClassRadioButton);
    m_modeButtonGroup->addButton(m_modeNameRadioButton);
    QObject::connect(m_modeButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                     this, &ChooseBorderFocusFromFileDialog::modeButtonGroupButtonClicked);
    
    QGroupBox* groupBox(new QGroupBox("Mode"));
    QVBoxLayout* layout(new QVBoxLayout(groupBox));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_modeClassRadioButton);
    layout->addWidget(m_modeNameRadioButton);
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    return groupBox;
}


/**
 * @return New instance of widget for selecting class and name
 */
QWidget*
ChooseBorderFocusFromFileDialog::createClassAndNameSelectionWidget()
{
    {
        /*
         * Class mode
         */
        QLabel* classLabel(new QLabel("Class: "));
        m_classWidgetClassSelectionComboBox = new QComboBox();
        setToolTipText(m_classWidgetClassSelectionComboBox, ToolTip::CLASS_SELECTION_CLASS_COMBO_BOX);
        QObject::connect(m_classWidgetClassSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                         this, &ChooseBorderFocusFromFileDialog::classWidgetClassComboBoxActivated);
        
        QLabel* nameLabel(new QLabel("Name: "));
        m_classWidgetNameSelectionComboBox  = new QComboBox();
        setToolTipText(m_classWidgetNameSelectionComboBox, ToolTip::CLASS_SELECTION_NAME_COMBO_BOX);
        
        m_classWidget = new QWidget();
        QGridLayout* classModeLayout(new QGridLayout(m_classWidget));
        classModeLayout->setColumnStretch(1, 100);
        classModeLayout->addWidget(classLabel, 0, 0);
        classModeLayout->addWidget(m_classWidgetClassSelectionComboBox, 0, 1);
        classModeLayout->addWidget(nameLabel, 1, 0);
        classModeLayout->addWidget(m_classWidgetNameSelectionComboBox, 1, 1);
    }

    {
        /*
         * Name mode
         */
        QLabel* classLabel(new QLabel("Class: "));
        m_nameWidgetClassSelectionComboBox = new QComboBox();
        setToolTipText(m_nameWidgetClassSelectionComboBox, ToolTip::NAME_SELECTION_CLASS_COMBO_BOX);
        
        QLabel* nameLabel(new QLabel("Name: "));
        m_nameWidgetNameSelectionComboBox  = new QComboBox();
        setToolTipText(m_nameWidgetNameSelectionComboBox, ToolTip::NAME_SELECTION_NAME_COMBO_BOX);
        QObject::connect(m_nameWidgetNameSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                         this, &ChooseBorderFocusFromFileDialog::nameWidgetNameComboBoxActivated);

        m_nameWidget = new QWidget();
        QGridLayout* classModeLayout(new QGridLayout(m_nameWidget));
        classModeLayout->setColumnStretch(1, 100);
        classModeLayout->addWidget(nameLabel, 0, 0);
        classModeLayout->addWidget(m_nameWidgetNameSelectionComboBox, 0, 1);
        classModeLayout->addWidget(classLabel, 1, 0);
        classModeLayout->addWidget(m_nameWidgetClassSelectionComboBox, 1, 1);
    }
    

    m_classOrNameStackedWidget = new QStackedWidget();
    m_classOrNameStackedWidget->addWidget(m_classWidget);
    m_classOrNameStackedWidget->addWidget(m_nameWidget);
    m_classOrNameStackedWidget->setSizePolicy(m_classOrNameStackedWidget->sizePolicy().horizontalPolicy(),
                                              QSizePolicy::Minimum);
    
    QGroupBox* groupBox(new QGroupBox("Selection"));
    QHBoxLayout* layout(new QHBoxLayout(groupBox));
    layout->addWidget(m_classOrNameStackedWidget);
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    return groupBox;

}


/**
 * Called when user clicks the OK button
 */
void
ChooseBorderFocusFromFileDialog::okButtonClicked()
{
    /*
     * Name must be non-empty
     */
    if (getSelectedName().isEmpty()) {
        WuQMessageBox::errorOk(this, "A name must be selected");
    }
    
    WuQDialogModal::okButtonClicked();
}

/**
 * @return The selected file name
 */
AString
ChooseBorderFocusFromFileDialog::getSelectedFileName() const
{
    return m_fileNameLineEdit->text().trimmed();
}

/**
 * @return The selected name or empty string if none selected
 */
AString
ChooseBorderFocusFromFileDialog::getSelectedName() const
{
    AString text;
    
    switch (m_selectionMode) {
        case SelectionMode::CLASS:
            text = m_classWidgetNameSelectionComboBox->currentText();
            break;
        case SelectionMode::NAME:
            text = m_nameWidgetNameSelectionComboBox->currentText();
            break;
    }
    
    return text;
}

/**
 * @return The selected class or empty string if none selected
 */
AString
ChooseBorderFocusFromFileDialog::getSelectedClass() const
{
    AString text;
    
    switch (m_selectionMode) {
        case SelectionMode::CLASS:
            text = m_classWidgetClassSelectionComboBox->currentText();
            break;
        case SelectionMode::NAME:
            text = m_nameWidgetClassSelectionComboBox->currentText();
            break;
    }
    
    return text;
}

/**
 * Set the selections to the given filename, class, and name
 * @param filename
 *    name of file
 * @param className
 *    name of class
 * @param name
 *    name of border or focus
 */
void
ChooseBorderFocusFromFileDialog::setSelections(const AString& filename,
                                               const AString& className,
                                               const AString& name)
{
    FileInformation fileInfo(filename);
    if (fileInfo.exists()) {
        if (loadFileName(filename)) {
            m_classWidgetClassSelectionComboBox->setCurrentText(className);
            classWidgetClassComboBoxActivated(m_classWidgetClassSelectionComboBox->currentIndex());
            m_classWidgetNameSelectionComboBox->setCurrentText(name);
            
            m_nameWidgetNameSelectionComboBox->setCurrentText(name);
            nameWidgetNameComboBoxActivated(m_nameWidgetNameSelectionComboBox->currentIndex());
            m_nameWidgetClassSelectionComboBox->setCurrentText(className);
        }
    }
}

/**
 * Called when class widget class combo box is selected
 * @param index
 *    Index of item selected
 */
void 
ChooseBorderFocusFromFileDialog::classWidgetClassComboBoxActivated(int index)
{
    if ((index >= 0)
        && (index < m_classWidgetClassSelectionComboBox->count())) {
        const QString className(m_classWidgetClassSelectionComboBox->currentText());
        switch (m_fileMode) {
            case FileMode::BORDER:
                CaretAssert(m_borderFile);
                loadComboBox(m_classWidgetNameSelectionComboBox,
                             m_borderFile->getAllBorderNamesThatUseClass(className));
                break;
            case FileMode::FOCUS:
                CaretAssert(m_fociFile);
                loadComboBox(m_classWidgetNameSelectionComboBox,
                             m_fociFile->getAllFociNamesThatUseClass(className));
                break;
        }
    }
}

/**
 * Called when name widget name combo box is selected
 * @param index
 *    Index of item selected
 */
void 
ChooseBorderFocusFromFileDialog::nameWidgetNameComboBoxActivated(int index)
{
    if ((index >= 0)
        && (index < m_nameWidgetNameSelectionComboBox->count())) {
        const QString name(m_nameWidgetNameSelectionComboBox->currentText());
        switch (m_fileMode) {
            case FileMode::BORDER:
                CaretAssert(m_borderFile);
                loadComboBox(m_nameWidgetClassSelectionComboBox,
                             m_borderFile->getAllClassesForBordersWithName(name));
                break;
            case FileMode::FOCUS:
                CaretAssert(m_fociFile);
                loadComboBox(m_nameWidgetClassSelectionComboBox,
                             m_fociFile->getAllClassesForFociWithName(name));
                break;
        }
    }

}


/**
 * Called to choose the border or focus file
 */
void
ChooseBorderFocusFromFileDialog::chooseFileActionTriggered()
{
    
    const AString filename(CaretFileDialog::getOpenFileName(this,
                                                            "Choose File",
                                                            QString(),
                                                            DataFileTypeEnum::toQFileDialogFilterForReading(m_dataFileType)));
    loadFileName(filename);
}

/**
 * Load the file with the given name
 * @param filename
 *    Name of file
 * @return True if file was successfully loaded
 */
bool
ChooseBorderFocusFromFileDialog::loadFileName(const AString& filename)
{
    bool successFlag(false);
    if ( ! filename.isEmpty()) {
        m_fileNameLineEdit->setText("");
        m_allNames.clear();
        m_allClassNames.clear();
        
        try {
            switch (m_fileMode) {
                case FileMode::BORDER:
                    m_borderFile.reset(new BorderFile());
                    m_borderFile->readFile(filename);
                    m_allNames = m_borderFile->getAllBorderNames();
                    m_allClassNames = m_borderFile->getAllBorderClasses();
                    break;
                case FileMode::FOCUS:
                    m_fociFile.reset(new FociFile());
                    m_fociFile->readFile(filename);
                    m_allNames = m_fociFile->getAllFociNames();
                    m_allClassNames = m_fociFile->getAllFociClasses();
                    break;
            }
            
            m_fileNameLineEdit->setText(filename);
            loadComboBox(m_nameWidgetNameSelectionComboBox,
                         m_allNames);
            loadComboBox(m_classWidgetClassSelectionComboBox,
                         m_allClassNames);
            successFlag = true;
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::errorOk(this, ("Error reading file: "
                                          + dfe.whatString()));
        }
    }
    
    m_modeClassRadioButton->setChecked(true);
    modeButtonGroupButtonClicked(m_modeClassRadioButton);
    
    return successFlag;
}


/**
 * Load combo box with the given items
 */
void
ChooseBorderFocusFromFileDialog::loadComboBox(QComboBox* comboBox,
                                              const std::vector<AString>& itemNames)
{
    comboBox->clear();
    CaretAssert(comboBox);
    for (const auto& s : itemNames) {
        comboBox->addItem(s);
    }
}

/**
 * Called when mode radio button is clicked
 * @param button
 *    Button that was clicked
 */
void
ChooseBorderFocusFromFileDialog::modeButtonGroupButtonClicked(QAbstractButton* button)
{
    if (button == m_modeClassRadioButton) {
        m_selectionMode = SelectionMode::CLASS;
    }
    else if (button == m_modeNameRadioButton) {
        m_selectionMode = SelectionMode::NAME;
    }
    else {
        CaretAssert(0);
    }
    
    switch (m_selectionMode) {
        case SelectionMode::CLASS:
            m_classOrNameStackedWidget->setCurrentWidget(m_classWidget);
            classWidgetClassComboBoxActivated(m_classWidgetClassSelectionComboBox->currentIndex());
            break;
        case SelectionMode::NAME:
            m_classOrNameStackedWidget->setCurrentWidget(m_nameWidget);
            nameWidgetNameComboBoxActivated(m_nameWidgetNameSelectionComboBox->currentIndex());
            break;
    }
}

/**
 * @return Set the tooltip text for the give tooltip enum to the given widget
 * @param widget
 *    Widget that has tooltip set
 * @param toolTip
 *    The tooltip enum
 */
void
ChooseBorderFocusFromFileDialog::setToolTipText(QWidget* widget,
                                                const ToolTip toolTip) const
{
    AString text;
    switch (toolTip) {
        case ToolTip::MODE_CLASS_BUTTON:
            text = ("Choose a class and then choose a XXX name from YYY that uses the chosen class");
            break;
        case ToolTip::MODE_NAME_BUTTON:
            text = ("Choose a XXX name and the class is set to the class used by the chosen XXX name");
            break;
        case ToolTip::CLASS_SELECTION_CLASS_COMBO_BOX:
            text = ("Choose a XXX class");
            break;
        case ToolTip::CLASS_SELECTION_NAME_COMBO_BOX:
            text = ("Choose a XXX name (choices are limited to YYY that use the chosen class)");
            break;
        case ToolTip::NAME_SELECTION_NAME_COMBO_BOX:
            text = ("Choose a XXX name");
            break;
        case ToolTip::NAME_SELECTION_CLASS_COMBO_BOX:
            text = ("Choose class (class is set to class used by chosen XXX name so there is usually just one choice available)");
            break;
    }

    switch (m_fileMode) {
        case FileMode::BORDER:
            text.replace("XXX", "border");
            text.replace("YYY", "borders");
            break;
        case FileMode::FOCUS:
            text.replace("XXX", "focus");
            text.replace("YYY", "foci");
            break;
    }
    
    text = WuQtUtilities::createWordWrappedToolTipText(text);
    
    widget->setToolTip(text);
}

