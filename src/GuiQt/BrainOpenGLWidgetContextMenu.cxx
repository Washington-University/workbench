
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

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "ConnectivityLoaderManager.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateTimeCourseDialog.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GuiManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "LabelFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Model.h"
#include "ProgressReportingDialog.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "UserInputModeFociWidget.h"
#include "VolumeFile.h"
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
BrainOpenGLWidgetContextMenu::BrainOpenGLWidgetContextMenu(SelectionManager* identificationManager,
                                                           BrowserTabContent* browserTabContent,
                                                           QWidget* parent)
: QMenu(parent)
{
    this->parentWidget = parent;
    this->identificationManager = identificationManager;
    this->browserTabContent = browserTabContent;
    
    /*
     * Accumlate identification actions
     */
    std::vector<QAction*> identificationActions;
    
    /*
     * Identify Border
     */
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    if (borderID->isValid()) {
        const QString text = ("Identify Border ("
                              + borderID->getBorder()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text, 
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceBorderSelected())));
    }

    /*
     * Identify Surface Focus
     */
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    if (focusID->isValid()) {
        const QString text = ("Identify Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifySurfaceFocusSelected())));
    }
    
    /*
     * Identify Node
     */
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    if (surfaceID->isValid()) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Identify Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ") Under Mouse");

        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceNodeSelected())));
    }
    
    /*
     * Identify Voxel
     */
    SelectionItemVoxel* idVoxel = this->identificationManager->getVoxelIdentification();
    if (idVoxel->isValid()) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        const AString text = ("Identify Voxel ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ")");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifyVoxelSelected())));
    }
    
    /*
     * Identify Volume Focus
     */
    SelectionItemFocusVolume* focusVolID = this->identificationManager->getVolumeFocusIdentification();
    if (focusVolID->isValid()) {
        const QString text = ("Identify Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifyVolumeFocusSelected())));
    }
    
    if (identificationActions.empty() == false) {
        this->addSeparator();
        
        for (std::vector<QAction*>::iterator idIter = identificationActions.begin();
             idIter != identificationActions.end();
             idIter++) {
            this->addAction(*idIter);
        }
    }
    
    
    std::vector<QAction*> borderConnectivityActions;
    
    /*static bool run = false;
    if(!run)
    {
        run = true;
        const AString actionName("Show Time series For Parcel ");
        QAction* action = connectivityActionGroup->addAction(actionName);
        timeSeriesActions.push_back(action);

    }*/

    if (borderID->isValid()) {
        Brain* brain = borderID->getBrain();
        std::vector<CiftiConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
        
        /*
         * Connectivity actions for borders
         */
        if (hasCiftiConnectivity) {
            const QString text = ("Show CIFTI Connectivity for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderCiftiConnectivitySelected()));
            borderConnectivityActions.push_back(action);
        }
        
        bool hasDenseConnectivity = brain->getNumberOfConnectivityDenseFiles() > 0 ? true : false;
        bool hasTimeSeries = brain->getNumberOfConnectivityTimeSeriesFiles() > 0 ? true : false;
        ConnectivityLoaderManager* clm = NULL;
        if (hasDenseConnectivity || hasTimeSeries) {
            clm = brain->getConnectivityLoaderManager();
        }        
        if (clm != NULL) {
            if(hasDenseConnectivity)
            {
                const QString text = ("Show Connectivity for Nodes Inside Border "
                                      + borderID->getBorder()->getName());
                QAction* action = WuQtUtilities::createAction(text,
                                                              "",
                                                              this,
                                                              this,
                                                              SLOT(borderConnectivitySelected()));
                borderConnectivityActions.push_back(action);
            }
            if(hasTimeSeries)
            {
                const QString text = ("Show Time Series for Nodes Inside Border "
                                      + borderID->getBorder()->getName());
                QAction* action = WuQtUtilities::createAction(text,
                                                              "",
                                                              this,
                                                              this,
                                                              SLOT(borderTimeSeriesSelected()));
                borderConnectivityActions.push_back(action);
            }   
        }
    }
    
    std::vector<QAction*> ciftiConnectivityActions;
    QActionGroup* ciftiConnectivityActionGroup = new QActionGroup(this);
    QObject::connect(ciftiConnectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiConnectivityActionSelected(QAction*)));
    
    std::vector<QAction*> connectivityActions;
    QActionGroup* connectivityActionGroup = new QActionGroup(this);
    QObject::connect(connectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelConnectivityActionSelected(QAction*)));
    
    std::vector<QAction*> timeSeriesActions;
    QActionGroup* timeSeriesActionGroup = new QActionGroup(this);
    QObject::connect(timeSeriesActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelTimeSeriesActionSelected(QAction*)));
    if (surfaceID->isValid()) {
        /*
         * Connectivity actions for labels
         */
        Brain* brain = surfaceID->getBrain();
        Surface* surface = surfaceID->getSurface();
        const int32_t nodeNumber = surfaceID->getNodeNumber();
        
        CiftiConnectivityMatrixDataFileManager* connMatrixMan = brain->getCiftiConnectivityMatrixDataFileManager();
        std::vector<CiftiConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
        
        ConnectivityLoaderManager* clm = NULL;
        bool hasDenseConnectivity = brain->getNumberOfConnectivityDenseFiles() > 0 ? true : false;
        bool hasTimeSeries = brain->getNumberOfConnectivityTimeSeriesFiles() > 0 ? true : false;
        if (hasDenseConnectivity || hasTimeSeries) {
            clm = brain->getConnectivityLoaderManager();
        }    
    
        Model* model = this->browserTabContent->getModelControllerForDisplay();
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
                                                                    clm,
                                                                    connMatrixMan);
                    this->parcelConnectivities.push_back(pc);
                    
                    if (hasCiftiConnectivity) {
                        const AString actionName("Show Cifti Connectivity For Parcel "
                                                 + giftiLabel->getName()
                                                 + " in map "
                                                 + mapName);
                        QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
                        action->setData(qVariantFromValue((void*)pc));
                        ciftiConnectivityActions.push_back(action);
                    }
                    
                    if (clm != NULL) {
                        if (hasDenseConnectivity) {
                            const AString actionName("Show Connectivity For Parcel "
                                                     + giftiLabel->getName()
                                                     + " in map "
                                                     + mapName);
                            QAction* action = connectivityActionGroup->addAction(actionName);
                            action->setData(qVariantFromValue((void*)pc));
                            connectivityActions.push_back(action);
                        }

                        if (hasTimeSeries) {
                            const AString tsActionName("Show Time Series For Parcel "
                                                       + giftiLabel->getName()
                                                       + " in map "
                                                       + mapName);
                            QAction* tsAction = timeSeriesActionGroup->addAction(tsActionName);
                            tsAction->setData(qVariantFromValue((void*)pc));                            
                            timeSeriesActions.push_back(tsAction);
                        }
                    }
                }
            }
        }
    }
    
    if (borderConnectivityActions.empty() == false) {
        this->addSeparator();
        for (std::vector<QAction*>::iterator borderIter = borderConnectivityActions.begin();
             borderIter != borderConnectivityActions.end();
             borderIter++) {
            this->addAction(*borderIter);
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
    
    if(timeSeriesActions.empty() == false) {
        this->addSeparator();            
        for (std::vector<QAction*>::iterator tsIter = timeSeriesActions.begin();
             tsIter != timeSeriesActions.end();
             tsIter++) {
            this->addAction(*tsIter);
        }
    }
    
    std::vector<QAction*> createActions;
    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();

    /*
     * Create focus at surface node or at ID symbol
     */
    if (surfaceID->isValid()
        && (focusID->isValid() == false)) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Create Focus at Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceFocusSelected())));
    }
    else if (idSymbol->isValid()
             && (focusID->isValid() == false)) {
        const int32_t nodeIndex = idSymbol->getNodeNumber();
        const Surface* surface = idSymbol->getSurface();
        const QString text = ("Create Focus at Selected Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceIDSymbolFocusSelected())));
    }

    /*
     * Create focus at voxel as long as there is no volume focus ID
     */
    if (idVoxel->isValid()
        && (focusVolID->isValid() == false)) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        float xyz[3];
        const VolumeFile* vf = idVoxel->getVolumeFile();
        vf->indexToSpace(ijk, xyz);
        
        const AString text = ("Create Focus at Voxel IJK ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ") XYZ ("
                              + AString::fromNumbers(xyz, 3, ",")
                              + ")...");
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createVolumeFocusSelected())));
    }
    
    if (createActions.empty() == false) {
        if (this->actions().count() > 0) {
            this->addSeparator();
        }
        for (std::vector<QAction*>::iterator iter = createActions.begin();
             iter != createActions.end();
             iter++) {
            this->addAction(*iter);
        }
    }
    
    /*
     * Actions for editing
     */
    std::vector<QAction*> editActions;
    
    /*
     * Edit Surface Focus
     */
    if (focusID->isValid()) {
        const QString text = ("Edit Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ")");
        editActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editSurfaceFocusSelected())));
    }
    
    /*
     * Edit volume focus
     */
    if (focusVolID->isValid()) {
        const QString text = ("Edit Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ")");
        editActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editVolumeFocusSelected())));
    }
    
    if (editActions.empty() == false) {
        if (this->actions().count() > 0) {
            this->addSeparator();
        }
        for (std::vector<QAction*>::iterator iter = editActions.begin();
             iter != editActions.end();
             iter++) {
            this->addAction(*iter);
        }
    }
    
    if (this->actions().count() > 0) {
        this->addSeparator();
    }
    this->addAction("Remove All Vertex Identification Symbols",
                    this,
                    SLOT(removeAllNodeIdentificationSymbolsSelected()));
    
    if (idSymbol->isValid()) {
        const AString text = ("Remove Identification of Vertices "
                              + AString::number(idSymbol->getNodeNumber()));
        
        this->addAction(WuQtUtilities::createAction(text,
                                                    "",
                                                    this,
                                                    this,
                                                    SLOT(removeNodeIdentificationSymbolSelected())));
    }
}


/**
 * @param brain
 *   Brain containing connectivity loader manager.
 * @return connectivity loader manager if there are dense files or 
 * NULL if no dense files.
 */
ConnectivityLoaderManager*
BrainOpenGLWidgetContextMenu::getConnectivityLoaderManager(Brain* brain)
{
    if (brain->getNumberOfConnectivityDenseFiles()) {
        ConnectivityLoaderManager* clm = brain->getConnectivityLoaderManager();
        return clm;
    }
    
    return NULL;
}


/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::~BrainOpenGLWidgetContextMenu()
{
    for (std::vector<ParcelConnectivity*>::iterator parcelIter = this->parcelConnectivities.begin();
         parcelIter != this->parcelConnectivities.end();
         parcelIter++) {
        ParcelConnectivity* pc = *parcelIter;
        delete pc;
    }
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
BrainOpenGLWidgetContextMenu::parcelCiftiConnectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    pc->labelFile->getNodeIndicesWithLabelKey(pc->labelFileMapIndex,
                                              pc->labelKey,
                                              nodeIndices);
    if (nodeIndices.empty()) {
        WuQMessageBox::errorOk(this,
                               "No vertices match label " + pc->labelName);
        return;
    }
    
    if (this->warnIfNetworkNodeCountIsLarge(pc->connectivityLoaderManager,
                                            nodeIndices) == false) {
        return;
    }
    
    try {
        ProgressReportingDialog progressDialog("Connectivity Withing Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        pc->ciftiConnectivityManager->loadAverageDataForSurfaceNodes(pc->surface,
                                                                      nodeIndices);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
                               "No vertices match label " + pc->labelName);
        return;
    }
    
    if (this->warnIfNetworkNodeCountIsLarge(pc->connectivityLoaderManager, 
                                            nodeIndices) == false) {
        return;
    }
    
    try {
        ProgressReportingDialog progressDialog("Connectivity Withing Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
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
 * Called when border cifti connectivity is selected.
 */
void
BrainOpenGLWidgetContextMenu::borderCiftiConnectivitySelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getConnectivityLoaderManager(),
                                                nodeIndices) == false) {
            return;
        }
        
        try {
            ProgressReportingDialog progressDialog("Connectivity Withing Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
            CiftiConnectivityMatrixDataFileManager* ciftiConnMann = borderID->getBrain()->getCiftiConnectivityMatrixDataFileManager();
            ciftiConnMann->loadAverageDataForSurfaceNodes(surface,
                                                          nodeIndices);
        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }
        
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
}

/**
 * Called when border connectivity is selected.
 */
void 
BrainOpenGLWidgetContextMenu::borderConnectivitySelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getConnectivityLoaderManager(), 
                                                nodeIndices) == false) {
            return;
        }
        
        try {
            ProgressReportingDialog progressDialog("Connectivity Withing Border",
                                                    "",
                                                    this);
            progressDialog.setValue(0);
            ConnectivityLoaderManager* connectivityLoaderManager = borderID->getBrain()->getConnectivityLoaderManager();
            connectivityLoaderManager->loadAverageDataForSurfaceNodes(surface,
                                                                          nodeIndices);
        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }
        
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());    
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
}

/**
 * Called when a connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void 
BrainOpenGLWidgetContextMenu::parcelTimeSeriesActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    pc->labelFile->getNodeIndicesWithLabelKey(pc->labelFileMapIndex, 
                                              pc->labelKey,
                                              nodeIndices);
    if (nodeIndices.empty()) {
        WuQMessageBox::errorOk(this,
                               "No vertices match label " + pc->labelName);
        return;
    }
    
    if (this->warnIfNetworkNodeCountIsLarge(pc->connectivityLoaderManager, 
                                            nodeIndices) == false) {
        return;
    }
    
    try {
        ProgressReportingDialog progressDialog("Data Series Withing Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        TimeLine tl;
        for(int i=0;i<3;i++) tl.point[i] = 0.0;
        tl.parcelName = pc->labelName;       
        tl.structureName = StructureEnum::toGuiName(pc->surface->getStructure());
        tl.label = tl.structureName + ":" + tl.parcelName;
        pc->connectivityLoaderManager->loadAverageTimeSeriesForSurfaceNodes(pc->surface,
                                                                      nodeIndices, tl);

        QList <TimeLine> tlV;
        pc->connectivityLoaderManager->getSurfaceTimeLines(tlV);
        if(tlV.size()!=0)
        {                    
                GuiManager::get()->addTimeLines(tlV);                    
        }
        EventUpdateTimeCourseDialog e;
        EventManager::get()->sendEvent(e.getPointer());
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }   
  
}

/**
 * Called when border timeseries is selected.
 */
void 
BrainOpenGLWidgetContextMenu::borderTimeSeriesSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getConnectivityLoaderManager(), 
                                                nodeIndices) == false) {
            return;
        }
        
        try {
            ProgressReportingDialog progressDialog("Data Series Withing Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
            TimeLine tl;
            for(int i=0;i<3;i++) tl.point[i] = 0.0;
            tl.borderClassName = border->getClassName();
            tl.borderName = border->getName();
            tl.structureName = StructureEnum::toGuiName(border->getStructure());
            tl.label =  tl.structureName + ":" + tl.borderClassName + ":" + tl.borderName;
            ConnectivityLoaderManager* connectivityLoaderManager = borderID->getBrain()->getConnectivityLoaderManager();
            connectivityLoaderManager->loadAverageTimeSeriesForSurfaceNodes(surface,
                                                                          nodeIndices,tl);
            QList <TimeLine> tlV;
            connectivityLoaderManager->getSurfaceTimeLines(tlV);
            if(tlV.size()!=0)
            {                    
                 GuiManager::get()->addTimeLines(tlV);                    
            }
            EventUpdateTimeCourseDialog e;
            EventManager::get()->sendEvent(e.getPointer());

        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }   
        
  
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }

}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceBorderSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Brain* brain = borderID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherSelectedItems(borderID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceFocusSelected()
{
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    const Surface* surface = surfaceID->getSurface();
    const int32_t nodeIndex = surfaceID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceIDSymbolFocusSelected()
{
    SelectionItemSurfaceNodeIdentificationSymbol* nodeSymbolID =
        this->identificationManager->getSurfaceNodeIdentificationSymbol();
    
    const Surface* surface = nodeSymbolID->getSurface();
    const int32_t nodeIndex = nodeSymbolID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}
/**
 * Called to create a focus at a voxel location
 */
void
BrainOpenGLWidgetContextMenu::createVolumeFocusSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    const VolumeFile* vf = voxelID->getVolumeFile();
    int64_t ijk[3];
    voxelID->getVoxelIJK(ijk);
    float xyz[3];
    vf->indexToSpace(ijk, xyz);
    
    const AString focusName = (vf->getFileNameNoPath()
                               + " IJK ("
                               + AString::fromNumbers(ijk, 3, ",")
                               + ")");
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to display identication information on the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::identifySurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Brain* brain = focusID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::identifyVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Brain* brain = focusID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to edit the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::editSurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to edit the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::editVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceNodeSelected()
{    
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    Brain* brain = surfaceID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherSelectedItems(surfaceID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    Surface* surface = surfaceID->getSurface();
    const StructureEnum::Enum structure = surface->getStructure();
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItemNode(idMessage,
                                                        structure,
                                                        surface->getNumberOfNodes(),
                                                        surfaceID->getNodeNumber()));
    
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifyVoxelSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    Brain* brain = voxelID->getBrain();
    const BrowserTabContent* btc = NULL;
    this->identificationManager->clearOtherSelectedItems(voxelID);
    const AString idMessage = this->identificationManager->getIdentificationText(btc,
                                                                                 brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to remove all node identification symbols.
 */
void 
BrainOpenGLWidgetContextMenu::removeAllNodeIdentificationSymbolsSelected()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    idManager->removeAllIdentifiedItems();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to remove node identification symbol from node.
 */
void
BrainOpenGLWidgetContextMenu::removeNodeIdentificationSymbolSelected()
{
   SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();
    if (idSymbol->isValid()) {
        Surface* surface = idSymbol->getSurface();
        
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        idManager->removeIdentifiedNodeItem(surface->getStructure(),
                                            surface->getNumberOfNodes(),
                                            idSymbol->getNodeNumber());

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


/**
 * If any enabled connectivity files retrieve data from the network
 * and the number of nodes is large, warn the user since this will
 * be a very slow operation.
 *
 * @param clm
 *    The connectivity manager.
 * @param nodeIndices
 *    Indices of nodes that will have connectivity data retrieved.
 * @return 
 *    true if process should continue, false if user cancels.
 */
bool 
BrainOpenGLWidgetContextMenu::warnIfNetworkNodeCountIsLarge(const ConnectivityLoaderManager* clm,
                                                            const std::vector<int32_t>& nodeIndices)
{
    const int32_t numNodes = static_cast<int32_t>(nodeIndices.size());
    if (numNodes < 200) {
        return true;
    }
    
    if (clm->hasNetworkFiles() == false) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numNodes)
                         + " vertices in the selected region.  Loading data for "
                         + "this quantity of vertices may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}


