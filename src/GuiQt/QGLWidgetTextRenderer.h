#ifndef __QGLWIDGET_TEXT_RENDERER_H__
#define __QGLWIDGET_TEXT_RENDERER_H__

/*
 * QOpenGLWidget does not have renderText() methods.
 * renderText() methods are in deprecated QGLWidget.
 */
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

class QFont;
class QGLWidget;

namespace caret {

    class QGLWidgetTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        QGLWidgetTextRenderer(QGLWidget* glWidget);
        
        virtual ~QGLWidgetTextRenderer();
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                              const AnnotationText& annotationText,
                                              const BrainOpenGLTextRenderInterface::DrawingFlags& flags) override;
        
        virtual void drawTextAtViewportCoords(const double viewportX,
                                              const double viewportY,
                                              const AnnotationText& annotationText,
                                              const BrainOpenGLTextRenderInterface::DrawingFlags& flags) override;
        
        virtual void drawTextAtModelCoordsFacingUser(const double modelX,
                                           const double modelY,
                                           const double modelZ,
                                           const AnnotationText& annotationText,
                                           const BrainOpenGLTextRenderInterface::DrawingFlags& flags) override;
        
        virtual void drawTextInModelSpace(const AnnotationText& annotationText,
                                          const float heightOrWidthForPercentageSizeText,
                                          const float normalVector[3],
                                          const DrawingFlags& flags) override;
        
        virtual void getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                                const double viewportWidth,
                                                const double viewportHeight,
                                                double& widthOut,
                                                double& heightOut) override;
        
        virtual void getBoundsForTextInModelSpace(const AnnotationText& annotationText,
                                                  const float heightOrWidthForPercentageSizeText,
                                                  const DrawingFlags& flags,
                                                  float bottomLeftOut[3],
                                                  float bottomRightOut[3],
                                                  float topRightOut[3],
                                                  float topLeftOut[3],
                                                  float underlineStartOut[3],
                                                  float underlineEndOut[3]) override;
        
        virtual void getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                      const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                                      const double viewportX,
                                                      const double viewportY,
                                                      const double viewportZ,
                                                      const double viewportWidth,
                                                      const double viewportHeight,
                                                      double bottomLeftOut[3],
                                                      double bottomRightOut[3],
                                                      double topRightOut[3],
                                                      double topLeftOut[3]) override;
        
        virtual void getBoundsWithoutMarginForTextAtViewportCoords(const AnnotationText& annotationText,
                                                                   const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                                                   const double viewportX,
                                                                   const double viewportY,
                                                                   const double viewportZ,
                                                                   const double viewportWidth,
                                                                   const double viewportHeight,
                                                                   double bottomLeftOut[3],
                                                                   double bottomRightOut[3],
                                                                   double topRightOut[3],
                                                                   double topLeftOut[3]) override;
        
        virtual bool isValid() const;

        virtual AString getName() const;


        // ADD_NEW_METHODS_HERE

    private:
        class FontData {
        public:
            FontData();
            
            FontData(const AnnotationText&  annotationText,
                     const int32_t viewportWidth,
                     const int32_t viewportHeight);
            
            ~FontData();
            
            void initialize(const AString& fontFileName);
            
            QFont* m_font;
            
            bool m_valid;
        };
        
        struct CharInfo {
            CharInfo(const QString& theChar,
                     double x,
                     double y) : m_char(theChar), m_x(x), m_y(y) { }
            
             QString m_char;
             double m_x;
             double m_y;
        };
        
        QGLWidgetTextRenderer(const QGLWidgetTextRenderer&);

        QGLWidgetTextRenderer& operator=(const QGLWidgetTextRenderer&);
        
        QFont* findFont(const AnnotationText& annotationText,
                        const bool creatingDefaultFontFlag);
        
        void getVerticalTextCharInfo(const AnnotationText& annotationText,
                                     const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                     double& xMinOut,
                                     double& xMaxOut,
                                     double& heightOut,
                                     std::vector<CharInfo>& charInfoOut);
        
        void drawHorizontalTextAtWindowCoords(const double windowX,
                                              const double windowY,
                                              const AnnotationText& annotationText,
                                              const BrainOpenGLTextRenderInterface::DrawingFlags& flags);
        
        void drawVerticalTextAtWindowCoords(const double windowX,
                                            const double windowY,
                                            const AnnotationText& annotationText,
                                            const BrainOpenGLTextRenderInterface::DrawingFlags& flags);
        
        void applyForegroundColoring(const AnnotationText& annotationText);
        
        void applyBackgroundColoring(const AnnotationText& annotationText,
                                     const double textBoundsBox[4]);
        
        void setViewportHeight();
        
        QGLWidget* m_glWidget;
        
        /**
         * The default font.  DO NOT delete it since it points to
         * a font in "m_fontNameToFontMap".
         */
        QFont* m_defaultFont;
        
        /**
         * Map for caching font
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
        
        /** Height of the viewport */
        int32_t m_viewportHeight;
        
        /** Width of the viewport */
        int32_t m_viewportWidth;
        
        // ADD_NEW_MEMBERS_HERE

        static const double s_textMarginSize;
    };
    
#ifdef __QT_OPEN_G_L_TEXT_RENDERER_DECLARE__
    const double QGLWidgetTextRenderer::s_textMarginSize = 2.0;
#endif // __QT_OPEN_G_L_TEXT_RENDERER_DECLARE__

} // namespace

#endif // not - WORKBENCH_USE_QT5_QOPENGL_WIDGET

#endif  //__QT_OPEN_G_L_TEXT_RENDERER_H__
