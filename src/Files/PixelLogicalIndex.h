#ifndef __PIXEL_LOGICAL_INDEX_H__
#define __PIXEL_LOGICAL_INDEX_H__

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
#include "VoxelIJK.h"

#include "SceneableInterface.h"

namespace caret {
    class PixelCoordinate;
    class SceneClassAssistant;

    class PixelLogicalIndex : public CaretObject, public SceneableInterface {
        
    public:
        PixelLogicalIndex();
        
        PixelLogicalIndex(const float i,
                   const float j,
                   const float k = 0);
        
        PixelLogicalIndex(const float ijk[3]);
        
        PixelLogicalIndex(const int64_t i,
                   const int64_t j,
                   const int64_t k = 0);
        
        PixelLogicalIndex(const int32_t i,
                   const int32_t j,
                   const int32_t k = 0);
        
        virtual ~PixelLogicalIndex();
        
        PixelLogicalIndex(const PixelLogicalIndex& obj);

        PixelLogicalIndex& operator=(const PixelLogicalIndex& obj);
        
        bool operator==(const PixelLogicalIndex& obj) const;
        
        /** @return The "i" component */
        inline int64_t getI() const { return m_ijk[0]; }
        
        /** @return The "j" component */
        inline int64_t getJ() const { return m_ijk[1]; }
        
        /** @return The "k" component */
        inline int64_t getK() const { return m_ijk[2]; }
        
        /** @return The "ijk" components */
        inline const std::array<int64_t, 3> getIJK() const { return m_ijk; }

        /** @return Converted to VoxelIJK */
        inline VoxelIJK getVoxelIJK() const { return VoxelIJK(m_ijk.data()); }

        /**
         * Set the "i" component
         * @param i
         *    The "i" component
         */
        inline void setI(const int64_t i) { m_ijk[0] = i; }
        
        /**
         * Set the "j" component
         * @param j
         *    The "j" component
         */
        inline void setJ(const int64_t j) { m_ijk[1] = j; }
        
        /**
         * Set the "k" component
         * @param k
         *    The "k" component
         */
        inline void setK(const int64_t k) { m_ijk[2] = k; }
        
        /**
         * Set the "ijk" components
         * @param ijk
         *    The "ijk" components
         */
        inline void setIJK(const std::array<int64_t, 3>& ijk) {
            m_ijk = ijk;
        }
        
        /**
         * Set the "ijk" components from floats
         * @param ijk
         *    The "ijk" components
         */
        inline void setIJK(const std::array<float, 3>& ijk) {
            m_ijk[0] = static_cast<int64_t>(ijk[0]);
            m_ijk[1] = static_cast<int64_t>(ijk[1]);
            m_ijk[2] = static_cast<int64_t>(ijk[2]);
        }
        
        /**
         * Set the "ijk" components
         * @param i
         *    The "i" component
         * @param j
         *    The "j" component
         * @param i
         *    The "k" component
         */
        inline void setIJK(const int64_t i,
                           const int64_t j,
                           const int64_t k) {
            m_ijk[0] = i;
            m_ijk[1] = j;
            m_ijk[2] = k;
        }
        
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
        void copyHelperPixelLogicalIndex(const PixelLogicalIndex& obj);

        void initializeMembers();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::array<int64_t, 3> m_ijk;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PIXEL_LOGICAL_INDEX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PIXEL_LOGICAL_INDEX_DECLARE__

} // namespace
#endif  //__PIXEL_LOGICAL_INDEX_H__
