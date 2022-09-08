#ifndef __HISTOLOGY_COORDINATE_H__
#define __HISTOLOGY_COORDINATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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
#include "Vector3D.h"


namespace caret {
    class SceneClassAssistant;
    
    class HistologyCoordinate : public CaretObject, public SceneableInterface {
        
    public:
        HistologyCoordinate();
        
        virtual ~HistologyCoordinate();
        
        HistologyCoordinate(const HistologyCoordinate& obj);
        
        HistologyCoordinate& operator=(const HistologyCoordinate& obj);
        
        
        Vector3D getStereotaxicXYZ() const;
        
        void setStereotaxicXYZ(const Vector3D& xyz);
        
        AString getHistologySlicesFileName() const;
        
        void setHistologySlicesFileName(const AString& histologySlicesFileName);
        
        AString getHistologyMediaFileName() const;
        
        void setHistologyMediaFileName(const AString& histologyMediaFileName);
        
        int64_t getSliceIndex() const;
        
        void setSliceIndex(const int64_t sliceIndex);
        
        Vector3D getPlaneXY() const;
        
        void setPlaneXYZ(const Vector3D& xyz);
        
        bool isStereotaxicXYZValid() const;
        
        bool isPlaneXYValid() const;
        
        bool isHistologySlicesFileNameValid() const;
        
        bool isHistologyMediaFileNameValid() const;
        
        bool isSliceIndexValid() const;
        
        int64_t getSliceNumber() const;
        
        void setSliceNumber(const int64_t sliceNumber);
        
        bool isSliceNumberValid() const;
        
        
        
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
        void copyHelperHistologyCoordinate(const HistologyCoordinate& obj);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        /** stereotaxic coordinate*/
        Vector3D m_stereotaxicXYZ;
        
        /** name of histology slices file*/
        AString m_histologySlicesFileName;
        
        /** name of histology media file*/
        AString m_histologyMediaFileName;
        
        /** index of slice*/
        int64_t m_sliceIndex = 0;
        
        /** plane XY coordinate, Z is slice index*/
        Vector3D m_planeXY;
        
        /** validity of stereotaxic XYZ*/
        bool m_stereotaxicXYZValid = false;
        
        /** validity of plane XY*/
        bool m_planeXYValid = false;
        
        /** validity of histology slices file name*/
        bool m_histologySlicesFileNameValid = false;
        
        /** validity of histology media file name*/
        bool m_histologyMediaFileNameValid = false;
        
        /** validity of slice index*/
        bool m_sliceIndexValid = false;
        
        /** number of slice*/
        int64_t m_sliceNumber = 0;
        
        /** validity of slice number*/
        bool m_sliceNumberValid = false;
        
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __HISTOLOGY_COORDINATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOLOGY_COORDINATE_DECLARE__
    
} // namespace
#endif  //__HISTOLOGY_COORDINATE_H__

