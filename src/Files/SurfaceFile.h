
#ifndef __SURFACE_FILE_H__
#define __SURFACE_FILE_H__

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

#include <vector>
#include <stdint.h>

#include "BrainConstants.h"
#include "CaretMutex.h"
#include "CaretPointer.h"
#include "EventManager.h"
#include "EventListenerInterface.h"
#include "GiftiTypeFile.h"
#include "SurfaceTypeEnum.h"

namespace caret {

    class BoundingBox;
    class CaretPointLocator;
    class DescriptiveStatistics;
    class FastStatistics;
    class GeodesicHelper;
    class GeodesicHelperBase;
    class GiftiDataArray;
    class Matrix4x4;
    class PlainTextStringBuilder;
    class SignedDistanceHelper;
    class SignedDistanceHelperBase;
    class TopologyHelper;
    class TopologyHelperBase;
    
    /**
     * A surface data file.
     */
    class SurfaceFile : public GiftiTypeFile, EventListenerInterface {
        
    public:
        SurfaceFile();
        
        SurfaceFile(const SurfaceFile& sf);
        
        SurfaceFile& operator=(const SurfaceFile& sf);
        
        virtual ~SurfaceFile();
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        virtual void receiveEvent(Event* event);
        
        virtual void clear();
        
        virtual int32_t getNumberOfNodes() const;
        
        virtual int32_t getNumberOfColumns() const;
        
        void setNumberOfNodesAndTriangles(const int32_t& nodes, const int32_t& triangles);
        
        const float* getCoordinate(const int32_t nodeIndex) const;
        
        void getCoordinate(const int32_t nodeIndex,
                           float xyzOut[3]) const;
        
        void setCoordinate(const int32_t nodeIndex,
                           const float xyzIn[3]);

        void setCoordinate(const int32_t nodeIndex, const float xIn, const float yIn, const float zIn);

        void setCoordinates(const float *coordinates);
        
        const float* getCoordinateData() const;
        
        const float* getNormalVector(const int32_t nodeIndex) const;
        
        const float* getNormalData() const;
        
        int getNumberOfTriangles() const;
        
        const int32_t* getTriangle(const int32_t index) const;
        
        void setTriangle(const int32_t& index, const int32_t* nodes);
        
        void setTriangle(const int32_t& index, const int32_t& node1, const int32_t& node2, const int32_t& node3);
        
        int32_t getTriangleThatSharesEdge(const int32_t n1,
                                          const int32_t n2,
                                          const int32_t oppositeTriangle) const;
        
        void computeNormals();
        
        std::vector<float> computeAverageNormals();
                
        void getTriangleNormalVector(const int32_t triangleIndex,
                                     float normalOut[3]) const;
        
        SurfaceTypeEnum::Enum getSurfaceType() const;
        
        void setSurfaceType(const SurfaceTypeEnum::Enum surfaceType);
        
        SecondarySurfaceTypeEnum::Enum getSecondaryType() const;
        
        void setSecondaryType(const SecondarySurfaceTypeEnum::Enum secondaryType);
        
        float getSphericalRadius() const;
        
        float getSurfaceArea() const;
        
        CaretPointer<TopologyHelper> getTopologyHelper(bool infoSorted = false) const;
        
        void getTopologyHelper(CaretPointer<TopologyHelper>& helpOut, bool infoSorted = false) const;
        
        CaretPointer<GeodesicHelper> getGeodesicHelper() const;
        
        void getGeodesicHelper(CaretPointer<GeodesicHelper>& helpOut) const;
        
        CaretPointer<SignedDistanceHelper> getSignedDistanceHelper() const;
        
        void getSignedDistanceHelper(CaretPointer<SignedDistanceHelper>& helpOut) const;
        
        CaretPointer<const CaretPointLocator> getPointLocator() const;
        
        void clearCachedHelpers() const;
        
        const BoundingBox* getBoundingBox() const;
        
        void matchSurfaceBoundingBox(const SurfaceFile* surfaceFile);
        
        void applyMatrix(const Matrix4x4& matrix);
        
        void getNodesSpacingStatistics(DescriptiveStatistics& statsOut) const;
        
        void getNodesSpacingStatistics(FastStatistics& statsOut) const;
        
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

        void invalidateNormals();
        
        void translateToCenterOfMass();
        
        void flipNormals();
        
        bool areNormalVectorsCorrect() const;
        
        ///check that it has EXACTLY the same topology, with no flipped normals or rotated or reordered triangles
        bool matchesTopology(const SurfaceFile& rhs) const;
        
        ///check only that each node is connected to the same set of other nodes, allow any other form of mischief
        bool hasNodeCorrespondence(const SurfaceFile& rhs) const;
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
    
        //override writeFile in order to check filename against type of file
        virtual void writeFile(const AString& filename);

    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading();
        
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
        
        bool m_normalsComputed;
        
        bool m_skipSanityCheck;

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
        
        ///the geodesic base for this surface
        mutable CaretPointer<SignedDistanceHelperBase> m_distBase;
        
        ///tracks allocated geodesic helpers for this class
        mutable std::vector<CaretPointer<SignedDistanceHelper> > m_distHelpers;
        
        ///used to search through geodesic helpers without starting from 0 every time, wraps around
        mutable int32_t m_distHelperIndex;
        
        ///used to search for the closest point in the surface
        mutable CaretPointer<CaretPointLocator> m_locator;
        
        ///used to track when the surface file gets changed
        void invalidateHelpers();
        
        mutable BoundingBox* boundingBox;
        
        mutable CaretMutex m_topoHelperMutex, m_geoHelperMutex, m_locatorMutex, m_distHelperMutex;
    };

} // namespace

#endif // __SURFACE_FILE_H__
