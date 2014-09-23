#ifndef __CIFTI_PARCEL_REORDERING_MODEL_H__
#define __CIFTI_PARCEL_REORDERING_MODEL_H__

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
    class SceneClassAssistant;

    class CiftiParcelReorderingModel : public CaretObject, public SceneableInterface {
        
    public:
        CiftiParcelReorderingModel();
        
        virtual ~CiftiParcelReorderingModel();
        
        CiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj);

        CiftiParcelReorderingModel& operator=(const CiftiParcelReorderingModel& obj);
        
        /**
         * Get the selected parcel label file used for reordering of parcels.
         *
         * @param selectedParcelLabelFileOut
         *    The selected parcel label file used for reordering the parcels.
         *    May be NULL!
         * @param selectedParcelLabelFileMapIndexOut
         *    Map index in the selected parcel label file.
         * @param enabledStatusOut
         *    Enabled status of reordering.
         */
        void getSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                           int32_t& selectedParcelLabelFileMapIndexOut,
                                                           bool& enabledStatusOut) const;
        
        /**
         * Set the selected parcel label file used for reordering of parcels.
         *
         * @param selectedParcelLabelFile
         *    The selected parcel label file used for reordering the parcels.
         *    May be NULL!
         * @param selectedParcelLabelFileMapIndex
         *    Map index in the selected parcel label file.
         * @param enabledStatus
         *    Enabled status of reordering.
         */
        void setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                              int32_t& selectedParcelLabelFileMapIndex,
                                                              bool& enabledStatus);
        
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
        void copyHelperCiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj);

        void validateSelectedParcelLabelFileAndMap() const;
        
        std::vector<CiftiParcelLabelFile*> getParcelLabelFiles() const;
        
        SceneClassAssistant* m_sceneAssistant;

        mutable CiftiParcelLabelFile* m_selectedParcelLabelFile;
        
        mutable int32_t m_selectedParcelLabelFileMapIndex;
        
        mutable bool m_parcelReorderingEnabledStatus;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_PARCEL_REORDERING_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_PARCEL_REORDERING_MODEL_DECLARE__

} // namespace
#endif  //__CIFTI_PARCEL_REORDERING_MODEL_H__
