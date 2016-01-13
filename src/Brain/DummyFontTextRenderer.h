#ifndef __DUMMY_FONT_TEXT_RENDERER_H__
#define __DUMMY_FONT_TEXT_RENDERER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "BrainOpenGLTextRenderInterface.h"



namespace caret {

    class DummyFontTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        DummyFontTextRenderer();
        
        virtual ~DummyFontTextRenderer();
        
        bool isValid() const;
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText);
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const AnnotationText& annotationText);
        
        virtual void drawTextAtModelCoords(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const AnnotationText& annotationText);
        
        virtual void getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                const double viewportHeight,
                                                double& widthOut,
                                                double& heightOut);
        
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
                                                      const double viewportHeight,
                                                      double bottomLeftOut[3],
                                                      double bottomRightOut[3],
                                                      double topRightOut[3],
                                                      double topLeftOut[3]);
        virtual AString getName() const;

        // ADD_NEW_METHODS_HERE

    private:
        DummyFontTextRenderer(const DummyFontTextRenderer&);

        DummyFontTextRenderer& operator=(const DummyFontTextRenderer&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DUMMY_FONT_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DUMMY_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__DUMMY_FONT_TEXT_RENDERER_H__
