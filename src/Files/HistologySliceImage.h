#ifndef __HISTOLOGY_SLICE_IMAGE_H__
#define __HISTOLOGY_SLICE_IMAGE_H__

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



#include <memory>

#include "CaretObject.h"

#include "EventListenerInterface.h"
#include "Matrix4x4.h"
#include "SceneableInterface.h"


namespace caret {
    class MediaFile;
    class SceneClassAssistant;

    class HistologySliceImage : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        HistologySliceImage(const AString& sceneName,
                            const AString& mediaFileName,
                            const AString& distanceFileName,
                            const Matrix4x4& scaledToPlaneMatrix,
                            const bool scaledToPlaneMatrixValidFlag);
        
        virtual ~HistologySliceImage();
        
        HistologySliceImage(const HistologySliceImage& obj);

        HistologySliceImage& operator=(const HistologySliceImage& obj);
        
        MediaFile* getMediaFile();
        
        const MediaFile* getMediaFile() const;
        
//        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
//                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const;
        
        void setPlaneToMillimetersMatrix(const Matrix4x4& planeToMillimetersMatrix,
                                         const bool planeToMillimetersMatrixValidFlag);
        
        std::vector<AString> getChildDataFilePathNames() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

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
        void copyHelperHistologySliceImage(const HistologySliceImage& obj);

        const MediaFile* getMediaFilePrivate() const;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        const AString m_sceneName;
        
        const AString m_mediaFileName;
        
        const AString m_distanceFileName;
        
        const Matrix4x4 m_scaledToPlaneMatrix;
        
        const bool m_scaledToPlaneMatrixValidFlag = false;
        
        Matrix4x4 m_planeToMillimetersMatrix;
        
        bool m_planeToMillimetersMatrixValidFlag = false;
        
        Matrix4x4 m_millimetersToPlaneMatrix;
        
        bool m_millimetersToPlaneMatrixValidFlag = false;
        
        mutable std::unique_ptr<MediaFile> m_mediaFile;
        
        mutable bool m_attemptedToReadMediaFileFlag = false;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SLICE_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOLOGY_SLICE_IMAGE_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SLICE_IMAGE_H__
