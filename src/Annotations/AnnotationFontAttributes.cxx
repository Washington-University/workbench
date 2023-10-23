
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __ANNOTATION_FONT_ATTRIBUTES_DECLARE__
#include "AnnotationFontAttributes.h"
#undef __ANNOTATION_FONT_ATTRIBUTES_DECLARE__

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationFontAttributes 
 * \brief Implementation of AnnotationFontAttributesInterface for use by class that implement AnnotationFontAttributesInterface
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param attributeDefaultType
 *    Default type for annotation
 */
AnnotationFontAttributes::AnnotationFontAttributes(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: CaretObjectTracksModification(),
AnnotationFontAttributesInterface(),
SceneableInterface()
{
    m_font                    = AnnotationTextFontNameEnum::VERA;
    m_colorText               = CaretColorEnum::WHITE;
    m_customColorText[0]      = 1.0;
    m_customColorText[1]      = 1.0;
    m_customColorText[2]      = 1.0;
    m_customColorText[3]      = 1.0;
    m_boldEnabled             = false;
    m_italicEnabled           = false;
    m_underlineEnabled        = false;
    m_fontPercentViewportSize = 5.0;

    
    switch (attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_font                    = AnnotationTextFontNameEnum::VERA;
            m_colorText               = CaretColorEnum::WHITE;
            m_customColorText[0]      = 1.0;
            m_customColorText[1]      = 1.0;
            m_customColorText[2]      = 1.0;
            m_customColorText[3]      = 1.0;
            m_boldEnabled             = false;
            m_italicEnabled           = false;
            m_underlineEnabled        = false;
            m_fontPercentViewportSize = 5.0;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            m_font                    = AnnotationText::s_userDefaultFont;
            m_colorText               = AnnotationText::s_userDefaultColorText;
            m_customColorText[0]      = AnnotationText::s_userDefaultCustomColorText[0];
            m_customColorText[1]      = AnnotationText::s_userDefaultCustomColorText[1];
            m_customColorText[2]      = AnnotationText::s_userDefaultCustomColorText[2];
            m_customColorText[3]      = AnnotationText::s_userDefaultCustomColorText[3];
            m_boldEnabled             = AnnotationText::s_userDefaultBoldEnabled;
            m_italicEnabled           = AnnotationText::s_userDefaultItalicEnabled;
            m_underlineEnabled        = AnnotationText::s_userDefaultUnderlineEnabled;
            m_fontPercentViewportSize = AnnotationText::s_userDefaultFontPercentViewportSize;
            break;
    }

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    /*
     * Assists with attributes that may be saved to scene (controlled by annotation property).
     */
    m_sceneAssistant.reset(new SceneClassAssistant());
    m_sceneAssistant->add<AnnotationTextFontNameEnum, AnnotationTextFontNameEnum::Enum>("m_font",
                                                                                             &m_font);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_colorText",
                                                                     &m_colorText);
    m_sceneAssistant->addArray("m_customColorText", m_customColorText, 4, 1.0);
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
 * Destructor.
 */
AnnotationFontAttributes::~AnnotationFontAttributes()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationFontAttributes::AnnotationFontAttributes(const AnnotationFontAttributes& obj)
: CaretObjectTracksModification(obj),
AnnotationFontAttributesInterface(),
SceneableInterface(obj)
{
    this->copyHelperAnnotationFontAttributes(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationFontAttributes&
AnnotationFontAttributes::operator=(const AnnotationFontAttributes& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotationFontAttributes(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationFontAttributes::copyHelperAnnotationFontAttributes(const AnnotationFontAttributes& obj)
{
    m_font = obj.m_font;
    
    m_fontPercentViewportSize = obj.m_fontPercentViewportSize;
    
    m_colorText = obj.m_colorText;
    
    m_customColorText[0] = obj.m_customColorText[0];
    m_customColorText[1] = obj.m_customColorText[1];
    m_customColorText[2] = obj.m_customColorText[2];
    m_customColorText[3] = obj.m_customColorText[3];

    m_boldEnabled = obj.m_boldEnabled;
    
    m_italicEnabled = obj.m_italicEnabled;
    
    m_underlineEnabled = obj.m_underlineEnabled;
    
    m_fontTooSmallWhenLastDrawnFlag = false;
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one hundred.
 */
float
AnnotationFontAttributes::getFontPercentViewportSize() const
{
    return m_fontPercentViewportSize;
}

/**
 * Set the size of the font as a percentage of the viewport height.
 *
 * @param fontPercentViewportHeight
 *    New value for percentage of viewport height.
 *    Range is zero to one hundred.
 */
void
AnnotationFontAttributes::setFontPercentViewportSize(const float fontPercentViewportHeight)
{
    if (fontPercentViewportHeight != m_fontPercentViewportSize) {
        m_fontPercentViewportSize = fontPercentViewportHeight;
        setModified();
    }
}

/**
 * @return The font.
 */
AnnotationTextFontNameEnum::Enum
AnnotationFontAttributes::getFont() const
{
    return m_font;
}

/**
 * Set the font for an annotation.
 
 * @param font
 *    Font for the annotation.
 */
void
AnnotationFontAttributes::setFont(const AnnotationTextFontNameEnum::Enum font)
{
    if (font != m_font) {
        m_font = font;
        setModified();
    }
}

/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
AnnotationFontAttributes::getTextColor() const
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
AnnotationFontAttributes::setTextColor(const CaretColorEnum::Enum color)
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
AnnotationFontAttributes::getTextColorRGBA(float rgbaOut[4]) const
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
AnnotationFontAttributes::getTextColorRGBA(uint8_t rgbaOut[4]) const
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
AnnotationFontAttributes::getCustomTextColor(float rgbaOut[4]) const
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
AnnotationFontAttributes::getCustomTextColor(uint8_t rgbaOut[4]) const
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
AnnotationFontAttributes::setCustomTextColor(const float rgba[4])
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
AnnotationFontAttributes::setCustomTextColor(const uint8_t rgba[4])
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
AnnotationFontAttributes::isBoldStyleEnabled() const
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
AnnotationFontAttributes::setBoldStyleEnabled(const bool enabled)
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
AnnotationFontAttributes::isItalicStyleEnabled() const
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
AnnotationFontAttributes::setItalicStyleEnabled(const bool enabled)
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
AnnotationFontAttributes::isUnderlineStyleEnabled() const
{
    return m_underlineEnabled;
}

/**
 * @return Is the font too small when it is last drawn
 * that may cause an OpenGL error and, as a result,
 * the text is not seen by the user.
 */
bool
AnnotationFontAttributes::isFontTooSmallWhenLastDrawn() const
{
    return m_fontTooSmallWhenLastDrawnFlag;
}

void
AnnotationFontAttributes::setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const
{
    m_fontTooSmallWhenLastDrawnFlag = tooSmallFontFlag;
}

/**
 * Set underline enabled.
 *
 * @param enabled
 *     New status for underline enabled.
 */
void
AnnotationFontAttributes::setUnderlineStyleEnabled(const bool enabled)
{
    if (enabled != m_underlineEnabled) {
        m_underlineEnabled = enabled;
        setModified();
    }
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
AnnotationFontAttributes::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationFontAttributes",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
AnnotationFontAttributes::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

