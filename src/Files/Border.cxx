
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
#include "MathFunctions.h"
#include "SurfaceProjectedItem.h"
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
    this->color = CaretColorEnum::BLACK;
    this->displayFlag = true;
}

/**
 * Destructor.
 */
Border::~Border()
{
    this->clear();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Border::Border(const Border& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperBorder(obj);
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
        this->copyHelperBorder(obj);
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
    this->clear();
    
    const int32_t numPoints = obj.getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*obj.points[i]);
        this->addPoint(spi);
    }
    
    this->name = obj.name;
    this->className = obj.className;
    this->displayFlag = obj.displayFlag;
    
    this->clearModified();
}

/**
 * Clear the border.
 * Removes all points, resets names, etc.
 */
void 
Border::clear()
{
    this->removeAllPoints();
    
    this->name = "";
    this->className = "";
    this->color = CaretColorEnum::BLACK;
    this->displayFlag = true;
}

/**
 * Remove all points in this border.
 */
void 
Border::removeAllPoints()
{
    const int32_t numPoints = this->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        delete this->points[i];
    }
    this->points.clear();
    
    this->setModified();
}

/**
 * @return the name of the border.
 */
AString 
Border::getName() const
{
    return this->name;
}

/**
 * Set the name of the border.
 * @param name
 *   New name for border.
 */
void 
Border::setName(const AString& name)
{
    this->name = name;
    this->setModified();
}

/**
 * @return the class name of the border.
 */
AString 
Border::getClassName() const
{
    return this->className;
}

/**
 * Set the class name of the border.
 * @param className
 *   New class name for border.
 */
void 
Border::setClassName(const AString& className)
{
    this->className = className;
    this->setModified();
}

/**
 * @return Color of the border.
 */
CaretColorEnum::Enum 
Border::getColor() const
{
    return this->color;
}

/**
 * Set the color of the border.
 * @param color
 *    New color for border.
 */
void 
Border::setColor(const CaretColorEnum::Enum color)
{
    this->color = color;
}

/**
 * @return Number of points in the border.
 */
int32_t
Border::getNumberOfPoints() const
{
    return this->points.size();
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
    CaretAssertVectorIndex(this->points, indx);
    return this->points[indx];
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
    CaretAssertVectorIndex(this->points, indx);
    return this->points[indx];
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
    const int32_t numPoints = this->getNumberOfPoints();
    if (numPoints <= 0) {
        return -1;
    }

    int32_t nearestIndex = -1;
    float nearestDistanceSQ = maximumDistance * maximumDistance;
    
    float pointXYZ[3];
    for (int32_t i = 0; i < numPoints; i++) {
        if (this->points[i]->getProjectedPosition(*surfaceFile, 
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
    this->points.push_back(point);
    this->setModified();
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
        this->addPoint(spi);
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
    CaretAssertVectorIndex(this->points, indx);
    delete this->points[indx];
    this->points.erase(this->points.begin() + indx);
    this->setModified();
}

/**
 * Remove the last point from the border.
 */
void 
Border::removeLastPoint()
{
    const int numPoints = this->getNumberOfPoints();
    if (numPoints > 0) {
        this->removePoint(numPoints - 1);
    }
}

/**
 * Reverse the order of points in a border.
 */
void 
Border::reverse()
{
    std::reverse(this->points.begin(),
                 this->points.end());
    this->setModified();
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
    const int32_t numPoints = this->getNumberOfPoints();
    if (numPoints <= 0) {
        throw BorderException("Border being update contains no points");
    }
    
    const int numberOfSegmentPoints = segment->getNumberOfPoints();
    if (numberOfSegmentPoints <= 0) {
        throw new BorderException("Border segment for erasing contains no points");
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
        this->findPointIndexNearestXYZ(surfaceFile, 
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
    this->getPoint(0)->getProjectedPosition(*surfaceFile, 
                                            borderStartXYZ, 
                                            true);
    const float distToStart = MathFunctions::distance3D(borderStartXYZ, 
                                                        segmentStartXYZ);
    
    float borderEndXYZ[3]; 
    this->getPoint(numPoints - 1)->getProjectedPosition(*surfaceFile, 
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
    
    this->replacePoints(&tempBorder);
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
        throw new BorderException("Border segment for erasing contains no points");
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

    const float tolerance = 5.0;
    
    /*
     * Find points in this border nearest the first and
     * last points in the erase segment
     */
    float distanceToStartPoint = 0.0;
    int32_t startPointIndex = this->findPointIndexNearestXYZ(surfaceFile, 
                                                            segmentStartXYZ, 
                                                            tolerance,
                                                            distanceToStartPoint);
    if (startPointIndex < 0) {
        throw BorderException("Start of segment drawn for erasing is not close enough to existing border");
    }
    float distanceToEndPoint = 0.0;
    int32_t endPointIndex   = this->findPointIndexNearestXYZ(surfaceFile, 
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
    
    this->replacePoints(&tempBorder);
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
    if (numberOfSegmentPoints <= 0) {
        throw new BorderException("Border segment for erasing contains no points");
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
    
    const float tolerance = 5.0;
    
    /*
     * Find points in this border nearest the first and
     * last points in the erase segment
     */
    float distanceToStartPoint = 0.0;
    int32_t startPointIndex = this->findPointIndexNearestXYZ(surfaceFile, 
                                                             segmentStartXYZ, 
                                                             tolerance,
                                                             distanceToStartPoint);
    if (startPointIndex < 0) {
        throw BorderException("Start of segment drawn for erasing is not close enough to existing border");
    }
    float distanceToEndPoint = 0.0;
    int32_t endPointIndex   = this->findPointIndexNearestXYZ(surfaceFile, 
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
    
    this->replacePoints(&tempBorder);
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
    this->removeAllPoints();
    
    const int32_t numPoints = border->getNumberOfPoints();
    for (int i = 0; i < numPoints; i++) {
        SurfaceProjectedItem* spi = new SurfaceProjectedItem(*border->getPoint(i));
        this->addPoint(spi);
    } 
}

/**
 * Is this border displayed?
 */
bool 
Border::isDisplayed() const
{
    return this->displayFlag;
}

/**
 * Set the displayed status of this border.
 * @param displayed
 *    New displayed status.
 */
void 
Border::setDisplayed(const bool displayed)
{
    this->displayFlag = displayed;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Border::toString() const
{
    return "Border " + this->name;
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
    
    xmlWriter.writeElementCharacters(XML_TAG_NAME, this->name);
    
    if (this->className.isEmpty() == false) {
        xmlWriter.writeElementCharacters(XML_TAG_CLASS_NAME, this->className);
    }
    xmlWriter.writeElementCharacters(XML_TAG_COLOR_NAME, CaretColorEnum::toName(this->color));
    
    const int32_t numPoints = this->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        this->points[i]->writeAsXML(xmlWriter);    
    }
    
    xmlWriter.writeEndElement();
}

