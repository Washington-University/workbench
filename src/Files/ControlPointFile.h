#ifndef __CONTROL_POINT_FILE_H__
#define __CONTROL_POINT_FILE_H__

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


#include "CaretDataFile.h"
#include "CaretPointer.h"


namespace caret {
    class ControlPoint3D;
    class Matrix4x4;
    class SceneClassAssistant;
    
    class ControlPointFile : public CaretDataFile {
        
    public:
        ControlPointFile();
        
        virtual ~ControlPointFile();

        bool isEmpty() const;
        
        virtual void clear();
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        int32_t getNumberOfControlPoints() const;
        
        void addControlPoint(const ControlPoint3D& controlPoint);
        
        const ControlPoint3D* getControlPointAtIndex(const int32_t index) const;

        ControlPoint3D* getControlPointAtIndex(const int32_t index);
        
        void removeAllControlPoints();
        
        void removeControlPointAtIndex(const int32_t index);
        
        bool updateLandmarkTransformationMatrix(AString& errorMessageOut);
        
        Matrix4x4* getLandmarkTransformationMatrix();
        
        const Matrix4x4* getLandmarkTransformationMatrix() const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        ControlPointFile(const ControlPointFile&);

        ControlPointFile& operator=(const ControlPointFile&);
        
        void clearPrivate();
        
        CaretPointer<GiftiMetaData> m_metadata;
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        std::vector<ControlPoint3D*> m_controlPoints;
        
        CaretPointer<Matrix4x4> m_landmarkTransformationMatrix;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CONTROL_POINT_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONTROL_POINT_FILE_DECLARE__

} // namespace
#endif  //__CONTROL_POINT_FILE_H__
