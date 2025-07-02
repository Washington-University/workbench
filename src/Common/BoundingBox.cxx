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

#include <limits>
#include <sstream>
#include <QtCore>
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;

/**
 * Constructor setup for update of an update method.
 *
 */
BoundingBox::BoundingBox()
: CaretObject()
{
    this->initializeMembersBoundingBox();
    this->resetForUpdate();
}

/**
 * Create a bounding box from a six-dimensional array containing
 * min-X, max-X, min-Y, max-Y, min-Z, max-Z.
 * 
 * @param minMaxXYZ - array described above.
 * @throws IllegalArgumentException if the array does not contain six
 *    elements or a min value is greater than a max value.
 *
 */
BoundingBox::BoundingBox(
                   const float minMaxXYZ[])
: CaretObject()
{
    this->initializeMembersBoundingBox();
    for (int i = 0; i < 6; i++) {
        this->boundingBox[i] = minMaxXYZ[i];
    }
}

/**
 * Copy constructor.
 * @param bb 
 *     BoundingBox that is copied.
 */
BoundingBox::BoundingBox(const BoundingBox& bb)
: CaretObject(bb)
{
    this->initializeMembersBoundingBox();
    this->copyHelper(bb);
}

/**
 * Assignment operator.
 * @param bb
 *     BoundingBox that replace this bounding box.
 */
BoundingBox& 
BoundingBox::operator=(const BoundingBox& bb)
{
    if (this != &bb) {
        CaretObject::operator=(bb);
        this->copyHelper(bb);
    }
    return *this;
}

/**
 * Destructor
 */
BoundingBox::~BoundingBox()
{
}

/**
 * Initialize data members.
 */
void
BoundingBox::initializeMembersBoundingBox()
{
}

/**
 * Helps with copy constructor and assignemnt operator.
 */
void 
BoundingBox::copyHelper(const BoundingBox& bo)
{
    for (int i = 0; i < 6; i++) {
        this->boundingBox[i] = bo.boundingBox[i];
    }
}

/**
 * @return True if the bounding box is valid for 3D (minX < maxX and minY < maxY and minZ < maxZ)
 */
bool
BoundingBox::isValid() const
{
    if ((getMinX() < getMaxX())
        && (getMinY() < getMaxY())
        && (getMinZ() < getMaxZ())) {
        return true;
    }
    return false;
}

/**
 * @return True if the bounding box is valid for 2D (minX < maxX and minY < maxY)
 */
bool
BoundingBox::isValid2D() const
{
    if ((getMinX() < getMaxX())
        && (getMinY() < getMaxY())) {
        return true;
    }
    return false;
}

/**
 * @return True if the bounding box is valid for at least two axes
 */
bool
BoundingBox::isValidTwoAxis() const
{
    int32_t validCount(0);
    if (getMinX() < getMaxX()) ++validCount;
    if (getMinY() < getMaxY()) ++validCount;
    if (getMinZ() < getMaxZ()) ++validCount;
    return (validCount >= 2);
}

/**
 * Reset a new bounding box with the minimum and maximum values
 * all set to zero.
 */
void
BoundingBox::resetZeros()
{
    this->setPrivate(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

/**
 * Reset a bounding box with the minimum values initialized to
 * the minimum float value and the maximum values initialized to the
 * maximum float value.  Use one of the setMax() or setMinY() methods
 * to update this bounding box.
 */
void
BoundingBox::resetWithMaximumExtent()
{
    const float f = std::numeric_limits<float>::max();
    this->setPrivate(-f, f, -f, f, -f, f);
}

/**
 * Reset a  bounding box with the minimum values initialized to
 * the maximum float value and the maximum values initialized to the
 * minimum float value.  Use the update(float[]) method to update
 * the bounding box.
 */
void
BoundingBox::resetForUpdate()
{
    const float f = std::numeric_limits<float>::max();
    this->setPrivate(f, -f, f, -f, f, -f);
}

/**
 * Set bounding box using the array of points.
 * @param points3D
 *     Array of three dimensional points.
 * @param numPoints
 *     Number of points.
 */
void
BoundingBox::set(const float* points3D, const int64_t numPoints)
{
    this->resetForUpdate();
    for (int64_t i = 0; i < numPoints; i++) {
        this->update(&points3D[i*3]);
    }
}

/**
 * Set a new bounding box.  If a "min" is greater than a "max", the values are swapped.
 * @param minX  Minimum X-coordinate for bounding box.
 * @param maxX  Maximum X-coordinate for bounding box.
 * @param minY  Minimum Y-coordinate for bounding box.
 * @param maxY  Maximum Y-coordinate for bounding box.
 * @param minZ  Minimum Z-coordinate for bounding box.
 * @param maxZ  Maximum Z-coordinate for bounding box.
 */
void
BoundingBox::set(const float minX,
                 const float maxX,
                 const float minY,
                 const float maxY,
                 const float minZ,
                 const float maxZ)
{
    this->boundingBox[0] = minX;
    this->boundingBox[1] = maxX;
    this->boundingBox[2] = minY;
    this->boundingBox[3] = maxY;
    this->boundingBox[4] = minZ;
    this->boundingBox[5] = maxZ;
    
    /*
     * Ensure "min" less than "max"
     */
    if (this->boundingBox[0] > this->boundingBox[1]) {
        std::swap(this->boundingBox[0], this->boundingBox[1]);
    }
    if (this->boundingBox[2] > this->boundingBox[3]) {
        std::swap(this->boundingBox[2], this->boundingBox[3]);
    }
    if (this->boundingBox[4] > this->boundingBox[5]) {
        std::swap(this->boundingBox[4], this->boundingBox[5]);
    }
}

/**
 * Set a bounding box from a six-dimensional array containing
 * min-X, max-X, min-Y, max-Y, min-Z, max-Z.
 *
 * @param minMaxXYZ - array described above.
 */
void
BoundingBox::set(const float minMaxXYZ[6])
{
    set(minMaxXYZ[0],
        minMaxXYZ[1],
        minMaxXYZ[2],
        minMaxXYZ[3],
        minMaxXYZ[4],
        minMaxXYZ[5]);
}

/**
 * Set the bound box to the extent found in the 4 given triplets
 * @param a
 * First triplet
 * @param b
 * Second triplet
 * @param c
 * Third triplet
 * @param d
 * Fourth triplet
 */
void
BoundingBox::set(const float a[3],
                 const float b[3],
                 const float c[3],
                 const float d[4])
{
    resetForUpdate();
    update(a);
    update(b);
    update(c);
    update(d);
}

/**
 * Set a new bounding box.
 * @param minX  Minimum X-coordinate for bounding box.
 * @param maxX  Maximum X-coordinate for bounding box.
 * @param minY  Minimum Y-coordinate for bounding box.
 * @param maxY  Maximum Y-coordinate for bounding box.
 * @param minZ  Minimum Z-coordinate for bounding box.
 * @param maxZ  Maximum Z-coordinate for bounding box.
 */
void
BoundingBox::setPrivate(const float minX,
                 const float maxX,
                 const float minY,
                 const float maxY,
                 const float minZ,
                 const float maxZ)
{
    this->boundingBox[0] = minX;
    this->boundingBox[1] = maxX;
    this->boundingBox[2] = minY;
    this->boundingBox[3] = maxY;
    this->boundingBox[4] = minZ;
    this->boundingBox[5] = maxZ;

}

/**
 * Update the bounding box with the XYZ value passed in.  The bound box
 * must have been created with newInstanceForUpdate() or properly
 * initialized by the user.
 *
 * @param xyz - Three dimensional array containing XYZ.
 *
 */
void
BoundingBox::update(const float xyz[3])
{
    if (xyz[0] < this->boundingBox[0]) this->boundingBox[0] = xyz[0];
    if (xyz[0] > this->boundingBox[1]) this->boundingBox[1] = xyz[0];
    if (xyz[1] < this->boundingBox[2]) this->boundingBox[2] = xyz[1];
    if (xyz[1] > this->boundingBox[3]) this->boundingBox[3] = xyz[1];
    if (xyz[2] < this->boundingBox[4]) this->boundingBox[4] = xyz[2];
    if (xyz[2] > this->boundingBox[5]) this->boundingBox[5] = xyz[2];
}
/**
 * Update the bounding box with the XYZ value passed in.  The bound box
 * must have been created with newInstanceForUpdate() or properly
 * initialized by the user.
 *
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 * @param Z
 *    Z-coordinate.
 *
 */
void
BoundingBox::update(const float x,
                    const float y,
                    const float z)
{
    if (x < this->boundingBox[0]) this->boundingBox[0] = x;
    if (x > this->boundingBox[1]) this->boundingBox[1] = x;
    if (y < this->boundingBox[2]) this->boundingBox[2] = y;
    if (y > this->boundingBox[3]) this->boundingBox[3] = y;
    if (z < this->boundingBox[4]) this->boundingBox[4] = z;
    if (z > this->boundingBox[5]) this->boundingBox[5] = z;
}

/**
 * Update the bounding box with the XYZ value passed in.  The bound box
 * must have been created with newInstanceForUpdate() or properly
 * initialized by the user.
 *
 * @param xyz - Three dimensional array containing XYZ.
 *
 */
void
BoundingBox::update(const std::array<float, 3>& xyz)
{
    update(xyz.data());
}

/**
 * Update the bounding box with the XYZ value passed in.  The bound box
 * must have been created with newInstanceForUpdate() or properly
 * initialized by the user.
 *
 * @param xyz - Three dimensional array containing XYZ.
 *
 */
void
BoundingBox::updateExcludeNanInf(const float xyz[3])
{
    if (MathFunctions::isNumeric(xyz[0])) {
        if (xyz[0] < this->boundingBox[0]) this->boundingBox[0] = xyz[0];
        if (xyz[0] > this->boundingBox[1]) this->boundingBox[1] = xyz[0];
    }
    if (MathFunctions::isNumeric(xyz[1])) {
        if (xyz[1] < this->boundingBox[2]) this->boundingBox[2] = xyz[1];
        if (xyz[1] > this->boundingBox[3]) this->boundingBox[3] = xyz[1];
    }
    if (MathFunctions::isNumeric(xyz[2])) {
        if (xyz[2] < this->boundingBox[4]) this->boundingBox[4] = xyz[2];
        if (xyz[2] > this->boundingBox[5]) this->boundingBox[5] = xyz[2];
    }
}

/**
 * Update the bounding box with the XYZ value passed in.  The bound box
 * must have been created with newInstanceForUpdate() or properly
 * initialized by the user.
 *
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 * @param Z
 *    Z-coordinate.
 *
 */
void
BoundingBox::updateExcludeNanInf(const float x,
                    const float y,
                    const float z)
{
    if (MathFunctions::isNumeric(x)) {
        if (x < this->boundingBox[0]) this->boundingBox[0] = x;
        if (x > this->boundingBox[1]) this->boundingBox[1] = x;
    }
    if (MathFunctions::isNumeric(y)) {
        if (y < this->boundingBox[2]) this->boundingBox[2] = y;
        if (y > this->boundingBox[3]) this->boundingBox[3] = y;
    }
    if (MathFunctions::isNumeric(z)) {
        if (z < this->boundingBox[4]) this->boundingBox[4] = z;
        if (z > this->boundingBox[5]) this->boundingBox[5] = z;
    }
}

/**
 * Get the bounds in an array.
 * @return  Array of six containing minX, maxX, minY, maxY, minZ, maxZ.
 *
 */
const float*
BoundingBox::getBounds() const
{
    return this->boundingBox;
}

/**
 * Get the bounds in an array.
 * @param Output array of six containing minX, maxX, minY, maxY, minZ, maxZ.
 *
 */
void 
BoundingBox::getBounds(float bounds[6]) const
{
    bounds[0] = this->boundingBox[0];
    bounds[1] = this->boundingBox[1];
    bounds[2] = this->boundingBox[2];
    bounds[3] = this->boundingBox[3];
    bounds[4] = this->boundingBox[4];
    bounds[5] = this->boundingBox[5];
}

/**
 * Get the X-Coordinate difference.
 * @return X-Coordinate difference.
 *
 */
float
BoundingBox::getDifferenceX() const
{
    return (this->boundingBox[1] - this->boundingBox[0]);
}

/**
 * Get the Y-Coordinate difference.
 * @return Y-Coordinate difference.
 *
 */
float
BoundingBox::getDifferenceY() const
{
    return (this->boundingBox[3] - this->boundingBox[2]);
}

/**
 * Get the Z-Coordinate difference.
 * @return Z-Coordinate difference.
 *
 */
float
BoundingBox::getDifferenceZ() const
{
    return (this->boundingBox[5] - this->boundingBox[4]);
}

/**
 * @return The maximum difference of the X, Y, Z differences.
 */
float
BoundingBox::getMaximumDifferenceOfXYZ() const
{
    const float maxDiff(std::max(getDifferenceX(),
                                 std::max(getDifferenceY(),
                                          getDifferenceZ())));
    return maxDiff;
}

/**
 * Get the X minimum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMinX() const
{
    return this->boundingBox[0];
}

/**
 * Get the X maximum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMaxX() const
{
    return this->boundingBox[1];
}

/**
 * Get the Y minimum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMinY() const
{
    return this->boundingBox[2];
}

/**
 * Get the Y maximum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMaxY() const
{
    return this->boundingBox[3];
}

/**
 * Get the Z minimum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMinZ() const
{
    return this->boundingBox[4];
}

/**
 * Get the Z maximum value.
 * @return Its value.
 *
 */
float
BoundingBox::getMaxZ() const
{
    return this->boundingBox[5];
}

/**
 * @return Minimum XYZ of the bounding box.
 */
std::array<float, 3>
BoundingBox::getMinXYZ() const
{
    std::array<float, 3> f;
    f[0] = this->boundingBox[0];
    f[1] = this->boundingBox[2];
    f[2] = this->boundingBox[4];
    return f;
}

/**
 * @return Maximum XYZ of the bounding box
 */
std::array<float, 3>
BoundingBox::getMaxXYZ() const
{
    std::array<float, 3> f;
    f[0] = this->boundingBox[1];
    f[1] = this->boundingBox[3];
    f[2] = this->boundingBox[5];
    return f;
}

/**
 * Set the minimum X value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMinX(const float value)
{
    this->boundingBox[0] = value;
}

/**
 * Set the maximum X value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMaxX(const float value)
{
    this->boundingBox[1] = value;
}

/**
 * Set the minimum Y value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMinY(const float value)
{
    this->boundingBox[2] = value;
}

/**
 * Set the maximum Y value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMaxY(const float value)
{
    this->boundingBox[3] = value;
}

/**
 * Set the minimum Z value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMinZ(const float value)
{
    this->boundingBox[4] = value;
}

/**
 * Set the maximum Z value.
 * @param value - new value.
 *
 */
void
BoundingBox::setMaxZ(const float value)
{
    this->boundingBox[5] = value;
}

/**
 * @return  X-Coordinate at center of the bounding box.
 */
float 
BoundingBox::getCenterX() const
{
    const float centerX = (this->boundingBox[0] + this->boundingBox[1]) * 0.5;
    return centerX;
}

/**
 * @return  Y-Coordinate at center of the bounding box.
 */
float 
BoundingBox::getCenterY() const
{
    const float centerY = (this->boundingBox[2] + this->boundingBox[3]) * 0.5;
    return centerY;
}

/**
 * @return  Z-Coordinate at center of the bounding box.
 */
float 
BoundingBox::getCenterZ() const
{
    const float centerZ = (this->boundingBox[4] + this->boundingBox[5]) * 0.5;
    return centerZ;
}

/**
 * Get the center of the bounding box.
 * @param centerOut
 *    Three dimensional array into which the center is loaded.
 */
void 
BoundingBox::getCenter(float centerOut[3]) const
{
    centerOut[0] = (this->boundingBox[0] + this->boundingBox[1]) * 0.5;
    centerOut[1] = (this->boundingBox[2] + this->boundingBox[3]) * 0.5;
    centerOut[2] = (this->boundingBox[4] + this->boundingBox[5]) * 0.5;
}

/**
 * Is the coordinate within the bounding box?
 * @param xyz - The coordinate.
 * @return  True if coordinate is within the bounding box, else false.
 *
 */
bool
BoundingBox::isCoordinateWithinBoundingBox(const float xyz[]) const
{
    if (xyz[0] < this->boundingBox[0]) return false;
    if (xyz[0] > this->boundingBox[1]) return false;
    if (xyz[1] < this->boundingBox[2]) return false;
    if (xyz[1] > this->boundingBox[3]) return false;
    if (xyz[2] < this->boundingBox[4]) return false;
    if (xyz[2] > this->boundingBox[5]) return false;
    return true;
}

/**
 * Limit the given coordinate to the bounding box.
 *
 * @param xyz
 *    The coordinate.
 */
void
BoundingBox::limitCoordinateToBoundingBox(float xyz[3]) const
{
    if (xyz[0] < this->boundingBox[0]) xyz[0] = this->boundingBox[0];
    if (xyz[0] > this->boundingBox[1]) xyz[0] = this->boundingBox[1];
    if (xyz[1] < this->boundingBox[2]) xyz[1] = this->boundingBox[2];
    if (xyz[1] > this->boundingBox[3]) xyz[1] = this->boundingBox[3];
    if (xyz[2] < this->boundingBox[4]) xyz[2] = this->boundingBox[4];
    if (xyz[2] > this->boundingBox[5]) xyz[2] = this->boundingBox[5];
}

/**
 * Limit the given coordinate to the bounding box.
 *
 * @param xyz
 *    The coordinate.
 */
void
BoundingBox::limitCoordinateToBoundingBox(double xyz[3]) const
{
    if (xyz[0] < this->boundingBox[0]) xyz[0] = this->boundingBox[0];
    if (xyz[0] > this->boundingBox[1]) xyz[0] = this->boundingBox[1];
    if (xyz[1] < this->boundingBox[2]) xyz[1] = this->boundingBox[2];
    if (xyz[1] > this->boundingBox[3]) xyz[1] = this->boundingBox[3];
    if (xyz[2] < this->boundingBox[4]) xyz[2] = this->boundingBox[4];
    if (xyz[2] > this->boundingBox[5]) xyz[2] = this->boundingBox[5];
}



/**
 * Get String representation of bounding box.
 * @return String containing bounding box.
 *
 */
AString
BoundingBox::toString() const
{
    std::stringstream str;
    str << "BoundingBox=["
    << "x=(" << this->boundingBox[0] << "," << this->boundingBox[1] << ")"
    << ", y=(" << this->boundingBox[2] << "," << this->boundingBox[3] << ")"
    << ", z=(" << this->boundingBox[4] << "," << this->boundingBox[5] << ")"
    << "]";
    AString s = AString::fromStdString(str.str());
    return s;
}

/**
 * @return True if this bounding box intersects the given bounding box
 * using only the X and Y coordinates.
 */
bool
BoundingBox::intersectsXY(const BoundingBox& bb) const
{
    /*
     * Does self overlap
     */
    if (this == &bb) {
        return true;
    }
    
    /*
     * Note: Since the geometry is aligned with the X- and Y-axes,
     * we only need to test for one to be above or the the right of the other
     *
     * https://www.geeksforgeeks.org/find-two-rectangles-overlap/
     * https://leetcode.com/articles/rectangle-overlap/
     */
    /* 'this' is on right side of 'other' */
    if (getMinX() >= bb.getMaxX()) {
        return false;
    }
    
    /* 'other' is on right side of 'this' */
    if (bb.getMinX() >= getMaxX()) {
        return false;
    }
    
    /* 'this' is above 'other */
    if (getMinY() >= bb.getMaxY()) {
        return false;
    }
    
    /* 'other' is above 'this' */
    if (bb.getMinY() >= getMaxY()) {
        return false;
    }
    
    return true;
}

/**
 * Add (union) the given bounding box
 * @param bb
 *    Bounding box to add
 */
void
BoundingBox::unionOperation(const BoundingBox& bb)
{
    this->update(bb.getMinXYZ());
    this->update(bb.getMaxXYZ());
}


