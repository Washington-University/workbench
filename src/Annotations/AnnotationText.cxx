
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

#define __ANNOTATION_TEXT_DECLARE__
#include "AnnotationText.h"
#undef __ANNOTATION_TEXT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationText 
 * \brief Text annotation.
 * \ingroup Annotations
 */

///**
// * Constructor for a text annotation.
// */
//AnnotationText::AnnotationText()
//: AnnotationTwoDimensionalShape(AnnotationTypeEnum::TEXT),
//m_fontSizeType(AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT)
//{
//    initializeAnnotationTextMembers();
//}

/**
 * Constructor for subclass.
 *
 * @param fontSizeType
 *    Type of font sizing.
 */
AnnotationText::AnnotationText(const AnnotationTextFontSizeTypeEnum::Enum fontSizeType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::TEXT),
m_fontSizeType(fontSizeType)
{
    initializeAnnotationTextMembers();
}

/**
 * Destructor.
 */
AnnotationText::~AnnotationText()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationText::AnnotationText(const AnnotationText& obj)
: AnnotationTwoDimensionalShape(obj),
m_fontSizeType(obj.m_fontSizeType)
{
    initializeAnnotationTextMembers();
    this->copyHelperAnnotationText(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationText&
AnnotationText::operator=(const AnnotationText& obj)
{
    if (this != &obj) {
        Annotation::operator=(obj);
        this->copyHelperAnnotationText(obj);
    }
    return *this;
}

/**
 * Initialize members of this class.
 */
void
AnnotationText::initializeAnnotationTextMembers()
{
    m_text                    = "";
    m_alignmentHorizontal     = s_defaultAlignmentHorizontal;
    m_alignmentVertical       = s_defaultAlignmentVertical;
    m_font                    = s_defaultFont;
    m_fontPointSize           = s_defaultPointSize;
    m_orientation             = s_defaultOrientation;
    m_boldEnabled             = s_defaultBoldEnabled;
    m_italicEnabled           = s_defaultItalicEnabled;
    m_underlineEnabled        = s_defaultUnderlineEnabled;
    m_connectToBrainordinate  = s_defaultConnectToBrainordinate;
    m_fontPercentViewportSize = s_defaultFontPercentViewportSize;
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_text",
                          &m_text);
    m_sceneAssistant->add<AnnotationTextAlignHorizontalEnum>("m_alignmentHorizontal",
                                                             &m_alignmentHorizontal);
    m_sceneAssistant->add<AnnotationTextAlignVerticalEnum>("m_alignmentVertical",
                                                           &m_alignmentVertical);
    m_sceneAssistant->add<AnnotationTextFontNameEnum>("m_font",
                                                  &m_font);
    m_sceneAssistant->add<AnnotationTextFontPointSizeEnum>("m_fontPointSize",
                                                  &m_fontPointSize);
    m_sceneAssistant->add<AnnotationTextOrientationEnum>("m_orientation",
                                                         &m_orientation);
    m_sceneAssistant->add<AnnotationTextConnectTypeEnum>("m_connectToBrainordinate",
                                                         &m_connectToBrainordinate);
    m_sceneAssistant->add("m_boldEnabled",
                          &m_boldEnabled);
    m_sceneAssistant->add("m_italicEnabled",
                          &m_italicEnabled);
    m_sceneAssistant->add("m_underlineEnabled",
                          &m_underlineEnabled);
    m_sceneAssistant->add("m_fontPercentViewportSize",
                          &m_fontPercentViewportSize);
}

/**
 * Get an encoded name that contains the
 * name of the font, the font height, and the
 * font style used by font rendering to provide
 * a name for cached fonts.
 *
 * The text annotation contains the height of the viewport
 * when the annotation was created.  When it is valid (
 * greater than zero), and the drawing viewport height
 * is also valid (greater than zero), a scaling value
 * (equal to drawing viewport height divided by creation
 * viewport height) is applied to the font size so that
 * the font can scale as the viewport change in size.
 * Thus, if the viewport becomes larger (smaller), the
 * text will become larger (smaller).  While one could
 * scale while drawing the text (using glScale()), the
 * quality is poor.
 *
 * @param drawingViewportHeight
 *      Height of the viewport that may be used to scale the font height.
 * @return
 *      Encoded name for font.
 */
AString
AnnotationText::getFontRenderingEncodedName(const float drawingViewportHeight) const
{
    AString fontSizeID = AnnotationTextFontPointSizeEnum::toName(m_fontPointSize);
    
    if (m_fontPercentViewportSize > 0.0) {
        if (drawingViewportHeight> 0.0) {
            const int32_t fontSizeInt = getFontSizeForDrawing(drawingViewportHeight);
            if (fontSizeInt > 0) {
                fontSizeID = "SIZE" + AString::number(fontSizeInt);
            }
        }
    }
    
    AString encodedName;
    encodedName.reserve(50);
    
    encodedName.append(AnnotationTextFontNameEnum::toName(m_font));
    
    encodedName.append("_" + fontSizeID);
    
    if (m_boldEnabled) {
        encodedName.append("_B");
    }
    
    if (m_italicEnabled) {
        encodedName.append("_I");
    }
    
    if (m_underlineEnabled) {
        encodedName.append("_U");
    }
    
    return encodedName;
}

/**
 * @return Text for the annotation.
 */
AString
AnnotationText::getText() const
{
    return m_text;
}

/**
 * Set the text for an annotation.
 * @param text
 *    Text for the annotation.
 */
void
AnnotationText::setText(const AString& text)
{
    if (text != m_text) {
        m_text = text;
        setModified();
    }
}

/**
 * @return The horizontal alignment.
 */
AnnotationTextAlignHorizontalEnum::Enum
AnnotationText::getHorizontalAlignment() const
{
    return m_alignmentHorizontal;
}

/**
 * Set the horizontal alignment.
 *
 * @param alignment
 *    New value for horizontal alignment.
 */
void
AnnotationText::setHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment)
{
    if (m_alignmentHorizontal != alignment) {
        m_alignmentHorizontal = alignment;
        setModified();
    }
}

/**
 * @return The vertical alignment.
 */
AnnotationTextAlignVerticalEnum::Enum
AnnotationText::getVerticalAlignment() const
{
    return m_alignmentVertical;
}

/**
 * Set the vertical alignment.
 *
 * @param alignment
 *    New value for vertical alignment.
 */
void
AnnotationText::setVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment)
{
    if (m_alignmentVertical != alignment) {
        m_alignmentVertical = alignment;
        setModified();
    }
}

/**
 * @param connect to brainordinate status (none, line, arrow).
 */
AnnotationTextConnectTypeEnum::Enum
AnnotationText::getConnectToBrainordinate() const
{
    return m_connectToBrainordinate;
}

/**
 * Set the connect to brainordinate status.
 *
 * @param connectToBrainordinate
 *    New value for connection to brainordinate (none, line, arrow)
 */
void
AnnotationText::setConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate)
{
    if (m_connectToBrainordinate != connectToBrainordinate) {
        m_connectToBrainordinate = connectToBrainordinate;
        setModified();
    }
}

/**
 * @return Is connect to brainordinate valid for this text annotation.
 */
bool
AnnotationText::isConnectToBrainordinateValid() const
{
    if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE) {
        return true;
    }
    return false;
}

/**
 * @return The font.
 */
AnnotationTextFontNameEnum::Enum
AnnotationText::getFont() const
{
    return m_font;
}

/**
 * Set the font for an annotation.
 
 * @param font
 *    Font for the annotation.
 */
void
AnnotationText::setFont(const AnnotationTextFontNameEnum::Enum font)
{
    if (font != m_font) {
        m_font = font;
        setModified();
    }
}

/**
 * Get the font size for drawing.  The font size may
 * be scaled to "best fit" the viewport.
 *
 * For a Point Size Text Annotation, the size returned is the text annotation's
 * point size.
 * 
 * For a Percentage Size Text Annotation, the size returned will be 
 * "percent size" of the viewport height in a range from zero to 
 * one where one equivalent to the viewport's height.
 *
 * @param drawingViewportHeight
 *      Height of the viewport that may be used to scale the font height.
 * @return
 *     Size of the font.
 */
int32_t
AnnotationText::getFontSizeForDrawing(const int32_t drawingViewportHeight) const
{
    float sizeForDrawing = AnnotationTextFontPointSizeEnum::toSizeNumeric(AnnotationTextFontPointSizeEnum::SIZE14);
    
    switch (m_fontSizeType) {
        case AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT:
        {
            /*
             * May need pixel to points conversion if not 72 DPI
             */
            const float pixelSize = drawingViewportHeight * m_fontPercentViewportSize;
            sizeForDrawing = pixelSize;
        }
            break;
        case AnnotationTextFontSizeTypeEnum::POINTS:
            sizeForDrawing = AnnotationTextFontPointSizeEnum::toSizeNumeric(m_fontPointSize);
            break;
    }
    
    if (sizeForDrawing < AnnotationTextFontPointSizeEnum::getMinimumSizeNumeric()) {
        sizeForDrawing = AnnotationTextFontPointSizeEnum::getMinimumSizeNumeric();
    }
    
    const int32_t sizeInt = static_cast<int32_t>(MathFunctions::round(sizeForDrawing));
    return sizeInt;
}

/**
 * @return The font point size.
 */
AnnotationTextFontPointSizeEnum::Enum
AnnotationText::getFontPointSizeProtected() const
{
    return m_fontPointSize;
}

/**
 * Set the font point size.
 *
 * @param fontPointSize
 *     New font point size.
 */
void
AnnotationText::setFontPointSizeProtected(const AnnotationTextFontPointSizeEnum::Enum fontPointSize)
{
    if (fontPointSize != m_fontPointSize) {
        m_fontPointSize = fontPointSize;
        setModified();
    }
}

/**
 * @param The font size type.
 */
AnnotationTextFontSizeTypeEnum::Enum
AnnotationText::getFontSizeType() const
{
    return m_fontSizeType;
}

/**
 * @return The text orientation.
 */
AnnotationTextOrientationEnum::Enum
AnnotationText::getOrientation() const
{
    return m_orientation;
}

/**
 * Set the text orientation.
 *
 * @param orientation
 *     New value for orientation.
 */
void
AnnotationText::setOrientation(const AnnotationTextOrientationEnum::Enum orientation)
{
    if (orientation != m_orientation) {
        m_orientation = orientation;
        setModified();
    }
}

/**
 * @return
 *    Is bold enabled ?
 */
bool
AnnotationText::isBoldEnabled() const
{
    return m_boldEnabled;
}

/**
 * Set bold enabled.
 *
 * @param enabled
 *     New status for bold enabled.
 */
void
AnnotationText::setBoldEnabled(const bool enabled)
{
    if (enabled != m_boldEnabled) {
        m_boldEnabled = enabled;
        setModified();
    }
}

/**
 * @return
 *    Is italic enabled ?
 */
bool
AnnotationText::isItalicEnabled() const
{
    return m_italicEnabled;
}

/**
 * Set italic enabled.
 *
 * @param enabled
 *     New status for italic enabled.
 */
void
AnnotationText::setItalicEnabled(const bool enabled)
{
    if (enabled != m_italicEnabled) {
        m_italicEnabled = enabled;
        setModified();
    }
}

/**
 * @return
 *    Is underline enabled ?
 */
bool
AnnotationText::isUnderlineEnabled() const
{
    return m_underlineEnabled;
}

/**
 * @return Is foreground line width supported?
 * Most annotations support a foreground line width.
 * Annotations that do not support a foreground line width
 * must override this method and return a value of false.
 */
bool
AnnotationText::isForegroundLineWidthSupported() const
{
    return true;
}


/**
 * Set underline enabled.
 *
 * @param enabled
 *     New status for underline enabled.
 */
void
AnnotationText::setUnderlineEnabled(const bool enabled)
{
    if (enabled != m_underlineEnabled) {
        m_underlineEnabled = enabled;
        setModified();
    }
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationText::copyHelperAnnotationText(const AnnotationText& obj)
{
    m_text                = obj.m_text;
    m_alignmentHorizontal = obj.m_alignmentHorizontal;
    m_alignmentVertical   = obj.m_alignmentVertical;
    m_font                = obj.m_font;
    m_fontPointSize       = obj.m_fontPointSize;
    m_orientation         = obj.m_orientation;
    m_boldEnabled         = obj.m_boldEnabled;
    m_italicEnabled       = obj.m_italicEnabled;
    m_underlineEnabled    = obj.m_underlineEnabled;
    m_connectToBrainordinate = obj.m_connectToBrainordinate;
    m_fontPercentViewportSize = obj.m_fontPercentViewportSize;
}

/**
 * Apply the coordinates, size, and rotation from the given annotation
 * to this annotation.
 *
 * @param otherAnnotation
 *     The other annotation from which attributes are obtained.
 */
void
AnnotationText::applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation)
{
    AnnotationTwoDimensionalShape::applyCoordinatesSizeAndRotationFromOther(otherAnnotation);
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one.
 */
float
AnnotationText::getFontPercentViewportSizeProtected() const
{
    return m_fontPercentViewportSize;
}

/**
 * Set the size of the font as a percentage of the viewport height.
 *
 * @param fontPercentViewportHeight
 *    New value for percentage of viewport height.
 *    Range is zero to one.
 */
void
AnnotationText::setFontPercentViewportSizeProtected(const float fontPercentViewportHeight)
{
    if ((fontPercentViewportHeight < 0.0)
        || (fontPercentViewportHeight > 1.0)) {
        const QString msg("Percent viewport height should range [0.0, 1.0] but value is "
                          + QString::number(fontPercentViewportHeight));
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
    }
    
    if (fontPercentViewportHeight != m_fontPercentViewportSize) {
        m_fontPercentViewportSize = fontPercentViewportHeight;
        setModified();
    }
}

/**
 * Apply a move or resize operation received from the GUI.
 *
 * @param handleSelected
 *     Annotatoion handle that is being dragged by the user.
 * @param viewportWidth
 *     Width of viewport
 * @param viewportHeight
 *     Height of viewport
 * @param mouseX
 *     Mouse X-coordinate.
 * @param mouseY
 *     Mouse Y-coordinate.
 * @param mouseDX
 *     Change in mouse X-coordinate.
 * @param mouseDY
 *     Change in mouse Y-coordinate.
 */
void
AnnotationText::applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                         const float viewportWidth,
                                         const float viewportHeight,
                                         const float mouseX,
                                         const float mouseY,
                                         const float mouseDX,
                                         const float mouseDY)
{
    /*
     * Text limits support of resize options
     */
    bool operationSupportedFlag = false;
    
    switch (handleSelected) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            operationSupportedFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            operationSupportedFlag = true;
            break;
    }
    
    if (operationSupportedFlag) {
        AnnotationTwoDimensionalShape::applyMoveOrResizeFromGUI(handleSelected,
                                                                viewportWidth,
                                                                viewportHeight,
                                                                mouseX,
                                                                mouseY,
                                                                mouseDX,
                                                                mouseDY);
    }
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationText::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationText::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Set the default value for horizontal alignment.
 *
 * @param alignment
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment)
{
    s_defaultAlignmentHorizontal = alignment;
}

/**
 * Set the default value for vertical alignment.
 *
 * @param alignment
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment)
{
    s_defaultAlignmentVertical = alignment;
}

/**
 * Set the default value for the font name.
 *
 * @param font
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultFont(const AnnotationTextFontNameEnum::Enum font)
{
    s_defaultFont = font;
}

/**
 * Set the default value for the text orientation.
 *
 * @param orientation
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultOrientation(const AnnotationTextOrientationEnum::Enum orientation)
{
    s_defaultOrientation = orientation;
}

/**
 * Set the default value for font point size.
 *
 * @param fontPointSize
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize)
{
    s_defaultPointSize = fontPointSize;
}

/**
 * Set the default value for font percent viewport height.
 *
 * @param fontPercentViewportHeight
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultFontPercentViewportSize(const float fontPercentViewportHeight)
{
    s_defaultFontPercentViewportSize = fontPercentViewportHeight;
}

/**
 * Set the default value for bold enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultBoldEnabled(const bool enabled)
{
    s_defaultBoldEnabled = enabled;
}

/**
 * Set the default value for italic enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultItalicEnabled(const bool enabled)
{
    s_defaultItalicEnabled = enabled;
}

/**
 * Set the default value for underline enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultUnderlineEnabled(const bool enabled)
{
    s_defaultUnderlineEnabled = enabled;
}

/**
 * Set the default value for connect to brainordinate.
 *
 * @param connectToBrainordinate
 *     Default for newly created text annotations.
 */
void AnnotationText::setDefaultConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate)
{
    s_defaultConnectToBrainordinate = connectToBrainordinate;
}
