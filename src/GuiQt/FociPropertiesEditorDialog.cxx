
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

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>

#define __FOCI_PROPERTIES_EDITOR_DIALOG__DECLARE__
#include "FociPropertiesEditorDialog.h"
#undef __FOCI_PROPERTIES_EDITOR_DIALOG__DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "Focus.h"
#include "FociFile.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "CaretFileDialog.h"
#include "DisplayPropertiesFoci.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "CaretLogger.h"
#include "GuiManager.h"
#include "SurfaceProjector.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * Create (edit and add) a new focus.
 *
 * @param focus 
 *     The focus that will be displayed in the focus editor.
 *     If the user accepts (presses OK) to add the focus
 *     it will be added to the selected foci file.  If the
 *     user cancels, the focus will be destroyed.  Thus,
 *     the caller MUST NEVER access the focus after calling
 *     this static method.
 * @param browserTabContent
 *     Tab content on which focis are created.
 * @param parent
 *     Parent widget on which dialog is displayed.
 * @return  
 *     true if user pressed OK, else false.
 */
bool
FociPropertiesEditorDialog::createFocus(Focus* focus,
                                        BrowserTabContent* browserTabContent,
                                        QWidget* parent)
{
    CaretAssert(focus);
    
    if (s_previousCreateFocus != NULL) {
        if (focus->getName().isEmpty()) {
            focus->setName(s_previousCreateFocus->getName());
        }
        if (focus->getArea().isEmpty()) {
            focus->setArea(s_previousCreateFocus->getArea());
        }
        if (focus->getClassName().isEmpty()) {
            focus->setClassName(s_previousCreateFocus->getClassName());
        }
        if (focus->getComment().isEmpty()) {
            focus->setComment(s_previousCreateFocus->getComment());
        }
        if (focus->getExtent() == 0.0) {
            focus->setExtent(s_previousCreateFocus->getExtent());
        }
        if (focus->getGeography().isEmpty()) {
            focus->setGeography(s_previousCreateFocus->getGeography());
        }
        if (focus->getRegionOfInterest().isEmpty()) {
            focus->setRegionOfInterest(s_previousCreateFocus->getRegionOfInterest());
        }
        if (focus->getStatistic().isEmpty()) {
            focus->setStatistic(s_previousCreateFocus->getStatistic());
        }
    }
    
    FociPropertiesEditorDialog focusCreateDialog("Create Focus",
                                                 s_previousCreateFociFile,
                                                 focus,
                                                 true,
                                                 parent);
    if (focusCreateDialog.exec() == FociPropertiesEditorDialog::Accepted) {
        s_previousCreateFociFile = focusCreateDialog.getSelectedFociFile();
        if (s_previousCreateFocus == NULL) {
            s_previousCreateFocus = new Focus();
        }
        focusCreateDialog.loadFromDialogIntoFocusData(s_previousCreateFocus);
        s_previousCreateFociFile->addFocus(focus);
        
        if (browserTabContent != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            DisplayPropertiesFoci* dsf = GuiManager::get()->getBrain()->getDisplayPropertiesFoci();
            DisplayGroupEnum::Enum displayGroup = dsf->getDisplayGroupForTab(tabIndex);
            dsf->setDisplayed(displayGroup,
                              tabIndex,
                              true);
        }
        
        return true;
    }
    
    delete focus;
    return false;
}

/**
 * Edit an existing focus.
 *
 * @param focus
 *     The focus that will be displayed in the focus editor.
 *     If the user presses the OK button the focus will be
 *     updated.  Otherwise, no further action is taken.
 * @param parent
 *     Parent widget on which dialog is displayed.
 * @return
 *     true if user pressed OK, else false.
 */
bool
FociPropertiesEditorDialog::editFocus(FociFile* fociFile,
                                      Focus* focus,
                                      QWidget* parent)
{
    FociPropertiesEditorDialog fped("Edit Focus",
                                    fociFile,
                                    focus,
                                    false,
                                    parent);
    if (fped.exec() == FociPropertiesEditorDialog::Accepted) {
        return true;
    }
    
    return false;
}

/**
 * Delete all static members to eliminate reported memory leaks.
 */
void
FociPropertiesEditorDialog::deleteStaticMembers()
{
    if (s_previousCreateFocus != NULL) {
        delete s_previousCreateFocus;
        s_previousCreateFocus = NULL;
    }
}
/**
 * Constructor.
 */
FociPropertiesEditorDialog::FociPropertiesEditorDialog(const QString& title,
                                                       FociFile* fociFile,
                                                       Focus* focus,
                                                       const bool allowFociFileSelection,
                                                       QWidget* parent)
: WuQDialogModal(title,
                 parent)
{
    CaretAssert(focus);
    m_focus = focus;
    m_classNameComboBox = NULL;
    
    /*
     * File selection combo box
     */
    QLabel* fociFileLabel = new QLabel("File");
    m_fociFileSelectionComboBox = new QComboBox();
    loadFociFileComboBox(fociFile);
    WuQtUtilities::setToolTipAndStatusTip(m_fociFileSelectionComboBox, 
                                          "Selects an existing focus file\n"
                                          "to which new focus are added.");
    QObject::connect(m_fociFileSelectionComboBox, SIGNAL(activated(int)),
                     this, SLOT(fociFileSelected()));
    QAction* newFileAction = WuQtUtilities::createAction("New",
                                                         "Create a new focus file", 
                                                         this, 
                                                         this,
                                                         SLOT(newFociFileButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    
    /*
     * Name
     */
    QLabel* nameLabel = new QLabel("Name");
    m_nameLineEdit = new QLineEdit();
    
    /*
     * Color
     */
    QLabel* colorLabel = new QLabel("Color");
    m_colorSelectionComboBox = new CaretColorEnumComboBox(this,
                                                             CaretColorEnum::OPTION_INCLUDE_CLASS);
    WuQtUtilities::setToolTipAndStatusTip(m_colorSelectionComboBox->getWidget(), 
                                          "If the color is set to \"CLASS\", the focus is colored\n"
                                          "using the color associated with the focus's class.\n"
                                          "Otherwise, if a color name is selected, it is used\n"
                                          "to color the focus.");

    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    m_classNameComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(m_classNameComboBox, 
                                          "The class is used to group focuss with similar\n"
                                          "characteristics.  Controls are available to\n"
                                          "display focuss by their class attributes.");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    
    /*
     * Coordinates
     */
    QLabel* coordLabel = new QLabel("XYZ (mm)");
    m_xCoordSpinBox = new QDoubleSpinBox();
    m_xCoordSpinBox->setDecimals(2);
    m_xCoordSpinBox->setSingleStep(1.0);
    m_xCoordSpinBox->setRange(-10000000.0,
                              10000000.0);
    
    m_yCoordSpinBox = new QDoubleSpinBox();
    m_yCoordSpinBox->setDecimals(2);
    m_yCoordSpinBox->setSingleStep(1.0);
    m_yCoordSpinBox->setRange(-10000000.0,
                              10000000.0);

    m_zCoordSpinBox = new QDoubleSpinBox();
    m_zCoordSpinBox->setDecimals(2);
    m_zCoordSpinBox->setSingleStep(1.0);
    m_zCoordSpinBox->setRange(-10000000.0,
                              10000000.0);
    
    /*
     * Comment
     */
    QLabel* commentLabel = new QLabel("Comment");
    m_commentTextEdit = new QTextEdit();
    m_commentTextEdit->setFixedHeight(60);
    
    QLabel* areaLabel = new QLabel("Area");
    m_areaLineEdit = new QLineEdit();
    
    QLabel* geographyLabel = new QLabel("Geography");
    m_geographyLineEdit = new QLineEdit();
    
    QLabel* extentLabel = new QLabel("Extent");
    m_extentSpinBox = new QDoubleSpinBox();
    m_extentSpinBox->setDecimals(2);
    m_extentSpinBox->setSingleStep(1.0);
    m_extentSpinBox->setRange(-10000000.0,
                              10000000.0);
    
    QLabel* regionOfInterestLabel = new QLabel("ROI");
    m_regionOfInterestLineEdit = new QLineEdit();
    
    QLabel* statisticLabel = new QLabel("Statistic");
    m_statisticLineEdit = new QLineEdit();
    
    m_projectCheckBox = new QCheckBox("Project to Surface");
    m_projectCheckBox->setChecked(s_previousFociProjectSelected);
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 4);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 100);
    gridLayout->setColumnStretch(3, 100);
    gridLayout->setColumnStretch(4, 0);
    
    int row = 0;
    gridLayout->addWidget(fociFileLabel, row, 0);
    gridLayout->addWidget(m_fociFileSelectionComboBox, row, 1, 1, 3);
    gridLayout->addWidget(newFileToolButton, row, 4);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 5);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_nameLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(colorLabel, row, 0);
    gridLayout->addWidget(m_colorSelectionComboBox->getWidget(), row, 1, 1, 3);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(m_classNameComboBox, row, 1, 1, 3);
    gridLayout->addWidget(displayClassEditorToolButton, row, 4);
    row++;
    gridLayout->addWidget(coordLabel, row, 0);
    gridLayout->addWidget(m_xCoordSpinBox, row, 1);
    gridLayout->addWidget(m_yCoordSpinBox, row, 2);
    gridLayout->addWidget(m_zCoordSpinBox, row, 3);
    row++;
    gridLayout->addWidget(commentLabel, row, 0);
    gridLayout->addWidget(m_commentTextEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(areaLabel, row, 0);
    gridLayout->addWidget(m_areaLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(geographyLabel, row, 0);
    gridLayout->addWidget(m_geographyLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(extentLabel, row, 0);
    gridLayout->addWidget(m_extentSpinBox, row, 1, 1, 1);
    row++;
    gridLayout->addWidget(regionOfInterestLabel, row, 0);
    gridLayout->addWidget(m_regionOfInterestLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(statisticLabel, row, 0);
    gridLayout->addWidget(m_statisticLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 4);
    row++;
    gridLayout->addWidget(m_projectCheckBox, row, 0, 1, 4);
    row++;

    /*
     * Allow/Disallow file selection
     */
    fociFileLabel->setEnabled(allowFociFileSelection);
    m_fociFileSelectionComboBox->setEnabled(allowFociFileSelection);
    newFileToolButton->setEnabled(allowFociFileSelection);
    
    /*
     * Set the widget for the dialog.
     */
    setCentralWidget(widget);
    
    loadFromFocusDataIntoDialog(m_focus);
}

/**
 * Destructor.
 */
FociPropertiesEditorDialog::~FociPropertiesEditorDialog()
{
    
}

/**
 * Get the selected focus file.
 * @return FociFile or NULL if no focus file.
 */
FociFile* 
FociPropertiesEditorDialog::getSelectedFociFile()
{
    FociFile* fociFile = NULL;
    const int fileComboBoxIndex = m_fociFileSelectionComboBox->currentIndex();
    if (fileComboBoxIndex >= 0) {
        void* filePointer = m_fociFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
        fociFile = (FociFile*)filePointer;
    }
    
    return fociFile;
}

/**
 * Load the focus files into the focus file combo box.
 * @param fociFile
 *    Selected foci file, may be NULL.
 */
void 
FociPropertiesEditorDialog::loadFociFileComboBox(const FociFile* selectedFociFile)
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numFociFiles = brain->getNumberOfFociFiles();
    m_fociFileSelectionComboBox->clear();
    
    int defaultFileComboIndex = 0;
    for (int32_t i = 0; i < numFociFiles; i++) {
        FociFile* fociFile = brain->getFociFile(i);
        const AString name = fociFile->getFileNameNoPath();
        m_fociFileSelectionComboBox->addItem(name,
                                                   qVariantFromValue((void*)fociFile));
        if (selectedFociFile == fociFile) {
            defaultFileComboIndex = m_fociFileSelectionComboBox->count() - 1;
        }
    }
    if (numFociFiles > 0) {
        m_fociFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
    }
}

/**
 * Called to create a new focus file.
 */
void 
FociPropertiesEditorDialog::newFociFileButtonClicked()
{
    /*
     * Create a new focus file that will have proper path
     */
    Brain* brain = GuiManager::get()->getBrain();
    FociFile* newFociFile = brain->addFociFile();
    
    /*
     * Let user choose a different path/name
     */
    AString newFociFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::FOCI,
                                                                      this,
                                                                      "Choose Scene File Name",
                                                                      newFociFile->getFileName());
    /*
     * If user cancels, delete the new focus file and return
     */
    if (newFociFileName.isEmpty()) {
        brain->removeDataFile(newFociFile);
        return;
    }
    
    /*
     * Set name of new scene file
     */
    newFociFile->setFileName(newFociFileName);
    loadFociFileComboBox(newFociFile);
    fociFileSelected();
}


/**
 * Called when a focus file is selected.
 */
void 
FociPropertiesEditorDialog::fociFileSelected()
{
    if (m_classNameComboBox != NULL) {
        loadClassNameComboBox();
    }
}

/**
 * Load the class name combo box.
 */
void 
FociPropertiesEditorDialog::loadClassNameComboBox(const QString& className)
{
    AString selectedClassName = m_classNameComboBox->currentText();
    if (className.isEmpty() == false) {
        selectedClassName = className;
    }
    
    m_classNameComboBox->clear();
    
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile != NULL) {
        const GiftiLabelTable* classLabelTable = fociFile->getColorTable();
        std::vector<int32_t> keys = classLabelTable->getLabelKeysSortedByName();
        for (std::vector<int32_t>::iterator keyIterator = keys.begin();
             keyIterator != keys.end();
             keyIterator++) {
            const int32_t key = *keyIterator;
            const GiftiLabel* gl = classLabelTable->getLabel(key);
            
            m_classNameComboBox->addItem(gl->getName());
        }
    }
    const int previousClassIndex = m_classNameComboBox->findText(selectedClassName);
    if (previousClassIndex >= 0) {
        m_classNameComboBox->setCurrentIndex(previousClassIndex);
    }
}

/**
 * Called when the OK button is pressed.
 */
void 
FociPropertiesEditorDialog::okButtonClicked()
{
    AString errorMessage;
    
    /*
     * Get focus file.
     */
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Foci file is not valid, use the New button to create a foci file.");
        return;
    }
    
    /*
     * Get data entered by the user.
     */
    const AString name = m_nameLineEdit->text();
    if (name.isEmpty()) {
        errorMessage += ("Name is invalid.\n");
    }
    const QString className = m_classNameComboBox->currentText().trimmed();
    const CaretColorEnum::Enum color = m_colorSelectionComboBox->getSelectedColor();
    
    if (color == CaretColorEnum::CLASS) {
        if (className.isEmpty()) {
            errorMessage += ("Color is set to class but no class is selected.  "
                             "Either change the color or add a class using "
                             "the Edit button to the right of the class control.\n");
        }
    }
    
    if ((m_xCoordSpinBox->value() == 0.0)
        && (m_yCoordSpinBox->value() == 0.0)
        && (m_zCoordSpinBox->value() == 0.0)) {
        errorMessage += "Coordinates are invalid (all zeros)\n";
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
     * Copy data to the focus
     */
    loadFromDialogIntoFocusData(m_focus);

    /*
     * Project the focus
     */
    if (m_projectCheckBox->isChecked()) {
        Brain* brain = GuiManager::get()->getBrain();
        
        std::vector<const SurfaceFile*> surfaceFiles = brain->getVolumeInteractionSurfaceFiles();
        
        try {
            SurfaceProjector projector(surfaceFiles);
            projector.projectFocus(0, m_focus);
        }
        catch (SurfaceProjectorException& spe) {
            CaretLogSevere(spe.whatString());
        }
    }
    
    /*
     * Save project status
     */
    s_previousFociProjectSelected = m_projectCheckBox->isChecked();
    
    updateGraphicsAndUserInterface();
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonClicked();
}

/**
 * Update the graphics and user interface.
 */
void
FociPropertiesEditorDialog::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Load data from the given focus into the dialog.
 * @param focus
 *     Focus from which data is obtained.
 */
void
FociPropertiesEditorDialog::loadFromFocusDataIntoDialog(const Focus* focus)
{
    m_nameLineEdit->setText(focus->getName());
    m_colorSelectionComboBox->setSelectedColor(focus->getColor());
    loadClassNameComboBox(focus->getClassName());
    float xyz[3] = { 0.0, 0.0, 0.0 };
    if (focus->getNumberOfProjections() > 0) {
        focus->getProjection(0)->getStereotaxicXYZ(xyz);
    }
    m_xCoordSpinBox->setValue(xyz[0]);
    m_yCoordSpinBox->setValue(xyz[1]);
    m_zCoordSpinBox->setValue(xyz[2]);
    m_commentTextEdit->setText(focus->getComment());
    m_areaLineEdit->setText(focus->getArea());
    m_geographyLineEdit->setText(focus->getGeography());
    m_extentSpinBox->setValue(focus->getExtent());
    m_regionOfInterestLineEdit->setText(focus->getRegionOfInterest());
    m_statisticLineEdit->setText(focus->getStatistic());
}

/**
 * Load data from dialog into the focus.
 * @param focus
 *    Focus into which data is placed.
 */
void
FociPropertiesEditorDialog::loadFromDialogIntoFocusData(Focus* focus) const
{
    focus->setName(m_nameLineEdit->text().trimmed());
    focus->setColor(m_colorSelectionComboBox->getSelectedColor());
    focus->setClassName(m_classNameComboBox->currentText().trimmed());
    const float xyz[3] = {
        m_xCoordSpinBox->value(),
        m_yCoordSpinBox->value(),
        m_zCoordSpinBox->value()
    };
    CaretAssert(focus->getNumberOfProjections() > 0);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(m_commentTextEdit->toPlainText().trimmed());
    focus->setArea(m_areaLineEdit->text().trimmed());
    focus->setGeography(m_geographyLineEdit->text().trimmed());
    focus->setExtent(m_extentSpinBox->value());
    focus->setRegionOfInterest(m_regionOfInterestLineEdit->text().trimmed());
    focus->setStatistic(m_statisticLineEdit->text().trimmed());
}



/**
 * Display the class editor
 */
void 
FociPropertiesEditorDialog::displayClassEditor()
{
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Focus file is not valid, use the New button to create a focus file.");
        return;
    }
    
    GiftiLabelTableEditor editor(fociFile,
                                 "Edit Class Attributes",
                                 this);
    const QString className = m_classNameComboBox->currentText();
    if (className.isEmpty() == false) {
        editor.selectLabelWithName(className);
    }
    editor.exec();
    
    loadClassNameComboBox();

    const QString selectedClassName = editor.getLastSelectedLabelName();
    if (selectedClassName.isEmpty() == false) {
        const int indx = m_classNameComboBox->findText(selectedClassName);
        if (indx >= 0) {
            m_classNameComboBox->setCurrentIndex(indx);
        }
    }
}

/**
 * @return Is the project checkbox selected?
 */
bool
FociPropertiesEditorDialog::isProjectSelected()
{
    return m_projectCheckBox->isChecked();
}

/**
 * set the status of the project checkbox.
 * @param selected
 *    New status.
 */
void
FociPropertiesEditorDialog::setProjectSelected(const bool selected)
{
    m_projectCheckBox->setChecked(selected);
}




