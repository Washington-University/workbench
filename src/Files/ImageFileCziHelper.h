
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



#include <memory>

#include "CaretObject.h"
#include "libCZI.h"

class QImage;

namespace caret {

    class ImageFileCziHelper : public CaretObject {
        
    public:
        ImageFileCziHelper();
        
        virtual ~ImageFileCziHelper();
        
        ImageFileCziHelper(const ImageFileCziHelper&) = delete;

        ImageFileCziHelper& operator=(const ImageFileCziHelper&) = delete;
        
        static QImage* readFile(const AString& filename,
                                AString& errorMessageOut);
        
        static QImage* readFileScaled(const AString& filename,
                                      const int32_t maxWidthHeightInPixels,
                                      AString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        static QImage* createImageData(libCZI::IBitmapData* bitmapData,
                                       const AString& filename,
                                       AString& errorMessageOut);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IMAGE_FILE_CZI_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IMAGE_FILE_CZI_HELPER_DECLARE__

} // namespace
#endif  //__IMAGE_FILE_CZI_HELPER_H__
