

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

#include <cstdlib>

#include <QStringList>

#define __BRAIN_OPENGL_DEFINE_H
#include "BrainOpenGL.h"
#undef __BRAIN_OPENGL_DEFINE_H

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "DummyFontTextRenderer.h"
#include "SessionManager.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param textRenderer
 *   The text renderer is used for text rendering.
 *   This parameter MUST NOT be NULL.  It must be
 *   a pointer to a text renderer.  This instance
 *   will take ownership of the text renderer and
 *   delete it at the appropriate time.
 */
BrainOpenGL::BrainOpenGL(BrainOpenGLTextRenderInterface* textRenderer)
{
    m_textRenderer = textRenderer;
    this->borderBeingDrawn = NULL;
    m_drawHighlightedEndPoints = false;
}

/**
 * Destructor.
 */
BrainOpenGL::~BrainOpenGL()
{
    if (m_textRenderer != NULL) {
        delete m_textRenderer;
        m_textRenderer = NULL;
    }
}

/**
 * @return The active text renderer.
 */
BrainOpenGLTextRenderInterface*
BrainOpenGL::getTextRenderer()
{
    return m_textRenderer;
}

/**
 * Set the text renderer.  The existing text renderer will
 * be destroyed.  This instance will take ownership of
 * the text renderer passed in and destory it at the
 * proper time.
 *
 * @param textRenderer
 *   The text renderer is used for text rendering.
 *   This parameter MUST NOT be NULL.  It must be
 *   a pointer to a text renderer.
 */
void
BrainOpenGL::setTextRenderer(BrainOpenGLTextRenderInterface* textRenderer)
{
    CaretAssert(textRenderer);
    
    if (m_textRenderer != NULL) {
        delete m_textRenderer;
        m_textRenderer = NULL;
    }
    
    m_textRenderer = textRenderer;
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
 * Determine if the given version of OpenGL is supported at runtime.
 * OpenGL is continually updated and this method is used to test for 
 * support of a given runtime version of OpenGL so that OpenGL functions
 * in the given version may be used.  For example, if a function in OpenGL
 * 2.1 is called on a system that it is OpenGL 1.1, a crash will likely occur.
 *
 * The OpenGL runtime version is two or three numbers separated by a period,
 * possibly followed by a space and then text.
 * The first number is the major version, the second number is the minor 
 * version, and the optional third number is the release of the major/minor 
 * version.
 *
 * A version of of OpenGL is supported when it is less than or equal to 
 * the runtime version of the OpenGL library.  However, there may be exceptions as 
 * OpenGL is deprecating functionality from OpenGL 1.x and 2.x in versions
 * 3.1 and later.  At this time, an extension is provided by all vendors so
 * that OpenGL 1.x and 2.x is available in 3.1 and later.  THIS MAY CHANGE.
 * This method does not check for this potential missing, deprecated capability.
 * 
 * @param versionOfOpenGL
 *    Version of OpenGL in the form X.Y.Z (eg: 1.1, 2.1, 3.0, 3.0.0, 3.1, etc.)
 *    for which support is tested.
 *
 * @return true if the given version of OpenGL is less than the runtime version.
 */
bool
BrainOpenGL::testForVersionOfOpenGLSupported(const AString& versionOfOpenGL)
{
    AString desiredMajorVersion;
    AString desiredMinorVersion;
    getOpenGLMajorMinorVersions(versionOfOpenGL,
                                desiredMajorVersion,
                                desiredMinorVersion);

    /*
     * If desired MAJOR version is LESS THAN the runtime MAJOR version
     * then is it supported.
     */
    if (desiredMajorVersion.toInt() < s_runtimeLibraryMajorVersionOfOpenGL.toInt()) {
        return true;
    }
    
    /*
     * Is the desired MAJOR version THE SAME as the runtime MAJOR version
     */
    if (s_runtimeLibraryMajorVersionOfOpenGL.toInt() == desiredMajorVersion.toInt()) {
        /*
         * If the desired MINOR version is LESS THAN OR EQUAL to the 
         * runtime MINOR version, then it is supported.
         */
        if (desiredMinorVersion.toInt() <= s_runtimeLibraryMinorVersionOfOpenGL.toInt()) {
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

///**
// * Initialize the drawing mode using the most optimal drawing given
// * the compile time and run time constraints.
// */
//void
//BrainOpenGL::initializeOpenGL()
//{
//    AString compileVersions = "OpenGL Header File Versions Supported: ";
//#ifdef GL_VERSION_1_1
//    compileVersions += " 1.1";
//#endif
//#ifdef GL_VERSION_1_2
//    compileVersions += " 1.2";
//#endif
//#ifdef GL_VERSION_1_3
//    compileVersions += " 1.3";
//#endif
//#ifdef GL_VERSION_1_4
//    compileVersions += " 1.4";
//#endif
//#ifdef GL_VERSION_1_5
//    compileVersions += " 1.5";
//#endif
//#ifdef GL_VERSION_2_0
//    compileVersions += " 2.0";
//#endif
//#ifdef GL_VERSION_2_1
//    compileVersions += " 2.1";
//#endif
//#ifdef GL_VERSION_3_0
//    compileVersions += " 3.0";
//#endif
//#ifdef GL_VERSION_3_1
//    compileVersions += " 3.1";
//#endif
//#ifdef GL_VERSION_3_2
//    compileVersions += " 3.2";
//#endif
//#ifdef GL_VERSION_3_3
//    compileVersions += " 3.3";
//#endif
//#ifdef GL_VERSION_4_0
//    compileVersions += " 4.0";
//#endif
//#ifdef GL_VERSION_4_1
//    compileVersions += " 4.1";
//#endif
//#ifdef GL_VERSION_4_2
//    compileVersions += " 4.2";
//#endif
//#ifdef GL_VERSION_4_3
//    compileVersions += " 4.3";
//#endif
//#ifdef GL_VERSION_4_4
//    compileVersions += " 4.4";
//#endif
//#ifdef GL_VERSION_4_4
//    compileVersions += " 4.4";
//#endif
//#ifdef GL_VERSION_4_5
//    compileVersions += " 4.5";
//#endif
//#ifdef GL_VERSION_5_0
//    compileVersions += " 5.0";
//#endif
//    
//#ifdef GL_OES_VERSION_1_0
//    compileVersions += " ES_1.0";
//#endif
//#ifdef GL_ES_VERSION_2_0
//    compileVersions += " ES_2.0";
//#endif
//#ifdef GL_ES_VERSION_3_0
//    compileVersions += " ES_3.0";
//#endif
//    
//    s_runtimeLibraryVersionOfOpenGL = QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
//    if (s_runtimeLibraryVersionOfOpenGL.isEmpty()) {
//        s_runtimeLibraryVersionOfOpenGL = "1.1";
//    }
//    getOpenGLMajorMinorVersions(s_runtimeLibraryVersionOfOpenGL,
//                                s_runtimeLibraryMajorVersionOfOpenGL,
//                                s_runtimeLibraryMinorVersionOfOpenGL);
//    
//    //
//    // Note: The version string might be something like 1.2.4.  std::atof()
//    // will get just the 1.2 which is okay.
//    //
//    const char* vendorStr = (char*)(glGetString(GL_VENDOR));
//    const char* renderStr = (char*)(glGetString(GL_RENDERER));
//    AString lineInfo = (compileVersions
//                        + "\nOpenGL Runtime Version: " + s_runtimeLibraryVersionOfOpenGL
//                        + "\nMajor Runtime Version: " + BrainOpenGL::s_runtimeLibraryMajorVersionOfOpenGL
//                        + "\nMinor Runtime Version: " + BrainOpenGL::s_runtimeLibraryMinorVersionOfOpenGL
//                        + "\nOpenGL Vendor: " + AString(vendorStr)
//                        + "\nOpenGL Renderer: " + AString(renderStr));
//    
//    lineInfo += "\n";
//    lineInfo += ("\nFont Renderer: " + m_textRenderer->getName());
//    lineInfo += "\n";
//    
//#ifdef GL_VERSION_2_0
//    if (testForVersionOfOpenGLSupported("2.0")) {
//        GLfloat values[2];
//        glGetFloatv (GL_ALIASED_LINE_WIDTH_RANGE, values);
//        const AString aliasedLineWidthRange = ("GL_ALIASED_LINE_WIDTH_RANGE value is "
//                                               + AString::fromNumbers(values, 2, ", "));
//        
//        glGetFloatv (GL_SMOOTH_LINE_WIDTH_RANGE, values);
//        const AString smoothLineWidthRange = ("GL_SMOOTH_LINE_WIDTH_RANGE value is "
//                                              + AString::fromNumbers(values, 2, ", "));
//        
//        glGetFloatv (GL_SMOOTH_LINE_WIDTH_GRANULARITY, values);
//        const AString smoothLineWidthGranularity = ("GL_SMOOTH_LINE_WIDTH_GRANULARITY value is "
//                                                    + AString::number(values[0]));
//        
//        lineInfo += ("\n" + aliasedLineWidthRange
//                     + "\n" + smoothLineWidthRange
//                     + "\n" + smoothLineWidthGranularity);
//    }
//#endif // GL_VERSION_2_0
////#else  // GL_VERSION_2_0
//    GLfloat values[2];
//    glGetFloatv (GL_LINE_WIDTH_RANGE, values);
//    const AString lineWidthRange = ("GL_LINE_WIDTH_RANGE value is "
//                                    + AString::fromNumbers(values, 2, ", "));
//    
//    glGetFloatv (GL_LINE_WIDTH_GRANULARITY, values);
//    const AString lineWidthGranularity = ("GL_LINE_WIDTH_GRANULARITY value is "
//                                          + AString::number(values[0]));
//    lineInfo += ("\n" + lineWidthRange
//                + "\n" + lineWidthGranularity);
////#endif // GL_VERSION_2_0
//    
//    float sizes[2];
//    glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
//    s_minPointSize = sizes[0];
//    s_maxPointSize = sizes[1];
//    glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
//    s_minLineWidth = sizes[0];
//    s_maxLineWidth = sizes[1];
//    
//    s_supportsDisplayLists = false;
//    s_supportsImmediateMode = false;
//    s_supportsVertexBuffers = false;
//    
//    GLint maximumNumberOfClipPlanes;
//    glGetIntegerv(GL_MAX_CLIP_PLANES,
//                  & maximumNumberOfClipPlanes);
//    lineInfo += ("\n\nMaximum number of clipping planes is "
//                 + AString::number(maximumNumberOfClipPlanes));
//    
//    GLint redBits, greenBits, blueBits, alphaBits;
//    glGetIntegerv(GL_RED_BITS,   &redBits);
//    glGetIntegerv(GL_GREEN_BITS, &greenBits);
//    glGetIntegerv(GL_BLUE_BITS,  &blueBits);
//    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
//    lineInfo += ("\n\nBuffer bits red/green/blue/apha: ("
//                 + AString::number(redBits) + ", "
//                 + AString::number(greenBits) + ", "
//                 + AString::number(blueBits) + ", "
//                 + AString::number(alphaBits) + ")");
//    
//    /*
//     * Get the OpenGL Extensions.
//     */
//    bool haveARBCompatibility = false;
//    const QString extensionsString((char*)glGetString(GL_EXTENSIONS));
//    const QStringList extensionsList = extensionsString.split(QChar(' '));
//    QStringListIterator extensionsIterator(extensionsList);
//    AString extInfo = ("\n\nOpenGL Extensions:");
//    while (extensionsIterator.hasNext()) {
//        const QString ext = extensionsIterator.next();
//        extInfo += ("\n   " + ext);
//        
//        if (ext == "GL_ARB_compatibility") {
//            haveARBCompatibility = true;
//        }
//    }
//    
//    if (testForVersionOfOpenGLSupported("3.1")) {
//        if (haveARBCompatibility == false) {
//            CaretLogSevere("OpenGL 3.1 or later and ARB compatibilty extensions not found.\n"
//                           "OpenGL may fail.");
//        }
//    }
//
//#if BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
//    s_supportsImmediateMode = true;
//#endif // BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
//    
//#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
//    s_supportsDisplayLists = true;
//#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
//    
//#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
//    s_supportsVertexBuffers = true;
//#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
//    
//    lineInfo += ("\n\nBest Drawing Mode: "
//            + BrainOpenGL::getBestDrawingModeName());
//    lineInfo += ("\nDisplay Lists Supported: "
//            + AString::fromBool(s_supportsDisplayLists));
//    lineInfo += ("\nImmediate Mode Supported: "
//            + AString::fromBool(s_supportsImmediateMode));
//    lineInfo += ("\nVertex Buffers Supported: "
//            + AString::fromBool(s_supportsVertexBuffers));
//    
//    lineInfo += extInfo;
//    
//    CaretLogConfig(lineInfo);
//    
//    m_openGLInformation = lineInfo;
//    
//    /*
//     * Call to validate the draw mode selection logic.
//     */
//    getBestDrawingMode();
//}

/**
 * Initialize the drawing mode using the most optimal drawing given
 * the compile time and run time constraints.
 */
void
BrainOpenGL::initializeOpenGL()
{

    s_runtimeLibraryVersionOfOpenGL = QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    if (s_runtimeLibraryVersionOfOpenGL.isEmpty()) {
        s_runtimeLibraryVersionOfOpenGL = "1.1";
    }
    getOpenGLMajorMinorVersions(s_runtimeLibraryVersionOfOpenGL,
                                s_runtimeLibraryMajorVersionOfOpenGL,
                                s_runtimeLibraryMinorVersionOfOpenGL);


    float sizes[2];
    glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
    s_minPointSize = sizes[0];
    s_maxPointSize = sizes[1];
    glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
    s_minLineWidth = sizes[0];
    s_maxLineWidth = sizes[1];

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
    m_openGLExtensionsInformation = ("\n\nOpenGL Extensions:");
    while (extensionsIterator.hasNext()) {
        const QString ext = extensionsIterator.next();
        m_openGLExtensionsInformation += ("\n   " + ext);

        if (ext == "GL_ARB_compatibility") {
            haveARBCompatibility = true;
        }
    }

    if (testForVersionOfOpenGLSupported("3.1")) {
        if (haveARBCompatibility == false) {
            CaretLogSevere("OpenGL 3.1 or later and ARB compatibilty extensions not found.\n"
                           "OpenGL may fail.");
        }
    }

#if BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
    s_supportsImmediateMode = true;
#endif // BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE

#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    s_supportsDisplayLists = true;
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS

#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    s_supportsVertexBuffers = true;
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS

    /*
     * Call to validate the draw mode selection logic.
     */
    getBestDrawingMode();
}

/**
 * @return String containing information about OpenGL.
 */
AString
BrainOpenGL::getOpenGLInformation()
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

    //
    // Note: The version string might be something like 1.2.4.  std::atof()
    // will get just the 1.2 which is okay.
    //
    const char* vendorStr = (char*)(glGetString(GL_VENDOR));
    const char* renderStr = (char*)(glGetString(GL_RENDERER));
    AString lineInfo = (compileVersions
                        + "\nOpenGL Runtime Version: " + s_runtimeLibraryVersionOfOpenGL
                        + "\nMajor Runtime Version: " + BrainOpenGL::s_runtimeLibraryMajorVersionOfOpenGL
                        + "\nMinor Runtime Version: " + BrainOpenGL::s_runtimeLibraryMinorVersionOfOpenGL
                        + "\nOpenGL Vendor: " + AString(vendorStr)
                        + "\nOpenGL Renderer: " + AString(renderStr));
    
    lineInfo += "\n";
    lineInfo += ("\nFont Renderer: " + m_textRenderer->getName());
    lineInfo += "\n";
    
#ifdef GL_VERSION_2_0
    if (testForVersionOfOpenGLSupported("2.0")) {
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

    
    GLint maximumNumberOfClipPlanes;
    glGetIntegerv(GL_MAX_CLIP_PLANES,
                  & maximumNumberOfClipPlanes);
    lineInfo += ("\n\nMaximum number of clipping planes is "
                 + AString::number(maximumNumberOfClipPlanes));
    
    GLint maxNameStackDepth, maxModelStackDepth, maxProjStackDepth;
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,
                  &maxProjStackDepth);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,
                  &maxModelStackDepth);
    glGetIntegerv(GL_MAX_NAME_STACK_DEPTH,
                  &maxNameStackDepth);
    lineInfo += ("\n\nMaximum Modelview Matrix Stack Depth "
                 + QString::number(maxModelStackDepth));
    lineInfo += ("\nMaximum Name Matrix Stack Depth "
                 + QString::number(maxNameStackDepth));
    lineInfo += ("\nMaximum Projection Matrix Stack Depth "
                 + QString::number(maxProjStackDepth));
    
    GLint redBits, greenBits, blueBits, alphaBits;
    glGetIntegerv(GL_RED_BITS,   &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS,  &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    lineInfo += ("\n\nBuffer bits red/green/blue/apha: ("
                 + AString::number(redBits) + ", "
                 + AString::number(greenBits) + ", "
                 + AString::number(blueBits) + ", "
                 + AString::number(alphaBits) + ")");
    
    lineInfo += ("\n\nBest Drawing Mode: "
                 + BrainOpenGL::getBestDrawingModeName());
    lineInfo += ("\nDisplay Lists Supported: "
                 + AString::fromBool(s_supportsDisplayLists));
    lineInfo += ("\nImmediate Mode Supported: "
                 + AString::fromBool(s_supportsImmediateMode));
    lineInfo += ("\nVertex Buffers Supported: "
                 + AString::fromBool(s_supportsVertexBuffers));
    
    lineInfo += "\n";
    lineInfo += "\n";
    lineInfo += "Note that State of OpenGL may be different when drawing objects.\n";
    lineInfo += getStateOfOpenGL();
    lineInfo += "\n";
    
    lineInfo += m_openGLExtensionsInformation;
    
    
    return lineInfo;
}

/**
 * @return The best drawing mode given the limitations of
 * the compile and run-time systems.
 */
BrainOpenGL::DrawMode
BrainOpenGL::getBestDrawingMode() {
    BrainOpenGL::DrawMode drawMode = BrainOpenGL::DRAW_MODE_INVALID;
    
    const OpenGLDrawingMethodEnum::Enum userPrefDrawMode = SessionManager::get()->getCaretPreferences()->getOpenDrawingMethod();
    
    bool useVertexBuffersFlag = false;
    switch (userPrefDrawMode) {
        case OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_OFF:
            break;
        case OpenGLDrawingMethodEnum::DRAW_WITH_VERTEX_BUFFERS_ON:
            if (s_supportsVertexBuffers) {
                useVertexBuffersFlag = true;
            }
            break;
    }
    
    if (s_supportsVertexBuffers
        && useVertexBuffersFlag) {
        drawMode = DRAW_MODE_VERTEX_BUFFERS;
    }
    else if (s_supportsDisplayLists) {
        drawMode = DRAW_MODE_DISPLAY_LISTS;
    }
    else if (s_supportsImmediateMode) {
        drawMode = DRAW_MODE_IMMEDIATE;
    }
    else if (s_supportsVertexBuffers) {
        drawMode = DRAW_MODE_VERTEX_BUFFERS;
    }
    else {
        CaretAssertMessage(0,
                           "OpenGL does not appear to support any valid drawing modes, should never occur."
                           "  Or, OpenGL was not initialized (failed to call BrainOpenGL::initializeOpenGL()).");
    }
    
    return drawMode;
}

/**
 * @return Text string describing the drawing mode for shapes.
 */
QString
BrainOpenGL::getBestDrawingModeName()
{
    QString modeName = "Invalid";
    
    switch (getBestDrawingMode()) {
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

/**
 * @return A string containing the state of OpenGL (depth testing, lighting, etc.)
 */
AString
BrainOpenGL::getStateOfOpenGL() const
{
    AString s;
    
    return s;
}

/**
 * Get the status of an OpenGL enum (enabled/disabled) as text in form
 * "name=true/false".
 *
 * @param enumName
 *     Text name of the enumerated value.
 * @param enumValue
 *     The OpenGl enum value.
 * @return
 *     String with "enumName=true/false".
 */
AString BrainOpenGL::getOpenGLEnabledEnumAsText(const AString& enumName,
                            const GLenum enumValue) const
{
    /*
     * Reset error status
     */
    glGetError();
    
    GLboolean boolValue= glIsEnabled(enumValue);
    
    AString errorText;
    const GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        const GLubyte* errorChars = gluErrorString(errorCode);
        if (errorChars != NULL) {
            errorText = ("ERROR = "
                         + AString((char*)errorChars));
        }
    }
    
    const AString s = (enumName
                       + "="
                       + ((boolValue == GL_TRUE) ? "true" : "false")
                       + " "
                       + errorText);
    return s;
}

/**
 * Get the status of an OpenGL boolean as text in form
 * "name=true/false".
 *
 * @param enumName
 *     Text name of the enumerated value.
 * @param enumValue
 *     The OpenGl enum value.
 * @return
 *     String with "enumName=true/false".
 */
AString BrainOpenGL::getOpenGLBooleanAsText(const AString& enumName,
                                                const GLenum enumValue) const
{
    /*
     * Reset error status
     */
    glGetError();
    
    GLboolean boolValue = GL_FALSE;
    glGetBooleanv(enumValue, &boolValue);
    
    AString errorText;
    const GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        const GLubyte* errorChars = gluErrorString(errorCode);
        if (errorChars != NULL) {
            errorText = ("ERROR = "
                         + AString((char*)errorChars));
        }
    }
    
    const AString s = (enumName
                       + "="
                       + ((boolValue == GL_TRUE) ? "true" : "false")
                       + " "
                       + errorText);
    return s;
}

/**
 * Get the status of an OpenGL enum (enabled/disabled) as text in form
 * "name=true/false".
 *
 * @param enumName
 *     Text name of the enumerated value.
 * @param enumValue
 *     The OpenGl enum value.
 * @param numberOfValues
 *     Number of floating pointer values associated with enumName.
 * @return
 *     String with "enumName=(1, 2, 3,..)".
 */
AString BrainOpenGL::getOpenGLFloatAsText(const AString& enumName,
                                          const GLenum enumValue,
                                          const int32_t numberOfValues) const
{
    /*
     * Reset error status
     */
    glGetError();
    
    AString valuesString;
    
    if (numberOfValues > 0) {
        std::vector<GLfloat> valuesVector(numberOfValues,
                                          0.0);
        GLfloat* values = &valuesVector[0];
        glGetFloatv(enumValue, values);

        const GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) {
            const GLubyte* errorChars = gluErrorString(errorCode);
            if (errorChars != NULL) {
                valuesString = ("ERROR = "
                             + AString((char*)errorChars));
            }
        }
        else {
            valuesString = ("("
                            + AString::fromNumbers(valuesVector, ",")
                            + ")");
        }
    }
    
    const AString s = (enumName
                       + "="
                       + valuesString);
    return s;
}

/**
 * Get the status of an OpenGL enum (enabled/disabled) as text in form
 * "name=true/false".
 *
 * @param enumName
 *     Text name of the enumerated value.
 * @param enumValue
 *     The OpenGl enum value.
 * @param enumValue
 *     The OpenGl enum value.
 * @param numberOfValues
 *     Number of floating pointer values associated with enumName.
 * @return
 *     String with "enumName=(1, 2, 3,..)".
 */
AString BrainOpenGL::getOpenGLLightAsText(const AString& enumName,
                                          const GLenum lightEnum,
                                          const GLenum enumValue,
                                          const int32_t numberOfValues) const
{
    /*
     * Reset error status
     */
    glGetError();
    
    AString valuesString;
    
    if (numberOfValues > 0) {
        std::vector<GLfloat> valuesVector(numberOfValues,
                                          0.0);
        GLfloat* values = &valuesVector[0];
        glGetLightfv(lightEnum,
                     enumValue,
                     values);
        
        const GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) {
            const GLubyte* errorChars = gluErrorString(errorCode);
            if (errorChars != NULL) {
                valuesString = ("ERROR = "
                                + AString((char*)errorChars));
            }
        }
        else {
            valuesString = ("("
                            + AString::fromNumbers(valuesVector, ",")
                            + ")");
        }
    }
    
    const AString s = (enumName
                       + "="
                       + valuesString);
    return s;
}

/**
 * Get the background color.
 *
 * @param backgroundColor
 *     Output containing RGB components [0, 255].
 */
void
BrainOpenGL::getBackgroundColor(uint8_t backgroundColor[3]) const
{
    backgroundColor[0] = m_backgroundColorByte[0];
    backgroundColor[1] = m_backgroundColorByte[1];
    backgroundColor[2] = m_backgroundColorByte[2];
}


