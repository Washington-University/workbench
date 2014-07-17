#ifndef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
#define __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_

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

class QGLWidget;

class QFont;

namespace caret {

    class BrainOpenGLWidgetTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        BrainOpenGLWidgetTextRenderer(QGLWidget* glWidget);
        
        virtual ~BrainOpenGLWidgetTextRenderer();
        
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
        BrainOpenGLWidgetTextRenderer(const BrainOpenGLWidgetTextRenderer&);

        BrainOpenGLWidgetTextRenderer& operator=(const BrainOpenGLWidgetTextRenderer&);
        
    private:
        class FontData {
        public:
            FontData();

            ~FontData();
            
            void initialize(const AString& fontName,
                            const TextStyle textStyle);
            
            QFont* m_font;
            bool m_fontValid;
        };
        
        FontData m_normalFont;
        
        FontData m_boldFont;
        
        QFont* findFont(const TextStyle textStyle,
                        const int fontHeight);
        
        QGLWidget* m_glWidget;
    };
    
#ifdef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
