
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#define __SCENE_DIALOG_DECLARE__
#include "SceneDialog.h"
#undef __SCENE_DIALOG_DECLARE__

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
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventImageCapture.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ProgressReportingDialog.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneCreateReplaceDialog.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "Scene.h"
#include "SessionManager.h"
#include "UsernamePasswordWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

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
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createMainPage(), "Scenes");
    tabWidget->addTab(createOptionPage(), "Options");
    
    /*
     * No apply buton
     */
    setApplyButtonText("");
    
    /*
     * Set the dialog's widget
     */
    this->setCentralWidget(tabWidget,
                           WuQDialog::SCROLL_AREA_NEVER);

    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
    
    setDialogSizeHint(650,
                      500);
    
    /*
     * Update the dialog.
     */
    updateDialog();
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
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
    
    
    bool validScene   = false;
    bool validPreview = false;
    if (validFile) {
        const Scene* scene = getSelectedScene();
        if (scene != NULL) {
            validScene = true;
            const SceneInfo* sceneInfo = scene->getSceneInfo();
            CaretAssert(scene);
            validPreview = sceneInfo->hasImage();
        }
    }
    m_addNewScenePushButton->setEnabled(validFile);
    m_deleteScenePushButton->setEnabled(validScene);
    m_replaceScenePushButton->setEnabled(validScene);
    m_showScenePushButton->setEnabled(validScene);
    m_showSceneImagePreviewPushButton->setEnabled(validPreview);
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
            }
            else {
                sciw->setBackgroundForSelected(false);
            }
        }
    }
    
    if ( ! sceneIndexValid) {
        m_selectedSceneClassInfoIndex = -1;
    }
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
    /*
     * Create a new scene file that will have proper path
     */
    Brain* brain = GuiManager::get()->getBrain();
    SceneFile* newSceneFile = brain->addSceneFile();
    
    /*
     * Let user choose a different path/name
     */
    AString newSceneFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::SCENE,
                                                                      this,
                                                                      "Choose Scene File Name",
                                                                      newSceneFile->getFileName());
    /*
     * If user cancels, delete the new scene file and return
     */
    if (newSceneFileName.isEmpty()) {
        brain->removeDataFile(newSceneFile);
        return;
    }
    
    /*
     * Set name of new scene file
     */
    newSceneFile->setFileName(newSceneFileName);
    this->loadSceneFileComboBox(newSceneFile);
    this->sceneFileSelected();    
}


/**
 * Called when a scene file is selected.
 */
void 
SceneDialog::sceneFileSelected()
{
    loadScenesIntoDialog(NULL);
}

/**
 * Called when add new scene button clicked.
 */
void 
SceneDialog::addNewSceneButtonClicked()
{
    if (checkForModifiedFiles() == false) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* newScene = SceneCreateReplaceDialog::createNewScene(m_addNewScenePushButton,
                                                                   sceneFile);
        loadScenesIntoDialog(newScene);
        
//        /*
//         * Create dialog for scene creation
//         */
//        WuQDataEntryDialog newSceneDialog("New Scene",
//                                          this);
//        
//        /*
//         * Will want to valid data.
//         */
//        QObject::connect(&newSceneDialog, SIGNAL(validateData(WuQDataEntryDialog*)),
//                         this, SLOT(validateContentOfCreateSceneDialog(WuQDataEntryDialog*)));
//        
//        /*
//         * Name for scene
//         */
//        QLineEdit* newSceneNameLineEdit = newSceneDialog.addLineEditWidget("Name", 
//                                                                           "");
//        newSceneNameLineEdit->selectAll();
//        newSceneNameLineEdit->setObjectName("sceneNameLineEdit");
//        
//        /*
//         * Description
//         */
//        QTextEdit* descriptionTextEdit = newSceneDialog.addTextEdit("Description", 
//                                                                "", 
//                                                                false);
//        
//        if (newSceneDialog.exec() == WuQDataEntryDialog::Accepted) {
//            const AString newSceneName = newSceneNameLineEdit->text();
//            Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
//            Scene::setSceneBeingCreated(newScene);
//            newScene->setName(newSceneName);
//            newScene->setDescription(descriptionTextEdit->toPlainText());
//            
//            /*
//             * Get all browser tabs and only save transformations for tabs
//             * that are valid.
//             */ 
//            EventBrowserTabGetAll getAllTabs;
//            EventManager::get()->sendEvent(getAllTabs.getPointer());
//            std::vector<int32_t> tabIndices = getAllTabs.getBrowserTabIndices();
//            
//            SceneAttributes* sceneAttributes = newScene->getAttributes();
//            sceneAttributes->setSceneFileName(sceneFile->getFileName());
//            sceneAttributes->setIndicesOfTabsForSavingToScene(tabIndices);
//            sceneAttributes->setSpecFileNameIncludedInScene(m_optionsCreateSceneAddSpecFileCheckBox->isChecked());
//            
//            newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
//                                                              "guiManager"));
//            
//            addImageToScene(newScene);
//            
//            sceneFile->addScene(newScene);
//            
//            Scene::setSceneBeingCreated(NULL);
//            
//            loadScenesIntoDialog(newScene);
//        }
    }
}

/**
 * Called when replace scene button clicked.
 */
void
SceneDialog::replaceSceneButtonClicked()
{
    if (checkForModifiedFiles() == false) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            Scene* newScene = SceneCreateReplaceDialog::replaceExistingScene(m_addNewScenePushButton,
                                                                             sceneFile,
                                                                             scene);
            loadScenesIntoDialog(newScene);
            
            
//            /*
//             * Create dialog for scene creation
//             */
//            WuQDataEntryDialog newSceneDialog("Replace Scene",
//                                              this);
//            
//            /*
//             * Will want to valid data.
//             */
//            QObject::connect(&newSceneDialog, SIGNAL(validateData(WuQDataEntryDialog*)),
//                             this, SLOT(validateContentOfCreateSceneDialog(WuQDataEntryDialog*)));
//            
//            /*
//             * Name for scene
//             */
//            QLineEdit* newSceneNameLineEdit = newSceneDialog.addLineEditWidget("Name",
//                                                                               scene->getName());
//            newSceneNameLineEdit->setObjectName("sceneNameLineEdit");
//            
//            /*
//             * Description
//             */
//            QTextEdit* descriptionTextEdit = newSceneDialog.addTextEdit("Description",
//                                                                        scene->getDescription(),
//                                                                        false);
//            
//            /*
//             * Error checking will not allow two scenes with the same name
//             * so temporarily modify name of scene being replaced and restore
//             * it if the user does not hit OK.
//             */
//            const AString savedSceneName = scene->getName();
//            scene->setName("slkkjdlkfjaslfjdljfdkljdfjsdfj");
//            
//            if (newSceneDialog.exec() == WuQDataEntryDialog::Accepted) {
//                const AString newSceneName = newSceneNameLineEdit->text();
//                Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
//                Scene::setSceneBeingCreated(newScene);
//                newScene->setName(newSceneName);
//                newScene->setDescription(descriptionTextEdit->toPlainText());
//                
//                /*
//                 * Get all browser tabs and only save transformations for tabs
//                 * that are valid.
//                 */
//                EventBrowserTabGetAll getAllTabs;
//                EventManager::get()->sendEvent(getAllTabs.getPointer());
//                std::vector<int32_t> tabIndices = getAllTabs.getBrowserTabIndices();
//                
//                SceneAttributes* sceneAttributes = newScene->getAttributes();
//                sceneAttributes->setSceneFileName(sceneFile->getFileName());
//                sceneAttributes->setIndicesOfTabsForSavingToScene(tabIndices);
//                sceneAttributes->setSpecFileNameIncludedInScene(m_optionsCreateSceneAddSpecFileCheckBox->isChecked());
//                
//                newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
//                                                                  "guiManager"));
//                
//                addImageToScene(newScene);
//                
//                sceneFile->replaceScene(newScene,
//                                        scene);
//                
//                Scene::setSceneBeingCreated(NULL);
//                
//                loadScenesIntoDialog(newScene);
//            }
//            else {
//                scene->setName(savedSceneName);
//            }
        }
    }
}

/**
 * Add an image to the scene.
 * 
 * @param scene
 *    Scene to which image is added.
 */
void
SceneDialog::addImageToScene(Scene* scene)
{
    AString errorMessage;
    
    CaretAssert(scene);
    
    /*
     * Capture an image of each window
     */
    std::vector<ImageFile*> imageFiles;
    std::vector<BrainBrowserWindow*> windows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (std::vector<BrainBrowserWindow*>::iterator iter = windows.begin();
         iter != windows.end();
         iter++) {
        BrainBrowserWindow* bbw = *iter;
        const int32_t browserWindowIndex = bbw->getBrowserWindowIndex();
        
        EventImageCapture imageCaptureEvent(browserWindowIndex);
        EventManager::get()->sendEvent(imageCaptureEvent.getPointer());
        
        if (imageCaptureEvent.getEventProcessCount() > 0) {
            if (imageCaptureEvent.isError()) {
                errorMessage.appendWithNewLine(imageCaptureEvent.getErrorMessage());
            }
            else {
                imageFiles.push_back(new ImageFile(imageCaptureEvent.getImage()));
            }
        }
    }
    
    /*
     * Assemble images of each window into a single image
     * and add it to the scene.  Use one image per row
     * since the images are limited in horizontal space
     * when shown in the listing of scenes.
     */
    if ( ! imageFiles.empty()) {
        try {
            const int32_t numImagesPerRow = 1;
            ImageFile compositeImageFile;
            uint8_t backgroundColor[4] = { 0, 0, 0, 255 };
            SessionManager::get()->getCaretPreferences()->getColorBackground(backgroundColor);
            compositeImageFile.combinePreservingAspectAndFillIfNeeded(imageFiles,
                                                                      numImagesPerRow,
                                                                      backgroundColor);
            
            compositeImageFile.resizeToMaximumWidth(512);
            
            QByteArray byteArray;
            compositeImageFile.getImageInByteArray(byteArray,
                                                   SceneDialog::PREFERRED_IMAGE_FORMAT);
            
            scene->getSceneInfo()->setImageBytes(byteArray,
                                                          SceneDialog::PREFERRED_IMAGE_FORMAT);
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::errorOk(m_addNewScenePushButton,
                                   dfe.whatString());
        }
    }
    
    /*
     * Free memory from the image files.
     */
    for (std::vector<ImageFile*>::iterator iter = imageFiles.begin();
         iter != imageFiles.end();
         iter++) {
        delete *iter;
    }
}


/**
 * Check to see if there are modified files.  If there are
 * allow the user to continue or cancel creation of the scene.
 *
 * @return
 *     true if the scene should be created, otherwise false.
 */
bool
SceneDialog::checkForModifiedFiles()
{
    /*
     * Exclude all 
     *   Scene Files
     *   Spec Files
     */
    std::vector<DataFileTypeEnum::Enum> dataFileTypesToExclude;
    DataFileTypeEnum::getAllConnectivityEnums(dataFileTypesToExclude);
    dataFileTypesToExclude.push_back(DataFileTypeEnum::SCENE);
    dataFileTypesToExclude.push_back(DataFileTypeEnum::SPECIFICATION);
    
    std::vector<CaretDataFile*> allDataFiles;
    GuiManager::get()->getBrain()->getAllDataFiles(allDataFiles);

    AString modifiedDataFilesMessage;
    AString modifiedPaletteFilesMessage;
    
    for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        CaretDataFile* caretDataFile = *iter;
        
        const DataFileTypeEnum::Enum dataFileType = caretDataFile->getDataFileType();
        if (std::find(dataFileTypesToExclude.begin(),
                      dataFileTypesToExclude.end(),
                      dataFileType) != dataFileTypesToExclude.end()) {
            continue;
        }
        
        CaretMappableDataFile* mappableDataFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile);
        
        if (caretDataFile->isModified()) {
            AString fileMsg = caretDataFile->getFileNameNoPath();
            
            /*
             * Is modification just the palette color mapping?
             */
            bool paletteOnlyModFlag = false;
            if (mappableDataFile != NULL) {
                if (mappableDataFile->isModifiedPaletteColorMapping()) {
                    if ( ! mappableDataFile->isModifiedExcludingPaletteColorMapping()) {
                        paletteOnlyModFlag = true;
                    }
                }
            }
            
            if (paletteOnlyModFlag) {
                modifiedPaletteFilesMessage.appendWithNewLine("    " + fileMsg);
            }
            else {
                modifiedDataFilesMessage.appendWithNewLine("    " + fileMsg);
            }
        }
    }
    
    if ( ! modifiedDataFilesMessage.isEmpty()) {
        modifiedDataFilesMessage.insert(0,
                                        "These file(s) contain modified data:\n");
        modifiedDataFilesMessage.append("\n");
    }
    
    if ( ! modifiedPaletteFilesMessage.isEmpty()) {
        modifiedPaletteFilesMessage.insert(0,
                                           "These file(s) contain modified palette color mapping.  It is not "
                                           "necessary to save these file(s) if the save palette color mapping "
                                           "option is selected on the scene creation dialog:\n");
        modifiedPaletteFilesMessage.append("\n");
    }

    bool result = true;
    if (( ! modifiedDataFilesMessage.isEmpty())
        || ( ! modifiedPaletteFilesMessage.isEmpty())) {

        const AString msg = (modifiedDataFilesMessage
                             + modifiedPaletteFilesMessage
                             + "\nContinue creating the scene?");
        result = WuQMessageBox::warningYesNo(this,
                                             WuQtUtilities::createWordWrappedToolTipText(msg));
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
    QObject::connect(newSceneFilePushButton, SIGNAL(clicked()),
                     this, SLOT(newSceneFileButtonClicked()));
    
    /*
     * Scene controls
     */ 
    QLabel* sceneLabel = new QLabel("Scenes");
    
    /*
     * Add new scene button
     */
    m_addNewScenePushButton = new QPushButton("Add...");
    QObject::connect(m_addNewScenePushButton, SIGNAL(clicked()),
                     this, SLOT(addNewSceneButtonClicked()));
    
    /*
     * Delete new scene button
     */
    m_deleteScenePushButton = new QPushButton("Delete...");
    QObject::connect(m_deleteScenePushButton, SIGNAL(clicked()),
                     this, SLOT(deleteSceneButtonClicked()));
    
    /*
     * Replace scene button
     */
    m_replaceScenePushButton = new QPushButton("Replace...");
    QObject::connect(m_replaceScenePushButton, SIGNAL(clicked()),
                     this, SLOT(replaceSceneButtonClicked()));
    
    /*
     * Show new scene button
     */
    m_showScenePushButton = new QPushButton("Show");
    QObject::connect(m_showScenePushButton, SIGNAL(clicked()),
                     this, SLOT(showSceneButtonClicked()));
    
    /*
     * Show scene image button
     */
    m_showSceneImagePreviewPushButton = new QPushButton("Preview...");
    QObject::connect(m_showSceneImagePreviewPushButton, SIGNAL(clicked()),
                     this, SLOT(showImagePreviewButtonClicked()));
    
    /*
     * Layout for scene buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(m_showScenePushButton);
    sceneButtonLayout->addWidget(m_showSceneImagePreviewPushButton);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_addNewScenePushButton);
    sceneButtonLayout->addWidget(m_replaceScenePushButton);
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
    QScrollArea* sceneSelectionScrollArea = new QScrollArea();
    sceneSelectionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sceneSelectionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sceneSelectionScrollArea->setWidget(m_sceneSelectionWidget);
    sceneSelectionScrollArea->setWidgetResizable(true);
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = 0;
    gridLayout->addWidget(sceneFileLabel, row, 0);
    gridLayout->addWidget(m_sceneFileSelectionComboBox, row, 1);
    gridLayout->addWidget(newSceneFilePushButton, row, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3);
    row++;
    gridLayout->addWidget(sceneLabel, row, 0, (Qt::AlignTop | Qt::AlignRight));
    gridLayout->addWidget(sceneSelectionScrollArea, row, 1);
    gridLayout->addLayout(sceneButtonLayout, row, 2);
    row++;
    
    return widget;
}

/**
 * Called when a scene is dropped by the user dragging a scene in the list box
 */
void 
SceneDialog::sceneWasDropped()
{
//    std::vector<Scene*> newlyOrderedScenes;
//    
//    /*
//     * Get the scenes from this list widget to obtain the new scene ordering.
//     */
//    const int32_t numItems = m_sceneSelectionListWidget->count();
//    for (int32_t i = 0; i < numItems; i++) {
//        QListWidgetItem* lwi = m_sceneSelectionListWidget->item(i);
//        if (lwi != NULL) {
//            if (lwi != NULL) {
//                Scene* scene = reinterpret_cast<Scene*>(qVariantValue<quintptr>(lwi->data(Qt::UserRole)));
//                newlyOrderedScenes.push_back(scene);
//            }
//        }
//    }
//    
//    if (newlyOrderedScenes.empty() == false) {
//        /*
//         * Update the order of the scenes in the scene file.
//         */
//        SceneFile* sceneFile = getSelectedSceneFile();
//        if (sceneFile != NULL) {
//            sceneFile->reorderScenes(newlyOrderedScenes);
//            sceneFileSelected();
//        }
//    }
}

/**
 * Create the options page.
 * @return the options page.
 */
QWidget* 
SceneDialog::createOptionPage()
{
    /*
     * Layout for page
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(createSceneCreateOptionsWidget());
    layout->addWidget(createSceneShowOptionsWidget());
    layout->addStretch();
    
    return widget;
}

/**
 * @return Create and return a widget with the create scenes options.
 */
QWidget* 
SceneDialog::createSceneCreateOptionsWidget()
{
    m_optionsCreateSceneAddSpecFileCheckBox = new QCheckBox("Add spec file name to scene");
    m_optionsCreateSceneAddSpecFileCheckBox->setChecked(true);
    
    /*
     * Create scene group box
     */
    const int COLUMN_LABEL = 0;
    QGroupBox* createScenesOptionsGroupBox = new QGroupBox("Create scenes options");
    QGridLayout* createScenesLayout = new QGridLayout(createScenesOptionsGroupBox);
    const int createSceneRow = createScenesLayout->rowCount(); // should be zero
    createScenesLayout->addWidget(m_optionsCreateSceneAddSpecFileCheckBox, createSceneRow, COLUMN_LABEL, 1, 2);
    
    return createScenesOptionsGroupBox;
}

/**
 * @return Create and return a widget with the show scenes options.
 */
QWidget* 
SceneDialog::createSceneShowOptionsWidget()
{
    /*
     * Show scene window behavior combo box
     */
    QLabel* optionShowSceneWindowBehaviorLabel = new QLabel("Window positioning: ");
    m_optionsShowSceneWindowBehaviorComboBox = new QComboBox();
    m_optionsShowSceneWindowBehaviorComboBox->addItem("Ignore window positions from scene",
                                                      (int)SceneAttributes::RESTORE_WINDOW_IGNORE_ALL_POSITIONS_AND_SIZES);
    m_optionsShowSceneWindowBehaviorComboBox->addItem("Position windows relative to first window",
                                                      (int)SceneAttributes::RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES);
    m_optionsShowSceneWindowBehaviorComboBox->addItem("Use all window positions from scene",
                                                      (int)SceneAttributes::RESTORE_WINDOW_USE_ALL_POSITIONS_AND_SIZES);
    m_optionsShowSceneWindowBehaviorComboBox->setCurrentIndex(1);
    
    /*
     * Show scene group box
     */
    const int COLUMN_LABEL = 0;
    const int COLUMN_WIDGET = 1;
    QGroupBox* showScenesOptionsGroupBox = new QGroupBox("Show scenes options");
    QGridLayout* showScenesLayout = new QGridLayout(showScenesOptionsGroupBox);
    const int showSceneRow = showScenesLayout->rowCount(); // should be zero
    showScenesLayout->addWidget(optionShowSceneWindowBehaviorLabel, showSceneRow, COLUMN_LABEL);
    showScenesLayout->addWidget(m_optionsShowSceneWindowBehaviorComboBox, showSceneRow, COLUMN_WIDGET);
    
    return showScenesOptionsGroupBox;
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
        QLineEdit* sceneNameLineEdit = dynamic_cast<QLineEdit*>(sceneNameWidget);
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
        
        ElapsedTimer timer;
        timer.start();
        
        displayScenePrivate(sceneFile,
                            scene,
                            false);
        
        const AString msg = ("Time to load scene: "
                             + AString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                             + " seconds.");
        WuQMessageBox::informationOk(m_showScenePushButton,
                                     msg);
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
        
        
        AString errorMessage;
        
        if (imageByteArray.length() > 0) {
            try {
                ImageFile imageFile;
                imageFile.setImageFromByteArray(imageByteArray,
                                                imageBytesFormat);
                const QImage* image = imageFile.getAsQImage();
                if (image != NULL) {
                    if (image->isNull()) {
                        errorMessage = "Image is invalid (isNull)";
                    }
                    else {
                        WuQDataEntryDialog ded(scene->getName(),
                                               m_showSceneImagePreviewPushButton,
                                               WuQDialog::SCROLL_AREA_AS_NEEDED);
                        QLabel* imageLabel = new QLabel();
                        imageLabel->setPixmap(QPixmap::fromImage(*image));
                        ded.addWidget("",
                                      imageLabel);
                        ded.setCancelButtonText("");
                        ded.setOkButtonText("Close");
                        ded.exec();
                    }
                }
                else {
                    errorMessage = "Image is not valid (NULL pointer)";
                }
            }
            catch (const DataFileException& dfe) {
                errorMessage = dfe.whatString();
            }
            
            if ( ! errorMessage.isEmpty()) {
                WuQMessageBox::errorOk(m_showSceneImagePreviewPushButton,
                                       errorMessage);
            }
        }
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
    const bool isSuccessful = displayScenePrivate(sceneFile,
                        scene,
                        true);
    loadSceneFileComboBox(sceneFile);
    loadScenesIntoDialog(scene);
    
    return isSuccessful;
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
SceneDialog::displayScenePrivate(SceneFile* sceneFile,
                                 Scene* scene,
                                 const bool showWaitCursor)
{
    CaretAssert(sceneFile);
    CaretAssert(scene);
    
    const AString sceneFileName = sceneFile->getFileName();
    
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        WuQMessageBox::errorOk(this,"Top level scene class should be guiManager but it is: "
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
     * Window restoration behavior
     */
    const int windowBehaviorIndex = m_optionsShowSceneWindowBehaviorComboBox->currentIndex();
    const SceneAttributes::RestoreWindowBehavior windowBehavior
    = static_cast<SceneAttributes::RestoreWindowBehavior>(m_optionsShowSceneWindowBehaviorComboBox->itemData(windowBehaviorIndex).toInt());
    
    SceneAttributes* sceneAttributes = scene->getAttributes();
    sceneAttributes->setSceneFileName(sceneFileName);
    sceneAttributes->setSceneName(scene->getName());
    sceneAttributes->setWindowRestoreBehaviorInSceneDisplay(windowBehavior);
    
    GuiManager::get()->restoreFromScene(sceneAttributes,
                                        guiManagerClass);
    
    cursor.restoreCursor();
    
    const AString sceneErrorMessage = sceneAttributes->getErrorMessage();
    if (sceneErrorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this,
                               sceneErrorMessage);
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
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);

        updateDialog();
        uiEvent->setEventProcessed();
    }
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
//    QWidget* w = new QWidget();
//    setFrameShape(QFrame::Box);
//    setFrameStyle(QFrame::Plain);
//    setLineWidth(1);
//    setMidLineWidth(0);
    
    m_scene = NULL;
    m_sceneIndex = -1;
    
    m_defaultBackgroundRole = backgroundRole();
    m_defaultAutoFillBackgroundStatus = autoFillBackground();
    
    m_nameLabel = new QLabel();
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    
    m_previewImageLabel = new QLabel();
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_nameLabel);
    rightLayout->addWidget(m_descriptionLabel);
    rightLayout->addStretch();
    
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_previewImageLabel);
    leftLayout->addStretch();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addLayout(rightLayout, 100);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed); //Minimum);
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
        setBackgroundRole(QPalette::AlternateBase);
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
        const SceneInfo* sceneInfo = m_scene->getSceneInfo();
        
        m_nameLabel->setText("<html><b>Name</b>: "
                             + sceneInfo->getName()
                             + "</html>");
        
        
        AString description = sceneInfo->getDescription();
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
                                "<b>Description:</b><br>");
        }
        
        m_descriptionLabel->setText(description);
//        m_descriptionLabel->setText("<html><b>Description</b>: "
//                                    + sceneInfo->getDescription()
//                                    + "</html>");
//        m_descriptionLabel->setWordWrap(true);
        
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                      imageBytesFormat);
        
        
        const int maximumPreviewImageSize = 64;
        
        QImage  previewImage;
        bool    previewImageValid = false;
        
        if (imageByteArray.length() > 0) {
            try {
                ImageFile imageFile;
                imageFile.setImageFromByteArray(imageByteArray,
                                                imageBytesFormat);
                imageFile.resizeToMaximumWidthOrHeight(maximumPreviewImageSize);
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
            //m_previewImageLabel->setText("");
            m_previewImageLabel->setPixmap(QPixmap::fromImage(previewImage));
        }
        else {
            m_previewImageLabel->setText("<html>No preview<br>image</html>");
            //m_previewImageLabel->setPixmap(QPixmap());
        }
        
        const int maximumLabelSize = maximumPreviewImageSize + 8;
        m_previewImageLabel->setFixedWidth(maximumLabelSize);
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

