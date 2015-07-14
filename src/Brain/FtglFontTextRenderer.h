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

class FTBBox;
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
            
            FontData(const AnnotationText&  annotationText);
            
            ~FontData();
            
            void initialize(const AString& fontFileName);
            
            QByteArray m_fontData;
            
            FTFont* m_font;
            
            bool m_valid;
        };
        
        class TextCell {
        public:
            TextCell(const QString& text,
                     const int32_t row,
                     const int32_t column,
                     const FTBBox& textBounds);
            
            const QString m_text;
            const int32_t m_row;
            const int32_t m_column;
            
            const double m_boundsMinX;
            const double m_boundsMaxX;
            const double m_boundsMinY;
            const double m_boundsMaxY;
            
            double m_width;
            double m_height;
            
            double m_viewportX;
            double m_viewportY;
            double m_viewportZ;
        };
        
        class TextDrawInfo {
        public:
            TextDrawInfo(const AnnotationText& annotationText,
                         const double viewportX,
                         const double viewportY,
                         const double viewportZ,
                         const double rotationAngle);
            
            void addTextCell(const TextCell& textCell);
            
            TextCell* getCellAtRowColumn(const int32_t row,
                                         const int32_t column);
            
            const TextCell* getCellAtRowColumn(const int32_t row,
                                         const int32_t column) const;
            
            void setBounds(const double minX,
                           const double maxX,
                           const double minY,
                           const double maxY);
            
            void getBounds(double bottomLeftOut[3],
                           double bottomRightOut[3],
                           double topRightOut[3],
                           double topLeftOut[3],
                           double rotationPointXYZOut[3]) const;

            const AnnotationText& m_annotationText;
            
            const double m_viewportX;
            const double m_viewportY;
            const double m_viewportZ;
            
            const double m_rotationAngle;
            
            int32_t m_numRows;
            int32_t m_numColumns;
            
            double m_minX;
            double m_maxX;
            double m_minY;
            double m_maxY;
            
            std::vector<TextCell> m_textCells;
        };
        
        void assignTextRowColumnLocations(const AnnotationText& annotationText,
                                       TextDrawInfo& textDrawInfoOut);
        
        void setTextViewportCoordinates(const double viewportX,
                                        const double viewportY,
                                        const double viewportZ,
                                        const AnnotationText& annotationText,
                                        TextDrawInfo& textDrawInfo);
        
        void drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                           const TextDrawInfo& textDrawInfo);
        
        void applyForegroundColoring(const AnnotationText& annotationText);
        
        void applyBackgroundColoring(const TextDrawInfo& textDrawInfo);
        
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
