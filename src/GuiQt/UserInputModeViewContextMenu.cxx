
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

#include <QActionGroup>

#define __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__
#include "UserInputModeViewContextMenu.h"
#undef __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "ChartingDataManager.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFiberTrajectoryManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
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
#include "SessionManager.h"
#include "Surface.h"
#include "UserInputModeFociWidget.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeViewContextMenu 
 * \brief Context (pop-up) menu for User Input View Mode
 *
 * Displays a menu in the BrainOpenGLWidget.  Content of menu
 * is dependent upon data under the cursor.
 */
/**
 * Constructor.
 * @param selectionManager
 *    The selection manager, provides data under the cursor.
 * @param browserTabContent
 *    Content of browser tab.
 * @param parentOpenGLWidget
 *    Parent OpenGL Widget on which the menu is displayed.
 */
UserInputModeViewContextMenu::UserInputModeViewContextMenu(SelectionManager* selectionManager,
                                                           BrowserTabContent* browserTabContent,
                                                           BrainOpenGLWidget* parentOpenGLWidget)
: QMenu(parentOpenGLWidget)
{
    this->parentOpenGLWidget = parentOpenGLWidget;
    this->selectionManager = selectionManager;
    this->browserTabContent = browserTabContent;
    
    /*
     * Add the identification actions.
     */
    addIdentificationActions();
    
    /*
     * Add the border options.
     */
    addBorderRegionOfInterestActions();
    
    /*
     * Add the foci actions.
     */
    addFociActions();
    
    /*
     * Show Label ROI operations only for surfaces
     */
        addLabelRegionOfInterestActions();
    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = selectionManager->getSurfaceNodeIdentificationSymbol();
    
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
 * Destructor.
 */
UserInputModeViewContextMenu::~UserInputModeViewContextMenu()
{
    for (std::vector<ParcelConnectivity*>::iterator parcelIter = this->parcelConnectivities.begin();
         parcelIter != this->parcelConnectivities.end();
         parcelIter++) {
        delete *parcelIter;
    }
}

/**
 * Add the actions to this context menu.
 *
 * @param actionsToAdd
 *     Actions to add to the menum.
 * @param addSeparatorBeforeActions
 *     If true and there are actions presently in the menu, a separator
 *     (horizontal bar) is added prior to adding the given actions.
 */
void
UserInputModeViewContextMenu::addActionsToMenu(QList<QAction*>& actionsToAdd,
                                               const bool addSeparatorBeforeActions)
{
    if (actionsToAdd.empty() == false) {
        if (addSeparatorBeforeActions) {
            if (actions().isEmpty() == false) {
                addSeparator();
            }
        }

        addActions(actionsToAdd);
    }
}


/**
 * Add the identification actions to the menu.
 */
void
UserInputModeViewContextMenu::addIdentificationActions()
{
    /*
     * Accumlate identification actions
     */
    QList<QAction*> identificationActions;
    
    /*
     * Identify Border
     */
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
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
    SelectionItemFocusSurface* focusID = this->selectionManager->getSurfaceFocusIdentification();
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
    SelectionItemSurfaceNode* surfaceID = this->selectionManager->getSurfaceNodeIdentification();
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
    SelectionItemVoxel* idVoxel = this->selectionManager->getVoxelIdentification();
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
    SelectionItemFocusVolume* focusVolID = this->selectionManager->getVolumeFocusIdentification();
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
    
    addActionsToMenu(identificationActions,
                     true);
}

/**
 * Add the border region of interest actions to the menu.
 */
void
UserInputModeViewContextMenu::addBorderRegionOfInterestActions()
{
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
    
    QList<QAction*> borderActions;
    
    if (borderID->isValid()) {
        Brain* brain = borderID->getBrain();
        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
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
            borderActions.push_back(action);
        }
        
        std::vector<ChartableLineSeriesBrainordinateInterface*> chartableFiles;
        brain->getAllChartableBrainordinateDataFiles(chartableFiles);
        
        if (chartableFiles.empty() == false) {
            const QString text = ("Show Charts for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderDataSeriesSelected()));
            borderActions.push_back(action);
        }
    }

    addActionsToMenu(borderActions,
                     true);
}

/**
 * Add all label region of interest options to the menu
 */
void
UserInputModeViewContextMenu::addLabelRegionOfInterestActions()
{
    Brain* brain = NULL;
    
    float voxelXYZ[3] = { 0.0, 0.0, 0.0 };
    SelectionItemVoxel* idVoxel = this->selectionManager->getVoxelIdentification();
    if (idVoxel->isValid()) {
        double voxelXYZDouble[3];
        idVoxel->getModelXYZ(voxelXYZDouble);
        
        voxelXYZ[0] = voxelXYZDouble[0];
        voxelXYZ[1] = voxelXYZDouble[1];
        voxelXYZ[2] = voxelXYZDouble[2];
        brain = idVoxel->getBrain();
    }
    
    Surface* surface = NULL;
    int32_t surfaceNodeIndex = -1;
    int32_t surfaceNumberOfNodes = -1;
    StructureEnum::Enum surfaceStructure = StructureEnum::INVALID;
    SelectionItemSurfaceNode* idNode = this->selectionManager->getSurfaceNodeIdentification();
    if (idNode->isValid()) {
        surface = idNode->getSurface();
        surfaceNodeIndex = idNode->getNodeNumber();
        surfaceNumberOfNodes = surface->getNumberOfNodes();
        surfaceStructure = surface->getStructure();
        brain = idNode->getBrain();
    }
    
    /*
     * If Brain is invalid, then there is no identified node or voxel
     */
    if (brain == NULL) {
        return;
    }
    
    /*
     * Manager for connectivity matrix files
     */
    CiftiConnectivityMatrixDataFileManager* ciftiConnectivityMatrixManager = SessionManager::get()->getCiftiConnectivityMatrixDataFileManager();
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
    
    /*
     * Manager for fiber trajectory
     */
    CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager = SessionManager::get()->getCiftiFiberTrajectoryManager();
    std::vector<CiftiFiberTrajectoryFile*> ciftiFiberTrajectoryFiles;
    const int32_t numFiberFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t i = 0; i < numFiberFiles; i++) {
        ciftiFiberTrajectoryFiles.push_back(brain->getConnectivityFiberTrajectoryFile(i));
    }
    const bool haveCiftiFiberTrajectoryFiles = (ciftiFiberTrajectoryFiles.empty() == false);
    
    /*
     * Manager for Chartable files
     */
    std::vector<ChartableLineSeriesBrainordinateInterface*> chartableFiles;
    brain->getAllChartableBrainordinateDataFiles(chartableFiles);
    const bool haveChartableFiles = (chartableFiles.empty() == false);
    ChartingDataManager* chartingDataManager = brain->getChartingDataManager();

    /*
     * Actions for each file type
     */
    QList<QAction*> ciftiConnectivityActions;
    QActionGroup* ciftiConnectivityActionGroup = new QActionGroup(this);
    QObject::connect(ciftiConnectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiConnectivityActionSelected(QAction*)));
    
    QList<QAction*> ciftiFiberTrajectoryActions;
    QActionGroup* ciftiFiberTrajectoryActionGroup = new QActionGroup(this);
    QObject::connect(ciftiFiberTrajectoryActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiFiberTrajectoryActionSelected(QAction*)));
    
    QList<QAction*> chartableDataActions;
    QActionGroup* chartableDataActionGroup = new QActionGroup(this);
    QObject::connect(chartableDataActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelChartableDataActionSelected(QAction*)));

    /*
     * Get all mappable files and find files mapped with using labels
     */
    std::vector<CaretMappableDataFile*> mappableFiles;
    brain->getAllMappableDataFiles(mappableFiles);

    /*
     * Process each map file
     */
    for (std::vector<CaretMappableDataFile*>::iterator mapFileIterator = mappableFiles.begin();
         mapFileIterator != mappableFiles.end();
         mapFileIterator++) {
        CaretMappableDataFile* mappableLabelFile = *mapFileIterator;
        
        if (mappableLabelFile->isMappedWithLabelTable()) {
            const int32_t numMaps = mappableLabelFile->getNumberOfMaps();
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                
                Surface* labelSurface = NULL;
                int32_t labelNodeNumber = -1;
                int32_t labelKey = -1;
                AString labelName;
                int64_t volumeDimensions[3] = { -1, -1, -1 };
                ParcelConnectivity::ParcelType parcelType = ParcelConnectivity::PARCEL_TYPE_INVALID;
                
                if (mappableLabelFile->isVolumeMappable()) {
                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
                    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mappableLabelFile);
                    VolumeMappableInterface* volumeInterface = dynamic_cast<VolumeMappableInterface*>(mappableLabelFile);
                    if (volumeInterface != NULL) {
                        int64_t voxelIJK[3];
                        float voxelValue;
                        bool voxelValueValid;
                        AString textValue;
                        if (ciftiLabelFile != NULL) {
                            if (ciftiLabelFile->getMapVolumeVoxelValue(mapIndex,
                                                                       voxelXYZ,
                                                                       voxelIJK,
                                                                       voxelValue,
                                                                       voxelValueValid,
                                                                       textValue)) {
                                if (voxelValueValid) {
                                    labelKey = static_cast<int32_t>(voxelValue);
                                    const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
                                    labelName =  labelTable->getLabelName(labelKey);
                                    
                                    if (labelName.isEmpty() == false) {
                                        parcelType = ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS;
                                    }
                                }
                            }
                        }
                        else if (volumeLabelFile != NULL) {
                            int64_t voxelIJK[3];
                            volumeLabelFile->enclosingVoxel(voxelXYZ, voxelIJK);
                            if (volumeLabelFile->indexValid(voxelIJK)) {
                                const float voxelValue = volumeLabelFile->getValue(voxelIJK[0],
                                                                                   voxelIJK[1],
                                                                                   voxelIJK[2],
                                                                                   mapIndex);
                                const GiftiLabelTable* labelTable = volumeLabelFile->getMapLabelTable(mapIndex);
                                labelKey = static_cast<int32_t>(voxelValue);
                                labelName = labelTable->getLabelName(voxelValue);

                                if (labelName.isEmpty() == false) {
                                    parcelType = ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS;
                                }
                            }
                        }
                        else {
                            CaretAssertMessage(0,
                                               "Should never get here, new or invalid label file type");
                        }
                        
                        std::vector<int64_t> dims;
                        volumeInterface->getDimensions(dims);
                        if (dims.size() >= 3) {
                            volumeDimensions[0] = dims[0];
                            volumeDimensions[1] = dims[1];
                            volumeDimensions[2] = dims[2];
                        }
                    }
                }
                
                if (mappableLabelFile->isSurfaceMappable()) {
                    if (labelName.isEmpty()) {
                        if (idNode->isValid()) {
                            labelSurface = idNode->getSurface();
                            labelNodeNumber = idNode->getNodeNumber();
                            
                            LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
                            CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
                            
                            if (labelFile != NULL) {
                                labelKey = labelFile->getLabelKey(labelNodeNumber, mapIndex);
                                const GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
                                labelName = labelTable->getLabelName(labelKey);
                                
                                if (labelName.isEmpty() == false) {
                                    parcelType = ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES;
                                }
                            }
                            else if (ciftiLabelFile != NULL) {
                                float nodeValue = 0.0;
                                bool nodeValueValid = false;
                                AString stringValue;
                                if (ciftiLabelFile->getMapSurfaceNodeValue(mapIndex,
                                                                           surfaceStructure,
                                                                           surfaceNodeIndex,
                                                                           surfaceNumberOfNodes,
                                                                           nodeValue,
                                                                           nodeValueValid,
                                                                           stringValue)) {
                                    if (nodeValueValid) {
                                        labelKey = nodeValue;
                                        const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
                                        labelName = labelTable->getLabelName(labelKey);

                                        if (labelName.isEmpty() == false) {
                                            parcelType = ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (parcelType != ParcelConnectivity::PARCEL_TYPE_INVALID) {
                    const AString mapName = (AString::number(mapIndex + 1)
                                             + ": "
                                             + mappableLabelFile->getMapName(mapIndex));
                    
                    ParcelConnectivity* parcelConnectivity = new ParcelConnectivity(brain,
                                                                                    parcelType,
                                                                                    mappableLabelFile,
                                                                                  mapIndex,
                                                                                  labelKey,
                                                                                  labelName,
                                                                                  labelSurface,
                                                                                  labelNodeNumber,
                                                                                    volumeDimensions,
                                                                                  chartingDataManager,
                                                                                  ciftiConnectivityMatrixManager,
                                                                                  ciftiFiberTrajectoryManager);
                        this->parcelConnectivities.push_back(parcelConnectivity);

                    if (hasCiftiConnectivity) {
                        bool matchFlag = false;
                        if (parcelType == ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES) {
                            matchFlag = true;
                        }
                        else if (parcelType == ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS) {
                            for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
                                 iter != ciftiMatrixFiles.end();
                                 iter++) {
                                const CiftiMappableConnectivityMatrixDataFile* ciftiFile = *iter;
                                if (ciftiFile->matchesDimensions(volumeDimensions[0],
                                                                 volumeDimensions[1],
                                                                 volumeDimensions[2])) {
                                    matchFlag = true;
                                    break;
                                }
                            }
                        }
                        
                        if (matchFlag) {
                            const AString actionName("Show Cifti Connectivity For Parcel "
                                                     + labelName
                                                     + " in map "
                                                     + mapName);
                            QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
                            action->setData(qVariantFromValue((void*)parcelConnectivity));
                            ciftiConnectivityActions.push_back(action);
                        }
                    }
                    
                    if (haveCiftiFiberTrajectoryFiles) {
                        const AString fiberTrajActionName("Show Average Fiber Trajectory for Parcel "
                                                          + labelName
                                                          + " in map "
                                                          + mapName);
                        QAction* fiberTrajAction = ciftiFiberTrajectoryActionGroup->addAction(fiberTrajActionName);
                        fiberTrajAction->setData(qVariantFromValue((void*)parcelConnectivity));
                        ciftiFiberTrajectoryActions.push_back(fiberTrajAction);
                    }
                    
                    if (haveChartableFiles) {
                        bool matchFlag = false;
                        if (parcelType == ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES) {
                            matchFlag = true;
                        }
                        else if (parcelType == ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS) {
                            for (std::vector<ChartableLineSeriesBrainordinateInterface*>::iterator iter = chartableFiles.begin();
                                 iter != chartableFiles.end();
                                 iter++) {
                                const ChartableLineSeriesBrainordinateInterface* chartFile = *iter;
                                const CaretMappableDataFile* mapDataFile = chartFile->getLineSeriesChartCaretMappableDataFile();
                                if (mapDataFile != NULL){
                                    if (mapDataFile->isVolumeMappable()) {
                                        const VolumeMappableInterface* volMap = dynamic_cast<const VolumeMappableInterface*>(mapDataFile);
                                        if (volMap->matchesDimensions(volumeDimensions[0],
                                                                      volumeDimensions[1],
                                                                      volumeDimensions[2])) {
                                            matchFlag = true;
                                            break;
                                        }
                                        
                                    }
                                }
                            }
                        }
                        
                        if (matchFlag) {
                            const AString tsActionName("Show Data/Time Series Graph For Parcel "
                                                       + labelName
                                                       + " in map "
                                                       + mapName);
                            QAction* tsAction = chartableDataActionGroup->addAction(tsActionName);
                            tsAction->setData(qVariantFromValue((void*)parcelConnectivity));
                            chartableDataActions.push_back(tsAction);
                        }
                    }
                }
            }
        }
    }

    addActionsToMenu(ciftiConnectivityActions,
                     true);
    addActionsToMenu(ciftiFiberTrajectoryActions,
                     true);
    addActionsToMenu(chartableDataActions,
                     true);
}

/**
 * Add the foci options to the menu.
 */
void
UserInputModeViewContextMenu::addFociActions()
{
    QList<QAction*> fociCreateActions;
    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = selectionManager->getSurfaceNodeIdentificationSymbol();
    SelectionItemFocusSurface* focusID = this->selectionManager->getSurfaceFocusIdentification();
    SelectionItemSurfaceNode* surfaceID = this->selectionManager->getSurfaceNodeIdentification();
    SelectionItemVoxel* idVoxel = this->selectionManager->getVoxelIdentification();
    SelectionItemFocusVolume* focusVolID = this->selectionManager->getVolumeFocusIdentification();
    
    
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
        
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
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
        
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
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
        const VolumeMappableInterface* vf = idVoxel->getVolumeFile();
        vf->indexToSpace(ijk, xyz);
        
        const AString text = ("Create Focus at Voxel IJK ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ") XYZ ("
                              + AString::fromNumbers(xyz, 3, ",")
                              + ")...");
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createVolumeFocusSelected())));
    }
    
    addActionsToMenu(fociCreateActions,
                     true);
    
    /*
     * Actions for editing
     */
    QList<QAction*> fociEditActions;
    
    /*
     * Edit Surface Focus
     */
    if (focusID->isValid()) {
        const QString text = ("Edit Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ")");
        fociEditActions.push_back(WuQtUtilities::createAction(text,
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
        fociEditActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editVolumeFocusSelected())));
    }
    
    addActionsToMenu(fociEditActions,
                     true);
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
UserInputModeViewContextMenu::parcelCiftiConnectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    std::vector<VoxelIJK> voxelIndices;
    
    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            
            if (pc->ciftiConnectivityManager->hasNetworkFiles(pc->brain)) {
                if (warnIfNetworkBrainordinateCountIsLarge(nodeIndices.size()) == false) {
                    return;
                }
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            pc->getVoxelIndices(voxelIndices);
            if (voxelIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No voxels match label " + pc->labelName);
                return;
            }
            if (pc->ciftiConnectivityManager->hasNetworkFiles(pc->brain)) {
                if (warnIfNetworkBrainordinateCountIsLarge(voxelIndices.size()) == false) {
                    return;
                }
            }
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Connectivity Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);

        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiConnectivityManager->loadAverageDataForSurfaceNodes(pc->brain,
                                                                             pc->surface,
                                                                             nodeIndices);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                pc->ciftiConnectivityManager->loadAverageDataForVoxelIndices(pc->brain,
                                                                             pc->volumeDimensions,
                                                                             voxelIndices);
                break;
        }
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
UserInputModeViewContextMenu::parcelCiftiFiberTrajectoryActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    
    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Trajectory Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiFiberTrajectoryManager->loadDataAverageForSurfaceNodes(pc->brain,
                                                                                pc->surface,
                                                                                nodeIndices);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                std::vector<VoxelIJK> voxelIndices;
                pc->getVoxelIndices(voxelIndices);
                pc->ciftiFiberTrajectoryManager->loadAverageDataForVoxelIndices(pc->brain,
                                                                                pc->volumeDimensions,
                                                                                voxelIndices);
                break;
        }
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when border cifti connectivity is selected.
 */
void
UserInputModeViewContextMenu::borderCiftiConnectivitySelected()
{
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
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
        
        if (borderID->getBrain()->getChartingDataManager()->hasNetworkFiles()) {
            if (warnIfNetworkBrainordinateCountIsLarge(nodeIndices.size()) == false) {
                return;
            }
        }
        
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        try {
            ProgressReportingDialog progressDialog("Connectivity Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);

            CiftiConnectivityMatrixDataFileManager* ciftiConnMann = SessionManager::get()->getCiftiConnectivityMatrixDataFileManager();
            ciftiConnMann->loadAverageDataForSurfaceNodes(borderID->getBrain(),
                                                          surface,
                                                          nodeIndices);
        }
        catch (const DataFileException& e) {
            cursor.restoreCursor();
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
UserInputModeViewContextMenu::parcelChartableDataActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    std::vector<VoxelIJK> voxelIndices;

    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            
            if (pc->chartingDataManager->hasNetworkFiles()) {
                if (warnIfNetworkBrainordinateCountIsLarge(nodeIndices.size()) == false) {
                    return;
                }
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            pc->getVoxelIndices(voxelIndices);
            if (voxelIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No voxels match label " + pc->labelName);
                return;
            }
            if (pc->chartingDataManager->hasNetworkFiles()) {
                if (warnIfNetworkBrainordinateCountIsLarge(voxelIndices.size()) == false) {
                    return;
                }
            }
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Data Series Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->chartingDataManager->loadAverageChartForSurfaceNodes(pc->surface,
                                                                         nodeIndices);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                cursor.restoreCursor();
                WuQMessageBox::errorOk(this,
                                       "Charting of voxel average has not been implemented.");
                break;
        }
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }   

    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when border timeseries is selected.
 */
void 
UserInputModeViewContextMenu::borderDataSeriesSelected()
{
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
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
        
        if (borderID->getBrain()->getChartingDataManager()->hasNetworkFiles()) {
            if (warnIfNetworkBrainordinateCountIsLarge(nodeIndices.size()) == false) {
                return;
            }
        }
        
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        try {
            ProgressReportingDialog progressDialog("Data Series Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
            
            ChartingDataManager* chartingDataManager = borderID->getBrain()->getChartingDataManager();
            chartingDataManager->loadAverageChartForSurfaceNodes(surface,
                                                                 nodeIndices);
        }
        catch (const DataFileException& e) {
            cursor.restoreCursor();
            WuQMessageBox::errorOk(this, e.whatString());
        }   
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }

    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * @return If no data series graphs are enabled, enable all of them and return 
 * true.  Else, return false.
 */
bool
UserInputModeViewContextMenu::enableDataSeriesGraphsIfNoneEnabled()
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<ChartableLineSeriesBrainordinateInterface*> chartFiles;
    brain->getAllChartableBrainordinateDataFiles(chartFiles);
    if (chartFiles.empty()) {
        return false;
    }
    
    const int32_t tabIndex = this->browserTabContent->getTabNumber();
    
    /*
     * Exit if any data series graph is enabled.
     */
    for (std::vector<ChartableLineSeriesBrainordinateInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableLineSeriesBrainordinateInterface* chartFile = *iter;
        if (chartFile->isLineSeriesChartingEnabled(tabIndex)) {
            return false;
        }
    }
    
    /*
     * Enable and display all data series graphs.
     */
    for (std::vector<ChartableLineSeriesBrainordinateInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableLineSeriesBrainordinateInterface* chartFile = *iter;
        chartFile->setLineSeriesChartingEnabled(tabIndex,
                                      true);
    }

    return true;
}

/**
 * Called to display identication information on the surface border.
 */
void 
UserInputModeViewContextMenu::identifySurfaceBorderSelected()
{
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
    Brain* brain = borderID->getBrain();
    this->selectionManager->clearOtherSelectedItems(borderID);
    const AString idMessage = this->selectionManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to create a focus at a node location
 */
void
UserInputModeViewContextMenu::createSurfaceFocusSelected()
{
    SelectionItemSurfaceNode* surfaceID = this->selectionManager->getSurfaceNodeIdentification();
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
                                            this->parentOpenGLWidget);
}


/**
 * Called to create a focus at a node location
 */
void
UserInputModeViewContextMenu::createSurfaceIDSymbolFocusSelected()
{
    SelectionItemSurfaceNodeIdentificationSymbol* nodeSymbolID =
        this->selectionManager->getSurfaceNodeIdentificationSymbol();
    
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
                                            this->parentOpenGLWidget);
}
/**
 * Called to create a focus at a voxel location
 */
void
UserInputModeViewContextMenu::createVolumeFocusSelected()
{
    SelectionItemVoxel* voxelID = this->selectionManager->getVoxelIdentification();
    const VolumeMappableInterface* vf = voxelID->getVolumeFile();
    int64_t ijk[3];
    voxelID->getVoxelIJK(ijk);
    float xyz[3];
    vf->indexToSpace(ijk, xyz);
    
    const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(vf);
    const AString focusName = (cmdf->getFileNameNoPath()
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
                                            this->parentOpenGLWidget);
}


/**
 * Called to display identication information on the surface focus.
 */
void
UserInputModeViewContextMenu::identifySurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->selectionManager->getSurfaceFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->selectionManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->selectionManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the volume focus.
 */
void
UserInputModeViewContextMenu::identifyVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->selectionManager->getVolumeFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->selectionManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->selectionManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to edit the surface focus.
 */
void
UserInputModeViewContextMenu::editSurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->selectionManager->getSurfaceFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentOpenGLWidget);
}

/**
 * Called to edit the volume focus.
 */
void
UserInputModeViewContextMenu::editVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->selectionManager->getVolumeFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentOpenGLWidget);
}

/**
 * Called to display identication information on the surface border.
 */
void 
UserInputModeViewContextMenu::identifySurfaceNodeSelected()
{    
    GuiManager::get()->processIdentification(this->browserTabContent->getTabNumber(),
                                             this->selectionManager,
                                             this->parentOpenGLWidget);    
}

/**
 * Called to display identication information on the surface border.
 */
void 
UserInputModeViewContextMenu::identifyVoxelSelected()
{
    SelectionItemVoxel* voxelID = this->selectionManager->getVoxelIdentification();
    Brain* brain = voxelID->getBrain();
    this->selectionManager->clearOtherSelectedItems(voxelID);
    const AString idMessage = this->selectionManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to remove all node identification symbols.
 */
void 
UserInputModeViewContextMenu::removeAllNodeIdentificationSymbolsSelected()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    idManager->removeAllIdentifiedItems();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to remove node identification symbol from node.
 */
void
UserInputModeViewContextMenu::removeNodeIdentificationSymbolSelected()
{
   SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = selectionManager->getSurfaceNodeIdentificationSymbol();
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
 * This is called when the data file is on the network to warn users if the
 * query is very large and may take a long time.
 *
 * @param numberOfBrainordinatesInROI
 *    Number of brainordinates in the ROI.
 */
bool
UserInputModeViewContextMenu::warnIfNetworkBrainordinateCountIsLarge(const int64_t numberOfBrainordinatesInROI)
{
    if (numberOfBrainordinatesInROI < 200) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numberOfBrainordinatesInROI)
                         + " brainordinates in the selected region.  Loading data from the network for "
                         + "this quantity of brainordinates may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}


/* ------------------------------------------------------------------------- */
/**
 * Constructor.
 */
UserInputModeViewContextMenu::ParcelConnectivity::ParcelConnectivity(Brain* brain,
                                                                     const ParcelType parcelType,
                                                                     CaretMappableDataFile* mappableLabelFile,
                                                                     const int32_t labelFileMapIndex,
                                                                     const int32_t labelKey,
                                                                     const QString& labelName,
                                                                     Surface* surface,
                                                                     const int32_t nodeNumber,
                                                                     const int64_t volumeDimensions[3],
                                                                     ChartingDataManager* chartingDataManager,
                                                                     CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager,
                                                                     CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager) {
    this->brain = brain;
    this->parcelType = parcelType;
    this->mappableLabelFile = mappableLabelFile;
    this->labelFileMapIndex = labelFileMapIndex;
    this->labelKey = labelKey;
    this->labelName = labelName;
    this->surface = surface;
    this->nodeNumber = nodeNumber;
    this->volumeDimensions[0] = volumeDimensions[0];
    this->volumeDimensions[1] = volumeDimensions[1];
    this->volumeDimensions[2] = volumeDimensions[2];
    this->chartingDataManager = chartingDataManager;
    this->ciftiConnectivityManager = ciftiConnectivityManager;
    this->ciftiFiberTrajectoryManager = ciftiFiberTrajectoryManager;
}

/**
 * Destructor.
 */
UserInputModeViewContextMenu::ParcelConnectivity::~ParcelConnectivity()
{
    
}

/**
 * Get the node indices comprising the parcel.
 *
 * @param nodeIndicesOut
 *    Contains node indices upon exit.
 */
void
UserInputModeViewContextMenu::ParcelConnectivity::getNodeIndices(std::vector<int32_t>& nodeIndicesOut) const
{
    nodeIndicesOut.clear();
    
    if (this->parcelType != PARCEL_TYPE_SURFACE_NODES) {
        return;
    }
    
    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
    LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
    if (ciftiLabelFile != NULL) {
        ciftiLabelFile->getNodeIndicesWithLabelKey(surface->getStructure(),
                                                   surface->getNumberOfNodes(),
                                                   labelFileMapIndex,
                                                   labelKey,
                                                   nodeIndicesOut);
    }
    else if (labelFile != NULL) {
        labelFile->getNodeIndicesWithLabelKey(labelFileMapIndex,
                                              labelKey,
                                              nodeIndicesOut);
    }
    else {
        CaretAssertMessage(0,
                           "Should never get here, new or invalid label file type for surface nodes");
    }
}

/**
 * Get the voxel indices comprising the parcel
 *
 * @param voxelIndicesOut
 *    Contains voxel indices upon exit.
 */
void
UserInputModeViewContextMenu::ParcelConnectivity::getVoxelIndices(std::vector<VoxelIJK>& voxelIndicesOut) const
{
    voxelIndicesOut.clear();

    if (this->parcelType != PARCEL_TYPE_VOLUME_VOXELS) {
        return;
    }

    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mappableLabelFile);
    if (ciftiLabelFile != NULL) {
        ciftiLabelFile->getVoxelIndicesWithLabelKey(labelFileMapIndex,
                                                    labelKey,
                                                    voxelIndicesOut);
    }
    else if (volumeLabelFile != NULL) {
        volumeLabelFile->getVoxelIndicesWithLabelKey(labelFileMapIndex,
                                                    labelKey,
                                                    voxelIndicesOut);
    }
    else {
        CaretAssertMessage(0,
                           "Should never get here, new or invalid label file type for volume voxels");
    }
}


