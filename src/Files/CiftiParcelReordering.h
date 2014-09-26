#ifndef __CIFTI_PARCEL_REORDERING_H__
#define __CIFTI_PARCEL_REORDERING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#include "SceneableInterface.h"


namespace caret {
    class CiftiParcelLabelFile;
    class CiftiParcelsMap;
    class SceneClassAssistant;

    class CiftiParcelReordering : public CaretObject, public SceneableInterface {
        
    public:
        CiftiParcelReordering();
        
        virtual ~CiftiParcelReordering();
        
        CiftiParcelReordering(const CiftiParcelReordering& obj);

        CiftiParcelReordering& operator=(const CiftiParcelReordering& obj);
        
        bool operator==(const CiftiParcelReordering& obj) const;
        
        bool isValid() const;
        
        bool isMatch(const CiftiParcelLabelFile* sourceParcelLabelFile,
                     const int32_t sourceParcelLabelFileMapIndex) const;
        
        bool createReordering(const CiftiParcelLabelFile* sourceParcelLabelFile,
                              const int32_t sourceParcelLabelFileMapIndex,
                              const CiftiParcelsMap& targetParcelsMap,
                              AString& errorMessageOut);
        
        std::vector<int32_t> getReorderedParcelIndices() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperCiftiParcelReordering(const CiftiParcelReordering& obj);

        SceneClassAssistant* m_sceneAssistant;

        /**
         * Parcel label file used to create the reordering
         */
        CiftiParcelLabelFile* m_sourceParcelLabelFile;
        
        /**
         * Index of map in parcel label file used to create reordering
         */
        int32_t m_sourceParcelLabelFileMapIndex;
        
        /**
         * Reordered parcel indices in data file that has its parcel's
         * reordered.
         */
        std::vector<int32_t> m_reorderedParcelIndices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_PARCEL_REORDERING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_PARCEL_REORDERING_DECLARE__

} // namespace
#endif  //__CIFTI_PARCEL_REORDERING_H__
