#ifndef __MEDIA_FILE_H__
#define __MEDIA_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include <memory>

#include "CaretDataFile.h"
#include "DefaultViewTransform.h"
#include "NiftiEnums.h"
#include "SceneClassAssistant.h"
#include "Vector3D.h"
#include "VoxelIJK.h"

namespace caret {

    class BoundingBox;
    class VolumeSpace;

    class MediaFile : public CaretDataFile {
        
    public:
        /**
         * \class caret::MediaFile::PixelCoordinate
         * \brief Coordinate for pixel data in media (image) files
         * \ingroup Files
         */
        class PixelCoordinate : public Vector3D {
        public:
            PixelCoordinate()
            : Vector3D(0.0, 0.0, 0.0) { }
            
            PixelCoordinate(const std::array<float,2>& xy)
            : Vector3D(xy[0], xy[1], 0.0) { }
            
            PixelCoordinate(const std::array<float,3>& xyz)
            : Vector3D(xyz[0], xyz[1], xyz[2]) { }
            
            PixelCoordinate(const float x,
                            const float y,
                            const float z = 0)
            : Vector3D(x, y, z) { }
            
            float getX() { return *this[0]; }
            
            float getY() { return *this[1]; }
            
            float getZ() { return *this[2]; }
            
            void setX(const float x) { *this[0] = x; }
            
            void setY(const float y) { *this[1] = y; }
            
            void setZ(const float z) { *this[2] = z; }
        };

        /**
         * \class caret::MediaFile::PixelIndex
         * \brief Indexing for pixel data in media (image) files
         * \ingroup Files
         */
        class PixelIndex : public VoxelIJK {
        public:
            PixelIndex() : VoxelIJK(0, 0, 0) { }
            
            PixelIndex(const int64_t i,
                       const int64_t j,
                       const int64_t k = 0)
            : VoxelIJK(i, j, k) { }
            
            PixelIndex(const PixelCoordinate& coordinate)
            : VoxelIJK(coordinate[0],
                       coordinate[1],
                       coordinate[2]) { }
            
            int64_t getI() const { return m_ijk[0]; }
            
            int64_t getJ() const { return m_ijk[1]; }

            int64_t getK() const { return m_ijk[2]; }
        };
        
        
        virtual ~MediaFile();
        
        MediaFile(const MediaFile& mediaFile);

        MediaFile& operator=(const MediaFile&) = delete;
        
        /**
         * @return The structure for this file.
         */
        virtual StructureEnum::Enum getStructure() const override;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        /**
         * @return Number of frames in the file
         */
        virtual int32_t getNumberOfFrames() const = 0;

        /**
         * @return Name of frame at given index.
         * @param frameIndex Index of the frame
         */
        virtual AString getFrameName(const int32_t frameIndex) const;

        /**
         * @return The units for the 'interval' between two consecutive frames.
         */
        NiftiTimeUnitsEnum::Enum getFrameIntervalUnits() const;

        /**
         * Get the units value for the first frame and the
         * quantity of units between consecutive frames.  If the
         * units for the frame is unknown, value of one (1) are
         * returned for both output values.
         *
         * @param firstFrameUnitsValueOut
         *     Output containing units value for first frame.
         * @param frameIntervalStepValueOut
         *     Output containing number of units between consecutive frame.
         */
        virtual void getFrameIntervalStartAndStep(float& firstFrameUnitsValueOut,
                                                  float& frameIntervalStepValueOut) const;
        

        MediaFile* castToMediaFile();
        
        const MediaFile* castToMediaFile() const;
        
        virtual bool indexValid(const PixelIndex& pixelIndex) const;
        
        virtual PixelIndex spaceToIndex(const PixelCoordinate& coordinate) const;
        
        virtual bool spaceToIndexValid(const PixelCoordinate& coordinate,
                                       PixelIndex& pixelIndexOut) const;
        
        const BoundingBox* getSpatialBoudingBox() const;
        
        virtual DefaultViewTransform getDefaultViewTransform() const = 0;
        
        static float getMediaDrawingOrthographicHalfHeight();
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
    protected: 
        MediaFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void initializeMembersMediaFile();
        
        void initializeVolumeSpace(const int32_t imageWidth,
                                   const int32_t imageHeight,
                                   const std::array<float,3> firstPixelXYZ,
                                   const std::array<float,3> pixelStepXYZ);
        
    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<VolumeSpace> m_volumeSpace;
        
        std::unique_ptr<BoundingBox> m_spatialBoundingBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MEDIA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_H__
