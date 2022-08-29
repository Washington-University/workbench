#ifndef __HISTOLOGY_SLICES_FILE_H
#define __HISTOLOGY_SLICES_FILE_H

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

#include "CaretDataFile.h"

#include "EventListenerInterface.h"
#include "Matrix4x4.h"

namespace caret {
    class HistologySlice;
    class SceneClassAssistant;

    class HistologySlicesFile : public CaretDataFile, public EventListenerInterface {
        
    public:
//        class Scene : public CaretObject {
//        public:
//            Scene(const AString& sceneName,
//                  const Matrix4x4& scaleToPlaneMatrix,
//                  const AString& cziFileName)
//            : m_sceneName(sceneName),
//            m_scaleToPlaneMatrix(scaleToPlaneMatrix),
//            m_cziFileName(cziFileName) { }
//
//            virtual AString toString() const override {
//                AString s;
//                s.appendWithNewLine("Scene Name: "
//                                    + m_sceneName);
//                s.appendWithNewLine("Scale to Plane Matrix: "
//                                    + m_scaleToPlaneMatrix.toString());
//                s.appendWithNewLine("Czi File Name: "
//                                    + m_cziFileName);
//                return s;
//            }
//
//            AString getSceneName() const { return m_sceneName; }
//
//            AString getCziFileName() const { return m_cziFileName; }
//
//            Matrix4x4 getScaleToPlaneMatrix() const { return m_scaleToPlaneMatrix; }
//
//            const AString m_sceneName;
//
//            const Matrix4x4 m_scaleToPlaneMatrix;
//
//            const AString m_cziFileName;
//        };
        
//        class Slice : public CaretObject {
//        public:
//            Slice(const int32_t sliceNumber,
//                  const AString& histToMriWarpFileName,
//                  const AString& mriToHistWarpFileName,
//                  const Matrix4x4& planeToMmMatrix)
//            : m_sliceNumber(sliceNumber),
//            m_histToMriWarpFileName(histToMriWarpFileName),
//            m_mriToHistWarpFileName(mriToHistWarpFileName),
//            m_planeToMmMatrix(planeToMmMatrix)
//            { }
//
//            void addScene(const Scene& scene) {
//                m_scenes.push_back(scene);
//            }
//
//            int32_t getNumberOfScenes() const {
//                return m_scenes.size();
//            }
//
//            const Scene* getScene(const int32_t sceneIndex) const {
//                return &m_scenes[sceneIndex];
//            }
//
//            Matrix4x4 getPlaneToMillimetersMatrix() const {
//                return m_planeToMmMatrix;
//            }
//
//            virtual AString toString() const override {
//                AString s;
//                s.appendWithNewLine("Slice Number: "
//                                    + AString::number(m_sliceNumber));
//                s.appendWithNewLine("Hist to MRI Warp File: "
//                         + m_histToMriWarpFileName);
//                s.appendWithNewLine("MRI to Hist Warp File: "
//                         + m_mriToHistWarpFileName);
//                s.appendWithNewLine("Plane to MM Matrix: "
//                         + m_planeToMmMatrix.toString());
//                for (auto& scene : m_scenes) {
//                    s.appendWithNewLine(scene.toString());
//                }
//                return s;
//            }
//
//            const int32_t m_sliceNumber;
//
//            const AString m_histToMriWarpFileName;
//
//            const AString m_mriToHistWarpFileName;
//
//            const Matrix4x4 m_planeToMmMatrix;
//
//            std::vector<Scene> m_scenes;
//        };
        
        HistologySlicesFile();
        
        virtual ~HistologySlicesFile();
        
        HistologySlicesFile(const HistologySlicesFile& obj);

        HistologySlicesFile& operator=(const HistologySlicesFile& obj);
        
        virtual bool isEmpty() const override;
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;

        virtual void clear() override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;

        virtual bool supportsWriting() const override;

        void addHistologySlice(HistologySlice* slice);
        
        int32_t getNumberOfHistologySlices() const;
        
        const HistologySlice* getHistologySliceByIndex(const int32_t sliceIndex) const;
        
        const HistologySlice* getHistologySliceByNumber(const int32_t sliceNumber) const;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual AString toString() const override;
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event) override;

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                          SceneClass* sceneClass) override;

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass) override;
        
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
        void copyHelperHistologySlicesFile(const HistologySlicesFile& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<GiftiMetaData> m_metaData;
        
        std::vector<std::unique_ptr<HistologySlice>> m_histologySlices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SLICES_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOLOGY_SLICES_FILE_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SLICES_FILE_H
