#ifndef __EVENT_IMAGE_CAPTURE_H__
#define __EVENT_IMAGE_CAPTURE_H__

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

#include <QImage>

#include "Event.h"

namespace caret {

    class EventImageCapture : public Event {
        
    public:
        EventImageCapture(const int32_t browserWindowIndex,
                          const int32_t captureOffsetX,
                          const int32_t captureOffsetY,
                          const int32_t captureWidth,
                          const int32_t captureHeight,
                          const int32_t outputWidth,
                          const int32_t outputHeight);
        
        EventImageCapture(const int32_t browserWindowIndex);
        
        virtual ~EventImageCapture();
        
        int32_t getBrowserWindowIndex() const;
        
        int32_t getOutputWidth() const;
        
        int32_t getOutputHeight() const;
        
        int32_t getCaptureOffsetX() const;
        
        int32_t getCaptureOffsetY() const;
        
        int32_t getCaptureWidth() const;
        
        int32_t getCaptureHeight() const;
        
        QImage getImage() const;
        
        void setImage(const QImage& image);
        
        void getBackgroundColor(uint8_t backgroundColor[3]) const;
        
        void setBackgroundColor(const uint8_t backgroundColor[3]);
        
    private:
        EventImageCapture(const EventImageCapture&);

        EventImageCapture& operator=(const EventImageCapture&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_captureOffsetX;
        
        const int32_t m_captureOffsetY;
        
        const int32_t m_captureWidth;
        
        const int32_t m_captureHeight;
        
        const int32_t m_outputWidth;
        
        const int32_t m_outputHeight;
        
        uint8_t m_backgroundColor[3];
        
        QImage m_image;
    };
    
#ifdef __EVENT_IMAGE_CAPTURE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_IMAGE_CAPTURE_DECLARE__

} // namespace
#endif  //__EVENT_IMAGE_CAPTURE_H__
