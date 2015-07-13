
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

static const bool doMultilineTextFlag =  false;


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
    const AString fontName = annotationText.getFontRenderingEncodedName();
    
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
    FontData* fontData = new FontData(annotationText);
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
 * The text is split up into pieces by examining the text for newlines
 * and orientation (horizontal or stacked).  Each piece of text is assigned
 * to a row and column.  In horizontal text, a newline results in the
 * remaining text moved to the next row.  In stacked text, a new line 
 * results in the remaining text moved to the next column.
 *
 * @param annotationText
 *     The text annotation.
 * @param textLocationsOut
 *     Output containing text broken up into cells with row/column assignments.
 */
void
FtglFontTextRenderer::assignTextRowColumnLocations(const AnnotationText& annotationText,
                                                TextDrawInfo& textDrawInfoOut)
{
    FTFont* font = getFont(annotationText,
                           false);
    if ( ! font) {
        return;
    }
    
    /*
     * Each row (horizontal text) or column (vertical text) is
     * separated by a newline character
     */
    QStringList textRowsColumns = annotationText.getText().split('\n',
                                                                 QString::SkipEmptyParts);
    const int32_t numTextRowsColumns = textRowsColumns.size();
    int32_t columnIndex = 0;
    int32_t rowIndex = 0;
    
    for (int32_t itrs = 0; itrs < numTextRowsColumns; itrs++) {
        const QString textChars(textRowsColumns.at(itrs));
        
        switch (annotationText.getOrientation()) {
            case AnnotationTextOrientationEnum::HORIZONTAL:
            {
                const FTBBox bounds = font->BBox(textChars.toAscii().constData());
                textDrawInfoOut.addTextCell(TextCell(textChars,
                                                    rowIndex,
                                                    columnIndex,
                                                     bounds));
                
                rowIndex++;
            }
                break;
            case AnnotationTextOrientationEnum::STACKED:
            {
                QString s = textRowsColumns[itrs];
                const int32_t numChars = s.size();
                for (int32_t ic = 0; ic < numChars; ic++) {
                    const QString oneChar(s.at(ic));
                    const FTBBox bounds = font->BBox(oneChar.toAscii().constData());
                    textDrawInfoOut.addTextCell(TextCell(oneChar,
                                                         rowIndex,
                                                         columnIndex,
                                                         bounds));
                    
                    rowIndex++;
                }
                columnIndex++;
                rowIndex = 0;
            }
                break;
        }
    }
}

/**
 * The input text has been already been split up with row and column assignments (TextDrawInfo).
 * This method will assign each piece of text viewport coordinates for drawing the text.  The
 * overall bounds of the text is also set and will be used for background coloring and
 * the box that surrounds text when it is selected.
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param viewportZ
 *     Viewport Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 * @param textDrawInfo
 *     Text broken up into cells with viewport coordinates assigned.
 */
void
FtglFontTextRenderer::setTextViewportCoordinates(const double viewportX,
                                                           const double viewportY,
                                                           const double viewportZ,
                                                           const AnnotationText& annotationText,
                                                           TextDrawInfo& textDrawInfo)
{
    assignTextRowColumnLocations(annotationText,
                                 textDrawInfo);
    
    double allTextMinX =  std::numeric_limits<float>::max();
    double allTextMaxX = -std::numeric_limits<float>::max();
    double allTextMinY =  std::numeric_limits<float>::max();
    double allTextMaxY = -std::numeric_limits<float>::max();
    bool allValidFlag = false;
    
    double columnX = viewportX;
    
    for (int32_t iCol = 0; iCol < textDrawInfo.m_numColumns; iCol++) {
        double columnMinX =  std::numeric_limits<float>::max();
        double columnMaxX = -std::numeric_limits<float>::max();
        double columnMinY =  std::numeric_limits<float>::max();
        double columnMaxY = -std::numeric_limits<float>::max();
        bool columnValidFlag = false;
        
        double rowY = viewportY;
        for (int32_t jRow = 0; jRow < textDrawInfo.m_numRows; jRow++) {
            TextCell* tc = textDrawInfo.getCellAtRowColumn(jRow, iCol);
            if (tc != NULL) {
                double textOffsetX = 0;
                
                switch (annotationText.getOrientation()) {
                    case AnnotationTextOrientationEnum::HORIZONTAL:
                        switch (annotationText.getHorizontalAlignment()) {
                            case AnnotationTextAlignHorizontalEnum::CENTER:
                                textOffsetX = -tc->m_width / 2.0;
                                break;
                            case AnnotationTextAlignHorizontalEnum::LEFT:
                                textOffsetX = -tc->m_boundsMinX;
                                break;
                            case AnnotationTextAlignHorizontalEnum::RIGHT:
                                textOffsetX = -tc->m_boundsMaxX;
                                break;
                        }
                        break;
                    case AnnotationTextOrientationEnum::STACKED:
                        textOffsetX = tc->m_boundsMinX;
                        break;
                }

                
                double textOffsetY = -tc->m_height;
//                double textOffsetY = 0;
//                switch (annotationText.getVerticalAlignment()) {
//                    case AnnotationTextAlignVerticalEnum::BOTTOM:
//                        textOffsetY = -tc->m_boundsMinY;
//                        break;
//                    case AnnotationTextAlignVerticalEnum::MIDDLE:
//                        textOffsetY = -tc->m_height / 2.0;
//                        break;
//                    case AnnotationTextAlignVerticalEnum::TOP:
//                        textOffsetY = -tc->m_boundsMaxY;
//                        break;
//                }
                
                tc->m_viewportX = columnX + textOffsetX;
                tc->m_viewportY = rowY + textOffsetY;
                tc->m_viewportZ = viewportZ;
                
                const double lineHeight = tc->m_height + s_textMarginSize;
                
                columnMinX = std::min(columnMinX,
                                      tc->m_viewportX);
                columnMaxX = std::max(columnMaxX,
                                      tc->m_viewportX + tc->m_width + s_textMarginSize);
                columnMinY = std::min(columnMinY,
                                      tc->m_viewportY - lineHeight);
                columnMaxY = std::max(columnMaxY,
                                      tc->m_viewportY);
                columnValidFlag = true;

                
                
                rowY -= lineHeight;
            }
        }
        
        
        if (columnValidFlag) {
            /*
             * Vertical alignment is computed and applied after all 
             * text within a column has been positioned with the 
             * Y-coordinate at the top of the first character
             */
            const double columnHeight = columnMaxY - columnMinY;
            //std::cout << "Column " << iCol << " height: " << columnHeight << std::endl;
            
            double offsetY = 0.0;
            switch (annotationText.getVerticalAlignment()) {
                case AnnotationTextAlignVerticalEnum::BOTTOM:
                    offsetY = columnHeight;
                    break;
                case AnnotationTextAlignVerticalEnum::MIDDLE:
                    offsetY = columnHeight / 2.0;
                    break;
                case AnnotationTextAlignVerticalEnum::TOP:
                    offsetY = 0.0;
                    break;
            }
            
            double firstLineHeight = 0.0;
            for (int32_t jRow = 0; jRow < textDrawInfo.m_numRows; jRow++) {
                TextCell* tc = textDrawInfo.getCellAtRowColumn(jRow, iCol);
                if (tc != NULL) {
                    tc->m_viewportY += offsetY;
                    if (jRow == 0) {
                        firstLineHeight = tc->m_height;
                    }
                }
            }
            
            
            allTextMinX = std::min(allTextMinX,
                                   columnMinX - s_textMarginSize);
            allTextMaxX = std::max(allTextMaxX,
                                   columnMaxX + s_textMarginSize);
            allTextMinY = std::min(allTextMinY,
                                   columnMinY + offsetY + firstLineHeight - s_textMarginSize);
            allTextMaxY = std::max(allTextMaxY,
                                   columnMaxY + offsetY + firstLineHeight + s_textMarginSize);
            allValidFlag = true;
        }
        
        if (columnValidFlag) {
            const double columnWidth = (columnMaxX - columnMinX);
            columnX += columnWidth;
        }
    }
    
    if (allValidFlag) {
        /*
         * Adjust X-coordinates for stacked text
         */
        switch (annotationText.getOrientation()) {
            case AnnotationTextOrientationEnum::HORIZONTAL:
                break;
            case AnnotationTextOrientationEnum::STACKED:
            {
                const double textWidth = allTextMaxX - allTextMinX;
                double offsetX = 0.0;
                
                switch (annotationText.getHorizontalAlignment()) {
                    case AnnotationTextAlignHorizontalEnum::CENTER:
                        offsetX = -textWidth / 2.0;
                        break;
                    case AnnotationTextAlignHorizontalEnum::LEFT:
                        break;
                    case AnnotationTextAlignHorizontalEnum::RIGHT:
                        offsetX = -textWidth;
                        break;
                }
                
                for (int32_t iCol = 0; iCol < textDrawInfo.m_numColumns; iCol++) {
                    for (int32_t jRow = 0; jRow < textDrawInfo.m_numRows; jRow++) {
                        TextCell* tc = textDrawInfo.getCellAtRowColumn(jRow,
                                                                       iCol);
                        if (tc != NULL) {
                            tc->m_viewportX += offsetX;
                        }
                    }
                }
                
                allTextMinX += offsetX;
                allTextMaxX += offsetX;
            }
                break;
        }
        
        textDrawInfo.setBounds(allTextMinX,
                               allTextMaxX,
                               allTextMinY,
                               allTextMaxY);
    }
}

/**
 * Draw the text piceces at their assigned viewport coordinates.
 *
 * @param annotationText
 *     Annotation text and attributes.
 * @param textDrawInfo
 *     Text broken up into cells with viewport coordinates assigned.
 *
 */
void
FtglFontTextRenderer::drawTextAtViewportCoordinatesInternal(const AnnotationText& annotationText,
                                                    const TextDrawInfo& textDrawInfo)
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
    if (drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.0);
        glPushMatrix();
        glTranslatef(textDrawInfo.m_viewportX, textDrawInfo.m_viewportY, 0.0);
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
    
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
    textDrawInfo.getBounds(bottomLeft, bottomRight, topRight, topLeft);
    
    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(annotationText,
                            bottomLeft,
                            bottomRight,
                            topRight,
                            topLeft);
    applyForegroundColoring(annotationText);
    
    for (std::vector<TextCell>::const_iterator iter = textDrawInfo.m_textCells.begin();
         iter != textDrawInfo.m_textCells.end();
         iter++) {
        const TextCell& tc = *iter;
        
        glPushMatrix();
        glLoadIdentity();
        
        glTranslated(tc.m_viewportX,
                     tc.m_viewportY,
                     tc.m_viewportZ);
        font->Render(tc.m_text.toAscii().constData());
        glPopMatrix();
    }

    
//    const double rotationAngle = annotationText.getRotationAngle();
//    if (rotationAngle != 0.0) {
//        glTranslated(textX, textY, windowZ);
//        glRotated(rotationAngle, 0.0, 0.0, -1.0);
//        font->Render(text.toAscii().constData());
//    }
//    else {
//        glTranslated(textX,
//                     textY,
//                     windowZ);
//        font->Render(text.toAscii().constData());
//    }
    
    
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
    drawTextAtViewportCoordsInternal(DEPTH_TEST_YES,
                                     viewportX,
                                     viewportY,
                                     viewportZ,
                                     annotationText);
}

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
    
    m_depthTestingStatus = depthTesting;
    
    TextDrawInfo textDrawInfo(viewportX,
                              viewportY,
                              viewportZ);
    setTextViewportCoordinates(viewportX,
                               viewportY,
                               viewportZ,
                               annotationText,
                               textDrawInfo);
    
//    std::cout << "Text: " << qPrintable(AnnotationTextOrientationEnum::toGuiName(annotationText.getOrientation())) << " " << qPrintable(annotationText.getText()) << std::endl;
//    std::cout << "   Bounds xMin=" << textDrawInfo.m_minX << " xMax=" << textDrawInfo.m_maxX << " yMin=" << textDrawInfo.m_minY << " yMax=" << textDrawInfo.m_maxY << std::endl;
//    for (int32_t iRow = 0; iRow < textDrawInfo.m_numRows; iRow++) {
//        for (int32_t iCol = 0; iCol < textDrawInfo.m_numColumns; iCol++) {
//            const TextCell* tc = textDrawInfo.getCellAtRowColumn(iRow, iCol);
//            if (tc != NULL) {
//                std::cout << "   row=" << tc->m_row << " col=" << tc->m_column << " : " << qPrintable(tc->m_text) << std::endl;
//                std::cout << "   vpX=" << tc->m_viewportX << " vpY=" << tc->m_viewportY  << " vpZ=" << tc->m_viewportZ << std::endl;
//            }
//        }
//    }
//    std::cout << std::endl << std::endl;
    
    drawTextAtViewportCoordinatesInternal(annotationText,
                                          textDrawInfo);
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
    TextDrawInfo textDrawInfo(viewportX,
                              viewportY,
                              viewportZ);
    
    setTextViewportCoordinates(viewportX,
                               viewportY,
                               viewportZ,
                               annotationText,
                               textDrawInfo);
    
    textDrawInfo.getBounds(bottomLeftOut,
                           bottomRightOut,
                           topRightOut,
                           topLeftOut);
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
FtglFontTextRenderer::applyBackgroundColoring(const AnnotationText& annotationText,
                                              const double bottomLeftOut[3],
                                              const double bottomRightOut[3],
                                              const double topRightOut[3],
                                              const double topLeftOut[3])
{
    float backgroundColor[4];
    annotationText.getBackgroundColorRGBA(backgroundColor);
    if (backgroundColor[3] > 0.0) {
//        const AString bg("Background for \"" + annotationText.getText() + ": BL="
//                         + AString::fromNumbers(bottomLeftOut, 3, ",") + "  BR="
//                         + AString::fromNumbers(bottomRightOut, 3, ",") + "  TR="
//                         + AString::fromNumbers(topRightOut, 3, ",") + "  TL="
//                         + AString::fromNumbers(topLeftOut, 3, ","));
//        std::cout << qPrintable(bg) << std::endl;
        glColor4fv(backgroundColor);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3dv(bottomLeftOut);
        glVertex3dv(bottomRightOut);
        glVertex3dv(topLeftOut);
        glVertex3dv(topRightOut);
        glEnd();
    }
}

/**
 * Apply the foreground color.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param textBoundsBox
 *   Bounding box for text (min-x, min-y, max-x, max-y)
 */
void
FtglFontTextRenderer::applyForegroundColoring(const AnnotationText& annotationText)
{
    float foregroundColor[4];
    annotationText.getForegroundColorRGBA(foregroundColor);
    glColor4fv(foregroundColor);
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
        
//        drawTextAtViewportCoords(x,
//                                 y,
//                                 windowZ,
//                                 annotationText);
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
 */
FtglFontTextRenderer::FontData::FontData(const AnnotationText&  annotationText)
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
                const AnnotationFontSizeEnum::Enum fontSizeEnum = annotationText.getFontSize();
                const int32_t fontSizeInt = AnnotationFontSizeEnum::toSizeNumeric(fontSizeEnum);
                if (m_font->FaceSize(fontSizeInt)) {
                    m_valid = true;
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










FtglFontTextRenderer::TextCell::TextCell(const QString& text,
         const int32_t row,
         const int32_t column,
         const FTBBox& textBounds)
: m_text(text),
m_row(row),
m_column(column),
m_boundsMinX(textBounds.Lower().X()),
m_boundsMaxX(textBounds.Upper().X()),
m_boundsMinY(textBounds.Lower().Y()),
m_boundsMaxY(textBounds.Upper().Y())
{
    m_width  = m_boundsMaxX - m_boundsMinX;
    m_height = m_boundsMaxY - m_boundsMinY;
    
    m_viewportX = 0.0;
    m_viewportY = 0.0;
    m_viewportZ = 0.0;
}

FtglFontTextRenderer::TextDrawInfo::TextDrawInfo(const double viewportX,
                                                 const double viewportY,
                                                 const double viewportZ)
: m_viewportX(viewportX),
m_viewportY(viewportY),
m_viewportZ(viewportZ),
m_numRows(0),
m_numColumns(0),
m_minX(0.0),
m_maxX(0.0),
m_minY(0.0),
m_maxY(0.0)
{
    
}

void
FtglFontTextRenderer::TextDrawInfo::TextDrawInfo::addTextCell(const TextCell& textCell)
{
    m_textCells.push_back(textCell);
    
    m_numColumns = std::max(m_numColumns,
                            textCell.m_column + 1);
    
    m_numRows = std::max(m_numRows,
                         textCell.m_row + 1);
}

FtglFontTextRenderer::TextCell*
FtglFontTextRenderer::TextDrawInfo::TextDrawInfo::getCellAtRowColumn(const int32_t row,
                             const int32_t column)
{
    for (std::vector<TextCell>::iterator iter = m_textCells.begin();
         iter != m_textCells.end();
         iter++) {
        TextCell& tc = *iter;
        if ((tc.m_row == row)
            && (tc.m_column == column)) {
            return &tc;
        }
    }
    return NULL;
}

void
FtglFontTextRenderer::TextDrawInfo::setBounds(const double minX,
                                              const double maxX,
                                              const double minY,
                                              const double maxY)
{
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
}
void
FtglFontTextRenderer::TextDrawInfo::getBounds(double bottomLeftOut[3],
                                              double bottomRightOut[3],
                                              double topRightOut[3],
                                              double topLeftOut[3]) const
{
    bottomLeftOut[0]  = m_minX;
    bottomLeftOut[1]  = m_minY;
    bottomLeftOut[2]  = m_viewportZ;
    bottomRightOut[0] = m_maxX;
    bottomRightOut[1] = m_minY;
    bottomRightOut[2] = m_viewportZ;
    topRightOut[0]    = m_maxX;
    topRightOut[1]    = m_maxY;
    topRightOut[2]    = m_viewportZ;
    topLeftOut[0]     = m_minX;
    topLeftOut[1]     = m_maxY;
    topLeftOut[2]     = m_viewportZ;
}




