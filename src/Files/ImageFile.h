/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#ifndef __IMAGE_FILE_H__
#define __IMAGE_FILE_H__

#include "DataFile.h"

class QImage;

namespace caret {
    
/// File for images
class ImageFile : public DataFile {
public:
    ImageFile();
    
    ImageFile(const QImage& img);
    
    ~ImageFile();
    
    void appendImageAtBottom(const ImageFile& img);
    
    void clear();
    
    virtual bool compareFileForUnitTesting(const DataFile* df,
                                           const float tolerance,
                                           AString& messageOut) const;
    
    bool isEmpty() const;
    
    QImage* getAsQImage();
    
    const QImage* getAsQImage() const;
    
    void setFromQImage(const QImage& img);
    
    virtual void readFile(const AString& filename) throw (DataFileException);
    
    virtual void writeFile(const AString& filename) throw (DataFileException);
    
    void cropImageRemoveBackground(const int marginSize,
                                   const uint8_t backgroundColor[3]);
    
    void findImageObject(const uint8_t backgroundColor[3],
                         int objectBoundsOut[4]) const; 
    
    void addMargin(const int marginSize,
                   const uint8_t backgroundColor[3]);
    
    void addMargin(const int marginSizeX,
                   const int marginSizeY,
                   const uint8_t backgroundColor[3]);
    
    void combinePreservingAspectAndFillIfNeeded(const std::vector<ImageFile*>& imageFiles,
                                                const int numImagesPerRow,
                                                const uint8_t backgroundColor[3]);
    
private:
    ImageFile(const ImageFile&);
    
    ImageFile& operator=(const ImageFile&);
    
    void insertImage(const QImage& otherImage,
                     const int x,
                     const int y) throw (DataFileException);
    
    static void insertImage(const QImage& insertThisImage,
                            QImage& intoThisImage,
                            const int positionX,
                            const int positionY) throw (DataFileException);
    
    QImage* image;
};

} // namespace

#endif // __IMAGE_FILE_H__
