#ifndef __FTGL_FONT_TEXT_RENDERER_H__
#define __FTGL_FONT_TEXT_RENDERER_H__

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


#include "BrainOpenGLTextRenderInterface.h"

class FTPixmapFont;

namespace caret {

    class FtglFontTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        FtglFontTextRenderer();
        
        virtual ~FtglFontTextRenderer();
        
        bool isValid() const;
        
        void drawTextAtWindowCoords(const int viewport[4],
                                    const int windowX,
                                    const int windowY,
                                    const QString& text,
                                    const TextAlignmentX alignmentX,
                                    const TextAlignmentY alignmentY,
                                    const TextStyle textStyle,
                                    const int fontHeight);
        
        void drawTextAtModelCoords(const double modelX,
                                   const double modelY,
                                   const double modelZ,
                                   const QString& text,
                                   const TextStyle textStyle,
                                   const int fontHeight);
        
        void getTextBoundsInPixels(int32_t& widthOut,
                                   int32_t& heightOut,
                                   const QString& text,
                                   const TextStyle textStyle,
                                   const int fontHeight);
        
        virtual AString getName() const;
        
    private:
        FtglFontTextRenderer(const FtglFontTextRenderer&);

        FtglFontTextRenderer& operator=(const FtglFontTextRenderer&);
        
    private:
        
        FTPixmapFont* getFont(const TextStyle textStyle,
                              const int fontHeight);
        
        class FontData {
        public:
            FontData();
            
            ~FontData();
            
            void initialize(const AString& fontFileName);
            
            QByteArray m_fontData;
            
            FTPixmapFont* m_pixmapFont;
            
            bool m_valid;
        };
        
        void saveStateOfOpenGL();
        
        void restoreStateOfOpenGL();

        FontData m_normalFont;
        
        FontData m_boldFont;
    };
    
#ifdef __FTGL_FONT_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FTGL_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__FTGL_FONT_TEXT_RENDERER_H__
