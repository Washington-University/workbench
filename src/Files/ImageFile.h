/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#ifndef __IMAGE_FILE_H__
#define __IMAGE_FILE_H__

#include "MediaFile.h"
#include "CaretPointer.h"
#include "GraphicsPrimitiveV3fT3f.h"

class QColor;
class QImage;

namespace caret {
    class ControlPointFile;
    class ControlPoint3D;
    class VolumeFile;
    
/// File for images
class ImageFile : public MediaFile {
public:
//    class ControlPoint {
//    public:
//        ControlPoint(const float i,
//                     const float j,
//                     const float k,
//                     const float x,
//                     const float y,
//                     const float z)
//        : i(i), j(j), k(k), x(x), y(y), z(z) { }
//        
//        void getSource(double pt[3]) const { pt[0] = i; pt[1] = j; pt[2] = k; }
//        
//        void getTarget(double pt[3]) const { pt[0] = x; pt[1] = y; pt[2] = z; }
//        
//        const float i;
//        const float j;
//        const float k;
//        const float x;
//        const float y;
//        const float z;
//    };
    
    /**
     * Location of origin in image data.
     */
    enum IMAGE_DATA_ORIGIN_LOCATION {
        /** Origin at bottom (OpenGL has origin at bottom) */
        IMAGE_DATA_ORIGIN_AT_BOTTOM,
        /** Origin at top (most image formats have origin at top) */
        IMAGE_DATA_ORIGIN_AT_TOP
    };
    
    /**
     * Convert to volume color mode
     */
    enum CONVERT_TO_VOLUME_COLOR_MODE {
        /** Create single component grayscale volume */
        CONVERT_TO_VOLUME_COLOR_GRAYSCALE,
        /** Create three component RGB volume */
        CONVERT_TO_VOLUME_COLOR_RGB
    };
    
//    enum LANDMARK_MODE {
//        VTK_LANDMARK_AFFINE,
//        VTK_LANDMARK_RIGIDBODY,
//        VTK_LANDMARK_SIMILARITY
//    };
    
    ImageFile();
    
    ImageFile(const unsigned char* imageDataRGBA,
              const int imageWidth,
              const int imageHeight,
              const IMAGE_DATA_ORIGIN_LOCATION imageOrigin);
    
    ImageFile(const QImage& img);
    
    ~ImageFile();
    
    void appendImageAtBottom(const ImageFile& img);
    
    void clear();
    
    /**
     * @return Number of frames in the file
     */
    virtual int32_t getNumberOfFrames() const override;

    /**
     * @return Get access to the file's metadata.
     */
    virtual GiftiMetaData* getFileMetaData() override;
    
    /**
     * @return Get access to unmodifiable file's metadata.
     */
    virtual const GiftiMetaData* getFileMetaData() const override;
    
    virtual bool compareFileForUnitTesting(const DataFile* df,
                                           const float tolerance,
                                           AString& messageOut) const;
    
    bool isEmpty() const override;
    
    //QImage* getAsQImage();
    
    const QImage* getAsQImage() const;
    
    void setFromQImage(const QImage& img);
    
    bool getImageBytesRGBA(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                           std::vector<uint8_t>& bytesRGBA,
                           int32_t& widthOut,
                           int32_t& heightOut) const;
    
    bool getImageResizedBytes(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                              const int32_t resizeToWidth,
                              const int32_t resizeToHeight,
                              std::vector<uint8_t>& bytesRGBAOut) const;
    
    bool getImagePixelRGBA(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                           const int32_t pixelI,
                           const int32_t pixelJ,
                           uint8_t pixelRGBAOut[4]) const;
    
    int32_t getWidth() const;
    
    int32_t getHeight() const;
    
    virtual void readFile(const AString& filename) override;
    
    virtual void writeFile(const AString& filename) override;
    
    void cropImageRemoveBackground(const int marginSize,
                                   const uint8_t backgroundColor[3]);
    
    void findImageObject(const uint8_t backgroundColor[3],
                         int objectBoundsOut[4]) const; 
    
    void addMargin(const int marginSize,
                   const uint8_t backgroundColor[3]);
    
    void addMargin(const int marginSizeX,
                   const int marginSizeY,
                   const uint8_t backgroundColor[3]);
    
    void setDotsPerMeter(const int x,
                         const int y);
    
    void resizeToMaximumWidthOrHeight(const int32_t maximumWidthOrHeight);
    
    void resizeToMaximumWidth(const int32_t maximumWidth);
    
    void resizeToMaximumHeight(const int32_t maximumHeight);
    
    void resizeToHeight(const int32_t height);
    
    void resizeToWidth(const int32_t width);
    
    void getImageInByteArray(QByteArray& byteArrayOut,
                             const AString& format) const;
    
    bool setImageFromByteArray(const QByteArray& byteArray,
                               const AString& format);
    
    void combinePreservingAspectAndFillIfNeeded(const std::vector<ImageFile*>& imageFiles,
                                                const int numImagesPerRow,
                                                const uint8_t backgroundColor[3]);
    
    static void getImageFileExtensions(std::vector<AString>& imageFileExtensions,
                                       AString& defaultExtension);
    
    static void getImageFileFilters(std::vector<AString>& imageFileFilters,
                                    AString& defaultFilter);

    static QImage scaleToSizeWithPadding(const QImage& image,
                                         const int width,
                                         const int height,
                                         const QColor* fillColor = NULL);
    
    VolumeFile* convertToVolumeFile(const CONVERT_TO_VOLUME_COLOR_MODE colorMode,
                                    AString& errorMessageOut) const;

    GraphicsPrimitiveV3fT3f* getGraphicsPrimitiveForMediaDrawing() const;
    
    ControlPointFile* getControlPointFile();
    
    const ControlPointFile* getControlPointFile() const;
    
    virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                     SceneClass* sceneClass) override;
    
    virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass) override;

    ImageFile* castToImageFile();

    const ImageFile* castToImageFile() const;
    
    virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
    
private:
    ImageFile(const ImageFile&);
    
    ImageFile& operator=(const ImageFile&);
    
    void insertImage(const QImage& otherImage,
                     const int x,
                     const int y);
    
    static void insertImage(const QImage& insertThisImage,
                            QImage& intoThisImage,
                            const int positionX,
                            const int positionY);
    
    QImage* m_image;
    
    CaretPointer<GiftiMetaData> m_fileMetaData;
    
    CaretPointer<ControlPointFile> m_controlPointFile;
    
    mutable std::unique_ptr<GraphicsPrimitiveV3fT3f> m_graphicsPrimitiveForMediaDrawing;

    static const float s_defaultWindowDepthPercentage;
};

} // namespace

#endif // __IMAGE_FILE_H__
