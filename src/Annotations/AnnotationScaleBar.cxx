
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

#include <limits>

#define __ANNOTATION_SCALE_BAR_DECLARE__
#include "AnnotationScaleBar.h"
#undef __ANNOTATION_SCALE_BAR_DECLARE__

#include "AnnotationCoordinate.h"
#include "AnnotationPercentSizeText.h"
#include "CaretAssert.h"
#include "EventAnnotationTextGetBounds.h"
#include "EventManager.h"
#include "MathFunctions.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationScaleBar
 * \brief Annotation used for drawing a color bar.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationScaleBar::AnnotationScaleBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::SCALE_BAR,
                                attributeDefaultType),
AnnotationFontAttributesInterface()
{
    reset();
    
    initializeScaleBarInstance();

    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
        m_sceneAssistant->add("m_length",
                              &m_length);
        m_sceneAssistant->add("m_showLengthTextFlag",
                              &m_showLengthTextFlag);
        m_sceneAssistant->add("m_showLengthUnitsTextFlag",
                              &m_showLengthUnitsTextFlag);
        m_sceneAssistant->add<AnnotationScaleBarUnitsTypeEnum, AnnotationScaleBarUnitsTypeEnum::Enum>("m_lengthUnits",
                                                                                            &m_lengthUnits);

        m_sceneAssistant->add<AnnotationTextFontNameEnum, AnnotationTextFontNameEnum::Enum>("m_fontName",
                                                                                            &m_fontName);
        m_sceneAssistant->add("m_fontPercentViewportHeight",
                              &m_fontPercentViewportHeight);
        m_sceneAssistant->add<AnnotationColorBarPositionModeEnum, AnnotationColorBarPositionModeEnum::Enum>("m_positionMode",
                                                                                                            &m_positionMode);
        m_sceneAssistant->add("m_displayedFlag",
                              &m_displayedFlag);
        
        m_sceneAssistant->add("m_showTickMarksFlag",
                              &m_showTickMarksFlag);
        m_sceneAssistant->add("m_tickMarksSubdivisions",
                              &m_tickMarksSubdivisions);
        m_sceneAssistant->add<CaretColorEnum,CaretColorEnum::Enum>("m_colorText",
                                                                   &m_colorText);
        m_sceneAssistant->addArray("m_customColorText",
                                   m_customColorText, 4, 1.0);
    }
}
/**
 * Destructor.
 */
AnnotationScaleBar::~AnnotationScaleBar()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationScaleBar::AnnotationScaleBar(const AnnotationScaleBar& obj)
: AnnotationTwoDimensionalShape(obj),
AnnotationFontAttributesInterface()
{
    initializeScaleBarInstance();
    
    this->copyHelperAnnotationScaleBar(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationScaleBar&
AnnotationScaleBar::operator=(const AnnotationScaleBar& obj)
{
    if (this != &obj) {
        AnnotationTwoDimensionalShape::operator=(obj);
        this->copyHelperAnnotationScaleBar(obj);
    }
    return *this;    
}

/**
 * Initialize an instance of the scale bar
 */
void
AnnotationScaleBar::initializeScaleBarInstance()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_lengthTextAnnotation.reset(new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL));
    m_lengthTextAnnotation->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
    m_lengthTextAnnotation->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
}

/**
 * Helps with copying an object of this type.
 *
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationScaleBar::copyHelperAnnotationScaleBar(const AnnotationScaleBar& obj)
{
    m_length                   = obj.m_length;
    m_lengthUnits              = obj.m_lengthUnits;
    m_showLengthTextFlag       = obj.m_showLengthTextFlag;
    m_showLengthUnitsTextFlag  = obj.m_showLengthUnitsTextFlag;
    m_showTickMarksFlag        = obj.m_showTickMarksFlag;
    m_tickMarksSubdivisions    = obj.m_tickMarksSubdivisions;
    m_drawingOrthographicWidth = obj.m_drawingOrthographicWidth;
    m_drawingViewportWidth     = obj.m_drawingViewportWidth;
    
    m_positionMode              = obj.m_positionMode;
    m_fontName                  = obj.m_fontName;
    m_fontPercentViewportHeight = obj.m_fontPercentViewportHeight;
    m_positionMode              = obj.m_positionMode;
    m_displayedFlag             = obj.m_displayedFlag;
    m_colorText           = obj.m_colorText;
    m_customColorText[0]  = obj.m_customColorText[0];
    m_customColorText[1]  = obj.m_customColorText[1];
    m_customColorText[2]  = obj.m_customColorText[2];
    m_customColorText[3]  = obj.m_customColorText[3];
    m_fontTooSmallWhenLastDrawnFlag = obj.m_fontTooSmallWhenLastDrawnFlag;
    
    *m_lengthTextAnnotation = *obj.m_lengthTextAnnotation;
}

/**
 * Reset the annotation colorbar.
 *
 * DO NOT make this method virtual is it is called from constructor.
 */
void
AnnotationScaleBar::reset()
{
    resetSizeAttributes();
    setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
    setTabIndex(-1);
    setLineWidthPercentage(0.5);
    
    m_fontName      = AnnotationTextFontNameEnum::getDefaultFontName();
    m_positionMode  = AnnotationColorBarPositionModeEnum::AUTOMATIC;
    m_displayedFlag = false;
    m_showTickMarksFlag = false;
    m_tickMarksSubdivisions = 2;
    
    m_colorText               = CaretColorEnum::WHITE;
    m_customColorText[0]      = 1.0;
    m_customColorText[1]      = 1.0;
    m_customColorText[2]      = 1.0;
    m_customColorText[3]      = 1.0;
    
    setLineColor(CaretColorEnum::WHITE);
    setBackgroundColor(CaretColorEnum::BLACK);
    setLength(25.0);
    
    m_fontTooSmallWhenLastDrawnFlag = false;
}

/**
 * Reset the size attributes of the color bar.
 */
void
AnnotationScaleBar::resetSizeAttributes()
{
    setWidth(25.0);
    setHeight(7.0);
    setRotationAngle(0.0);
    m_fontPercentViewportHeight = 3.33;
}

/**
 * @return Length of scale bar
 */
float
AnnotationScaleBar::getLength() const
{
    return m_length;
}

/**
 * Sets the length of the scale bar
 * @param length
 *    New length
 */
void
AnnotationScaleBar::setLength(const float length)
{
    m_length = length;
}

/**
 * @return Show length text
 */
bool
AnnotationScaleBar::isShowLengthText() const
{
    return m_showLengthTextFlag;
}

/**
 * Sets show the length text
 * @param status
 *    New statius
 */
void
AnnotationScaleBar::setShowLengthText(const bool status)
{
    m_showLengthTextFlag = status;
}

/**
 * @return Show length units text
 */
bool
AnnotationScaleBar::isShowLengthUnitsText() const
{
    return m_showLengthUnitsTextFlag;
}

/**
 * Sets show the length units text
 * @param status
 *    New status
 */
void
AnnotationScaleBar::setShowLengthUnitsText(const bool status)
{
    m_showLengthUnitsTextFlag = status;
}

/**
 * @return Showtick marks
 */
bool
AnnotationScaleBar::isShowTickMarks() const
{
    return m_showTickMarksFlag;
}

/**
 * Sets show the tick marks
 * @param status
 *    New status
 */
void
AnnotationScaleBar::setShowTickMarks(const bool status)
{
    m_showTickMarksFlag = status;
}

/**
 * @return Number of tick marks subdivisions
 */
int32_t
AnnotationScaleBar::getTickMarksSubdivsions() const
{
    return m_tickMarksSubdivisions;
}

/**
 * Set the number of tick marks subdivisions
 * @param subdivisions
 * Number of subdivisions
 */
void
AnnotationScaleBar::setTickMarksSubdivisions(const int32_t subdivisions)
{
    m_tickMarksSubdivisions = subdivisions;
}

/**
 * @return Length units
 */

float
AnnotationScaleBar::getTickMarksHeight() const
{
    const float height(std::max(1.0,
                                (getLineWidthPercentage() / 2.0)));
    return height;
}

/**
 * @return Length units
 */
AnnotationScaleBarUnitsTypeEnum::Enum
AnnotationScaleBar::getLengthUnits() const
{
    return m_lengthUnits;
}

/**
 * Sets the length of the scale bar
 * @param lengthUnits
 *    New length units
 */
void
AnnotationScaleBar::setLengthUnits(const AnnotationScaleBarUnitsTypeEnum::Enum lengthUnits)
{
    m_lengthUnits = lengthUnits;
}

/**
 * Set the orthographic width when the model is drawn
 * @param drawingOrthographicWidgth
 *  Width of the orthographic projection when the model was drawn.
 *
 *   Note: This width is set when the model is drawn in model space.  The scale bar is drawn
 *   in tab space but the length of the scale bar is in model space.  This length is used with the
 *   scale bar's length and the tab space viewport to draw the scale bar in the proper size.
 */
void
AnnotationScaleBar::setDrawingOrthographicWidth(const float drawingOrthographicWidth)
{
    m_drawingOrthographicWidth = drawingOrthographicWidth;
}

/**
 *  @return The orthographic width from when the model was drawn
 */
float
AnnotationScaleBar::getDrawingOrthographicWidth() const
{
    return m_drawingOrthographicWidth;
}

/**
 * Set the viewport width when the model is drawn
 * @param drawingViewportWidth
 *  Width of the viewport when the model was drawn.
 *
 *   Note: This width is set when the model is drawn in model space.  The scale bar is drawn
 *   in tab space but the length of the scale bar is in model space.  This length is used with the
 *   scale bar's length and the tab space viewport to draw the scale bar in the proper size.
 */
void
AnnotationScaleBar::setDrawingViewportWidth(const float drawingViewportWidth)
{
    m_drawingViewportWidth = drawingViewportWidth;
}

/**
 *  @return The viewport width from when the model was drawn
 */
float
AnnotationScaleBar::getDrawingViewportWidth() const
{
    return m_drawingViewportWidth;
}

/**
 * @return The font.
 */
AnnotationTextFontNameEnum::Enum
AnnotationScaleBar::getFont() const
{
    return m_fontName;
}

/**
 * Set the font.
 *
 * @param font
 *     New value for font.
 */
void
AnnotationScaleBar::setFont(const AnnotationTextFontNameEnum::Enum font)
{
    if (font != m_fontName) {
        m_fontName = font;
        setModified();
    }
}

/**
 * @return THe percent viewport height for the font.
 */
float
AnnotationScaleBar::getFontPercentViewportSize() const
{
    return m_fontPercentViewportHeight;
}

/**
 * Set the percent viewport size for the font.
 *
 * @param fontPercentViewportHeight
 *     New value for percent viewport height.
 */
void
AnnotationScaleBar::setFontPercentViewportSize(const float fontPercentViewportHeight)
{
    if (fontPercentViewportHeight != m_fontPercentViewportHeight) {
        m_fontPercentViewportHeight = fontPercentViewportHeight;
        setModified();
    }
}

/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
AnnotationScaleBar::getTextColor() const
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
AnnotationScaleBar::setTextColor(const CaretColorEnum::Enum color)
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
AnnotationScaleBar::getTextColorRGBA(float rgbaOut[4]) const
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
            CaretColorEnum::toRGBAFloat(m_colorText,
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
AnnotationScaleBar::getTextColorRGBA(uint8_t rgbaOut[4]) const
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
AnnotationScaleBar::getCustomTextColor(float rgbaOut[4]) const
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
AnnotationScaleBar::getCustomTextColor(uint8_t rgbaOut[4]) const
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
AnnotationScaleBar::setCustomTextColor(const float rgba[4])
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
AnnotationScaleBar::setCustomTextColor(const uint8_t rgba[4])
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
 * @return
 *    Is bold enabled ?
 */
bool
AnnotationScaleBar::isBoldStyleEnabled() const
{
    return false;
}

/**
 * Set bold enabled.
 *
 * @param enabled
 *     New status for bold enabled.
 */
void
AnnotationScaleBar::setBoldStyleEnabled(const bool /*enabled*/)
{
}

/**
 * @return
 *    Is italic enabled ?
 */
bool
AnnotationScaleBar::isItalicStyleEnabled() const
{
    return false;
}

/**
 * Set italic enabled.
 *
 * @param enabled
 *     New status for italic enabled.
 */
void
AnnotationScaleBar::setItalicStyleEnabled(const bool /*enabled*/)
{
}

/**
 * @return
 *    Is underline enabled ?
 */
bool
AnnotationScaleBar::isUnderlineStyleEnabled() const
{
    return false;
}

/**
 * Set underline enabled.
 *
 * @param enabled
 *     New status for underline enabled.
 */
void
AnnotationScaleBar::setUnderlineStyleEnabled(const bool /*enabled*/)
{
}

/**
 * @return
 *    Is outline enabled ?
 */
bool
AnnotationScaleBar::isOutlineStyleEnabled() const
{
    return false;
}

/**
 * Set outline enabled.
 *
 * @param enabled
 *     New status for outline enabled.
 */
void
AnnotationScaleBar::setOutlineStyleEnabled(const bool /*enabled*/)
{
}

/**
 * @return The position mode for the colorbar annotation.
 */
AnnotationColorBarPositionModeEnum::Enum
AnnotationScaleBar::getPositionMode() const
{
    return m_positionMode;
}

/**
 * Set the position mode for the colorbar.
 *
 * @param positionMode
 *     New position mode for the colorbar.
 */
void
AnnotationScaleBar::setPositionMode(const AnnotationColorBarPositionModeEnum::Enum positionMode)
{
    if (positionMode != m_positionMode) {
        m_positionMode = positionMode;
        setModified();
    }
}

/**
 * @return Display status of colorbar.
 */
bool
AnnotationScaleBar::isDisplayed() const
{
    return m_displayedFlag;
}

/**
 * Set the color bar annotation displayed.
 *
 * Note that this also sets the annotation's selection
 * status to off so that if the user turns off display
 * of the annotation while the annotation is selected
 * the annotation does not show up as selected when
 * the color bar is later displayed by the user.
 *
 * @param displayed
 *     New status for display of colorbar.
 */
void
AnnotationScaleBar::setDisplayed(const bool displayed)
{
    if (displayed != m_displayedFlag) {
        m_displayedFlag = displayed;
        setDeselectedForEditing();
        setModified();
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
AnnotationScaleBar::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
AnnotationScaleBar::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Is the font too small when it is last drawn
 * that may cause an OpenGL error and, as a result,
 * the text is not seen by the user.
 */
bool
AnnotationScaleBar::isFontTooSmallWhenLastDrawn() const
{
    return m_fontTooSmallWhenLastDrawnFlag;
}

void
AnnotationScaleBar::setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const
{
    m_fontTooSmallWhenLastDrawnFlag = tooSmallFontFlag;
}

/**
 * Get drawing information for drawing a scale bar
 * *
 * @param viewportWidth
 *  Width of viewport
 * @param viewportHeight
 *  Height of viewport
 * @param viewportXYZ
 *  Bottom corner of viewport
 * @param drawingInfoOut
 *  Upon exit, contains drawing information
 */
void
AnnotationScaleBar::getScaleBarDrawingInfo(const float viewportWidth,
                                            const float viewportHeight,
                                            const std::array<float, 3>& viewportXYZ,
                                            DrawingInfo& drawingInfoOut) const
{
    drawingInfoOut.reset();
    
    float convertToMM(1.0);
    switch (getLengthUnits()) {
        case AnnotationScaleBarUnitsTypeEnum::CENTIMETERS:
            convertToMM = 10.0;
            break;
        case AnnotationScaleBarUnitsTypeEnum::MICROMETERS:
            convertToMM = 0.10;
            break;
        case AnnotationScaleBarUnitsTypeEnum::MILLIMETERS:
            convertToMM = 1.0;
            break;
    }
    
    const float scaleBarLengthModelCoords(getLength() * convertToMM);
    const float orthographicWidth(getDrawingOrthographicWidth());
    if (orthographicWidth <= 0.0) {
        return;
    }
    
    /*
     * Get the width and height of the text
     */
    EventAnnotationTextGetBounds textBoundsEvent(*getLengthTextAnnotation(),
                                                 viewportWidth,
                                                 viewportHeight);
    EventManager::get()->sendEvent(textBoundsEvent.getPointer());
    const float textDrawingWidth(textBoundsEvent.getTextWidth());
    const float textDrawingHeight(textBoundsEvent.getTextHeight());

    /*
     * Scale bar uses line width for height not annotation height
     */
    const float tabPercentageWidth(scaleBarLengthModelCoords / orthographicWidth);
    const float scaleBarWidthPixels(viewportWidth * tabPercentageWidth);
    const float scaleBarHeightPixels((getLineWidthPercentage() / 100.0) * viewportHeight);
    
    std::vector<float> tickMarkBounds;
    float tickHeight(0.0);
    float tickWidth(0.0);
    if (isShowTickMarks()) {
        const int32_t numSubDiv = getTickMarksSubdivsions();
        if (numSubDiv > 0) {
            const int32_t tickCount(numSubDiv + 1);
            tickWidth = scaleBarHeightPixels;
            tickHeight = scaleBarHeightPixels;

            tickWidth  = MathFunctions::limitRange(scaleBarHeightPixels * 0.10, 2.0, 6.0);
            tickHeight = MathFunctions::limitRange(scaleBarHeightPixels * 0.25, 4.0, 10.0);
            
            const float halfTickWidth(tickWidth / 2.0);
            const float startX(0.0);
            const float endX(scaleBarWidthPixels);
            const float xRange((endX - startX));
            const float deltaX(xRange / numSubDiv);
            
            float tickX(startX);
            const float y(0);
            
            /*
             * Create bounds for each tick mark
             */
            for (int32_t i = 0; i < tickCount; i++) {
                int32_t x(tickX);
                if (i > 0) {
                    if (i == (tickCount - 1)) {
                        x = scaleBarWidthPixels - tickWidth;
                    }
                    else {
                        x -= halfTickWidth;
                    }
                }
                /* bottom left */
                tickMarkBounds.push_back(x);
                tickMarkBounds.push_back(y);
                tickMarkBounds.push_back(0.0);
                
                /* bottom right */
                tickMarkBounds.push_back(x + tickWidth);
                tickMarkBounds.push_back(y);
                tickMarkBounds.push_back(0.0);
                
                /* top right */
                tickMarkBounds.push_back(x + tickWidth);
                tickMarkBounds.push_back(y + tickHeight);
                tickMarkBounds.push_back(0.0);
                
                /* top left */
                tickMarkBounds.push_back(x);
                tickMarkBounds.push_back(y + tickHeight);
                tickMarkBounds.push_back(0.0);

                tickX += deltaX;
            }
        }
    }
    
    const float barAndTicksHeight(scaleBarHeightPixels + tickHeight);
    
    float textWidth(0.0);
    float textHeight(0.0);
    const float spaceBetweenScaleBarAndText(5.0);
    if (isShowLengthText()) {
        textWidth  = textDrawingWidth + spaceBetweenScaleBarAndText;
        textHeight = textDrawingHeight;
    }
    
    const float margin(3.0);
    const float totalHeight = (std::max(barAndTicksHeight,
                                        textHeight)
                               + (margin * 2.0));
    const float totalWidth(scaleBarWidthPixels + textWidth+ (margin * 2.0));
    
    /*
     * Vertically align bar with text
     */
    float barOffsetY(0.0);
    if (barAndTicksHeight < textHeight) {
        barOffsetY = (textHeight - barAndTicksHeight) / 2.0;
    }
    
    /*
     * Overall (background) bounds
     * bottom left
     */
    drawingInfoOut.m_backgroundBounds[0] = viewportXYZ[0];
    drawingInfoOut.m_backgroundBounds[1] = viewportXYZ[1];
    drawingInfoOut.m_backgroundBounds[2] = viewportXYZ[2];
    
    /* bottom right */
    drawingInfoOut.m_backgroundBounds[3] = viewportXYZ[0] + totalWidth;
    drawingInfoOut.m_backgroundBounds[4] = viewportXYZ[1];
    drawingInfoOut.m_backgroundBounds[5] = viewportXYZ[2];
    
    /* top right */
    drawingInfoOut.m_backgroundBounds[6] = viewportXYZ[0] + totalWidth;
    drawingInfoOut.m_backgroundBounds[7] = viewportXYZ[1] + totalHeight;
    drawingInfoOut.m_backgroundBounds[8] = viewportXYZ[2];
    
    /* top left */
    drawingInfoOut.m_backgroundBounds[9] = viewportXYZ[0];
    drawingInfoOut.m_backgroundBounds[10] = viewportXYZ[1] + totalHeight;
    drawingInfoOut.m_backgroundBounds[11] = viewportXYZ[2];
    
    /*
     * Bounds of the bar
     * bottom left
     */
    drawingInfoOut.m_barBounds[0] = viewportXYZ[0] + margin;
    drawingInfoOut.m_barBounds[1] = viewportXYZ[1] + margin + barOffsetY;
    drawingInfoOut.m_barBounds[2] = viewportXYZ[2];
    
    /* bottom right */
    drawingInfoOut.m_barBounds[3] = viewportXYZ[0] + margin + scaleBarWidthPixels;
    drawingInfoOut.m_barBounds[4] = viewportXYZ[1] + margin + barOffsetY;
    drawingInfoOut.m_barBounds[5] = viewportXYZ[2];
    
    /* top right */
    drawingInfoOut.m_barBounds[6] = viewportXYZ[0] + margin + scaleBarWidthPixels;
    drawingInfoOut.m_barBounds[7] = viewportXYZ[1] + margin + scaleBarHeightPixels + barOffsetY;
    drawingInfoOut.m_barBounds[8] = viewportXYZ[2];
    
    /* top left */
    drawingInfoOut.m_barBounds[9] = viewportXYZ[0] + margin;
    drawingInfoOut.m_barBounds[10] = viewportXYZ[1] + margin + scaleBarHeightPixels + barOffsetY;
    drawingInfoOut.m_barBounds[11] = viewportXYZ[2];
    
    if (isShowLengthText()) {
        /*
         * Text is on right side of the BAR
         *   X -> to the right side of the bar
         *   Y -> centered in BACKGROUND BOUNDS
         *   Z -> all Z's are same
         */
        drawingInfoOut.m_textStartXYZ[0] = drawingInfoOut.m_barBounds[3] + spaceBetweenScaleBarAndText;
        drawingInfoOut.m_textStartXYZ[1] = drawingInfoOut.m_backgroundBounds[4] + (totalHeight / 2.0);
        drawingInfoOut.m_textStartXYZ[2] = drawingInfoOut.m_barBounds[5];
    }
    
    if (isShowTickMarks()) {
        const float topLeftXYZ[3] = {
             drawingInfoOut.m_barBounds[9],
             drawingInfoOut.m_barBounds[10],
             drawingInfoOut.m_barBounds[11]
        };
        
        const float maxBarX(drawingInfoOut.m_barBounds[3]);
        
        /*
         * ticks are relative to top left of the bar
         */
        const int32_t numTickBounds = static_cast<int32_t>(tickMarkBounds.size() / 12);
        for (int32_t i = 0; i < numTickBounds; i++) {
            const int32_t i12(i * 12);
            CaretAssertVectorIndex(tickMarkBounds, i12 + 11);
            std::array<float, 12> tb {
                /* bottom left */
                tickMarkBounds[i12]   + topLeftXYZ[0],
                tickMarkBounds[i12+1] + topLeftXYZ[1],
                tickMarkBounds[i12+2] + topLeftXYZ[2],
                /* bottom right */
                tickMarkBounds[i12+3] + topLeftXYZ[0],
                tickMarkBounds[i12+4] + topLeftXYZ[1],
                tickMarkBounds[i12+5] + topLeftXYZ[2],
                /* top right */
                tickMarkBounds[i12+6] + topLeftXYZ[0],
                tickMarkBounds[i12+7] + topLeftXYZ[1],
                tickMarkBounds[i12+8] + topLeftXYZ[2],
                /* top left */
                tickMarkBounds[i12+9] + topLeftXYZ[0],
                tickMarkBounds[i12+10] + topLeftXYZ[1],
                tickMarkBounds[i12+11] + topLeftXYZ[2]
            };
            
            /*
             * Adjust bounds of last tick so that its right-most X-coord
             * is the same as the scale bar
             */
            if (i == (numTickBounds - 1)) {
                tb[0] = maxBarX - tickWidth;
                tb[3] = maxBarX;
                tb[6] = maxBarX;
                tb[9] = maxBarX - tickWidth;
            }
            
            drawingInfoOut.m_ticksBounds.push_back(tb);
        }
    }
    
    drawingInfoOut.setValid(true);
}

/**
 * @return Pointer to annotation for drawing the length text
 */
const AnnotationPercentSizeText*
AnnotationScaleBar::getLengthTextAnnotation() const
{
    m_lengthTextAnnotation->setFont(getFont());
    
    m_lengthTextAnnotation->setFontPercentViewportSize(getFontPercentViewportSize());
    m_lengthTextAnnotation->setTextColor(CaretColorEnum::CUSTOM);
    float rgba[4];
    getTextColorRGBA(rgba);
    m_lengthTextAnnotation->setCustomTextColor(rgba);
    m_lengthTextAnnotation->setRotationAngle(0.0);
    
    const int32_t numDecimals(1);
    QString lengthText(QString::number(getLength(), 'f', numDecimals));
    if (isShowLengthUnitsText()) {
        lengthText.append(AnnotationScaleBarUnitsTypeEnum::toGuiName(getLengthUnits()));
    }
    m_lengthTextAnnotation->setText(lengthText);
    
    return m_lengthTextAnnotation.get();
}

