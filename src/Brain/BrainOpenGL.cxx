

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <cstdlib>

#include <QStringList>

#define __BRAIN_OPENGL_DEFINE_H
#include "BrainOpenGL.h"
#undef __BRAIN_OPENGL_DEFINE_H

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param textRenderer
 *   The text renderer is used for text rendering.
 *   This parameter may be NULL in which case no text
 *   rendering is performed.
 */
BrainOpenGL::BrainOpenGL(BrainOpenGLTextRenderInterface* textRenderer)
{
    this->textRenderer = textRenderer;
    this->borderBeingDrawn = NULL;
    m_drawHighlightedEndPoints = false;
}

/**
 * Destructor.
 */
BrainOpenGL::~BrainOpenGL()
{
}

/**
 * Get the minimum and maximum values for the size of a point.
 * @param minPointSizeOut
 *    Gets minimum size of point.
 * @param maxPointSizeOut
 *    Gets maximum size of point.
 */
void 
BrainOpenGL::getMinMaxPointSize(float& minPointSizeOut, float& maxPointSizeOut)
{
    minPointSizeOut = BrainOpenGL::s_minPointSize;
    maxPointSizeOut = BrainOpenGL::s_maxPointSize;
}

/**
 * Get the minimum and maximum values for the width of a line.
 * @param minLineWidthOut
 *    Gets minimum line width.
 * @param maxLineWidthOut
 *    Gets maximum line width.
 */
void 
BrainOpenGL::getMinMaxLineWidth(float& minLineWidthOut, float& maxLineWidthOut)
{
    minLineWidthOut = BrainOpenGL::s_minLineWidth;
    maxLineWidthOut = BrainOpenGL::s_maxLineWidth;
}

/**
 * Set the border being drawn by the user.  If not NULL then 
 * subclasses should draw the border.
 *
 * @param borderBeingDrawn
 *    Pointer to border that is being drawn.
 */
void 
BrainOpenGL::setBorderBeingDrawn(Border* borderBeingDrawn)
{
    this->borderBeingDrawn = borderBeingDrawn;
}

/**
 * @return Should border end points be highlighted?
 */
bool
BrainOpenGL::isDrawHighlightedEndPoints() const
{
    return m_drawHighlightedEndPoints;
}

/**
 * Set border end points should be highlighted.
 */
void
BrainOpenGL::setDrawHighlightedEndPoints(const bool drawHighlightedEndPoints)
{
    m_drawHighlightedEndPoints = drawHighlightedEndPoints;
}



/**
 * @return The runtime version of OpenGL (e.g. 1.0, 2.1, etc.)
 */
AString
BrainOpenGL::getRuntimeVersionOfOpenGL()
{
    return s_versionOfOpenGL;
}

/**
 * @return true if the version of OpenGL is supported at runtime.
 * @param versionNumber
 *    Version of OpenGL in the form Y.X (eg: 1.1, 2.1, 3.0, etc.)
 */
bool
BrainOpenGL::isRuntimeVersionOfOpenGLSupported(const AString& versionNumber)
{
    AString majorVersion;
    AString minorVersion;
    getOpenGLMajorMinorVersions(versionNumber,
                                majorVersion,
                                minorVersion);

    if (s_majorVersionOfOpenGL.toInt() >= majorVersion.toInt()) {
        return true;
    }
    else if (s_majorVersionOfOpenGL.toInt() == majorVersion.toInt()) {
        if (s_minorVersionOfOpenGL.toInt() >= minorVersion.toInt()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Extract the major and minor versions from an OpenGL version string.
 * @param versionString
 *   The OpenGL version string which is "<major>.<minor>[.optionalVendorInfo].
 *   Usually strings such as 1.1, 1.2, 2.1, 3.0, etc.
 * @param majorVersionOut
 *   Output containing the major version.
 * @param minorVersionOut
 *   Output containing the minor version.
 */
void
BrainOpenGL::getOpenGLMajorMinorVersions(const AString& versionString,
                                         AString& majorVersionOut,
                                         AString& minorVersionOut)
{
    /*
     * Major and minor version are separated by a period.
     * Vendor information may follow and begin with whitespace.
     */
    const QStringList sl = versionString.split(QRegExp("[\\s|\\.]"));
    
    if (sl.count() >= 2) {
        minorVersionOut = sl.at(1);
    }
    else {
        minorVersionOut = "1";
    }
    
    if (sl.count() >= 1) {
        majorVersionOut = sl.at(0);
    }
    else {
        majorVersionOut = "1";
    }
}

/**
 * Initialize the drawing mode using the most optimal drawing given
 * the compile time and run time constraints.
 */
void
BrainOpenGL::initializeOpenGL()
{
    AString compileVersions = "OpenGL Header File Versions Supported: ";
#ifdef GL_VERSION_1_1
    compileVersions += " 1.1";
#endif
#ifdef GL_VERSION_1_2
    compileVersions += " 1.2";
#endif
#ifdef GL_VERSION_1_3
    compileVersions += " 1.3";
#endif
#ifdef GL_VERSION_1_4
    compileVersions += " 1.4";
#endif
#ifdef GL_VERSION_1_5
    compileVersions += " 1.5";
#endif
#ifdef GL_VERSION_2_0
    compileVersions += " 2.0";
#endif
#ifdef GL_VERSION_2_1
    compileVersions += " 2.1";
#endif
#ifdef GL_VERSION_3_0
    compileVersions += " 3.0";
#endif
#ifdef GL_VERSION_3_1
    compileVersions += " 3.1";
#endif
#ifdef GL_VERSION_3_2
    compileVersions += " 3.2";
#endif
#ifdef GL_VERSION_3_3
    compileVersions += " 3.3";
#endif
#ifdef GL_VERSION_4_0
    compileVersions += " 4.0";
#endif
#ifdef GL_VERSION_4_1
    compileVersions += " 4.1";
#endif
#ifdef GL_VERSION_4_2
    compileVersions += " 4.2";
#endif
#ifdef GL_VERSION_4_3
    compileVersions += " 4.3";
#endif
#ifdef GL_VERSION_4_4
    compileVersions += " 4.4";
#endif
#ifdef GL_VERSION_4_4
    compileVersions += " 4.4";
#endif
#ifdef GL_VERSION_4_5
    compileVersions += " 4.5";
#endif
#ifdef GL_VERSION_5_0
    compileVersions += " 5.0";
#endif
    
#ifdef GL_OES_VERSION_1_0
    compileVersions += " ES_1.0";
#endif
#ifdef GL_ES_VERSION_2_0
    compileVersions += " ES_2.0";
#endif
#ifdef GL_ES_VERSION_3_0
    compileVersions += " ES_3.0";
#endif
    
    s_versionOfOpenGL = QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    if (s_versionOfOpenGL.isEmpty()) {
        s_versionOfOpenGL = "1.1";
    }
    getOpenGLMajorMinorVersions(s_versionOfOpenGL,
                                s_majorVersionOfOpenGL,
                                s_minorVersionOfOpenGL);
    
    //
    // Note: The version string might be something like 1.2.4.  std::atof()
    // will get just the 1.2 which is okay.
    //
    const char* vendorStr = (char*)(glGetString(GL_VENDOR));
    const char* renderStr = (char*)(glGetString(GL_RENDERER));
    AString lineInfo = (compileVersions
                        + "\nOpenGL Runtime Version: " + s_versionOfOpenGL
                        + "\nMajor Runtime Version: " + BrainOpenGL::s_majorVersionOfOpenGL
                        + "\nMinor Runtime Version: " + BrainOpenGL::s_minorVersionOfOpenGL
                        + "\nOpenGL Vendor: " + AString(vendorStr)
                        + "\nOpenGL Renderer: " + AString(renderStr));
    
    lineInfo += "\n";
#ifdef GL_VERSION_2_0
    if (isRuntimeVersionOfOpenGLSupported("2.0")) {
        GLfloat values[2];
        glGetFloatv (GL_ALIASED_LINE_WIDTH_RANGE, values);
        const AString aliasedLineWidthRange = ("GL_ALIASED_LINE_WIDTH_RANGE value is "
                                               + AString::fromNumbers(values, 2, ", "));
        
        glGetFloatv (GL_SMOOTH_LINE_WIDTH_RANGE, values);
        const AString smoothLineWidthRange = ("GL_SMOOTH_LINE_WIDTH_RANGE value is "
                                              + AString::fromNumbers(values, 2, ", "));
        
        glGetFloatv (GL_SMOOTH_LINE_WIDTH_GRANULARITY, values);
        const AString smoothLineWidthGranularity = ("GL_SMOOTH_LINE_WIDTH_GRANULARITY value is "
                                                    + AString::number(values[0]));
        
        lineInfo += ("\n" + aliasedLineWidthRange
                     + "\n" + smoothLineWidthRange
                     + "\n" + smoothLineWidthGranularity);
    }
#endif // GL_VERSION_2_0
//#else  // GL_VERSION_2_0
    GLfloat values[2];
    glGetFloatv (GL_LINE_WIDTH_RANGE, values);
    const AString lineWidthRange = ("GL_LINE_WIDTH_RANGE value is "
                                    + AString::fromNumbers(values, 2, ", "));
    
    glGetFloatv (GL_LINE_WIDTH_GRANULARITY, values);
    const AString lineWidthGranularity = ("GL_LINE_WIDTH_GRANULARITY value is "
                                          + AString::number(values[0]));
    lineInfo += ("\n" + lineWidthRange
                + "\n" + lineWidthGranularity);
//#endif // GL_VERSION_2_0
    
    float sizes[2];
    glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
    s_minPointSize = sizes[0];
    s_maxPointSize = sizes[1];
    glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
    s_minLineWidth = sizes[0];
    s_maxLineWidth = sizes[1];
    
    s_drawingMode = DRAW_MODE_INVALID;
    s_supportsDisplayLists = false;
    s_supportsImmediateMode = false;
    s_supportsVertexBuffers = false;
    
    /*
     * Get the OpenGL Extensions.
     */
    bool haveARBCompatibility = false;
    const QString extensionsString((char*)glGetString(GL_EXTENSIONS));
    const QStringList extensionsList = extensionsString.split(QChar(' '));
    QStringListIterator extensionsIterator(extensionsList);
    AString extInfo = ("\n\nOpenGL Extensions:");
    while (extensionsIterator.hasNext()) {
        const QString ext = extensionsIterator.next();
        extInfo += ("\n   " + ext);
        
        if (ext == "GL_ARB_compatibility") {
            haveARBCompatibility = true;
        }
    }
    
    if (isRuntimeVersionOfOpenGLSupported("3.1")) {
        if (haveARBCompatibility == false) {
            CaretLogSevere("OpenGL 3.1 or later and ARB compatibilty extensions not found.\n"
                           "OpenGL may fail.");
        }
    }
    
#if BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
    s_drawingMode = DRAW_MODE_IMMEDIATE;
    s_supportsImmediateMode = true;
#endif // BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
    
#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    s_drawingMode = DRAW_MODE_DISPLAY_LISTS;
    s_supportsDisplayLists = true;
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGL::isRuntimeVersionOfOpenGLSupported("2.1")) {
        s_drawingMode = DRAW_MODE_VERTEX_BUFFERS;
        s_supportsVertexBuffers = true;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    
    lineInfo += ("\n\nBest Drawing Mode: "
            + BrainOpenGL::getBestDrawingModeName());
    lineInfo += ("\nDisplay Lists Supported: "
            + AString::fromBool(s_supportsDisplayLists));
    lineInfo += ("\nImmediate Mode Supported: "
            + AString::fromBool(s_supportsImmediateMode));
    lineInfo += ("\nVertex Buffers Supported: "
            + AString::fromBool(s_supportsVertexBuffers));
    
    lineInfo += extInfo;
    
    CaretLogConfig(lineInfo);
    
    if (s_drawingMode == DRAW_MODE_INVALID) {
        CaretAssert(0);
    }
    
    s_openGLInformation = lineInfo;
}

/**
 * @return String containing information about OpenGL.
 */
AString
BrainOpenGL::getOpenGLInformation()
{
    return s_openGLInformation;
}


/**
 * @return Text string describing the drawing mode for shapes.
 */
QString
BrainOpenGL::getBestDrawingModeName()
{
    QString modeName = "Invalid";
    
    switch (s_drawingMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
            modeName = "Display Lists";
            break;
        case BrainOpenGL::DRAW_MODE_IMMEDIATE:
            modeName = "Immediate";
            break;
        case BrainOpenGL::DRAW_MODE_INVALID:
            modeName = "Invalid";
            break;
        case BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS:
            modeName = "Vertex Buffers";
            break;
    }
    
    return modeName;
}

