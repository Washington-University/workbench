
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_MOUSE_EVENT_INFO_DECLARE__
#include "WuQMacroMouseEventInfo.h"
#undef __WU_Q_MACRO_MOUSE_EVENT_INFO_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroMouseEventInfo 
 * \brief Information about and related to a QMouseEvent
 * \ingroup Common
 */

/**
 * Constructor.
 * @param mouseEventType
 *     Type of mouse event
 * @param  mouseButton
 *     Button that caused the event
 * @param  mouseButtonsMask
 *     Mask with buttons down during mouse event
 * @param  keyboardModifiersMask
 *     Mask with any keys down during mouse event
 * @param  widgetWidth
 *     Width of widget where mouse event occurred
 * @param  widgetHeight
 *    Width of widget where mouse event occurred
 */
WuQMacroMouseEventInfo::WuQMacroMouseEventInfo(const WuQMacroMouseEventTypeEnum::Enum mouseEventType,
                                               const uint32_t mouseButton,
                                               const uint32_t mouseButtonsMask,
                                               const uint32_t keyboardModifiersMask,
                                               const int32_t widgetWidth,
                                               const int32_t widgetHeight)
: CaretObject(),
m_mouseEventType(mouseEventType),
m_mouseButton(mouseButton),
m_mouseButtonsMask(mouseButtonsMask),
m_keyboardModifiersMask(keyboardModifiersMask),
m_widgetWidth(widgetWidth),
m_widgetHeight(widgetHeight)
{
}

/**
 * Destructor.
 */
WuQMacroMouseEventInfo::~WuQMacroMouseEventInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroMouseEventInfo::WuQMacroMouseEventInfo(const WuQMacroMouseEventInfo& obj)
: CaretObject(obj)
{
    this->copyHelperWuQMacroMouseEventInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
WuQMacroMouseEventInfo&
WuQMacroMouseEventInfo::operator=(const WuQMacroMouseEventInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperWuQMacroMouseEventInfo(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
WuQMacroMouseEventInfo::copyHelperWuQMacroMouseEventInfo(const WuQMacroMouseEventInfo& obj)
{
    m_mouseEventType = obj.m_mouseEventType;
    m_localXY = obj.m_localXY;
    m_mouseButton = obj.m_mouseButton;
    m_mouseButtonsMask = obj.m_mouseButtonsMask;
    m_keyboardModifiersMask = obj.m_keyboardModifiersMask;
    m_widgetWidth = obj.m_widgetWidth;
    m_widgetHeight = obj.m_widgetHeight;
}

/**
 * Widget size may change so scale the x and y position to fit new widget size
 *
 * @param newWidth
 *    Current width of widget
 * @param newHeight
 *    Current height of widget
 * @param localX
 *    Local X position
 * @param localY
 *    Local Y position
 * @param xLocalOut
 *    Output with adjusted local-X position
 * @param yLocalOut
 *    Output with adjusted local-Y position
 */
void
WuQMacroMouseEventInfo::getLocalPositionRescaledToWidgetSize(const int32_t newWidth,
                                                             const int32_t newHeight,
                                                             const int32_t localX,
                                                             const int32_t localY,
                                                             int32_t& xLocalOut,
                                                             int32_t& yLocalOut) const
{
    xLocalOut = localX;
    yLocalOut = localY;
    
    if ((newWidth != m_widgetWidth)
        || (newHeight != m_widgetHeight)) {
        const float normalizedWidth = (static_cast<float>(localX)
                                       / static_cast<float>(m_widgetWidth));
        const float normalizedHeight = (static_cast<float>(localY)
                                        / static_cast<float>(m_widgetHeight));
        
        xLocalOut = (newWidth * normalizedWidth);
        yLocalOut = (newHeight * normalizedHeight);
    }
}

/**
 * @return Type of the mouse event
 */
WuQMacroMouseEventTypeEnum::Enum
WuQMacroMouseEventInfo::getMouseEventType() const
{
    return m_mouseEventType;
}

/**
 * Append local mouse X/Y coordinates
 *
 * @param localX
 *    The local X-coordinate
 * @param localY
 *    The local Y-coordinate
 */
void
WuQMacroMouseEventInfo::addLocalXY(const int32_t localX,
                                   const int32_t localY)
{
    m_localXY.push_back(localX);
    m_localXY.push_back(localY);
}

int32_t
WuQMacroMouseEventInfo::getNumberOfLocalXY() const
{
    const int32_t num = (m_localXY.size() / 2);
    return num;
}

/**
 * @return X-coordinate of mouse relative to widget at the given index
 *
 * @param index
 *     Index of the coordinate
 */
int32_t
WuQMacroMouseEventInfo::getLocalX(const int32_t index) const
{
    const int32_t offset = (index * 2);
    CaretAssertVectorIndex(m_localXY, offset);
    return m_localXY[offset];
}

/** 
 * @return Y-coordinate of mouse relative to widget at the given index
 *
 * @param index
 *     Index of the coordinate
 */
int32_t
WuQMacroMouseEventInfo::getLocalY(const int32_t index) const
{
    const int32_t offset = (index * 2) + 1;
    CaretAssertVectorIndex(m_localXY, offset);
    return m_localXY[offset];
}

/** 
 * @return Button that caused the event 
 */
uint32_t
WuQMacroMouseEventInfo::getMouseButton() const
{
    return m_mouseButton;
}

/** 
 * @return Mask with buttons down during mouse event
 */
uint32_t WuQMacroMouseEventInfo::getMouseButtonsMask() const
{
    return m_mouseButtonsMask;
}

/** 
 * @@return Mask with any keys down during mouse event 
 */
uint32_t
WuQMacroMouseEventInfo::getKeyboardModifiersMask() const
{
    return m_keyboardModifiersMask;
}

/** 
 * @return Width of widget where mouse event occurred 
 */
int32_t
WuQMacroMouseEventInfo::getWidgetWidth() const
{
    return m_widgetWidth;
}

/** 
 * @return Width of widget where mouse event occurred 
 */
int32_t
WuQMacroMouseEventInfo::getWidgetHeight() const
{
    return m_widgetHeight;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
WuQMacroMouseEventInfo::toString() const
{
    return "WuQMacroMouseEventInfo";
}

