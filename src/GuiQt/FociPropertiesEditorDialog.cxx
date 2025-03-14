
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
#include "CaretFileDialog.h"
#include "DisplayPropertiesFoci.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "GiftiLabelTableSelectionComboBox.h"
#include "CaretLogger.h"
#include "GuiManager.h"
#include "SurfaceProjector.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::FociPropertiesEditorDialog
 * \brief dialog for Foci Properties Editor
 * \ingroup GuiQt
 */
    
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
        focus->setName(s_previousCreateFocus->getName());
        focus->setArea(s_previousCreateFocus->getArea());
        focus->setClassName(s_previousCreateFocus->getClassName());
        focus->setComment(s_previousCreateFocus->getComment());
        focus->setExtent(s_previousCreateFocus->getExtent());
        focus->setGeography(s_previousCreateFocus->getGeography());
        focus->setRegionOfInterest(s_previousCreateFocus->getRegionOfInterest());
        focus->setStatistic(s_previousCreateFocus->getStatistic());
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
        
        focusCreateDialog.updateGraphicsAndUserInterface();
        
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
    m_classComboBox = NULL;
    
    /*
     * File selection combo box
     */
    QLabel* fociFileLabel = new QLabel("File");
    m_fociFileSelectionComboBox = new QComboBox();
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
                                          "The class is used to group focuss with similar\n"
                                          "characteristics.  Controls are available to\n"
                                          "display focuss by their class attributes.");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Add/Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(displayClassEditorToolButton);
    
    /*
     * Coordinates
     */
    QLabel* coordLabel = new QLabel("XYZ (mm)");
    m_xCoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_xCoordSpinBox->setDecimals(2);
    m_xCoordSpinBox->setSingleStep(1.0);
    m_xCoordSpinBox->setRange(-10000000.0,
                              10000000.0);
    
    m_yCoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_yCoordSpinBox->setDecimals(2);
    m_yCoordSpinBox->setSingleStep(1.0);
    m_yCoordSpinBox->setRange(-10000000.0,
                              10000000.0);

    m_zCoordSpinBox = WuQFactory::newDoubleSpinBox();
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
    m_extentSpinBox = WuQFactory::newDoubleSpinBox();
    m_extentSpinBox->setDecimals(2);
    m_extentSpinBox->setSingleStep(1.0);
    m_extentSpinBox->setRange(-10000000.0,
                              10000000.0);
    
    QLabel* focusIDLabel = new QLabel("Focus ID");
    m_focusIDLineEdit = new QLineEdit();
    
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
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 4);
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
    gridLayout->addWidget(m_nameComboBox->getWidget(), row, 1, 1, 3);
    gridLayout->addWidget(displayNameColorEditorToolButton, row, 4);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(m_classComboBox->getWidget(), row, 1, 1, 3);
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
    gridLayout->addWidget(focusIDLabel, row, 0);
    gridLayout->addWidget(m_focusIDLineEdit, row, 1, 1, 1);
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
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    loadFociFileComboBox(fociFile);
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
                                                   QVariant::fromValue((void*)fociFile));
        if (selectedFociFile == fociFile) {
            defaultFileComboIndex = m_fociFileSelectionComboBox->count() - 1;
        }
    }
    
    if (numFociFiles > 0) {
        m_fociFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
        
        const FociFile* fociFile = getSelectedFociFile();
        if (fociFile != NULL) {
            m_nameCompleterStringList = fociFile->getAllFociNamesSorted();
            m_nameCompleterStringListModel->setStringList(m_nameCompleterStringList);
        }
    }
}

/**
 * Called to create a new focus file.
 */
void 
FociPropertiesEditorDialog::newFociFileButtonClicked()
{
    
    /*
     * Let user choose a different path/name
     */
    FociFile* newFociFile = new FociFile();
    GuiManager::get()->getBrain()->convertDataFilePathNameToAbsolutePathName(newFociFile);
    AString newFociFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::FOCI,
                                                                      this,
                                                                      "Choose Foci File Name",
                                                                      newFociFile->getFileName());
    /*
     * If user cancels, delete the new focus file and return
     */
    if (newFociFileName.isEmpty()) {
        delete newFociFile;
        return;
    }
    
    /*
     * Set name of new scene file
     */
    newFociFile->setFileName(newFociFileName);
    EventManager::get()->sendEvent(EventDataFileAdd(newFociFile).getPointer());
    loadFociFileComboBox(newFociFile);
    fociFileSelected();
}


/**
 * Called when a focus file is selected.
 */
void 
FociPropertiesEditorDialog::fociFileSelected()
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
FociPropertiesEditorDialog::loadClassComboBox(const QString& name)
{
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile != NULL) {
        m_classComboBox->updateContent(fociFile->getClassColorTable());
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
FociPropertiesEditorDialog::loadNameComboBox(const QString& name)
{
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile != NULL) {
        m_nameComboBox->updateContent(fociFile->getNameColorTable());
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
    const AString name = m_nameComboBox->getSelectedLabelName();
    if (name.isEmpty()) {
        errorMessage += ("Name is invalid.\n");
    }
    else {
        const int32_t unassignedNameKey = fociFile->getNameColorTable()->getUnassignedLabelKey();
        const int32_t selectedNameKey = m_nameComboBox->getSelectedLabelKey();
        if (selectedNameKey == unassignedNameKey) {
            errorMessage += "Choose or create a name for the focus.\n";
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
    
    /*
     * Copy data to the focus
     */
    loadFromDialogIntoFocusData(m_focus);

    /*
     * Project the focus
     */
    if (m_projectCheckBox->isChecked()) {
        Brain* brain = GuiManager::get()->getBrain();
        
        std::vector<const SurfaceFile*> surfaceFiles = brain->getPrimaryAnatomicalSurfaceFiles();
        
        if ( ! surfaceFiles.empty()) {
            try {
                SurfaceProjector projector(surfaceFiles);
                projector.projectFocus(0, m_focus);
            }
            catch (SurfaceProjectorException& spe) {
                CaretLogSevere(spe.whatString());
            }
        }
    }
    
    /*
     * Save project status
     */
    s_previousFociProjectSelected = m_projectCheckBox->isChecked();
    
    updateGraphicsAndUserInterface();
    
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
 * Update the graphics and user interface.
 */
void
FociPropertiesEditorDialog::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


/**
 * Load data from the given focus into the dialog.
 * @param focus
 *     Focus from which data is obtained.
 */
void
FociPropertiesEditorDialog::loadFromFocusDataIntoDialog(const Focus* focus)
{
    loadNameComboBox(focus->getName());
    loadClassComboBox(focus->getClassName());
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
    m_focusIDLineEdit->setText(focus->getFocusID());
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
    focus->setName(m_nameComboBox->getSelectedLabelName());
    focus->setClassName(m_classComboBox->getSelectedLabelName());
    const float xyz[3] = {
        (float)m_xCoordSpinBox->value(),
        (float)m_yCoordSpinBox->value(),
        (float)m_zCoordSpinBox->value()
    };
    CaretAssert(focus->getNumberOfProjections() > 0);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(m_commentTextEdit->toPlainText().trimmed());
    focus->setArea(m_areaLineEdit->text().trimmed());
    focus->setGeography(m_geographyLineEdit->text().trimmed());
    focus->setExtent(m_extentSpinBox->value());
    focus->setFocusID(m_focusIDLineEdit->text().trimmed());
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
                                 fociFile->getClassColorTable(),
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
}

/**
 * Display the name editor
 */
void
FociPropertiesEditorDialog::displayNameEditor()
{
    FociFile* fociFile = getSelectedFociFile();
    if (fociFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "Focus file is not valid, use the New button to create a focus file.");
        return;
    }
    
    GiftiLabelTableEditor editor(fociFile,
                                 fociFile->getNameColorTable(),
                                 "Edit Name Attributes",
                                 GiftiLabelTableEditor::OPTION_UNASSIGNED_LABEL_HIDDEN,
                                 this);
    const QString name = this->m_nameComboBox->getSelectedLabelName();
    if (name.isEmpty() == false) {
        editor.selectLabelWithName(name);
    }
    const int dialogResult = editor.exec();
    
    this->loadNameComboBox();
    
    if (dialogResult == GiftiLabelTableEditor::Accepted) {
        const QString selectedName = editor.getLastSelectedLabelName();
        if (selectedName.isEmpty() == false) {
            m_nameComboBox->setSelectedLabelName(selectedName);
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




