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
                                                double& widthOut,
                                                double& heightOut);
        
        
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
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
        
        void drawTextAtViewportCoordsInternal(const DepthTestEnum depthTesting,
                                              const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText);
        
        FTFont* getFont(const AnnotationText& annotationText,
                        const bool creatingDefaultFontFlag);
        
        
        class FontData {
        public:
            FontData();
            
            FontData(const AnnotationText&  annotationText,
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
            TextCharacter(const QString& character,
                          const double horizontalAdvance,
                          const double glyphMinX,
                          const double glyphMaxX,
                          const double glyphMinY,
                          const double glyphMaxY);
            
            ~TextCharacter();
            
            void print(const AString& offsetString);
            
            const QString m_character;
            
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
                       FTFont* font);
            
            ~TextString();
            
            void print(const AString& offsetString);
            
            void setGlyphBounds();
            
            void getTextBoundsInViewportCoordinates(double& viewportMinX,
                                                    double& viewportMaxX,
                                                    double& viewportMinY,
                                                    double& viewportMaxY) const;
            
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
                          const double rotationAngle);
            
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
        
        
        
        
        
        
        
        
        
        
        
        
        
        class TextCell {
        public:
            TextCell(const QString& text,
                     const int32_t row,
                     const int32_t column,
                     const double boundsMinX,
                     const double boundsMaxX,
                     const double boundsMinY,
                     const double boundsMaxY);
            
            void getViewportBoundingBox(double& minXOut,
                                        double& maxXOut,
                                        double& minYOut,
                                        double& maxYOut) const;
            
            void print() const;
            
            const QString m_text;
            const int32_t m_row;
            const int32_t m_column;
            
            /*
             * Bounding box that encloses text with 0.0 at the "pen"
             * http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
             */
            const double m_glyphMinX;
            const double m_glyphMaxX;
            const double m_glyphMinY;
            const double m_glyphMaxY;
            
            const double m_width;
            const double m_height;
            
            const double m_centerX;
            const double m_centerY;
            
            double m_viewportX;
            double m_viewportY;
            double m_viewportZ;
        };
        
        class TextMatrix {
        public:
            TextMatrix(const AnnotationText& annotationText,
                       FTFont* font,
                         const double viewportX,
                         const double viewportY,
                         const double viewportZ,
                         const double rotationAngle);
            
        private:
            void splitTextIntoCells();
            
            void setRowWidthsAndColumnHeights();
            
            void applyAlignmentsToTextCoordinates();
            
            float getStackedTextCenterOffset(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             const float columnMinimumX,
                                             const float columnMaximumX);
            
            void positionTextStartingAtViewportCoordinates();
            
            void positionTextInColumn(const int32_t columnIndex,
                                      const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
                                      const float viewportOffsetX);
            
            void positionStackedText();
            
            void setViewportBoundingBox();
            
            void addTextCell(const TextCell& textCell);
            
        public:
            TextCell* getCellAtRowColumn(const int32_t row,
                                         const int32_t column);
            
            const TextCell* getCellAtRowColumn(const int32_t row,
                                         const int32_t column) const;
            
//            void setBounds(const double minX,
//                           const double maxX,
//                           const double minY,
//                           const double maxY);
            
            void getBounds(const double margin,
                           double bottomLeftOut[3],
                           double bottomRightOut[3],
                           double topRightOut[3],
                           double topLeftOut[3],
                           double rotationPointXYZOut[3]) const;

            void print() const;
            
            const AnnotationText& m_annotationText;
            
            FTFont* m_font;
            
            const double m_viewportX;
            const double m_viewportY;
            const double m_viewportZ;
            
            const double m_rotationAngle;
            
            std::vector<double> m_stackedOrientColumnHeights;
            std::vector<double> m_stackedOrientColumnWidths;
            double m_stackedOrientMaximumColumnHeight;

            std::vector<double> m_horizontalOrientRowHeights;
            std::vector<double> m_horizontalOrientRowWidths;
//            double m_horizontalOrientMaximumRowWidth;
            
            int32_t m_numRows;
            int32_t m_numColumns;
            
            
            double m_viewportMinX;
            double m_viewportMaxX;
            double m_viewportMinY;
            double m_viewportMaxY;
            
            std::vector<TextCell> m_textCells;
        };
        
//        void splitTextIntoRowsAndColumns(const AnnotationText& annotationText,
//                                          TextMatrix& textMatrixOut);
//        
//        void setTextViewportCoordinates(const double viewportX,
//                                        const double viewportY,
//                                        const double viewportZ,
//                                        const AnnotationText& annotationText,
//                                        TextMatrix& textMatrix);
        
        void drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                           const TextMatrix& textMatrix);
        
        void drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                                   const TextStringGroup& textStringGroup);
        
        void applyForegroundColoring(const AnnotationText& annotationText);
        
        void applyBackgroundColoring(const TextMatrix& textMatrix);
        
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
        
        /** Height of the viewport */
        int32_t m_viewportHeight;
        
        static const double s_textMarginSize;
    };
    
#ifdef __FTGL_FONT_TEXT_RENDERER_DECLARE__
    const double FtglFontTextRenderer::s_textMarginSize = 3.0;
#endif // __FTGL_FONT_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__FTGL_FONT_TEXT_RENDERER_H__
