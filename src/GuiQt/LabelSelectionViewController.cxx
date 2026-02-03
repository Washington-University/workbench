
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

#include <QGridLayout>
#include <QLabel>
#include <QLayout>

#define __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "LabelSelectionViewController.h"
#undef __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "VolumeFile.h"
#include "WuQMacroManager.h"

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
 *
 * @param browserWindowIndex
 *    Index of browser window
 * @param parentObjectName
 *    Name of parent object
 * @param parent
 *    The parent object
 */
LabelSelectionViewController::LabelSelectionViewController(const int32_t browserWindowIndex,
                                                           const QString& parentObjectName,
                                                           QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":Label")
{
    m_browserWindowIndex = browserWindowIndex;
    
    QWidget* selectionWidget = this->createSelectionWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(selectionWidget);
    
    LabelSelectionViewController::allLabelSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
LabelSelectionViewController::~LabelSelectionViewController()
{
    LabelSelectionViewController::allLabelSelectionViewControllers.erase(this);
}


QWidget* 
LabelSelectionViewController::createSelectionWidget()
{
    m_labelClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex,
                                                                                      (m_objectNamePrefix
                                                                                       + ":Selection"),
                                                                                      "labels",
                                                                                      this);
    return m_labelClassNameHierarchyViewController;
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
    
    /*
     * Get all of label files.
     */
    std::vector<LabelFile*> allLabelFiles;
    const int numBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t ibs = 0; ibs < numBrainStructures; ibs++) {
        BrainStructure* brainStructure = brain->getBrainStructure(ibs);
        const int32_t numLabelFiles = brainStructure->getNumberOfLabelFiles();
        for (int32_t ilf = 0; ilf < numLabelFiles; ilf++) {
            allLabelFiles.push_back(brainStructure->getLabelFile(ilf));
        }
    }
    
    /*
     * Get all CIFTI label files
     */
    std::vector<CiftiBrainordinateLabelFile*> allCiftiLabelFiles;
    const int32_t numCiftiLabelFiles = brain->getNumberOfConnectivityDenseLabelFiles();
    for (int32_t iclf = 0; iclf < numCiftiLabelFiles; iclf++) {
        allCiftiLabelFiles.push_back(brain->getConnectivityDenseLabelFile(iclf));
    }
    
    /*
     * Get all Volume Files that are mapped with label tables
     */
    std::vector<VolumeFile*> allVolumeLabelFiles;
    const int32_t numVolumeFiles = brain->getNumberOfVolumeFiles();
    for (int32_t iVol = 0; iVol < numVolumeFiles; iVol++) {
        VolumeFile* vf = brain->getVolumeFile(iVol);
        if (vf->isMappedWithLabelTable()) {
            allVolumeLabelFiles.push_back(vf);
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
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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



