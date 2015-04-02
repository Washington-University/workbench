
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
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationText 
 * \brief Text annotation.
 * \ingroup Annotations
 */

/**
 * Constructor for a text annotation.
 */
AnnotationText::AnnotationText()
: Annotation(AnnotationTypeEnum::TEXT)
{
    m_text = "";
    m_font = AnnotationFontNameEnum::VERA;
    m_fontHeight = 14;
    m_orientation = AnnotationTextOrientationEnum::HORIZONTAL;
    m_boldEnabled = false;
    m_italicEnabled = false;
    m_underlineEnabled = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_text",
                          &m_text);
    m_sceneAssistant->add<AnnotationFontNameEnum>("m_font",
                                              &m_font);
    m_sceneAssistant->add("m_fontHeight",
                          &m_fontHeight);
    m_sceneAssistant->add<AnnotationTextOrientationEnum>("m_orientation",
                                                         &m_orientation);
    m_sceneAssistant->add("m_boldEnabled",
                          &m_boldEnabled);
    m_sceneAssistant->add("m_italicEnabled",
                          &m_italicEnabled);
    m_sceneAssistant->add("m_underlineEnabled",
                          &m_underlineEnabled);
}

/**
 * Destructor.
 */
AnnotationText::~AnnotationText()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationText::AnnotationText(const AnnotationText& obj)
: Annotation(obj)
{
    this->copyHelperAnnotationText(obj);
}

/**
 * @return An encoded name that contains the
 * name of the font, the font height, and the
 * font style used by font rendering to provide
 * a name for cached fonts.
 */
AString
AnnotationText::getFontRenderingEncodedName() const
{
    AString encodedName;
    encodedName.reserve(50);
    
    encodedName.append(AnnotationFontNameEnum::toName(m_font));
    
    encodedName.append("_" + AString::number(m_fontHeight));
    
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
 * @return The font.
 */
AnnotationFontNameEnum::Enum
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
AnnotationText::setFont(const AnnotationFontNameEnum::Enum font)
{
    if (font != m_font) {
        m_font = font;
        setModified();
    }
}

/**
 * @return The font height.
 */
int32_t
AnnotationText::getFontHeight() const
{
    return m_fontHeight;
}

/**
 * Set the font height.
 *
 * @param fontHeight
 *     New font height.
 */
void
AnnotationText::setFontHeight(const int32_t fontHeight)
{
    if (fontHeight != m_fontHeight) {
        m_fontHeight = fontHeight;
        setModified();
    }
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
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationText::copyHelperAnnotationText(const AnnotationText& obj)
{
    m_text             = obj.m_text;
    m_font             = obj.m_font;
    m_fontHeight       = obj.m_fontHeight;
    m_orientation      = obj.m_orientation;
    m_boldEnabled      = obj.m_boldEnabled;
    m_italicEnabled    = obj.m_italicEnabled;
    m_underlineEnabled = obj.m_underlineEnabled;
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
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

