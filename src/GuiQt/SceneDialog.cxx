
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
#include <QToolButton>
#include <QVBoxLayout>

#define __SCENE_DIALOG_DECLARE__
#include "SceneDialog.h"
#undef __SCENE_DIALOG_DECLARE__

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
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ProgressReportingDialog.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneCreateReplaceDialog.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "ScenePreviewDialog.h"
#include "SessionManager.h"
#include "UsernamePasswordWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQDialogNonModal.h"
#include "WuQMessageBox.h"
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
     * No apply buton
     */
    setApplyButtonText("");
    
    /*
     * Set the dialog's widget
     */
    this->setCentralWidget(createMainPage(),
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
 * Update the scene dialog.
 */
void
SceneDialog::updateDialog()
{
    loadSceneFileComboBox(NULL);
    loadScenesIntoDialog(NULL);
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
        m_sceneFileSelectionComboBox->addItem(name,
                                              qVariantFromValue((void*)sceneFile));
    }
    
    
    if (numSceneFiles > 0) {
        m_sceneFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);
        loadSceneFileMetaDataWidgets();
    }
    
    m_sceneFileButtonsGroup->setEnabled(getSelectedSceneFile() != NULL);
}

/**
 * Load the Scene File BALSA Study ID Line Edit
 */
void
SceneDialog::loadSceneFileMetaDataWidgets()
{
    m_fileBalsaStudyIDLineEdit->clear();
    m_fileBaseDirectoryLineEdit->clear();
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        m_fileBalsaStudyIDLineEdit->setText(sceneFile->getBalsaStudyID());
        m_fileBaseDirectoryLineEdit->setText(sceneFile->getBaseDirectory());
    }
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
    
    for (std::vector<SceneClassInfoWidget*>::iterator iter = m_sceneClassInfoWidgets.begin();
         iter != m_sceneClassInfoWidgets.end();
         iter++) {
        SceneClassInfoWidget* sciw = *iter;
        sciw->blockSignals(true);
        sciw->updateContent(NULL,
                            -1);
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
                m_sceneSelectionLayout->addWidget(sciw);
            }
            else {
                sciw = m_sceneClassInfoWidgets[i];
            }
            
            sciw->updateContent(scene,
                                i);
            
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
    m_testScenesPushButton->setEnabled(haveScenes);
    m_insertNewScenePushButton->setEnabled(validScene);
    m_replaceScenePushButton->setEnabled(validScene);
    m_showScenePushButton->setEnabled(validScene);
    m_showSceneImagePreviewPushButton->setEnabled(validScene);
}

/**
 * Enable the move up/down buttons
 */
void
SceneDialog::enableSceneMoveUpAndDownButtons()
{
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
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
 * Called to create a new scene file.
 */
void 
SceneDialog::newSceneFileButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    /*
     * Let user choose a different path/name
     */
    SceneFile* newSceneFile = new SceneFile();
    GuiManager::get()->getBrain()->convertDataFilePathNameToAbsolutePathName(newSceneFile);
    AString newSceneFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::SCENE,
                                                                      this,
                                                                      "Choose Scene File Name",
                                                                      newSceneFile->getFileName());
    /*
     * If user cancels, delete the new scene file and return
     */
    if (newSceneFileName.isEmpty()) {
        delete newSceneFile;
        return;
    }
    
    /*
     * Set name of new scene file and add to brain
     */
    newSceneFile->setFileName(newSceneFileName);
    EventManager::get()->sendEvent(EventDataFileAdd(newSceneFile).getPointer());
    this->loadSceneFileComboBox(newSceneFile);
    this->sceneFileSelected();    
}

/**
 * Called when upload scene file is selected.
 */
void
SceneDialog::uploadSceneFileButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_uploadSceneFilePushButton,
                               "No Scene File is selected.");
        return;
    }
    if (sceneFile->isModified()) {
        WuQMessageBox::errorOk(m_uploadSceneFilePushButton,
                               "Selected Scene File is modified and must be saved prior to uploading.");
        return;
    }
    
    if (sceneFile->getBalsaStudyID().trimmed().isEmpty()) {
        const QString msg("The BALSA Study ID is missing.  You must go to the "
                          "<a href=\"https://balsa.wustl.edu\">BALSA Database</a> and get a BALSA Study ID.");
        WuQMessageBox::errorOk(m_uploadSceneFilePushButton, msg);
        return;
    }
    
    BalsaDatabaseUploadSceneFileDialog uploadDialog(sceneFile,
                                                    this);
    uploadDialog.exec();
}


/**
 * Called when zip scene file is selected.
 */
void
SceneDialog::zipSceneFileButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(m_zipSceneFilePushButton,
                               "No Scene File is selected.");
        return;
    }
    if (sceneFile->isModified()) {
        WuQMessageBox::errorOk(m_zipSceneFilePushButton,
                               "Selected Scene File is modified and must be saved prior to zipping.");
        return;
    }
    
    ZipSceneFileDialog zipDialog(sceneFile,
                                 this);
    zipDialog.exec();
}

/**
 * Called when a scene file is selected.
 */
void 
SceneDialog::sceneFileSelected()
{
    loadScenesIntoDialog(NULL);
    loadSceneFileMetaDataWidgets();
}

/**
 * Called when Edit Study ID button is clicked.
 */
void
SceneDialog::editFileBalsaStudyIDButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        return;
    }
    
    WuQDataEntryDialog ded("Edit BALSA Database Info",
                           m_editBalsaStudyIDPushButton,
                           WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    QLineEdit* lineEdit = ded.addLineEditWidget("BALSA Study ID");
    lineEdit->setText(sceneFile->getBalsaStudyID());
    lineEdit->setMinimumWidth(200);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const AString idText = lineEdit->text().trimmed();
        sceneFile->setBalsaStudyID(idText);
        loadSceneFileMetaDataWidgets();
    }
}

/**
 * Called when upload scene file is selected.
 */
void
SceneDialog::editBaseDirectoryPushButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        return;
    }
    
    WuQDataEntryDialog ded("Edit Base Directory",
                           m_editBaseDirectoryPushButton,
                           WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    QLineEdit* lineEdit = ded.addLineEditWidget("Base Directory");
    lineEdit->setText(sceneFile->getBaseDirectory());
    lineEdit->setMinimumWidth(500);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const AString idText = lineEdit->text().trimmed();
        sceneFile->setBaseDirectory(idText);
        loadSceneFileMetaDataWidgets();
    }
}

/**
 * Called when upload scene file is selected.
 */
void
SceneDialog::browseBaseDirectoryPushButtonClicked()
{
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile == NULL) {
        return;
    }
    
    /*
     * Let user choose directory path
     */
    QString directoryName;
    FileInformation fileInfo(sceneFile->getBaseDirectory());
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            directoryName = fileInfo.getAbsoluteFilePath();
        }
    }
    AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(m_browseBaseDirectoryPushButton,
                                                                           "Choose Base Directory",
                                                                           directoryName);
    /*
     * If user cancels,  return
     */
    if (newDirectoryName.isEmpty()) {
        return;
    }
    
    /*
     * Set name of new scene file and add to brain
     */
    sceneFile->setBaseDirectory(newDirectoryName);
    loadSceneFileMetaDataWidgets();
}

/**
 * Called when add new scene button clicked.
 */
void
SceneDialog::addNewSceneButtonClicked()
{
    if ( ! displayNewSceneWarning()) {
        return;
    }
    
    if ( ! checkForModifiedFiles(true)) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* newScene = SceneCreateReplaceDialog::createNewScene(m_addNewScenePushButton,
                                                                   sceneFile);
        if (newScene != NULL) {
            s_informUserAboutScenesOnExitFlag = false;
        }
        
        loadScenesIntoDialog(newScene);
    }
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
    
    if ( ! checkForModifiedFiles(true)) {
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
            }
            
            loadScenesIntoDialog(newScene);
        }
    }
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
    
    std::vector<AString> sceneNames;
    std::vector<AString> sceneErrors;
    std::vector<QImage> sceneImages;
    std::vector<QImage> newImages;
    
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
    
    showWaitCursor();
    
    const int IMAGE_DISPLAY_WIDTH = 500;
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* origScene = sceneFile->getSceneAtIndex(i);
        if (origScene != NULL) {
            if (origScene->hasFilesWithRemotePaths()) {
                CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                 password);
            }
            
            AString errorMessage;
            const bool successFlag = displayScenePrivateWithErrorMessage(sceneFile,
                                                                         origScene,
                                                                         false,
                                                                         errorMessage);
            if (successFlag) {
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                
                QImage image;
                AString imageErrorMessage;
                const bool validImage = SceneCreateReplaceDialog::createSceneImage(image,
                                                                                   imageErrorMessage);
                if (validImage) {
                    newImages.push_back(image.scaledToWidth(IMAGE_DISPLAY_WIDTH));
                }
                else {
                    newImages.push_back(QImage());
                }
            }
            else {
                newImages.push_back(QImage());
            }
            

            
            sceneNames.push_back(origScene->getName());
            const QImage* imageFromScene = getQImageFromSceneInfo(origScene->getSceneInfo());
            if (imageFromScene != NULL) {
                sceneImages.push_back(imageFromScene->scaledToWidth(IMAGE_DISPLAY_WIDTH));
                delete imageFromScene;
            }
            else {
                sceneImages.push_back(QImage());
            }
            
            sceneErrors.push_back(errorMessage);
            
        }
    }
    
    CaretAssert(sceneNames.size() == sceneErrors.size());
    CaretAssert(sceneNames.size() == sceneImages.size());
    CaretAssert(sceneNames.size() == sceneErrors.size());
    CaretAssert(sceneNames.size() == newImages.size());
    
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

    WuQDialogNonModal* dialog = new WuQDialogNonModal("Scene Comparisons",
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
    
    if ( ! checkForModifiedFiles(true)) {
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
            }
            
            loadScenesIntoDialog(newScene);
        }
    }
}

///**
// * Add an image to the scene.
// * 
// * @param scene
// *    Scene to which image is added.
// */
//void
//SceneDialog::addImageToScene(Scene* scene)
//{
//    AString errorMessage;
//    
//    CaretAssert(scene);
//    
//    uint8_t backgroundColor[3] = { 0, 0, 0 };
//    bool backgroundColorValid = false;
//    
//    /*
//     * Capture an image of each window
//     */
//    std::vector<ImageFile*> imageFiles;
//    std::vector<BrainBrowserWindow*> windows = GuiManager::get()->getAllOpenBrainBrowserWindows();
//    for (std::vector<BrainBrowserWindow*>::iterator iter = windows.begin();
//         iter != windows.end();
//         iter++) {
//        BrainBrowserWindow* bbw = *iter;
//        const int32_t browserWindowIndex = bbw->getBrowserWindowIndex();
//        
//        EventImageCapture imageCaptureEvent(browserWindowIndex);
//        EventManager::get()->sendEvent(imageCaptureEvent.getPointer());
//        
//        if (imageCaptureEvent.getEventProcessCount() > 0) {
//            if (imageCaptureEvent.isError()) {
//                errorMessage.appendWithNewLine(imageCaptureEvent.getErrorMessage());
//            }
//            else {
//                imageFiles.push_back(new ImageFile(imageCaptureEvent.getImage()));
//                if ( ! backgroundColorValid) {
//                    imageCaptureEvent.getBackgroundColor(backgroundColor);
//                    backgroundColorValid = true;
//                }
//            }
//        }
//    }
//    
//    /*
//     * Assemble images of each window into a single image
//     * and add it to the scene.  Use one image per row
//     * since the images are limited in horizontal space
//     * when shown in the listing of scenes.
//     */
//    if ( ! imageFiles.empty()) {
//        try {
//            const int32_t numImagesPerRow = 1;
//            ImageFile compositeImageFile;
//            compositeImageFile.combinePreservingAspectAndFillIfNeeded(imageFiles,
//                                                                      numImagesPerRow,
//                                                                      backgroundColor);
//            
//            compositeImageFile.resizeToMaximumWidth(512);
//            
//            QByteArray byteArray;
//            compositeImageFile.getImageInByteArray(byteArray,
//                                                   SceneDialog::PREFERRED_IMAGE_FORMAT);
//            
//            scene->getSceneInfo()->setImageBytes(byteArray,
//                                                          SceneDialog::PREFERRED_IMAGE_FORMAT);
//        }
//        catch (const DataFileException& dfe) {
//            WuQMessageBox::errorOk(m_addNewScenePushButton,
//                                   dfe.whatString());
//        }
//    }
//    
//    /*
//     * Free memory from the image files.
//     */
//    for (std::vector<ImageFile*>::iterator iter = imageFiles.begin();
//         iter != imageFiles.end();
//         iter++) {
//        delete *iter;
//    }
//}

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
SceneDialog::checkForModifiedFiles(const bool creatingSceneFlag)
{
    if (creatingSceneFlag) {
        EventModelGetAll allModelsEvent;
        EventManager::get()->sendEvent(allModelsEvent.getPointer());
        if (allModelsEvent.getModels().empty()) {
            const QString msg("No surfaces or volumes are loaded.  Continue creating scene?");
            if ( ! WuQMessageBox::warningYesNo(this,
                                               msg)) {
                return false;
            }
        }
    }
    
    AString dialogMessage;
    AString modifiedFilesMessage;
    GuiManager::TestModifiedMode testMode = GuiManager::TEST_FOR_MODIFIED_FILES_MODE_FOR_SCENE_SHOW;
    if (creatingSceneFlag) {
        testMode = GuiManager::TEST_FOR_MODIFIED_FILES_MODE_FOR_SCENE_ADD;
    }
    const bool haveModifiedFilesFlag = GuiManager::get()->testForModifiedFiles(testMode,
                                                                               dialogMessage,
                                                                               modifiedFilesMessage);

    bool result = true;
    
    if (haveModifiedFilesFlag) {
        QMessageBox warningDialog(QMessageBox::Warning,
                               "Warning",
                               dialogMessage,
                               QMessageBox::NoButton,
                               this);
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
 * Create the main page.
 * @return the main page.
 */
QWidget* 
SceneDialog::createMainPage()
{
    QWidget* sceneFileWidget = createSceneFileWidget();
    
    /*
     * Scene controls
     */ 
    QLabel* sceneLabel = new QLabel("Scenes");
    
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
    m_showSceneImagePreviewPushButton->setToolTip("Show larger image and full description\n"
                                                  "for the selected scene");
    QObject::connect(m_showSceneImagePreviewPushButton, SIGNAL(clicked()),
                     this, SLOT(showImagePreviewButtonClicked()));
    
    /*
     * Layout for scene buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(m_showScenePushButton);
    sceneButtonLayout->addWidget(m_showSceneImagePreviewPushButton);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(m_addNewScenePushButton);
    sceneButtonLayout->addWidget(m_insertNewScenePushButton);
    sceneButtonLayout->addWidget(m_replaceScenePushButton);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_testScenesPushButton);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(m_moveSceneUpPushButton);
    sceneButtonLayout->addWidget(m_moveSceneDownPushButton);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(m_deleteScenePushButton);

    /*
     * Widget and layout containing the scene class info.
     */
    m_sceneSelectionLayout = new QVBoxLayout();
    m_sceneSelectionWidget = new QWidget();
    m_sceneSelectionWidget->setSizePolicy(m_sceneSelectionWidget->sizePolicy().horizontalPolicy(),
                                          QSizePolicy::Fixed);
    QVBoxLayout* sceneSelectionWidgetLayout = new QVBoxLayout(m_sceneSelectionWidget);
    sceneSelectionWidgetLayout->addLayout(m_sceneSelectionLayout);
    sceneSelectionWidgetLayout->addStretch();
    m_sceneSelectionScrollArea = new QScrollArea();
    m_sceneSelectionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sceneSelectionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sceneSelectionScrollArea->setWidget(m_sceneSelectionWidget);
    m_sceneSelectionScrollArea->setWidgetResizable(true);
    
    QVBoxLayout* showOptionsLabelsLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutMargins(showOptionsLabelsLayout, 0);
    showOptionsLabelsLayout->addWidget(new QLabel(" Show Scene"), 0, Qt::AlignRight);
    showOptionsLabelsLayout->addWidget(new QLabel("Options"), 0, Qt::AlignHCenter);
    
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
    gridLayout->addLayout(showOptionsLabelsLayout, row, 0, Qt::AlignTop);
    gridLayout->addWidget(createShowOptionsWidget(), row, 1, 1, 1); //, Qt::AlignTop);
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
                     this, SLOT(sceneFileSelected()));
    
    /*
     * New File button
     */
    QPushButton* newSceneFilePushButton = new QPushButton("New...");
    newSceneFilePushButton->setToolTip("Create a new scene file");
    QObject::connect(newSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(newSceneFileButtonClicked()));
    
    /*
     * Upload button
     */
    m_uploadSceneFilePushButton = new QPushButton("Upload...");
    m_uploadSceneFilePushButton->setToolTip("Upload the selected scene file to the BALSA Database");
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
     * Scene BALSA Study ID
     */
    QLabel* studyIDLabel = new QLabel("BALSA Study ID");
    m_fileBalsaStudyIDLineEdit = new QLineEdit();
    m_fileBalsaStudyIDLineEdit->setToolTip("Press Edit button to change Study ID for use with BALSA Database");
    m_fileBalsaStudyIDLineEdit->setReadOnly(true);
    
    /*
     * Edit BALSA Study ID button
     */
    m_editBalsaStudyIDPushButton = new QPushButton("Edit...");
    m_editBalsaStudyIDPushButton->setToolTip("Edit the Scene File's BALSA Study ID");
    QObject::connect(m_editBalsaStudyIDPushButton, SIGNAL(clicked()),
                     this, SLOT(editFileBalsaStudyIDButtonClicked()));
    
    /*
     * Base Directory
     */
    QLabel* baseDirectoryLabel = new QLabel("Base Directory");
    m_fileBaseDirectoryLineEdit = new QLineEdit();
    m_fileBaseDirectoryLineEdit->setToolTip("Press Browse or Edit button to change base directory");
    m_fileBaseDirectoryLineEdit->setReadOnly(true);
    m_editBaseDirectoryPushButton = new QPushButton("Edit...");
    m_editBaseDirectoryPushButton->setToolTip("Edit the base directory in a text edit");
    QObject::connect(m_editBaseDirectoryPushButton, SIGNAL(clicked()),
                     this, SLOT(editBaseDirectoryPushButtonClicked()));
    m_browseBaseDirectoryPushButton = new QPushButton("Browse...");
    m_browseBaseDirectoryPushButton->setToolTip("Use a file system dialog to choose the base directory");
    QObject::connect(m_browseBaseDirectoryPushButton, SIGNAL(clicked()),
                     this, SLOT(browseBaseDirectoryPushButtonClicked()));
    
    /*
     * Group for scene file buttons that are only valid when a scene file is available
     */
    m_sceneFileButtonsGroup = new WuQWidgetObjectGroup(this);
    m_sceneFileButtonsGroup->add(m_uploadSceneFilePushButton);
    m_sceneFileButtonsGroup->add(m_zipSceneFilePushButton);
    m_sceneFileButtonsGroup->add(m_editBalsaStudyIDPushButton);
    m_sceneFileButtonsGroup->add(m_editBaseDirectoryPushButton);
    m_sceneFileButtonsGroup->add(m_browseBaseDirectoryPushButton);
    
    /*
     * Scene file buttons layout
     */
    QHBoxLayout* fileButtonsLayout = new QHBoxLayout();
    fileButtonsLayout->setContentsMargins(0, 0, 0, 0);
    fileButtonsLayout->addWidget(newSceneFilePushButton);
    fileButtonsLayout->addWidget(m_uploadSceneFilePushButton);
    fileButtonsLayout->addWidget(m_zipSceneFilePushButton);
    
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
    int row = 0;
    gridLayout->addWidget(sceneFileLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_sceneFileSelectionComboBox, row, 1);
    gridLayout->addLayout(fileButtonsLayout, row, 2, 1, 2);
//    gridLayout->addWidget(newSceneFilePushButton, row, 2);
//    gridLayout->addWidget(m_uploadSceneFilePushButton, row, 3);
//    gridLayout->addWidget(m_zipSceneFilePushButton, row, 4);
    row++;
    gridLayout->addWidget(baseDirectoryLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_fileBaseDirectoryLineEdit, row, 1);
    gridLayout->addWidget(m_editBaseDirectoryPushButton, row, 2);
    gridLayout->addWidget(m_browseBaseDirectoryPushButton, row, 3);
    row++;
    gridLayout->addWidget(studyIDLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_fileBalsaStudyIDLineEdit, row, 1);
    gridLayout->addWidget(m_editBalsaStudyIDPushButton, row, 2);
    
    return widget;
}


/**
 * @return The show scene options widget.
 */
QWidget*
SceneDialog::createShowOptionsWidget()
{
    /*
     * Use colors contained in the scene for background and foreground
     */
    const QString colorTip1("Scenes created after 01 May 2016 contain the active background and foreground\n"
                            "colors.  When this box is checked and the colors are present in the scene loaded,\n"
                            "they will temporarily override the colors in the Preferences Dialog.  The background\n"
                            "and foreground colors will revert to those on the Preferences Dialog when any\n"
                            "of these events occur:\n"
                            "   * This checkbox is unchecked\n"
                            "   * A scene without background and foreground colors is loaded\n"
                            "   * A Spec File and its data files are loaded\n"
                            "   * File Menu->Close All Files is selected\n"
                            "   * Any of the colors on the Preferences Dialog are changed\n"
                            "   * The user quits wb_view");

    m_useSceneColorsCheckBox = new QCheckBox("Use background and foreground colors from scene");
    m_useSceneColorsCheckBox->setToolTip(colorTip1);
    m_useSceneColorsCheckBox->setChecked(true);
    QObject::connect(m_useSceneColorsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(useSceneColorsCheckBoxClicked(bool)));
    
    QFrame* frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(1);
    frame->setMidLineWidth(1);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    WuQtUtilities::setLayoutMargins(layout, 5);
    layout->addWidget(m_useSceneColorsCheckBox, 0, Qt::AlignLeft);
    
    return frame;
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
        }
    }
}

/**
 * Called when show scene button clicked.
 */
void 
SceneDialog::showSceneButtonClicked()
{
// At this time, do not test for modified files
// since a scene previously loaded may contain
// modified palette status.
//    if ( ! checkForModifiedFiles(false)) {
//        return;
//    }
    
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
        
        displayScenePrivateWithErrorMessageDialog(sceneFile,
                            scene,
                            false);
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
            AString descriptionText;
            const int32_t negativeIsUnlimitedNumberOfLines = -1;
            SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                             nameText,
                                                                             sceneIdText,
                                                                             descriptionText,
                                                                             negativeIsUnlimitedNumberOfLines);
            QLabel* nameLabel = new QLabel(nameText);
            ded.addWidget("",
                          nameLabel);
            
            QLabel* sceneIdLabel = new QLabel(sceneIdText);
            ded.addWidget("",
                          sceneIdLabel);
            
            if (! descriptionText.isEmpty()) {
                QLabel* descriptionLabel = new QLabel(descriptionText);
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
    const bool isSuccessful = displayScenePrivateWithErrorMessageDialog(sceneFile,
                        scene,
                        true);
    loadSceneFileComboBox(sceneFile);
    loadScenesIntoDialog(scene);
    
    return isSuccessful;
}

/**
 * Display the given scene from the given scene file.
 * If the scene fails to load, an error message is displayed
 * in a dialog.
 *
 * @param sceneFile
 *     Scene file.
 * @param scene
 *     Scene that is displayed.
 * param showWaitCursor
 *     Show a wait cursor while loading scene
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displayScenePrivateWithErrorMessageDialog(SceneFile* sceneFile,
                                                       Scene* scene,
                                                       const bool showWaitCursor)
{
    AString errorMessage;
    
    const bool successFlag = displayScenePrivateWithErrorMessage(sceneFile,
                                                 scene,
                                                 showWaitCursor,
                                                 errorMessage);
    if ( ! successFlag) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
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
 * @return
 *     true if scene was displayed without error, else false.
 */
bool
SceneDialog::displayScenePrivateWithErrorMessage(SceneFile* sceneFile,
                                                 Scene* scene,
                                                 const bool showWaitCursor,
                                                 AString& errorMessageOut)
{
    CaretAssert(sceneFile);
    CaretAssert(scene);
    
    errorMessageOut.clear();
    
    const AString sceneFileName = sceneFile->getFileName();
    
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        errorMessageOut = ("Top level scene class should be guiManager but it is: "
                           + guiManagerClass->getName());
        return false;
    }
    
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
    sceneAttributes->setUseSceneForegroundAndBackgroundColors(m_useSceneColorsCheckBox->isChecked());
    
    GuiManager::get()->restoreFromScene(sceneAttributes,
                                        guiManagerClass);
    
    cursor.restoreCursor();
    
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
            prefs->addToPreviousSceneFiles(sceneFile->getFileName());
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
    
    m_nameLabel = new QLabel();
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    
    m_sceneIdLabel = new QLabel();
    
    m_previewImageLabel = new QLabel();
    m_previewImageLabel->setContentsMargins(0, 0, 0, 0);
    
    m_rightSideWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightSideWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(3);
    rightLayout->addWidget(m_nameLabel);
    rightLayout->addWidget(m_sceneIdLabel);
    rightLayout->addWidget(m_descriptionLabel);
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
    layout->addWidget(m_rightSideWidget, 100);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
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
 */
void
SceneClassInfoWidget::updateContent(Scene* scene,
                                    const int32_t sceneIndex)
{
    m_scene = scene;
    m_sceneIndex = sceneIndex;
    
    if ((m_scene != NULL)
        && (m_sceneIndex >= 0)) {
        AString nameText;
        AString sceneIdText;
        AString descriptionText;
        const int32_t numLinesToDisplay = 9;
        SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                         nameText,
                                                                         sceneIdText,
                                                                         descriptionText,
                                                                         numLinesToDisplay);
        
        m_nameLabel->setText(nameText);
        m_sceneIdLabel->setText(sceneIdText);
        m_descriptionLabel->setText(descriptionText);
        
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                      imageBytesFormat);
        
        
        const int previewImageWidth = 192; //128;
        
        QImage  previewImage;
        bool    previewImageValid = false;
        
        if (imageByteArray.length() > 0) {
            try {
                ImageFile imageFile;
                imageFile.setImageFromByteArray(imageByteArray,
                                                imageBytesFormat);
                imageFile.resizeToWidth(previewImageWidth);
                previewImage = *imageFile.getAsQImage();
                previewImageValid = true;
            }
            catch (const DataFileException& dfe) {
                CaretLogSevere(dfe.whatString());
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
        
//        const int32_t maxHeight = 150;
//        const int32_t maxSizeHintHeight = std::max(m_leftSideWidget->sizeHint().height(),
//                                                   m_rightSideWidget->sizeHint().height());
//        const int32_t fixedHeight = std::min(maxSizeHintHeight,
//                                             maxHeight);
//        setFixedHeight(fixedHeight);
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
 * @param desciptionTextOut
 *    Text for description.
 * @param maximumLinesInDescription
 *    Maximum number of lines allowed in description.
 *    If value is negative, an unlimited number of lines are allowed.
 */
void
SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                                 AString& nameTextOut,
                                                                 AString& sceneIdTextOut,
                                                                 AString& descriptionTextOut,
                                                                 const int32_t maximumLinesInDescription)
{
    CaretAssert(sceneInfo);
    
    AString name = sceneInfo->getName();
    if (name.isEmpty()) {
        name = "NAME IS MISSING !!!";
    }
    
    nameTextOut = ("<html><b>NAME</b>:  "
                   + name
                   + "</html>");
    
    sceneIdTextOut = ("<html><b>BALSA SCENE ID</b>:  "
                      + sceneInfo->getBalsaSceneID()
                      + "</html>");
    
    AString description = sceneInfo->getDescription();
    
    if (maximumLinesInDescription > 0) {
        SceneClassInfoWidget::limitToNumberOfLines(description,
                                                   maximumLinesInDescription);
    }
    
    if ( ! description.isEmpty()) {
        /*
         * HTML formatting is needed so text is properly displayed.
         * Want to put "Description" at beginning in bold but any
         * HTML tags are converted to text.  So, after conversion to
         * HTML, perform a replace to insert "Description" in bold.
         */
        const AString replaceWithDescriptionBoldText = "REPLACE_WITH_DESCRIPTION";
        description = WuQtUtilities::createWordWrappedToolTipText(replaceWithDescriptionBoldText
                                                                  + description);
        description.replace(replaceWithDescriptionBoldText,
                            "<b>DESCRIPTION:</b> ");
    }
    
    descriptionTextOut = description;
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

