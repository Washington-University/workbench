#ifndef __PIXEL_COORDINATE_H__
#define __PIXEL_COORDINATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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


#include <array>
#include <memory>

#include "CaretObject.h"

#include "SceneableInterface.h"
#include "Vector3D.h"

namespace caret {
    class SceneClassAssistant;

    class PixelCoordinate : public CaretObject, public SceneableInterface {
        
    public:
        PixelCoordinate();
        
        virtual ~PixelCoordinate();
        
        PixelCoordinate(const PixelCoordinate& obj);

        PixelCoordinate(const std::array<float,3>& xyz);
        
        PixelCoordinate(const float x,
                        const float y,
                        const float z);
        
        PixelCoordinate& operator=(const PixelCoordinate& obj);
        
        bool operator==(const PixelCoordinate& obj) const;
        
        inline float getX() const { return m_pixelCoord[0]; }
        
        inline float getY() const { return m_pixelCoord[1]; }
        
        inline float getZ() const { return m_pixelCoord[2]; }
        
        //inline const float* getXYZ() const { return m_xyz.data(); };
        
        inline void setX(const float x) { m_pixelCoord[0] = x; }
        
        inline void setY(const float y) { m_pixelCoord[1] = y; }
        
        inline void setZ(const float z) { m_pixelCoord[2] = z; }

        inline const Vector3D& getRefToVector3D() const { return m_pixelCoord; }
        
        inline Vector3D& getRefToVector3D() { return m_pixelCoord; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void initializeMembers();
        
        void copyHelperPixelCoordinate(const PixelCoordinate& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Vector3D m_pixelCoord;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PIXEL_COORDINATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PIXEL_COORDINATE_DECLARE__

} // namespace
#endif  //__PIXEL_COORDINATE_H__
