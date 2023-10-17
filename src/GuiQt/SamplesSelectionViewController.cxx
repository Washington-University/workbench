
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

#define __SAMPLES_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "SamplesSelectionViewController.h"
#undef __SAMPLES_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "SamplesFile.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupAndTabItemViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesSamples.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SamplesSelectionViewController
 * \brief View controller for display of samples.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parentObjectName
 *     Name of parent object
 * @param parent
 *     The parent widget.
 */
SamplesSelectionViewController::SamplesSelectionViewController(const int32_t browserWindowIndex,
                                                                     const QString& parentObjectName,
                                                                     QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    const QString objectNamePrefix(parentObjectName
                                   + ":SampleSelection");
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    QLabel* groupLabel = new QLabel("Group");
    
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this,
                                                          (objectNamePrefix
                                                           + ":DisplayGroup"),
                                                          "samples");
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupSelectionLayout = new QHBoxLayout();
    groupSelectionLayout->addWidget(groupLabel);
    groupSelectionLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupSelectionLayout->addStretch();
    QMargins groupLayoutMargins = groupSelectionLayout->contentsMargins();
    groupLayoutMargins.setBottom(0);
    groupLayoutMargins.setTop(0);

    m_displaySamplesCheckBox = new QCheckBox("Display Samples");
    m_displaySamplesCheckBox->setToolTip("Disables/enables display of samples in all windows");
    QObject::connect(m_displaySamplesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    m_displaySamplesCheckBox->setObjectName(objectNamePrefix
                                                + "DisplaySamples");
    macroManager->addMacroSupportToObject(m_displaySamplesCheckBox,
                                          "Enable display of samples");
    
    m_displaySampleNamesCheckBox = new QCheckBox("Display Sample Names");
    m_displaySampleNamesCheckBox->setToolTip("Disables/enables display of sample names in all windows");
    QObject::connect(m_displaySampleNamesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    m_displaySampleNamesCheckBox->setObjectName(objectNamePrefix
                                            + "DisplaySampleNames");
    macroManager->addMacroSupportToObject(m_displaySampleNamesCheckBox,
                                          "Enable display of sample names");

    m_sceneAssistant = new SceneClassAssistant();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displaySamplesCheckBox);
    layout->addWidget(m_displaySampleNamesCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(groupSelectionLayout);
    layout->addWidget(createSelectionWidget(), 100);
    
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    s_allSamplesSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
SamplesSelectionViewController::~SamplesSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
    
    s_allSamplesSelectionViewControllers.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
SamplesSelectionViewController::receiveEvent(Event* event)
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
        updateSampleSelections();
    }
}

/**
 * Update other selection samples selector since they may share properties
 */
void
SamplesSelectionViewController::updateOtherSampleViewControllers()
{
    for (std::set<SamplesSelectionViewController*>::iterator iter = s_allSamplesSelectionViewControllers.begin();
         iter != s_allSamplesSelectionViewControllers.end();
         iter++) {
        SamplesSelectionViewController* avc = *iter;
        if (avc != this) {
            avc->updateSampleSelections();
        }
    }
}

/**
 * Update the samples selections.
 */
void
SamplesSelectionViewController::updateSampleSelections()
{
    const DisplayPropertiesSamples* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesSamples();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    m_displaySamplesCheckBox->setChecked(dpa->isDisplaySamples());
    m_displaySampleNamesCheckBox->setChecked(dpa->isDisplaySampleNames());
    
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<SamplesFile*> samplesFiles(brain->getAllSamplesFiles());
    
    std::vector<DisplayGroupAndTabItemInterface*> fileItems;
    for (auto&sf : samplesFiles) {
        fileItems.push_back(sf);
    }
    
    const DisplayGroupEnum::Enum displayGroup = dpa->getDisplayGroupForTab(browserTabIndex);
    m_displayGroupComboBox->setSelectedDisplayGroup(displayGroup);
    
    const bool allowSampleSelectionFlag(true);
    
    m_selectionViewController->updateContent(fileItems,
                                             displayGroup,
                                             browserTabIndex,
                                             allowSampleSelectionFlag);
}

QWidget*
SamplesSelectionViewController::createSelectionWidget()
{
    m_selectionViewController = new DisplayGroupAndTabItemViewController(DataFileTypeEnum::ANNOTATION,
                                                                         m_browserWindowIndex);
    return m_selectionViewController;
}


/**
 * Called when one of the checkboxes is clicked.
 */
void
SamplesSelectionViewController::checkBoxToggled()
{
    DisplayPropertiesSamples* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesSamples();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }

    dpa->setDisplaySamples(m_displaySamplesCheckBox->isChecked());
    dpa->setDisplaySampleNames(m_displaySampleNamesCheckBox->isChecked());
    
    updateOtherSampleViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the display group combo box is changed.
 */
void
SamplesSelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    DisplayPropertiesSamples* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesSamples();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    dpa->setDisplayGroupForTab(browserTabIndex, displayGroup);
    
    updateSampleSelections();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
SamplesSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SamplesSelectionViewController",
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
SamplesSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
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

