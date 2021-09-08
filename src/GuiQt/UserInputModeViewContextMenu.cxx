
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
#include <QLineEdit>

#define __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__
#include "UserInputModeViewContextMenu.h"
#undef __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "ChartingDataManager.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFiberTrajectoryManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventImageCapture.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentifiedItemUniversal.h"
#include "LabelFile.h"
#include "MapFileDataSelector.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "MetricDynamicConnectivityFile.h"
#include "Model.h"
#include "ModelChartTwo.h"
#include "ProgressReportingDialog.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemChartTwoLabel.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemUniversalIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "Surface.h"
#include "UserInputModeFociWidget.h"
#include "UserInputModeViewContextTileTabsSubMenu.h"
#include "VolumeDynamicConnectivityFile.h"
#include "VolumeFile.h"
#include "WuQDataEntryDialog.h"
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
 *
 * @param mouseEvent
 *    The mouse event
 * @param viewportContent
 *    Content of the viewport.
 * @param selectionManager
 *    The selection manager, provides data under the cursor.
 * @param parentOpenGLWidget
 *    Parent OpenGL Widget on which the menu is displayed.
 */
UserInputModeViewContextMenu::UserInputModeViewContextMenu(const MouseEvent& mouseEvent,
                                                           BrainOpenGLViewportContent* viewportContent,
                                                           SelectionManager* selectionManager,
                                                           BrainOpenGLWidget* parentOpenGLWidget)
: QMenu(parentOpenGLWidget)
{
    this->viewportContent = viewportContent;
    CaretAssert(this->viewportContent);
    this->parentOpenGLWidget = parentOpenGLWidget;
    this->selectionManager = selectionManager;
    this->browserTabContent = viewportContent->getBrowserTabContent();
    CaretAssert(this->browserTabContent);
    
    UserInputModeViewContextTileTabsSubMenu* tabMenu = new UserInputModeViewContextTileTabsSubMenu(mouseEvent,
                                                                                                   this->parentOpenGLWidget,
                                                                                                   this->viewportContent);
    if (tabMenu->isValid()) {
        addMenu(tabMenu);
    }
    else {
        delete tabMenu;
        tabMenu = NULL;
    }
    /*
     * Add the identification actions.
     */
    QMenu* identifyMenu = createIdentifyMenu();
    if (identifyMenu != NULL) {
        addMenu(identifyMenu);
    }
    
    /*
     * Add the border options.
     */
    QMenu* borderMenu = createBorderRegionOfInterestMenu();
    if (borderMenu != NULL) {
        addMenu(borderMenu);
    }
    
    /*
     * Add the chart actions
     */
    QMenu* chartMenu = createChartMenu();
    if (chartMenu != NULL) {
        addMenu(chartMenu);
    }
    
    /*
     * Add the foci actions.
     */
    QMenu* fociMenu = createFociMenu();
    if (fociMenu != NULL) {
        addMenu(fociMenu);
    }
    
    /*
     * Show Label ROI operations only for surfaces
     */
    QMenu* labelMenu = createLabelRegionOfInterestMenu();
    if (labelMenu != NULL) {
        addMenu(labelMenu);
    }
    
    if (SessionManager::get()->getCaretPreferences()->isDevelopMenuEnabled()) {
        if (this->actions().count() > 0) {
            this->addSeparator();
        }
            
        QAction* rgbaPixelAction = this->addAction("Show Pixel RGBA...");
        QObject::connect(rgbaPixelAction, &QAction::triggered,
                         this, &UserInputModeViewContextMenu::showFrameBufferPixelRgbaSelected);
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
 *     Actions to add to the menu.
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
 * Add a submenu to this menu.
 *
 * @param menu
 *     Menu that is added.
 * @param addSeparatorBeforeMenu
 *     If true and the menu is not empty, add a separator before
 *     adding the sub menu.
 */
void
UserInputModeViewContextMenu::addSubMenuToMenu(QMenu* menu,
                                               const bool addSeparatorBeforeMenu)
{
    CaretAssert(menu);
    
    if (addSeparatorBeforeMenu) {
        if (actions().isEmpty() == false) {
            addSeparator();
        }
        
        addMenu(menu);
    }
    
}

/**
 * @return The identification menu
 */
QMenu*
UserInputModeViewContextMenu::createIdentifyMenu()
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
    
    bool showRemoveVertexSymbolsFlag = false;
    if (this->browserTabContent != NULL) {
        switch (this->browserTabContent->getSelectedModelType()) {
            case ModelTypeEnum::MODEL_TYPE_CHART:
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                break;
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
                showRemoveVertexSymbolsFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                showRemoveVertexSymbolsFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                showRemoveVertexSymbolsFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                showRemoveVertexSymbolsFlag = true;
                break;
        }
    }
    if (showRemoveVertexSymbolsFlag) {
        identificationActions.push_back(WuQtUtilities::createAction("Remove All Identification Symbols",
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(removeAllIdentificationSymbolsSelected())));
    }
    
    SelectionItemUniversalIdentificationSymbol* idSymbol = selectionManager->getUniversalIdentificationSymbol();
    if (idSymbol->isValid()) {
        identificationActions.push_back(WuQtUtilities::createAction("Remove Identification Symbol",
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(removeIdentificationSymbolSelected())));
    }
    
    QMenu* menu(NULL);
    
    if ( ! identificationActions.isEmpty()) {
        menu = new QMenu("Identify");
        
        for (auto action : identificationActions) {
            menu->addAction(action);
        }
    }
    
    return menu;
}

/**
 * @return the border  menu.
 */
QMenu*
UserInputModeViewContextMenu::createBorderRegionOfInterestMenu()
{
    SelectionItemBorderSurface* borderID = this->selectionManager->getSurfaceBorderIdentification();
    
    QList<QAction*> borderActions;
    
    if (borderID->isValid()) {
        Brain* brain = borderID->getBrain();
        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasConnectivityFile = (ciftiMatrixFiles.empty() == false);
        
        std::vector<MetricDynamicConnectivityFile*> metricDynConnFiles;
        brain->getMetricDynamicConnectivityFiles(metricDynConnFiles);
        for (auto mdc : metricDynConnFiles) {
            if (mdc->isDataLoadingEnabled()) {
                hasConnectivityFile = true;
            }
        }
        
        /*
         * Connectivity actions for borders
         */
        if (hasConnectivityFile) {
            const QString text = ("Show Connectivity for Vertices Inside Border "
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
            const QString text = ("Show Charts for Vertices Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderDataSeriesSelected()));
            borderActions.push_back(action);
        }
    }

    QMenu* menu(NULL);
    
    if ( ! borderActions.isEmpty()) {
        menu = new QMenu("Borders");
        
        for (auto action : borderActions) {
            menu->addAction(action);
        }
    }
    addActionsToMenu(borderActions,
                     true);
    
    return menu;
}

/**
 * Create the parcel (label) information.  It finds 
 * ALL label files and uses the label from every map
 * in the file that is associated withe the selected
 * brainordinate.
 */
void
UserInputModeViewContextMenu::createParcelConnectivities()
{
    Brain* brain = NULL;
    
    /*
     * Note that in an 'ALL' view there may be both
     * an identified surface node and an identified voxel
     */
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
    
    if (brain == NULL) {
        return;
    }

    if (idNode->isValid()
        && idVoxel->isValid()) {
        /* std::cout << "Have both surface and volume ID" << std::endl; */
    }
    
    /*
     * If true, only labels in file in the selected tab
     * are available.
     */
    const bool limitToFilesInSelectedTabFlag(true);
    
    std::vector<CaretMappableDataFile*> mapFiles;
    if (limitToFilesInSelectedTabFlag) {
        CaretAssert(this->browserTabContent);
        std::vector<CaretDataFile*> dataFiles;
        this->browserTabContent->getFilesDisplayedInTab(dataFiles);
        
        for (auto df : dataFiles) {
            CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(df);
            if (cmdf != NULL) {
                mapFiles.push_back(cmdf);
            }
        }
    }
    else {
        brain->getAllMappableDataFiles(mapFiles);
    }
    
    
    for (auto mapFile : mapFiles) {
        if (mapFile->isMappedWithLabelTable()) {
            const int32_t numberOfMaps = mapFile->getNumberOfMaps();
            for (int32_t mapIndex = 0; mapIndex < numberOfMaps; mapIndex++) {
                
                Surface* labelSurface = NULL;
                int32_t labelNodeNumber = -1;
                int32_t labelKey = -1;
                AString labelName;
                int64_t volumeDimensions[3] = { -1, -1, -1 };
                ParcelType parcelType = ParcelType::PARCEL_TYPE_INVALID;
                
                AString mapName = AString::number(mapIndex + 1);
                if (mapFile->getMapName(mapIndex).isEmpty()) {
                    mapName.append(": <map name empty>");
                }
                else {
                    mapName.append(": "
                                   + mapFile->getMapName(mapIndex));
                }
                
                /*
                 * Is this a volume label file, if so, find the
                 * label for this 'mapIndex'
                 */
                if (mapFile->isVolumeMappable()) {
                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mapFile);
                    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mapFile);
                    VolumeMappableInterface* volumeInterface = dynamic_cast<VolumeMappableInterface*>(mapFile);
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
                                        parcelType = ParcelType::PARCEL_TYPE_VOLUME_VOXELS;
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
                                    parcelType = ParcelType::PARCEL_TYPE_VOLUME_VOXELS;
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
                        
                        /*
                         * Create the parcel connectivity
                         */
                        if (labelName == "???") {
                            parcelType = ParcelType::PARCEL_TYPE_INVALID;
                        }
                        if (parcelType != ParcelType::PARCEL_TYPE_INVALID) {
                            ParcelConnectivity* parcelConnectivity = new ParcelConnectivity(brain,
                                                                                            parcelType,
                                                                                            mapFile,
                                                                                            mapIndex,
                                                                                            mapName,
                                                                                            labelKey,
                                                                                            labelName,
                                                                                            labelSurface,
                                                                                            labelNodeNumber,
                                                                                            volumeDimensions,
                                                                                            brain->getChartingDataManager(),
                                                                                            SessionManager::get()->getCiftiConnectivityMatrixDataFileManager(),
                                                                                            SessionManager::get()->getCiftiFiberTrajectoryManager());
                            this->parcelConnectivities.push_back(parcelConnectivity);
                        }
                    }
                }
                
                /*
                 * Is this a surface mapped label file, if so,
                 * find the label for this 'mapIndex'
                 */
                if (mapFile->isSurfaceMappable()) {
                    if (labelName.isEmpty()) {
                        if (idNode->isValid()) {
                            labelSurface = idNode->getSurface();
                            labelNodeNumber = idNode->getNodeNumber();
                            
                            LabelFile* labelFile = dynamic_cast<LabelFile*>(mapFile);
                            CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mapFile);
                            
                            if (labelFile != NULL) {
                                labelKey = labelFile->getLabelKey(labelNodeNumber, mapIndex);
                                const GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
                                labelName = labelTable->getLabelName(labelKey);
                                
                                if (labelName.isEmpty() == false) {
                                    parcelType = ParcelType::PARCEL_TYPE_SURFACE_NODES;
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
                                            parcelType = ParcelType::PARCEL_TYPE_SURFACE_NODES;
                                        }
                                    }
                                }
                            }
                            
                            /*
                             * Create the parcel connectivity
                             */
                            if (labelName == "???") {
                                parcelType = ParcelType::PARCEL_TYPE_INVALID;
                            }
                            if (parcelType != ParcelType::PARCEL_TYPE_INVALID) {
                                ParcelConnectivity* parcelConnectivity = new ParcelConnectivity(brain,
                                                                                                parcelType,
                                                                                                mapFile,
                                                                                                mapIndex,
                                                                                                mapName,
                                                                                                labelKey,
                                                                                                labelName,
                                                                                                labelSurface,
                                                                                                labelNodeNumber,
                                                                                                volumeDimensions,
                                                                                                brain->getChartingDataManager(),
                                                                                                SessionManager::get()->getCiftiConnectivityMatrixDataFileManager(),
                                                                                                SessionManager::get()->getCiftiFiberTrajectoryManager());
                                this->parcelConnectivities.push_back(parcelConnectivity);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @return The label region of interest menu
 */
QMenu*
UserInputModeViewContextMenu::createLabelRegionOfInterestMenu()
{
    createParcelConnectivities();

    /*
     * File types of interest
     */
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    std::vector<CiftiFiberTrajectoryFile*> ciftiFiberTrajectoryFiles;
    std::vector<ChartableLineSeriesBrainordinateInterface*> chartableFiles;
    std::vector<MetricDynamicConnectivityFile*> metricDynConnFiles;
    std::vector<VolumeDynamicConnectivityFile*> volDynConnFiles;
    /*
     * Get all files in displayed overlays
     */
    EventCaretMappableDataFilesAndMapsInDisplayedOverlays allOverlayDisplayedFilesEvent;
    EventManager::get()->sendEvent(allOverlayDisplayedFilesEvent.getPointer());
    auto mapFilesAndIndices = allOverlayDisplayedFilesEvent.getFilesAndMaps();
    
    /*
     * Find matrix, fiber trajectory, and line-series files that are displayed
     */
    for (auto mapFileAndIndex : mapFilesAndIndices) {
        CaretMappableDataFile* mapFile = mapFileAndIndex.m_mapFile;
        CaretAssert(mapFile);
        
        CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapFile);
        if (matrixFile != NULL) {
            ciftiMatrixFiles.push_back(matrixFile);
        }
        
        CiftiFiberTrajectoryFile* fiberTrajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(mapFile);
        if (fiberTrajFile != NULL) {
            ciftiFiberTrajectoryFiles.push_back(fiberTrajFile);
        }
        
        ChartableLineSeriesBrainordinateInterface* lineSeriesFile = dynamic_cast<ChartableLineSeriesBrainordinateInterface*>(mapFile);
        if (lineSeriesFile != NULL) {
            chartableFiles.push_back(lineSeriesFile);
        }
        
        MetricDynamicConnectivityFile* metricDynConnFile = dynamic_cast<MetricDynamicConnectivityFile*>(mapFile);
        if (metricDynConnFile != NULL) {
            metricDynConnFiles.push_back(metricDynConnFile);
        }
        
        VolumeDynamicConnectivityFile* volDynnFile = dynamic_cast<VolumeDynamicConnectivityFile*>(mapFile);
        if (volDynnFile != NULL) {
            volDynConnFiles.push_back(volDynnFile);
        }
    }
    const bool hasDynamicConnectivity = ( ( ! ciftiMatrixFiles.empty())
                                         || ( ! metricDynConnFiles.empty())
                                         || ( ! volDynConnFiles.empty()) );
    const bool haveCiftiFiberTrajectoryFiles = ( ! ciftiFiberTrajectoryFiles.empty());
    const bool haveChartableFiles = ( ! chartableFiles.empty());

    /*
     * Actions for each file type
     */
    QList<QAction*> connectivityActions;
    QActionGroup* connectivityActionGroup = new QActionGroup(this);
    QObject::connect(connectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(connectivityActionSelected(QAction*)));
    
    QList<QAction*> ciftiFiberTrajectoryActions;
    QActionGroup* ciftiFiberTrajectoryActionGroup = new QActionGroup(this);
    QObject::connect(ciftiFiberTrajectoryActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiFiberTrajectoryActionSelected(QAction*)));
    
    QList<QAction*> chartableDataActions;
    QActionGroup* chartableDataActionGroup = new QActionGroup(this);
    QObject::connect(chartableDataActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelChartableDataActionSelected(QAction*)));
    
    for (auto parcelConnectivity : this->parcelConnectivities) {
        const ParcelType parcelType = parcelConnectivity->parcelType;
        const AString sourceLabelName("Region \""
                                      + parcelConnectivity->labelName
                                      + "\" from map \""
                                      + parcelConnectivity->mapName
                                      + "\"");
        if (hasDynamicConnectivity) {
            bool matchFlag = false;
            if (parcelType == ParcelType::PARCEL_TYPE_SURFACE_NODES) {
                matchFlag = true;
            }
            else if (parcelType == ParcelType::PARCEL_TYPE_VOLUME_VOXELS) {
                for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
                     iter != ciftiMatrixFiles.end();
                     iter++) {
                    const CiftiMappableConnectivityMatrixDataFile* ciftiFile = *iter;
                    if (ciftiFile->matchesDimensions(parcelConnectivity->volumeDimensions[0],
                                                     parcelConnectivity->volumeDimensions[1],
                                                     parcelConnectivity->volumeDimensions[2])) {
                        matchFlag = true;
                        break;
                    }
                }
                
                for (auto volDynFile : volDynConnFiles) {
                    if (volDynFile->matchesDimensions(parcelConnectivity->volumeDimensions[0],
                                                      parcelConnectivity->volumeDimensions[1],
                                                      parcelConnectivity->volumeDimensions[2])) {
                        matchFlag = true;
                        break;
                    }
                }
            }
            
            if (matchFlag) {
                const AString actionName("Show Connectivity for "
                                         + sourceLabelName);
                QAction* action = connectivityActionGroup->addAction(actionName);
                action->setData(QVariant::fromValue((void*)parcelConnectivity));
                connectivityActions.push_back(action);
            }
        }
        
        if (haveCiftiFiberTrajectoryFiles) {
            const AString fiberTrajActionName("Show Average Fiber Trajectory for "
                                              + sourceLabelName);
            QAction* fiberTrajAction = ciftiFiberTrajectoryActionGroup->addAction(fiberTrajActionName);
            fiberTrajAction->setData(QVariant::fromValue((void*)parcelConnectivity));
            ciftiFiberTrajectoryActions.push_back(fiberTrajAction);
        }
        
        if (haveChartableFiles) {
            bool matchFlag = false;
            if (parcelType == ParcelType::PARCEL_TYPE_SURFACE_NODES) {
                matchFlag = true;
            }
            else if (parcelType == ParcelType::PARCEL_TYPE_VOLUME_VOXELS) {
                for (std::vector<ChartableLineSeriesBrainordinateInterface*>::iterator iter = chartableFiles.begin();
                     iter != chartableFiles.end();
                     iter++) {
                    const ChartableLineSeriesBrainordinateInterface* chartFile = *iter;
                    const CaretMappableDataFile* mapDataFile = chartFile->getLineSeriesChartCaretMappableDataFile();
                    if (mapDataFile != NULL){
                        if (mapDataFile->isVolumeMappable()) {
                            const VolumeMappableInterface* volMap = dynamic_cast<const VolumeMappableInterface*>(mapDataFile);
                            if (volMap->matchesDimensions(parcelConnectivity->volumeDimensions[0],
                                                          parcelConnectivity->volumeDimensions[1],
                                                          parcelConnectivity->volumeDimensions[2])) {
                                matchFlag = true;
                                break;
                            }
                            
                        }
                    }
                }
            }
            
            if (matchFlag) {
                const AString tsActionName("Show Data/Time Series Graph For "
                                           + sourceLabelName);
                QAction* tsAction = chartableDataActionGroup->addAction(tsActionName);
                tsAction->setData(QVariant::fromValue((void*)parcelConnectivity));
                chartableDataActions.push_back(tsAction);
            }
        }
    }

    std::vector<QAction*> allActions;
    allActions.insert(allActions.end(), connectivityActions.begin(), connectivityActions.end());
    allActions.insert(allActions.end(), ciftiFiberTrajectoryActions.begin(), ciftiFiberTrajectoryActions.end());
    allActions.insert(allActions.end(), chartableDataActions.begin(), chartableDataActions.end());
   
    QMenu* menu(NULL);
    
    if ( ! allActions.empty()) {
        menu = new QMenu("Label ROI");
        
        for (auto action : allActions) {
            menu->addAction(action);
        }
    }
    
    return menu;
}

/**
 * Add chart options to the menu.
 * @return The Chart menu
 */
QMenu*
UserInputModeViewContextMenu::createChartMenu()
{
    QList<QAction*> chartActions;

    SelectionItemChartTwoLabel* labelID = this->selectionManager->getChartTwoLabelIdentification();
    if (labelID->isValid()) {
        chartActions.push_back(WuQtUtilities::createAction("Edit Chart Axis Label...",
                                                                "",
                                                                this,
                                                                this,
                                                                SLOT(editChartLabelSelected())));
    }
    
    if (this->browserTabContent != NULL) {
        const ModelChartTwo* chartTwoModel = this->browserTabContent->getDisplayedChartTwoModel();
        if (chartTwoModel != NULL) {
            const int32_t tabIndex = this->browserTabContent->getTabNumber();
            switch (chartTwoModel->getSelectedChartTwoDataType(tabIndex)) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                {
                    QList<QAction*> layerActions = getChartTwoLineLayerMenuActions();
                    chartActions.append(layerActions);
                }
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
            }
        }
    }
    
    QMenu* menu(NULL);

    if ( ! chartActions.isEmpty()) {
        menu = new QMenu("Chart");
        
        for (auto action : chartActions) {
            menu->addAction(action);
        }
    }
    
    return menu;
}

/**
 * @return Menu actions for chart two line layer
 */
QList<QAction*>
UserInputModeViewContextMenu::getChartTwoLineLayerMenuActions()
{
    QList<QAction*> actions;
    
    const ChartTwoOverlaySet* overlaySet = this->browserTabContent->getChartTwoOverlaySet();
    const int32_t numValidOverlays = overlaySet->getNumberOfDisplayedOverlays();
    if (numValidOverlays > 0) {
    }

    return actions;
}


/**
 * Called to edit the chart label.
 */
void
UserInputModeViewContextMenu::editChartLabelSelected()
{
    SelectionItemChartTwoLabel* labelID = this->selectionManager->getChartTwoLabelIdentification();
    if (labelID->isValid()) {
        ChartTwoCartesianAxis* axis = labelID->getChartTwoCartesianAxis();
        ChartTwoOverlaySet* chartOverlaySet = labelID->getChartOverlaySet();
        if ((axis != NULL)
            && (chartOverlaySet != NULL)) {
            WuQDataEntryDialog newNameDialog("Axis Label",
                                             this);
            QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Label");
            lineEdit->setText(chartOverlaySet->getAxisLabel(axis));
            if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
                const AString name = lineEdit->text().trimmed();
                chartOverlaySet->setAxisLabel(axis,
                                              name);
                
                /*
                 * Update graphics.
                 */
                EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
        }
    }
}

/**
 * @return The foci menu
 */
QMenu*
UserInputModeViewContextMenu::createFociMenu()
{
    QList<QAction*> fociActions;
    
    const SelectionItemUniversalIdentificationSymbol* idSymbol = selectionManager->getUniversalIdentificationSymbol();
    SelectionItemFocusSurface* focusID = this->selectionManager->getSurfaceFocusIdentification();
    SelectionItemSurfaceNode* surfaceID = this->selectionManager->getSurfaceNodeIdentification();
    SelectionItemVoxel* idVoxel = this->selectionManager->getVoxelIdentification();
    SelectionItemFocusVolume* focusVolID = this->selectionManager->getVolumeFocusIdentification();
    
    
    /*
     * Create focus at surface node or at ID symbol
     */
    if (surfaceID->isValid()
        && ( ! focusID->isValid())) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const QString text = ("Create Focus at Vertex "
                              + QString::number(nodeIndex)
                              + "...");
        
        fociActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceFocusSelected())));
    }
    else if (idSymbol->isValid()
             && ( ! focusID->isValid())) {
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        StructureEnum::Enum structure(StructureEnum::INVALID);
        int32_t surfaceNumberOfVertices(-1);
        int32_t surfaceVertexIndex(-1);
        if (idManager->getSurfaceInformationForIdentificationSymbol(idSymbol,
                                                                    structure,
                                                                    surfaceNumberOfVertices,
                                                                    surfaceVertexIndex)) {
            
            const QString text = ("Create Focus at Selected Vertex "
                                  + QString::number(surfaceVertexIndex)
                                  + "...");
            
            fociActions.push_back(WuQtUtilities::createAction(text,
                                                              "",
                                                              this,
                                                              this,
                                                              SLOT(createSurfaceIDSymbolFocusSelected())));
        }
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
        fociActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createVolumeFocusSelected())));
    }
    
    /*
     * Edit Surface Focus
     */
    if (focusID->isValid()) {
        const QString text = ("Edit Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ")");
        fociActions.push_back(WuQtUtilities::createAction(text,
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
        fociActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editVolumeFocusSelected())));
    }

    QMenu* menu(NULL);
   
    if ( ! fociActions.isEmpty()) {
        menu = new QMenu("Foci");
        
        for (auto action : fociActions) {
            menu->addAction(action);
        }
    }

    return menu;
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
UserInputModeViewContextMenu::connectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    std::vector<VoxelIJK> voxelIndices;
    
    switch (pc->parcelType) {
        case ParcelType::PARCEL_TYPE_INVALID:
            break;
        case ParcelType::PARCEL_TYPE_SURFACE_NODES:
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
        case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
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
            case ParcelType::PARCEL_TYPE_INVALID:
                break;
            case ParcelType::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiConnectivityManager->loadAverageDataForSurfaceNodes(pc->brain,
                                                                             pc->surface,
                                                                             nodeIndices);
            {
                std::vector<MetricDynamicConnectivityFile*> metricDynConnFiles;
                pc->brain->getMetricDynamicConnectivityFiles(metricDynConnFiles);
                for (auto mdcf : metricDynConnFiles) {
                    mdcf->loadAverageDataForSurfaceNodes(pc->surface->getNumberOfNodes(),
                                                         pc->surface->getStructure(),
                                                         nodeIndices);
                }
            }
                break;
            case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
                pc->ciftiConnectivityManager->loadAverageDataForVoxelIndices(pc->brain,
                                                                             pc->volumeDimensions,
                                                                             voxelIndices);
                
            {
                std::vector<VolumeDynamicConnectivityFile*> volDynConnFiles;
                pc->brain->getVolumeDynamicConnectivityFiles(volDynConnFiles);
                for (auto vdcf : volDynConnFiles) {
                    vdcf->loadMapAverageDataForVoxelIndices(pc->volumeDimensions,
                                                            voxelIndices);
                }
            }
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
        case ParcelType::PARCEL_TYPE_INVALID:
            break;
        case ParcelType::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            break;
        case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
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
            case ParcelType::PARCEL_TYPE_INVALID:
                break;
            case ParcelType::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiFiberTrajectoryManager->loadDataAverageForSurfaceNodes(pc->brain,
                                                                                pc->surface,
                                                                                nodeIndices);
                break;
            case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
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
            
            {
                Brain* brain = GuiManager::get()->getBrain();
                std::vector<MetricDynamicConnectivityFile*> metricDynConnFiles;
                brain->getMetricDynamicConnectivityFiles(metricDynConnFiles);
                for (auto mdcf : metricDynConnFiles) {
                    mdcf->loadAverageDataForSurfaceNodes(surface->getNumberOfNodes(),
                                                         surface->getStructure(),
                                                         nodeIndices);
                }
            }
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
        case ParcelType::PARCEL_TYPE_INVALID:
            break;
        case ParcelType::PARCEL_TYPE_SURFACE_NODES:
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
        case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
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
            case ParcelType::PARCEL_TYPE_INVALID:
                break;
            case ParcelType::PARCEL_TYPE_SURFACE_NODES:
                pc->chartingDataManager->loadAverageChartForSurfaceNodes(pc->surface,
                                                                         nodeIndices);
                break;
            case ParcelType::PARCEL_TYPE_VOLUME_VOXELS:
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
    
    int32_t tabIndex = -1;
    if (this->browserTabContent != NULL) {
        tabIndex = this->browserTabContent->getTabNumber();
    }
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(IdentifiedItemUniversal::newInstanceTextNoSymbolIdentification(this->selectionManager->getSimpleIdentificationText(brain),
                                                                                                this->selectionManager->getFormattedIdentificationText(brain, tabIndex)));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to create a focus at a node location
 */
void
UserInputModeViewContextMenu::createSurfaceFocusSelected()
{
    SelectionItemSurfaceNode* surfaceID = this->selectionManager->getSurfaceNodeIdentification();
    const Surface* focusSurface = surfaceID->getSurface();
    const StructureEnum::Enum structure = focusSurface->getStructure();
    const Surface* anatSurface = GuiManager::get()->getBrain()->getPrimaryAnatomicalSurfaceForStructure(structure);
    const Surface* surface((anatSurface != NULL)
                           ? anatSurface
                           : focusSurface);
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
    SelectionManager* selectionManager(GuiManager::get()->getBrain()->getSelectionManager());
    const SelectionItemUniversalIdentificationSymbol* idSymbol = selectionManager->getUniversalIdentificationSymbol();
    
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    StructureEnum::Enum structure(StructureEnum::INVALID);
    int32_t surfaceNumberOfVertices(-1);
    int32_t surfaceVertexIndex(-1);
    if (idManager->getSurfaceInformationForIdentificationSymbol(idSymbol,
                                                                structure,
                                                                surfaceNumberOfVertices,
                                                                surfaceVertexIndex)) {
        double modelXYZ[3];
        idSymbol->getModelXYZ(modelXYZ);
        const float xyz[3] {
            static_cast<float>(modelXYZ[0]),
            static_cast<float>(modelXYZ[1]),
            static_cast<float>(modelXYZ[2])
        };
        const AString focusName = (StructureEnum::toGuiName(structure)
                                   + " Vertex "
                                   + AString::number(surfaceVertexIndex));
        
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
    
    int32_t tabIndex = -1;
    if (this->browserTabContent != NULL) {
        tabIndex = this->browserTabContent->getTabNumber();
    }
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(IdentifiedItemUniversal::newInstanceTextNoSymbolIdentification(this->selectionManager->getSimpleIdentificationText(brain),
                                                                                                this->selectionManager->getFormattedIdentificationText(brain, tabIndex)));
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
    
    int32_t tabIndex = -1;
    if (this->browserTabContent != NULL) {
        tabIndex = this->browserTabContent->getTabNumber();
    }
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(IdentifiedItemUniversal::newInstanceTextNoSymbolIdentification(this->selectionManager->getSimpleIdentificationText(brain),
                                                                                                this->selectionManager->getFormattedIdentificationText(brain, tabIndex)));
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
    
    int32_t tabIndex = -1;
    if (this->browserTabContent != NULL) {
        tabIndex = this->browserTabContent->getTabNumber();
    }
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(IdentifiedItemUniversal::newInstanceTextNoSymbolIdentification(this->selectionManager->getSimpleIdentificationText(brain),
                                                                                                this->selectionManager->getFormattedIdentificationText(brain, tabIndex)));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to remove all node identification symbols.
 */
void 
UserInputModeViewContextMenu::removeAllIdentificationSymbolsSelected()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    idManager->removeAllIdentifiedItems();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to remove identification symbol from node.
 */
void
UserInputModeViewContextMenu::removeIdentificationSymbolSelected()
{
    SelectionItemUniversalIdentificationSymbol* idSymbol = selectionManager->getUniversalIdentificationSymbol();
    if (idSymbol->isValid()) {
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        idManager->removeIdentifiedItem(idSymbol);
        
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

/**
 * Called to display RGBA for pixel under mouse
 */
void
UserInputModeViewContextMenu::showFrameBufferPixelRgbaSelected()
{
    CaretAssert(this->parentOpenGLWidget);
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    QImage image(this->parentOpenGLWidget->grabFramebuffer());
#else
    QImage image(this->parentOpenGLWidget->grabFrameBuffer());
#endif

    const QPoint mouseXY(this->parentOpenGLWidget->mapFromGlobal(pos()));
    const int32_t x(mouseXY.x());
    const int32_t y(mouseXY.y());

    if ( ! image.isNull()) {
        if ((x >= 0)
            && (x < image.width())
            && (y >= 0)
            && (y < image.height())) {
            QColor color = image.pixelColor(x, y);
            double red(color.redF());
            double green(color.greenF());
            double blue(color.blueF());
            double alpha(color.alphaF());
            
            int redInt(color.red());
            int greenInt(color.green());
            int blueInt(color.blue());
            int alphaInt(color.alpha());
            
            QString formatName(QString::number(image.format()));
            switch (image.format()) {
                case QImage::Format_ARGB32_Premultiplied:
                    formatName = "Format_ARGB32_Premultiplied";
                    break;
                case QImage::Format_ARGB32:
                    formatName = "Format_ARGB32";
                    break;
                default:
                    break;
            }
            
            const int intWidth(5);
            const int floatWidth(4);
            const int precision(2);
            const char format('f');
            AString txt("<html>");
            txt.appendWithNewLine("QImage::Format: " + formatName);
            txt.appendWithNewLine(QString("Pixel XY (origin top left): (%1, %2) <br>").arg(x).arg(y));
            txt.appendWithNewLine(QString("Red:    %1   %2 <br>").arg(redInt, intWidth).arg(red, floatWidth, format, precision));
            txt.appendWithNewLine(QString("Green:  %1   %2 <br>").arg(greenInt, intWidth).arg(green, floatWidth, format, precision));
            txt.appendWithNewLine(QString("Blue:   %1   %2 <br>").arg(blueInt, intWidth).arg(blue, floatWidth, format, precision));
            if (image.hasAlphaChannel()) {
                txt.appendWithNewLine(QString("Alpha:  %1   %2 <br>").arg(alphaInt, intWidth).arg(alpha, floatWidth, format, precision));
            }
            else {
                txt.appendWithNewLine("Alpha: no alpha channel in image.<br>");
            }

            if (image.format() == QImage::Format_ARGB32_Premultiplied) {
                red   *= alpha;
                green *= alpha;
                blue  *= alpha;
                
                redInt   = static_cast<int>(red * 255.0);
                greenInt = static_cast<int>(green * 255.0);
                blueInt  = static_cast<int>(blue * 255.0);
                alphaInt = static_cast<int>(alpha * 255.0);
                
                txt.appendWithNewLine("<p>");
                txt.appendWithNewLine("After removal of alpha pre-multiplication:");
                txt.appendWithNewLine(QString("Red:    %1   %2 <br>").arg(redInt, intWidth).arg(red, floatWidth, format, precision));
                txt.appendWithNewLine(QString("Green:  %1   %2 <br>").arg(greenInt, intWidth).arg(green, floatWidth, format, precision));
                txt.appendWithNewLine(QString("Blue:   %1   %2 <br>").arg(blueInt, intWidth).arg(blue, floatWidth, format, precision));
                if (image.hasAlphaChannel()) {
                    txt.appendWithNewLine(QString("Alpha:  %1   %2 <br>").arg(alphaInt, intWidth).arg(alpha, floatWidth, format, precision));
                }
            }

            const bool debugFlag(false);
            if (debugFlag) {
                for (int32_t i = 0; i < 30; i++) {
                    QImage imageCopy = image.convertToFormat((QImage::Format)i);
                    QColor color = imageCopy.pixelColor(x, y);
                    AString txt;
                    txt.appendWithNewLine("<p>");
                    txt.appendWithNewLine("QImage::Format: " + AString::number(imageCopy.format()));
                    txt.appendWithNewLine(QString("Pixel XY (origin top left): (%1, %2) <br>").arg(x).arg(y));
                    txt.appendWithNewLine(QString("Red:    %1   %2 <br>").arg((int)color.red(), intWidth).arg((double)color.redF(), floatWidth, format, precision));
                    txt.appendWithNewLine(QString("Green:  %1   %2 <br>").arg((int)color.green(), intWidth).arg((double)color.greenF(), floatWidth, format, precision));
                    txt.appendWithNewLine(QString("Blue:   %1   %2 <br>").arg((int)color.blue(), intWidth).arg((double)color.blueF(), floatWidth, format, precision));
                    if (image.hasAlphaChannel()) {
                        txt.appendWithNewLine(QString("Alpha:  %1   %2 <br>").arg((int)color.alpha(), intWidth).arg((double)color.alphaF(), floatWidth, format, precision));
                    }
                    else {
                        txt.appendWithNewLine("Alpha: no alpha channel in image.<br>");
                    }
                    std::cout << txt << std::endl << std::endl;
                }
            }
            
            txt.appendWithNewLine("</html>");
            
            WuQMessageBox::informationOk(this->parentOpenGLWidget,
                                         txt.replace(" ", "&nbsp;"));
        }
    }
}


/* ------------------------------------------------------------------------- */
/**
 * Constructor.
 */
UserInputModeViewContextMenu::ParcelConnectivity::ParcelConnectivity(Brain* brain,
                                                                     const ParcelType parcelType,
                                                                     CaretMappableDataFile* mappableLabelFile,
                                                                     const int32_t labelFileMapIndex,
                                                                     const AString& mapName,
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
    this->mapName = mapName;
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
    
    if (this->parcelType != ParcelType::PARCEL_TYPE_SURFACE_NODES) {
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

    if (this->parcelType != ParcelType::PARCEL_TYPE_VOLUME_VOXELS) {
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


