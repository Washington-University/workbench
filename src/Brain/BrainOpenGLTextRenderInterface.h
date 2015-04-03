#ifndef __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_
#define __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_

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


#include "CaretObject.h"

namespace caret {
    class AnnotationText;

    /// An interface for a system that renders text in OpenGL commands
    class BrainOpenGLTextRenderInterface : public CaretObject {
        
    protected:
        /**
         * Constructor.
         */
        BrainOpenGLTextRenderInterface() { }
        
    public:
        /**
         * Destructor.
         */
        virtual ~BrainOpenGLTextRenderInterface() { }
        
        /**
         * Draw annnotation text using its attributes
         * for the style and position of the text.
         *
         * @param annotationText
         *   Text that is to be drawn.
         */
        virtual void drawAnnotationText(const AnnotationText& annotationText) = 0;
        
//        /**
//         * Draw annnotation text at the given VIEWPORT coordinates.
//         *
//         * The origin (0, 0) is at the bottom left corner
//         * of the viewport and (viewport-width, viewport-height)
//         * is at the top right corner of the viewport.
//         *
//         * @param windowX
//         *   X-coordinate of the text.
//         * @param windowY
//         *   Y-coordinate of the text.
//         * @param annotationText
//         *   Text that is to be drawn.
//         */
//        virtual void drawTextAtViewportCoords(const double windowX,
//                                              const double windowY,
//                                              const AnnotationText& annotationText) = 0;
        
//        /**
//         * Draw text at the given model coordinates.
//         *
//         * @param modelX
//         *   X-coordinate in model space of first text character
//         * @param modelY
//         *   Y-coordinate in model space.
//         * @param modelZ
//         *   Z-coordinate in model space.
//         * @param annotationText
//         *   Text that is to be drawn.
//         */
//        virtual void drawTextAtModelCoords(const double modelX,
//                                           const double modelY,
//                                           const double modelZ,
//                                           const AnnotationText& annotationText) = 0;
        
        /**
         * Get the bounds of text (in pixels) using the given text
         * attributes.
         *
         * See http://ftgl.sourceforge.net/docs/html/metrics.png
         *
         * @param annotationText
         *   Text that is to be drawn.
         * @param xMinOut
         *    Minimum X of text.
         * @param xMaxOut
         *    Maximum X of text.
         * @param yMinOut
         *    Minimum Y of text.
         * @param yMaxOut
         *    Maximum Y of text.
         */
        virtual void getTextBoundsInPixels(const AnnotationText& annotationText,
                                           double& xMinOut,
                                           double& xMaxOut,
                                           double& yMinOut,
                                           double& yMaxOut) = 0;
        
        /**
         * @return The font system is valid.
         */
        virtual bool isValid() const = 0;

        /**
         * @return Name of the text renderer.
         */
        virtual AString getName() const = 0;
        
    private:
        BrainOpenGLTextRenderInterface(const BrainOpenGLTextRenderInterface&);

        BrainOpenGLTextRenderInterface& operator=(const BrainOpenGLTextRenderInterface&);
        
    private:
    };
    
#ifdef __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE__H_
