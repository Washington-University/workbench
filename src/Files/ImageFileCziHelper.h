
#ifndef __IMAGE_FILE_CZI_HELPER_H__
#define __IMAGE_FILE_CZI_HELPER_H__

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



#include <QRect>

#include "CaretAssert.h"
#include "CaretObject.h"
#include "libCZI.h"

class QImage;

namespace caret {

    class ImageFileCziHelper : public CaretObject {
        
    public:
    public:
        class ReadResult {
        public:
            ReadResult(QImage* image,
                       const QRect& imageRoiRect,
                       const QRect& fullImageRect)
            : m_image(image),
            m_imageRoiRect(imageRoiRect),
            m_fullImageRect(fullImageRect),
            m_valid(true) {
                CaretAssert(m_image);
            }
            
            ReadResult(const AString& errorMessage)
            : m_errorMessage(errorMessage),
            m_valid(false) {
                CaretAssert(! m_errorMessage.isEmpty());
            }
            
            /**
             * Image is NOT deleted as caller is expected to take ownership of the image
             */
            QImage* m_image;
            
            /**
             * Region of interest this image occupies in the original, full image.
             * This may be different width/height than m_image due to zooming.
             */
            QRect m_imageRoiRect;
            
            /**
             * Full image rectangle
             */
            QRect m_fullImageRect;
            
            AString m_errorMessage;
            
            bool m_valid = false;
        };
        
        ImageFileCziHelper();
        
        virtual ~ImageFileCziHelper();
        
        ImageFileCziHelper(const ImageFileCziHelper&) = delete;

        ImageFileCziHelper& operator=(const ImageFileCziHelper&) = delete;
        
        static ReadResult readFile(const AString& filename);
        
        static ReadResult readFileScaled(const AString& filename,
                                         const int32_t maxWidthHeightInPixels,
                                         const QRect& roiRect);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        static ReadResult createImageData(libCZI::IBitmapData* bitmapData,
                                          const libCZI::IntRect& imageRoiRect,
                                          const libCZI::IntRect& originalImageRect,
                                          const AString& filename);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IMAGE_FILE_CZI_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IMAGE_FILE_CZI_HELPER_DECLARE__

} // namespace
#endif  //__IMAGE_FILE_CZI_HELPER_H__
