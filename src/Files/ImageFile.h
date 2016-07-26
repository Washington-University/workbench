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

#include "CaretDataFile.h"
#include "CaretPointer.h"

class QImage;

namespace caret {
    class Matrix4x4;
    class PaletteFile;
    class VolumeFile;
    
/// File for images
class ImageFile : public CaretDataFile {
public:
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
     * @return The structure for this file.
     */
    virtual StructureEnum::Enum getStructure() const;
    
    /**
     * Set the structure for this file.
     * @param structure
     *   New structure for this file.
     */
    virtual void setStructure(const StructureEnum::Enum structure);
    
    /**
     * @return Get access to the file's metadata.
     */
    virtual GiftiMetaData* getFileMetaData();
    
    /**
     * @return Get access to unmodifiable file's metadata.
     */
    virtual const GiftiMetaData* getFileMetaData() const;
    
    virtual bool compareFileForUnitTesting(const DataFile* df,
                                           const float tolerance,
                                           AString& messageOut) const;
    
    bool isEmpty() const;
    
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
    
    int32_t getWidth() const;
    
    int32_t getHeight() const;
    
    float getWindowZ() const;
    
    void setWindowZ(const float windowZ);
    
    virtual void readFile(const AString& filename);
    
    virtual void writeFile(const AString& filename);
    
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
    
    void setImageFromByteArray(const QByteArray& byteArray,
                               const AString& format);
    
    void combinePreservingAspectAndFillIfNeeded(const std::vector<ImageFile*>& imageFiles,
                                                const int numImagesPerRow,
                                                const uint8_t backgroundColor[3]);
    
    static void getImageFileExtensions(std::vector<AString>& imageFileExtensions,
                                       AString& defaultExtension);
    
    static void getImageFileFilters(std::vector<AString>& imageFileFilters,
                                    AString& defaultFilter);

    VolumeFile* convertToVolumeFile(const CONVERT_TO_VOLUME_COLOR_MODE colorMode,
                                    const Matrix4x4& sformMatrix,
                                    const PaletteFile* paletteFile,
                                    AString& errorMessageOut) const;

    virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                     SceneClass* sceneClass);
    
    virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass);
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
    
    float m_windowZ;
    
    CaretPointer<GiftiMetaData> m_fileMetaData;
    
    static const float s_defaultWindowDepthPercentage;
};

} // namespace

#endif // __IMAGE_FILE_H__
