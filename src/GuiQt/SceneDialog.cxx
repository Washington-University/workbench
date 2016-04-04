
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
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventDataFileAdd.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventImageCapture.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
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
    
//    setDialogSizeHint(650,
//                      500);
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
    
    resize(650,
           500);
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
    if (validFile) {
        const Scene* scene = getSelectedScene();
        if (scene != NULL) {
            validScene = true;
        }
    }
    
    enableSceneMoveUpAndDownButtons();
    
    m_addNewScenePushButton->setEnabled(validFile);
    m_deleteScenePushButton->setEnabled(validScene);
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
                
                m_sceneSelectionScrollArea->ensureWidgetVisible(sciw);
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
    }
}

/**
 * Called when insert new scene button clicked.
 */
void
SceneDialog::insertSceneButtonClicked()
{
    if (checkForModifiedFiles() == false) {
        return;
    }
    
    SceneFile* sceneFile = getSelectedSceneFile();
    if (sceneFile != NULL) {
        Scene* scene = getSelectedScene();
        if (scene != NULL) {
            Scene* newScene = SceneCreateReplaceDialog::createNewSceneInsertBeforeScene(m_insertNewScenePushButton,
                                                                                       sceneFile,
                                                                                       scene);
            loadScenesIntoDialog(newScene);
        }
    }
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
    
    uint8_t backgroundColor[3] = { 0, 0, 0 };
    bool backgroundColorValid = false;
    
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
                if ( ! backgroundColorValid) {
                    imageCaptureEvent.getBackgroundColor(backgroundColor);
                    backgroundColorValid = true;
                }
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
//    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_addNewScenePushButton);
    sceneButtonLayout->addWidget(m_insertNewScenePushButton);
    sceneButtonLayout->addWidget(m_replaceScenePushButton);
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
    m_sceneSelectionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_sceneSelectionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sceneSelectionScrollArea->setWidget(m_sceneSelectionWidget);
    m_sceneSelectionScrollArea->setWidgetResizable(true);
    
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
    gridLayout->addWidget(m_sceneSelectionScrollArea, row, 1);
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
        
//        ElapsedTimer timer;
//        timer.start();
        
        displayScenePrivate(sceneFile,
                            scene,
                            false);
        
//        const AString msg = ("Time to load scene: "
//                             + AString::number(timer.getElapsedTimeSeconds(), 'f', 3)
//                             + " seconds.");
//        WuQMessageBox::informationOk(m_showScenePushButton,
//                                     msg);
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
            AString descriptionText;
            SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                             nameText,
                                                                             descriptionText);
            QLabel* nameLabel = new QLabel(nameText);
            ded.addWidget("",
                          nameLabel);
            
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
     * Setup scene attributes
     */
    SceneAttributes* sceneAttributes = scene->getAttributes();
    sceneAttributes->clearErrorMessage();
    sceneAttributes->setSceneFileName(sceneFileName);
    sceneAttributes->setSceneName(scene->getName());
    sceneAttributes->setWindowRestoreBehaviorInSceneDisplay(SceneAttributes::RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES);
    
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
    m_scene = NULL;
    m_sceneIndex = -1;
    
    m_defaultBackgroundRole = backgroundRole();
    m_defaultAutoFillBackgroundStatus = autoFillBackground();
    
    m_nameLabel = new QLabel();
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    
    m_previewImageLabel = new QLabel();
    m_previewImageLabel->setContentsMargins(0, 0, 0, 0);
    
    m_rightSideWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightSideWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(m_nameLabel);
    rightLayout->addSpacing(5);
    rightLayout->addWidget(m_descriptionLabel);
    rightLayout->addStretch();
    
    m_leftSideWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(m_leftSideWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(m_previewImageLabel);
    leftLayout->addStretch();
    
    WuQtUtilities::matchWidgetHeights(m_leftSideWidget,
                                      m_rightSideWidget);
    
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
        AString descriptionText;
        SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(scene->getSceneInfo(),
                                                                         nameText,
                                                                         descriptionText);
        m_nameLabel->setText(nameText);
        m_descriptionLabel->setText(descriptionText);
        
        QByteArray imageByteArray;
        AString imageBytesFormat;
        scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                      imageBytesFormat);
        
        
        const int previewImageWidth = 128;
        
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
            m_leftSideWidget->setMaximumHeight(1000);
            m_rightSideWidget->setMaximumHeight(m_leftSideWidget->sizeHint().height());
        }
        else {
            m_previewImageLabel->setText("<html>No preview<br>image</html>");
            int32_t maxHeight = std::max(m_leftSideWidget->sizeHint().height(),
                                              m_rightSideWidget->sizeHint().height());
            maxHeight = std::min(maxHeight,
                                    previewImageWidth);
            m_leftSideWidget->setMaximumHeight(maxHeight);
            m_rightSideWidget->setMaximumHeight(maxHeight);
        }
        
//        const int maximumLabelSize = maximumPreviewImageSize + 8;
//        m_previewImageLabel->setFixedWidth(maximumLabelSize);
    }
}

/**
 * Get formatted text for display of scene name and description.
 *
 * @param sceneInfo
 *    Info for the scene.
 * @param nameTextOut
 *    Text for name.
 * @param desciptionTextOut
 *    Text for description.
 */
void
SceneClassInfoWidget::getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                       AString& nameTextOut,
                                                       AString& descriptionTextOut)
{
    CaretAssert(sceneInfo);
    
    AString name = sceneInfo->getName();
    if (name.isEmpty()) {
        name = "NAME IS MISSING !!!";
    }
    
    nameTextOut = ("<html><b>NAME</b>:  "
                   + name
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

