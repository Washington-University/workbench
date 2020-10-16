#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__

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


#include <CaretObject.h>

#include <AString.h>

#include <stdint.h>

namespace caret {

/**
 * A bounding box - minimum and maximum X, Y, and Z values for something.
 */
class BoundingBox : public CaretObject {

public:
    BoundingBox();

    BoundingBox(const float minMaxXYZ[]);

    BoundingBox(const BoundingBox& bb);
    
    BoundingBox& operator=(const BoundingBox& co);
    
    virtual ~BoundingBox();

public:
    bool isValid2D() const;
    
    void resetZeros();

    void resetWithMaximumExtent();

    void resetForUpdate();

    void set(const float* points3D, const int64_t numPoints);

    void set(const float minX,
                    const float maxX,
                    const float minY,
                    const float maxY,
                    const float minZ,
                    const float maxZ);

    void set(const float minMaxXYZ[6]);

    void set(const float a[3],
             const float b[3],
             const float c[3],
             const float d[4]);
    
    void update(const float xyz[]);

    void update(const float x,
                const float y,
                const float z);
    
    void updateExcludeNanInf(const float xyz[]);
    
    void updateExcludeNanInf(const float x,
                             const float y,
                             const float z);

    const float* getBounds() const;

    void getBounds(float bounds[6]) const;
    
    float getDifferenceX() const;

    float getDifferenceY() const;

    float getDifferenceZ() const;

    float getMinX() const;

    float getMaxX() const;

    float getMinY() const;

    float getMaxY() const;

    float getMinZ() const;

    float getMaxZ() const;

    float* getMinXYZ() const;

    float* getMaxXYZ() const;

    void setMinX(const float value);

    void setMaxX(const float value);

    void setMinY(const float value);

    void setMaxY(const float value);

    void setMinZ(const float value);

    void setMaxZ(const float value);

    float getCenterX() const;
    
    float getCenterY() const;
    
    float getCenterZ() const;
    
    void getCenter(float centerOut[3]) const;
    
    bool isCoordinateWithinBoundingBox(const float xyz[]) const;

    void limitCoordinateToBoundingBox(float xyz[3]) const;
    
    void limitCoordinateToBoundingBox(double xyz[3]) const;
    
    bool intersectsXY(const BoundingBox& bb) const;
    
    AString toString() const;

private:
    void initializeMembersBoundingBox();

    void copyHelper(const BoundingBox& bo);
    
private:
    float boundingBox[6];

};

} // namespace

#endif // __BOUNDINGBOX_H__
