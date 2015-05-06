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

#include <map>
#include <set>

#include "BrainOpenGLTextRenderInterface.h"

class FTFont;

namespace caret {

    class FtglFontTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        FtglFontTextRenderer();
        
        virtual ~FtglFontTextRenderer();
        
        bool isValid() const;
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const AnnotationText& annotationText);
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText);
        
        virtual void drawTextAtModelCoords(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const AnnotationText& annotationText);
        
        void getTextBoundsInPixels(const AnnotationText& annotationText,
                                   double& xMinOut,
                                   double& xMaxOut,
                                   double& yMinOut,
                                   double& yMaxOut);
        
        void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                              const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              double bottomLeftOut[3],
                                              double bottomRightOut[3],
                                              double topRightOut[3],
                                              double topLeftOut[3]);
        
        virtual AString getName() const;
        
    private:
        enum DepthTestEnum {
            DEPTH_TEST_NO,
            DEPTH_TEST_YES
        };
        
        FtglFontTextRenderer(const FtglFontTextRenderer&);

        FtglFontTextRenderer& operator=(const FtglFontTextRenderer&);
        
        void drawHorizontalTextAtWindowCoords(const double windowX,
                                              const double windowY,
                                              const double windowZ,
                                              const AnnotationText& annotationText);
        
        void drawVerticalTextAtWindowCoords(const double windowX,
                                            const double windowY,
                                            const double windowZ,
                                            const AnnotationText& annotationText);
        
        FTFont* getFont(const AnnotationText& annotationText,
                        const bool creatingDefaultFontFlag);
        
        
        class FontData {
        public:
            FontData();
            
            FontData(const AnnotationText&  annotationText);
            
            ~FontData();
            
            void initialize(const AString& fontFileName);
            
            QByteArray m_fontData;
            
            FTFont* m_font;
            
            bool m_valid;
        };
        
        class CharInfo {
        public:
            CharInfo(const QString& theChar,
                     double x,
                     double y) : m_char(theChar), m_x(x), m_y(y) { }
            QString m_char;
            double m_x;
            double m_y;
        };
        
        void getBoundsForHorizontalTextAtWindowCoords(const AnnotationText& annotationText,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
                                                      double bottomLeftOut[3],
                                                      double bottomRightOut[3],
                                                      double topRightOut[3],
                                                      double topLeftOut[3],
                                                      double firstTextCharacterXYZOut[3],
                                                      double rotationPointXYZOut[3]);
        
        void getBoundsForVerticalTextAtWindowCoords(const AnnotationText& annotationText,
                                                    const double viewportX,
                                                    const double viewportY,
                                                    const double viewportZ,
                                                    double bottomLeftOut[3],
                                                    double bottomRightOut[3],
                                                    double topRightOut[3],
                                                    double topLeftOut[3],
                                                    double rotationPointXYZOut[3],
                                                    std::vector<CharInfo>& textCharsToDraw);
        
        void getVerticalTextCharInfo(const AnnotationText& annotationText,
                                     double& xMinOut,
                                     double& xMaxOut,
                                     double& heightOut,
                                     std::vector<CharInfo>& charInfoOut);
        
        void applyForegroundColoring(const AnnotationText& annotationText);
        
        void applyBackgroundColoring(const AnnotationText& annotationText,
                                     const double bottomLeftOut[3],
                                     const double bottomRightOut[3],
                                     const double topRightOut[3],
                                     const double topLeftOut[3]);
        
        void saveStateOfOpenGL();
        
        void restoreStateOfOpenGL();
        
        /**
         * The default font.  DO NOT delete it since it points to
         * a font in "m_fontNameToFontMap".
         */
        FTFont* m_defaultFont;
        
        /**
         * Map for caching fonts
         */
        typedef std::map<AString, FontData*> FONT_MAP;
        
        /**
         * Iterator for cached fonts.
         */
        typedef FONT_MAP::iterator FONT_MAP_ITERATOR;
        
        /**
         * Caches fonts as they are created
         */
        FONT_MAP m_fontNameToFontMap;
        
        /**
         * Tracks fonts that failed creation to avoid
         * printing an error message more than once.
         */
        std::set<AString> m_failedFontNames;
        
        /** Depth testing enabled status */
        DepthTestEnum m_depthTestingStatus;
        
        static const double s_textMarginSize;
    };
    
#ifdef __FTGL_FONT_TEXT_RENDERER_DECLARE__
    const double FtglFontTextRenderer::s_textMarginSize = 3.0;
#endif // __FTGL_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__FTGL_FONT_TEXT_RENDERER_H__
