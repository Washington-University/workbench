
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

/**
 * Qt licence is included since some of its GLWidget API is copied.
 */
/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtOpenGL module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#define __FTGL_FONT_TEXT_RENDERER_DECLARE__
#include "FtglFontTextRenderer.h"
#undef __FTGL_FONT_TEXT_RENDERER_DECLARE__

#include <cmath>
#include <iostream>
#include <limits>

#include <QFile>
#include <QStringList>

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"

#ifdef HAVE_FREETYPE
#include <FtglConfig.h>
#include <FTGL/ftgl.h>
using namespace FTGL;
#endif // HAVE_FREETYPE

using namespace caret;

static const bool debugPrintFlag =  false;


/**
 * \class caret::FtglFontTextRenderer 
 * \brief Text rendering using FTGL
 * \ingroup Brain
 *
 * Draws text using the FTGL library.  
 *
 * FTGL's Texture font is used for drawing text.  
 * The texture font is positioned using the current OpenGL
 * model view matrix which allows rotation and scaling.
 * 
 * There are other font types such as Pixmap font.  The
 * pixmap font was used, but it cannot be rotated nor
 * scaled.  In addition, the pixmmap font drawing
 * requires a raster position and if the raster position 
 * is slightly outside the viewport all text is clipped.
 */

/**
 * Constructor.
 */
FtglFontTextRenderer::FtglFontTextRenderer()
: BrainOpenGLTextRenderInterface()
{
    m_defaultFont = NULL;
#ifdef HAVE_FREETYPE
    AnnotationText defaultAnnotationText;
    defaultAnnotationText.setFontSize(AnnotationFontSizeEnum::SIZE14);
    defaultAnnotationText.setFont(AnnotationFontNameEnum::VERA);
    defaultAnnotationText.setItalicEnabled(false);
    defaultAnnotationText.setBoldEnabled(false);
    defaultAnnotationText.setUnderlineEnabled(false);
    m_defaultFont = getFont(defaultAnnotationText,
                            true);
#endif // HAVE_FREETYPE
    m_depthTestingStatus = DEPTH_TEST_NO;
}

/**
 * Destructor.
 */
FtglFontTextRenderer::~FtglFontTextRenderer()
{
#ifdef HAVE_FREETYPE
    for (FONT_MAP_ITERATOR iter = m_fontNameToFontMap.begin();
         iter != m_fontNameToFontMap.end();
         iter++) {
        delete iter->second;
    }
    m_fontNameToFontMap.clear();
    
    /*
     * Do not delete "m_defaultFont" since it points to a font
     * in m_fontNameToFontMap.  Doing so would cause
     * a double delete.
     */
#endif // HAVE_FREETYPE
}

/**
 * @return The font system is valid.
 */
bool
FtglFontTextRenderer::isValid() const
{
    return (m_defaultFont != NULL);
}

/*
 * Get the font with the given font attributes.
 * If the font is not created, return the default font.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param creatingDefaultFontFlag
 *    True if creating the default font.
 * @return
 *    The FTGL font.  If there are errors this value will
 *    be NULL.
 */
FTFont*
FtglFontTextRenderer::getFont(const AnnotationText& annotationText,
                              const bool creatingDefaultFontFlag)
{
#ifdef HAVE_FREETYPE
    const AString fontName = annotationText.getFontRenderingEncodedName(m_viewportHeight);
    
    /*
     * Has the font already has been created?
     */
    FONT_MAP_ITERATOR fontIter = m_fontNameToFontMap.find(fontName);
    if (fontIter != m_fontNameToFontMap.end()) {
        FontData* fontData = fontIter->second;
        CaretAssert(fontData);
        return fontData->m_font;
    }
    
    /*
     * Create and save the font
     */
    FontData* fontData = new FontData(annotationText,
                                      m_viewportHeight);
    if (fontData->m_valid) {
        /*
         * Request font is valid.
         */
        m_fontNameToFontMap.insert(std::make_pair(fontName,
                                                  fontData));
        CaretLogFine("Created font with encoded name "
                     + fontName);
        return fontData->m_font;
    }
    else {
        /*
         * Error creating font
         */
        delete fontData;
        fontData = NULL;
        
        /*
         * Issue a message about failure to create font but
         * don't print same message more than once.
         */
        if (std::find(m_failedFontNames.begin(),
                      m_failedFontNames.end(),
                      fontName) == m_failedFontNames.end()) {
            m_failedFontNames.insert(fontName);
            CaretLogSevere("Failed to create font with encoded name "
                           + fontName);
        }
    }
    
    /*
     * Were we trying to create the default font?
     */
    if (creatingDefaultFontFlag) {
        return NULL;
    }
    
    /*
     * Failed so use the default font.
     */
    return m_defaultFont;
    
#else  // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but FTGL is not valid.");
    return NULL;
#endif // HAVE_FREETYPE
}




/**
 * Draw the text piceces at their assigned viewport coordinates.
 *
 * @param annotationText
 *     Annotation text and attributes.
 * @param textMatrix
 *     Text broken up into cells with viewport coordinates assigned.
 *
 */
void
FtglFontTextRenderer::drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                                            const TextStringGroup& textStringGroup)
{
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(annotationText,
                           false);
    if (! font) {
        return;
    }
    
    if (annotationText.getText().isEmpty()) {
        return;
    }
    
    saveStateOfOpenGL();
    
    /*
     * Depth testing ?
     */
    switch (m_depthTestingStatus) {
        case DEPTH_TEST_NO:
            glDisable(GL_DEPTH_TEST);
            break;
        case DEPTH_TEST_YES:
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    /*
     * Get the viewport
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    /*
     * Get depth range for orthographic projection.
     */
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);
    
    /*
     * Set the orthographic projection so that its origin is in the bottom
     * left corner.  It needs to be there since we are drawing in window
     * coordinates.  We do not know the true size of the window but that
     * is okay since we can set the orthographic view so that the bottom
     * left corner is the origin and the top right corner is the top
     * right corner of the user's viewport.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,
            (viewport[2]),
            0,
            (viewport[3]),
            depthRange[0], //-1,
            depthRange[1]); //1);
    
    /*
     * Viewing projection is just the identity matrix since
     * we are drawing in window coordinates.
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const bool drawCrosshairsAtFontStartingCoordinate = false;
    if (debugPrintFlag
        || drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.0);
        glPushMatrix();
        glTranslatef(textStringGroup.m_viewportX, textStringGroup.m_viewportY, 0.0);
        const float xy = 500.0;
        glBegin(GL_LINES);
        glVertex2f(-xy, 0.0);
        glVertex2f( xy, 0.0);
        glVertex2f(0.0, -xy);
        glVertex2f(0.0,  xy);
        glEnd();
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3], rotationPointXYZ[3];
    textStringGroup.getViewportBounds(s_textMarginSize,
                         bottomLeft, bottomRight, topRight, topLeft, rotationPointXYZ);
    
    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(textStringGroup);
    applyForegroundColoring(annotationText);
    
    const float rotationAngle = annotationText.getRotationAngle();
    if (rotationAngle != 0.0){
        glTranslated(rotationPointXYZ[0], rotationPointXYZ[1], rotationPointXYZ[2]);
        glRotated(rotationAngle, 0.0, 0.0, -1.0);
        
        for (std::vector<TextString*>::const_iterator iter = textStringGroup.m_textStrings.begin();
             iter != textStringGroup.m_textStrings.end();
             iter++) {
            const TextString* ts = *iter;
            
            double x = ts->m_viewportX;
            double y = ts->m_viewportY;
            double z = ts->m_viewportZ;
            
            for (std::vector<TextCharacter*>::const_iterator charIter = ts->m_characters.begin();
                 charIter != ts->m_characters.end();
                 charIter++) {
                const TextCharacter* tc = *charIter;
                x += tc->m_offsetX;
                y += tc->m_offsetY;
                z += tc->m_offsetZ;
                
                const double offsetX = x - rotationPointXYZ[0];
                const double offsetY = y - rotationPointXYZ[1];
                const double offsetZ = z - rotationPointXYZ[2];
                
                glPushMatrix();
                glTranslated(offsetX,
                             offsetY,
                             offsetZ);
                font->Render(&tc->m_character,
                             1);
                glPopMatrix();
            }
        }
    }
    else {
        for (std::vector<TextString*>::const_iterator iter = textStringGroup.m_textStrings.begin();
             iter != textStringGroup.m_textStrings.end();
             iter++) {
            const TextString* ts = *iter;
            glPushMatrix();
            glLoadIdentity();
            
            glTranslated(ts->m_viewportX,
                         ts->m_viewportY,
                         ts->m_viewportZ);
            
            for (std::vector<TextCharacter*>::const_iterator charIter = ts->m_characters.begin();
                 charIter != ts->m_characters.end();
                 charIter++) {
                const TextCharacter* tc = *charIter;
                
                glTranslated(tc->m_offsetX,
                             tc->m_offsetY,
                             tc->m_offsetZ);
                font->Render(&tc->m_character,
                             1);
            }
            
            glPopMatrix();
        }
    }
    
    glPopMatrix();
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is DISABLED when drawing text with this method.
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
FtglFontTextRenderer::drawTextAtViewportCoords(const double viewportX,
                                               const double viewportY,
                                               const AnnotationText& annotationText)
{
    setViewportHeight();
    
    drawTextAtViewportCoordsInternal(DEPTH_TEST_NO,
                                     viewportX,
                                     viewportY,
                                     0.0,
                                     annotationText);
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED when drawing text with this method.
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param viewportZ
 *     Viewport Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
FtglFontTextRenderer::drawTextAtViewportCoords(const double viewportX,
                                               const double viewportY,
                                               const double viewportZ,
                                               const AnnotationText& annotationText)
{
    setViewportHeight();
    
    drawTextAtViewportCoordsInternal(DEPTH_TEST_YES,
                                     viewportX,
                                     viewportY,
                                     viewportZ,
                                     annotationText);
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text and the
 * depth testing method.
 *
 * @param depthTesting
 *     Type of depth testing when drawing text.
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param viewportZ
 *     Viewport Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
FtglFontTextRenderer::drawTextAtViewportCoordsInternal(const DepthTestEnum depthTesting,
                                      const double viewportX,
                                      const double viewportY,
                                      const double viewportZ,
                                      const AnnotationText& annotationText)
{
    if (annotationText.getText().isEmpty()) {
        return;
    }
    
    FTFont* font = getFont(annotationText, false);
    if ( ! font) {
        return;
    }
    
    m_depthTestingStatus = depthTesting;
    
    TextStringGroup tsg(annotationText,
                        font,
                        viewportX,
                        viewportY,
                        viewportZ,
                        annotationText.getRotationAngle());
    //tsg.print();
    drawTextAtViewportCoordinatesInternal(annotationText,
                                          tsg);
}



/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.  A margin is included around the text.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param viewportX
 *    Viewport X-coordinate.
 * @param viewportY
 *    Viewport Y-coordinate.
 * @param viewportZ
 *    Viewport Z-coordinate.
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 */
void
FtglFontTextRenderer::getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                     const double viewportX,
                                                     const double viewportY,
                                                     const double viewportZ,
                                                       double bottomLeftOut[3],
                                                       double bottomRightOut[3],
                                                       double topRightOut[3],
                                                       double topLeftOut[3])
{
    setViewportHeight();
    
    FTFont* font = getFont(annotationText, false);
    if ( ! font) {
        return;
    }
    
    TextStringGroup textStringGroup(annotationText,
                          font,
                              viewportX,
                              viewportY,
                              viewportZ,
                              annotationText.getRotationAngle());
    
    double rotationPointXYZ[3];
    textStringGroup.getViewportBounds(s_textMarginSize,
                                      bottomLeftOut,
                                      bottomRightOut,
                                      topRightOut,
                                      topLeftOut,
                                      rotationPointXYZ);
}




/**
 * Apply the background coloring by drawing a rectangle in the background
 * color that encloses the text.  If the background color is
 * invalid (alpha => 0), no action is taken.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 */
void
FtglFontTextRenderer::applyBackgroundColoring(const TextStringGroup& textStringGroup)
{
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3], rotationPointXYZ[3];
    textStringGroup.getViewportBounds(s_textMarginSize, bottomLeft, bottomRight, topRight, topLeft, rotationPointXYZ);
    
    float backgroundColor[4];
    textStringGroup.m_annotationText.getBackgroundColorRGBA(backgroundColor);

    if (backgroundColor[3] > 0.0) {
        //        const AString bg("Background for \"" + annotationText.getText() + ": BL="
        //                         + AString::fromNumbers(bottomLeftOut, 3, ",") + "  BR="
        //                         + AString::fromNumbers(bottomRightOut, 3, ",") + "  TR="
        //                         + AString::fromNumbers(topRightOut, 3, ",") + "  TL="
        //                         + AString::fromNumbers(topLeftOut, 3, ","));
        //        std::cout << qPrintable(bg) << std::endl;
        glColor4fv(backgroundColor);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3dv(bottomLeft);
        glVertex3dv(bottomRight);
        glVertex3dv(topLeft);
        glVertex3dv(topRight);
        glEnd();
    }
}

/**
 * Apply the foreground color.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 */
void
FtglFontTextRenderer::applyForegroundColoring(const AnnotationText& annotationText)
{
    float foregroundColor[4];
    annotationText.getForegroundColorRGBA(foregroundColor);
    glColor4fv(foregroundColor);
}

/**
 * Set the height of the viewport.  This method must be called
 * at the beginning of all public methods.
 */
void
FtglFontTextRenderer::setViewportHeight()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    m_viewportHeight = viewport[3];
}

/**
 * Get the estimated width and height of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text for width and height estimation.
 * @param widthOut
 *    Estimated width of text.
 * @param heightOut
 *    Estimated height of text.
 */
void
FtglFontTextRenderer::getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                 double& widthOut,
                                                 double& heightOut)
{
    setViewportHeight();
    
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
    getBoundsForTextAtViewportCoords(annotationText, 0.0, 0.0, 0.0, bottomLeft, bottomRight, topRight, topLeft);
    
    widthOut  = MathFunctions::distance3D(bottomLeft, bottomRight);
    heightOut = MathFunctions::distance3D(topLeft, bottomLeft);
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED when drawing text with this method.
 *
 * @param modelX
 *     Model X-coordinate.
 * @param modelY
 *     Model Y-coordinate.
 * @param modelZ
 *     Model Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
FtglFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                          const double modelY,
                                          const double modelZ,
                                          const AnnotationText& annotationText)
{
    setViewportHeight();
    
    m_depthTestingStatus = DEPTH_TEST_YES;

    GLdouble modelMatrix[16];
    GLdouble projectionMatrix[16];
    GLint viewport[4];
    
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,
                 projectionMatrix);
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    /*
     * Project model coordinate to a window coordinate.
     */
    GLdouble windowX, windowY, windowZ;
    if (gluProject(modelX, modelY, modelZ,
                   modelMatrix, projectionMatrix, viewport,
                   &windowX, &windowY, &windowZ) == GL_TRUE) {
        
        /*
         * From OpenGL Programming Guide 3rd Ed, p 133:
         *
         * If the near value is 1.0 and the far value is 3.0,
         * objects must have z-coordinates between -1.0 and -3.0 in order to be visible.
         * So, negative the Z-value to be negative.
         */
        windowZ = -windowZ;
        
        /*
         * Convert window coordinate to viewport coordinatde
         */
        const double x = windowX - viewport[0];
        const double y = windowY - viewport[1];
        drawTextAtViewportCoordsInternal(DEPTH_TEST_YES,
                                         x,
                                         y,
                                         windowZ,
                                         annotationText);
        
    }
    else {
        CaretLogSevere("gluProject() failed for drawing text at model coordinates.");
    }
}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
FtglFontTextRenderer::saveStateOfOpenGL()
{
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * Restore the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_restore_gl_state().
 */
void
FtglFontTextRenderer::restoreStateOfOpenGL()
{
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    glPopClientAttrib();
}

/**
 * Constructs invalid font data.
 */
FtglFontTextRenderer::FontData::FontData()
{
    m_valid    = false;
    m_font     = NULL;
}

/**
 * Constructs a font with the given attributes.
 * Called should verify that this instance is valid (construction was successful).
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param viewportHeight
 *    Height of the viewport in which text is drawn.
 */
FtglFontTextRenderer::FontData::FontData(const AnnotationText&  annotationText,
                                         const int32_t viewportHeight)
{
    m_valid    = false;
    m_font     = NULL;
    
#ifdef HAVE_FREETYPE
    const AnnotationFontNameEnum::Enum fontName = annotationText.getFont();
    
    AString fontFileName = AnnotationFontNameEnum::getResourceFontFileName(fontName);
    if (annotationText.isBoldEnabled()
        && annotationText.isItalicEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceBoldItalicFontFileName(fontName);
    }
    else if (annotationText.isBoldEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceBoldFontFileName(fontName);
        
    }
    else if (annotationText.isItalicEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceItalicFontFileName(fontName);
    }
    
    CaretAssert( ! fontFileName.isEmpty());
    
    /*
     * Open the font file in the Workbench resources
     */
    QFile file(fontFileName);
    if (file.open(QFile::ReadOnly)) {
        /*
         * Read all data which MUST remain valid until the FTGL
         * font is no longer used.
         */
        m_fontData = file.readAll();
        const size_t numBytes = m_fontData.size();
        if (numBytes > 0) {
            /*
             * Create the FTGL font.
             */
            m_font = new FTTextureFont((const unsigned char*)m_fontData.data(),
                                       numBytes);
            
            CaretAssert(m_font);
            
            /*
             * Font created successfully ?
             */
            if ( ! m_font->Error()) {
                /*
                 * Font size successful ?
                 */
                const int32_t fontSizeInt = annotationText.getFontSizeForDrawing(viewportHeight);
                if (m_font->FaceSize(fontSizeInt)) {
                    m_valid = true;
                    
                    CaretLogFine("Created font size="
                                 + AnnotationFontSizeEnum::toName(annotationText.getFontSize())
                                 + " scaled-size="
                                 + AString::number(fontSizeInt)
                                 + " from font file "
                                 + file.fileName());
                }
                else {
                    CaretLogSevere("Error creating font size "
                                   + AString::number(fontSizeInt)
                                   + " from font file "
                                   + file.fileName());
                }
            }
            else {
                CaretLogSevere("Error creating FTGL Font from "
                               + file.fileName()
                               + ".  Error code from FreeType is "
                               + AString::number(m_font->Error()));
            }
        }
        else {
            CaretLogSevere("Error reading data for FTGL Font from "
                           + file.fileName()
                           + " error: "
                           + file.errorString());
        }
    }
    else {
        CaretLogSevere("Unable to open FTGL Font File "
                       + file.fileName()
                       + " error: "
                       + file.errorString());
    }
    
    if ( ! m_valid) {
        m_fontData.clear();
        
        if (m_font != NULL) {
            delete m_font;
            m_font = NULL;
        }
    }
#endif // HAVE_FREETYPE
}

/**
 * Destructs font data.
 */
FtglFontTextRenderer::FontData::~FontData()
{
#ifdef HAVE_FREETYPE
    if (m_font != NULL) {
        delete m_font;
        m_font = NULL;
    }
#endif // HAVE_FREETYPE
}

/**
 * @return Name of the text renderer.
 */
AString
FtglFontTextRenderer::getName() const
{
    return "FTGL Text Renderer";
}






/* ================================================================================== */

/**
 * Constructor.
 * 
 * @param character
 *    The character for this instance.
 * @param horizontalAdvance
 *    The advance used when drawing the string in a horizontal orientation.
 * @param glyphMinX
 *    Minimum X relative to the character's origin form FTGL/Freetype.
 * @param glyphMaxX
 *    Maximum X relative to the character's origin form FTGL/Freetype.
 * @param glyphMinY
 *    Minimum Y relative to the character's origin form FTGL/Freetype.
 * @param glyphMaxY
 *    Maximum Y relative to the character's origin form FTGL/Freetype.
 */
FtglFontTextRenderer::TextCharacter::TextCharacter(const wchar_t& character,
                                                   const double horizontalAdvance,
                                                   const double glyphMinX,
                                                   const double glyphMaxX,
                                                   const double glyphMinY,
                                                   const double glyphMaxY)
: m_character(character),
m_horizontalAdvance(horizontalAdvance),
m_glyphMinX(glyphMinX),
m_glyphMaxX(glyphMaxX),
m_glyphMinY(glyphMinY),
m_glyphMaxY(glyphMaxY),
m_offsetX(0.0),
m_offsetY(0.0),
m_offsetZ(0.0)
{
    
}


/**
 * Destructor.
 */
FtglFontTextRenderer::TextCharacter::~TextCharacter()
{
    
}

/**
 * Print information about this string using the given offset.
 *
 * @param offsetString
 *     Offset prepended to each line that is printed.
 */
void
FtglFontTextRenderer::TextCharacter::print(const AString& offsetString)
{
    const QString msg(offsetString
                      + "Char: "
                      + QString::fromWCharArray(&m_character,
                                                1)
                      + " horizAdvance="
                      + AString::number(m_horizontalAdvance)
                      + " minX="
                      + AString::number(m_glyphMinX)
                      + " maxX="
                      + AString::number(m_glyphMaxX)
                      + " minY="
                      + AString::number(m_glyphMinY)
                      + " maxY="
                      + AString::number(m_glyphMaxY));
    const QString msg2(offsetString
                       + " Offset x="
                       + AString::number(m_offsetX)
                       + " y="
                       + AString::number(m_offsetY)
                       + " z="
                       + AString::number(m_offsetZ));
    
    std::cout << qPrintable(msg) << std::endl;
    std::cout << qPrintable(msg2) << std::endl;
}


/* ================================================================================== */

/**
 * Constructor
 * 
 * @param textString
 *     The text string.
 * @parm orientation
 *     Orientation of the text string.
 * @param font
 *     Font for drawing the text string.
 */
FtglFontTextRenderer::TextString::TextString(const QString& textString,
                                             const AnnotationTextOrientationEnum::Enum orientation,
                                             FTFont* font)
: m_viewportX(0.0),
m_viewportY(0.0),
m_viewportZ(0.0),
m_stringGlyphsMinX(0.0),
m_stringGlyphsMaxX(0.0),
m_stringGlyphsMinY(0.0),
m_stringGlyphsMaxY(0.0)
{
    /*
     * Split the string into individual characters.
     */
    const int32_t numChars = textString.length();
    for (int32_t i = 0; i < numChars; i++) {
        const std::wstring theWideCharStr = textString.mid(i, 1).toStdWString();
        const wchar_t theWideChar = theWideCharStr.at(0);
        FTBBox bbox = font->BBox(theWideCharStr.c_str(),
                                 theWideCharStr.length());
        
        /*
         * A space character has a valid horizontal advance.
         * BUT, the bounds of the space character are all zero and since
         * the bounds are used for stacked text vertical advance we need
         * some value so use the bounds for a lowercase 'o'.
         *
         * The L indicates a wide character (wchar_t)
         */
        if ((theWideChar == L' ')
            && (orientation == AnnotationTextOrientationEnum::STACKED)) {
            bbox = font->BBox("o");
        }

        double advanceValue = 0.0;
        if (i < (numChars - 1)) {
            const std::wstring nextWideCharStr = textString.mid(i + 1, 1).toStdWString();
            const wchar_t nextWideChar = nextWideCharStr.at(0);
            advanceValue = font->Advance(theWideChar,
                                         nextWideChar);
        }
        
        TextCharacter* tc = new TextCharacter(theWideChar,
                                              advanceValue,
                                              bbox.Lower().Xf(),
                                              bbox.Upper().Xf(),
                                              bbox.Lower().Yf(),
                                              bbox.Upper().Yf());
        
        m_characters.push_back(tc);
    }
    
    /*
     * Set the offset for each character in this text string
     */
    initializeTextCharacterOffsets(orientation);
    
    /*
     * Set the bounds of the characters in this string.
     */
    setGlyphBounds();
}

/**
 * Destructor.
 */
FtglFontTextRenderer::TextString::~TextString()
{
    for (std::vector<TextCharacter*>::iterator iter = m_characters.begin();
         iter != m_characters.end();
         iter++) {
        delete *iter;
    }
    m_characters.clear();
}

/**
 * Initialize the offset of each character from 
 * its preceding character.
 *
 * @param orientation
 *     Orientation of the text string.
 */
void
FtglFontTextRenderer::TextString::initializeTextCharacterOffsets(const AnnotationTextOrientationEnum::Enum orientation)
{
    bool stackedTextFlag = false;
    switch (orientation) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            break;
        case AnnotationTextOrientationEnum::STACKED:
            stackedTextFlag = true;
            break;
    }
    
    double stringMinX =  std::numeric_limits<float>::max();
    double stringMaxX = -std::numeric_limits<float>::max();
    
    const float verticalSpacing = s_textMarginSize * 2.0;
    
    /*
     * For each character, set its offset from the previous character
     */
    const int numChars = static_cast<int32_t>(m_characters.size());
    for (int32_t iChar = 0; iChar < numChars; iChar++) {
        CaretAssertVectorIndex(m_characters, iChar);
        TextCharacter* tc = m_characters[iChar];
        
        double offsetX = 0.0;
        double offsetY = 0.0;
        double offsetZ = 0.0;
        if (iChar > 0) {
            CaretAssertVectorIndex(m_characters, iChar - 1);
            const TextCharacter* prevChar = m_characters[iChar - 1];

            if (stackedTextFlag) {
                double offsetY1 = prevChar->m_glyphMinY;
                double offsetY2 = -verticalSpacing;
                double offsetY3 = -tc->m_glyphMaxY;
                
                offsetY = (offsetY1 + offsetY2 + offsetY3);
            }
            else {
                offsetX = prevChar->m_horizontalAdvance;
            }
        }
        
        tc->m_offsetX = offsetX;
        tc->m_offsetY = offsetY;
        tc->m_offsetZ = offsetZ;
        
        stringMinX = std::min(stringMinX,
                              tc->m_glyphMinX);
        stringMaxX = std::max(stringMaxX,
                              tc->m_glyphMaxX);
    }
    
    /*
     * When drawing stacked text, the characters
     * need their X offset adjusted so that they
     * are horizontally aligned in the center of
     * the "column" containing the text.
     */
    if (stackedTextFlag) {
        /*
         * X-coordinate of character in the vertical column
         * so that the characters are centered horizontally
         * in the column.
         */
        std::vector<double> characterColumnCoordX(numChars, 0.0);
        
        const double columnWidth = stringMaxX - stringMinX;
        if (columnWidth > 0.0) {
            for (int32_t iChar = 0; iChar < numChars; iChar++) {
                CaretAssertVectorIndex(m_characters, iChar);
                TextCharacter* tc = m_characters[iChar];
                
                const double characterWidth = tc->m_glyphMaxX - tc->m_glyphMinX;
                if (characterWidth > 0.0) {
                    const double leftAndRightPadding = columnWidth - characterWidth;
                    const double leftPadding = leftAndRightPadding / 2.0;
                        const double characterX = leftPadding - tc->m_glyphMinX;
//                        std::cout << "Char " << qPrintable(tc->m_character
//                                                           + ": "
//                                                           + " CharMinX="
//                                                           + QString::number(tc->m_glyphMinX)
//                                                           + " CharMaxX="
//                                                           + QString::number(tc->m_glyphMaxX)
//                                                           + " CharWidth="
//                                                           + QString::number(characterWidth)
//                                                           + " Padding="
//                                                           + QString::number(leftPadding)
//                                                           + " CharacterX="
//                                                           + QString::number(characterX)) << std::endl;
                    if (leftPadding >= -0.01) {
                        
                        CaretAssertVectorIndex(characterColumnCoordX, iChar);
                        characterColumnCoordX[iChar] = characterX;
                    }
                    else {
                        CaretLogSevere("Text Character ("
                                       + QString::fromWCharArray(&tc->m_character,
                                                                 1)
                                       + ") has invalid left padding="
                                       + QString::number(leftPadding));
                    }
                }
            }
            
            /*
             * Set the character offsets.
             * SECOND and additional characters are offset from
             * the immediately previous character
             */
            for (int32_t iChar = 0; iChar < numChars; iChar++) {
                CaretAssertVectorIndex(m_characters, iChar);
                TextCharacter* tc = m_characters[iChar];
                if (iChar >= 1) {
                    CaretAssertVectorIndex(m_characters, iChar - 1);
                    CaretAssertVectorIndex(m_characters, iChar);
                    tc->m_offsetX = characterColumnCoordX[iChar] - characterColumnCoordX[iChar - 1];
                }
                else {
                    CaretAssertVectorIndex(m_characters, iChar);
                    tc->m_offsetX = characterColumnCoordX[iChar];
                }
            }
        }
    }
}

/**
 * Print information about this string using the given offset.
 *
 * @param offsetString
 *     Offset prepended to each line that is printed.
 */
void
FtglFontTextRenderer::TextString::print(const AString& offsetString)
{
    QString msg(offsetString
                + "Text String Bounds: minx="
                + QString::number(m_stringGlyphsMinX)
                + " maxx=" + QString::number(m_stringGlyphsMaxX)
                + " miny=" + QString::number(m_stringGlyphsMinY)
                + " maxy=" + QString::number(m_stringGlyphsMaxY));
    std::cout << qPrintable(msg) << std::endl;
    
    const QString doubleOffset(offsetString + offsetString);
    
    for (std::vector<TextCharacter*>::iterator iter = m_characters.begin();
         iter != m_characters.end();
         iter++) {
        TextCharacter* tc = *iter;
        tc->print(doubleOffset);
    }
}

/**
 * Set the bounds of the glyphs for the text string.
 * The origin is at the first character.
 */
void
FtglFontTextRenderer::TextString::setGlyphBounds()
{
    m_stringGlyphsMinX =  std::numeric_limits<float>::max();
    m_stringGlyphsMaxX = -std::numeric_limits<float>::max();
    m_stringGlyphsMinY =  std::numeric_limits<float>::max();
    m_stringGlyphsMaxY = -std::numeric_limits<float>::max();
    
    double x = 0.0;
    double y = 0.0;
    
    for (std::vector<TextCharacter*>::iterator iter = m_characters.begin();
         iter != m_characters.end();
         iter++) {
        TextCharacter* tc = *iter;
        x += tc->m_offsetX;
        y += tc->m_offsetY;
        
        const double left   = x + tc->m_glyphMinX;
        const double right  = x + tc->m_glyphMaxX;
        const double bottom = y + tc->m_glyphMinY;
        const double top    = y + tc->m_glyphMaxY;
        
        m_stringGlyphsMinX = std::min(m_stringGlyphsMinX,
                                left);
        m_stringGlyphsMaxX = std::max(m_stringGlyphsMaxX,
                                right);
        m_stringGlyphsMinY = std::min(m_stringGlyphsMinY,
                                bottom);
        m_stringGlyphsMaxY = std::max(m_stringGlyphsMaxY,
                                top);
    }
}

/**
 * Get the bounds of the text string in viewport coordinates.
 *
 * @param viewportMinX
 *     Viewport minimum X.
 * @param viewportMaxX
 *     Viewport maximum X.
 * @param viewportMinY
 *     Viewport minimum Y.
 * @param viewportMaxY
 *     Viewport minimum Y.
 */
void
FtglFontTextRenderer::TextString::getTextBoundsInViewportCoordinates(double& viewportMinX,
                                                                     double& viewportMaxX,
                                                                     double& viewportMinY,
                                                                     double& viewportMaxY) const
{
    viewportMinX = m_viewportX + m_stringGlyphsMinX;
    viewportMaxX = m_viewportX + m_stringGlyphsMaxX;
    viewportMinY = m_viewportY + m_stringGlyphsMinY;
    viewportMaxY = m_viewportY + m_stringGlyphsMaxY;
}


/* ================================================================================== */

/**
 * Constructor for a text string group.
 *
 * @param annotationText
 *    The text annotation.
 * @param font
 *    Font used for drawing the annotation.
 * @param viewportX
 *    X-coordinate in the viewport.
 * @param viewportY
 *    Y-coordinate in the viewport.
 * @param viewportZ
 *    Z-coordinate in the viewport.
 * @param rotationAngle
 *    Rotation angle for the text.
 */
FtglFontTextRenderer::TextStringGroup::TextStringGroup(const AnnotationText& annotationText,
                                                       FTFont* font,
                                                       const double viewportX,
                                                       const double viewportY,
                                                       const double viewportZ,
                                                       const double rotationAngle)
: m_annotationText(annotationText),
m_font(font),
m_viewportX(viewportX),
m_viewportY(viewportY),
m_viewportZ(viewportZ),
m_rotationAngle(rotationAngle),
m_viewportBoundsMinX(0.0),
m_viewportBoundsMaxX(0.0),
m_viewportBoundsMinY(0.0),
m_viewportBoundsMaxY(0.0)
{
    CaretAssert(font);
    
    /*
     * Each row (horizontal text) or column (vertical text) is
     * separated by a newline character
     */
    QStringList textList = m_annotationText.getText().split('\n',
                                                            QString::KeepEmptyParts);
    const int32_t textListSize = textList.size();

    for (int32_t i = 0; i < textListSize; i++) {
        TextString* ts = new TextString(textList.at(i),
                                        annotationText.getOrientation(),
                                        font);
        m_textStrings.push_back(ts);
    }
    
    initializeTextPositions();
    
    updateTextBounds();
    
    applyAlignmentsToTextStrings();

    /*
     * Alignment moves text so bounds need to be updated
     */
    updateTextBounds();
}

/**
 * Destructor.
 */
FtglFontTextRenderer::TextStringGroup::~TextStringGroup()
{
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        delete *iter;
    }
    m_textStrings.clear();
}

/**
 * Get the bounds of the all text that is drawn.  The
 * bounds is used for selection and the background color.
 *
 * @param margin
 *     Margin added around sides
 * @param bottomLeftOut
 *     Coordinate of bottom left.
 * @param bottomRightOut
 *     Coordinate of bottom right.
 * @param topRightOut
 *     Coordinate of top right.
 * @param topLeftOut
 *     Coordinate of top left.
 * @param rotationPointXYZOut
 *     Output containing rotation point for bounds.
 */
void
FtglFontTextRenderer::TextStringGroup::getViewportBounds(const double margin,
                                                 double bottomLeftOut[3],
                                                 double bottomRightOut[3],
                                                 double topRightOut[3],
                                                 double topLeftOut[3],
                                                 double rotationPointXYZOut[3]) const
{
    bottomLeftOut[0]  = m_viewportBoundsMinX;
    bottomLeftOut[1]  = m_viewportBoundsMinY;
    bottomLeftOut[2]  = m_viewportZ;
    bottomRightOut[0] = m_viewportBoundsMaxX;
    bottomRightOut[1] = m_viewportBoundsMinY;
    bottomRightOut[2] = m_viewportZ;
    topRightOut[0]    = m_viewportBoundsMaxX;
    topRightOut[1]    = m_viewportBoundsMaxY;
    topRightOut[2]    = m_viewportZ;
    topLeftOut[0]     = m_viewportBoundsMinX;
    topLeftOut[1]     = m_viewportBoundsMaxY;
    topLeftOut[2]     = m_viewportZ;
    
    double rotationX = m_viewportBoundsMinX;
    double rotationY = m_viewportBoundsMinY;
    
    if (m_rotationAngle != 0.0) {
        
        switch (m_annotationText.getVerticalAlignment()) {
            case AnnotationTextAlignVerticalEnum::BOTTOM:
                switch (m_annotationText.getHorizontalAlignment()) {
                    case AnnotationTextAlignHorizontalEnum::CENTER:
                        rotationX = (bottomLeftOut[0] + bottomRightOut[0]) / 2.0;
                        rotationY = (bottomLeftOut[1] + bottomRightOut[1]) / 2.0;
                        break;
                    case AnnotationTextAlignHorizontalEnum::LEFT:
                        rotationX = bottomLeftOut[0];
                        rotationY = bottomLeftOut[1];
                        break;
                    case AnnotationTextAlignHorizontalEnum::RIGHT:
                        rotationX = bottomRightOut[0];
                        rotationY = bottomRightOut[1];
                        break;
                }
                break;
            case AnnotationTextAlignVerticalEnum::MIDDLE:
                switch (m_annotationText.getHorizontalAlignment()) {
                    case AnnotationTextAlignHorizontalEnum::CENTER:
                        rotationX = (bottomLeftOut[0] + bottomRightOut[0] + topLeftOut[0] + topRightOut[0]) / 4.0;
                        rotationY = (bottomLeftOut[1] + bottomRightOut[1] + topLeftOut[1] + topRightOut[1]) / 4.0;
                        break;
                    case AnnotationTextAlignHorizontalEnum::LEFT:
                        rotationX = (bottomLeftOut[0] + topLeftOut[0]) / 2.0;
                        rotationY = (bottomLeftOut[1] + topLeftOut[1]) / 2.0;
                        break;
                    case AnnotationTextAlignHorizontalEnum::RIGHT:
                        rotationX = (topRightOut[0] + bottomRightOut[0]) / 2.0;
                        rotationY = (topRightOut[1] + bottomRightOut[1]) / 2.0;
                        break;
                }
                break;
            case AnnotationTextAlignVerticalEnum::TOP:
                switch (m_annotationText.getHorizontalAlignment()) {
                    case AnnotationTextAlignHorizontalEnum::CENTER:
                        rotationX = (topLeftOut[0] + topRightOut[0]) / 2.0;
                        rotationY = (topLeftOut[1] + topRightOut[1]) / 2.0;
                        break;
                    case AnnotationTextAlignHorizontalEnum::LEFT:
                        rotationX = topLeftOut[0];
                        rotationY = topLeftOut[1];
                        break;
                    case AnnotationTextAlignHorizontalEnum::RIGHT:
                        rotationX = topRightOut[0];
                        rotationY = topRightOut[1];
                        break;
                }
                break;
        }
        
        rotationPointXYZOut[0] = rotationX;
        rotationPointXYZOut[1] = rotationY;
        rotationPointXYZOut[2] = 0.0;
        
        Matrix4x4 matrix;
        matrix.translate(-rotationX, -rotationY, 0.0);
        matrix.rotateZ(-m_rotationAngle);
        matrix.translate(rotationX, rotationY, 0.0);
        
        /*
         * Add margin to the viewport bounds.
         * Margin is NOT included when rotation point is computed
         * as it will move the rotation point to the wrong position.
         */
        bottomLeftOut[0]  -= margin;
        bottomLeftOut[1]  -= margin;
        bottomRightOut[0] += margin;
        bottomRightOut[1] -= margin;
        topRightOut[0]    += margin;
        topRightOut[1]    += margin;
        topLeftOut[0]     -= margin;
        topLeftOut[1]     += margin;
        
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
}

void
FtglFontTextRenderer::TextStringGroup::initializeTextPositions()
{
    double x = m_viewportX; //100.0;  // m_viewportX;
    double y = m_viewportY; //500.0;  // m_viewportY;
    double z = m_viewportZ; //0.0;    // m_viewportZ;
    
    const int32_t numStrings = static_cast<int32_t>(m_textStrings.size());
    for (int32_t iStr = 0; iStr < numStrings; iStr++) {
        
        CaretAssertVectorIndex(m_textStrings, iStr);
        TextString* textString = m_textStrings[iStr];
        
        if (iStr >= 1) {
            CaretAssertVectorIndex(m_textStrings, iStr - 1);
            TextString* prevTextString = m_textStrings[iStr - 1];
            
            switch (m_annotationText.getOrientation()) {
                case AnnotationTextOrientationEnum::HORIZONTAL:
                {
                    /*
                     * Move coordinate DOWN for next ROW of text
                     */
                    const double offsetY1 = prevTextString->m_stringGlyphsMinY;
                    const double offsetY2 = -(s_textMarginSize * 2.0);
                    const double offsetY3 = -textString->m_stringGlyphsMaxY;
                    const double offsetY  = (offsetY1 + offsetY2 + offsetY3);
                    
                    y += offsetY;
                }
                    break;
                case AnnotationTextOrientationEnum::STACKED:
                {
                    /*
                     * Move coordinate RIGHT for next COLUMN of text
                     */
                    const double offsetX1 = prevTextString->m_stringGlyphsMaxX;
                    const double offsetX2 = s_textMarginSize;
                    const double offsetX3 = -textString->m_stringGlyphsMinX;
                    const double offsetX  = (offsetX1 + offsetX2 + offsetX3);
                    
                    x += offsetX;
                }
                    break;
            }
        }
        
        
        textString->m_viewportX = x;
        textString->m_viewportY = y;
        textString->m_viewportZ = z;
    }
}

/**
 * Print info about the text.
 */
void
FtglFontTextRenderer::TextStringGroup::print()
{
    QString msg("Text String Group: "
                + m_annotationText.getText());
    std::cout << qPrintable(msg) << std::endl;
    
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        TextString* ts = *iter;
        ts->print("    ");
    }
    std::cout << std::endl;
}

/**
 * Update the bounds (extent) of the text.
 */
void
FtglFontTextRenderer::TextStringGroup::updateTextBounds()
{
    m_viewportBoundsMinX =  std::numeric_limits<float>::max();
    m_viewportBoundsMaxX = -std::numeric_limits<float>::max();
    m_viewportBoundsMinY =  std::numeric_limits<float>::max();
    m_viewportBoundsMaxY = -std::numeric_limits<float>::max();
    
    double x = 0.0;
    double y = 0.0;
    
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        TextString* ts = *iter;
        
        double stringMinX = 0.0;
        double stringMaxX = 0.0;
        double stringMinY = 0.0;
        double stringMaxY = 0.0;
        ts->getTextBoundsInViewportCoordinates(stringMinX,
                                               stringMaxX,
                                               stringMinY,
                                               stringMaxY);
        
        const double left   = x + stringMinX;
        const double right  = x + stringMaxX;
        const double bottom = y + stringMinY;
        const double top    = y + stringMaxY;
        
        m_viewportBoundsMinX = std::min(m_viewportBoundsMinX,
                                left);
        m_viewportBoundsMaxX = std::max(m_viewportBoundsMaxX,
                                right);
        m_viewportBoundsMinY = std::min(m_viewportBoundsMinY,
                                bottom);
        m_viewportBoundsMaxY = std::max(m_viewportBoundsMaxY,
                                top);
    }
}

/**
 * Adjust the positions of the horizontal text strings so
 * they are aligned properly for both horizontal and
 * vertical alignment.
 */
void
FtglFontTextRenderer::TextStringGroup::applyAlignmentsToHorizontalTextStrings()
{
    double dy = 0.0;
    switch (m_annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            dy = m_viewportY - m_viewportBoundsMinY;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            dy = m_viewportY - (m_viewportBoundsMinY + m_viewportBoundsMaxY) / 2.0;
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            dy = m_viewportY - m_viewportBoundsMaxY;
            break;
    }
    
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        TextString* ts = *iter;
        
        double dx = 0.0;
        switch (m_annotationText.getHorizontalAlignment()) {
            case AnnotationTextAlignHorizontalEnum::CENTER:
            {
                const double centerX = (ts->m_stringGlyphsMinX + ts->m_stringGlyphsMaxX) / 2.0;
                dx = -centerX;
            }
                break;
            case AnnotationTextAlignHorizontalEnum::LEFT:
                dx = -ts->m_stringGlyphsMinX;
                break;
            case AnnotationTextAlignHorizontalEnum::RIGHT:
                dx = -ts->m_stringGlyphsMaxX;
                break;
        }
        
        ts->m_viewportX += dx;
        ts->m_viewportY += dy;
    }
}

/**
 * Adjust the positions of the stacked text strings so
 * they are aligned properly for both horizontal and
 * vertical alignment.
 */
void
FtglFontTextRenderer::TextStringGroup::applyAlignmentsToStackedTextStrings()
{
    double dx = 0.0;
    switch (m_annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
        {
            const double centerX = (m_viewportBoundsMinX + m_viewportBoundsMaxX) / 2.0;
            dx = m_viewportX - centerX;
        }
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            dx = m_viewportX - m_viewportBoundsMinX;
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            dx = m_viewportX - m_viewportBoundsMaxX;
            break;
    }
    
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        TextString* ts = *iter;
        
        double dy = 0.0;
        switch (m_annotationText.getVerticalAlignment()) {
            case AnnotationTextAlignVerticalEnum::BOTTOM:
                dy = -ts->m_stringGlyphsMinY;
                break;
            case AnnotationTextAlignVerticalEnum::MIDDLE:
                dy = - (ts->m_stringGlyphsMinY + ts->m_stringGlyphsMaxY) / 2.0;
                break;
            case AnnotationTextAlignVerticalEnum::TOP:
                dy = -ts->m_stringGlyphsMaxY;
                break;
        }

        ts->m_viewportX += dx;
        ts->m_viewportY += dy;
    }
}

/**
 * Adjust the positions of the text strings so
 * they are aligned properly.
 */
void
FtglFontTextRenderer::TextStringGroup::applyAlignmentsToTextStrings()
{
    switch (m_annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            applyAlignmentsToHorizontalTextStrings();
            break;
        case AnnotationTextOrientationEnum::STACKED:
            applyAlignmentsToStackedTextStrings();
            break;
    }
}
