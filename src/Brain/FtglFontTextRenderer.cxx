
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

///**
// * The text is split up into pieces by examining the text for newlines
// * and orientation (horizontal or stacked).  Each piece of text is assigned
// * to a row and column.  In horizontal text, a newline results in the
// * remaining text moved to the next row.  In stacked text, a new line 
// * results in the remaining text moved to the next column.
// *
// * @param annotationText
// *     The text annotation.
// * @param textLocationsOut
// *     Output containing text broken up into cells with row/column assignments.
// */
//void
//FtglFontTextRenderer::splitTextIntoRowsAndColumns(const AnnotationText& annotationText,
//                                                   TextMatrix& textMatrixOut)
//{
//#ifdef HAVE_FREETYPE
//    FTFont* font = getFont(annotationText,
//                           false);
//    if ( ! font) {
//        return;
//    }
//    
//    /*
//     * Each row (horizontal text) or column (vertical text) is
//     * separated by a newline character
//     */
//    QStringList textRowsColumns = annotationText.getText().split('\n',
//                                                                 QString::SkipEmptyParts);
//    const int32_t numTextRowsColumns = textRowsColumns.size();
//    int32_t columnIndex = 0;
//    int32_t rowIndex = 0;
//    
//    for (int32_t itrs = 0; itrs < numTextRowsColumns; itrs++) {
//        const QString textChars(textRowsColumns.at(itrs));
//        
//        switch (annotationText.getOrientation()) {
//            case AnnotationTextOrientationEnum::HORIZONTAL:
//            {
//                const FTBBox bounds = font->BBox(textChars.toAscii().constData());
//                textMatrixOut.addTextCell(TextCell(textChars,
//                                                     rowIndex,
//                                                     columnIndex,
//                                                     bounds.Lower().X(),
//                                                     bounds.Upper().X(),
//                                                     bounds.Lower().Y(),
//                                                     bounds.Upper().Y()));
////                const AString msg("Drawing: "
////                                  + annotationText.getText()
////                                  + " size="
////                                  + annotationText.getFontRenderingEncodedName(m_viewportHeight)
////                                  + " bound=("
////                                  + AString::number(bounds.Lower().X())
////                                  + ", "
////                                  + AString::number(bounds.Lower().Y())
////                                  + ", "
////                                  + AString::number(bounds.Upper().X())
////                                  + ", "
////                                  + AString::number(bounds.Upper().Y())
////                                  + ")");
////                std::cout << qPrintable(msg) << std::endl;
//                
//                rowIndex++;
//            }
//                break;
//            case AnnotationTextOrientationEnum::STACKED:
//            {
//                QString s = textRowsColumns[itrs];
//                const int32_t numChars = s.size();
//                for (int32_t ic = 0; ic < numChars; ic++) {
//                    const QString oneChar(s.at(ic));
//                    const FTBBox bounds = font->BBox(oneChar.toAscii().constData());
//                    textMatrixOut.addTextCell(TextCell(oneChar,
//                                                         rowIndex,
//                                                         columnIndex,
//                                                         bounds.Lower().X(),
//                                                         bounds.Upper().X(),
//                                                         bounds.Lower().Y(),
//                                                         bounds.Upper().Y()));
//                    
//                    rowIndex++;
//                }
//                columnIndex++;
//                rowIndex = 0;
//            }
//                break;
//        }
//    }
//#else  // HAVE_FREETYPE
//    CaretLogSevere("Trying to use FTGL Font rendering but FTGL is not valid.");
//#endif // HAVE_FREETYPE
//}

///**
// * The input text has been already been split up with row and column assignments (TextMatrix).
// * This method will assign each piece of text viewport coordinates for drawing the text.  The
// * overall bounds of the text is also set and will be used for background coloring and
// * the box that surrounds text when it is selected.
// *
// * @param viewportX
// *     Viewport X-coordinate.
// * @param viewportY
// *     Viewport Y-coordinate.
// * @param viewportZ
// *     Viewport Z-coordinate.
// * @param annotationText
// *     Annotation text and attributes.
// * @param textMatrix
// *     Text broken up into cells with viewport coordinates assigned.
// */
//void
//FtglFontTextRenderer::setTextViewportCoordinates(const double viewportX,
//                                                           const double viewportY,
//                                                           const double viewportZ,
//                                                           const AnnotationText& annotationText,
//                                                           TextMatrix& textMatrix)
//{
//    splitTextIntoRowsAndColumns(annotationText,
//                                 textMatrix);
//    
//    double allTextMinX =  std::numeric_limits<float>::max();
//    double allTextMaxX = -std::numeric_limits<float>::max();
//    double allTextMinY =  std::numeric_limits<float>::max();
//    double allTextMaxY = -std::numeric_limits<float>::max();
//    bool allValidFlag = false;
//    
//    double columnX = viewportX;
//    
//    for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
//        double columnMinX =  std::numeric_limits<float>::max();
//        double columnMaxX = -std::numeric_limits<float>::max();
//        double columnMinY =  std::numeric_limits<float>::max();
//        double columnMaxY = -std::numeric_limits<float>::max();
//        bool columnValidFlag = false;
//        
//        double rowY = viewportY;
//        for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//            TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
//            if (tc != NULL) {
//                double textOffsetX = 0;
//                
//                switch (annotationText.getOrientation()) {
//                    case AnnotationTextOrientationEnum::HORIZONTAL:
//                        switch (annotationText.getHorizontalAlignment()) {
//                            case AnnotationTextAlignHorizontalEnum::CENTER:
//                                textOffsetX = -tc->m_width / 2.0;
//                                break;
//                            case AnnotationTextAlignHorizontalEnum::LEFT:
//                                textOffsetX = -tc->m_boundsMinX;
//                                break;
//                            case AnnotationTextAlignHorizontalEnum::RIGHT:
//                                textOffsetX = -tc->m_boundsMaxX;
//                                break;
//                        }
//                        break;
//                    case AnnotationTextOrientationEnum::STACKED:
//                        textOffsetX = -tc->m_boundsMinX - (tc->m_width / 2.0);
//                        break;
//                }
//
//                
//                double textOffsetY = -tc->m_height;
////                double textOffsetY = 0;
////                switch (annotationText.getVerticalAlignment()) {
////                    case AnnotationTextAlignVerticalEnum::BOTTOM:
////                        textOffsetY = -tc->m_boundsMinY;
////                        break;
////                    case AnnotationTextAlignVerticalEnum::MIDDLE:
////                        textOffsetY = -tc->m_height / 2.0;
////                        break;
////                    case AnnotationTextAlignVerticalEnum::TOP:
////                        textOffsetY = -tc->m_boundsMaxY;
////                        break;
////                }
//                
//                tc->m_viewportX = columnX + textOffsetX;
//                tc->m_viewportY = rowY + textOffsetY;
//                tc->m_viewportZ = viewportZ;
//                
//                const double lineHeight = tc->m_height + s_textMarginSize;
//                
//                columnMinX = std::min(columnMinX,
//                                      tc->m_viewportX);
//                columnMaxX = std::max(columnMaxX,
//                                      tc->m_viewportX + tc->m_width + s_textMarginSize);
//                columnMinY = std::min(columnMinY,
//                                      tc->m_viewportY - lineHeight);
//                columnMaxY = std::max(columnMaxY,
//                                      tc->m_viewportY);
//                columnValidFlag = true;
//
//                
//                
//                rowY -= lineHeight;
//            }
//        }
//        
//        
//        if (columnValidFlag) {
//            /*
//             * Vertical alignment is computed and applied after all 
//             * text within a column has been positioned with the 
//             * Y-coordinate at the top of the first character
//             */
//            const double columnHeight = columnMaxY - columnMinY;
//            //std::cout << "Column " << iCol << " height: " << columnHeight << std::endl;
//            
//            double offsetY = 0.0;
//            switch (annotationText.getVerticalAlignment()) {
//                case AnnotationTextAlignVerticalEnum::BOTTOM:
//                    offsetY = columnHeight;
//                    break;
//                case AnnotationTextAlignVerticalEnum::MIDDLE:
//                    offsetY = columnHeight / 2.0;
//                    break;
//                case AnnotationTextAlignVerticalEnum::TOP:
//                    offsetY = 0.0;
//                    break;
//            }
//            
//            double firstLineHeight = 0.0;
//            for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//                TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
//                if (tc != NULL) {
//                    tc->m_viewportY += offsetY;
//                    if (jRow == 0) {
//                        firstLineHeight = tc->m_height;
//                    }
//                }
//            }
//            
//            
//            allTextMinX = std::min(allTextMinX,
//                                   columnMinX - s_textMarginSize);
//            allTextMaxX = std::max(allTextMaxX,
//                                   columnMaxX + s_textMarginSize);
//            allTextMinY = std::min(allTextMinY,
//                                   columnMinY + offsetY + firstLineHeight - s_textMarginSize);
//            allTextMaxY = std::max(allTextMaxY,
//                                   columnMaxY + offsetY + firstLineHeight + s_textMarginSize);
//            allValidFlag = true;
//        }
//        
//        if (columnValidFlag) {
//            const double columnWidth = (columnMaxX - columnMinX);
//            columnX += columnWidth;
//        }
//    }
//    
//    if (allValidFlag) {
//        /*
//         * Stacked text may contain multiple columns so apply an 
//         * offset to each column so that the columns of text are
//         * properly aligned.
//         */
//        switch (annotationText.getOrientation()) {
//            case AnnotationTextOrientationEnum::HORIZONTAL:
//                break;
//            case AnnotationTextOrientationEnum::STACKED:
//            {
//                const double textWidth = allTextMaxX - allTextMinX;
//                double offsetX = 0.0;
//                
//                switch (annotationText.getHorizontalAlignment()) {
//                    case AnnotationTextAlignHorizontalEnum::CENTER:
//                        offsetX = -textWidth / 2.0;
//                        break;
//                    case AnnotationTextAlignHorizontalEnum::LEFT:
//                        break;
//                    case AnnotationTextAlignHorizontalEnum::RIGHT:
//                        offsetX = -textWidth;
//                        break;
//                }
//                
//                for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
//                    for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//                        TextCell* tc = textMatrix.getCellAtRowColumn(jRow,
//                                                                       iCol);
//                        if (tc != NULL) {
//                            tc->m_viewportX += offsetX;
//                        }
//                    }
//                }
//                
//                allTextMinX += offsetX;
//                allTextMaxX += offsetX;
//            }
//                break;
//        }
//        
//        textMatrix.setBounds(allTextMinX,
//                               allTextMaxX,
//                               allTextMinY,
//                               allTextMaxY);
//    }
//}

///*
// * Set the text in X and Y positions so that each row of text in the column
// * is properly aligned, assuming that text is at x=0??????
// */
//void alignTextInColumn(const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
//                       const int32_t columnIndex,
//                       TextMatrix& textMatrix)
//{
//    
//}

///**
// * The input text has been already been split up with row and column assignments (TextMatrix).
// * This method will assign each piece of text viewport coordinates for drawing the text.  The
// * overall bounds of the text is also set and will be used for background coloring and
// * the box that surrounds text when it is selected.
// *
// * @param viewportX
// *     Viewport X-coordinate.
// * @param viewportY
// *     Viewport Y-coordinate.
// * @param viewportZ
// *     Viewport Z-coordinate.
// * @param annotationText
// *     Annotation text and attributes.
// * @param textMatrix
// *     Text broken up into cells with viewport coordinates assigned.
// */
//void
//FtglFontTextRenderer::setTextViewportCoordinates(const double viewportX,
//                                                           const double viewportY,
//                                                           const double viewportZ,
//                                                           const AnnotationText& annotationText,
//                                                           TextMatrix& textMatrix)
//{
//    splitTextIntoRowsAndColumns(annotationText,
//                                 textMatrix);
//
//    /*
//     * Find the maximum width of
//     */
//    std::vector<double> columnMaximumWidths;
//    double maxHeight = 0.0;
//    for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
//        double maxWidth = 0.0;
//        for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//            TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
//            if (tc != NULL) {
//                maxWidth  = std::max(tc->m_width,
//                                     maxWidth);
//                maxHeight = std::max(tc->m_height,
//                                     maxHeight);
//            }
//        }
//        columnMaximumWidths.push_back(maxWidth);
//    }
//    
////    std::cout << "Max width=" << maxWidth << " height=" << maxHeight << std::endl;
//    
//    
//    double allTextMinX =  std::numeric_limits<float>::max();
//    double allTextMaxX = -std::numeric_limits<float>::max();
//    double allTextMinY =  std::numeric_limits<float>::max();
//    double allTextMaxY = -std::numeric_limits<float>::max();
//    bool allValidFlag = false;
//
//    double columnX = 0.0; //viewportX;
//
//    for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
//        double columnMinX =  std::numeric_limits<float>::max();
//        double columnMaxX = -std::numeric_limits<float>::max();
//        double columnMinY =  std::numeric_limits<float>::max();
//        double columnMaxY = -std::numeric_limits<float>::max();
//        bool columnValidFlag = false;
//
//        double rowY = 0.0;  //viewportY;
//        for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//            TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
//            if (tc != NULL) {
//                
//
//                /*
//                 * We start with offsets that move the text so that
//                 * the viewport X- and Y-coordinates are in the 
//                 * center of the text.
//                 */
//                double textOffsetX = (-tc->m_width / 2.0) - tc->m_boundsMinX; //-tc->m_centerX;
//// 9:19                double textOffsetY = -tc->m_centerY;
//                double textOffsetY = (-tc->m_height / 2.0) - tc->m_boundsMinY;
////                switch (annotationText.getOrientation()) {
////                    case AnnotationTextOrientationEnum::HORIZONTAL:
////                        switch (annotationText.getHorizontalAlignment()) {
////                            case AnnotationTextAlignHorizontalEnum::CENTER:
////                                textOffsetX = -tc->m_centerX; // -tc->m_width / 2.0;
////                                break;
////                            case AnnotationTextAlignHorizontalEnum::LEFT:
////                                textOffsetX = -tc->m_boundsMinX;
////                                break;
////                            case AnnotationTextAlignHorizontalEnum::RIGHT:
////                                textOffsetX = -tc->m_boundsMaxX;
////                                break;
////                        }
////                        break;
////                    case AnnotationTextOrientationEnum::STACKED:
////                        textOffsetX = -tc->m_centerX;  //  -tc->m_boundsMinX - (tc->m_width / 2.0);
////                        break;
////                }
//
//
////                double textOffsetY = -tc->m_height;
//
//                tc->m_viewportX = columnX + textOffsetX;
//                tc->m_viewportY = rowY + textOffsetY;
//                tc->m_viewportZ = viewportZ;
//
//                const double lineHeight = tc->m_height + s_textMarginSize;
//
//                /*
//                 * Bounds around the text
//                 */
//                columnMinX = std::min(columnMinX,
//                                      columnX - (tc->m_width / 2.0)); //tc->m_viewportX + tc->m_boundsMinX);
//                columnMaxX = std::max(columnMaxX,
//                                      columnX + (tc->m_width / 2.0)); //tc->m_viewportX + tc->m_width); //tc->m_centerX); //tc->m_width);  // + s_textMarginSize);
//// 9:19               columnMinY = std::min(columnMinY,
//// 9:19                                     tc->m_viewportY);
//// 9:19               columnMaxY = std::max(columnMaxY,
//// 9:19                                     tc->m_viewportY + tc->m_height);
//                columnMinY = std::min(columnMinY,
//                                      rowY - (tc->m_height / 2.0));
//                columnMaxY = std::max(columnMaxY,
//                                      rowY + (tc->m_height / 2.0));
//                columnValidFlag = true;
//
//
//
//                rowY -= lineHeight;
//                rowY -= s_textMarginSize;
//            }
//        }
//
//
//        if (columnValidFlag) {
////            /*
////             * Vertical alignment is computed and applied after all
////             * text within a column has been positioned with the
////             * Y-coordinate at the top of the first character
////             */
////            const double columnHeight = columnMaxY - columnMinY;
////            //std::cout << "Column " << iCol << " height: " << columnHeight << std::endl;
////
////            double offsetY = 0.0;
////            switch (annotationText.getVerticalAlignment()) {
////                case AnnotationTextAlignVerticalEnum::BOTTOM:
////                    offsetY = columnHeight;
////                    break;
////                case AnnotationTextAlignVerticalEnum::MIDDLE:
////                    offsetY = columnHeight / 2.0;
////                    break;
////                case AnnotationTextAlignVerticalEnum::TOP:
////                    offsetY = 0.0;
////                    break;
////            }
////
////            double firstLineHeight = 0.0;
////            for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
////                TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
////                if (tc != NULL) {
////                    tc->m_viewportY += offsetY;
////                    if (jRow == 0) {
////                        firstLineHeight = tc->m_height;
////                    }
////                }
////            }
//
//
//            allTextMinX = std::min(allTextMinX,
//                                   columnMinX); // - s_textMarginSize);
//            allTextMaxX = std::max(allTextMaxX,
//                                   columnMaxX); // + s_textMarginSize);
//            allTextMinY = std::min(allTextMinY,
//                                   columnMinY); // - s_textMarginSize);
//            allTextMaxY = std::max(allTextMaxY,
//                                   columnMaxY); // + s_textMarginSize);
////            allTextMinY = std::min(allTextMinY,
////                                   columnMinY + offsetY + firstLineHeight - s_textMarginSize);
////            allTextMaxY = std::max(allTextMaxY,
////                                   columnMaxY + offsetY + firstLineHeight + s_textMarginSize);
//            allValidFlag = true;
//        }
//
//        if (columnValidFlag) {
//            const double columnWidth = (columnMaxX - columnMinX);
//            columnX += columnWidth;
//        }
//    }
//
//    if (allValidFlag) {
//        /*
//         * Stacked text may contain multiple columns so apply an
//         * offset to each column so that the columns of text are
//         * properly aligned.
//         */
////        switch (annotationText.getOrientation()) {
////            case AnnotationTextOrientationEnum::HORIZONTAL:
////                break;
////            case AnnotationTextOrientationEnum::STACKED:
////            {
////                const double textWidth = allTextMaxX - allTextMinX;
////                double offsetX = 0.0;
////
////                switch (annotationText.getHorizontalAlignment()) {
////                    case AnnotationTextAlignHorizontalEnum::CENTER:
////                        offsetX = -textWidth / 2.0;
////                        break;
////                    case AnnotationTextAlignHorizontalEnum::LEFT:
////                        break;
////                    case AnnotationTextAlignHorizontalEnum::RIGHT:
////                        offsetX = -textWidth;
////                        break;
////                }
////
////                for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
////                    for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
////                        TextCell* tc = textMatrix.getCellAtRowColumn(jRow,
////                                                                       iCol);
////                        if (tc != NULL) {
////                            tc->m_viewportX += offsetX;
////                        }
////                    }
////                }
////
////                allTextMinX += offsetX;
////                allTextMaxX += offsetX;
////            }
////                break;
////        }
//
//        
//        
//        for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
//            for (int32_t jRow = 0; jRow < textMatrix.m_numRows; jRow++) {
//                TextCell* tc = textMatrix.getCellAtRowColumn(jRow, iCol);
//                if (tc != NULL) {
//                    float dx = 0.0;
//                    float dy = 0.0;
//                    
//                    switch (annotationText.getHorizontalAlignment()) {
//                        case AnnotationTextAlignHorizontalEnum::CENTER:
//                            dx = viewportX - (allTextMinX + allTextMaxX) / 2.0;
//                            break;
//                        case AnnotationTextAlignHorizontalEnum::LEFT:
//                            tc->m_viewportX = viewportX;
//                            break;
//                        case AnnotationTextAlignHorizontalEnum::RIGHT:
//                            dx = viewportX - allTextMaxX;
//                            break;
//                    }
//                    
//                    switch (annotationText.getVerticalAlignment()) {
//                        case AnnotationTextAlignVerticalEnum::BOTTOM:
//                            dy = viewportY - allTextMinY;
//                            break;
//                        case AnnotationTextAlignVerticalEnum::MIDDLE:
//                            dy = viewportY - (allTextMinY + allTextMaxY) / 2.0;
//                            break;
//                        case AnnotationTextAlignVerticalEnum::TOP:
//                            dy = viewportY - allTextMaxY;
//                            break;
//                    }
//                }
//            }
//        }
//        
////        allTextMinX += dx;
////        allTextMaxX += dx;
////        allTextMinY += dy;
////        allTextMaxY += dy;
//        
//        textMatrix.setBounds(allTextMinX,
//                               allTextMaxX,
//                               allTextMinY,
//                               allTextMaxY);
//    }
//}



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
                                                    const TextMatrix& textMatrix)
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
        glTranslatef(textMatrix.m_viewportX, textMatrix.m_viewportY, 0.0);
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
    textMatrix.getBounds(s_textMarginSize,
                         bottomLeft, bottomRight, topRight, topLeft, rotationPointXYZ);
    
    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(textMatrix);
    applyForegroundColoring(annotationText);
    
    const float rotationAngle = annotationText.getRotationAngle();
    const TextCell* firstTextCell = textMatrix.getCellAtRowColumn(0, 0);
    if ((rotationAngle != 0.0)
      && (firstTextCell != NULL)){
        glTranslated(rotationPointXYZ[0], rotationPointXYZ[1], rotationPointXYZ[2]);
        glRotated(rotationAngle, 0.0, 0.0, -1.0);
        
        for (std::vector<TextCell>::const_iterator iter = textMatrix.m_textCells.begin();
             iter != textMatrix.m_textCells.end();
             iter++) {
            const TextCell& tc = *iter;

            const double offsetX = tc.m_viewportX - rotationPointXYZ[0];
            const double offsetY = tc.m_viewportY - rotationPointXYZ[1];
            const double offsetZ = tc.m_viewportZ - rotationPointXYZ[2];
            
            glPushMatrix();
            glTranslated(offsetX,
                         offsetY,
                         offsetZ);
            font->Render(tc.m_text.toAscii().constData());
            glPopMatrix();
        }
    }
    else {
        for (std::vector<TextCell>::const_iterator iter = textMatrix.m_textCells.begin();
             iter != textMatrix.m_textCells.end();
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
    }
    
    glPopMatrix();
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
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
                font->Render(tc->m_character.toAscii().constData());
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
                font->Render(tc->m_character.toAscii().constData());
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
    tsg.print();
    
//    TextMatrix textMatrix(annotationText,
//                          font,
//                              viewportX,
//                              viewportY,
//                              viewportZ,
//                              annotationText.getRotationAngle());
//    
////    setTextViewportCoordinates(viewportX,
////                               viewportY,
////                               viewportZ,
////                               annotationText,
////                               textMatrix);
//    
////    if (debugPrintFlag) {
////        std::cout << "Text: " << qPrintable(AnnotationTextOrientationEnum::toGuiName(annotationText.getOrientation())) << " " << qPrintable(annotationText.getText()) << std::endl;
////        std::cout << "   Bounds xMin=" << textMatrix.m_minX << " xMax=" << textMatrix.m_maxX << " yMin=" << textMatrix.m_minY << " yMax=" << textMatrix.m_maxY << std::endl;
////        for (int32_t iRow = 0; iRow < textMatrix.m_numRows; iRow++) {
////            for (int32_t iCol = 0; iCol < textMatrix.m_numColumns; iCol++) {
////                const TextCell* tc = textMatrix.getCellAtRowColumn(iRow, iCol);
////                if (tc != NULL) {
////                    std::cout << "   row=" << tc->m_row << " col=" << tc->m_column << " text=" << qPrintable(tc->m_text) << std::endl;
////                    std::cout << "   Text  vpX=" << tc->m_viewportX << " vpY=" << tc->m_viewportY  << " vpZ=" << tc->m_viewportZ << std::endl;
////                    std::cout << "   Input vpX=" << viewportX << " vpY=" << viewportY << std::endl;
////                }
////            }
////        }
////        std::cout << std::endl << std::endl;
////    }
//    
//    drawTextAtViewportCoordinatesInternal(annotationText,
//                                          textMatrix);
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
    
    TextMatrix textMatrix(annotationText,
                          font,
                              viewportX,
                              viewportY,
                              viewportZ,
                              annotationText.getRotationAngle());
    
//    setTextViewportCoordinates(viewportX,
//                               viewportY,
//                               viewportZ,
//                               annotationText,
//                               textMatrix);
    
    double rotationPointXYZ[3];
    textMatrix.getBounds(s_textMarginSize,
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
FtglFontTextRenderer::applyBackgroundColoring(const TextMatrix& textMatrix)
{
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3], rotationPointXYZ[3];

    textMatrix.getBounds(s_textMarginSize, bottomLeft, bottomRight, topRight, topLeft, rotationPointXYZ);
    
    float backgroundColor[4];
    textMatrix.m_annotationText.getBackgroundColorRGBA(backgroundColor);
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
FtglFontTextRenderer::TextCharacter::TextCharacter(const QString& character,
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


FtglFontTextRenderer::TextCharacter::~TextCharacter()
{
    
}

void
FtglFontTextRenderer::TextCharacter::print(const AString& offsetString)
{
    const QString msg(offsetString
                      + "Char: "
                      + m_character
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
    const int32_t numChars = textString.length();
    for (int32_t i = 0; i < numChars; i++) {
        const QString theCharStr(textString.at(i));
        const char theChar = textString.at(i).toAscii();
        const FTBBox bbox = font->BBox(theCharStr.toAscii().constData());

        double advanceValue = 0.0;
        if (i < (numChars - 1)) {
            const char nextChar = textString.at(i + 1).toAscii();
            advanceValue = font->Advance(theChar, nextChar);
        }
        
        TextCharacter* tc = new TextCharacter(QString(theChar),
                                              font->Advance(theCharStr.toAscii().constData()),
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
    
    setGlyphBounds();
}

FtglFontTextRenderer::TextString::~TextString()
{
    for (std::vector<TextCharacter*>::iterator iter = m_characters.begin();
         iter != m_characters.end();
         iter++) {
        delete *iter;
    }
    m_characters.clear();
}

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
        std::cout << "Column width=" << columnWidth << std::endl;
        if (columnWidth > 0.0) {
            for (int32_t iChar = 0; iChar < numChars; iChar++) {
                CaretAssertVectorIndex(m_characters, iChar);
                TextCharacter* tc = m_characters[iChar];
                
                const double characterWidth = tc->m_glyphMaxX - tc->m_glyphMinX;
                if (characterWidth > 0.0) {
                    const double leftAndRightPadding = columnWidth - characterWidth;
                    const double leftPadding = leftAndRightPadding / 2.0;
                        const double characterX = leftPadding - tc->m_glyphMinX;
                        std::cout << "Char " << qPrintable(tc->m_character
                                                           + ": "
                                                           + " CharMinX="
                                                           + QString::number(tc->m_glyphMinX)
                                                           + " CharMaxX="
                                                           + QString::number(tc->m_glyphMaxX)
                                                           + " CharWidth="
                                                           + QString::number(characterWidth)
                                                           + " Padding="
                                                           + QString::number(leftPadding)
                                                           + " CharacterX="
                                                           + QString::number(characterX)) << std::endl;
//                        tc->m_offsetX = characterX;
                    if (leftPadding >= -0.01) {
                        
                        CaretAssertVectorIndex(characterColumnCoordX, iChar);
                        characterColumnCoordX[iChar] = characterX;
                        
//                        /*
//                         * The offset is RELATIVE to the previous character
//                         */
//                        if (iChar > 0) {
//                            //CaretAssertVectorIndex(m_characters, iChar - 1);
//                            //const TextCharacter* prevChar = m_characters[iChar - 1];
//                            CaretAssertVectorIndex(m_characters, iChar - 1);
//                            tc->m_offsetX -= previousCharacterCoordX[iChar - 1]; //prevChar->m_offsetX;
//                        }
                    }
                    else {
                        CaretLogSevere("Text Character ("
                                       + tc->m_character
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
    
    
    
//    FTBBox* box = font->Bounds(m_ann
       // and then print the font bounds to compare with computed character
       // bounds
    
    
    const QString doubleOffset(offsetString + offsetString);
    
    for (std::vector<TextCharacter*>::iterator iter = m_characters.begin();
         iter != m_characters.end();
         iter++) {
        TextCharacter* tc = *iter;
        tc->print(doubleOffset);
    }
}

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
 * @param minX
 *     Bounds minimum X.
 * @param maxX
 *     Bounds maximum X.
 * @param minY
 *     Bounds minimum Y.
 * @param maxY
 *     Bounds maximum Y.
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
//        ts->m_viewportX = x;
//        ts->m_viewportY = y;
//        ts->m_viewportZ = z;
//
//        
//        y -= 80.0;
//        ts->initializeTextPositions(m_annotationText.getOrientation(),
//                                    x,
//                                    y,
//                                    z);
    }
}


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

/*
 * Moves individual text strings so that they align
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
    
    
    for (std::vector<TextString*>::iterator iter = m_textStrings.begin();
         iter != m_textStrings.end();
         iter++) {
        TextString* ts = *iter;
    }
}






/* ================================================================================== */
/* ================================================================================== */
/* ================================================================================== */
/* ================================================================================== */
/* ================================================================================== */
/* ================================================================================== */



/**
 * Constructs a text cell which contains information
 * for rendering text characters.
 *
 * @param text
 *     The text characters.
 * @param row
 *     The cell's row.
 * @param column
 *     The cell's column.
 * @param boundsMinX
 *     Minimum-X of text bounds.
 * @param boundsMaxX
 *     Maximum-X of text bounds.
 * @param boundsMinY
 *     Minimum-Y of text bounds.
 * @param boundsMaxY
 *     Maximum-Y of text bounds.
 */
FtglFontTextRenderer::TextCell::TextCell(const QString& text,
                                         const int32_t row,
                                         const int32_t column,
                                         const double boundsMinX,
                                         const double boundsMaxX,
                                         const double boundsMinY,
                                         const double boundsMaxY)
: m_text(text),
m_row(row),
m_column(column),
m_glyphMinX(boundsMinX),
m_glyphMaxX(boundsMaxX),
m_glyphMinY(boundsMinY),
m_glyphMaxY(boundsMaxY),
m_width(m_glyphMaxX - m_glyphMinX),
m_height(m_glyphMaxY - m_glyphMinY),
m_centerX((m_glyphMaxX + m_glyphMinX) / 2.0),
m_centerY((m_glyphMaxY + m_glyphMinY) / 2.0)
{
//    m_width  = m_boundsMaxX - m_boundsMinX;
//    m_height = m_boundsMaxY - m_boundsMinY;
//    
//    m_centerX = (m_boundsMaxX + m_boundsMinX) / 2.0;
//    m_centerY = (m_boundsMaxY + m_boundsMinY) / 2.0;
    
    m_viewportX = 0.0;
    m_viewportY = 0.0;
    m_viewportZ = 0.0;
    
//    std::cout << qPrintable("Text Cell: "
//                            + text
//                            + "\n     minX="
//                            + QString::number(m_glyphMinX)
//                            + " maxX="
//                            + QString::number(m_glyphMaxX)
//                            + " minY="
//                            + QString::number(m_glyphMinY)
//                            + " maxY="
//                            + QString::number(m_glyphMaxY)
//                            + "\n     width="
//                            + QString::number(m_width)
//                            + " height="
//                            + QString::number(m_height)
//                            + " centerX="
//                            + QString::number(m_centerX)
//                            + " centerY="
//                            + QString::number(m_centerY)) << std::endl << std::endl;
}

/**
 * Get a bounding box for the text after it has been positioned in
 * the viewport.
 *
 * @param minXOut
 *     Output with minimum x viewport coordinate.
 * @param maxXOut
 *     Output with minimum x viewport coordinate.
 * @param minYOut
 *     Output with minimum y viewport coordinate.
 * @param maxYOut
 *     Output with minimum y viewport coordinate.
 */
void
FtglFontTextRenderer::TextCell::getViewportBoundingBox(double& minXOut,
                                                       double& maxXOut,
                                                       double& minYOut,
                                                       double& maxYOut) const
{
    minXOut = m_glyphMinX + m_viewportX;
    maxXOut = m_glyphMaxX + m_viewportX;
    minYOut = m_glyphMinY + m_viewportY;
    maxYOut = m_glyphMaxY + m_viewportY;
}


void
FtglFontTextRenderer::TextCell::print() const
{
    const QString msg = ("Text Cell Row/Col: ("
                         + AString::number(m_row)
                         + " "
                         + AString::number(m_column)
                         + ") Text:"
                         + m_text
                         + "\n    XYZ: ("
                         + AString::number(m_viewportX)
                         + ", "
                         + AString::number(m_viewportY)
                         + ", "
                         + AString::number(m_viewportZ)
                         + ")\n    W/H: ("
                         + AString::number(m_width)
                         + ", "
                         + AString::number(m_height)
                         + ")");
    std::cout << qPrintable(msg) << std::endl;
}

/**
 * Constructor for text drawing information.  It contains information
 * for drawing text that is split up using the text orientation
 * and newlines.
 *
 * @param annotationText
 *     The annotation.
 * @param font
 *     Font for drawing the text.
 * @param viewportX
 *     X viewport coordinate for drawing text.
 * @param viewportY
 *     Y viewport coordinate for drawing text.
 * @param viewportZ
 *     Z viewport coordinate for drawing text.
 * @param rotationAngle
 *     Annotation's rotation angle for drawing text.
 */
FtglFontTextRenderer::TextMatrix::TextMatrix(const AnnotationText& annotationText,
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
m_stackedOrientMaximumColumnHeight(0.0),
//m_horizontalOrientMaximumRowWidth(0.0),
m_numRows(0),
m_numColumns(0),
m_viewportMinX(0.0),
m_viewportMaxX(0.0),
m_viewportMinY(0.0),
m_viewportMaxY(0.0)
{
    CaretAssert(font);
    
    splitTextIntoCells();
    
    positionTextStartingAtViewportCoordinates();
    
    applyAlignmentsToTextCoordinates();

//    setRowWidthsAndColumnHeights();
//    
//    switch (m_annotationText.getOrientation()) {
//        case AnnotationTextOrientationEnum::HORIZONTAL:
//            positionTextInColumn(0,
//                                 m_annotationText.getHorizontalAlignment(),
//                                 0.0);
//            break;
//        case AnnotationTextOrientationEnum::STACKED:
//            positionStackedText();
//            break;
//    }
    
    setViewportBoundingBox();
    
//    print();
}

/**
 * Position text so that the first character's origin is at
 * the viewport coordiantes
 */
void
FtglFontTextRenderer::TextMatrix::positionTextStartingAtViewportCoordinates()
{
    if ((m_numRows <= 0)
        || (m_numColumns <=0)) {
        return;
    }
    
    bool stackedTextFlag = false;
    switch (m_annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            break;
        case AnnotationTextOrientationEnum::STACKED:
            stackedTextFlag = true;
            break;
    }
    
    const float verticalSpacing = s_textMarginSize * 2.0;
    
    std::vector<double> columnMinX(m_numColumns, 0.0);
    std::vector<double> columnMaxX(m_numColumns, 0.0);
    
    /*
     * Find the minimum and maximum X-values for each column
     */
    for (int32_t jCol = 0; jCol < m_numColumns; jCol++) {
        double minX =  std::numeric_limits<float>::max();
        double maxX = -std::numeric_limits<float>::max();
        
        bool haveCellsInColumnFlag = false;
        for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
            TextCell* cell = getCellAtRowColumn(iRow, jCol);
            if (cell != NULL) {
                minX = std::min(minX, cell->m_glyphMinX);
                maxX = std::max(maxX, cell->m_glyphMaxX);
                haveCellsInColumnFlag = true;
            }
        }
        if (haveCellsInColumnFlag) {
            CaretAssertVectorIndex(columnMinX, jCol);
            CaretAssertVectorIndex(columnMaxX, jCol);
            
            columnMinX[jCol] = minX;
            columnMaxX[jCol] = maxX;
            //std::cout << "Column " << jCol << ": minX=" << minX << ", maxX=" << maxX << std::endl;
        }
    }
    
    
    /*
     * Offset columns of text so that each column is
     * "to the right" of the previous column
     */
    double columnOffset = 0.0;
    
    for (int32_t jCol = 0; jCol < m_numColumns; jCol++) {
        if (jCol >= 1) {
            /*
             * When more than one column,
             * the additional columns need to be offset
             * from the previous column
             */
            CaretAssertVectorIndex(columnMaxX, jCol - 1);
            columnOffset += columnMaxX[jCol - 1];
            columnOffset += s_textMarginSize;
            CaretAssertVectorIndex(columnMinX, jCol);
            columnOffset += columnMinX[jCol];
        }
        
        /*
         * Offset each cell in the column so that
         * each successive cell is "under" the
         * previous cell.
         */
        double verticalOffset = 0.0;
        for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
            TextCell* cell = getCellAtRowColumn(iRow, jCol);
            if (cell != NULL) {
                if (iRow >= 1) {
                    /*
                     * When more than one row,
                     * the additional rows need to be offset
                     * from the previous row
                     */
                    TextCell* previousCell = getCellAtRowColumn(iRow - 1,
                                                                jCol);
                    if (previousCell != NULL) {
                        verticalOffset += previousCell->m_glyphMinY;
                    }
                    verticalOffset -= verticalSpacing;
                    verticalOffset -= cell->m_glyphMaxY;
                }
                
                float stackedOffset = 0.0;
                if (stackedTextFlag) {
                    CaretAssertVectorIndex(columnMinX, jCol);
                    CaretAssertVectorIndex(columnMaxX, jCol);
                    stackedOffset = getStackedTextCenterOffset(iRow,
                                                               jCol,
                                                               columnMinX[jCol],
                                                               columnMaxX[jCol]);
                }
                
                /*
                 * Set the position for the text to the 
                 * viewport coordinates plus offsets
                 * for the row and column.
                 */
                cell->m_viewportX = m_viewportX + columnOffset + stackedOffset;
                cell->m_viewportY = m_viewportY + verticalOffset;
            }
        }
    }
}

/**
 * When the text is stacked (vertially oriented), the positions of the
 * characters need to be adjusted horizontally so that they are properly
 * aligned.
 *
 *
 * @param rowIndex
 *     Index of the row.
 * @param columnIndex
 *     Index of the column.
 * @param columnMinimumX
 *     Minimum X of the column.
 * @param columnMaximumX
 *     Maximum X of the column.
 * @return
 *     Offset for the cell's text due.
 */
float
FtglFontTextRenderer::TextMatrix::getStackedTextCenterOffset(const int32_t rowIndex,
                                                             const int32_t columnIndex,
                                                             const float columnMinimumX,
                                                             const float columnMaximumX)
{
    float offset = 0.0;
    
    
    TextCell* tc = getCellAtRowColumn(rowIndex, columnIndex);
    if (tc != NULL) {
//        const float columnHalfWidth = (columnMaximumX - columnMinimumX) / 2.0;
//        const float textHalfWidth = (tc->m_glyphMaxX - tc->m_glyphMinX) / 2.0;
//        offset = columnHalfWidth - textHalfWidth;
        
//        const float columnMiddleX = (columnMinimumX + columnMaximumX) / 2.0;
//        const float columnMiddleOffset = columnMiddleX - columnMinimumX;
//        
//        const float textMiddleX = (tc->m_glyphMinX + tc->m_glyphMaxX) / 2.0;
//        const float textMiddleOffset = textMiddleX - tc->m_glyphMinX;
//        
//        offset = columnMiddleOffset - textMiddleOffset;
        
        const float columnWidth = columnMaximumX - columnMinimumX;
        const float leftAndRightPadding = columnWidth - tc->m_width;
        const float leftPadding = leftAndRightPadding / 2.0;
        offset = leftPadding - tc->m_glyphMinX;
    }
    
    return offset;
}

/**
 * All the text has been positioned but the first character's
 * origin is at the viewport coordinate.  Now, adjust the
 * position of the text using the horizontal and vertical
 * alignments.
 */
void
FtglFontTextRenderer::TextMatrix::applyAlignmentsToTextCoordinates()
{
    /*
     * Sets the bounds for all of the text in the viewport.
     * Using these bounds, we determine the translations
     * that are appled to the text.
     */
    setViewportBoundingBox();
    
    /*
     * Translation from horizontal alignment
     */
    float translateX = 0.0;
    switch (m_annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::LEFT:
            translateX = m_viewportX - m_viewportMinX;
            break;
        case AnnotationTextAlignHorizontalEnum::CENTER:
            translateX = m_viewportX - ((m_viewportMinX + m_viewportMaxX) / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            translateX = m_viewportX - m_viewportMaxX;
            break;
    }
    
    /*
     * Translation from vertical alignment
     */
    float translateY = 0.0;
    switch (m_annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            translateY = m_viewportY - m_viewportMinY;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            translateY = m_viewportY - ((m_viewportMinY + m_viewportMaxY) / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            translateY = m_viewportY - m_viewportMaxY;
            break;
    }
    
    for (int32_t jCol = 0; jCol < m_numColumns; jCol++) {
        for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
            TextCell* cell = getCellAtRowColumn(iRow, jCol);
            if (cell != NULL) {
                cell->m_viewportX += translateX;
                cell->m_viewportY += translateY;
            }
        }
    }
}



/* Replaces FtglFontTextRenderer::splitTextIntoRowsAndColumns */
/**
 * Note that FTGL only draws text in a horizontal orientation.
 * FTGL does NOT wrap text when it encounters a newline.
 * FTGL does NOT draw stacked (vertically oriented) text.
 *
 * This methods splits up the text into a "matrix" of text 
 * cells.  When the text is in a vertical orientation, each
 * individual character is in its own cell.  When the text
 * is in a horizontal orientation, a cell may contain any
 * number of characters.
 * 
 * When a newline is found, a new row is created for 
 * horizontally oriented text and a new column is created
 * for stacked (vertical) text.
 */
void
FtglFontTextRenderer::TextMatrix::splitTextIntoCells()
{
#ifdef HAVE_FREETYPE
    /*
     * Each row (horizontal text) or column (vertical text) is
     * separated by a newline character
     */
    QStringList textRowsColumns = m_annotationText.getText().split('\n',
                                                                 QString::SkipEmptyParts);
    const int32_t numTextRowsColumns = textRowsColumns.size();
    int32_t columnIndex = 0;
    int32_t rowIndex = 0;
    
    for (int32_t itrs = 0; itrs < numTextRowsColumns; itrs++) {
        const QString textChars(textRowsColumns.at(itrs));
        
        switch (m_annotationText.getOrientation()) {
            case AnnotationTextOrientationEnum::HORIZONTAL:
            {
                /*
                 * For horizontally oriented text, there will be
                 * one column and one or more rows where each row
                 * is delineated by a newline character.
                 */
                const FTBBox bounds = m_font->BBox(textChars.toAscii().constData());
                addTextCell(TextCell(textChars,
                                     rowIndex,
                                     columnIndex,
                                     bounds.Lower().X(),
                                     bounds.Upper().X(),
                                     bounds.Lower().Y(),
                                     bounds.Upper().Y()));
                rowIndex++;
            }
                break;
            case AnnotationTextOrientationEnum::STACKED:
            {
                /*
                 * For vertically oriented text, each cell contains one and only one character
                 * as FTGL does not support vertically oriented text.  Columns are separated
                 * by a newline character.
                 */
                QString s = textRowsColumns[itrs];
                const int32_t numChars = s.size();
                for (int32_t ic = 0; ic < numChars; ic++) {
                    const QString oneChar(s.at(ic));
                    const FTBBox bounds = m_font->BBox(oneChar.toAscii().constData());
                    addTextCell(TextCell(oneChar,
                                         rowIndex,
                                         columnIndex,
                                         bounds.Lower().X(),
                                         bounds.Upper().X(),
                                         bounds.Lower().Y(),
                                         bounds.Upper().Y()));
                    
                    rowIndex++;
                }
                columnIndex++;
                rowIndex = 0;
            }
                break;
        }
    }
#else  // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but FTGL is not valid.");
#endif // HAVE_FREETYPE
    
}

/**
 * Examine the text cells to determine maximum widths/heights
 * that will be used to position text and the bounds of the text.
 */
void
FtglFontTextRenderer::TextMatrix::setRowWidthsAndColumnHeights()
{
    if ((m_numRows > 0)
        && (m_numColumns > 0)) {
        /*
         * Find the maximum height/width for each row
         */
        m_horizontalOrientRowHeights.resize(m_numRows, 0);
        m_horizontalOrientRowWidths.resize(m_numRows, 0);
//        m_horizontalOrientMaximumRowWidth = 0.0;
        for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
            double maxHeightInRow = 0.0;
            double totalRowWidth  = 0.0;
            for (int32_t iCol = 0; iCol < m_numColumns; iCol++) {
                const TextCell* tc = getCellAtRowColumn(iRow, iCol);
                if (tc != NULL) {
                    maxHeightInRow = std::max(tc->m_height,
                                         maxHeightInRow);
                    totalRowWidth += tc->m_width;
                }
            }
            CaretAssertVectorIndex(m_horizontalOrientRowHeights, iRow);
            m_horizontalOrientRowHeights[iRow] = maxHeightInRow;
            
            CaretAssertVectorIndex(m_horizontalOrientRowWidths, iRow);
            m_horizontalOrientRowWidths[iRow] = totalRowWidth;
            
//            m_horizontalOrientMaximumRowWidth = std::max(totalRowWidth,
//                                                         m_horizontalOrientMaximumRowWidth);
        }
        
        /*
         * Find the maximum height/width for each column
         */
        m_stackedOrientColumnHeights.resize(m_numColumns, 0);
        m_stackedOrientColumnWidths.resize(m_numColumns, 0);
        m_stackedOrientMaximumColumnHeight = 0.0;
        for (int32_t iCol = 0; iCol < m_numColumns; iCol++) {
            double maxWidthInColumn  = 0.0;
            double totalColumnHeight = 0.0;
            for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
                const TextCell* tc = getCellAtRowColumn(iRow, iCol);
                if (tc != NULL) {
                    maxWidthInColumn = std::max(tc->m_width,
                                                maxWidthInColumn);
                    totalColumnHeight += tc->m_height;
                }
            }
            
            CaretAssertVectorIndex(m_stackedOrientColumnWidths, iCol);
            m_stackedOrientColumnWidths[iCol] = maxWidthInColumn;
            
            CaretAssertVectorIndex(m_stackedOrientColumnHeights, iCol);
            m_stackedOrientColumnHeights[iCol] = totalColumnHeight;
            
            m_stackedOrientMaximumColumnHeight = std::max(totalColumnHeight,
                                                          m_stackedOrientMaximumColumnHeight);
        }
    }
}

/**
 * Position text for all rows in the given column.
 *
 * @param columnIndex
 *     Index of the column.
 * @param horizontalAlignment
 *     Alignment of text within the column.
 * @param viewportOffsetX
 *     Offset of viewport coordinate when positioning text.
 */
void
FtglFontTextRenderer::TextMatrix::positionTextInColumn(const int32_t columnIndex,
                                                       const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
                                                       const float viewportOffsetX)
{
    CaretAssertVectorIndex(m_stackedOrientColumnWidths, columnIndex);
    CaretAssertVectorIndex(m_stackedOrientColumnHeights, columnIndex);
    CaretAssert((columnIndex >= 0) && (columnIndex < m_numColumns));
    
    std::vector<double> rowOffsetX(m_numRows, 0);
    
//    const double columnMaximumWidth = m_horizontalOrientMaximumRowWidth;
    const double columnMaximumWidth = m_stackedOrientColumnWidths[columnIndex];
    
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        TextCell* tc = getCellAtRowColumn(iRow, columnIndex);
        if (tc != NULL) {
            CaretAssertVectorIndex(rowOffsetX, iRow);
            
            /* 
             * Default to left alignment (translates left edge of text to x=0
             */
            rowOffsetX[iRow] = -tc->m_glyphMinX;
            
            const double extraSpace = columnMaximumWidth - tc->m_width;
            const double halfExtraSpace = extraSpace / 2.0;
            
            switch (horizontalAlignment) {
                case AnnotationTextAlignHorizontalEnum::CENTER:
                    rowOffsetX[iRow] -= ((columnMaximumWidth / 2.0) - halfExtraSpace);
                    break;
                case AnnotationTextAlignHorizontalEnum::LEFT:
                    break;
                case AnnotationTextAlignHorizontalEnum::RIGHT:
                    rowOffsetX[iRow] -= (columnMaximumWidth - extraSpace);
                    break;
            }
        }
    }
    
//    /*
//     * Bounds minimum and maximum X
//     */
//    m_minX = m_viewportX;
//    switch (m_annotationText.getHorizontalAlignment()) {
//        case AnnotationTextAlignHorizontalEnum::CENTER:
//            m_minX -= m_horizontalOrientMaximumRowWidth / 2.0;
//            break;
//        case AnnotationTextAlignHorizontalEnum::LEFT:
//            break;
//        case AnnotationTextAlignHorizontalEnum::RIGHT:
//            m_minX -= m_horizontalOrientMaximumRowWidth;
//            break;
//    }
//    m_maxX = m_minX + m_horizontalOrientMaximumRowWidth;
    
    /*
     * Compute total height of the rows
     * with space between each row
     */
    double totalHeight = 0.0;
    const int32_t lastRowIndex = m_numRows - 1;
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        TextCell* tc = getCellAtRowColumn(iRow, columnIndex);
        if (tc != NULL) {
            totalHeight += tc->m_height;
            if (iRow < lastRowIndex) {
                totalHeight += s_textMarginSize;
            }
        }
    }
    
    /*
     * Offset from Y=0 for the first row of text.
     */
    double offsetFromZeroY = 0.0;
    switch (m_annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            offsetFromZeroY = totalHeight;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            offsetFromZeroY = totalHeight / 2.0;
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            break;
    }
    
//    /*
//     * Bounds minimum and maximum Y
//     */
//    m_maxY = m_viewportY + offsetFromZeroY;
//    m_minY = m_maxY - totalHeight;
    
    /*
     * Set the Y-offset for each row
     */
    std::vector<double> rowOffsetY(m_numRows, 0);
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        TextCell* tc = getCellAtRowColumn(iRow, columnIndex);
        if (tc != NULL) {
            /*
             * Translate text cell so top of text is at local zero
             */
            rowOffsetY[iRow] = -tc->m_glyphMaxY;
            
            /*
             * Offset to put text on next row
             */
            rowOffsetY[iRow] += offsetFromZeroY;
            
            /*
             * Move to next row
             */
            offsetFromZeroY -= tc->m_height;
            offsetFromZeroY -= s_textMarginSize;
        }
    }
    
    /*
     * Set positions of each text cell
     */
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        TextCell* tc = getCellAtRowColumn(iRow, columnIndex);
        if (tc != NULL) {
            tc->m_viewportX = m_viewportX + rowOffsetX[iRow] + viewportOffsetX;
            tc->m_viewportY = m_viewportY + rowOffsetY[iRow];
            tc->m_viewportZ = m_viewportZ;
        }
    }
}

void
FtglFontTextRenderer::TextMatrix::positionStackedText()
{
    /*
     * Position text in each row
     */
    double columnOffsetX = 0.0;
    for (int32_t jCol = 0; jCol < m_numColumns; jCol++) {
        positionTextInColumn(jCol,
                             AnnotationTextAlignHorizontalEnum::CENTER,
                             columnOffsetX);
        CaretAssertVectorIndex(m_stackedOrientColumnWidths, jCol);
        columnOffsetX += m_stackedOrientColumnWidths[jCol];
        columnOffsetX += s_textMarginSize;
    }
    
    /*
     * Set the viewport bounds of the text so that we know
     * the bounds of the text.  This method will also be
     * called when this method returns to the constructor.
     */
    setViewportBoundingBox();

    /*
     * Default offset so left edge of text is at viewport X
     * and then adjust for each horizontal orientation.
     */
    double viewportOffsetX = m_viewportX - m_viewportMinX;
    switch (m_annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            viewportOffsetX += -(m_viewportMaxX - m_viewportMinX) / 2.0;
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            viewportOffsetX += -(m_viewportMaxX - m_viewportMinX);
            break;
    }
    
    /*
     * Adjust the viewport position of each text cell 
     * using the offset
     */
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        for (int32_t jCol = 0; jCol < m_numColumns; jCol++) {
            TextCell* tc = getCellAtRowColumn(iRow, jCol);
            if (tc != NULL) {
                tc->m_viewportX += viewportOffsetX;
            }
        }
    }
}

/**
 * Set the bounds of the viewport surrounding the text.
 */
void
FtglFontTextRenderer::TextMatrix::setViewportBoundingBox()
{
    m_viewportMinX =  std::numeric_limits<float>::max();
    m_viewportMaxX = -std::numeric_limits<float>::max();
    m_viewportMinY =  std::numeric_limits<float>::max();
    m_viewportMaxY = -std::numeric_limits<float>::max();
    
    for (std::vector<TextCell>::const_iterator iter = m_textCells.begin();
         iter != m_textCells.end();
         iter++) {
        const TextCell& tc = *iter;
        
        double tcMinX = 0.0, tcMaxX = 0.0, tcMinY = 0.0, tcMaxY = 0.0;
        tc.getViewportBoundingBox(tcMinX, tcMaxX, tcMinY, tcMaxY);
        
        m_viewportMinX = std::min(m_viewportMinX, tcMinX);
        m_viewportMaxX = std::max(m_viewportMaxX, tcMaxX);
        m_viewportMinY = std::min(m_viewportMinY, tcMinY);
        m_viewportMaxY = std::max(m_viewportMaxY, tcMaxY);
    }
    
    //std::cout << "Bounds: " << m_viewportMinX << ", " << m_viewportMaxX << ", " << m_viewportMinY << ", " << m_viewportMaxY << std::endl;
    
    /*
     * Failed to set bounds???
     */
    if ((m_viewportMinX > m_viewportMaxX)
        || (m_viewportMinY > m_viewportMaxY)) {
        m_viewportMinX = 0.0;
        m_viewportMaxX = 0.0;
        m_viewportMinY = 0.0;
        m_viewportMaxY = 0.0;
    }
}


/*
 * Add the given text cell.
 *
 * @param textCell
 *     Text cell that is added.
 */
void
FtglFontTextRenderer::TextMatrix::addTextCell(const TextCell& textCell)
{
    m_textCells.push_back(textCell);
    
    m_numColumns = std::max(m_numColumns,
                            textCell.m_column + 1);
    
    m_numRows = std::max(m_numRows,
                         textCell.m_row + 1);
}

/**
 * Get the text cell at the given row and column.
 *
 * @param row
 *     Row of text cell.
 * @param column
 *     Column of text cell.
 * @return
 *     Pointer to cell at row/column or NULL if not found.
 */
FtglFontTextRenderer::TextCell*
FtglFontTextRenderer::TextMatrix::getCellAtRowColumn(const int32_t row,
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

/**
 * Get the text cell at the given row and column.
 *
 * @param row
 *     Row of text cell.
 * @param column
 *     Column of text cell.
 * @return
 *     Pointer to cell at row/column or NULL if not found.
 */
const FtglFontTextRenderer::TextCell*
FtglFontTextRenderer::TextMatrix::getCellAtRowColumn(const int32_t row,
                                                                     const int32_t column) const
{
    for (std::vector<TextCell>::const_iterator iter = m_textCells.begin();
         iter != m_textCells.end();
         iter++) {
        const TextCell& tc = *iter;
        if ((tc.m_row == row)
            && (tc.m_column == column)) {
            return &tc;
        }
    }
    return NULL;
}

///**
// * Set the bounds of the all text that is drawn.  The
// * bounds is used for selection and the background color.
// *
// * @param minX
// *     Bounds minimum X.
// * @param maxX
// *     Bounds maximum X.
// * @param minY
// *     Bounds minimum Y.
// * @param maxY
// *     Bounds maximum Y.
// */
//void
//FtglFontTextRenderer::TextMatrix::setBounds(const double minX,
//                                              const double maxX,
//                                              const double minY,
//                                              const double maxY)
//{
//    m_minX = minX;
//    m_maxX = maxX;
//    m_minY = minY;
//    m_maxY = maxY;
//}

/**
 * Get the bounds of the all text that is drawn.  The
 * bounds is used for selection and the background color.
 *
 * @param margin
 *     Margin added around sides
 * @param minX
 *     Bounds minimum X.
 * @param maxX
 *     Bounds maximum X.
 * @param minY
 *     Bounds minimum Y.
 * @param maxY
 *     Bounds maximum Y.
 * @param rotationPointXYZOut
 *     Output containing rotation point for bounds.
 */
void
FtglFontTextRenderer::TextMatrix::getBounds(const double margin,
                                            double bottomLeftOut[3],
                                              double bottomRightOut[3],
                                              double topRightOut[3],
                                              double topLeftOut[3],
                                              double rotationPointXYZOut[3]) const
{
    bottomLeftOut[0]  = m_viewportMinX - margin;
    bottomLeftOut[1]  = m_viewportMinY - margin;
    bottomLeftOut[2]  = m_viewportZ;
    bottomRightOut[0] = m_viewportMaxX + margin;
    bottomRightOut[1] = m_viewportMinY - margin;
    bottomRightOut[2] = m_viewportZ;
    topRightOut[0]    = m_viewportMaxX + margin;
    topRightOut[1]    = m_viewportMaxY + margin;
    topRightOut[2]    = m_viewportZ;
    topLeftOut[0]     = m_viewportMinX - margin;
    topLeftOut[1]     = m_viewportMaxY + margin;
    topLeftOut[2]     = m_viewportZ;
    
    double rotationX = m_viewportMinX;
    double rotationY = m_viewportMinY;
    
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
        
        
//        double allRotationX = m_minX;
//        switch (m_annotationText.getHorizontalAlignment()) {
//            case AnnotationTextAlignHorizontalEnum::CENTER:
//                allRotationX = (m_minX + m_maxX) / 2.0;
//                break;
//            case AnnotationTextAlignHorizontalEnum::LEFT:
//                allRotationX = m_minX;
//                break;
//            case AnnotationTextAlignHorizontalEnum::RIGHT:
//                allRotationX = m_maxX;
//                break;
//        }
//        
//        double allRotationY = m_minY;
//        switch (m_annotationText.getVerticalAlignment()) {
//            case AnnotationTextAlignVerticalEnum::BOTTOM:
//                allRotationY = m_minY;
//                break;
//            case AnnotationTextAlignVerticalEnum::MIDDLE:
//                allRotationY = (m_minY + m_maxY) / 2.0;
//                break;
//            case AnnotationTextAlignVerticalEnum::TOP:
//                allRotationY = m_maxY;
//                break;
//        }
        
        rotationPointXYZOut[0] = rotationX;
        rotationPointXYZOut[1] = rotationY;
        rotationPointXYZOut[2] = 0.0;
        
        Matrix4x4 matrix;
        matrix.translate(-rotationX, -rotationY, 0.0);
        matrix.rotateZ(-m_rotationAngle);
        matrix.translate(rotationX, rotationY, 0.0);
        
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
}


void
FtglFontTextRenderer::TextMatrix::print() const
{
    std::cout << "Line Height/Ascender/Descender: " << m_font->LineHeight() << ", " << m_font->Ascender() << ", " << m_font->Descender() << std::endl;
    for (int32_t iRow = 0; iRow < m_numRows; iRow++) {
        for (int32_t iCol = 0; iCol < m_numColumns; iCol++) {
            const TextCell* tc = getCellAtRowColumn(iRow, iCol);
            if (tc != NULL) {
                tc->print();
            }
        }
    }
}



