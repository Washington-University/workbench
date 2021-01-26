
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

#include <cmath>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGroupBox>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTabWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#define __SCENE_DIALOG_DECLARE__
#include "SceneDialog.h"
#undef __SCENE_DIALOG_DECLARE__

#include "Annotation.h"
#include "AnnotationManager.h"
#include "ApplicationInformation.h"
#include "BalsaDatabaseUploadSceneFileDialog.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CursorDisplayScoped.h"
#include "CaretPreferences.h"
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventDataFileAdd.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventImageCapture.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventHelpViewerDisplay.h"
#include "EventUserInterfaceUpdate.h"
#include "EventShowDataFileReadWarningsDialog.h"
#include "EventSceneActive.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ProgressReportingDialog.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneCreateReplaceDialog.h"
#include "SceneFile.h"
#include "SceneFileInformationDialog.h"
#include "SceneInfo.h"
#include "ScenePreviewDialog.h"
#include "SceneReplaceAllDialog.h"
#include "SceneShowOptionsDialog.h"
#include "SessionManager.h"
#include "UsernamePasswordWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQDialogNonModal.h"
#include "WuQMessageBox.h"
#include "WuQTextEditorDialog.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"
#include "ZipSceneFileDialog.h"

using namespace caret;


    
/**
 * \class caret::SceneDialog 
 * \brief Dialog for manipulation of scenes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget.
 */
SceneDialog::SceneDialog(QWidget* parent)
: WuQDialogNonModal("Scenes",
                    parent)
{
    m_selectedSceneClassInfoIndex = -1;
    
    m_replaceAllScenesDescription = ("This operation may be slow as it loads and replaces all scenes.");
    m_testAllScenesDescription = ("This operation may be slow as it requires displaying all scenes. "
                                  "\n"
                                  "Each scene is displayed and an image of the graphic(s) regions is generated "
                                  "from the scene.  After all scenes have been loaded, a dialog is "
                                  "displayed that shows a scene's preview image on the left and "
                                  "on the right, the image created by loading the scene.  If the "
                                  "two images do not match, it indicates a problem with the scene.  "
                                  "Possible problems include data files missing, data files that "
                                  "have changed, or a change to the software.");
    m_testAllScenesDescription = WuQtUtilities::createWordWrappedToolTipText(m_testAllScenesDescription);
    
    /*
     * No apply buton and show help button
     */
    setApplyButtonText("");
    setStandardButtonText(QDialogButtonBox::Help,
                          "Help");
    
    
    /*
     * Icons
     */
    m_cautionIconValid = WuQtUtilities::loadIcon(":/SceneFileDialog/caution.png",
                                                 m_cautionIcon);
    
    /*
     * Set the dialog's widget
     */
    this->setCentralWidget(createScenesWidget(),
                           WuQDialog::SCROLL_AREA_NEVER);

    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
    
    setSaveWindowPositionForNextTime(true);
    
    /*
     * Update the dialog.
     */
    updateDialog();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);

    /*
     * Need to use a "processed" event listener for file read and reload events
     * so that our receiveEvent() method is called after the files have been 
     * read.
     */
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_DATA_FILE_ADD);
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_DATA_FILE_RELOAD);
    
    resize(900,
           700);
}

/**
 * Destructor.
 */
SceneDialog::~SceneDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Process a close event.
 *
 * @param event
 *     The close event.
 */
void
SceneDialog::closeEvent(QCloseEvent* event)
{
      WuQDialogNonModal::closeEvent(event);
}

/**
 * Called when the close button is clicked.
 */
void
SceneDialog::closeButtonClicked()
{
    /*
     * If there are any modified scene files, inform the user
     */
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numSceneFiles = brain->getNumberOfSceneFiles();
    int32_t numberOfModifiedSceneFiles = 0;
    for (int32_t i = 0; i < numSceneFiles; i++) {
        const SceneFile* sceneFile = brain->getSceneFile(i);
        if (sceneFile->isModified()) {
            numberOfModifiedSceneFiles++;
        }
    }
    
    bool allowToCloseFlag = true;
    
    if (numberOfModifiedSceneFiles > 0) {
        const AString msg("<html>There are modified scene file(s).  You can save modified scene files using File Menu->Save/Manage Files "
                          "or by selecting them on the Scene Dialog and using the Save/Save As buttons.<p>"
                          "Continue closing this Scene Dialog?");
        if ( ! WuQMessageBox::warningOkCancel(this, msg)) {
            allowToCloseFlag = false;
        }
    }
    
    if (allowToCloseFlag) {
        WuQDialogNonModal::closeButtonClicked();
    }
}

/**
 * Update the scene dialog.
 */
void
SceneDialog::updateDialog()
{
    loadSceneFileComboBox(NULL);
    loadScenesIntoDialog(NULL);
    
    updateSceneFileModifiedStatusLabel();
}

/**
 * Create the default scene file if 
 * there are any models and there are no
 * scene files.
 */
void
SceneDialog::createDefaultSceneFile()
{
    if (getSelectedSceneFile() == NULL) {
        EventModelGetAll allModelsEvent;
        EventManager::get()->sendEvent(allModelsEvent.getPointer());
        if ( ! allModelsEvent.getModels().empty()) {
            newSceneFileButtonClicked();
        }
    }
}


/**
 * Get the selected scene file.
 * @return SceneFile or NULL if no scene file.
 */
SceneFile* 
SceneDialog::getSelectedSceneFile()
{
    SceneFile* sceneFile = NULL;
    const int fileComboBoxIndex = m_sceneFileSelectionComboBox->currentIndex();
    if (fileComboBoxIndex >= 0) {
        void* filePointer = m_sceneFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
        sceneFile = (SceneFile*)filePointer;
    }
    
    return sceneFile;
}

/**
 * @return The selected scene.
 */
Scene*
SceneDialog::getSelectedScene()
{
    Scene* selectedScene = NULL;
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        int32_t numValidScenes = sceneFile->getNumberOfScenes();
        
        bool selectedSceneChanged = false;
        
        if (m_selectedSceneClassInfoIndex >= numValidScenes) {
            m_selectedSceneClassInfoIndex = numValidScenes - 1;
            
            selectedSceneChanged = true;
        }
        if (m_selectedSceneClassInfoIndex < 0) {
            if (numValidScenes > 0) {
                m_selectedSceneClassInfoIndex = 0;
                
                selectedSceneChanged = true;
            }
        }
        
        if ((m_selectedSceneClassInfoIndex >= 0)
            && (m_selectedSceneClassInfoIndex < numValidScenes)) {
            selectedScene = sceneFile->getSceneAtIndex(m_selectedSceneClassInfoIndex);
        }
        
        if (selectedSceneChanged) {
            highlightSceneAtIndex(m_selectedSceneClassInfoIndex);
        }
    }
    
    return selectedScene;
}

/**
 * Update the modified icons in the scene file combo box.
 */
void
SceneDialog::updateSceneFileComboBoxModifiedIcons()
{
    if ( ! m_cautionIconValid) {
        return;
    }
    
    m_sceneFileSelectionComboBox->blockSignals(true);
    
    const int32_t numItems = m_sceneFileSelectionComboBox->count();
    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
        void* filePointer = m_sceneFileSelectionComboBox->itemData(itemIndex).value<void*>();
        SceneFile* sceneFile = (SceneFile*)filePointer;
        CaretAssert(sceneFile);
        if (sceneFile->isModified()) {
            m_sceneFileSelectionComboBox->setItemIcon(itemIndex,
                                                      m_cautionIcon);
        }
        else {
            m_sceneFileSelectionComboBox->setItemIcon(itemIndex,
                                                      QIcon());
        }
    }
    
    m_sceneFileSelectionComboBox->blockSignals(false);
    /* comment */
}

/**
 * Load the scene files into the scene file combo box.
 *
 * @param selectedSceneFileIn
 *     Scene file that is added.
 */
void 
SceneDialog::loadSceneFileComboBox(SceneFile* selectedSceneFileIn)
{
    SceneFile* selectedSceneFile = selectedSceneFileIn;
    if (selectedSceneFile == NULL) {
        selectedSceneFile = getSelectedSceneFile();
    }
    
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numSceneFiles = brain->getNumberOfSceneFiles();
    m_sceneFileSelectionComboBox->clear();
    
    int defaultFileComboIndex = 0;
    for (int32_t i = 0; i < numSceneFiles; i++) {
        SceneFile* sceneFile = brain->getSceneFile(i);
        
        if (sceneFile == selectedSceneFile) {
            defaultFileComboIndex = i;
        }
        
        const AString name = sceneFile->getFileNameNoPath();
        if (sceneFile->isModified()
            && m_cautionIconValid) {
            m_sceneFileSelectionComboBox->addItem(m_cautionIcon,
                                                  name,
                                                  QVariant::fromValue((void*)sceneFile));
        }
        else {
            m_sceneFileSelectionComboBox->addItem(name,
                                                  QVariant::fromValue((void*)sceneFile));
        }
    }
    
    
    if (numSceneFiles > 0) {
        m_sceneFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
    }
    
    m_sceneFileButtonsGroup->setEnabled(getSelectedSceneFile() != NULL);

    updateSceneFileModifiedStatusLabel();
}

/**
 * Load the scene into the dialog.
 *
 * @param selectedSceneIn
 *     Scene to add.
 */
void 
SceneDialog::loadScenesIntoDialog(Scene* selectedSceneIn)
{
    SceneFile* sceneFile = getSelectedSceneFile();
    Scene* selectedScene = selectedSceneIn;
    if (selectedScene == NULL) {
        selectedScene = getSelectedScene();
    }
    
    EventSceneActive activeSceneEvent(EventSceneActive::MODE_GET);
    EventManager::get()->sendEvent(activeSceneEvent.getPointer());
    const Scene* activeScene = activeSceneEvent.getScene();
    
    for (std::vector<SceneClassInfoWidget*>::iterator iter = m_sceneClassInfoWidgets.begin();
         iter != m_sceneClassInfoWidgets.end();
         iter++) {
        SceneClassInfoWidget* sciw = *iter;
        sciw->blockSignals(true);
        sciw->updateContent(NULL,
                            -1,
                            false);
    }
    
    int32_t numberOfValidSceneInfoWidgets = 0;
    
    int32_t defaultIndex = -1;
    
    if (sceneFile != NULL) {
        const int32_t numScenes = sceneFile->getNumberOfScenes();
        for (int32_t i = 0; i < numScenes; i++) {
            Scene* scene = sceneFile->getSceneAtIndex(i);
            
            QByteArray imageByteArray;
            AString imageBytesFormat;
            scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                          imageBytesFormat);
            
            SceneClassInfoWidget* sciw = NULL;
            
            if (i >= static_cast<int32_t>(m_sceneClassInfoWidgets.size())) {
                sciw = new SceneClassInfoWidget();
                QObject::connect(sciw, SIGNAL(activated(int32_t)),
                                 this, SLOT(sceneActivated(int32_t)));
                QObject::connect(sciw, SIGNAL(highlighted(int32_t)),
                                 this, SLOT(sceneHighlighted(int32_t)));
                m_sceneClassInfoWidgets.push_back(sciw);
                m_sceneSelectionLayout->addWidget(sciw, 1);
            }
            else {
                sciw = m_sceneClassInfoWidgets[i];
            }
            
            const bool activeSceneFlag(scene == activeScene);
            sciw->updateContent(scene,
                                i,
                                activeSceneFlag);
            
            sciw->setBackgroundForSelected(i == 1);
            
            if (scene == selectedScene) {
                defaultIndex = i;
            }
            else if (defaultIndex < 0) {
                defaultIndex = i;
            }
        }
        numberOfValidSceneInfoWidgets = numScenes;
    }
    
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        bool visibilityStatus = false;
        if (i < numberOfValidSceneInfoWidgets) {
            visibilityStatus = true;
        }
        m_sceneClassInfoWidgets[i]->setVisible(visibilityStatus);
        m_sceneClassInfoWidgets[i]->blockSignals(false);
    }
    
    if (defaultIndex >= 0) {
        highlightSceneAtIndex(defaultIndex);
    }

    const bool validFile = (getSelectedSceneFile() != NULL);
    
    
    bool validScene = false;
    bool haveScenes = false;
    if (validFile) {
        const Scene* scene = getSelectedScene();
        if (scene != NULL) {
            validScene = true;
        }
        haveScenes = (sceneFile->getNumberOfScenes() > 0);
    }
    
    enableSceneMoveUpAndDownButtons();
    
    m_addNewScenePushButton->setEnabled(validFile);
    m_deleteScenePushButton->setEnabled(validScene);
    if (m_replaceAllScenesPushButton != NULL) {
        m_replaceAllScenesPushButton->setEnabled(haveScenes);
    }
    m_testScenesPushButton->setEnabled(haveScenes);
    m_insertNewScenePushButton->setEnabled(validScene);
    m_replaceScenePushButton->setEnabled(validScene);
    m_showScenePushButton->setEnabled(validScene);
    m_showSceneImagePreviewPushButton->setEnabled(validScene);
    m_showSceneOptionsPushButton->setEnabled(validScene);
}

/**
 * @return Number of valid scene info widgets.
 */
int32_t
SceneDialog::getNumberOfValidSceneInfoWidgets() const
{
    int32_t numValid = 0;
    
    for (auto ssiw : m_sceneClassInfoWidgets) {
        if (ssiw->isValid()) {
            numValid++;
        }
    }
    
    return numValid;
}

/**
 * Enable the move up/down buttons
 */
void
SceneDialog::enableSceneMoveUpAndDownButtons()
{
    const int32_t numberOfSceneInfoWidgets = getNumberOfValidSceneInfoWidgets();
    m_moveSceneUpPushButton->setEnabled(m_selectedSceneClassInfoIndex > 0);
    m_moveSceneDownPushButton->setEnabled((m_selectedSceneClassInfoIndex >= 0)
                                          && (m_selectedSceneClassInfoIndex < (numberOfSceneInfoWidgets - 1)));
    
}

/**
 * Highlight the scene at the given index.
 *
 * @param sceneIndex
 *     Index of scene to highlight.
 */
void
SceneDialog::highlightSceneAtIndex(const int32_t sceneIndex)
{
    bool sceneIndexValid = false;
    
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        SceneClassInfoWidget* sciw = m_sceneClassInfoWidgets[i];
        if (sciw->isValid()) {
            if (sciw->getSceneIndex() == sceneIndex) {
                sciw->setBackgroundForSelected(true);
                sceneIndexValid = true;
                m_selectedSceneClassInfoIndex = i;
                
                /*
                 * Ensure that the selected scene remains visible
                 * and do not alter value of horiztonal scroll bar
                 */
                const int horizValue = m_sceneSelectionScrollArea->horizontalScrollBar()->value();
                const int xMargin = 0;
                const int yMargin = 50;
                m_sceneSelectionScrollArea->ensureWidgetVisible(sciw,
                                                                xMargin,
                                                                yMargin);
                m_sceneSelectionScrollArea->horizontalScrollBar()->setSliderPosition(horizValue);
            }
            else {
                sciw->setBackgroundForSelected(false);
            }
        }
    }
    
    if ( ! sceneIndexValid) {
        m_selectedSceneClassInfoIndex = -1;
    }
    
    enableSceneMoveUpAndDownButtons();
}

/**
 * Highlight the given scene.
 *
 * @param scene
 *     Scene to highlight.
 */
void
SceneDialog::highlightScene(const Scene* scene)
{
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        SceneClassInfoWidget* sciw = m_sceneClassInfoWidgets[i];
        if (sciw->isValid()) {
            if (sciw->getScene() == scene) {
                highlightSceneAtIndex(sciw->getSceneIndex());
                return;
            }
        }
    }
}

/**
 * Called to open a scene file.
 */
void
SceneDialog::openSceneFileButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! warnIfSceneFileIsModified(ModifiedWarningType::OPEN_FILE_BUTTON)) {
        return;
    }
    
    /*
     * Let user choose a different path/name
     */
    AString openSceneFileName = CaretFileDialog::getOpenFileNameDialog(DataFileTypeEnum::SCENE,
                                                                       m_openSceneFilePushButton,
                                                                       "Open a Scene File",
                                                                       "");
    /*
     * If user cancels, return
     */
    if (openSceneFileName.isEmpty()) {
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    /*
     * Note that this dialog receives EventDataFileRead
     * as a post-processed event and will take care of
     * the dialog updates.
     */
    EventDataFileRead dataFileEvent(GuiManager::get()->getBrain());
    dataFileEvent.addDataFile(DataFileTypeEnum::SCENE, openSceneFileName);
    EventManager::get()->sendEvent(dataFileEvent.getPointer());
    
    EventManager::get()->sendEvent(EventShowDataFileReadWarningsDialog().getPointer());
}


/**
 * Called to create a new scene file.
 */
void 
SceneDialog::newSceneFileButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! warnIfSceneFileIsModified(ModifiedWarningType::NEW_FILE_BUTTON)) {
        return;
    }
    
    SceneFile* newSceneFile = new SceneFile();
    GuiManager::get()->getBrain()->convertDataFilePathNameToAbsolutePathName(newSceneFile);
    newSceneFile->clearModified();

    /*
     * Note that this dialog receives EventDataFileAdd
     * as a post-processed event and will take care of
     * the dialog updates.
     */
    EventManager::get()->sendEvent(EventDataFileAdd(newSceneFile).getPointer());
}

/**
 * Save the selected scene file.
 */
void
SceneDialog::saveSceneFileButtonClicked()
{
    saveSelectedSceneFile();
}

/**
 * Save the selected scene file with a new name.
 */
void SceneDialog::saveAsSceneFileButtonClicked()
{
    saveAsSelectedSceneFile();
}

/**
 * Save the selected scene file using the file' current name.
 *
 * @return True if file was saved else false and an error message is displayed.
 */
bool
SceneDialog::saveSelectedSceneFile()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_saveSceneFilePushButton, "There is no selected scene file to save");
        return false;
    }

    try {
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        Brain* brain = GuiManager::get()->getBrain();
        brain->writeDataFile(sceneFile);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(m_saveSceneFilePushButton, e.whatString());
        return false;
    }
    
    loadSceneFileComboBox(sceneFile);
    
    return true;
}

/**
 * Save the selected scene file with display of a file selection dialog using the file' current name.
 *
 * @return True if file was saved else false and an error message is displayed.
 */
bool
SceneDialog::saveAsSelectedSceneFile()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_saveAsSceneFilePushButton, "There is no selected scene file to save");
        return false;
    }
    
    const AString filename = CaretFileDialog::getSaveFileNameDialog(sceneFile->getDataFileType(),
                                                                    m_saveAsSceneFilePushButton,
                                                                    "Save Scene File As",
                                                                    sceneFile->getFileName());
    if (filename.isEmpty()) {
        return false;
    }
    
    try {
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        sceneFile->setFileName(filename);
        Brain* brain = GuiManager::get()->getBrain();
        brain->writeDataFile(sceneFile);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(m_saveAsSceneFilePushButton, e.whatString());
        return false;
    }
    
    loadSceneFileComboBox(sceneFile);
    
    return true;
}

/**
 * If the selected scene file is modified, warn the user and
 * suggest saving the file prior to continuing.
 *
 * @param warningType
 *     Type of warning.
 * @return
 *     True if the operation should continue, else false.
 */
bool
SceneDialog::warnIfSceneFileIsModified(const ModifiedWarningType warningType)
{
    const SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        return true;
    }
    if ( ! sceneFile->isModified()) {
        return true;
    }

    /**
     * Return true if warning is disabled for an operation
     */
    switch (warningType) {
        case ModifiedWarningType::CLOSE_DIALOG_BUTTON:
            break;
        case ModifiedWarningType::FILE_COMBO_BOX_CHANGE_FILE:
            return true;
            break;
        case ModifiedWarningType::NEW_FILE_BUTTON:
            return true;
            break;
        case ModifiedWarningType::OPEN_FILE_BUTTON:
            return true;
            break;
        case ModifiedWarningType::UPLOAD_FILE_BUTTON:
            break;
        case ModifiedWarningType::ZIP_FILE_BUTTON:
            break;
    }
    
    /*
     * Parent for message box dialogs
     */
    QWidget* messageBoxParent = NULL;
    switch (warningType) {
        case ModifiedWarningType::CLOSE_DIALOG_BUTTON:
            messageBoxParent = this;
            break;
        case ModifiedWarningType::FILE_COMBO_BOX_CHANGE_FILE:
            messageBoxParent = m_sceneFileSelectionComboBox;
            break;
        case ModifiedWarningType::NEW_FILE_BUTTON:
            messageBoxParent = m_newSceneFilePushButton;
            break;
        case ModifiedWarningType::OPEN_FILE_BUTTON:
            messageBoxParent = m_openSceneFilePushButton;
            break;
        case ModifiedWarningType::UPLOAD_FILE_BUTTON:
            messageBoxParent = m_uploadSceneFilePushButton;
            break;
        case ModifiedWarningType::ZIP_FILE_BUTTON:
            messageBoxParent = m_zipSceneFilePushButton;
            break;
    }
    CaretAssert(messageBoxParent);
    
    if (sceneFile->getNumberOfScenes() <= 0) {
        switch (warningType) {
            case ModifiedWarningType::CLOSE_DIALOG_BUTTON:
                return true;
                break;
            case ModifiedWarningType::FILE_COMBO_BOX_CHANGE_FILE:
                return true;
                break;
            case ModifiedWarningType::NEW_FILE_BUTTON:
                return true;
                break;
            case ModifiedWarningType::OPEN_FILE_BUTTON:
                return true;
                break;
            case ModifiedWarningType::UPLOAD_FILE_BUTTON:
                break;
            case ModifiedWarningType::ZIP_FILE_BUTTON:
                break;
        }
        
        WuQMessageBox::errorOk(messageBoxParent,
                               "There are no scenes in the scene file !");
        return false;
    }
    
    bool enableContinueButton = false;
    bool enableSaveButton     = sceneFile->exists();
    QMessageBox::Icon messageBoxIcon = QMessageBox::NoIcon;
    
    /*
     * Set up the informative text
     */
    AString saveText = ("Press the <B>Save</B> button to save the scene file with its current name ");
    AString saveAsText = ("Press the <B>Save As</B> button to save the scene file to a new name using a file dialog ");
    AString continueText = ("Press the <B>Continue</B> button to skip saving the scene file (you may save it at a later time) ");
    AString cancelText = ("Press the <B>Cancel</B> button to skip saving the scene file and ");
    switch (warningType) {
        case ModifiedWarningType::CLOSE_DIALOG_BUTTON:
            messageBoxIcon = QMessageBox::Warning;
            enableContinueButton = true;
            saveText.append("and close the scene dialog");
            saveAsText.append("and close the scene dialog");
            continueText.append("and close the scene dialog");
            cancelText.append("do not close the scene dialog");
            break;
        case ModifiedWarningType::FILE_COMBO_BOX_CHANGE_FILE:
            messageBoxIcon = QMessageBox::Warning;
            enableContinueButton = true;
            saveText.append("and change the selected scene file");
            saveAsText.append("and change the selected scene file");
            continueText.append("and change the selected scene file");
            cancelText.append("cancel changing the selected scene file");
            break;
        case ModifiedWarningType::NEW_FILE_BUTTON:
            messageBoxIcon = QMessageBox::Warning;
            enableContinueButton = true;
            saveText.append(" and create a new scene file");
            saveAsText.append(" and create a new scene file");
            continueText.append(" and create a new scene file");
            cancelText.append("cancel creation of a new scene file");
            break;
        case ModifiedWarningType::OPEN_FILE_BUTTON:
            messageBoxIcon = QMessageBox::Warning;
            enableContinueButton = true;
            saveText.append("and display a dialog for opening a scene file");
            saveAsText.append("and display a dialog for opening a scene file");
            continueText.append("and display a dialog for opening a scene file");
            cancelText.append("cancel opening a scene file");
            break;
        case ModifiedWarningType::UPLOAD_FILE_BUTTON:
            messageBoxIcon = QMessageBox::Warning;
            saveText.append("and continue uploading the scene file");
            saveAsText.append("and continue uploading the scene file");
            continueText.append("");
            cancelText.append("cancel uploading the scene file");
            break;
        case ModifiedWarningType::ZIP_FILE_BUTTON:
            messageBoxIcon = QMessageBox::Warning;
            saveText.append("and zip the scene file");
            saveAsText.append("and zip the scene file");
            continueText.append("and zip the scene file");
            cancelText.append("and cancel zipping the scene file");
            break;
    }
    
    AString informativeText("<html>");
    if (enableSaveButton) {
        informativeText.append(saveText + "<p>");
    }
    informativeText.append(saveAsText + "<p>");
    if (enableContinueButton) {
        informativeText.append(continueText + "<p>");
    }
    informativeText.append(cancelText + "<p>");
    informativeText.append("</html>");
    
    /*
     * Create and display the message box dialog
     */
    QMessageBox messageBox(messageBoxParent);
    messageBox.setText("The selected scene file has been modified but not saved: "
                       + sceneFile->getFileNameNoPath());
    messageBox.setInformativeText(informativeText);
    messageBox.setIcon(messageBoxIcon);
    QPushButton* saveButton = NULL;
    if (enableSaveButton) {
        saveButton = messageBox.addButton("Save", QMessageBox::AcceptRole);
    }
    QPushButton* saveAsButton   = messageBox.addButton("Save As...", QMessageBox::AcceptRole);
    QPushButton* cancelButton   = messageBox.addButton("Cancel", QMessageBox::NoRole);
    QPushButton* continueButton = NULL;
    if (enableContinueButton) {
        continueButton = messageBox.addButton("Continue", QMessageBox::AcceptRole);
    }
    
    if (saveButton != NULL) {
        messageBox.setDefaultButton(saveButton);
    }
    else {
        messageBox.setDefaultButton(saveAsButton);
    }
    
    messageBox.exec();

    QAbstractButton* button = messageBox.clickedButton();
    CaretAssert(button);

    bool successFlag = false;
    
    if (button == saveButton) {
        successFlag = saveSelectedSceneFile();
    }
    else if (button == saveAsButton) {
        successFlag = saveAsSelectedSceneFile();
        
    }
    else if (button == cancelButton) {
        successFlag = false;
        
    }
    else if (button == continueButton) {
        successFlag = true;
    }
    else {
        CaretAssertMessage(0, "Button clicked not handled " + button->text());
    }
    
    return successFlag;
}

/**
 * Display a message if there are missing files in the Scene File.
 * 
 * @param missingFilesMode
 *      The missing file mode.
 * @return
 *      True if there are no missing files, else false.
 */
bool
SceneDialog::warnIfMissingFilesInSceneFile(SceneFile* sceneFile,
                                           const MissingFilesMode missingFilesMode)
{
    CaretAssert(sceneFile);
    
    AString baseDirectoryName;
    std::vector<AString> missingFileNames;
    AString errorMessage;
    sceneFile->findBaseDirectoryForDataFiles(baseDirectoryName,
                                             missingFileNames,
                                             errorMessage);

    if (missingFileNames.empty()) {
        return true;
    }
    
    AString verbText;
    AString acceptButtonText;
    switch (missingFilesMode) {
        case MissingFilesMode::UPLOAD:
            acceptButtonText = "Upload";
            verbText      = "uploading to BALSA";
            break;
        case MissingFilesMode::ZIP:
            acceptButtonText = "Zip";
            verbText      = "zipping the scene file";
            break;
    }
    
    const AString labelText("Do you want to continue " + verbText + " ?");
    AString text;
    for (auto name : missingFileNames) {
        text.append(name + "\n");
    }

    WuQDataEntryDialog dialog("Data Files Not Found",
                              this,
                              false);
    dialog.setTextAtTop("These data files were not found but are used by scenes in the scene file.  Scenes may not display correctly.  "
                        + labelText,
                        true);
    dialog.addTextEdit("", text, true);
    dialog.setMinimumWidth(500);
    
    if (dialog.exec() == WuQDataEntryDialog::Accepted) {
        return true;
    }
    return false;
}


/**
 * Called when upload scene file is selected.
 */
void
SceneDialog::uploadSceneFileButtonClicked()
{
    if ( ! warnIfSceneFileIsModified(ModifiedWarningType::UPLOAD_FILE_BUTTON)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    
    if ( ! warnIfMissingFilesInSceneFile(sceneFile,
                                         MissingFilesMode::UPLOAD)) {
        return;
    }
    
    BalsaDatabaseUploadSceneFileDialog uploadDialog(sceneFile,
                                                    this);
    uploadDialog.exec();
    
    /* Scene ID may have been added and it is shown in info section for each scene */
    loadScenesIntoDialog(NULL);
    
    updateSceneFileModifiedStatusLabel();
}


/**
 * Called when zip scene file is selected.
 */
void
SceneDialog::zipSceneFileButtonClicked()
{
    if ( ! warnIfSceneFileIsModified(ModifiedWarningType::ZIP_FILE_BUTTON)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();

    if ( ! warnIfMissingFilesInSceneFile(sceneFile,
                                         MissingFilesMode::ZIP)) {
        return;
    }
    
    ZipSceneFileDialog zipDialog(sceneFile,
                                 this);
    zipDialog.exec();
    
    updateSceneFileModifiedStatusLabel();
}

/**
 * Called when a scene file is selected.
 *
 * @param index
 *     Index of item selected.
 */
void 
SceneDialog::sceneFileSelected(int /*index*/)
{
    loadScenesIntoDialog(NULL);
    
    updateSceneFileModifiedStatusLabel();
}

//Modified status for both scenes and _LIBCPP_POP_MACROS
//In Scene info panel indicate if scene is the active scene
//Add macro group to Scene
//Remove macro group from scene FILE
//List active scene file on macros Dialog

/**
 * Called when add new scene button clicked.
 */
void
SceneDialog::addNewSceneButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! checkForModifiedFiles(GuiManager::TEST_FOR_MODIFIED_FILES_EXCLUDING_PALETTES_MODE_FOR_SCENE_ADD,
                                 this)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* newScene = SceneCreateReplaceDialog::createNewScene(m_addNewScenePushButton,
                                                                   sceneFile);
        if (newScene != NULL) {
            s_informUserAboutScenesOnExitFlag = false;

            /*
             * Set the active scene
             */
            EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
            activeSceneEvent.setScene(newScene);
            EventManager::get()->sendEvent(activeSceneEvent.getPointer());
        }
        
        loadScenesIntoDialog(newScene);
    }
    
    updateSceneFileModifiedStatusLabel();

    /* Ensures macros dialog gets updated with active scene */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Called when insert new scene button clicked.
 */
void
SceneDialog::insertSceneButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! checkForModifiedFiles(GuiManager::TEST_FOR_MODIFIED_FILES_EXCLUDING_PALETTES_MODE_FOR_SCENE_ADD,
                                 this)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            Scene* newScene = SceneCreateReplaceDialog::createNewSceneInsertBeforeScene(m_insertNewScenePushButton,
                                                                                       sceneFile,
                                                                                       scene);
            if (newScene != NULL) {
                s_informUserAboutScenesOnExitFlag = false;

                /*
                 * Set the active scene
                 */
                EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
                activeSceneEvent.setScene(newScene);
                EventManager::get()->sendEvent(activeSceneEvent.getPointer());
            }
            
            loadScenesIntoDialog(newScene);
        }
    }
    
    updateSceneFileModifiedStatusLabel();

    /* Ensures macros dialog gets updated with active scene */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Get the image from the scene info.
 *
 * @param sceneInfo
 *     The scene info.
 * @return
 *     Point to image that caller must delete.  Will be NULL
 *     if image is not valid.
 */
QImage*
SceneDialog::getQImageFromSceneInfo(const SceneInfo* sceneInfo) const
{
    QImage* imageOut = NULL;
    
    QByteArray imageByteArray;
    AString imageBytesFormat;
    sceneInfo->getImageBytes(imageByteArray,
                             imageBytesFormat);
    
    if (imageByteArray.isEmpty()) {
        return new QImage();
    }
    
    ImageFile imageFile;
    imageFile.setImageFromByteArray(imageByteArray,
                                    imageBytesFormat);
    const QImage* image = imageFile.getAsQImage();
    if (image != NULL) {
        if (image->isNull()) {
            CaretLogSevere("Preview image is invalid (isNull)");
        }
        else {
            imageOut = new QImage(*image);
        }
    }

    return imageOut;
}

/**
 * Replace all scenes.
 */
void
SceneDialog::replaceAllScenesPushButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_replaceAllScenesPushButton, "No scene file is loaded.");
        return;
    }
    
    const int32_t numScenes = sceneFile->getNumberOfScenes();
    if (numScenes == 0) {
        WuQMessageBox::errorOk(m_replaceAllScenesPushButton, "Selected scene file contains no scenes.");
        return;
    }
    
    SceneReplaceAllDialog replaceDialog(m_replaceAllScenesDescription,
                                        m_replaceAllScenesPushButton);
    if (replaceDialog.exec() == SceneReplaceAllDialog::Rejected) {
        return;
    }
    
    AString username;
    AString password;
    
    /*
     * Check to see if any scenes need a login/password.
     * Same login/password is used for all scenes.
     */
    for (int32_t i = 0; i < numScenes; i++) {
        const Scene* scene = sceneFile->getSceneAtIndex(i);
        if (scene != NULL) {
            if (scene->hasFilesWithRemotePaths()) {
                const QString msg("This scene contains files that are on the network.  "
                                  "If accessing the files requires a username and "
                                  "password, enter it here.  Otherwise, remove any "
                                  "text from the username and password fields.");
                
                
                if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(m_replaceAllScenesPushButton,
                                                                           "Username and Password",
                                                                           msg,
                                                                           username,
                                                                           password)) {
                    CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                     password);
                    break;
                }
            }
        }
    }
    
    showWaitCursor();
    
    ProgressReportingDialog progressDialog("Replace All Scenes",
                                           "Starting...",
                                           m_testScenesPushButton);
    progressDialog.setEventReceivingEnabled(false);
    progressDialog.setMinimumDuration(0);
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(numScenes);
    progressDialog.setValue(1);
    progressDialog.setLabelText("Starting replacement...");
    progressDialog.repaint();
    progressDialog.setCursor(Qt::ArrowCursor);
    progressDialog.show();
    QApplication::processEvents();
    
    const int IMAGE_DISPLAY_WIDTH = 400;
    std::vector<AString> sceneNames;
    std::vector<AString> sceneErrors;
    std::vector<AString> sceneWarnings;
    std::vector<QImage> oldImages;
    std::vector<QImage> newImages;
    
    bool canceledFlag = false;
    for (int32_t i = 0; i < numScenes; i++) {
        if (progressDialog.wasCanceled()) {
            canceledFlag = true;
            break;
        }
        
        Scene* origScene = sceneFile->getSceneAtIndex(i);
        if (origScene != NULL) {
            progressDialog.setValue(i+1);
            progressDialog.setLabelText("Replacing "
                                        + AString::number(i + 1)
                                        + " of "
                                        + AString::number(numScenes)
                                        + ": "
                                        + origScene->getName()
                                        + "\n\nPressing the Cancel button will stop replacement after the next scene finishes loading.");
            progressDialog.repaint();
            progressDialog.show();
            QApplication::processEvents();
            
            if (origScene->hasFilesWithRemotePaths()) {
                CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                 password);
            }
            
            const QImage* imageFromScene = getQImageFromSceneInfo(origScene->getSceneInfo());
            if (imageFromScene != NULL) {
                if (imageFromScene->isNull()) {
                    oldImages.push_back(QImage());
                }
                else {
                    oldImages.push_back(imageFromScene->scaledToWidth(IMAGE_DISPLAY_WIDTH));
                }
                delete imageFromScene;
            }
            else {
                oldImages.push_back(QImage());
            }

            /*
             * Display the scene
             */
            AString errorMessage;
            AString warningMessage;
            SceneDialog::displayScenePrivateWithErrorMessage(sceneFile,
                                                              origScene,
                                                              false,
                                                              errorMessage,
                                                             warningMessage);
            sceneNames.push_back(origScene->getName());
            
            /*
             * Assume scene loads correctly 
             */
            Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
            Scene::setSceneBeingCreated(newScene);
            newScene->setName(origScene->getName());
            newScene->setDescription(origScene->getDescription());
            newScene->setBalsaSceneID(origScene->getBalsaSceneID());
            
            /*
             * Set the active scene
             */
            EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
            activeSceneEvent.setScene(newScene);
            EventManager::get()->sendEvent(activeSceneEvent.getPointer());
            
            /*
             * Process options
             */
            if (replaceDialog.isChangeSurfaceAnnotationOffsetToOffset()) {
                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                std::vector<Annotation*> annotations = annMan->getAllAnnotations();
                for (auto ann : annotations) {
                    ann->changeSurfaceSpaceToTangentOffset();
                }
            }
            
            const std::vector<int32_t> windowIndices = GuiManager::get()->getAllOpenBrainBrowserWindowIndices();
            
            /*
             * Get all browser tabs and only save transformations for tabs
             * that are valid.
             */
            std::vector<int32_t> tabIndices;
            EventBrowserTabGetAll getAllTabs;
            EventManager::get()->sendEvent(getAllTabs.getPointer());
            tabIndices = getAllTabs.getBrowserTabIndices();
            std::sort(tabIndices.begin(),
                      tabIndices.end());
            
            SceneAttributes* sceneAttributes = newScene->getAttributes();
            sceneAttributes->setSceneFileName(getSelectedSceneFile()->getFileName());
            sceneAttributes->setSceneName(origScene->getName());
            sceneAttributes->setIndicesOfTabsAndWindowsForSavingToScene(tabIndices,
                                                                        windowIndices);
            sceneAttributes->setSpecFileNameSavedToScene(true);
            sceneAttributes->setAllLoadedFilesSavedToScene(true);
            sceneAttributes->setModifiedPaletteSettingsSavedToScene(true);
            
            newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
                                                              "guiManager"));
            
            AString imageErrorMessage;
            SceneCreateReplaceDialog::addImageToScene(newScene,
                                                      imageErrorMessage);
            if ( ! imageErrorMessage.isEmpty()) {
                errorMessage.appendWithNewLine(imageErrorMessage);
            }
            
            getSelectedSceneFile()->replaceScene(newScene,
                                                 origScene);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());

            /* Ensures macros dialog gets updated with active scene */
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());

            loadScenesIntoDialog(newScene);
            const QImage* newImage = getQImageFromSceneInfo(newScene->getSceneInfo());
            if (newImage != NULL) {
                if (newImage->isNull()) {
                    newImages.push_back(QImage());
                }
                else {
                    newImages.push_back(newImage->scaledToWidth(IMAGE_DISPLAY_WIDTH));
                }
                delete newImage;
            }
            else {
                newImages.push_back(QImage());
            }
            sceneErrors.push_back(errorMessage);
            sceneWarnings.push_back(warningMessage);
        }
    }
    

    progressDialog.close();
    
    CaretAssert(sceneNames.size() == sceneErrors.size());
    CaretAssert(sceneNames.size() == oldImages.size());
    CaretAssert(sceneNames.size() == newImages.size());
    CaretAssert(sceneNames.size() == sceneWarnings.size());
    
    showNormalCursor();
    
    if (canceledFlag) {
        WuQMessageBox::warningOk(m_replaceAllScenesPushButton,
                                 "Replacment of scenes was canceled so scene file may contain both original and "
                                 "replaced scenes.  It may be best to reload the scene file or exit without saving "
                                 "the scene file.");
    }
    
    const int32_t numValidScenes = static_cast<int32_t>(sceneNames.size());
    if (numValidScenes <= 0) {
        WuQMessageBox::errorOk(m_replaceAllScenesPushButton,
                               "No scenes were loaded.");
        return;
    }
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    
    for (int32_t i = 0; i < numValidScenes; i++) {
        int row = gridLayout->rowCount();
        if (i > 0) {
            gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                                  row, 0, 1, 4);
            row++;
        }
        
        gridLayout->addWidget(new QLabel("Scene Name: " + sceneNames[i]),
                              row, 0, Qt::AlignLeft);
        
        if ( ! sceneErrors[i].isEmpty()) {
            QLabel* errorLabel = new QLabel("Scene Errors: " + sceneErrors[i]);
            errorLabel->setWordWrap(true);
            gridLayout->addWidget(errorLabel,
                                  row, 1, Qt::AlignLeft);
        }
        row++;
        
        if ( ! sceneWarnings[i].isEmpty()) {
            QLabel* warningLabel = new QLabel("Scene Warnings: " + sceneWarnings[i]);
            warningLabel->setWordWrap(true);
            gridLayout->addWidget(warningLabel,
                                  row, 1, Qt::AlignLeft);
        }
        row++;
        
        const QSizePolicy::Policy imageSizePolicy = QSizePolicy::Preferred;
        
        QLabel* sceneImageLabel = new QLabel("Image Invalid");
        if ( ! oldImages[i].isNull()) {
            sceneImageLabel->clear();
            sceneImageLabel->setPixmap(QPixmap::fromImage(oldImages[i]));
            sceneImageLabel->setSizePolicy(imageSizePolicy,
                                           imageSizePolicy);
        }
        gridLayout->addWidget(sceneImageLabel,
                              row, 0);
        
        QLabel* newImageLabel = new QLabel("Image Invalid");
        if ( ! newImages[i].isNull()) {
            newImageLabel->clear();
            newImageLabel->setPixmap(QPixmap::fromImage(newImages[i]));
            newImageLabel->setSizePolicy(imageSizePolicy,
                                           imageSizePolicy);
        }
        gridLayout->addWidget(newImageLabel,
                              row, 1);
    }
    
    const int numRows = gridLayout->rowCount();
    for (int32_t i = 0; i < numRows; i++) {
        gridLayout->setRowStretch(i, 0);
    }
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(gridLayout);
    
    WuQDialogNonModal* dialog = new WuQDialogNonModal("Replace All Scenes Comparison",
                                                      m_replaceAllScenesPushButton);
    dialog->setDeleteWhenClosed(true);
    dialog->setApplyButtonText("");
    dialog->setCentralWidget(dialogWidget,
                             WuQDialogNonModal::SCROLL_AREA_ALWAYS);
    dialog->show();
    
    WuQtUtilities::limitWindowSizePercentageOfMaximum(dialog,
                                                      100.0,
                                                      100.0);
    
    const int dialogWidth  = (IMAGE_DISPLAY_WIDTH * 2) + 50;
    const int dialogHeight = (IMAGE_DISPLAY_WIDTH * 2.5);
    WuQtUtilities::resizeWindow(dialog,
                                dialogWidth,
                                dialogHeight);
    
    dialog->show();
}

/**
 * Test all scenes by loading them and then
 * displaying images that are stored with the
 * scene and new image of the displayed scene.
 */
void
SceneDialog::testScenesPushButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_testScenesPushButton, "No scene file is loaded.");
        return;
    }
    
    
    const int32_t numScenes = sceneFile->getNumberOfScenes();
    if (numScenes == 0) {
        WuQMessageBox::errorOk(m_testScenesPushButton, "Selected scene file contains no scenes.");
        return;
    }
    
    if ( ! WuQMessageBox::warningOkCancel(m_testScenesPushButton,
                                       "Test All Scenes",
                                       m_testAllScenesDescription)) {
        return;
    }
    
    AString username;
    AString password;
    
    /*
     * Check to see if any scenes need a login/password.
     * Same login/password is used for all scenes.
     */
    for (int32_t i = 0; i < numScenes; i++) {
        const Scene* scene = sceneFile->getSceneAtIndex(i);
        if (scene != NULL) {
            if (scene->hasFilesWithRemotePaths()) {
                const QString msg("This scene contains files that are on the network.  "
                                  "If accessing the files requires a username and "
                                  "password, enter it here.  Otherwise, remove any "
                                  "text from the username and password fields.");
                
                
                if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(this,
                                                                           "Username and Password",
                                                                           msg,
                                                                           username,
                                                                           password)) {
                    CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                     password);
                    break;
                }
            }
        }
    }
    
    ProgressReportingDialog progressDialog("Test All Scenes",
                                           "Starting...",
                                           m_testScenesPushButton);
    progressDialog.setEventReceivingEnabled(false);
    progressDialog.setMinimumDuration(0);
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(numScenes);
    progressDialog.setValue(1);
    progressDialog.setLabelText("Starting testing...");
    progressDialog.repaint();
    progressDialog.setCursor(Qt::ArrowCursor);
    progressDialog.show();
    QApplication::processEvents();
    
    std::vector<AString> sceneNames;
    std::vector<AString> sceneErrors;
    std::vector<AString> sceneWarnings;
    std::vector<QImage> sceneImages;
    std::vector<QImage> newImages;
    
    const int IMAGE_DISPLAY_WIDTH = 500;
    for (int32_t i = 0; i < numScenes; i++) {
        if (progressDialog.wasCanceled()) {
            break;
        }
        
        Scene* origScene = sceneFile->getSceneAtIndex(i);
        if (origScene != NULL) {
            progressDialog.setValue(i+1);
            progressDialog.setLabelText("Testing "
                                        + AString::number(i + 1)
                                        + " of "
                                        + AString::number(numScenes)
                                        + ": "
                                        + origScene->getName()
                                        + "\n\nPressing the Cancel button will stop testing after the next scene finishes loading.");
            progressDialog.repaint();
            progressDialog.show();
            QApplication::processEvents();
            if (origScene->hasFilesWithRemotePaths()) {
                CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                 password);
            }
            
            AString errorMessage;
            AString warningMessage;
            SceneDialog::displayScenePrivateWithErrorMessage(sceneFile,
                                                             origScene,
                                                             false,
                                                             errorMessage,
                                                             warningMessage);
            /*
             * Set the active scene
             */
            EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
            activeSceneEvent.setScene(origScene);
            EventManager::get()->sendEvent(activeSceneEvent.getPointer());

            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());

            /* Ensures macros dialog gets updated with active scene */
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());

            /*
             * Always generate an image, even if the scene fails and failure may 
             * be a very minor error.
             */
            QImage image;
            AString imageErrorMessage;
            const bool validImage = SceneCreateReplaceDialog::createSceneImage(image,
                                                                               imageErrorMessage);
            if (validImage) {
                newImages.push_back(image.scaledToWidth(IMAGE_DISPLAY_WIDTH));
            }
            else {
                newImages.push_back(QImage());
                errorMessage.appendWithNewLine("Generate image error: " + imageErrorMessage);
            }
            
            sceneNames.push_back(origScene->getName());
            const QImage* imageFromScene = getQImageFromSceneInfo(origScene->getSceneInfo());
            if (imageFromScene != NULL) {
                if (imageFromScene->isNull()) {
                    sceneImages.push_back(QImage());
                }
                else {
                    sceneImages.push_back(imageFromScene->scaledToWidth(IMAGE_DISPLAY_WIDTH));
                }
                delete imageFromScene;
            }
            else {
                sceneImages.push_back(QImage());
            }
            
            sceneErrors.push_back(errorMessage);
            sceneWarnings.push_back(warningMessage);
        }
    }
    
    progressDialog.close();
    
    CaretAssert(sceneNames.size() == sceneErrors.size());
    CaretAssert(sceneNames.size() == sceneImages.size());
    CaretAssert(sceneNames.size() == sceneErrors.size());
    CaretAssert(sceneNames.size() == newImages.size());
    CaretAssert(sceneNames.size() == sceneWarnings.size());
    
    showNormalCursor();
    
    const int32_t numValidScenes = static_cast<int32_t>(sceneNames.size());
    if (numValidScenes <= 0) {
        WuQMessageBox::errorOk(m_testScenesPushButton,
                               "No scenes were loaded.");
        return;
    }
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 100);
    
    for (int32_t i = 0; i < numValidScenes; i++) {
        int row = gridLayout->rowCount();
        if (i > 0) {
            gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                                  row, 0, 1, 4);
            row = gridLayout->rowCount();
        }
        
        gridLayout->addWidget(new QLabel("Scene Name: " + sceneNames[i]),
                              row, 0, 1, 2, Qt::AlignLeft);
        
        row = gridLayout->rowCount();
        
        if ( ! sceneErrors[i].isEmpty()) {
            QLabel* errorLabel = new QLabel("Scene Errors: " + sceneErrors[i]);
            errorLabel->setWordWrap(true);
            gridLayout->addWidget(errorLabel,
                                  row, 0, 1, 2, Qt::AlignLeft);
            
            row = gridLayout->rowCount();
        }
        
        if ( ! sceneWarnings[i].isEmpty()) {
            QLabel* warningLabel = new QLabel("Scene Warnings: " + sceneWarnings[i]);
            warningLabel->setWordWrap(true);
            gridLayout->addWidget(warningLabel,
                                  row, 0, 1, 2, Qt::AlignLeft);
            
            row = gridLayout->rowCount();
        }
        const QSizePolicy::Policy imageSizePolicy = QSizePolicy::Preferred;
        
        QLabel* sceneImageLabel = new QLabel("Image Invalid");
        if ( ! sceneImages[i].isNull()) {
            sceneImageLabel->clear();
            sceneImageLabel->setPixmap(QPixmap::fromImage(sceneImages[i]));
            sceneImageLabel->setSizePolicy(imageSizePolicy,
                                           imageSizePolicy);
        }
        gridLayout->addWidget(sceneImageLabel,
                              row, 0);
        
        QLabel* newImageLabel = new QLabel("Image Invalid");
        if ( ! newImages[i].isNull()) {
            newImageLabel->clear();
            newImageLabel->setPixmap(QPixmap::fromImage(newImages[i]));
            newImageLabel->setSizePolicy(imageSizePolicy,
                                         imageSizePolicy);
        }
        gridLayout->addWidget(newImageLabel,
                              row, 1);
    }
    
    const int numRows = gridLayout->rowCount();
    for (int32_t i = 0; i < numRows; i++) {
        gridLayout->setRowStretch(i, 0);
    }
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addStretch();

    WuQDialogNonModal* dialog = new WuQDialogNonModal("Test All Scene Comparisons",
                                                      m_testScenesPushButton);
    dialog->setDeleteWhenClosed(true);
    dialog->setApplyButtonText("");
    dialog->setCentralWidget(dialogWidget,
                             WuQDialogNonModal::SCROLL_AREA_ALWAYS);
    dialog->show();
    WuQtUtilities::limitWindowSizePercentageOfMaximum(dialog,
                                                      100.0,
                                                      100.0);
    
    const int dialogWidth  = (IMAGE_DISPLAY_WIDTH * 2) + 50;
    const int dialogHeight = (IMAGE_DISPLAY_WIDTH * 2.5);
    WuQtUtilities::resizeWindow(dialog,
                                dialogWidth,
                                dialogHeight);
    
    dialog->show();
}


/**
 * Move the selected scene up.
 */
void
SceneDialog::moveSceneUpButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            sceneFile->moveScene(scene, -1);
            loadScenesIntoDialog(scene);
            
            updateSceneFileModifiedStatusLabel();
        }
    }
}

/**
 * Move the selected scene down.
 */
void
SceneDialog::moveSceneDownButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            sceneFile->moveScene(scene, 1);
            loadScenesIntoDialog(scene);
            
            updateSceneFileModifiedStatusLabel();
        }
    }
}

/**
 * Called when replace scene button clicked.
 */
void
SceneDialog::replaceSceneButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! checkForModifiedFiles(GuiManager::TEST_FOR_MODIFIED_FILES_EXCLUDING_PALETTES_MODE_FOR_SCENE_ADD,
                                 this)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            Scene* newScene = SceneCreateReplaceDialog::replaceExistingScene(m_addNewScenePushButton,
                                                                             sceneFile,
                                                                             scene);
            if (newScene != NULL) {
                s_informUserAboutScenesOnExitFlag = false;
                
                /*
                 * Set the active scene
                 */
                EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
                activeSceneEvent.setScene(newScene);
                EventManager::get()->sendEvent(activeSceneEvent.getPointer());
            }
            
            loadScenesIntoDialog(newScene);
            
            updateSceneFileModifiedStatusLabel();
        }
    }

    /* Ensures macros dialog gets updated with active scene */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Check to see if there are modified files.  If there are
 * allow the user to continue or cancel creation of the scene.
 *
 * param creatingSceneFlag
 *     When true scene is being created, else a scene is being shown
 * @return
 *     true if the scene should be created, otherwise false.
 */
bool
SceneDialog::checkForModifiedFiles(const GuiManager::TestModifiedMode testMode,
                                   QWidget* parent)
{
    bool creatingSceneFlag = false;
    switch (testMode) {
        case GuiManager::TEST_FOR_MODIFIED_FILES_EXCLUDING_PALETTES_MODE_FOR_SCENE_ADD:
            creatingSceneFlag = true;
            break;
        case GuiManager::TEST_FOR_MODIFIED_FILES_MODE_FOR_EXIT:
            break;
        case GuiManager::TEST_FOR_MODIFIED_FILES_MODE_FOR_SCENE_SHOW:
            break;
        case GuiManager::TEST_FOR_MODIFIED_FILES_PALETTE_ONLY_MODE_FOR_SCENE_ADD:
            break;
    }
    
    if (creatingSceneFlag) {
        EventModelGetAll allModelsEvent;
        EventManager::get()->sendEvent(allModelsEvent.getPointer());
        if (allModelsEvent.getModels().empty()) {
            const QString msg("No surfaces or volumes are loaded.  Continue creating scene?");
            if ( ! WuQMessageBox::warningYesNo(parent,
                                               msg)) {
                return false;
            }
        }
    }
    
    AString dialogMessage;
    AString modifiedFilesMessage;
    const bool haveModifiedFilesFlag = GuiManager::get()->testForModifiedFiles(testMode,
                                                                               dialogMessage,
                                                                               modifiedFilesMessage);

    bool result = true;
    
    if (haveModifiedFilesFlag) {
        QMessageBox warningDialog(QMessageBox::Warning,
                               "Warning",
                               dialogMessage,
                               QMessageBox::NoButton,
                               parent);
        warningDialog.setInformativeText(modifiedFilesMessage);
        
        QPushButton* yesButton = warningDialog.addButton("Yes", QMessageBox::AcceptRole);
        QPushButton* noButton = warningDialog.addButton("No", QMessageBox::RejectRole);
        
        warningDialog.setDefaultButton(noButton);
        warningDialog.setEscapeButton(noButton);
        
        warningDialog.exec();
        const QAbstractButton* clickedButton = warningDialog.clickedButton();
        if (clickedButton == yesButton) {
            result = true;
        }
        else if (clickedButton == noButton) {
            result = false;
        }
        else {
            CaretAssert(0);
        }
    }
    
    return result;
}

/**
 * Show the data file structure in a dialog.
 */
void
SceneDialog::showFileStructure()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    CaretAssert(sceneFile);
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();

    SceneFileInformationDialog* infoDialog = new SceneFileInformationDialog(sceneFile,
                                                                            this);
    infoDialog->setVisible(true);
    infoDialog->show();
    infoDialog->activateWindow();
    infoDialog->raise();
}

/**
 * @return The Scenes widget.
 */
QWidget* 
SceneDialog::createScenesWidget()
{
    QWidget* sceneFileWidget = createSceneFileWidget();
    
    /*
     * Scene controls
     */ 
    QLabel* sceneLabel = new QLabel(" Scenes");
    
    /*
     * Add new scene button
     */
    m_addNewScenePushButton = new QPushButton("Add...");
    m_addNewScenePushButton->setToolTip("Add a new scene to the END of the list");
    QObject::connect(m_addNewScenePushButton, SIGNAL(clicked()),
                     this, SLOT(addNewSceneButtonClicked()));
    
    /*
     * Delete new scene button
     */
    m_deleteScenePushButton = new QPushButton("Delete...");
    m_deleteScenePushButton->setToolTip("Delete the selected scene");
    QObject::connect(m_deleteScenePushButton, SIGNAL(clicked()),
                     this, SLOT(deleteSceneButtonClicked()));
    
    /*
     * Replace all scenes
     */
    m_replaceAllScenesPushButton = new QPushButton("Replace All...");
    m_replaceAllScenesPushButton->setToolTip(m_replaceAllScenesDescription);
    QObject::connect(m_replaceAllScenesPushButton, SIGNAL(clicked()),
                     this, SLOT(replaceAllScenesPushButtonClicked()));
    
    /*
     * Test all scenes
     */
    m_testScenesPushButton = new QPushButton("Test All...");
    m_testScenesPushButton->setToolTip(m_testAllScenesDescription);
    QObject::connect(m_testScenesPushButton, SIGNAL(clicked()),
                     this, SLOT(testScenesPushButtonClicked()));
    
    /*
     * Move scene up button
     */
    m_moveSceneUpPushButton = new QPushButton("Move Up");
    m_moveSceneUpPushButton->setToolTip("Move the selected scene up one position");
    QObject::connect(m_moveSceneUpPushButton, SIGNAL(clicked()),
                     this, SLOT(moveSceneUpButtonClicked()));
    
    /*
     * Move scene down button
     */
    m_moveSceneDownPushButton = new QPushButton("Move Down");
    m_moveSceneDownPushButton->setToolTip("Move the selected scene down one position");
    QObject::connect(m_moveSceneDownPushButton, SIGNAL(clicked()),
                     this, SLOT(moveSceneDownButtonClicked()));
    
    /*
     * Insert scene button
     */
    m_insertNewScenePushButton = new QPushButton("Insert...");
    m_insertNewScenePushButton->setToolTip("Insert a new scene ABOVE the selected scene in the list");
    QObject::connect(m_insertNewScenePushButton, SIGNAL(clicked()),
                     this, SLOT(insertSceneButtonClicked()));
    
    /*
     * Replace scene button
     */
    m_replaceScenePushButton = new QPushButton("Replace...");
    m_replaceScenePushButton->setToolTip("Replace the selected scene");
    QObject::connect(m_replaceScenePushButton, SIGNAL(clicked()),
                     this, SLOT(replaceSceneButtonClicked()));
    
    /*
     * Show new scene button
     */
    m_showScenePushButton = new QPushButton("Show");
    m_showScenePushButton->setToolTip("Show the selected scene");
    QObject::connect(m_showScenePushButton, SIGNAL(clicked()),
                     this, SLOT(showSceneButtonClicked()));
    
    /*
     * Show scene image button
     */
    m_showSceneImagePreviewPushButton = new QPushButton("Preview...");
    m_showSceneImagePreviewPushButton->setToolTip("Show larger preview image and full description\n"
                                                  "for the selected scene");
    QObject::connect(m_showSceneImagePreviewPushButton, SIGNAL(clicked()),
                     this, SLOT(showImagePreviewButtonClicked()));
    
    /*
     * Show scene options button
     */
    m_showSceneOptionsPushButton = new QPushButton("Options...");
    m_showSceneOptionsPushButton->setToolTip("Display a dialog for show scene options");
    QObject::connect(m_showSceneOptionsPushButton, &QPushButton::clicked,
                     this, &SceneDialog::showSceneOptionsButtonClicked);
    
    /*
     * Group for show buttons
     */
    QGroupBox* showGroupBox = new QGroupBox("Show Scene");
    showGroupBox->setFlat(true);
    showGroupBox->setSizePolicy(showGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* showGroupLayout = new QVBoxLayout(showGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(showGroupLayout, 2, 2);
    showGroupLayout->addWidget(m_showScenePushButton);
    showGroupLayout->addWidget(m_showSceneImagePreviewPushButton);
    showGroupLayout->addWidget(m_showSceneOptionsPushButton);
    
    /*
     * Group for create buttons
     */
    QGroupBox* createGroupBox = new QGroupBox("Create Scene");
    createGroupBox->setFlat(true);
    createGroupBox->setSizePolicy(createGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* createGroupLayout = new QVBoxLayout(createGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(createGroupLayout, 2, 2);
    createGroupLayout->addWidget(m_addNewScenePushButton);
    createGroupLayout->addWidget(m_insertNewScenePushButton);
    createGroupLayout->addWidget(m_replaceScenePushButton);

    /*
     * Group for test buttons
     */
    QGroupBox* testGroupBox = new QGroupBox("Testing");
    testGroupBox->setFlat(true);
    testGroupBox->setSizePolicy(testGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* testGroupLayout = new QVBoxLayout(testGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(testGroupLayout, 2, 2);
    if (m_replaceAllScenesPushButton != NULL) {
        testGroupLayout->addWidget(m_replaceAllScenesPushButton);
    }
    testGroupLayout->addWidget(m_testScenesPushButton);
    
    /*
     * Group for organize buttons
     */
    QGroupBox* organizeGroupBox = new QGroupBox("Selected Scene");
    organizeGroupBox->setFlat(true);
    organizeGroupBox->setSizePolicy(organizeGroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QVBoxLayout* organizeGroupLayout = new QVBoxLayout(organizeGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(organizeGroupLayout, 2, 2);
    organizeGroupLayout->addWidget(m_moveSceneUpPushButton);
    organizeGroupLayout->addWidget(m_moveSceneDownPushButton);
    organizeGroupLayout->addSpacing(5);
    organizeGroupLayout->addWidget(m_deleteScenePushButton);
    
    /*
     * Layout for scene buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(showGroupBox);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(createGroupBox);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(organizeGroupBox);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(testGroupBox);

    /*
     * Widget and layout containing the scene class info.
     */
    m_sceneSelectionLayout = new QVBoxLayout();
    m_sceneSelectionWidget = new QWidget();
    QVBoxLayout* sceneSelectionWidgetLayout = new QVBoxLayout(m_sceneSelectionWidget);
    sceneSelectionWidgetLayout->addLayout(m_sceneSelectionLayout);
    sceneSelectionWidgetLayout->addStretch();
    m_sceneSelectionScrollArea = new QScrollArea();
    m_sceneSelectionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sceneSelectionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sceneSelectionScrollArea->setWidget(m_sceneSelectionWidget);
    m_sceneSelectionScrollArea->setWidgetResizable(true);
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setColumnStretch(0,   0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2,   0);
    gridLayout->setSpacing(6);
    WuQtUtilities::setLayoutMargins(gridLayout, 0);
    int row = 0;
    gridLayout->addWidget(sceneFileWidget, row, 0, 1, 3);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3);
    row++;
    gridLayout->addWidget(sceneLabel, row, 0, (Qt::AlignTop | Qt::AlignRight));
    gridLayout->addWidget(m_sceneSelectionScrollArea, row, 1);
    gridLayout->addLayout(sceneButtonLayout, row, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3);
    row++;
    
    return widget;
}

/**
 * @return The Scene File Widget.
 */
QWidget*
SceneDialog::createSceneFileWidget()
{
    /*
     * Scene File Controls
     */
    QLabel* sceneFileLabel = new QLabel("Scene File");
    m_sceneFileSelectionComboBox = new QComboBox();
    m_sceneFileSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    m_sceneFileSelectionComboBox->setSizePolicy(QSizePolicy::MinimumExpanding,
                                                m_sceneFileSelectionComboBox->sizePolicy().verticalPolicy());
    WuQtUtilities::setToolTipAndStatusTip(m_sceneFileSelectionComboBox,
                                          "Selects an existing scene file\n"
                                          "to which new scenes are added.");
    QObject::connect(m_sceneFileSelectionComboBox, SIGNAL(activated(int)),
                     this, SLOT(sceneFileSelected(int)));
    
    /*
     * Scene file modified status
     */
    QLabel* modifiedLabel = new QLabel("Modified");
    m_sceneFileModifiedStatusLabel = new QLabel("");
    
    /*
     * File structure buttons
     */
    m_showFileStructurePushButton = new QPushButton("Show Files and Folders...");
    WuQtUtilities::setWordWrappedToolTip(m_showFileStructurePushButton,
                                         "In a dialog, show the organization of files and folders contained in the Scene File");
    QObject::connect(m_showFileStructurePushButton, &QPushButton::clicked,
                     this, &SceneDialog::showFileStructure);
    
    /*
     * New File button
     */
    m_newSceneFilePushButton = new QPushButton("New");
    m_newSceneFilePushButton->setToolTip("Create a new scene file");
    QObject::connect(m_newSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(newSceneFileButtonClicked()));
    
    /*
     * Open File button
     */
    m_openSceneFilePushButton = new QPushButton("Open...");
    m_openSceneFilePushButton->setToolTip("Open an existing scene file");
    QObject::connect(m_openSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(openSceneFileButtonClicked()));
    
    /*
     * Save File button
     */
    m_saveSceneFilePushButton = new QPushButton("Save");
    m_saveSceneFilePushButton->setToolTip("Save the scene file using its current name");
    QObject::connect(m_saveSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(saveSceneFileButtonClicked()));
    
    /*
     * Save As File button
     */
    m_saveAsSceneFilePushButton = new QPushButton("Save As...");
    m_saveAsSceneFilePushButton->setToolTip("Display a file selection dialog for saving scene file with a new name");
    QObject::connect(m_saveAsSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(saveAsSceneFileButtonClicked()));
    
    /*
     * Upload button
     */
    m_uploadSceneFilePushButton = new QPushButton("Upload...");
    m_uploadSceneFilePushButton->setToolTip("<html>Upload the selected scene file to the BALSA Database, balsa.wustl.edu</html>");
    QObject::connect(m_uploadSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(uploadSceneFileButtonClicked()));
    
    /*
     * Zip button
     */
    m_zipSceneFilePushButton = new QPushButton("Zip...");
    m_zipSceneFilePushButton->setToolTip("Zip the selected scene file");
    QObject::connect(m_zipSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(zipSceneFileButtonClicked()));
    
    /*
     * Group for scene file buttons that are only valid when a scene file is available
     */
    m_sceneFileButtonsGroup = new WuQWidgetObjectGroup(this);
    m_sceneFileButtonsGroup->add(m_saveSceneFilePushButton);
    m_sceneFileButtonsGroup->add(m_saveAsSceneFilePushButton);
    m_sceneFileButtonsGroup->add(m_uploadSceneFilePushButton);
    m_sceneFileButtonsGroup->add(m_zipSceneFilePushButton);
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    QMargins gridMargins = gridLayout->contentsMargins();
    gridMargins.setBottom(0);
    gridMargins.setTop(3);
    gridLayout->setContentsMargins(gridMargins);
    gridLayout->setSpacing(6);
    gridLayout->setColumnStretch(0,   0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2,   0);
    gridLayout->setColumnStretch(3,   0);
    gridLayout->setColumnStretch(4,   0);
    gridLayout->setColumnStretch(5,   0);
    int row = 0;
    gridLayout->addWidget(sceneFileLabel,                 row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_sceneFileSelectionComboBox,   row, 1, 1, 2);
    gridLayout->addWidget(m_newSceneFilePushButton,       row, 3);
    gridLayout->addWidget(m_saveSceneFilePushButton,      row, 4);
    gridLayout->addWidget(m_zipSceneFilePushButton,       row, 5);
    row++;
    gridLayout->addWidget(modifiedLabel,                  row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_sceneFileModifiedStatusLabel, row, 1, Qt::AlignLeft);
    gridLayout->addWidget(m_showFileStructurePushButton,  row, 2);
    gridLayout->addWidget(m_openSceneFilePushButton,      row, 3);
    gridLayout->addWidget(m_saveAsSceneFilePushButton,    row, 4);
    gridLayout->addWidget(m_uploadSceneFilePushButton,    row, 5);
    row++;
    
    return widget;
}

/**
 * Gets called to verify the content of the scene creation dialog.
 * @param sceneCreateDialog
 *    Scene creation dialog.
 */
void 
SceneDialog::validateContentOfCreateSceneDialog(WuQDataEntryDialog* sceneCreateDialog)
{
    CaretAssert(sceneCreateDialog);
    
    bool dataValid = true;
    QString errorMessage = "";
    
    QWidget* sceneNameWidget = sceneCreateDialog->getDataWidgetWithName("sceneNameLineEdit");
    if (sceneNameWidget != NULL) {
        QLineEdit* sceneNameLineEdit = qobject_cast<QLineEdit*>(sceneNameWidget);
        const AString sceneName = sceneNameLineEdit->text().trimmed();
        if (sceneName.isEmpty()) {
            dataValid = false;
            if (errorMessage.isEmpty() == false) {
                errorMessage += "\n";
            }
            errorMessage += ("Scene Name is empty.");
        }
        else {
            SceneFile* sceneFile = getSelectedSceneFile();
            if (sceneFile != NULL) {
                const int32_t numScenes = sceneFile->getNumberOfScenes();
                for (int32_t i = 0; i < numScenes; i++) {
                    Scene* scene = sceneFile->getSceneAtIndex(i);
                    if (scene->getName() == sceneName) {
                        dataValid = false;
                        if (errorMessage.isEmpty() == false) {
                            errorMessage += "\n";
                        }
                        errorMessage += ("An existing scene uses the name \""
                                         + sceneName
                                         + "\".  Scene names must be unique.");
                        break;
                    }
                }
            }
        }
    }
    
    sceneCreateDialog->setDataValid(dataValid, 
                                    errorMessage);
}

/**
 * Slot that get called when a scene is highlighted (clicked).
 *
 * @param sceneIndex
 *    Index of the scene.
 */
void
SceneDialog::sceneHighlighted(const int32_t sceneIndex)
{
    highlightSceneAtIndex(sceneIndex);
}

/**
 * Slot that get called when a scene is activated (double clicked).
 * 
 * @param sceneIndex
 *    Index of the scene.
 */
void
SceneDialog::sceneActivated(const int32_t sceneIndex)
{
    highlightSceneAtIndex(sceneIndex);
    showSceneButtonClicked();
}


/**
 * Called when delete scene button clicked.
 */
void 
SceneDialog::deleteSceneButtonClicked()
{
    Scene* scene = getSelectedScene();
    if (scene != NULL) {
        const AString sceneName = scene->getName();
        const AString msg = ("Are you sure you want to delete scene named: "
                             + sceneName);
        if (WuQMessageBox::warningYesNo(m_deleteScenePushButton,
                                        msg)) {
            SceneFile* sceneFile = getSelectedSceneFile();
            sceneFile->removeScene(scene);
            updateDialog();

            /* Ensures macros dialog gets updated with active scene */
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        }
    }
}

/**
 * Called when show scene button clicked.
 */
void 
SceneDialog::showSceneButtonClicked()
{
    /*
     * Any clicks on the button are blocked until this method
     * returns to prevent 'double clicks" that call this
     * method a second time before the first time completes.
     */
    QSignalBlocker blocker(m_showScenePushButton);
    
    if ( ! checkForModifiedFiles(GuiManager::TEST_FOR_MODIFIED_FILES_MODE_FOR_SCENE_SHOW,
                                 this)) {
        return;
    }
    
    AString sceneFileName;
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        sceneFileName = sceneFile->getFileName();
    }
    Scene* scene = getSelectedScene();
    if (scene != NULL) {
        if (scene->hasFilesWithRemotePaths()) {
            const QString msg("This scene contains files that are on the network.  "
                              "If accessing the files requires a username and "
                              "password, enter it here.  Otherwise, remove any "
                              "text from the username and password fields.");
            
            AString username;
            AString password;
            
            if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(this,
                                                                       "Username and Password",
                                                                       msg,
                                                                       username,
                                                                       password)) {
                CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                 password);
            }
            else {
                return;
            }
        }
        
        ProgressReportingDialog progressDialog(("Restoring Scene " + scene->getName()),
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        displayScenePrivateWithErrorMessageDialog(this,
                                                  sceneFile,
                                                  scene,
                                                  false);
    }

    /* Ensures macros dialog gets updated with active scene */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Display the scene show options dialog
 */
void
SceneDialog::showSceneOptionsButtonClicked()
{
    Scene* scene = getSelectedScene();
    if (scene != NULL) {
        SceneShowOptionsDialog::Options options(s_useSceneForegroundBackgroundColorsFlag);
        SceneShowOptionsDialog optionsDialog(options,
                                             m_showSceneOptionsPushButton);
        if (optionsDialog.exec() == QDialog::Accepted) {
            const SceneShowOptionsDialog::Options optionsResult = optionsDialog.getOptions();
            s_useSceneForegroundBackgroundColorsFlag = optionsResult.isUseSceneColorsSelected();

            if ( ! optionsResult.isUseSceneColorsSelected()) {
                CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
                prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
        }
    }
}

/**
 * Called when show image preview button is clicked.
 */
void
SceneDialog::showImagePreviewButtonClicked()
{
    const Scene* scene = getSelectedScene();
    if (scene != NULL) {
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                             imageBytesFormat);
        
        bool useNonModalDialogFlag = true;
        if (useNonModalDialogFlag) {
            /*
             * Scene preview dialog will be deleted when user closes it
             */
            ScenePreviewDialog* spd = new ScenePreviewDialog(scene,
                                                             m_showSceneImagePreviewPushButton);
            spd->show();
        }
        else {
            WuQDataEntryDialog ded(scene->getName(),
                                   m_showSceneImagePreviewPushButton,
                                   WuQDialog::SCROLL_AREA_AS_NEEDED);
            ded.setCancelButtonText("");
            ded.setOkButtonText("Close");
            
            try {
                if (imageByteArray.length() > 0) {
                    ImageFile imageFile;
                    imageFile.setImageFromByteArray(imageByteArray,
                                                    imageBytesFormat);
                    const QImage* image = imageFile.getAsQImage();
                    if (image != NULL) {
                        if (image->isNull()) {
                            CaretLogSevere("Preview image is invalid (isNull)");
                        }
                        else {
                            QLabel* imageLabel = new QLabel();
                            imageLabel->setPixmap(QPixmap::fromImage(*image));
                            ded.addWidget("",
                                          imageLabel);
                        }
                    }
                }
                
            }
            catch (const DataFileException& dfe) {
                CaretLogSevere("Converting preview to image: "
                               + dfe.whatString());
            }
            
            AString nameText;
            AString sceneIdText;
            AString abbreviatedDescriptionText;
            AString fullDescriptionText;
            SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                             -1,
                                                                             nameText,
                                                                             sceneIdText,
                                                                             abbreviatedDescriptionText,
                                                                             fullDescriptionText);
            QLabel* nameLabel = new QLabel(nameText);
            ded.addWidget("",
                          nameLabel);
            
            QLabel* sceneIdLabel = new QLabel(sceneIdText);
            ded.addWidget("",
                          sceneIdLabel);
            
            if (! fullDescriptionText.isEmpty()) {
                QLabel* descriptionLabel = new QLabel(fullDescriptionText);
                descriptionLabel->setWordWrap(true);
                ded.addWidget("",
                              descriptionLabel);
            }
            
            ded.exec();
        }
    }
}

/**
 * Gets called when the use scene colors checkbox is clicked.
 *
 * @param checked
 *     New checked status.
 */
void
SceneDialog::useSceneColorsCheckBoxClicked(bool checked)
{
    if ( ! checked) {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Display the given scene from the given scene file.
 * @param sceneFile
 *     Scene file.
 * @param scene
 *     Scene that is displayed.
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displayScene(SceneFile* sceneFile,
                          Scene* scene)
{
    const bool isSuccessful = displayScenePrivateWithErrorMessageDialog(this,
                                                                        sceneFile,
                                                                        scene,
                                                                        true);
    loadSceneFileComboBox(sceneFile);
    loadScenesIntoDialog(scene);
    
    /* Ensures macros dialog gets updated with active scene */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    return isSuccessful;
}

/**
 * Display the given scene from the given scene file.
 * If the scene fails to load, an error message is displayed
 * in a dialog.
 *
 * @param dialogParent
 *     Parent for error message dialog
 * @param sceneFile
 *     Scene file.
 * @param scene
 *     Scene that is displayed.
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displaySceneWithErrorMessageDialog(QWidget* dialogParent,
                                                SceneFile* sceneFile,
                                                Scene* scene)
{
    const bool showWaitCursorFlag(true);
    const bool flag = SceneDialog::displayScenePrivateWithErrorMessageDialog(dialogParent,
                                                                             sceneFile,
                                                                             scene,
                                                                             showWaitCursorFlag);
    return flag;
}

/**
 * Display the given scene from the given scene file.
 * If the scene fails to load, an error message is displayed
 * in a dialog.
 *
 * @param dialogParent
 *     Parent for error message dialog
 * @param sceneFile
 *     Scene file.
 * @param scene
 *     Scene that is displayed.
 * @param showWaitCursor
 *     If true, show a wait cursor
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displayScenePrivateWithErrorMessageDialog(QWidget* dialogParent,
                                                       SceneFile* sceneFile,
                                                       Scene* scene,
                                                       const bool showWaitCursor)
{
    CaretAssert(scene);
    AString errorMessage;
    AString warningMessage;
    
    ElapsedTimer et;
    et.start();
    
    const bool successFlag = displayScenePrivateWithErrorMessage(sceneFile,
                                                                 scene,
                                                                 showWaitCursor,
                                                                 errorMessage,
                                                                 warningMessage);
    AString msg = (AString::number(et.getElapsedTimeSeconds())
                   + " seconds to read Scene "
                   + scene->getName());
    CaretLogInfo(msg);
    
    if ( ! successFlag) {
        WuQMessageBox::errorDetailedTextOk(dialogParent,
                                           ("Error reading scene: "
                                            + scene->getName()),
                                           errorMessage);
    }
    
    if ( ! warningMessage.isEmpty()) {
        warningMessage.replace("\n", "<br>");
        warningMessage.insert(0, "<html><body>");
        warningMessage.append("</body></html>");
        WuQTextEditorDialog::runNonModal("Scene Warnings",
                                         warningMessage,
                                         WuQTextEditorDialog::TextMode::HTML,
                                         WuQTextEditorDialog::WrapMode::YES,
                                         dialogParent);
    }
    
    EventManager::get()->sendEvent(EventShowDataFileReadWarningsDialog().getPointer());
    
    return successFlag;
}

/**
 * Display the given scene from the given scene file.
 * @param sceneFile
 *     Scene file.
 * @param scene
 *     Scene that is displayed.
 * param showWaitCursor
 *     Show a wait cursor while loading scene
 * @param errorMessageOut
 *     Contains error information if scene fails to load.
 * @param warningMessageOut
 *     Contains non-fatal warnings encountered while loading scene
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displayScenePrivateWithErrorMessage(SceneFile* sceneFile,
                                                 Scene* scene,
                                                 const bool showWaitCursor,
                                                 AString& errorMessageOut,
                                                 AString& warningMessageOut)
{
    CaretAssert(sceneFile);
    CaretAssert(scene);
    
    errorMessageOut.clear();
    warningMessageOut.clear();
    
    const AString sceneFileName = sceneFile->getFileName();
    
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        errorMessageOut = ("Top level scene class should be guiManager but it is: "
                           + guiManagerClass->getName());
        return false;
    }
    
    /*
     * When compiled in debug mode, log any SceneObject and 
     * subclasses that fail to restore
     */
    bool logObjectFailedToRestoreFlag = true;
#ifdef NDEBUG
    logObjectFailedToRestoreFlag = false;
#endif // NDEBUG
    std::vector<SceneObject*> allSceneObjects;
    if (logObjectFailedToRestoreFlag) {
        allSceneObjects = scene->getDescendants();
    }
    
    SceneClass::setDebugLoggingEnabled(logObjectFailedToRestoreFlag);
    
    /*
     * Show the wait cursor
     */
    CursorDisplayScoped cursor;
    if (showWaitCursor) {
        cursor.showWaitCursor();
    }

    /*
     * Setup scene attributes
     */
    SceneAttributes* sceneAttributes = scene->getAttributes();
    sceneAttributes->clearErrorMessage();
    sceneAttributes->setSceneFileName(sceneFileName);
    sceneAttributes->setSceneName(scene->getName());
    sceneAttributes->setWindowRestoreBehaviorInSceneDisplay(SceneAttributes::RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES);
    sceneAttributes->setUseSceneForegroundAndBackgroundColors(s_useSceneForegroundBackgroundColorsFlag);
    
    if ( ! allSceneObjects.empty()) {
        SceneObject::setRestoredStatus(allSceneObjects,
                                       false);
    }
    
    GuiManager::get()->restoreFromScene(sceneAttributes,
                                        guiManagerClass);
    guiManagerClass->setRestored(true);
    
    if ( ! allSceneObjects.empty()) {
        SceneObject::logObjectsFailedRestore(scene->getName(),
                                             allSceneObjects);
    }
    
    SceneClass::setDebugLoggingEnabled(false);
    
    warningMessageOut = sceneAttributes->getSceneLoadWarningMessage();
    
    cursor.restoreCursor();
    
    /*
     * Set the active scene
     */
    EventSceneActive activeSceneEvent(EventSceneActive::MODE_SET);
    activeSceneEvent.setScene(scene);
    EventManager::get()->sendEvent(activeSceneEvent.getPointer());
    
    const AString sceneErrorMessage = sceneAttributes->getErrorMessage();
    if (sceneErrorMessage.isEmpty()) {
        /*
         * Add to recent scene files but only if the scene
         * file is NOT modified.
         *
         * It is possible for the user to create a new scene in
         * a new scene file and never write the scene file.
         * So, we don't want a non-existent scene file in the
         * recent scene file's menu.
         */
        if ( ! sceneFile->isModified()) {
            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            prefs->addToRecentFilesAndOrDirectories(sceneFile->getFileName());
        }
    }
    else {
        errorMessageOut = sceneErrorMessage;
        return false;
    }
    
    return true;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
SceneDialog::receiveEvent(Event* event)
{
    SceneFile* lastSceneFileRead = NULL;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);

        updateDialog();
        uiEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_ADD) {
        EventDataFileAdd* addEvent =
        dynamic_cast<EventDataFileAdd*>(event);
        CaretAssert(addEvent);
        
        /*
         * Determine if a scene file was read
         */
        CaretDataFile* dataFile = addEvent->getCaretDataFile();
        if (dataFile != NULL) {
            lastSceneFileRead = dynamic_cast<SceneFile*>(dataFile);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ) {
        EventDataFileRead* readEvent =
        dynamic_cast<EventDataFileRead*>(event);
        CaretAssert(readEvent);
        
        /*
         * Determine if a scene file was read
         */
        const int32_t numFilesRead = readEvent->getNumberOfDataFilesToRead();
        for (int32_t i = (numFilesRead - 1); i >= 0; i--) {
            if ( ! readEvent->isFileError(i)) {
                if (readEvent->getDataFileType(i) == DataFileTypeEnum::SCENE) {
                    CaretDataFile* dataFileRead = readEvent->getDataFileRead(i);
                    lastSceneFileRead = dynamic_cast<SceneFile*>(dataFileRead);
                    if (lastSceneFileRead != NULL) {
                        break;
                    }
                }
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_RELOAD) {
        EventDataFileReload* reloadEvent =
        dynamic_cast<EventDataFileReload*>(event);
        CaretAssert(reloadEvent);
        
        if ( ! reloadEvent->isError()) {
            CaretDataFile* dataFileRead = reloadEvent->getCaretDataFile();
            lastSceneFileRead = dynamic_cast<SceneFile*>(dataFileRead);
        }
    }
    
    /*
     * If a scene file was read, make it the selected scene file
     * in this dialog.
     */
    if (lastSceneFileRead != NULL) {
        loadSceneFileComboBox(lastSceneFileRead);
        loadScenesIntoDialog(NULL);
        highlightSceneAtIndex(0);
        m_sceneSelectionScrollArea->horizontalScrollBar()->setSliderPosition(0);
    }
    
    updateSceneFileModifiedStatusLabel();
}

/**
 * @return True if user should be informed about
 * creating scenes when exiting wb_view (user
 * never created a scene).
 */
bool
SceneDialog::isInformUserAboutScenesOnExit()
{
    return s_informUserAboutScenesOnExitFlag;
}

/**
 * Under some circumstances, the user may be warned
 * when creating a new scene or scene file.
 *
 * @return
 *     True if scene or scene file should be created, else false.
 */
bool
SceneDialog::displayNewSceneWarning()
{
    if (s_warnUserWhenCreatingSceneFlag) {
        s_warnUserWhenCreatingSceneFlag = false;
        
        ApplicationInformation appInfo;
        switch (appInfo.getWorkbenchSpecialVersion()) {
            case WorkbenchSpecialVersionEnum::WORKBENCH_SPECIAL_VERSION_NO:
                break;
            case WorkbenchSpecialVersionEnum::WORKBENCH_SPECIAL_VERSION_FIRST_CHARTING:
            {
                const QString text("This is a special version of Workbench that correctly "
                                   "displays scene files containing the first implementation "
                                   "of matrix and line charting.  Charting is undergoing a "
                                   "redesign and any scenes created with this special version "
                                   "of Workbench will not display correctly in newer, official "
                                   "releases of Workbench.");
                
                if (WuQMessageBox::warningOkCancel(this,
                                                   "Do you want to continue?",
                                                   text)) {
                    return true;
                }
                else {
                    return false;
                }
            }
                break;
        }
    }
    
    return true;
}

/**
 * Update the modified status label.
 */
void
SceneDialog::updateSceneFileModifiedStatusLabel()
{
    /*
     * Modified icons in scene file selection combo box
     */
    updateSceneFileComboBoxModifiedIcons();
    
    /*
     * Modified status of selected scene file
     */
    AString statusText;
    
    /*
     * Save button is enabled only if:
     *   (1) The scene file is modified
     *   (2) The filename is not the untitled file name
     */
    bool haveScenesFlag = false;
    bool saveButtonEnabledFlag = false;
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        if (sceneFile->isModified()) {
            if (sceneFile->exists()) {
                saveButtonEnabledFlag = true;
            }
            statusText = "<html><font color=\"red\">YES</font></html>";
        }
        else {
            statusText = "NO";
        }
        
        if (sceneFile->getNumberOfScenes() > 0) {
            haveScenesFlag = true;
        }
    }
    
    m_sceneFileModifiedStatusLabel->setText(statusText);
    m_saveSceneFilePushButton->setEnabled(saveButtonEnabledFlag);
    
    m_showFileStructurePushButton->setEnabled(haveScenesFlag);
    m_zipSceneFilePushButton->setEnabled(haveScenesFlag);
    m_uploadSceneFilePushButton->setEnabled(haveScenesFlag);
}

/**
 * Called when help button is clicked.
 */
void
SceneDialog::helpButtonClicked()
{
    EventHelpViewerDisplay helpViewerEvent(NULL,
                                           "Scenes_Window");
    EventManager::get()->sendEvent(helpViewerEvent.getPointer());
}

/* ======================================================================== */
/**
 * \class caret::SceneClassWidget
 * \brief Dialog for manipulation of scenes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
SceneClassInfoWidget::SceneClassInfoWidget()
: QGroupBox(0)
{
    m_scene = NULL;
    m_sceneIndex = -1;
    
    m_defaultBackgroundRole = backgroundRole();
    m_defaultAutoFillBackgroundStatus = autoFillBackground();
    
    m_activeSceneLabel = new QLabel();
    m_nameLabel = new QLabel();
    m_nameLabel->setWordWrap(true);
    
    QWidget* descriptionWidget(NULL);
    m_descriptionLabel = NULL;
    m_descriptionTextEdit = NULL;
    switch (s_scrollableDescriptionMode) {
        case 1:
            m_descriptionTextEdit = new QTextEdit();
            m_descriptionTextEdit->setReadOnly(true);
            descriptionWidget = m_descriptionTextEdit;
            break;
        case 2: /* fallthrough */
        default:
            m_descriptionLabel = new QLabel();
            m_descriptionLabel->setWordWrap(true);
            descriptionWidget = m_descriptionLabel;
            break;
    }
    
    m_sceneIdLabel = new QLabel();
    
    m_previewImageLabel = new QLabel();
    m_previewImageLabel->setContentsMargins(0, 0, 0, 0);
    
    m_rightSideWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightSideWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(3);
    rightLayout->addWidget(m_activeSceneLabel);
    rightLayout->addWidget(m_nameLabel,1);
    rightLayout->addWidget(m_sceneIdLabel);
    rightLayout->addWidget(descriptionWidget, 100);
    rightLayout->addStretch();
    
    m_leftSideWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(m_leftSideWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(m_previewImageLabel);
    leftLayout->addStretch();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 3, 0, 0);
    layout->setSpacing(3);
    layout->addWidget(m_leftSideWidget);
    layout->addWidget(m_rightSideWidget, 100, Qt::AlignTop);
}

/**
 * Destructor.
 */
SceneClassInfoWidget::~SceneClassInfoWidget()
{
    
}

/**
 * Set/reset the background so that the widget appears to be 
 * selected/unselected.
 *
 * @param selected
 *     Selection status.
 */
void
SceneClassInfoWidget::setBackgroundForSelected(const bool selected)
{
    if (selected) {
        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Highlight);
    }
    else {
        setAutoFillBackground(m_defaultAutoFillBackgroundStatus);
        setBackgroundRole(m_defaultBackgroundRole);
    }
}

/**
 * Update the content.
 *
 * @param scene
 *     Scene for display.
 * @param sceneIndex
 *     Index of the scene.
 * @param activeSceneFlag
 *     True if this is the active scene
 */
void
SceneClassInfoWidget::updateContent(Scene* scene,
                                    const int32_t sceneIndex,
                                    const bool activeSceneFlag)
{
    m_scene = scene;
    m_sceneIndex = sceneIndex;
    
    if ((m_scene != NULL)
        && (m_sceneIndex >= 0)) {
        AString nameText;
        AString sceneIdText;
        AString descriptionText;
        AString abbreviatedDescriptionText;
        AString fullDescriptionText;
        SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                         sceneIndex,
                                                                         nameText,
                                                                         sceneIdText,
                                                                         abbreviatedDescriptionText,
                                                                         fullDescriptionText);
        
        if (activeSceneFlag) {
            m_activeSceneLabel->setText("<html><font color=\"red\">Current Scene</font></html>");
            m_activeSceneLabel->setVisible(true);
        }
        else {
            m_activeSceneLabel->setText("");
            m_activeSceneLabel->setVisible(false);
        }
        m_nameLabel->setText(nameText);
        m_sceneIdLabel->setText(sceneIdText);
        if (m_descriptionLabel != NULL) {
            if (s_scrollableDescriptionMode == 2) {
                m_descriptionLabel->setText(fullDescriptionText);
            }
            else {
                m_descriptionLabel->setText(abbreviatedDescriptionText);
            }
        }
        if (m_descriptionTextEdit != NULL) {
            m_descriptionTextEdit->setText(fullDescriptionText);
        }
        
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                      imageBytesFormat);
        
        
        const int previewImageWidth = 192;
        
        QImage  previewImage;
        bool    previewImageValid = false;
        
        if (imageByteArray.length() > 0) {
            ImageFile imageFile;
            imageFile.setImageFromByteArray(imageByteArray,
                                            imageBytesFormat);
            
            previewImage = *imageFile.getAsQImage();
            if ( ! previewImage.isNull()) {
                imageFile.resizeToWidth(previewImageWidth);
                QImage newPreviewImage = *imageFile.getAsQImage();
                if ( ! newPreviewImage.isNull()) {
                    previewImage = newPreviewImage;
                    previewImageValid = true;
                }
            }
        }
        
        m_previewImageLabel->clear();
        m_previewImageLabel->setAlignment(Qt::AlignHCenter
                                          | Qt::AlignTop);
        if (previewImageValid) {
            m_previewImageLabel->setPixmap(QPixmap::fromImage(previewImage));
        }
        else {
            m_previewImageLabel->setText("<html>No preview<br>image</html>");
        }
    }
}

/**
 * Examine the given string for newlines and remove
 * any lines that exceed the maximum allowed.
 *
 * @param textLines
 *     String containing lines of text separated by a newline character.
 * @param maximumNumberOfLines
 *     Maximum number of lines for the text.
 */
void
SceneClassInfoWidget::limitToNumberOfLines(AString& textLines,
                                           const int32_t maximumNumberOfLines)
{
    if (textLines.isEmpty()) {
        return;
    }
    
    const QString lineSeparator("\n");
    
    QStringList descriptionLines = textLines.split(lineSeparator,
                                                   QString::KeepEmptyParts);
    const int32_t numLines = descriptionLines.size();
    const int32_t numLinesToRemove = numLines - maximumNumberOfLines;
    if (numLinesToRemove > 0) {
        for (int32_t i = 0; i < numLinesToRemove; i++) {
            descriptionLines.pop_back();
        }
    }
    textLines = descriptionLines.join(lineSeparator);
}

/**
 * Get formatted text for display of scene name and description.
 *
 * @param sceneInfo
 *    Info for the scene.
 * @param nameTextOut
 *    Text for name.
 * @param sceneIdTextOut
 *    Text for scene ID.
 * @param abbreviatedDescriptionTextOut
 *    Abbreviated text for description.
 * @param fullDescriptionTextOut
 *    Full text for description.
 */
void
SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                                 const int32_t sceneIndex,
                                                                 AString& nameTextOut,
                                                                 AString& sceneIdTextOut,
                                                                 AString& abbreviatedDescriptionTextOut,
                                                                 AString& fullDescriptionTextOut)
{
    CaretAssert(sceneInfo);
    
    AString name = sceneInfo->getName();
    if (name.isEmpty()) {
        name = "NAME IS MISSING !!!";
    }
    
    AString indexText;
    const bool showSceneIndexFlag = false;
    if (showSceneIndexFlag) {
        if (sceneIndex >= 0) {
            indexText = ("(" + AString::number(sceneIndex + 1) + ") ");
        }
    }
    
    nameTextOut = ("<html>"
                   + indexText
                   + "<b>NAME</b> ("
                   + AString::number(sceneIndex + 1)
                   + "):  "
                   + name
                   + "</html>");
    
    sceneIdTextOut = ("<html><b>BALSA SCENE ID</b>:  "
                      + sceneInfo->getBalsaSceneID()
                      + "</html>");
    
    fullDescriptionTextOut = sceneInfo->getDescription();
    abbreviatedDescriptionTextOut = fullDescriptionTextOut;
    const int32_t maximumLinesInDescription(9);
    SceneClassInfoWidget::limitToNumberOfLines(abbreviatedDescriptionTextOut,
                                               maximumLinesInDescription);

    if ( ! fullDescriptionTextOut.isEmpty()) {
        /*
         * HTML formatting is needed so text is properly displayed.
         * Want to put "Description" at beginning in bold but any
         * HTML tags are converted to text.  So, after conversion to
         * HTML, perform a replace to insert "Description" in bold.
         */
        const AString replaceWithDescriptionBoldText = "REPLACE_WITH_DESCRIPTION";
        fullDescriptionTextOut = WuQtUtilities::createWordWrappedToolTipText(replaceWithDescriptionBoldText
                                                                  + fullDescriptionTextOut);
        fullDescriptionTextOut.replace(replaceWithDescriptionBoldText,
                            "<b>DESCRIPTION:</b> ");
    }

    if ( ! abbreviatedDescriptionTextOut.isEmpty()) {
        /*
         * HTML formatting is needed so text is properly displayed.
         * Want to put "Description" at beginning in bold but any
         * HTML tags are converted to text.  So, after conversion to
         * HTML, perform a replace to insert "Description" in bold.
         */
        const AString replaceWithDescriptionBoldText = "REPLACE_WITH_DESCRIPTION";
        abbreviatedDescriptionTextOut = WuQtUtilities::createWordWrappedToolTipText(replaceWithDescriptionBoldText
                                                                  + abbreviatedDescriptionTextOut);
        abbreviatedDescriptionTextOut.replace(replaceWithDescriptionBoldText,
                            "<b>DESCRIPTION:</b> ");
    }
}


/**
 * Called by Qt when the mouse is pressed.
 *
 * @param event
 *    The mouse event information.
 */
void
SceneClassInfoWidget::mousePressEvent(QMouseEvent* event)
{
    emit highlighted(m_sceneIndex);
    event->setAccepted(true);
}

/**
 * Called when the mouse is double clicked.
 *
 * @param event
 *    The mouse event information.
 */
void
SceneClassInfoWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit activated(m_sceneIndex);
    event->setAccepted(true);
}

/**
 * @return The scene.
 */
Scene*
SceneClassInfoWidget::getScene()
{
    return m_scene;
}

/**
 * @return The index of the scene.
 */
int32_t
SceneClassInfoWidget::getSceneIndex() const
{
    return m_sceneIndex;
}


/**
 * @return True if this scene class info widget is valid (has
 * a scene with a valid index).
 */
bool
SceneClassInfoWidget::isValid() const
{
    if ((m_scene != NULL)
        && (m_sceneIndex >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Enable/disable scrollable scene description so all text visible
 * @param scrollMode
 *    The scroll mode
 */
void
SceneClassInfoWidget::setScrollableDescriptionMode(const int32_t scrollMode)
{
    s_scrollableDescriptionMode = scrollMode;
}

