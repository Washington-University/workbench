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
         * Draw annnotation text at the given viewport coordinates using
         * the the annotations attributes for the style of text.
         *
         * @param viewportX
         *     Viewport X-coordinate.
         * @param viewportY
         *     Viewport Y-coordinate.
         * @param viewportZ
         *     Viewport Z-coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText) = 0;
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * @param modelX
         *     Model X-coordinate.
         * @param modelY
         *     Model Y-coordinate.
         * @param modelZ
         *     Model Z-coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        virtual void drawTextAtModelCoords(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const AnnotationText& annotationText) = 0;
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * @param modelXYZ
         *     Model XYZ coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        void drawTextAtModelCoords(const double modelXYZ[3],
                                   const AnnotationText& annotationText) {
            drawTextAtModelCoords(modelXYZ[0], modelXYZ[1], modelXYZ[2], annotationText);
        }
        
        /**
         * Draw annnotation text at the given model coordinates using
         * the the annotations attributes for the style of text.
         *
         * @param modelXYZ
         *     Model XYZ coordinate.
         * @param annotationText
         *     Annotation text and attributes.
         */
        void drawTextAtModelCoords(const float modelXYZ[3],
                                   const AnnotationText& annotationText) {
            drawTextAtModelCoords(modelXYZ[0], modelXYZ[1], modelXYZ[2], annotationText);
        }
        
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
         * Get the bounds of text (in pixels) using the given text
         * attributes.
         *
         * See http://ftgl.sourceforge.net/docs/html/metrics.png
         *
         * @param annotationText
         *   Text that is to be drawn.
         * @param viewportX
         *    Viewport X-coordinate.
         * @param viewportY
         *    Viewport Y-coordinate.
         * @param viewportZ
         *    Viewport Z-coordinate.
         * @param xMinOut
         *    Minimum X of text.
         * @param xMaxOut
         *    Maximum X of text.
         * @param yMinOut
         *    Minimum Y of text.
         * @param yMaxOut
         *    Maximum Y of text.
         */
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
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
