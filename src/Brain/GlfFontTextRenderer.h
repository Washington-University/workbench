#ifndef __GLF_FONT_TEXT_RENDERER_H__
#define __GLF_FONT_TEXT_RENDERER_H__

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
#include "glf.h"

namespace caret {

    class GlfFontTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        GlfFontTextRenderer();
        
        virtual ~GlfFontTextRenderer();
        
        bool isValid() const;
        
        void drawTextAtWindowCoords(const int viewport[4],
                                    const int windowX,
                                    const int windowY,
                                    const QString& text,
                                    const TextAlignmentX alignmentX,
                                    const TextAlignmentY alignmentY,
                                    const TextStyle textStyle = NORMAL,
                                    const int fontHeight = 14,
                                    const AString& fontName = "");
        
        void drawTextAtModelCoords(const double modelX,
                                   const double modelY,
                                   const double modelZ,
                                   const QString& text,
                                   const TextStyle textStyle = NORMAL,
                                   const int fontHeight = 14,
                                   const AString& fontName = "");
        
         void getTextBoundsInPixels(const QString& text,
                                           const TextStyle textStyle,
                                           const int fontHeight,
                                    const AString& fontName,
                                    int32_t& widthOut,
                                    int32_t& heightOut);
        
    private:
        void drawString(char *str);
        
        GlfFontTextRenderer(const GlfFontTextRenderer&);

        GlfFontTextRenderer& operator=(const GlfFontTextRenderer&);
        
    private:
        void saveStateOfOpenGL();
        
        void restoreStateOfOpenGL();
        
//        class FontData {
//        public:
//            FontData(QFont* font,
//                     const QString fontName,
//                     const int fontHeight,
//                     const TextStyle textStyle);
//
//            ~FontData();
//            
//            QFont* m_font;
//            QString m_fontName;
//            int m_fontHeight;
//            TextStyle m_textStyle;
//        };
//        
//        std::vector<FontData*> m_fonts;
//        
//        QFont* findFont(const QString& fontName,
//                        const int fontHeight,
//                        const TextStyle textStyle);
        
        QString m_emptyNameFont;
        
        int m_arialFont;
        
        bool m_fontFileValid;
    };
    
#ifdef __GLF_FONT_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GLF_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__GLF_FONT_TEXT_RENDERER_H__
