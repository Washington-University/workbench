
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

#include "AnnotationPercentSizeText.h"
#include "AnnotationSpatialModification.h"
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
 * @param attributeDefaultType
 *    Type for attribute defaults
 * @param fontSizeType
 *    Type of font sizing.
 */
AnnotationText::AnnotationText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                               const AnnotationTextFontSizeTypeEnum::Enum fontSizeType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::TEXT,
                                attributeDefaultType),
AnnotationFontAttributesInterface(),
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
AnnotationFontAttributesInterface(),
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
    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_alignmentHorizontal     = AnnotationTextAlignHorizontalEnum::LEFT;
            m_alignmentVertical       = AnnotationTextAlignVerticalEnum::TOP;
            m_font                    = AnnotationTextFontNameEnum::VERA;
            m_fontPointSize           = AnnotationTextFontPointSizeEnum::SIZE14;
            m_orientation             = AnnotationTextOrientationEnum::HORIZONTAL;
            m_colorText               = CaretColorEnum::WHITE;
            m_customColorText[0]      = 1.0;
            m_customColorText[1]      = 1.0;
            m_customColorText[2]      = 1.0;
            m_customColorText[3]      = 1.0;
            m_boldEnabled             = false;
            m_italicEnabled           = false;
            m_underlineEnabled        = false;
            m_connectToBrainordinate  = AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE;
            m_fontPercentViewportSize = 5.0;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            m_alignmentHorizontal     = s_userDefaultAlignmentHorizontal;
            m_alignmentVertical       = s_userDefaultAlignmentVertical;
            m_font                    = s_userDefaultFont;
            m_fontPointSize           = s_userDefaultPointSize;
            m_orientation             = s_userDefaultOrientation;
            m_colorText               = s_userDefaultColorText;
            m_customColorText[0]      = s_userDefaultCustomColorText[0];
            m_customColorText[1]      = s_userDefaultCustomColorText[1];
            m_customColorText[2]      = s_userDefaultCustomColorText[2];
            m_customColorText[3]      = s_userDefaultCustomColorText[3];
            m_boldEnabled             = s_userDefaultBoldEnabled;
            m_italicEnabled           = s_userDefaultItalicEnabled;
            m_underlineEnabled        = s_userDefaultUnderlineEnabled;
            m_connectToBrainordinate  = s_userDefaultConnectToBrainordinate;
            m_fontPercentViewportSize = s_userDefaultFontPercentViewportSize;
            break;
    }
    
    m_text = "";
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_text",
                          &m_text);
    m_sceneAssistant->add<AnnotationTextAlignHorizontalEnum, AnnotationTextAlignHorizontalEnum::Enum>("m_alignmentHorizontal",
                                                             &m_alignmentHorizontal);
    m_sceneAssistant->add<AnnotationTextAlignVerticalEnum, AnnotationTextAlignVerticalEnum::Enum>("m_alignmentVertical",
                                                           &m_alignmentVertical);
    m_sceneAssistant->add<AnnotationTextFontNameEnum, AnnotationTextFontNameEnum::Enum>("m_font",
                                                  &m_font);
    m_sceneAssistant->add<AnnotationTextFontPointSizeEnum, AnnotationTextFontPointSizeEnum::Enum>("m_fontPointSize",
                                                  &m_fontPointSize);
    m_sceneAssistant->add<AnnotationTextOrientationEnum, AnnotationTextOrientationEnum::Enum>("m_orientation",
                                                         &m_orientation);
    m_sceneAssistant->add<AnnotationTextConnectTypeEnum, AnnotationTextConnectTypeEnum::Enum>("m_connectToBrainordinate",
                                                         &m_connectToBrainordinate);
    m_sceneAssistant->add<CaretColorEnum,CaretColorEnum::Enum>("m_colorText",
                                                               &m_colorText);
    m_sceneAssistant->addArray("m_customColorText",
                               m_customColorText, 4, 1.0);
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

//  Underline and outline are not drawn by font rendering
//    if (m_underlineEnabled) {
//        encodedName.append("_U");
//    }
//    
//    if (m_outlineEnabled) {
//        encodedName.append("_O");
//    }
    
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
        textAnnotationResetName();
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
            const float pixelSize = drawingViewportHeight * (m_fontPercentViewportSize / 100.0);
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
 * Get the icon color for this item.  Icon is filled with background
 * color, outline color is drawn around edges, and text color is small
 * square in center.  For any colors that do not apply, use an alpha
 * value (last element) of zero.
 *
 * @param backgroundRgbaOut
 *     Red, green, blue, alpha components for background ranging [0, 1].
 * @param outlineRgbaOut
 *     Red, green, blue, alpha components for outline ranging [0, 1].
 * @param textRgbaOut
 *     Red, green, blue, alpha components for text ranging [0, 1].
 */
void
AnnotationText::getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                  float outlineRgbaOut[4],
                                  float textRgbaOut[4]) const
{
    /*
     * Sets the background and outline colors
     */
    Annotation::getItemIconColorsRGBA(backgroundRgbaOut,
                                      outlineRgbaOut,
                                      textRgbaOut);
    
    getTextColorRGBA(textRgbaOut);
}


/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
AnnotationText::getTextColor() const
{
    return m_colorText;
}

/**
 * Set the foreground color.
 *
 * @param color
 *     New value for foreground color.
 */
void
AnnotationText::setTextColor(const CaretColorEnum::Enum color)
{
    if (m_colorText != color) {
        m_colorText = color;
        setModified();
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
AnnotationText::getTextColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorText) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomTextColor(rgbaOut);
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBFloat(m_colorText,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
AnnotationText::getTextColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getTextColorRGBA(rgbaFloat);
    
    rgbaOut[0] = static_cast<uint8_t>(rgbaFloat[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(rgbaFloat[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(rgbaFloat[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(rgbaFloat[3] * 255.0);
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationText::getCustomTextColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorText[0];
    rgbaOut[1] = m_customColorText[1];
    rgbaOut[2] = m_customColorText[2];
    rgbaOut[3] = m_customColorText[3];
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationText::getCustomTextColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorText[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorText[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorText[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorText[3] * 255.0);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationText::setCustomTextColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorText[i]) {
            m_customColorText[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationText::setCustomTextColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorText[i]) {
            m_customColorText[i] = component;
            setModified();
        }
    }
}

/**
 * Are font styles (Bold, Italic, Underline) supported?
 */
bool
AnnotationText::isStylesSupported() const
{
    return true;
}

/**
 * @return
 *    Is bold enabled ?
 */
bool
AnnotationText::isBoldStyleEnabled() const
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
AnnotationText::setBoldStyleEnabled(const bool enabled)
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
AnnotationText::isItalicStyleEnabled() const
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
AnnotationText::setItalicStyleEnabled(const bool enabled)
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
AnnotationText::isUnderlineStyleEnabled() const
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
AnnotationText::isLineWidthSupported() const
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
AnnotationText::setUnderlineStyleEnabled(const bool enabled)
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
    m_colorText           = obj.m_colorText;
    m_customColorText[0]  = obj.m_customColorText[0];
    m_customColorText[1]  = obj.m_customColorText[1];
    m_customColorText[2]  = obj.m_customColorText[2];
    m_customColorText[3]  = obj.m_customColorText[3];
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
    
    /*
     * Text size may change when an annotation is moved to a different
     * coordinate space (ie: Tab  Space with Tile Tabs on to Window Space).
     */
    const AnnotationPercentSizeText* otherTextAnn = dynamic_cast<const AnnotationPercentSizeText*>(otherAnnotation);
    AnnotationPercentSizeText* textAnn = dynamic_cast<AnnotationPercentSizeText*>(this);
    if ((textAnn != NULL)
        && (otherTextAnn != NULL)) {
        textAnn->setFontPercentViewportSize(otherTextAnn->getFontPercentViewportSize());
    }
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one-hundred.
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
 *    Range is zero to one-hundred.
 */
void
AnnotationText::setFontPercentViewportSizeProtected(const float fontPercentViewportHeight)
{
    if ((fontPercentViewportHeight < 0.0)
        || (fontPercentViewportHeight > 100.0)) {
        const QString msg("Percent viewport height should range [0.0, 100.0] but value is "
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
 * Apply a spatial modification to an annotation.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationText::applySpatialModification(const AnnotationSpatialModification& spatialModification)
{
    /*
     * Text limits support of resize options
     */
    bool operationSupportedFlag = false;
    
    switch (spatialModification.m_sizingHandleType) {
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
    
    bool validFlag = false;
    if (operationSupportedFlag) {
        validFlag = AnnotationTwoDimensionalShape::applySpatialModification(spatialModification);
    }
    
    return validFlag;
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
void AnnotationText::setUserDefaultHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment)
{
    s_userDefaultAlignmentHorizontal = alignment;
}

/**
 * Set the default value for vertical alignment.
 *
 * @param alignment
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment)
{
    s_userDefaultAlignmentVertical = alignment;
}

/**
 * Set the default value for the font name.
 *
 * @param font
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultFont(const AnnotationTextFontNameEnum::Enum font)
{
    s_userDefaultFont = font;
}

/**
 * Set the default value for the text orientation.
 *
 * @param orientation
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultOrientation(const AnnotationTextOrientationEnum::Enum orientation)
{
    s_userDefaultOrientation = orientation;
}

/**
 * Set the default value for font point size.
 *
 * @param fontPointSize
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize)
{
    s_userDefaultPointSize = fontPointSize;
}

/**
 * Set the default value for font percent viewport height.
 *
 * @param fontPercentViewportHeight
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultFontPercentViewportSize(const float fontPercentViewportHeight)
{
    s_userDefaultFontPercentViewportSize = fontPercentViewportHeight;
}

/**
 * Set the default value for text color
 *
 * @param color
 *     Default for newly created annotations.
 */
void
AnnotationText::setUserDefaultTextColor(const CaretColorEnum::Enum color)
{
    s_userDefaultColorText = color;
}

/**
 * Set the default value for custom text color
 *
 * @param rgba
 *     Default for newly created annotations.
 */
void
AnnotationText::setUserDefaultCustomTextColor(const float rgba[4])
{
    s_userDefaultCustomColorText[0] = rgba[0];
    s_userDefaultCustomColorText[1] = rgba[1];
    s_userDefaultCustomColorText[2] = rgba[2];
    s_userDefaultCustomColorText[3] = rgba[3];
}

/**
 * Set the default value for bold enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultBoldEnabled(const bool enabled)
{
    s_userDefaultBoldEnabled = enabled;
}

/**
 * Set the default value for italic enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultItalicEnabled(const bool enabled)
{
    s_userDefaultItalicEnabled = enabled;
}

/**
 * Set the default value for underline enabled.
 *
 * @param enabled
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultUnderlineEnabled(const bool enabled)
{
    s_userDefaultUnderlineEnabled = enabled;
}

/**
 * Set the default value for connect to brainordinate.
 *
 * @param connectToBrainordinate
 *     Default for newly created text annotations.
 */
void AnnotationText::setUserDefaultConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate)
{
    s_userDefaultConnectToBrainordinate = connectToBrainordinate;
}
