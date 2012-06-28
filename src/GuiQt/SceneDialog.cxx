
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

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>

#define __SCENE_DIALOG_DECLARE__
#include "SceneDialog.h"
#undef __SCENE_DIALOG_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneException.h"
#include "SceneFile.h"
#include "Scene.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SceneDialog 
 * \brief Dialog for manipulation of scenes.
 */

/**
 * Constructor.
 */
SceneDialog::SceneDialog(QWidget* parent)
: WuQDialogNonModal("Scenes",
                    parent)
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
     * Show new scene button
     */
    m_showScenePushButton = new QPushButton("Show...");
    QObject::connect(m_showScenePushButton, SIGNAL(clicked()),
                     this, SLOT(showSceneButtonClicked()));
    
    /*
     * Layout for scene buttons
     */
    QVBoxLayout* sceneButtonLayout = new QVBoxLayout();
    sceneButtonLayout->addWidget(m_showScenePushButton);
    sceneButtonLayout->addStretch();
    sceneButtonLayout->addWidget(m_addNewScenePushButton);
    sceneButtonLayout->addWidget(m_deleteScenePushButton);
    
    /*
     * Scene selection list widget
     */
    m_sceneSelectionListWidget = new QListWidget();
    m_sceneSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    QObject::connect(m_sceneSelectionListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(sceneSelected()));

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
    
    /*
     * No apply buton
     */
    setApplyButtonText("");
    
    /*
     * Set the dialog's widget
     */
    this->setCentralWidget(widget);

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
        void* ptr = qVariantValue<void*>(lwi->data(Qt::UserRole));
        scene = (Scene*)ptr;
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
            lwi->setData(Qt::UserRole,
                         qVariantFromValue((void*)scene));
            
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
    m_showScenePushButton->setEnabled(validScene);
}


/**
 * Called to create a new scene file.
 */
void 
SceneDialog::newSceneFileButtonClicked()
{
    const QString fileExtension = DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SCENE);
    QString newFileName = ("NewFile." 
                           + fileExtension);
    
    WuQDataEntryDialog newFileDialog("New Scene File",
                                     this);
    QLineEdit* newFileNameLineEdit = newFileDialog.addLineEditWidget("New Scene File Name", 
                                                                     newFileName);
    
    if (newFileDialog.exec() == WuQDataEntryDialog::Accepted) {
        QString sceneFileName   = newFileNameLineEdit->text();
        
        try {
            if (sceneFileName.endsWith(fileExtension) == false) {
                sceneFileName += ("."
                                   + fileExtension);
            }
            
            SceneFile* sceneFile = GuiManager::get()->getBrain()->addSceneFile();
            sceneFile->setFileName(sceneFileName);
            
            this->loadSceneFileComboBox(sceneFile);
            this->sceneFileSelected();
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::errorOk(this, 
                                   dfe.whatString());
        }
    }
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
    SceneFile* sceneFile = getSelectedSceneFile();
    Scene* newScene = NULL;
    if (sceneFile != NULL) {
        try {
            WuQDataEntryDialog newSceneDialog("New Scene",
                                              this);
            AString newSceneName = "Scene X";
            QLineEdit* newSceneNameLineEdit = newSceneDialog.addLineEditWidget("New Scene Name", 
                                                                               newSceneName);
            newSceneNameLineEdit->selectAll();
            
            QStringList newSceneTypes;
            newSceneTypes.append(SceneTypeEnum::toGuiName(SceneTypeEnum::SCENE_TYPE_FULL));
            newSceneTypes.append(SceneTypeEnum::toGuiName(SceneTypeEnum::SCENE_TYPE_GENERIC));
            QComboBox* newSceneTypeComboBox = newSceneDialog.addComboBox("Scene Type", newSceneTypes);
            if (newSceneDialog.exec() == WuQDataEntryDialog::Accepted) {
                newSceneName = newSceneNameLineEdit->text();
                bool isValidType = false;
                SceneTypeEnum::Enum sceneType = SceneTypeEnum::fromGuiName(newSceneTypeComboBox->currentText(),
                                                                           &isValidType);
                
                newScene = new Scene(sceneType);
                newScene->setName(newSceneName);
                
                SceneAttributes* sceneAtributes = newScene->getAttributes();
                newScene->addClass(GuiManager::get()->saveToScene(sceneAtributes,
                                                                  "guiManager"));
                
                sceneFile->addScene(newScene);
            }
        }
        catch (const SceneException& se) {
            WuQMessageBox::errorOk(this, 
                                   se.whatString());
        }
    }
    
    loadSceneListWidget(newScene);
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
    Scene* scene = getSelectedScene();
    if (scene != NULL) {
        try {
            const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
            if (guiManagerClass->getName() != "guiManager") {
                throw SceneException("Top level scene class should be guiManager but it is: "
                                     + guiManagerClass->getName());
            }
            const SceneAttributes* sceneAttributes = scene->getAttributes();
            GuiManager::get()->restoreFromScene(sceneAttributes, 
                                                guiManagerClass);
        }
        catch (const SceneException& se) {
            WuQMessageBox::errorOk(this, 
                                   se.whatString());
        }
    }
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


