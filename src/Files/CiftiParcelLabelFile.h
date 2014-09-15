#ifndef __CIFTI_PARCEL_LABEL_FILE_H__
#define __CIFTI_PARCEL_LABEL_FILE_H__

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


#include "CiftiMappableDataFile.h"



namespace caret {

    class SceneClassAssistant;
    
    class CiftiParcelLabelFile : public CiftiMappableDataFile {
        
    public:
        CiftiParcelLabelFile();
        
        virtual ~CiftiParcelLabelFile();
        

        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        CiftiParcelLabelFile(const CiftiParcelLabelFile&);

        CiftiParcelLabelFile& operator=(const CiftiParcelLabelFile&);
        
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_PARCEL_LABEL_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_PARCEL_LABEL_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_PARCEL_LABEL_FILE_H__
