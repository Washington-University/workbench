
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

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
//#include <QTabWidget>
#include <QToolButton>

#define __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "LabelSelectionViewController.h"
#undef __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CiftiBrainordinateLabelDynamicFile.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "SceneClass.h"
#include "VolumeFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionViewController 
 * \brief Widget for controlling display of labels
 * \ingroup GuiQt
 *
 * Widget for controlling the display of labels including
 * different display groups.
 */

/**
 * Constructor.
 */
LabelSelectionViewController::LabelSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_labelsDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_labelsDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(labelDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_labelsDisplayGroupComboBox->getWidget());
    groupLayout->addStretch(); 
    
    QWidget* selectionWidget = this->createSelectionWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(groupLayout);
    layout->addWidget(selectionWidget, 0, Qt::AlignLeft);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    LabelSelectionViewController::allLabelSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
LabelSelectionViewController::~LabelSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    LabelSelectionViewController::allLabelSelectionViewControllers.erase(this);
}


QWidget* 
LabelSelectionViewController::createSelectionWidget()
{
    m_labelClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex);
    
    return m_labelClassNameHierarchyViewController;
}

/**
 * Called when the label display group combo box is changed.
 */
void 
LabelSelectionViewController::labelDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    /*
     * Update selected display group in model.
     */
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesLabels* dsb = brain->getDisplayPropertiesLabels();
    dsb->setDisplayGroupForTab(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateLabelViewController();
    
    /*
     * Apply the changes.
     */
    processLabelSelectionChanges();
}

/**
 * Update the label selection widget.
 */
void 
LabelSelectionViewController::updateLabelViewController()
{
    setWindowTitle("Labels");
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesLabels* dpb = brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    
    m_labelsDisplayGroupComboBox->setSelectedDisplayGroup(dpb->getDisplayGroupForTab(browserTabIndex));
    
    /*
     * Get all of label type files
     */
    std::vector<LabelFile*> allLabelFiles;
    std::vector<CiftiBrainordinateLabelFile*> allCiftiLabelFiles;
    std::vector<VolumeFile*> allVolumeLabelFiles;

    std::vector<CaretMappableDataFile*> allMapDataFiles;
    brain->getAllMappableDataFiles(allMapDataFiles);
    for (auto file : allMapDataFiles) {
        switch (file->getDataFileType()) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                allCiftiLabelFiles.push_back(dynamic_cast<CiftiBrainordinateLabelFile*>(file));
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                allLabelFiles.push_back(dynamic_cast<LabelFile*>(file));
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::VOLUME:
                if (file->isMappedWithLabelTable()) {
                    allVolumeLabelFiles.push_back(dynamic_cast<VolumeFile*>(file));
                }
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
        }
    }
    /*
     * Update the class/name hierarchy
     */
    m_labelClassNameHierarchyViewController->updateContents(allLabelFiles,
                                                            allCiftiLabelFiles,
                                                            allVolumeLabelFiles,
                                                             displayGroup);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
LabelSelectionViewController::updateOtherLabelViewControllers()
{
    for (std::set<LabelSelectionViewController*>::iterator iter = LabelSelectionViewController::allLabelSelectionViewControllers.begin();
         iter != LabelSelectionViewController::allLabelSelectionViewControllers.end();
         iter++) {
        LabelSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateLabelViewController();
        }
    }
}

/**
 * Gets called when label selections are changed.
 */
void 
LabelSelectionViewController::processLabelSelectionChanges()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(browserTabContent);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesLabels* dsb = brain->getDisplayPropertiesLabels();
    dsb->setDisplayGroupForTab(browserTabIndex, 
                         m_labelsDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
LabelSelectionViewController::processSelectionChanges()
{
    updateOtherLabelViewControllers();
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
LabelSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isToolBoxUpdate()) {
                doUpdate = true;
                uiEvent->setEventProcessed();
            }
        }
    }

    if (doUpdate) {
        updateLabelViewController();
    }
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
LabelSelectionViewController::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionViewController",
                                            1);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
LabelSelectionViewController::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
}



