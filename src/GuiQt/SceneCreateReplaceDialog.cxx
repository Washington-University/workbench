
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include <algorithm>

#define __SCENE_CREATE_REPLACE_DIALOG_DECLARE__
#include "SceneCreateReplaceDialog.h"
#undef __SCENE_CREATE_REPLACE_DIALOG_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabGetAllViewed.h"
#include "EventImageCapture.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "SessionManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::SceneCreateReplaceDialog 
 * \brief Dialog for creating or replacing a scene.
 * \ingroup GuiQt
 *
 */

#include "Scene.h"

/**
 * Constructor.
 *
 * @param dialogTitle
 *     Title of the dialog.
 * @param parent
 *     Parent on which dialog is displayed.
 * @param sceneFile
 *     Scene file to which scene is added or replaced.
 * @param sceneToReplace
 *     If non-NULL, this scene will be replaced.
 */
SceneCreateReplaceDialog::SceneCreateReplaceDialog(const AString& dialogTitle,
                                                   QWidget* parent,
                                                   SceneFile* sceneFile,
                                                   Scene* sceneToReplace)
: WuQDialogModal(dialogTitle,
                 parent)
{
    CaretAssert(sceneFile);
    
    if ( ! s_previousSelectionsValid) {
        s_previousSelectionsValid = true;
        
        s_previousSelections.m_addAllLoadedFiles          = true;
        s_previousSelections.m_addAllTabs                 = true;
        s_previousSelections.m_addModifiedPaletteSettings = false;
        s_previousSelections.m_addSpecFileNameToScene     = true;
    }
    
    m_sceneFile = sceneFile;
    m_sceneToReplace = sceneToReplace;
    m_sceneThatWasCreated = NULL;
    
    /*
     * Options widget
     */
    QLabel* optionsLabel = new QLabel("Options");
    QWidget* optionsWidget = createSceneOptionsWidget();
    
    /*
     * Create scene information widgets
     */
    QLabel* nameLabel = new QLabel("Name");
    m_nameLineEdit = new QLineEdit();
    
    QLabel* descriptionLabel = new QLabel("Description");
    m_descriptionTextEdit = new QPlainTextEdit();
    
    const Qt::Alignment labelAlignment = (Qt::AlignLeft | Qt::AlignTop);
    
    /*
     * Layout for  widgets
     */
    int32_t columnCounter = 0;
    const int32_t labelColumn  = columnCounter++;
    const int32_t widgetColumn = columnCounter++;
    QGridLayout* infoGridLayout = new QGridLayout();
    infoGridLayout->setColumnStretch(labelColumn,   0);
    infoGridLayout->setColumnStretch(widgetColumn, 100);
    int32_t rowCounter = 0;
    infoGridLayout->setRowStretch(rowCounter, 0);
    infoGridLayout->addWidget(nameLabel,
                              rowCounter, labelColumn,
                              labelAlignment);
    infoGridLayout->addWidget(m_nameLineEdit,
                              rowCounter, widgetColumn);
    rowCounter++;
    infoGridLayout->setRowStretch(rowCounter, 100);
    infoGridLayout->addWidget(descriptionLabel,
                              rowCounter, labelColumn,
                              labelAlignment);
    infoGridLayout->addWidget(m_descriptionTextEdit,
                              rowCounter, widgetColumn);
    rowCounter++;
    infoGridLayout->setRowStretch(rowCounter, 0);
    infoGridLayout->addWidget(optionsLabel,
                              rowCounter, labelColumn,
                              labelAlignment);
    infoGridLayout->addWidget(optionsWidget,
                              rowCounter, widgetColumn);
    rowCounter++;
    
    /*
     * Add the layout to a widget and return the widget.
     */
    QWidget* infoWidget = new QWidget();
    infoWidget->setLayout(infoGridLayout);

    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(infoWidget,
                            100);
    
    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_NEVER);

    AString description;
    
    std::vector<BrainBrowserWindow*> windows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (std::vector<BrainBrowserWindow*>::iterator iter = windows.begin();
         iter != windows.end();
         iter++) {
        BrainBrowserWindow* window = *iter;
        description.appendWithNewLine(window->toString());
        description.appendWithNewLine("");
    }
    
    if (sceneToReplace != NULL) {
        m_nameLineEdit->setText(sceneToReplace->getName());
        m_descriptionTextEdit->setPlainText(sceneToReplace->getDescription());
    }
    else {
        m_descriptionTextEdit->setPlainText(description);
    }
    
    setMinimumWidth(500);
    setMinimumHeight(300);
}

/**
 * Destructor.
 */
SceneCreateReplaceDialog::~SceneCreateReplaceDialog()
{
}

/**
 * Static method that creates a dialog for creating a new scene and 
 * returns the scene that was created or NULL if scene was not created.
 *
 * @param parent
 *     Parent widget on which dialog is displayed.
 * @param sceneFile
 *     Scene file to which new scene is added.
 * @return
 *     Scene that was created or NULL if user cancelled or there was an error.
 */
Scene*
SceneCreateReplaceDialog::createNewScene(QWidget* parent,
                                         SceneFile* sceneFile)
{
    QScopedPointer<SceneCreateReplaceDialog> dialog(new SceneCreateReplaceDialog("Create New Scene",
                                                                                 parent,
                                                                                 sceneFile,
                                                                                 NULL));
    dialog->exec();
    
    return dialog->m_sceneThatWasCreated;
}

/**
 * Static method that creates a dialog for replacing and existing scene and
 * returns the scene that was created or NULL if scene was not created.
 *
 * @param parent
 *     Parent widget on which dialog is displayed.
 * @param sceneFile
 *     File in which the given scene exists and will be replaced.
 * @param sceneToReplace
 *     Scene that is being replaced.  If the user presses the OK button
 *     to replace this scene it will be destroyed so the pointer must not
 *     be deferenced at any time after calling this method.
 * @return
 *     Scene that was created or NULL if user cancelled or there was an error.
 */
Scene*
SceneCreateReplaceDialog::replaceExistingScene(QWidget* parent,
                                               SceneFile* sceneFile,
                                               Scene* sceneToReplace)
{
    const AString title = ("Replace Scene: "
                           + sceneToReplace->getName());
    
    QScopedPointer<SceneCreateReplaceDialog> dialog(new SceneCreateReplaceDialog(title,
                                                                                 parent,
                                                                                 sceneFile,
                                                                                 sceneToReplace));
    
    /*
     * Error checking will not allow two scenes with the same name
     * so temporarily modify name of scene being replaced and restore
     * it if the user does not hit OK.
     */
    const AString savedSceneName = sceneToReplace->getName();
    sceneToReplace->setName("slkkjdlkfjaslfjdljfdkljdfjsdfj");
    
    if (dialog->exec() == SceneCreateReplaceDialog::Rejected) {
        sceneToReplace->setName(savedSceneName);
    }
    
    return dialog->m_sceneThatWasCreated;
}

/**
 * Add an image to the scene.
 *
 * @param scene
 *    Scene to which image is added.
 */
void
SceneCreateReplaceDialog::addImageToScene(Scene* scene)
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
            
            const AString PREFERRED_IMAGE_FORMAT = "jpg";
            
            QByteArray byteArray;
            compositeImageFile.getImageInByteArray(byteArray,
                                                   PREFERRED_IMAGE_FORMAT);
            
            scene->getSceneInfo()->setImageBytes(byteArray,
                                                 PREFERRED_IMAGE_FORMAT);
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::errorOk(this,
                                   (dfe.whatString()
                                    + "\n\nEven though image failed, scene was created."));
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
 * @return Widget containing the scene options widgets.
 */
QWidget*
SceneCreateReplaceDialog::createSceneOptionsWidget()
{
    /*
     * Create scene options widgets
     */
    m_addSpecFileNameToSceneCheckBox = new QCheckBox("Add name of spec file to scene");
    m_addSpecFileNameToSceneCheckBox->setChecked(s_previousSelections.m_addSpecFileNameToScene);
    WuQtUtilities::setWordWrappedToolTip(m_addSpecFileNameToSceneCheckBox,
                                         "Include name of spec file in the scene");
    
    m_addAllTabsCheckBox = new QCheckBox("Add all tabs to scene");
    m_addAllTabsCheckBox->setChecked(s_previousSelections.m_addAllTabs);
    WuQtUtilities::setWordWrappedToolTip(m_addAllTabsCheckBox,
                                         "Add all tabs to the scene.  When this option is selected, "
                                         "the scene will be larger and require additional time to "
                                         "load.  If NOT selected, only the selected tab in each "
                                         "window is saved to the scene.");
    
    m_addAllLoadedFilesCheckBox = new QCheckBox("Add all loaded files to scene");
    m_addAllLoadedFilesCheckBox->setChecked(s_previousSelections.m_addAllLoadedFiles);
    WuQtUtilities::setWordWrappedToolTip(m_addAllLoadedFilesCheckBox,
                                         "Add all loaded files to scene.  When this option is selected, "
                                         "the scene may require additional time to load as file that "
                                         "play no role in reproducing the scene will be loaded.  If NOT "
                                         "selected, the scene may load more quickly.");
    
    m_addModifiedPaletteSettingsCheckBox = new QCheckBox("Add modified palette settings to scene");
    m_addModifiedPaletteSettingsCheckBox->setChecked(s_previousSelections.m_addModifiedPaletteSettings);
    WuQtUtilities::setWordWrappedToolTip(m_addModifiedPaletteSettingsCheckBox,
                                         "The palette settings are saved within each data files that maps "
                                         "its data to brainordinates.  However, there are instances in which "
                                         "the user wants the scene to display the data with palette settings "
                                         "that are different from those in the file.  If this option is "
                                         "selected, modified palettes settings will be saved to the scene.");
    
    /*
     * Layout for scene options widgets
     */
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    optionsLayout->addWidget(m_addSpecFileNameToSceneCheckBox);
    optionsLayout->addWidget(m_addAllTabsCheckBox);
    optionsLayout->addWidget(m_addAllLoadedFilesCheckBox);
    optionsLayout->addWidget(m_addModifiedPaletteSettingsCheckBox);
    
    /*
     * Add the layout to a widget and return the widget.
     */
    QFrame* optionsWidget = new QFrame();
    optionsWidget->setFrameStyle(QFrame::Box
                         | QFrame::Plain);
    optionsWidget->setLineWidth(1);
//    QWidget* optionsWidget = new QWidget();
    optionsWidget->setLayout(optionsLayout);
    return optionsWidget;
}

/**
 * Gets called if the user presses the OK button.
 */
void
SceneCreateReplaceDialog::okButtonClicked()
{
    s_previousSelections.m_addAllLoadedFiles          = m_addAllLoadedFilesCheckBox->isChecked();
    s_previousSelections.m_addAllTabs                 = m_addAllTabsCheckBox->isChecked();
    s_previousSelections.m_addModifiedPaletteSettings = m_addModifiedPaletteSettingsCheckBox->isChecked();
    s_previousSelections.m_addSpecFileNameToScene     = m_addSpecFileNameToSceneCheckBox->isChecked();
    
    const AString newSceneName = m_nameLineEdit->text();
    
    AString errorMessage;
    if (newSceneName.isEmpty()) {
        errorMessage = "Scene Name is empty.";
    }
    else if (m_sceneFile->getSceneWithName(newSceneName) != NULL) {
        errorMessage = ("An existing scene uses the name \""
                        + newSceneName
                        + "\".  Scene names must be unique.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    Scene* newScene = new Scene(SceneTypeEnum::SCENE_TYPE_FULL);
    Scene::setSceneBeingCreated(newScene);
    newScene->setName(newSceneName);
    newScene->setDescription(m_descriptionTextEdit->toPlainText());
    
    /*
     * Get all browser tabs and only save transformations for tabs
     * that are valid.
     */
    std::vector<int32_t> tabIndices;
    if (m_addAllTabsCheckBox->isChecked()) {
        EventBrowserTabGetAll getAllTabs;
        EventManager::get()->sendEvent(getAllTabs.getPointer());
        tabIndices = getAllTabs.getBrowserTabIndices();
    }
    else {
        EventBrowserTabGetAllViewed getViewedTabs;
        EventManager::get()->sendEvent(getViewedTabs.getPointer());
        tabIndices = getViewedTabs.getViewdedBrowserTabIndices();
    }
    std::sort(tabIndices.begin(),
              tabIndices.end());
    
    SceneAttributes* sceneAttributes = newScene->getAttributes();
    sceneAttributes->setSceneFileName(m_sceneFile->getFileName());
    sceneAttributes->setIndicesOfTabsForSavingToScene(tabIndices);
    sceneAttributes->setSpecFileNameSavedToScene(m_addSpecFileNameToSceneCheckBox->isChecked());
    sceneAttributes->setAllLoadedFilesSavedToScene(m_addAllLoadedFilesCheckBox->isChecked());
    
    newScene->addClass(GuiManager::get()->saveToScene(sceneAttributes,
                                                      "guiManager"));
    
    addImageToScene(newScene);
    
    if (m_sceneToReplace != NULL) {
        m_sceneFile->replaceScene(newScene,
                                  m_sceneToReplace);
    }
    else {
        m_sceneFile->addScene(newScene);
    }
    
    m_sceneThatWasCreated = newScene;
    
    Scene::setSceneBeingCreated(NULL);
    
    WuQDialogModal::okButtonClicked();
}


