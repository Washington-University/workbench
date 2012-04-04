
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

#include <QActionGroup>

#define __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__
#include "BrainOpenGLWidgetContextMenu.h"
#undef __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderManager.h"
#include "CursorDisplay.h"
#include "EventManager.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventInformationTextDisplay.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "IdentificationItemBorderSurface.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationManager.h"
#include "LabelFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "ModelDisplayController.h"
#include "Surface.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLWidgetContextMenu 
 * \brief Context (pop-up) menu for BrainOpenGLWidget
 *
 * Displays a menu in the BrainOpenGLWidget.  Content of menu
 * is dependent upon data under the cursor.
 */
/**
 * Constructor.
 * @param identificationManager
 *    The identification manager, provides data under the cursor.
 * @param parent
 *    Parent on which the menu is displayed.
 */
BrainOpenGLWidgetContextMenu::BrainOpenGLWidgetContextMenu(IdentificationManager* identificationManager,
                                                           BrowserTabContent* browserTabContent,
                                                           QWidget* parent)
: QMenu(parent)
{
    this->identificationManager = identificationManager;
    this->browserTabContent = browserTabContent;
    
    this->addAction("Remove Node Identification Symbols",
                    this,
                    SLOT(removeNodeIdentificationSymbolsSelected()));
    
    /*
     * Identify actions
     */
    std::vector<QAction*> identificationActions;
    if (this->identificationManager->getSurfaceBorderIdentification()->isValid()) {
        identificationActions.push_back(WuQtUtilities::createAction("Identify Border Under Mouse", 
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceBorderSelected())));
    }

    IdentificationItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    if (surfaceID->isValid()) {
        identificationActions.push_back(WuQtUtilities::createAction("Identify Node Under Mouse", 
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceNodeSelected())));
    }
    
    if (this->identificationManager->getVoxelIdentification()->isValid()) {
        identificationActions.push_back(WuQtUtilities::createAction("Identify Voxel Under Mouse", 
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifyVoxelSelected())));
    }
    
    if (identificationActions.empty() == false) {
        this->addSeparator();
        
        for (std::vector<QAction*>::iterator idIter = identificationActions.begin();
             idIter != identificationActions.end();
             idIter++) {
            this->addAction(*idIter);
        }
    }
    
    if (surfaceID->isValid()) {
        QActionGroup* connectivityActionGroup = new QActionGroup(this);
        QObject::connect(connectivityActionGroup, SIGNAL(triggered(QAction*)),
                         this, SLOT(parcelConnectivityActionSelected(QAction*)));
        
        /*
         * Connectivity actions
         */
        std::vector<QAction*> connectivityActions;
        Brain* brain = surfaceID->getBrain();
        Surface* surface = surfaceID->getSurface();
        const int32_t nodeNumber = surfaceID->getNodeNumber();
        ConnectivityLoaderManager* clm = brain->getConnectivityLoaderManager();
        const int32_t numConnLoaders = clm->getNumberOfConnectivityLoaderFiles();
        for (int32_t i = 0; i < numConnLoaders; i++) {
            ConnectivityLoaderFile* clf = clm->getConnectivityLoaderFile(i);
            if (clf->isDense()) {
                if (clf->isEmpty() == false) {
                    ModelDisplayController* model = this->browserTabContent->getModelControllerForDisplay();
                    if (model != NULL) {
                        OverlaySet* overlaySet = model->getOverlaySet(this->browserTabContent->getTabNumber());
                        
                        std::vector<LabelFile*> labelFiles;
                        std::vector<int32_t> labelMapIndices;
                        
                        bool useAllLabelFiles = true;
                        if (useAllLabelFiles) {
                            std::vector<LabelFile*> brainStructureLabelFiles;
                            surface->getBrainStructure()->getLabelFiles(brainStructureLabelFiles);
                            const int numBrainStructureLabelFiles = static_cast<int32_t>(brainStructureLabelFiles.size());
                            for (int32_t i = 0; i < numBrainStructureLabelFiles; i++) {
                                LabelFile* lf = brainStructureLabelFiles[i];
                                const int32_t numMaps = lf->getNumberOfMaps();
                                for (int im = 0; im < numMaps; im++) {
                                    labelFiles.push_back(lf);
                                    labelMapIndices.push_back(im);
                                }
                            }
                        }
                        else {
                            overlaySet->getLabelFilesForSurface(surface, 
                                                                labelFiles, 
                                                                labelMapIndices); 
                        }
                        const int32_t numberOfLabelFiles = static_cast<int32_t>(labelFiles.size());
                        for (int32_t ilf = 0; ilf < numberOfLabelFiles; ilf++) {
                            LabelFile* labelFile = dynamic_cast<LabelFile*>(labelFiles[ilf]);
                            const int32_t mapIndex = labelMapIndices[ilf];
                            const int labelKey = labelFile->getLabelKey(nodeNumber, 
                                                                        mapIndex);
                            const AString mapName = labelFile->getMapName(mapIndex);
                            const GiftiLabel* giftiLabel = labelFile->getLabelTable()->getLabel(labelKey);
                            if (giftiLabel != NULL) {
                                ParcelConnectivity* pc = new ParcelConnectivity(labelFile,
                                                                                mapIndex,
                                                                                labelKey,
                                                                                giftiLabel->getName(),
                                                                                surface,
                                                                                nodeNumber,
                                                                                clm);
                                this->parcelConntivities.push_back(pc);
                                
                                const AString actionName("Show Connectivity For Parcel "
                                                         + giftiLabel->getName()
                                                         + " in map "
                                                         + mapName);
                                QAction* action = connectivityActionGroup->addAction(actionName);
                                action->setData(qVariantFromValue((void*)pc));
                                
                                connectivityActions.push_back(action);
                            }
                        }
                    }
                }
            }
        }
        if (connectivityActions.empty() == false) {
            this->addSeparator();            
            for (std::vector<QAction*>::iterator connIter = connectivityActions.begin();
                 connIter != connectivityActions.end();
                 connIter++) {
                this->addAction(*connIter);
            }
        }
    }
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::~BrainOpenGLWidgetContextMenu()
{
    for (std::vector<ParcelConnectivity*>::iterator parcelIter = this->parcelConntivities.begin();
         parcelIter != this->parcelConntivities.end();
         parcelIter++) {
        ParcelConnectivity* pc = *parcelIter;
        delete pc;
    }
}

/**
 * Called when a connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void 
BrainOpenGLWidgetContextMenu::parcelConnectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    pc->labelFile->getNodeIndicesWithLabelKey(pc->labelFileMapIndex, 
                                              pc->labelKey,
                                              nodeIndices);
    if (nodeIndices.empty()) {
        WuQMessageBox::errorOk(this,
                               "No nodes match label " + pc->labelName);
        return;
    }
    
    try {
        CursorDisplay cursor(Qt::WaitCursor);
        pc->connectivityLoaderManager->loadAverageDataForSurfaceNodes(pc->surface,
                                                                      nodeIndices);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());    
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceBorderSelected()
{
    IdentificationItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Brain* brain = borderID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherIdentifiedItems(borderID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    EventManager::get()->sendEvent(EventInformationTextDisplay(idMessage,
                                                               EventInformationTextDisplay::TYPE_HTML).getPointer());
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceNodeSelected()
{    
    IdentificationItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    Brain* brain = surfaceID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherIdentifiedItems(surfaceID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    EventManager::get()->sendEvent(EventInformationTextDisplay(idMessage,
                                                               EventInformationTextDisplay::TYPE_HTML).getPointer());
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifyVoxelSelected()
{
    IdentificationItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    Brain* brain = voxelID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherIdentifiedItems(voxelID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    EventManager::get()->sendEvent(EventInformationTextDisplay(idMessage,
                                                               EventInformationTextDisplay::TYPE_HTML).getPointer());
}

/**
 * Called to remove node identification symbols.
 */
void 
BrainOpenGLWidgetContextMenu::removeNodeIdentificationSymbolsSelected()
{    
    EventManager::get()->sendEvent(EventIdentificationSymbolRemoval().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

