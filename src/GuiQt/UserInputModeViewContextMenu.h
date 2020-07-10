#ifndef __USER_INPUT_MODE_VIEW_CONTEXT_MENU_H__
#define __USER_INPUT_MODE_VIEW_CONTEXT_MENU_H__

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

#include <stdint.h>
#include <vector>

#include <QMenu>

#include "StructureEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VoxelIJK.h"

class QAction;

namespace caret {

    class Brain;
    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    class BrowserTabContent;
    class CaretMappableDataFile;
    class CiftiConnectivityMatrixDataFileManager;
    class CiftiFiberTrajectoryManager;
    class ChartingDataManager;
    class ModelChartTwo;
    class MouseEvent;
    class SelectionManager;
    class LabelFile;
    class Surface;
    
    class UserInputModeViewContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        UserInputModeViewContextMenu(const MouseEvent& mouseEvent,
                                     BrainOpenGLViewportContent* viewportContent,
                                     SelectionManager* selectionManager,
                                     BrainOpenGLWidget* parentOpenGLWidget);
        
        virtual ~UserInputModeViewContextMenu();
        
    private slots:
        void createSurfaceFocusSelected();
        
        void createSurfaceIDSymbolFocusSelected();
        
        void createVolumeFocusSelected();
        
        void editSurfaceFocusSelected();
        
        void editVolumeFocusSelected();
        
        void removeAllNodeIdentificationSymbolsSelected();
        
        void removeNodeIdentificationSymbolSelected();
        
        void identifySurfaceBorderSelected();
        
        void identifySurfaceFocusSelected();
        
        void identifyVolumeFocusSelected();
        
        void identifySurfaceNodeSelected();

        void identifyVoxelSelected();

        void parcelCiftiFiberTrajectoryActionSelected(QAction* action);
        
        void connectivityActionSelected(QAction* action);

        void parcelChartableDataActionSelected(QAction* action);
        
        void borderCiftiConnectivitySelected();

        void borderDataSeriesSelected();
        
        void editChartLabelSelected();
        
    private:
        enum class ParcelType {
            PARCEL_TYPE_INVALID,
            PARCEL_TYPE_SURFACE_NODES,
            PARCEL_TYPE_VOLUME_VOXELS
        };
        
        class ParcelConnectivity {
        public:
            ParcelConnectivity(Brain* brain,
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
                               CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager);
            
            virtual ~ParcelConnectivity();
            
            void getNodeIndices(std::vector<int32_t>& nodeIndicesOut) const;
            
            void getVoxelIndices(std::vector<VoxelIJK>& voxelIndicesOut) const;
            
            Brain* brain;
            ParcelType parcelType;
            CaretMappableDataFile* mappableLabelFile;
            int32_t labelFileMapIndex;
            AString mapName;
            int32_t labelKey;
            QString labelName;
            Surface* surface;
            int32_t nodeNumber;
            int64_t volumeDimensions[3];
            CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager;
            ChartingDataManager* chartingDataManager;
            CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager;
        };
        
        UserInputModeViewContextMenu(const UserInputModeViewContextMenu&);

        UserInputModeViewContextMenu& operator=(const UserInputModeViewContextMenu&);
        
        bool warnIfNetworkBrainordinateCountIsLarge(const int64_t numberOfBrainordinatesInROI);
        
        bool enableDataSeriesGraphsIfNoneEnabled();
        
        QMenu* createIdentifyMenu();
        
        QMenu* createBorderRegionOfInterestMenu();
        
        QMenu* createChartMenu();
        
        QMenu* createFociMenu();
        
        QMenu* createLabelRegionOfInterestMenu();
        
        QList<QAction*> getChartTwoLineLayerMenuActions(const ModelChartTwo* chartTwoModel);
        
        void addSubMenuToMenu(QMenu* menu,
                              const bool addSeparatorBeforeMenu);
        
        void addActionsToMenu(QList<QAction*>& actionsToAdd,
                              const bool addSeparatorBeforeActions);
        
        void createParcelConnectivities();
        
        BrainOpenGLWidget* parentOpenGLWidget;

        std::vector<ParcelConnectivity*> parcelConnectivities;
        
        BrainOpenGLViewportContent* viewportContent;
        
        SelectionManager* selectionManager;
        
        BrowserTabContent* browserTabContent;
    };
    
#ifdef __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_VIEW_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_VIEW_CONTEXT_MENU_H__
