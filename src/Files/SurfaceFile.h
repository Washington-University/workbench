
#ifndef __SURFACE_FILE_H__
#define __SURFACE_FILE_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <vector>
#include <stdint.h>

#include "BrainConstants.h"
#include "CaretMutex.h"
#include "CaretPointer.h"
#include "CaretPointLocator.h"
#include "EventManager.h"
#include "EventListenerInterface.h"
#include "GeodesicHelper.h"
#include "GiftiTypeFile.h"
#include "SurfaceTypeEnum.h"
#include "TopologyHelper.h"

namespace caret {

    class BoundingBox;
    class GiftiDataArray;
    
    /**
     * A surface data file.
     */
    class SurfaceFile : public GiftiTypeFile, EventListenerInterface {
        
    public:
        SurfaceFile();
        
        SurfaceFile(const SurfaceFile& sf);
        
        SurfaceFile& operator=(const SurfaceFile& sf);
        
        virtual ~SurfaceFile();
        
        virtual void receiveEvent(Event* event);
        
        virtual void clear();
        
        virtual int32_t getNumberOfNodes() const;
        
        virtual int32_t getNumberOfColumns() const;
        
        const float* getCoordinate(const int32_t nodeIndex) const;
        
        void getCoordinate(const int32_t nodeIndex,
                           float xyzOut[3]) const;
        
        const float* getCoordinateData() const;
        
        const float* getNormalVector(const int32_t nodeIndex) const;
        
        const float* getNormalData() const;
        
        int getNumberOfTriangles() const;
        
        const int32_t* getTriangle(const int32_t) const;
        
        void computeNormals(bool averageNormals = false);
                
        const float* getNodeColor(const int32_t nodeIndex) const;
        
        SurfaceTypeEnum::Enum getSurfaceType() const;
        
        void setSurfaceType(const SurfaceTypeEnum::Enum surfaceType);
        
        SecondarySurfaceTypeEnum::Enum getSecondaryType() const;
        
        void setSecondaryType(const SecondarySurfaceTypeEnum::Enum secondaryType);
        
        CaretPointer<TopologyHelper> getTopologyHelper(bool infoSorted = false) const;
        
        void getTopologyHelper(CaretPointer<TopologyHelper>& helpOut, bool infoSorted = false) const;
        
        CaretPointer<GeodesicHelper> getGeodesicHelper() const;
        
        void getGeodesicHelper(CaretPointer<GeodesicHelper>& helpOut) const;
        
        const BoundingBox* getBoundingBox() const;
        
        void computeNodeAreas(std::vector<float>& areasOut) const;
        
        ///find the closest node on the surface, within maxDist if maxDist is positive
        int32_t closestNode(const float target[3], const float maxDist = -1.0f) const;
        
        virtual void setModified();
        
        AString getInformation() const;
        
        float* getSurfaceNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex);
        
        void setSurfaceNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                                              const float* rgbaNodeColorComponents);
        
        float* getSurfaceMontageNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex);
        
        void setSurfaceMontageNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                                                     const float* rgbaNodeColorComponents);
        
        float* getWholeBrainNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex);
        
        void setWholeBrainNodeColoringRgbaForBrowserTab(const int32_t browserTabIndex,
                                              const float* rgbaNodeColorComponents);
        
    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading() throw (DataFileException);
        
        void copyHelperSurfaceFile(const SurfaceFile& sf);
        
        void initializeMembersSurfaceFile();
        
    private:
        void invalidateNodeColoringForBrowserTabs();
        
        void allocateSurfaceNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                                      const bool zeroizeColorsFlag);
        
        void allocateSurfaceMontageNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                                      const bool zeroizeColorsFlag);
        
        void allocateWholeBrainNodeColoringForBrowserTab(const int32_t browserTabIndex,
                                                         const bool zeroizeColorsFlag);
        
        /** Data array containing the coordinates. */
        GiftiDataArray* coordinateDataArray;
        
        /** 
         * This coloring is used when a ONE surface is displayed.
         * Node color components Red, Green, Blue, Alpha for each browser tab.
         * Each element of the vector points to the coloring
         * for a browser tab with the corresponding index.
         */
        std::vector<float> surfaceNodeColoringForBrowserTabs[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** 
         * This coloring is used when a surface montage is displayed.
         * Node color components Red, Green, Blue, Alpha for each browser tab.
         * Each element of the vector points to the coloring
         * for a browser tab with the corresponding index.
         */
        std::vector<float> surfaceMontageNodeColoringForBrowserTabs[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** 
         * This coloring is used when a Whole Brain is displayed.
         * Node color components Red, Green, Blue, Alpha for each browser tab.
         * Each element of the vector points to the coloring
         * for a browser tab with the corresponding index.
         */
        std::vector<float> wholeBrainNodeColoringForBrowserTabs[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Points to memory containing the coordinates. */
        float* coordinatePointer;
        
        /** Data array containing the triangles. */
        GiftiDataArray* triangleDataArray;
        
        /** Points to memory containing the triangles. */
        int32_t* trianglePointer;
        
        /** surface normal vectors. */
        std::vector<float> normalVectors;
        
        bool m_normalsAveraged, m_normalsComputed;

        /** The node coloring. */
        std::vector<float> nodeColoring;
        
        ///topology base for surface
        mutable CaretPointer<TopologyHelperBase> m_topoBase;
        
        ///tracks allocated TopologyHelpers for this class
        mutable std::vector<CaretPointer<TopologyHelper> > m_topoHelpers;
        
        ///used to search through topology helpers without starting from 0 every time, wraps around
        mutable int32_t m_topoHelperIndex;
        
        ///the geodesic base for this surface
        mutable CaretPointer<GeodesicHelperBase> m_geoBase;
        
        ///tracks allocated geodesic helpers for this class
        mutable std::vector<CaretPointer<GeodesicHelper> > m_geoHelpers;
        
        ///used to search through geodesic helpers without starting from 0 every time, wraps around
        mutable int32_t m_geoHelperIndex;
        
        ///used to search for the closest point in the surface
        mutable CaretPointer<CaretPointLocator> m_locator;
        
        ///used to track when the surface file gets changed
        void invalidateHelpers();
        
        mutable BoundingBox* boundingBox;
        
        mutable CaretMutex m_topoHelperMutex, m_geoHelperMutex, m_locatorMutex;
    };

} // namespace

#endif // __SURFACE_FILE_H__
