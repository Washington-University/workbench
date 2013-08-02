
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

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
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
#include "CursorDisplayScoped.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneFile.h"
#include "Scene.h"
#include "UsernamePasswordWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQListWidget.h"
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
 */
SceneDialog::SceneDialog(QWidget* parent)
: WuQDialogNonModal("Scenes",
                    parent)
{
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
    this->setCentralWidget(tabWidget);

    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
    
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
    loadSceneListWidget(NULL);    
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
    Scene* scene = NULL;
    QListWidgetItem* lwi = m_sceneSelectionListWidget->currentItem();
    if (lwi != NULL) {
        scene = reinterpret_cast<Scene*>(qVariantValue<quintptr>(lwi->data(Qt::UserRole)));
    }
    return scene;
}



/**
 * Load the scene files into the scene file combo box.
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
 * Load the scene selection list widget.
 */
void 
SceneDialog::loadSceneListWidget(Scene* selectedSceneIn)
{
    SceneFile* sceneFile = getSelectedSceneFile();
    Scene* selectedScene = selectedSceneIn;
    if (selectedScene == NULL) {
        selectedScene = getSelectedScene();
    }
    
    m_sceneSelectionListWidget->blockSignals(true);
    m_sceneSelectionListWidget->clear();
    
    if (sceneFile != NULL) {
        int32_t defaultIndex = 0;
        const int32_t numScenes = sceneFile->getNumberOfScenes();
        for (int32_t i = 0; i < numScenes; i++) {
            Scene* scene = sceneFile->getSceneAtIndex(i);
            
            QListWidgetItem* lwi = new QListWidgetItem(scene->getName());
            lwi->setToolTip(WuQtUtilities::createWordWrappedToolTipText(scene->getDescription()));
            lwi->setData(Qt::UserRole,
                         qVariantFromValue(reinterpret_cast<quintptr>(scene)));
            
            if (scene == selectedScene) {
                defaultIndex = i;
            }
            m_sceneSelectionListWidget->addItem(lwi);
        }
        
        if (numScenes > 0) {
            m_sceneSelectionListWidget->setCurrentRow(defaultIndex);
        }
    }
    m_sceneSelectionListWidget->blockSignals(false);

    const bool validFile = (getSelectedSceneFile() != NULL);
    const bool validScene = (getSelectedScene() != NULL);    
    
    m_addNewScenePushButton->setEnabled(validFile);
    m_deleteScenePushButton->setEnabled(validScene);
    m_replaceScenePushButton->setEnabled(validScene);
    m_showScenePushButton->setEnabled(validScene);
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
    loadSceneListWidget(NULL);
}

/**
 * Called when a scene is selected.
 */
void 
SceneDialog::sceneSelected()
{
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
        /*
         * Create dialog for scene creation
         */
        WuQDataEntryDialog newSceneDialog("New Scene",
                                          this);
        
        /*
         * Will want to valid data.
         */
        QObject::connect(&newSceneDialog, SIGNAL(validateData(WuQDataEntryDialog*)),
                         this, SLOT(validateContentOfCreateSceneDialog(WuQDataEntryDialog*)));
        
        /*
         * Name for scene
         */
        QLineEdit* newSceneNameLineEdit = newSceneDialog.addLineEditWidget("Name", 
                                                                           "");
        newSceneNameLineEdit->selectAll();
        newSceneNameLineEdit->setObjectName("sceneNameLineEdit");
        
        /*
         * Description
         */
        QTextEdit* descriptionTextEdit = newSceneDialog.addTextEdit("Description", 
                                                                "", 
                                                                false);
        
        if (newSceneDialog.exec() == WuQDataEntryDialog::Accepted) {
            const AString newSceneName = newSceneNameLineEdit->text();
            Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
            Scene::setSceneBeingCreated(newScene);
            newScene->setName(newSceneName);
            newScene->setDescription(descriptionTextEdit->toPlainText());
            
            /*
             * Get all browser tabs and only save transformations for tabs
             * that are valid.
             */ 
            EventBrowserTabGetAll getAllTabs;
            EventManager::get()->sendEvent(getAllTabs.getPointer());
            std::vector<int32_t> tabIndices = getAllTabs.getBrowserTabIndices();
            
            SceneAttributes* sceneAttributes = newScene->getAttributes();
            sceneAttributes->setSceneFileName(sceneFile->getFileName());
            sceneAttributes->setIndicesOfTabsForSavingToScene(tabIndices);
            sceneAttributes->setSpecFileNameIncludedInScene(m_optionsCreateSceneAddSpecFileCheckBox->isChecked());
            
            newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
                                                              "guiManager"));
            
            sceneFile->addScene(newScene);
            
            Scene::setSceneBeingCreated(NULL);
            
            loadSceneListWidget(newScene);
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
            /*
             * Create dialog for scene creation
             */
            WuQDataEntryDialog newSceneDialog("Replace Scene",
                                              this);
            
            /*
             * Will want to valid data.
             */
            QObject::connect(&newSceneDialog, SIGNAL(validateData(WuQDataEntryDialog*)),
                             this, SLOT(validateContentOfCreateSceneDialog(WuQDataEntryDialog*)));
            
            /*
             * Name for scene
             */
            QLineEdit* newSceneNameLineEdit = newSceneDialog.addLineEditWidget("Name",
                                                                               scene->getName());
            newSceneNameLineEdit->setObjectName("sceneNameLineEdit");
            
            /*
             * Description
             */
            QTextEdit* descriptionTextEdit = newSceneDialog.addTextEdit("Description",
                                                                        scene->getDescription(),
                                                                        false);
            
            /*
             * Error checking will not allow two scenes with the same name
             * so temporarily modify name of scene being replaced and restore
             * it if the user does not hit OK.
             */
            const AString savedSceneName = scene->getName();
            scene->setName("slkkjdlkfjaslfjdljfdkljdfjsdfj");
            
            if (newSceneDialog.exec() == WuQDataEntryDialog::Accepted) {
                const AString newSceneName = newSceneNameLineEdit->text();
                Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
                Scene::setSceneBeingCreated(newScene);
                newScene->setName(newSceneName);
                newScene->setDescription(descriptionTextEdit->toPlainText());
                
                /*
                 * Get all browser tabs and only save transformations for tabs
                 * that are valid.
                 */
                EventBrowserTabGetAll getAllTabs;
                EventManager::get()->sendEvent(getAllTabs.getPointer());
                std::vector<int32_t> tabIndices = getAllTabs.getBrowserTabIndices();
                
                SceneAttributes* sceneAttributes = newScene->getAttributes();
                sceneAttributes->setSceneFileName(sceneFile->getFileName());
                sceneAttributes->setIndicesOfTabsForSavingToScene(tabIndices);
                sceneAttributes->setSpecFileNameIncludedInScene(m_optionsCreateSceneAddSpecFileCheckBox->isChecked());
                
                newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
                                                                  "guiManager"));
                
                sceneFile->replaceScene(newScene,
                                        scene);
                
                Scene::setSceneBeingCreated(NULL);
                
                loadSceneListWidget(newScene);
            }
            else {
                scene->setName(savedSceneName);
            }
        }
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
     *   Connectivity Files
     *   Scene Files
     *   Spec Files
     */
    std::vector<DataFileTypeEnum::Enum> dataFileTypesToExclude;
    DataFileTypeEnum::getAllConnectivityEnums(dataFileTypesToExclude);
    dataFileTypesToExclude.push_back(DataFileTypeEnum::SCENE);
    dataFileTypesToExclude.push_back(DataFileTypeEnum::SPECIFICATION);
    
    bool result = true;
    if (GuiManager::get()->getBrain()->areFilesModified(dataFileTypesToExclude)) {
        result = WuQMessageBox::warningOkCancel(this,
                                           "Files are modified and should be saved "
                                           "before creating the scene.\n"
                                           "\n"
                                                "Continue creating scene?");
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
     * Layout for scene buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(m_showScenePushButton);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_addNewScenePushButton);
    sceneButtonLayout->addWidget(m_replaceScenePushButton);
    sceneButtonLayout->addWidget(m_deleteScenePushButton);
    
    /*
     * Scene selection list widget
     */
    m_sceneSelectionListWidget = new WuQListWidget();
    
    QObject::connect(m_sceneSelectionListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(sceneSelected()));
    QObject::connect(m_sceneSelectionListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                     this, SLOT(showSceneButtonClicked()));  // show the scene
    QObject::connect(m_sceneSelectionListWidget, SIGNAL(itemWasDropped()),
                     this, SLOT(sceneWasDropped())); 
    
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
    gridLayout->addWidget(m_sceneSelectionListWidget, row, 1);
    gridLayout->addLayout(sceneButtonLayout, row, 2);
    
    return widget;
}

/**
 * Called when a scene is dropped by the user dragging a scene in the list box
 */
void 
SceneDialog::sceneWasDropped()
{
    std::vector<Scene*> newlyOrderedScenes;
    
    /*
     * Get the scenes from this list widget to obtain the new scene ordering.
     */
    const int32_t numItems = m_sceneSelectionListWidget->count();
    for (int32_t i = 0; i < numItems; i++) {
        QListWidgetItem* lwi = m_sceneSelectionListWidget->item(i);
        if (lwi != NULL) {
            if (lwi != NULL) {
                Scene* scene = reinterpret_cast<Scene*>(qVariantValue<quintptr>(lwi->data(Qt::UserRole)));
                newlyOrderedScenes.push_back(scene);
            }
        }
    }
    
    if (newlyOrderedScenes.empty() == false) {
        /*
         * Update the order of the scenes in the scene file.
         */
        SceneFile* sceneFile = getSelectedSceneFile();
        if (sceneFile != NULL) {
            sceneFile->reorderScenes(newlyOrderedScenes);
            sceneFileSelected();
        }
    }
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
//    const int COLUMN_WIDGET = 1;
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
        
        displayScenePrivate(sceneFile,
                            scene,
                            false);
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
    loadSceneListWidget(scene);
    
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
    sceneAttributes->setWindowRestoreBehavior(windowBehavior);
    
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
