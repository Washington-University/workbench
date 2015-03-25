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

    class BrainOpenGLTextAttributes;

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
         * Draw text at the given window coordinates.
         *
         * @param viewport
         *   The current viewport.
         * @param windowX
         *   X-coordinate in the window of first text character
         *   using the 'alignment'
         * @param windowY
         *   Y-coordinate in the window at which bottom of text is placed.
         * @param text
         *   Text that is to be drawn.
         * @param textAttributes
         *   Attributes for text drawing.
         */
        virtual void drawTextAtWindowCoords(const int viewport[4],
                                            const double windowX,
                                            const double windowY,
                                            const QString& text,
                                            const BrainOpenGLTextAttributes& textAttributes) = 0;
        
        /**
         * Draw vertical text at the given window coordinates.
         *
         * @param viewport
         *   The current viewport.
         * @param windowX
         *   X-coordinate in the window of first text character
         *   using the 'alignment'
         * @param windowY
         *   Y-coordinate in the window at which bottom of text is placed.
         * @param text
         *   Text that is to be drawn.
         * @param textAttributes
         *   Attributes for text drawing.
         */
        virtual void drawVerticalTextAtWindowCoords(const int viewport[4],
                                                    const double windowX,
                                                    const double windowY,
                                                    const QString& text,
                                                    const BrainOpenGLTextAttributes&  textAttributes) = 0;
//        {
//            const int32_t numChars = text.length();
//            int y = windowY + ((numChars * fontHeight) / 2.0);
//            
//            for (int32_t i = 0; i < text.length(); i++) {
//                drawTextAtWindowCoords(viewport,
//                                       windowX,
//                                       y,
//                                       text[i],
//                                       alignmentX,
//                                       alignmentY,
//                                       textStyle,
//                                       fontHeight);
//                y -= fontHeight;
//            }
//        }
        
        /**
         * Draw text at the given model coordinates.
         *
         * @param modelX
         *   X-coordinate in model space of first text character
         * @param modelY
         *   Y-coordinate in model space.
         * @param modelZ
         *   Z-coordinate in model space.
         * @param text
         *   Text that is to be drawn.
         * @param textAttributes
         *   Attributes for text drawing.
         */
        virtual void drawTextAtModelCoords(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const QString& text,
                                           const BrainOpenGLTextAttributes& textAttributes) = 0;

        /**
         * Get the bounds of text (in pixels) using the given text
         * attributes.
         *
         * @param widthOut
         *   Output containing width of text characters.
         * @param heightOut
         *   Output containing height of text characters.
         * @param text
         *   Text that is to be drawn.
         * @param textAttributes
         *   Attributes for text drawing.
         */
        virtual void getTextBoundsInPixels(double& widthOut,
                                           double& heightOut,
                                           const QString& text,
                                           const BrainOpenGLTextAttributes& textAttributes) = 0;
        
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
