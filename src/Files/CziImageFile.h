#ifndef __CZI_IMAGE_FILE_H__
#define __CZI_IMAGE_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <QRect>

#include "MediaFile.h"
#include "libCZI.h"

using namespace libCZI;

class QImage;

namespace caret {

    class GraphicsPrimitiveV3fT3f;
    class VolumeSpace;
    
    class CziImageFile : public MediaFile {
        
    public:
        /**
         * Data for an image read from a CZI file
         */
        class CziImage {
        public:
            CziImage(const AString& filename,
                     QImage* image,
                     VolumeSpace* pixelToCoordinateTransform,
                     BoundingBox* spatialBoundingBox);
            
            ~CziImage();
            
            CziImage(const CziImage&) = delete;
            
            CziImage& operator=(const CziImage&) = delete;
            
            GraphicsPrimitiveV3fT3f* getGraphicsPrimitiveForMediaDrawing() const;
            
        private:
            const AString m_filename;
            
            std::unique_ptr<QImage> m_image;
            
            /**
             * Converts from pixel index to pixel coordinate
             */
            std::unique_ptr<VolumeSpace> m_pixelToCoordinateTransform;
            
            /**
             * Bounds of the image
             */
            std::unique_ptr<BoundingBox> m_spatialBoundingBox;
            

            mutable std::unique_ptr<GraphicsPrimitiveV3fT3f> m_graphicsPrimitiveForMediaDrawing;
            
            /*
             * Maps from pixels
             */
            //std::unique_ptr<VolumeSpace> m_pixelsToCziBoundingBox;
            
            friend class CziImageFile;
        };
        
        CziImageFile();
        
        virtual ~CziImageFile();
        
        CziImageFile(const CziImageFile&) = delete;

        CziImageFile& operator=(const CziImageFile&) = delete;
        
        virtual CziImageFile* castToCziImageFile() override;
        
        virtual const CziImageFile* castToCziImageFile() const override;

        bool isEmpty() const override;
        
        virtual void clearModified() override;
        
        virtual bool isModified() const override;
        
        GiftiMetaData* getFileMetaData() override;
        
        const GiftiMetaData* getFileMetaData() const override;
        
        virtual int32_t getNumberOfFrames() const override;
        
        virtual DefaultViewTransform getDefaultViewTransform(const int32_t tabIndex) const override;

        virtual const BoundingBox* getSpatialBoundingBox(const int32_t tabIndex) const override;
        
        virtual const VolumeSpace* getPixelToCoordinateTransform(const int32_t tabIndex) const override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual bool supportsWriting() const override;
        
        CziImage* getImageForTab(const int32_t tabIndex);
        
        const CziImage* getImageForTab(const int32_t tabIndex) const;

        bool getImagePixelRGBA(const int32_t tabIndex,
                               const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                               const PixelIndex& pixelIndex,
                               uint8_t pixelRGBAOut[4]) const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        enum class Status {
            CLOSED,
            OPEN,
            ERROR
        };

        CziImage* getDefaultImage();
        
        const CziImage* getDefaultImage() const;
        
        void closeFile();
        
        CziImage* readFromCziImageFile(const QRect& regionOfInterest,
                                       const int64_t outputImageWidthHeightMaximum,
                                       AString& errorMessageOut);
        
        QImage* createQImageFromBitmapData(IBitmapData* bitmapData,
                                           AString& errorMessageOut);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Status m_status = Status::CLOSED;
        
        AString m_errorMessage;
        
        std::shared_ptr<IStream> m_stream;
        
        std::shared_ptr<ICZIReader> m_reader;

        std::shared_ptr<ISingleChannelScalingTileAccessor> m_scalingTileAccessor;
        
        mutable DefaultViewTransform m_defaultViewTransform;
        
        mutable bool m_defaultViewTransformValidFlag = false;

        mutable std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        std::unique_ptr<CziImage> m_defaultImage;
        
        /*
         * Bounding box as reported by CZI statistics.
         * Origin is in the upper left.
         */
        QRect m_fullBoundingBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_FILE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_FILE_H__
