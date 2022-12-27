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

#include "BoundingBox.h"
#include "CaretDataFile.h"
#include "EventListenerInterface.h"
#include "Matrix4x4.h"

namespace caret {
    class HistologyCoordinate;
    class HistologySlice;
    class SceneClassAssistant;

    class HistologySlicesFile : public CaretDataFile, public EventListenerInterface {
        
    public:
        static bool isOverlapTestingEnabled();
        
        static void setOverlapTestingEnabled(const bool enabled);
        
        HistologySlicesFile();
        
        virtual ~HistologySlicesFile();
        
        HistologySlicesFile(const HistologySlicesFile& obj);

        HistologySlicesFile& operator=(const HistologySlicesFile& obj);
        
        virtual HistologySlicesFile* castToHistologySlicesFile() override;
        
        virtual const HistologySlicesFile* castToHistologySlicesFile() const override;
        
        virtual bool isEmpty() const override; 
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;

        virtual void clear() override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;

        virtual bool supportsWriting() const override;

        void addHistologySlice(HistologySlice* slice);
        
        int32_t getNumberOfHistologySlices() const;
        
        HistologySlice* getHistologySliceByIndex(const int32_t sliceIndex);
        
        const HistologySlice* getHistologySliceByIndex(const int32_t sliceIndex) const;
        
        const HistologySlice* getHistologySliceByNumber(const int32_t sliceNumber) const;
        
        int32_t getSliceNumberBySliceIndex(const int32_t sliceIndex) const;
        
        int32_t getSliceIndexFromSliceNumber(const int32_t sliceNumber) const;
        
        virtual BoundingBox getStereotaxicXyzBoundingBox() const;
        
        virtual BoundingBox getPlaneXyzBoundingBox() const;
        
        const HistologySlice* getSliceNearestStereotaxicXyz(const Vector3D& stereotaxicXYZ,
                                                            float& mmDistanceToSlice,
                                                            Vector3D& nearestOnSliceStereotaxicXYZ) const;

        virtual void getIdentificationText(const int32_t tabIndex,
                                           const HistologyCoordinate& histologyCoordinate,
                                           std::vector<AString>& columnOneTextOut,
                                           std::vector<AString>& columnTwoTextOut,
                                           std::vector<AString>& toolTipTextOut) const;

        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual AString toString() const override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event) override;

        virtual std::vector<AString> getChildDataFilePathNames() const override;

        float getSliceSpacing() const;
        
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
        void addFileWarningsForMissingChildFiles();
        
        MediaFile* findMediaFileWithName(const AString& mediaFileName) const;
        
        void copyHelperHistologySlicesFile(const HistologySlicesFile& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<GiftiMetaData> m_metaData;
        
        std::vector<std::unique_ptr<HistologySlice>> m_histologySlices;
        
        mutable BoundingBox m_stereotaxicXyzBoundingBox;
        
        mutable bool m_stereotaxicXyzBoundingBoxValidFlag = false;
        
        mutable BoundingBox m_planeXyzBoundingBox;
        
        mutable bool m_planeXyzBoundingBoxValidFlag = false;
        
        mutable float m_sliceSpacing = 2.0;
        
        mutable bool m_sliceSpacingValid = false;
        
        static bool s_overlapTestingEnabled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SLICES_FILE_DECLARE__
    bool HistologySlicesFile::s_overlapTestingEnabled = false;
#endif // __HISTOLOGY_SLICES_FILE_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SLICES_FILE_H
