#ifndef __PIXEL_INDEX_H__
#define __PIXEL_INDEX_H__

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
#include <cstdint>
#include <memory>

#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class PixelCoordinate;
    class SceneClassAssistant;

    class PixelIndex : public CaretObject, public SceneableInterface {
        
    public:
        PixelIndex();
        
        PixelIndex(const int64_t i,
                   const int64_t j,
                   const int64_t k = 0);
        
        PixelIndex(const PixelCoordinate& coordinate);
        
        virtual ~PixelIndex();
        
        PixelIndex(const PixelIndex& obj);

        PixelIndex& operator=(const PixelIndex& obj);
        
        bool operator==(const PixelIndex& obj) const;
        
        /** @return The "i" component */
        inline int64_t getI() const { return m_ijk[0]; }
        
        /** @return The "j" component */
        inline int64_t getJ() const { return m_ijk[1]; }
        
        /** @return The "k" component */
        inline int64_t getK() const { return m_ijk[2]; }
        
        /** @return The "ijk" components */
        inline std::array<int64_t, 3> getIJK() const { return m_ijk; }

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
        inline void setIJK(const std::array<int64_t, 3>& ijk) { m_ijk = ijk; }
        
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
        void copyHelperPixelIndex(const PixelIndex& obj);

        void initializeMembers();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::array<int64_t, 3> m_ijk;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PIXEL_INDEX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PIXEL_INDEX_DECLARE__

} // namespace
#endif  //__PIXEL_INDEX_H__
