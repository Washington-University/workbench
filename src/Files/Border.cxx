
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __BORDER_DECLARE__
#include "Border.h"
#undef __BORDER_DECLARE__

#include <algorithm>
#include <cmath>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "GeodesicHelper.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "XmlWriter.h"

using namespace caret;
    
/**
 * \class caret::Border 
 * \brief A border is a connected line segment on a source or volume.
 */

/**
 * Constructor.
 */
Border::Border()
: CaretObjectTracksModification()
{
    clear();
//    m_color = CaretColorEnum::BLACK;
//    m_selectionClassNameModificationStatus = true; // name/class is new!!
}

/**
 * Destructor.
 */
Border::~Border()
{
    clear();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Border::Border(const Border& obj)
: CaretObjectTracksModification(obj)
{
    copyHelperBorder(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Border&
Border::operator=(const Border& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        copyHelperBorder(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Border::copyHelperBorder(const Border& obj)
{
    clear();
    
    const int32_t numPoints = obj.getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*obj.m_points[i]);
        addPoint(spi);
    }
    
    m_name = obj.m_name;
    m_className = obj.m_className;
    
    clearModified();
    setNameOrClassModified(); // new name/class so modified
}

/**
 * Clear the border.
 * Removes all points, resets names, etc.
 */
void 
Border::clear()
{
    removeAllPoints();
 
    m_groupNameSelectionItem = NULL;
    
    m_classRgbaColor[0] = 0.0;
    m_classRgbaColor[1] = 0.0;
    m_classRgbaColor[2] = 0.0;
    m_classRgbaColor[3] = 1.0;
    m_classRgbaColorValid = false;
    
    m_nameRgbaColor[0] = 0.0;
    m_nameRgbaColor[1] = 0.0;
    m_nameRgbaColor[2] = 0.0;
    m_nameRgbaColor[3] = 1.0;
    m_nameRgbaColorValid = false;
    
    m_name = "";
    m_className = "";
    setNameOrClassModified(); // new name/class so modified
}

/**
 * @return Structure to which this border is assigned.
 */
StructureEnum::Enum 
Border::getStructure()
{
    StructureEnum::Enum structure = StructureEnum::INVALID;
    
    if (m_points.empty() == false) {
        structure = m_points[0]->getStructure();
    }
    
    return structure;
}

/**
 * @return Structure to which this border is assigned.
 */
StructureEnum::Enum
Border::getStructure() const
{
    StructureEnum::Enum structure = StructureEnum::INVALID;
    
    if (m_points.empty() == false) {
        structure = m_points[0]->getStructure();
    }
    
    return structure;
}

/**
 * @return Is the class RGBA color valid?
 */
bool 
Border::isClassRgbaValid() const
{
    return m_classRgbaColorValid;
}

/**
 * Set then class RGBA color invalid.
 */
void 
Border::setClassRgbaInvalid()
{
    m_classRgbaColorValid = false;
}

/**
 * @return The class RGBA color components 
 * ranging zero to one.
 */
const float* 
Border::getClassRgba() const
{
    return m_classRgbaColor;
}

/**
 * Get the class RGBA color components 
 * ranging zero to one.
 */
void
Border::getClassRgba(float rgba[4]) const
{
    rgba[0] = m_classRgbaColor[0];
    rgba[1] = m_classRgbaColor[1];
    rgba[2] = m_classRgbaColor[2];
    rgba[3] = m_classRgbaColor[3];
}

/**
 * Set the RGBA color components assigned to the class.
 * @param rgba
 *     Red, green, blue, alpha ranging zero to one.
 */
void 
Border::setClassRgba(const float rgba[3])
{
    m_classRgbaColor[0] = rgba[0];
    m_classRgbaColor[1] = rgba[1];
    m_classRgbaColor[2] = rgba[2];
    m_classRgbaColor[3] = rgba[3];
    m_classRgbaColorValid = true;
}

/**
 * @return Is the name RGBA color valid?
 */
bool
Border::isNameRgbaValid() const
{
    return m_nameRgbaColorValid;
}

/**
 * Set then name RGBA color invalid.
 */
void
Border::setNameRgbaInvalid()
{
    m_nameRgbaColorValid = false;
}

/**
 * @return The name RGBA color components
 * ranging zero to one.
 */
const float*
Border::getNameRgba() const
{
    return m_nameRgbaColor;
}

/**
 * Get the name RGBA color components
 * ranging zero to one.
 */
void
Border::getNameRgba(float rgba[4]) const
{
    rgba[0] = m_nameRgbaColor[0];
    rgba[1] = m_nameRgbaColor[1];
    rgba[2] = m_nameRgbaColor[2];
    rgba[3] = m_nameRgbaColor[3];
}

/**
 * Set the RGBA color components assigned to the name.
 * @param rgba
 *     Red, green, blue, alpha ranging zero to one.
 */
void
Border::setNameRgba(const float rgba[4])
{
    m_nameRgbaColor[0] = rgba[0];
    m_nameRgbaColor[1] = rgba[1];
    m_nameRgbaColor[2] = rgba[2];
    m_nameRgbaColor[3] = rgba[3];
    m_nameRgbaColorValid = true;
}

/**
 * @return True if all points are on the 
 * same structure, else false.
 */
bool 
Border::verifyAllPointsOnSameStructure() const
{
    const int32_t numPoints = getNumberOfPoints();
    if (numPoints <= 1) {
        return true;
    }
    
    StructureEnum::Enum structure = m_points[0]->getStructure();

    for (int32_t i = 1; i < numPoints; i++) {
        if (m_points[i]->getStructure() != structure) {
            return false;
        }
    }
    
    return true;
}

/**
 * Remove all points in this border.
 */
void 
Border::removeAllPoints()
{
    const int32_t numPoints =getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        delete m_points[i];
    }
    m_points.clear();
    
    setModified();
}

/**
 * @return the name of the border.
 */
AString 
Border::getName() const
{
    return m_name;
}

/**
 * Set the name of the border.
 * @param name
 *   New name for border.
 */
void 
Border::setName(const AString& name)
{
    if (m_name != name) {
        m_name = name;
        setModified();
        setNameOrClassModified();
    }
}

/**
 * @return the class name of the border.
 */
AString 
Border::getClassName() const
{
    return m_className;
}

/**
 * Set the class name of the border.
 * @param className
 *   New class name for border.
 */
void 
Border::setClassName(const AString& className)
{
    if (m_className != className) {
        m_className = className;
        setModified();
        setNameOrClassModified();
    }
}

/**
 * @return Number of points in the border.
 */
int32_t
Border::getNumberOfPoints() const
{
    return m_points.size();
}

/**
 * Get the border point at the given index.
 * @param indx
 *   Index of desired border point.
 * @return
 *   Pointer to border point.
 */
const SurfaceProjectedItem* 
Border::getPoint(const int32_t indx) const
{
    CaretAssertVectorIndex(m_points, indx);
    return m_points[indx];
}

/**
 * Get the border point at the given index.
 * @param indx
 *   Index of desired border point.
 * @return
 *   Pointer to border point.
 */
SurfaceProjectedItem* 
Border::getPoint(const int32_t indx)
{
    CaretAssertVectorIndex(m_points, indx);
    return m_points[indx];
}

/**
 * Returns the index of the border point nearest
 * the given XYZ coordinate and within the 
 * given maximum distance.
 * 
 * @param surfaceFile
 *    Surface file used for unprojecting border points
 *    and producing XYZ coordinates.
 * @param xyz
 *    The XYZ coordinates for which nearest border
 *    point is desired.
 * @param maximumDistance
 *    Border points searched are limited to those
 *    within this distance from the XYZ coordinate.
 * @param distanceToNearestPointOut
 *    If a point is found within the maximum distance
 * @return
 *    Index of nearest border point or negative if
 *    no border points is within the maximum distance.
 */
int32_t 
Border::findPointIndexNearestXYZ(const SurfaceFile* surfaceFile,
                                const float xyz[3],
                                const float maximumDistance,
                                float& distanceToNearestPointOut) const
{
    CaretAssert(surfaceFile);
    const int32_t numPoints = getNumberOfPoints();
    if (numPoints <= 0) {
        return -1;
    }
    
    if (surfaceFile->getStructure() != getStructure()) {
        return -1;
    }

    int32_t nearestIndex = -1;
    float nearestDistanceSQ = maximumDistance * maximumDistance;
    
    float pointXYZ[3];
    for (int32_t i = 0; i < numPoints; i++) {
        if (m_points[i]->getProjectedPosition(*surfaceFile, 
                                              pointXYZ, 
                                                  true)) {
            const float distSQ = MathFunctions::distanceSquared3D(xyz, 
                                                                  pointXYZ);
            if (distSQ <= nearestDistanceSQ) {
                nearestDistanceSQ = distSQ;
                nearestIndex = i;
            }
        }
    }
    
    if (nearestIndex >= 0) {
        distanceToNearestPointOut = std::sqrt(nearestDistanceSQ);
    }
    return nearestIndex;
}

/**
 * Add a point to the border.  NOTE: the border
 * takes ownership of the point and will delete
 * it.  After calling this method DO NOT ever
 * use the point passed to this method.
 *
 * @param point
 *    Point that is added to the border.
 */
void 
Border::addPoint(SurfaceProjectedItem* point)
{
    m_points.push_back(point);
    setModified();
}

/**
 * Add copies of points from the given border starting
 * at startPointIndex and adding a total of pointCount
 * points.
 *
 * @param border
 *    Border from which points are copied.
 * @param startPointIndex
 *    Index of first point that is copied from border.
 *    If this value is negative, points will be copied
 *    starting from the first point in the border.
 * @param pointCount
 *    Number of points that are copied.  If this value
 *    is negative all of the remaining points in the
 *    border are copied.  If zero, none are copied.
 */
void 
Border::addPoints(const Border* border,
               const int32_t startPointIndex,
               const int32_t pointCount)
{
    CaretAssert(border);
    const int32_t startIndex = (startPointIndex >= 0) ? startPointIndex : 0;
    const int32_t endIndex   = (pointCount >= 0) ? (startIndex + pointCount) : border->getNumberOfPoints();
    
    for (int32_t i = startIndex; i < endIndex; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*border->getPoint(i));
        addPoint(spi);
    }
}

/**
 * Add points to the border so that the last point
 * connects to the first point.
 */
void 
Border::addPointsToCloseBorderWithGeodesic(const SurfaceFile* surfaceFile)
{
    const int32_t numberOfPoints = getNumberOfPoints();
    if (numberOfPoints < 3) {
        return;
    }
    
    /*
     * Index of surface node nearest first border point
     */
    float firstBorderPointXYZ[3];
    m_points[0]->getProjectedPosition(*surfaceFile, firstBorderPointXYZ, true);
    const int firstNodeIndex = surfaceFile->closestNode(firstBorderPointXYZ);
    if (firstNodeIndex < 0) {
        return;
    }
    
    /*
     * Index of surface node nearest last border point
     */
    float lastBorderPointXYZ[3];
    m_points[numberOfPoints - 1]->getProjectedPosition(*surfaceFile, lastBorderPointXYZ, true);
    const int lastNodeIndex = surfaceFile->closestNode(lastBorderPointXYZ);
    if (lastNodeIndex < 0) {
        return;
    }
    
    /*
     * Geodesics from node nearest last border point
     */
    std::vector<int32_t> nodeParents;
    std::vector<float> nodeDistances;
    CaretPointer<GeodesicHelper> geoHelp = surfaceFile->getGeodesicHelper();
    geoHelp->getGeoFromNode(lastNodeIndex,
                            nodeDistances,
                            nodeParents,
                            true);
    
    /*
     * Get path along border points
     */
    const int32_t numberOfSurfaceNodes = surfaceFile->getNumberOfNodes();
    std::vector<int32_t> pathFromFirstNodeToLastNode;
    int32_t geoNodeIndex = firstNodeIndex;
    int32_t failCounter = 0;
    while (geoNodeIndex >= 0) {
        geoNodeIndex = nodeParents[geoNodeIndex];
        if (geoNodeIndex == lastNodeIndex) {
            geoNodeIndex = -1;
        }
        else if (geoNodeIndex >= 0) {
            pathFromFirstNodeToLastNode.push_back(geoNodeIndex);
        }
        
        failCounter ++;
        if (failCounter > numberOfSurfaceNodes) {
            CaretLogWarning("Geodesic path for closing border failed.");
            pathFromFirstNodeToLastNode.clear();
        }
    }
    
    /*
     * Add points to border.
     */
    const float triangleAreas[3] = { 1.0, 0.0, 0.0 };
    const StructureEnum::Enum structure = surfaceFile->getStructure();
    const int32_t numNewPoints = static_cast<int32_t>(pathFromFirstNodeToLastNode.size());
    for (int32_t i = (numNewPoints - 1); i >= 0; i--) {
        const int32_t nodeIndex = pathFromFirstNodeToLastNode[i];
        const float* xyz = surfaceFile->getCoordinate(nodeIndex);
        
        SurfaceProjectedItem* spi = new SurfaceProjectedItem();
        spi->setStereotaxicXYZ(xyz);
        spi->setStructure(structure);
        SurfaceProjectionBarycentric* bp = spi->getBarycentricProjection();
        bp->setTriangleAreas(triangleAreas);
        const int32_t triangleNodes[3] = { nodeIndex, nodeIndex, nodeIndex };
        bp->setTriangleNodes(triangleNodes);
        bp->setValid(true);
        
        addPoint(spi);
    }
}

/**
 * Remove the point at the given index.
 * @param indx
 *    Index of point for removal.
 */
void 
Border::removePoint(const int32_t indx)
{
    CaretAssertVectorIndex(m_points, indx);
    delete m_points[indx];
    m_points.erase(m_points.begin() + indx);
    setModified();
}

/**
 * Remove the last point from the border.
 */
void 
Border::removeLastPoint()
{
    const int numPoints = getNumberOfPoints();
    if (numPoints > 0) {
        removePoint(numPoints - 1);
    }
}

/**
 * Reverse the order of points in a border.
 */
void 
Border::reverse()
{
    std::reverse(m_points.begin(),
                 m_points.end());
    setModified();
}

/**
 * Revise a border by extending from the end of a border.
 *
 * @param surfaceFile
 *    Surface on which border extension is performed.
 * @param segment
 *    A border segment containing the extension.
 * @throws BorderException
 *    If there is an error revising the border.
 */
void 
Border::reviseExtendFromEnd(SurfaceFile* surfaceFile,
                            const Border* segment) throw (BorderException)
{
    const int32_t numPoints = getNumberOfPoints();
    if (numPoints <= 0) {
        throw BorderException("Border being update contains no points");
    }
    
    const int numberOfSegmentPoints = segment->getNumberOfPoints();
    if (numberOfSegmentPoints <= 0) {
        throw BorderException("Border segment for erasing contains no points");
    }
    
    /*
     * Get coordinate of first point in new segment
     */
    float segmentStartXYZ[3];
    if (segment->getPoint(0)->getProjectedPosition(*surfaceFile, 
                                                   segmentStartXYZ, 
                                                   true) == false) {
        throw BorderException("First point in erase segment has invalid coordinate.  Redraw.");
    }
    
    /*
     * Find point in this border nearest start of new segment
     */
    const float distanceTolerance = 5.0;
    float distanceToStartOfNewSegment = 0.0;
    const int32_t borderPointNearestNewSegmentStart =
        findPointIndexNearestXYZ(surfaceFile, 
                                       segmentStartXYZ, 
                                       distanceTolerance, 
                                       distanceToStartOfNewSegment);
    if (borderPointNearestNewSegmentStart < 0) {
        throw BorderException("New segment does not start near an existing border");
    }

    /*
     * Get distance from both ends of existing border to first
     * point in new segment
     */
    float borderStartXYZ[3];
    getPoint(0)->getProjectedPosition(*surfaceFile, 
                                            borderStartXYZ, 
                                            true);
    const float distToStart = MathFunctions::distance3D(borderStartXYZ, 
                                                        segmentStartXYZ);
    
    float borderEndXYZ[3]; 
    getPoint(numPoints - 1)->getProjectedPosition(*surfaceFile, 
                                                        borderEndXYZ, 
                                                        true);
    const float distToEnd = MathFunctions::distance3D(borderEndXYZ, 
                                                      segmentStartXYZ);
    
    /*
     * Add on to start or ending end of border
     */
    int32_t startPointIndex = -1;
    int32_t endPointIndex   = -1;
    bool reverseOrderFlag = false;
    if (distToStart < distToEnd) {
        if (distToStart > distanceTolerance) {
            throw BorderException("New segment does not start near the end of a border.");
        }
        endPointIndex = borderPointNearestNewSegmentStart;
        reverseOrderFlag = true;
    }
    else {
        if (distToEnd > distanceTolerance) {
            throw BorderException("New segment does not start near the end of a border.");
        }
        startPointIndex = borderPointNearestNewSegmentStart;
    }
    /*
     * If needed, swap point indices
     *
    if (reverseOrderFlag) {
        std::swap(startPointIndex, endPointIndex);
    }
    */
    
    /*
     * Create a temporary border
     */
    Border tempBorder;
    
    /*
     * Add in points prior to updated points
     */
    if (startPointIndex >= 0) {
        tempBorder.addPoints(this,
                             0,
                             startPointIndex);
    }
    
    /*
     * Add new points
     */
    Border segmentCopy = *segment;
    if (reverseOrderFlag) {
        segmentCopy.reverse();
    }
    tempBorder.addPoints(&segmentCopy);
    
    /*
     * Add in points after updated points
     */
    if (endPointIndex >= 0) {
        tempBorder.addPoints(this,
                             (endPointIndex + 1));
    }
    
    replacePoints(&tempBorder);
}

/**
 * Revise a border by erasing from the end of a border.
 *
 * @param surfaceFile
 *    Surface on which border erasing is performed.
 * @param segment
 *    A border segment containing the erasing.
 * @throws BorderException
 *    If there is an error revising the border.
 */
void 
Border::reviseEraseFromEnd(SurfaceFile* surfaceFile,
                           const Border* segment) throw (BorderException)
{
    /*
     * Get coordinate of first and last points in the segment
     */
    const int numberOfSegmentPoints = segment->getNumberOfPoints();
    if (numberOfSegmentPoints <= 0) {
        throw BorderException("Border segment for erasing contains no points");
    }
    float segmentStartXYZ[3];
    if (segment->getPoint(0)->getProjectedPosition(*surfaceFile, 
                                                   segmentStartXYZ, 
                                                   true) == false) {
        throw BorderException("First point in erase segment has invalid coordinate.  Redraw.");
    }
    
    float segmentEndXYZ[3];
    if (segment->getPoint(numberOfSegmentPoints - 1)->getProjectedPosition(*surfaceFile, 
                                                                           segmentEndXYZ, 
                                                                           true) == false) {
        throw BorderException("End point in erase segment has invalid coordinate.  Redraw.");
    }

    const float tolerance = 10.0;
    
    /*
     * Find points in this border nearest the first and
     * last points in the erase segment
     */
    float distanceToStartPoint = 0.0;
    int32_t startPointIndex = findPointIndexNearestXYZ(surfaceFile, 
                                                            segmentStartXYZ, 
                                                            tolerance,
                                                            distanceToStartPoint);
    if (startPointIndex < 0) {
        throw BorderException("Start of segment drawn for erasing is not close enough to existing border");
    }
    float distanceToEndPoint = 0.0;
    int32_t endPointIndex   = findPointIndexNearestXYZ(surfaceFile, 
                                                            segmentEndXYZ, 
                                                            tolerance,
                                                            distanceToEndPoint);
    if (endPointIndex < 0) {
        throw BorderException("End of segment drawn for erasing is not close enough to existing border");
    }
    
    /*
     * If needed, swap point indices
     */
    const bool reverseOrderFlag = (startPointIndex > endPointIndex);
    if (reverseOrderFlag) {
        std::swap(startPointIndex, endPointIndex);
    }

    /*
     * Create a temporary border
     */
    Border tempBorder;
    
    /*
     * Add in points prior to updated points
     */
    if (startPointIndex >= 0) {
        tempBorder.addPoints(this,
                             0,
                             startPointIndex);
    }
    
    /*
     * Add in points after updated points
     */
    if (endPointIndex >= 0) {
        tempBorder.addPoints(this,
                             (endPointIndex + 1));
    }
    
    replacePoints(&tempBorder);
}

/**
 * Revise a border by replacing a segment in a border.
 *
 * @param surfaceFile
 *    Surface on which border segment replacement is performed.
 * @param segment
 *    A border containing the new segment.
 * @throws BorderException
 *    If there is an error replacing the segment in the border.
 */
void
Border::reviseReplaceSegment(SurfaceFile* surfaceFile,
                             const Border* segment) throw (BorderException)
{
    /*
     * Get coordinate of first and last points in the segment
     */
    const int numberOfSegmentPoints = segment->getNumberOfPoints();
    if (numberOfSegmentPoints < 2) {
        throw BorderException("Border segment for replacing contains less than 2 points");
    }
    float segmentStartXYZ[3];
    if (segment->getPoint(0)->getProjectedPosition(*surfaceFile,
                                                   segmentStartXYZ,
                                                   true) == false) {
        throw BorderException("First point in replace segment has invalid coordinate.  Redraw.");
    }
    
    float segmentEndXYZ[3];
    if (segment->getPoint(numberOfSegmentPoints - 1)->getProjectedPosition(*surfaceFile,
                                                                           segmentEndXYZ,
                                                                           true) == false) {
        throw BorderException("End point in replace segment has invalid coordinate.  Redraw.");
    }
    
    const float tolerance = 10.0;
    
    /*
     * Locate points in this border that are nearest the start
     * and end points in the new border segment.
     */
    float distanceToLowestPointIndex = 0.0;
    int32_t lowestPointIndex = findPointIndexNearestXYZ(surfaceFile,
                                                       segmentStartXYZ,
                                                       tolerance,
                                                       distanceToLowestPointIndex);
    if (lowestPointIndex < 0) {
        throw BorderException("Start of segment drawn for replacing is not close enough to existing border");
    }
    float distanceToHighestPointIndex = 0.0;
    int32_t highestPointIndex   = findPointIndexNearestXYZ(surfaceFile,
                                                       segmentEndXYZ,
                                                       tolerance,
                                                       distanceToHighestPointIndex);
    if (highestPointIndex < 0) {
        throw BorderException("End of segment drawn for replacing is not close enough to existing border");
    }
    
    /*
     * Swap lowest and highest point indexes so that lowest < highest
     */
    if (lowestPointIndex > highestPointIndex) {
        std::swap(lowestPointIndex, highestPointIndex);
    }
    
    /*
     * Length in this border from lowest to highest point index
     */
    const float lowestToHighestLength = getSegmentLength(surfaceFile,
                                                    lowestPointIndex,
                                                    highestPointIndex);
    /*
     * Length in this border from highest to lowest point index (assumes border
     * is closed.
     */
    const float highestToLowestLength = getSegmentLength(surfaceFile,
                                                    highestPointIndex,
                                                    lowestPointIndex);
    
    /*
     * Create a temporary border
     */
    Border newBorder;
    Border newBorderSecondSegment;
    
    
    /*
     * Keep part of this border that between low and high
     * indices that is the LONGEST
     */
    if (lowestToHighestLength > highestToLowestLength) {
        /*
         * Keep part of this border from lowest index to highest index
         */
        const int32_t lowToHighCount = highestPointIndex - lowestPointIndex + 1;
        newBorder.addPoints(this,
                            lowestPointIndex,
                            lowToHighCount);
    }
    else {
        /*
         * Keep segment from highest index to end
         */
        const int32_t highToEndCount = getNumberOfPoints() - highestPointIndex;
        newBorderSecondSegment.addPoints(this,
                            highestPointIndex,
                            highToEndCount);
        
        /*
         * Keep segment from start to lowest point index
         */
        const int32_t startToLowCount = lowestPointIndex + 1;
        newBorder.addPoints(this,
                            0,
                            startToLowCount);
    }

    const int32_t newBorderNumberOfPoints = newBorder.getNumberOfPoints();
    if (newBorderNumberOfPoints > 0) {
        float newBorderLastXYZ[3];
        if (newBorder.getPoint(newBorderNumberOfPoints - 1)->getProjectedPosition(*surfaceFile,
                                                            newBorderLastXYZ,
                                                            true)) {
            /*
             * Examine new segment to find point that is closest to last
             * part of new border that is being created.
             */
            float segmentFirstPointXYZ[3];
            const bool validFirstPoint = segment->getPoint(0)->getProjectedPosition(*surfaceFile,
                                                                                    segmentFirstPointXYZ,
                                                                                    true);
            float segmentLastPointXYZ[3];
            const bool validLastPoint = segment->getPoint(numberOfSegmentPoints - 1)->getProjectedPosition(*surfaceFile,
                                                                                    segmentLastPointXYZ,
                                                                                    true);
            
            if (validFirstPoint
                && validLastPoint) {
                const float firstDistance = MathFunctions::distance3D(newBorderLastXYZ,
                                                                      segmentFirstPointXYZ);
                const float lastDistance = MathFunctions::distance3D(newBorderLastXYZ,
                                                                     segmentLastPointXYZ);
                
                if (firstDistance < lastDistance) {
                    /*
                     * Add new segment onto the end of the existing border piece
                     */
                    newBorder.addPoints(segment,
                                        0,
                                        numberOfSegmentPoints);
                }
                else {
                    /*
                     * New segment is probably opposite orientation 
                     * (clockwise/counter-clockwise) that border that is
                     * being edited.
                     */
                    Border reversedSegment(*segment);
                    reversedSegment.reverse();
                    
                    newBorder.addPoints(&reversedSegment,
                                        0,
                                        reversedSegment.getNumberOfPoints());
                }
                
                if (newBorderSecondSegment.getNumberOfPoints() > 0) {
                    newBorder.addPoints(&newBorderSecondSegment,
                                        0,
                                        newBorderSecondSegment.getNumberOfPoints());
                }
                
                /*
                 * Replace this border with the newly created border
                 */
                replacePoints(&newBorder);
            }
            else {
                throw BorderException("Border replacement failed: First or last point in new segment failed to project.");
            }
        }
        else {
            throw BorderException("Border replacement failed: Failed to project original border segment.");
        }
    }
    else {
        throw BorderException("Border replacement failed: No points were kept from original border.");
    }
}

///**
// * Revise a border by replacing a segment in a border.
// *
// * @param surfaceFile
// *    Surface on which border segment replacement is performed.
// * @param segment
// *    A border containing the new segment.
// * @throws BorderException
// *    If there is an error replacing the segment in the border.
// */
//void
//Border::reviseReplaceSegment(SurfaceFile* surfaceFile,
//                             const Border* segment) throw (BorderException)
//{
//    /*
//     * Get coordinate of first and last points in the segment
//     */
//    const int numberOfSegmentPoints = segment->getNumberOfPoints();
//    if (numberOfSegmentPoints <= 0) {
//        throw BorderException("Border segment for erasing contains no points");
//    }
//    float segmentStartXYZ[3];
//    if (segment->getPoint(0)->getProjectedPosition(*surfaceFile,
//                                                   segmentStartXYZ,
//                                                   true) == false) {
//        throw BorderException("First point in erase segment has invalid coordinate.  Redraw.");
//    }
//    
//    float segmentEndXYZ[3];
//    if (segment->getPoint(numberOfSegmentPoints - 1)->getProjectedPosition(*surfaceFile,
//                                                                           segmentEndXYZ,
//                                                                           true) == false) {
//        throw BorderException("End point in erase segment has invalid coordinate.  Redraw.");
//    }
//    
//    const float tolerance = 10.0;
//    
//    /*
//     * Find points in this border nearest the first and
//     * last points in the erase segment
//     */
//    float distanceToStartPoint = 0.0;
//    int32_t segmentStartPointIndex = findPointIndexNearestXYZ(surfaceFile,
//                                                              segmentStartXYZ,
//                                                              tolerance,
//                                                              distanceToStartPoint);
//    if (segmentStartPointIndex < 0) {
//        throw BorderException("Start of segment drawn for erasing is not close enough to existing border");
//    }
//    float distanceToEndPoint = 0.0;
//    int32_t segmentEndPointIndex   = findPointIndexNearestXYZ(surfaceFile,
//                                                              segmentEndXYZ,
//                                                              tolerance,
//                                                              distanceToEndPoint);
//    if (segmentEndPointIndex < 0) {
//        throw BorderException("End of segment drawn for erasing is not close enough to existing border");
//    }
//    
//    /*
//     * Make copy of segment
//     */
//    Border replacementSegment(*segment);
//    
//    /*
//     * If needed, swap point indices and reverse order in segment
//     */
//    const bool reverseOrderFlag = (segmentStartPointIndex > segmentEndPointIndex);
//    if (reverseOrderFlag) {
//        std::swap(segmentStartPointIndex, segmentEndPointIndex);
//    }
//    
//    /*
//     * Determine which segment in border to replace by using the
//     * segment with the minimal length.
//     */
//    const float startToEndLength = getSegmentLength(surfaceFile,
//                                                    segmentStartPointIndex,
//                                                    segmentEndPointIndex);
//    const float endToStartLength = getSegmentLength(surfaceFile,
//                                                    segmentEndPointIndex,
//                                                    segmentStartPointIndex);
//    
//    /*
//     * Create a temporary border
//     */
//    Border tempBorder;
//    
//    if (startToEndLength < endToStartLength) {
//        /*
//         * Add in points from start of border
//         */
//        if (segmentStartPointIndex >= 0) {
//            tempBorder.addPoints(this,
//                                 0,
//                                 segmentStartPointIndex);
//        }
//        
//        /*
//         * Add new points
//         */
//        if (reverseOrderFlag) {
//            replacementSegment.reverse();
//        }
//        tempBorder.addPoints(&replacementSegment);
//        
//        /*
//         * Add in points from end of border
//         */
//        if (segmentEndPointIndex >= 0) {
//            tempBorder.addPoints(this,
//                                 (segmentEndPointIndex + 1));
//        }
//    }
//    else {
//        /*
//         * Add new points
//         */
//        tempBorder.addPoints(&replacementSegment);
//        
//        /*
//         * Add points from start to end
//         */
//        const int32_t numSegmentPoints = (segmentEndPointIndex
//                                          - segmentStartPointIndex
//                                          + 1);
//        tempBorder.addPoints(this,
//                             segmentStartPointIndex,
//                             numSegmentPoints);
//    }
//    
//    replacePoints(&tempBorder);
//}

/**
 * Get the length of the border segment formed by the all of the points
 * using the starting and ending point indices (inclusively).
 *
 * When (segmentStartPointIndex < endPointIndex), the segment length is that 
 * formed by the points from startPointIndex to endPointIndex.
 *
 * When (segmentStartPointIndex > endPointIndex), the segment length is that 
 * formed by startPointIndex to the last point, last point to first point
 * and first point to endPointIndex. (Assumes border is a circular border).
 *
 * @param segmentStartPointIndex
 *    Index of first border point in the border segement.
 * @param segmentEndPointIndex
 *    Index of last border point in the border segment.
 * @param surfaceFile
 *    Surface on which straightline distance between border points
 *    is calculated.
 */
float
Border::getSegmentLength(SurfaceFile* surfaceFile,
                         const int32_t segmentStartPointIndex,
                         const int32_t segmentEndPointIndex)
{
    CaretAssert(surfaceFile);
    
    const int32_t numPoints = getNumberOfPoints();
    if (numPoints <= 1) {
        return 0.0;
    }
    const int32_t lastPointIndex = numPoints - 1;
    
    CaretAssert((segmentStartPointIndex >= 0)
                && (segmentStartPointIndex < numPoints));
    CaretAssert((segmentEndPointIndex >= 0)
                && (segmentEndPointIndex < numPoints));
    
    float segmentLength = 0.0;
    
    if (segmentStartPointIndex > segmentEndPointIndex) {
        CaretAssert(segmentStartPointIndex <= lastPointIndex);
        const float d1 = getSegmentLength(surfaceFile,
                                          segmentStartPointIndex,
                                          lastPointIndex);
        
        float d2 = 0.0;
        {
            float xyz[3], xyzNext[3];
            if (m_points[lastPointIndex]->getProjectedPosition(*surfaceFile,
                                                               xyz,
                                                               true)
                && m_points[0]->getProjectedPosition(*surfaceFile,
                                                     xyzNext,
                                                     true)) {
                    d2 = MathFunctions::distance3D(xyz,
                                                   xyzNext);
            }
        }
        
        CaretAssert(0 <= segmentEndPointIndex);
        const float d3 = getSegmentLength(surfaceFile,
                                          0,
                                          segmentEndPointIndex);
        segmentLength = (d1 + d2 + d3);
    }
    else {
        for (int32_t i = segmentStartPointIndex; i < segmentEndPointIndex; i++) {
            float xyz[3], xyzNext[3];
            const int32_t iNext = i + 1;
            CaretAssert(iNext < numPoints);
            if (m_points[i]->getProjectedPosition(*surfaceFile,
                                                  xyz,
                                                  true)
                && m_points[iNext]->getProjectedPosition(*surfaceFile,
                                                       xyzNext,
                                                       true)) {
                segmentLength += MathFunctions::distance3D(xyz,
                                                           xyzNext);
            }
        }
    }
    
    return segmentLength;
}


/**
 * Replace the points in this border with
 * the given border.
 * @param border Border whose points are copied
 *    into this border.
 */
void 
Border::replacePoints(const Border* border)
{
    removeAllPoints();
    
    const int32_t numPoints = border->getNumberOfPoints();
    for (int i = 0; i < numPoints; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*border->getPoint(i));
        addPoint(spi);
    } 
}

/**
 * Set modification status of name/class to modified.
 * 
 * Name/Class modification status is used
 * by the selection controls that display
 * borders based upon selected classes and
 * names.
 */
void 
Border::setNameOrClassModified()
{
    m_groupNameSelectionItem = NULL;
    m_nameRgbaColorValid = false;
    m_classRgbaColorValid = false;
}

/**
 * Set the selection item for the group/name hierarchy.
 * 
 * @param item
 *     The selection item from the group/name hierarchy.
 */
void
Border::setGroupNameSelectionItem(GroupAndNameHierarchyItem* item)
{
    m_groupNameSelectionItem = item;
}

/**
 * @return The selection item for the Group/Name selection hierarchy.
 *      May be NULL in some circumstances.
 */
const GroupAndNameHierarchyItem*
Border::getGroupNameSelectionItem() const
{
    return m_groupNameSelectionItem;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Border::toString() const
{
    return "Border " + m_name;
}

/**
 * Write the border to the XML Writer.
 * @param xmlWriter
 *   Writer for XML output.
 */
void 
Border::writeAsXML(XmlWriter& xmlWriter) throw (XmlException)
{
    xmlWriter.writeStartElement(XML_TAG_BORDER);
    
    xmlWriter.writeElementCharacters(XML_TAG_NAME, m_name);
    
    if (m_className.isEmpty() == false) {
        xmlWriter.writeElementCharacters(XML_TAG_CLASS_NAME, m_className);
    }
    const int32_t numPoints = getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        m_points[i]->writeAsXML(xmlWriter);    
    }
    
    xmlWriter.writeEndElement();
}

