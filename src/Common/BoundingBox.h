#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__

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

    void update(const float xyz[]);

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

    AString toString() const;

private:
    void initializeMembersBoundingBox();

    void copyHelper(const BoundingBox& bo);
    
private:
    float boundingBox[6];

};

} // namespace

#endif // __BOUNDINGBOX_H__
