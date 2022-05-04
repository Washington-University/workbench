
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

#define __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_DECLARE__
#include "AnnotationTextSubstitutionViewController.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_VIEW_CONTROLLER_DECLARE__

#include <algorithm>

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

#include "AnnotationTextSubstitutionFile.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesAnnotationTextSubstitution.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventMapYokingSelectMap.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionViewController 
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
AnnotationTextSubstitutionViewController::AnnotationTextSubstitutionViewController(const int32_t browserWindowIndex,
                                                                                   const QString& parentObjectName,
                                                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_enableSubstitutionsCheckBox = new QCheckBox("Enable Substitutions");
    QObject::connect(m_enableSubstitutionsCheckBox, &QCheckBox::clicked,
                     this, &AnnotationTextSubstitutionViewController::enableCheckBoxClicked);
    m_enableSubstitutionsCheckBox->setObjectName(parentObjectName
                                                 + ":Substitutions:Enable");
    m_enableSubstitutionsCheckBox->setToolTip("Enable Text Annotation Substitutions");
    WuQMacroManager::instance()->addMacroSupportToObject(m_enableSubstitutionsCheckBox,
                                                         "Enable annotation subsitutions");
    
    QButtonGroup* buttonGroup = new QButtonGroup();
    
    QWidget* fileWidget = new QWidget();
    fileWidget->setSizePolicy(fileWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    QGridLayout* fileGridLayout = new QGridLayout(fileWidget);
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_FILES; i++) {
        QRadioButton* rb = new QRadioButton(" ");
        QObject::connect(rb, &QCheckBox::clicked,
                         [=] { fileRadioButtonClicked(i); } );
        buttonGroup->addButton(rb);
        
        MapYokingGroupComboBox* ycb = new MapYokingGroupComboBox(this);
        QObject::connect(ycb, &MapYokingGroupComboBox::itemActivated,
                         [=] { fileMapYokingGroupComboBoxChanged(i); } );
        
        QSpinBox* sb = new QSpinBox();
        sb->setRange(1, 100);
#if QT_VERSION >= 0x050700
        QObject::connect(sb, QOverload<int>::of(&QSpinBox::valueChanged),
                         [=] { valueIndexSpinBoxChanged(i); } );
#else
        QObject::connect(sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                         [=] { valueIndexSpinBoxChanged(i); } );
#endif
        
        QLabel* fl = new QLabel();
        
        if (i == 0) {
            const int row = fileGridLayout->rowCount();
            int32_t column = 0;
            fileGridLayout->setColumnStretch(column, 0);
            fileGridLayout->addWidget(new QLabel("On"), row, column++, Qt::AlignHCenter);
            fileGridLayout->setColumnStretch(column, 0);
            fileGridLayout->addWidget(new QLabel("Yoke"), row, column++, Qt::AlignHCenter);
            fileGridLayout->setColumnStretch(column, 0);
            fileGridLayout->addWidget(new QLabel("Index"), row, column++, Qt::AlignHCenter);
            fileGridLayout->setColumnStretch(column, 100);
            fileGridLayout->addWidget(new QLabel("File"), row, column++, Qt::AlignHCenter);
        }
        
        const int row = fileGridLayout->rowCount();
        int32_t column = 0;
        fileGridLayout->addWidget(rb, row, column++);
        fileGridLayout->addWidget(ycb->getWidget(), row, column++);
        fileGridLayout->addWidget(sb, row, column++);
        fileGridLayout->addWidget(fl, row, column++);
        
        FileGuiControls* fgc = new FileGuiControls(rb, sb, ycb, fl);
        m_fileGuiControls.push_back(fgc);
    }
    
    QScrollArea* fileScrollArea = new QScrollArea();
    fileScrollArea->setWidget(fileWidget);
    fileScrollArea->setWidgetResizable(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_enableSubstitutionsCheckBox, 0);
    layout->addWidget(fileScrollArea, 0);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    s_allAnnotationTextSubstitutionViewControllers.insert(this);
    
    updateSelections();
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionViewController::~AnnotationTextSubstitutionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
    
    s_allAnnotationTextSubstitutionViewControllers.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationTextSubstitutionViewController::receiveEvent(Event* event)
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
 * Called when enable checkbox is clicked.
 *
 * @param clicked
 *     New checked status
 */
void
AnnotationTextSubstitutionViewController::enableCheckBoxClicked(bool clicked)
{
    DisplayPropertiesAnnotationTextSubstitution* dpats = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotationTextSubstitution();
    dpats->setEnableSubstitutions(clicked);
    updateOtherViewControllersAndGraphics(-1);
}

/**
 * Called when a file radio button is clicked.
 * 
 * @param index
 *     Index of radio button that was clicked.
 */
void
AnnotationTextSubstitutionViewController::fileRadioButtonClicked(int32_t fileIndex)
{
    if (fileIndex >= 0) {
        CaretAssertVectorIndex(m_fileGuiControls, fileIndex);
        DisplayPropertiesAnnotationTextSubstitution* dpats = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotationTextSubstitution();
        dpats->setSelectedFile(m_fileGuiControls[fileIndex]->m_dataFile);
        updateOtherViewControllersAndGraphics(fileIndex);
    }
}

/**
 * Called when a file value index spin box is changed.
 *
 * @param fileIndex
 *     Index of file value index spin box that was changed.
 */
void
AnnotationTextSubstitutionViewController::valueIndexSpinBoxChanged(int32_t fileIndex)
{
    if (fileIndex >= 0) {
        CaretAssertVectorIndex(m_fileGuiControls, fileIndex);
        AnnotationTextSubstitutionFile* textSubFile = m_fileGuiControls[fileIndex]->m_dataFile;
        CaretAssert(textSubFile);
        QSpinBox* spinBox = m_fileGuiControls[fileIndex]->m_valueIndexSpinBox;
        CaretAssert(spinBox);
        const int32_t valueIndex = (spinBox->value()
                                    - spinBox->minimum());
        MapYokingGroupEnum::Enum mapYoking = textSubFile->getMapYokingGroup();
        if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            CaretMappableDataFile* nullMapFile(NULL);
            MediaFile* nullMediaFile(NULL);
            EventMapYokingSelectMap selectMapEvent(mapYoking,
                                                   nullMapFile,
                                                   textSubFile,
                                                   nullMediaFile,
                                                   valueIndex,
                                                   true);
            EventManager::get()->sendEvent(selectMapEvent.getPointer());
        }
        else {
            textSubFile->setSelectedValueIndex(valueIndex);
        }
        updateOtherViewControllersAndGraphics(fileIndex);
    }
    
}

/**
 * Called when a file map yoking group combo box is changed.
 *
 * @param fileIndex
 *     Index of file map yoking group combo box that was changed.
 */
void
AnnotationTextSubstitutionViewController::fileMapYokingGroupComboBoxChanged(int32_t fileIndex)
{
    if (fileIndex >= 0) {
        CaretAssertVectorIndex(m_fileGuiControls, fileIndex);
        AnnotationTextSubstitutionFile* textSubFile = m_fileGuiControls[fileIndex]->m_dataFile;
        CaretAssert(textSubFile);
        
        MapYokingGroupComboBox* mapComboBox = m_fileGuiControls[fileIndex]->m_mapYokingGroupComboBox;
        mapComboBox->validateYokingChange(textSubFile);
        
        updateOtherViewControllersAndGraphics(fileIndex);
    }
}

/**
 * Update other selection annotation selectors since they may share properties
 * and also update graphics.
 *
 * @param fileIndex
 *    Index of file.
 */
void
AnnotationTextSubstitutionViewController::updateOtherViewControllersAndGraphics(const int32_t fileIndex)
{
    for (auto avc : s_allAnnotationTextSubstitutionViewControllers) {
        if (avc != this) {
            avc->updateSelections();
        }
    }

    bool updateAllFlag = false;
    if (fileIndex >= 0) {
        CaretAssertVectorIndex(m_fileGuiControls, fileIndex);
        if (m_fileGuiControls[fileIndex]->m_mapYokingGroupComboBox->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            updateAllFlag = true;
        }
    }
    if (updateAllFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the annotation selections.
 */
void
AnnotationTextSubstitutionViewController::updateSelections()
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    
    DisplayPropertiesAnnotationTextSubstitution* dpats = brain->getDisplayPropertiesAnnotationTextSubstitution();
    m_enableSubstitutionsCheckBox->setChecked(dpats->isEnableSubstitutions());
    AnnotationTextSubstitutionFile* selectedFile = dpats->getSelectedFile();
    
    std::vector<AnnotationTextSubstitutionFile*> allFiles;
    brain->getAnnotationTextSubstitutionFiles(allFiles);
    
    const int32_t numFiles = static_cast<int32_t>(allFiles.size());
    const int32_t numItems = static_cast<int32_t>(m_fileGuiControls.size());
    for (int32_t i = 0; i < numItems; i++) {
        CaretAssertVectorIndex(m_fileGuiControls, i);
        FileGuiControls* fgc = m_fileGuiControls[i];
        CaretAssert(fgc);
        
        /*
         * Spin boxes always emit signals when value changed
         */
        QSignalBlocker spinBoxBlocker(fgc->m_valueIndexSpinBox);
        
        bool visibleFlag = false;
        if (i < numFiles) {
            visibleFlag = true;
            
            CaretAssertVectorIndex(allFiles, i);
            AnnotationTextSubstitutionFile* file = allFiles[i];
            CaretAssert(file);
            fgc->m_selectionRadioButton->setChecked(file == selectedFile);
            fgc->m_mapYokingGroupComboBox->setMapYokingGroup(file->getMapYokingGroup());
            const int32_t maxValue = std::max(1, file->getNumberOfValues());
            fgc->m_valueIndexSpinBox->setRange(1, maxValue);
            fgc->m_valueIndexSpinBox->setValue(file->getSelectedValueIndex()
                                               + fgc->m_valueIndexSpinBox->minimum());
            fgc->m_fileNameLabel->setText(file->getFileNameNoPath());
            fgc->m_fileNameLabel->setToolTip(file->getFileName());
            fgc->m_dataFile = file;
        }
        else {
            fgc->m_dataFile = NULL;
        }
        
        fgc->m_selectionRadioButton->setVisible(visibleFlag);
        fgc->m_mapYokingGroupComboBox->getWidget()->setVisible(visibleFlag);
        fgc->m_valueIndexSpinBox->setVisible(visibleFlag);
        fgc->m_fileNameLabel->setVisible(visibleFlag);
    }
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
AnnotationTextSubstitutionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationTextSubstitutionViewController",
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
AnnotationTextSubstitutionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
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

