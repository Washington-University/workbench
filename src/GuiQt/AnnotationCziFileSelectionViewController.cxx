
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_CZI_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "AnnotationCziFileSelectionViewController.h"
#undef __ANNOTATION_CZI_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CziImageFile.h"
#include "DataFileContentCopyMoveDialog.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCziFileSelectionViewController
 * \brief View controller for display of annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parentObjectName
 *     Name of parent object for macros
 * @param parent
 *     The parent widget.
 */
AnnotationCziFileSelectionViewController::AnnotationCziFileSelectionViewController(const int32_t browserWindowIndex,
                                                                                   const QString& parentObjectName,
                                                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_objectName = (parentObjectName + ":AnnotationCziFileSelectionViewController");
    m_sceneAssistant = new SceneClassAssistant();
    
    const Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    const DisplayPropertiesAnnotation* dpa(brain->getDisplayPropertiesAnnotation());

    m_displayCziAnnotationsCheckBox = new QCheckBox("Display Annotations in CZI Files");
    m_displayCziAnnotationsCheckBox->setChecked(dpa->isDisplayCziAnnotations());
    
    QObject::connect(m_displayCziAnnotationsCheckBox, &QCheckBox::clicked,
                     this, &AnnotationCziFileSelectionViewController::displayCziAnnotationsCheckBoxClicked);
    
    m_filesGridLayout = new QGridLayout();
    
    QWidget* layersWidget(new QWidget());
    QVBoxLayout* layerLayout(new QVBoxLayout(layersWidget));
    layerLayout->setContentsMargins(0, 0, 0, 0);
    layerLayout->addLayout(m_filesGridLayout);
    layerLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(layersWidget);
    scrollArea->setWidgetResizable(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displayCziAnnotationsCheckBox, 0, Qt::AlignLeft);
    layout->addWidget(scrollArea, 100);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    updateSelections();
}

/**
 * Destructor.
 */
AnnotationCziFileSelectionViewController::~AnnotationCziFileSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationCziFileSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdateFlag = false;
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        doUpdateFlag = true;
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
       EventUserInterfaceUpdate* eventUI = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUI);

        doUpdateFlag = true;
        
        eventUI->setEventProcessed();
    }
    
    if (doUpdateFlag) {
        updateSelections();
    }
}


/**
 * Update the annotation selections.
 */
void
AnnotationCziFileSelectionViewController::updateSelections()
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    
    std::vector<AnnotationFile*> cziAnnFiles(brain->getAllCziAnnotationFiles());
    const int32_t numCziAnnFiles(cziAnnFiles.size());

    const int32_t COLUMN_CHECKBOX(0);
    const int32_t COLUMN_COPY_BUTTON(1);
    const int32_t COLUMN_STRETCH(2);
    m_filesGridLayout->setColumnStretch(COLUMN_STRETCH, 100);

    const int32_t numRows(m_enableCheckBoxes.size());
    
    const AString toolButtonToolTip("Copy Annotations (CZI Graphics) from the CZI File "
                                    "to the Scene Annotations or an Annotation File");
    for (int32_t i = 0; i < numCziAnnFiles; i++) {
        if (i >= numRows) {
            QCheckBox* cb(new QCheckBox(""));
            QObject::connect(cb, &QCheckBox::clicked,
                             [=]() { checkBoxClicked(i); });
            QToolButton* tb(new QToolButton());
            QObject::connect(tb, &QToolButton::clicked,
                             [=]() { toolButtonClicked(i); });
            tb->setText("Copy");
            tb->setToolTip(WuQtUtilities::createWordWrappedToolTipText(toolButtonToolTip));
            m_enableCheckBoxes.push_back(cb);
            m_copyToolButtons.push_back(tb);
            m_cziImageFiles.push_back(NULL);
            m_filesGridLayout->addWidget(cb, i, COLUMN_CHECKBOX);
            m_filesGridLayout->addWidget(tb, i, COLUMN_COPY_BUTTON);
        }
        
        CaretAssertVectorIndex(cziAnnFiles, i);
        const AnnotationFile* annFile(cziAnnFiles[i]);
        CaretAssert(annFile);
        const CziImageFile* cziImageFile(annFile->getParentCziImageFile());
        CaretAssert(cziImageFile);
        
        CaretAssertVectorIndex(m_enableCheckBoxes, i);
        CaretAssertVectorIndex(m_copyToolButtons, i);
        m_enableCheckBoxes[i]->setText(cziImageFile->getFileNameNoPath());
        m_enableCheckBoxes[i]->setChecked(cziImageFile->isAnnotationFileDisplayed());
        m_enableCheckBoxes[i]->setVisible(true);
        m_copyToolButtons[i]->setVisible(true);
        m_cziImageFiles[i] = const_cast<CziImageFile*>(cziImageFile);
    }
    
    for (int32_t i = numCziAnnFiles; i < numRows; i++) {
        CaretAssertVectorIndex(m_enableCheckBoxes, i);
        CaretAssertVectorIndex(m_copyToolButtons, i);
        m_enableCheckBoxes[i]->setVisible(false);
        m_copyToolButtons[i]->setVisible(false);
    }
    
    CaretAssert(m_enableCheckBoxes.size() == m_copyToolButtons.size());
    CaretAssert(m_enableCheckBoxes.size() == m_cziImageFiles.size());
}

/**
 * Called when Display CZI annotations in button clicked
 * @param checked
 *    New checked status
 */
void
AnnotationCziFileSelectionViewController::displayCziAnnotationsCheckBoxClicked(bool checked)
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    DisplayPropertiesAnnotation* dpa(brain->getDisplayPropertiesAnnotation());
    dpa->setDisplayCziAnnotations(checked);
    updateUserInterfaceAndGraphics();
}

/**
 * Called when a checkbox is clicked
 * @param index
 *    Index of checkbox
 */
void
AnnotationCziFileSelectionViewController::checkBoxClicked(const int32_t index)
{
    CaretAssertVectorIndex(m_cziImageFiles, index);
    CziImageFile* cziImageFile(m_cziImageFiles[index]);
    CaretAssert(cziImageFile);
    CaretAssertVectorIndex(m_enableCheckBoxes, index);
    cziImageFile->setAnnotationFileDisplayed(m_enableCheckBoxes[index]->isChecked());
    
    updateUserInterfaceAndGraphics();
}

/**
 * Called when a tool button is clicked
 * @param index
 *    Index of tool button
 */
void
AnnotationCziFileSelectionViewController::toolButtonClicked(const int32_t index)
{
    CaretAssertVectorIndex(m_cziImageFiles, index);
    CziImageFile* cziImageFile(m_cziImageFiles[index]);
    CaretAssert(cziImageFile);
    AnnotationFile* annotationFile(cziImageFile->getAnnotationFile());

    DataFileContentCopyMoveInterface* copyMoveInterfaceFile = dynamic_cast<DataFileContentCopyMoveInterface*>(annotationFile);
    CaretAssert(copyMoveInterfaceFile);
    
    Brain* brain = GuiManager::get()->getBrain();
    
    std::vector<CaretDataFile*> copyMoveDataFiles;
    
    std::vector<AnnotationFile*> annotationFiles;
    brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    copyMoveDataFiles.insert(copyMoveDataFiles.end(),
                             annotationFiles.begin(),
                             annotationFiles.end());

    
    std::vector<DataFileContentCopyMoveInterface*> copyMoveDestinationFiles;
    for (std::vector<CaretDataFile*>::iterator fileIter = copyMoveDataFiles.begin();
         fileIter != copyMoveDataFiles.end();
         fileIter++) {
        DataFileContentCopyMoveInterface* cmFile = dynamic_cast<DataFileContentCopyMoveInterface*>(*fileIter);
        if (cmFile != NULL) {
            copyMoveDestinationFiles.push_back(cmFile);
            /*
             * Turn of display of annotation from this CZI file
             * since they have been copied to either scene
             * annotations or an annotation file
             */
            cziImageFile->setAnnotationFileDisplayed(false);
        }
    }
    
    DataFileContentCopyMoveDialog::Options options;
    options.setCziAnnotationFile(true);
    DataFileContentCopyMoveDialog copyMoveDialog(m_browserWindowIndex,
                                                 copyMoveInterfaceFile,
                                                 copyMoveDestinationFiles,
                                                 options,
                                                 this);
    copyMoveDialog.exec();
    
    updateUserInterfaceAndGraphics();
}

/**
 * Update user interface and graphics
 */
void
AnnotationCziFileSelectionViewController::updateUserInterfaceAndGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
AnnotationCziFileSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationCziFileSelectionViewController",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
AnnotationCziFileSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
}

