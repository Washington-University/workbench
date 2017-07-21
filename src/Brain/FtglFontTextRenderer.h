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

#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextOrientationEnum.h"
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
        
        virtual void getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                const double viewportWidth,
                                                const double viewportHeight,
                                                double& widthOut,
                                                double& heightOut);
        
        
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
                                                      const double viewportWidth,
                                                      const double viewportHeight,
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
        
        void drawTextAtViewportCoordsInternal(const DepthTestEnum depthTesting,
                                              const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText);
        
        FTFont* getFont(const AnnotationText& annotationText,
                        const bool creatingDefaultFontFlag);
        
        void drawUnderline(const double lineStartX,
                           const double lineEndX,
                           const double lineY,
                           const double lineZ,
                           const double underlineThickness,
                           uint8_t foregroundRgba[4]);
        
        void drawOutline(const double minX,
                         const double maxX,
                         const double minY,
                         const double maxY,
                         const double z,
                         const double outlineThickness,
                         uint8_t foregroundRgba[4]);
        
        static void expandBox(float bottomLeft[3],
                              float bottomRight[3],
                              float topRight[3],
                              float topLeft[3],
                              const float extraSpaceX,
                              const float extraSpaceY);
        
        double getLineWidthFromPercentageHeight(const double percentageHeight) const;
        
        
        class FontData {
        public:
            FontData();
            
            FontData(const AnnotationText&  annotationText,
                     const int32_t viewportWidth,
                     const int32_t viewportHeight);
            
            ~FontData();
            
            void initialize(const AString& fontFileName);
            
            QByteArray m_fontData;
            
            FTFont* m_font;
            
            bool m_valid;
        };
        
        
        
        /**
         * A text character
         */
        class TextCharacter {
        public:
            TextCharacter(const wchar_t& character,
                          const double horizontalAdvance,
                          const double glyphMinX,
                          const double glyphMaxX,
                          const double glyphMinY,
                          const double glyphMaxY);
            
            ~TextCharacter();
            
            void print(const AString& offsetString);
            
            const wchar_t m_character;
            
            const double m_horizontalAdvance;
            
            /*
             * Bounding box that encloses text with 0.0 at the "pen"
             * http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
             */
            const double m_glyphMinX;
            const double m_glyphMaxX;
            const double m_glyphMinY;
            const double m_glyphMaxY;
            
            /*
             * Offset from previous character
             */
            double m_offsetX;
            double m_offsetY;
            double m_offsetZ;
        };
        
        /**
         * A string of text
         */
        class TextString {
        public:
            TextString(const QString& textString,
                       const AnnotationTextOrientationEnum::Enum orientation,
                       const double underlineThickness,
                       const double outlineThickness,
                       FTFont* font);
            
            ~TextString();
            
            void print(const AString& offsetString);
            
            void setGlyphBounds();
            
            void getTextBoundsInViewportCoordinates(double& viewportMinX,
                                                    double& viewportMaxX,
                                                    double& viewportMinY,
                                                    double& viewportMaxY) const;
            
            const double m_underlineThickness;
            const double m_outlineThickness;
            
            std::vector<TextCharacter*> m_characters;
            
            double m_viewportX;
            double m_viewportY;
            double m_viewportZ;
            
            /*
             * Bounds relative to origin of first character
             */
            double m_stringGlyphsMinX;
            double m_stringGlyphsMaxX;
            double m_stringGlyphsMinY;
            double m_stringGlyphsMaxY;
            
        private:
            void initializeTextCharacterOffsets(const AnnotationTextOrientationEnum::Enum orientation);
            
        };
        
        /**
         * The "high level" text object containing strings of text
         */
        class TextStringGroup {
        public:
            TextStringGroup(const AnnotationText& annotationText,
                            FTFont* font,
                            const double viewportX,
                            const double viewportY,
                            const double viewportZ,
                            const double rotationAngle,
                            const double lineThicknessForViewportHeight);
            
            ~TextStringGroup();
            
            void getViewportBounds(const double margin,
                           double bottomLeftOut[3],
                           double bottomRightOut[3],
                           double topRightOut[3],
                           double topLeftOut[3],
                           double rotationPointXYZOut[3]) const;
            
            void print();
            
            std::vector<TextString*> m_textStrings;
            
            const AnnotationText& m_annotationText;
            
            FTFont* m_font;
            
            const double m_viewportX;
            
            const double m_viewportY;
            
            const double m_viewportZ;
            
            const double m_rotationAngle;
            
            const double m_lineThicknessForViewportHeight;
            
            double m_underlineThickness;
            
            double m_outlineThickness;
            
            /*
             * Bounds relative to origin of first character
             */
            double m_viewportBoundsMinX;
            double m_viewportBoundsMaxX;
            double m_viewportBoundsMinY;
            double m_viewportBoundsMaxY;
            
        private:
            void applyAlignmentsToHorizontalTextStrings();
            void applyAlignmentsToStackedTextStrings();
            void applyAlignmentsToTextStrings();
            void initializeTextPositions();
            void updateTextBounds();
            void splitTextIntoLines();
        };
        
        void drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                                   const TextStringGroup& textStringGroup);
        
        void applyTextColoring(const AnnotationText& annotationText);
        
        void applyBackgroundColoring(const TextStringGroup& textStringGroup);
        
        void setViewportHeight();
        
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
        
        /** Width of the viewport */
        int32_t m_viewportWidth;
        
        /** Height of the viewport */
        int32_t m_viewportHeight;
        
        float m_lineWidthMinimum = 1.0f;
        
        float m_lineWidthMaximum = 5.0f;
        
        static const double s_textMarginSize;
    };
    
#ifdef __FTGL_FONT_TEXT_RENDERER_DECLARE__
    const double FtglFontTextRenderer::s_textMarginSize = 3.0;
#endif // __FTGL_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__FTGL_FONT_TEXT_RENDERER_H__
