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

#include <array>
#include <memory>

#include <QRect>

#include "MediaFile.h"
#include "CaretPointer.h"

class QColor;
class QImage;

namespace caret {
    class ControlPointFile;
    class ControlPoint3D;
    class GraphicsPrimitiveV3fT3f;
    class RectangleTransform;
    class VolumeFile;
    
/// File for images
class ImageFile : public MediaFile {
public:
    /**
     * Convert to volume color mode
     */
    enum CONVERT_TO_VOLUME_COLOR_MODE {
        /** Create single component grayscale volume */
        CONVERT_TO_VOLUME_COLOR_GRAYSCALE,
        /** Create three component RGB volume */
        CONVERT_TO_VOLUME_COLOR_RGB
    };
    
    ImageFile();
    
    ImageFile(const ImageFile& imageFile);
    
    ImageFile(const unsigned char* imageDataRGBA,
              const int imageWidth,
              const int imageHeight,
              const IMAGE_DATA_ORIGIN_LOCATION imageOrigin);
    
    ImageFile(const QImage& qimage);
    
    ImageFile(QImage* qimage);
    
    ~ImageFile();
    
    void appendImageAtBottom(const ImageFile& img);
    
    void clear();
    
    virtual void clearModified() override;
    
    virtual bool isModified() const override;
    
    /**
     * @return Number of frames in the file
     */
    virtual int32_t getNumberOfFrames() const override;

    /**
     * @return Name of frame at given index.
     * @param frameIndex Index of the frame
     */
    virtual AString getFrameName(const int32_t frameIndex) const override;

    /**
     * @return Get access to the file's metadata.
     */
    virtual GiftiMetaData* getFileMetaData() override;
    
    /**
     * @return Get access to unmodifiable file's metadata.
     */
    virtual const GiftiMetaData* getFileMetaData() const override;
    
    virtual bool supportsFileMetaData() const override;
    
    virtual bool compareFileForUnitTesting(const DataFile* df,
                                           const float tolerance,
                                           AString& messageOut) const;
    
    bool isEmpty() const override;
    
    const QImage* getAsQImage() const;
    
    void setFromQImage(const QImage& img);
    
    bool getImageBytesRGBA(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                           std::vector<uint8_t>& bytesRGBA,
                           int32_t& widthOut,
                           int32_t& heightOut) const;
    
    static bool getImageBytesRGBA(const QImage* qImage,
                                  const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                                  std::vector<uint8_t>& bytesRGBA,
                                  int32_t& widthOut,
                                  int32_t& heightOut);

    bool getImageResizedBytes(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                              const int32_t resizeToWidth,
                              const int32_t resizeToHeight,
                              std::vector<uint8_t>& bytesRGBAOut) const;
    
    bool getImagePixelRGBA(const int32_t tabIndex,
                           const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                           const PixelIndex& pixelIndex,
                           uint8_t pixelRGBAOut[4]) const override;
    
    virtual int32_t getWidth() const override;
    
    virtual int32_t getHeight() const override;

    virtual bool pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndexOriginAtTop,
                                            const bool includeNonlinearFlag,
                                            std::array<float, 3>& xyzOut) const override;
    
    virtual bool stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                            const bool includeNonlinearFlag,
                                            PixelIndex& pixelIndexOriginAtTopLeftOut) const override;

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
    
    static void getSaveQFileDialogImageFilters(std::vector<AString>& imageFileFilters,
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
    
    virtual bool isPixelIndexValid(const int32_t tabIndex,
                                   const PixelIndex& pixelIndex) const override;
    
    virtual void getPixelIdentificationText(const int32_t tabIndex,
                                            const PixelIndex& pixelIndex,
                                            std::vector<AString>& columnOneTextOut,
                                            std::vector<AString>& columnTwoTextOut,
                                            std::vector<AString>& toolTipTextOut) const;
    
    virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
    
    static void getQtSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                  std::vector<AString>& writableExtensionsOut);
    
    static void getImageFileQtSupportedOptionExtensions(std::vector<AString>& clipRectReadableExtensionsOut,
                                                        std::vector<AString>& scaledClipRectReadableExtensionsOut,
                                                        std::vector<AString>& metaDataReadableWritableExtensionsOut);
    
    static void getWorkbenchSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                         std::vector<AString>& writableExtensionsOut,
                                                         AString& defaultWritableExtension);
    
protected:
    
private:
    ImageFile& operator=(const ImageFile&);
    
    void initializeMembersImageFile();
    
    void insertImage(const QImage& otherImage,
                     const int x,
                     const int y);
    
    static void insertImage(const QImage& insertThisImage,
                            QImage& intoThisImage,
                            const int positionX,
                            const int positionY);
    
    static QImage* limitImageDimensions(QImage* image,
                                        const AString& filename);

    void readFileMetaDataFromQImage();
    
    void writeFileMetaDataToQImage() const;
    
    PixelIndex transformPixelBottomLeftToTopLeft(const PixelIndex& pixelIndexBottomLeft) const;
    
    QImage* m_image;
    
    mutable bool m_sceneCreatedBeforeDefaultScaling = false;
    
    mutable CaretPointer<GiftiMetaData> m_fileMetaData;
    
    mutable AString m_frameOneName;
    
    CaretPointer<ControlPointFile> m_controlPointFile;
    
    mutable std::unique_ptr<RectangleTransform> m_pixelBottomLeftToTopLeftTransform;
    
    mutable std::unique_ptr<GraphicsPrimitiveV3fT3f> m_graphicsPrimitiveForMediaDrawing;
    
    static const AString SCENE_VERSION_NUMBER;
};

#ifdef __IMAGE_FILE_DECLARE__
    const AString ImageFile::SCENE_VERSION_NUMBER = "SCENE_VERSION_NUMBER";
#endif // __IMAGE_FILE_DECLARE__

} // namespace

#endif // __IMAGE_FILE_H__
