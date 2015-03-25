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

#include "BrainOpenGLTextAttributes.h"
#include "BrainOpenGLTextRenderInterface.h"

class FTFont;

namespace caret {

    class FtglFontTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        FtglFontTextRenderer();
        
        virtual ~FtglFontTextRenderer();
        
        bool isValid() const;
        
        virtual void drawTextAtWindowCoords(const int viewport[4],
                                            const double windowX,
                                            const double windowY,
                                            const QString& text,
                                            const BrainOpenGLTextAttributes& textAttributes);
        
        void drawVerticalTextAtWindowCoords(const int viewport[4],
                                            const double windowX,
                                            const double windowY,
                                            const QString& text,
                                            const BrainOpenGLTextAttributes&  textAttributes);
        
        void drawTextAtModelCoords(const double modelX,
                                   const double modelY,
                                   const double modelZ,
                                   const QString& text,
                                   const BrainOpenGLTextAttributes& textAttributes);
        
        void getTextBoundsInPixels(double& widthOut,
                                   double& heightOut,
                                   const QString& text,
                                   const BrainOpenGLTextAttributes& textAttributes);
        
        virtual AString getName() const;
        
    private:
        enum FontType {
            FONT_TYPE_PIXMAP,
            FONT_TYPE_TEXTURE
        };
        
        FtglFontTextRenderer(const FtglFontTextRenderer&);

        FtglFontTextRenderer& operator=(const FtglFontTextRenderer&);
        
        FTFont* getFont(const BrainOpenGLTextAttributes& textAttributes);
        
        class FontData {
        public:
            enum PositionType {
                POSITION_TYPE_MATRIX,
                POSITION_TYPE_RASTER
            };
            
            FontData();
            
            ~FontData();
            
            void initialize(const AString& fontFileName,
                            const FontType fontType);
            
            PositionType m_positionType;
            
            QByteArray m_fontData;
            
            FTFont* m_font;
            
            bool m_valid;
        };
        
        struct CharInfo {
            CharInfo(const QString& theChar,
                     double x,
                     double y) : m_char(theChar), m_x(x), m_y(y) { }
            
            const QString m_char;
            const double m_x;
            const double m_y;
        };
        
        void getVerticalTextCharInfo(const QString& text,
                                     const BrainOpenGLTextAttributes& textAttributes,
                                     double& textWidthOut,
                                     double& textHeightOut,
                                     std::vector<CharInfo>& charInfoOut);
        
        void applyColoring(const BrainOpenGLTextAttributes& textAttributes,
                           const double textBoundsBox[4]);
        
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
