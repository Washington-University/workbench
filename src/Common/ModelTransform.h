#ifndef __MODEL_TRANSFORM_H__
#define __MODEL_TRANSFORM_H__

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

#include "CaretObject.h"

namespace caret {

    class Matrix4x4Interface;
    
    class ModelTransform : public CaretObject {
        
    public:
        static void getIdentityQuaternion(float quaternionSXYZ[4]);
        
        ModelTransform();
        
        virtual ~ModelTransform();
        
        ModelTransform(const ModelTransform&);
        
        ModelTransform& operator=(const ModelTransform&);
        
        AString getName() const;
        
        void getTranslation(float translation[3]) const;
        
        void getRotation(float rotation[4][4]) const;
        
        void getObliqueRotation(float obliqueRotation[4][4]) const;
        
        void getMprTwoRotationAngles(float mprRotationAngles[3]) const;
        
        void getMprThreeRotationAngles(float mprRotationAngles[3]) const;
        
        float getMprThreeSingleSliceRotationAngle() const;
        
        void getFlatRotation(float flatRotation[4][4]) const;
        
        void getRightCortexFlatMapOffset(float& rightCortexFlatMapOffsetX,
                                         float& rightCortexFlatMapOffsetY) const;
        
        float getRightCortexFlatMapZoomFactor() const;
        
        float getScaling() const;
        
        void setName(const AString& name);
        
        AString getComment() const;
        
        void setComment(const AString& comment);
        
        void setTranslation(const float translation[3]);
        
        void setTranslation(const float translationX,
                            const float translationY,
                            const float translationZ);
        
        void setRotation(const float rotation[4][4]);
        
        void setObliqueRotation(const float obliqueRotation[4][4]);
        
        void setMprTwoRotationAngles(const float mprRotationAngles[3]);
        
        void setMprThreeRotationAngles(const float mprRotationAngles[3]);
        
        void setMprThreeSingleSliceRotationAngle(const float angle);
        
        void setFlatRotation(const float flatRotation[4][4]);
        
        void setRightCortexFlatMapOffset(const float rightCortexFlatMapOffsetX,
                                         const float rightCortexFlatMapOffsetY);
        
        void setRightCortexFlatMapZoomFactor(const float rightCortexFlatMapZoomFactor);
        
        void setScaling(const float scaling);
        
        void setPanningRotationMatrixAndZoom(const float panX,
                                             const float panY,
                                             const float panZ,
                                             const float rotationMatrix[4][4],
                                             const float obliqueRotationMatrix[4][4],
                                             const float mprTwoRotationAngles[3],
                                             const float mprThreeRotationAngles[3],
                                             const float mprThreeSingleSliceRotationAngle,
                                             const float flatRotationMatrix[4][4],
                                             const float zoom,
                                             const float rightCortexFlatMapOffsetX,
                                             const float rightCortexFlatMapOffsetY,
                                             const float rightCortexFlatMapZoomFactor);
        
        void getPanningRotationMatrixAndZoom(float& panX,
                                             float& panY,
                                             float& panZ,
                                             float rotationMatrix[4][4],
                                             float obliqueRotationMatrix[4][4],
                                             float mprTwoRotationAngles[3],
                                             float mprThreeRotationAngles[3],
                                             float& mprThreeSingleSliceRotationAngle,
                                             float flatRotationMatrix[4][4],
                                             float& zoom,
                                             float& rightCortexFlatMapOffsetX,
                                             float& rightCortexFlatMapOffsetY,
                                             float& rightCortexFlatMapZoomFactor) const;
        
        AString getAsString() const;
        
        AString getAsPrettyString(Matrix4x4Interface& matrixForCalculations,
                                  const float leftRotationMatrix[4][4]) const;
        
        bool setFromString(const AString& s);
        
        void setToIdentity();
        
        bool operator<(const ModelTransform& view) const;
        
        bool operator==(const ModelTransform& view) const;
        
    public:
        virtual AString toString() const;
        
    private:
        void copyHelper(const ModelTransform& ModelTransform);
        
        AString name;
        
        AString comment;
        
        float translation[3];
        
        float rotation[4][4];
        
        float obliqueRotation[4][4];
        
        float mprTwoRotationAngles[3];
        
        float mprThreeRotationAngles[3];
        
        float mprThreeSingleSliceRotationAngle;
        
        float flatRotation[4][4];
        
        float scaling;
        
        float rightCortexFlatMapOffsetXY[2];
        
        float rightCortexFlatMapZoomFactor;
        
        static const QString s_separatorInPreferences;
    };
    
#ifdef __MODEL_TRANSFORM_DECLARE__
    const QString ModelTransform::s_separatorInPreferences = "::::";
#endif // __MODEL_TRANSFORM_DECLARE__

} // namespace
#endif  //__MODEL_TRANSFORM_H__
