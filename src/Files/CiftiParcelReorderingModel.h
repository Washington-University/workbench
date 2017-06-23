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

#include <map>

#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class CiftiMappableDataFile;
    class CiftiParcelLabelFile;
    class CiftiParcelReordering;
    class CiftiParcelsMap;
    class SceneClassAssistant;

    class CiftiParcelReorderingModel : public CaretObject, public SceneableInterface {
        
    public:
        CiftiParcelReorderingModel(const CiftiMappableDataFile* ciftiMappableDataFile);
        
        virtual ~CiftiParcelReorderingModel();
        
        CiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj);

        CiftiParcelReorderingModel& operator=(const CiftiParcelReorderingModel& obj);
        
        void getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& parcelLabelFilesOut,
                                                           CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                           int32_t& selectedParcelLabelFileMapIndexOut,
                                                           bool& enabledStatusOut) const;
        
        void setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                           const int32_t selectedParcelLabelFileMapIndex,
                                                           const bool enabledStatus);
        
        bool createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                    const int32_t parcelLabelFileMapIndex,
                                    AString& errorMessageOut);
        
        const CiftiParcelReordering* getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                         const int32_t parcelLabelFileMapIndex) const;
        
        CiftiParcelReordering* getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                         const int32_t parcelLabelFileMapIndex);
        
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

        void validateSelectedParcelLabelFileAndMap(std::vector<CiftiParcelLabelFile*>* optionalParcelLabelFilesOut) const;
        
        std::vector<CiftiParcelLabelFile*> getParcelLabelFiles() const;
        
        void clearCiftiParcelReordering();
        
        mutable std::map<CiftiParcelLabelFile*, bool> m_parcelLabelFileCompatibilityStatus;
        
        const CiftiMappableDataFile* m_parentCiftiMappableDataFile;
        
        SceneClassAssistant* m_sceneAssistant;

        std::vector<CiftiParcelReordering*> m_parcelReordering;
        
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
