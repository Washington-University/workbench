#ifndef __CONTROL_POINT3_D_H__
#define __CONTROL_POINT3_D_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "CaretObjectTracksModification.h"
#include "SceneableInterface.h"




namespace caret {
    class SceneClassAssistant;

    class ControlPoint3D : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ControlPoint3D(const float sourceXYZ[3],
                       const float targetXYZ[3]);
        
        ControlPoint3D(const float sourceX,
                       const float sourceY,
                       const float sourceZ,
                       const float targetX,
                       const float targetY,
                       const float targetZ);
        
        virtual ~ControlPoint3D();
        
        ControlPoint3D(const ControlPoint3D& obj);

        ControlPoint3D& operator=(const ControlPoint3D& obj);
        
        void getSourceXYZ(double pt[3]) const;
        
        void getTargetXYZ(double pt[3]) const;
        
        void getTransformedXYZ(double pt[3]) const;
        
        void setTransformedXYZ(const double pt[3]);
        
        void getSourceXYZ(float pt[3]) const;
        
        void getTargetXYZ(float pt[3]) const;

        void getTransformedXYZ(float pt[3]) const;
        
        void setTransformedXYZ(const float pt[3]);
        
        float getSourceX() const;
        
        float getSourceY() const;
        
        float getSourceZ() const;
        
        float getTargetX() const;
        
        float getTargetY() const;
        
        float getTargetZ() const;
        
        void getErrorMeasurements(float xyzTotalErrorOut[4]) const;
        
        virtual AString toString() const;
        
        static void getSourceNormalVector(const std::vector<ControlPoint3D>& controlPoints,
                                          float sourceNormalVectorOut[3]);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void copyHelperControlPoint3D(const ControlPoint3D& obj);

        void initializeInstance();
        
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        float m_sourceX;
        
        float m_sourceY;
        
        float m_sourceZ;
        
        float m_targetX;
        
        float m_targetY;
        
        float m_targetZ;
        
        float m_transformedX;
        
        float m_transformedY;
        
        float m_transformedZ;
        
    };
    
#ifdef __CONTROL_POINT3_D_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONTROL_POINT3_D_DECLARE__

} // namespace
#endif  //__CONTROL_POINT3_D_H__
