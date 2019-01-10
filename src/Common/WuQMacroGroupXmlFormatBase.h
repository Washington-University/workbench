#ifndef __WU_Q_MACRO_GROUP_XML_FORMAT_BASE_H__
#define __WU_Q_MACRO_GROUP_XML_FORMAT_BASE_H__

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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class WuQMacroGroupXmlFormatBase : public CaretObject {

    protected:
        WuQMacroGroupXmlFormatBase();
        
    public:
        virtual ~WuQMacroGroupXmlFormatBase();
        
        WuQMacroGroupXmlFormatBase(const WuQMacroGroupXmlFormatBase&) = delete;

        WuQMacroGroupXmlFormatBase& operator=(const WuQMacroGroupXmlFormatBase&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    protected:
        
        static const QString ATTRIBUTE_NAME;
        static const QString ATTRIBUTE_OBJECT_CLASS;
        static const QString ATTRIBUTE_OBJECT_DATA_TYPE;
        static const QString ATTRIBUTE_OBJECT_DATA_TYPE_TWO;
        static const QString ATTRIBUTE_OBJECT_DESCRIPTIVE_NAME;
        static const QString ATTRIBUTE_OBJECT_VALUE;
        static const QString ATTRIBUTE_OBJECT_VALUE_TWO;
        static const QString ATTRIBUTE_SHORT_CUT_KEY;
        static const QString ATTRIBUTE_UNIQUE_IDENTIFIER;
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ATTRIBUTE_MOUSE_BUTTON;
        static const QString ATTRIBUTE_MOUSE_BUTTONS_MASK;
        static const QString ATTRIBUTE_MOUSE_EVENT_TYPE;
        static const QString ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK;
        static const QString ATTRIBUTE_MOUSE_LOCAL_X;
        static const QString ATTRIBUTE_MOUSE_LOCAL_Y;
        static const QString ATTRIBUTE_MOUSE_SCREEN_X;
        static const QString ATTRIBUTE_MOUSE_SCREEN_Y;
        static const QString ATTRIBUTE_MOUSE_WIDGET_WIDTH;
        static const QString ATTRIBUTE_MOUSE_WIDGET_HEIGHT;
        static const QString ATTRIBUTE_MOUSE_WINDOW_X;
        static const QString ATTRIBUTE_MOUSE_WINDOW_Y;
        
        
        
        static const QString ELEMENT_DESCRIPTION;
        static const QString ELEMENT_MACRO;
        static const QString ELEMENT_MACRO_COMMAND;
        static const QString ELEMENT_MACRO_GROUP;
        static const QString ELEMENT_MOUSE_EVENT_INFO;
        static const QString ELEMENT_TOOL_TIP;
        static const QString VALUE_BOOL_FALSE;
        static const QString VALUE_BOOL_TRUE;
        static const QString VALUE_VERSION_ONE;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_XML_FORMAT_BASE_DECLARE__
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_NAME = "Name";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_CLASS = "ObjectClass";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_DATA_TYPE = "DataType";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_DATA_TYPE_TWO = "DataTypeTwo";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_DESCRIPTIVE_NAME = "DescriptiveName";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_VALUE = "DataValue";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_OBJECT_VALUE_TWO = "DataValueTwo";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_SHORT_CUT_KEY = "ShortCutKey";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_UNIQUE_IDENTIFIER = "UniqueIdentifier";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_VERSION = "Version";
    
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_BUTTON = "MouseButton";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_BUTTONS_MASK = "MouseButtonsMask";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_EVENT_TYPE = "MouseEventType";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK = "KeyboardModifiersMask";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_LOCAL_X = "LocalX";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_LOCAL_Y = "LocalY";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_SCREEN_X = "ScreenX";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_SCREEN_Y = "ScreenY";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_WIDGET_WIDTH = "WidgetWidth";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_WIDGET_HEIGHT = "WidgetHeight";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_WINDOW_X = "WindowX";
    const QString WuQMacroGroupXmlFormatBase::ATTRIBUTE_MOUSE_WINDOW_Y = "WindowY";
    
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_DESCRIPTION      = "Description";
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_MACRO            = "Macro";
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_MACRO_COMMAND    = "MacroCommand";
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_MACRO_GROUP      = "MacroGroup";
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_MOUSE_EVENT_INFO = "MouseEventInfo";
    const QString WuQMacroGroupXmlFormatBase::ELEMENT_TOOL_TIP         = "ToolTip";

    const QString WuQMacroGroupXmlFormatBase::VALUE_BOOL_FALSE   = "false";
    const QString WuQMacroGroupXmlFormatBase::VALUE_BOOL_TRUE    = "true";
    const QString WuQMacroGroupXmlFormatBase::VALUE_VERSION_ONE  = "1";
#endif // __WU_Q_MACRO_GROUP_XML_FORMAT_BASE_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_XML_FORMAT_BASE_H__
