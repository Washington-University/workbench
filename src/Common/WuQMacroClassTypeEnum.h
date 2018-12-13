#ifndef __WU_Q_MACRO_CLASS_TYPE_ENUM_H__
#define __WU_Q_MACRO_CLASS_TYPE_ENUM_H__

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

class WuQMacroClassTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Invalid type */
        INVALID,
        /** a QAction */
        ACTION,
        /** a QAction Group */
        ACTION_GROUP,
        /** a QButtonGroup */
        BUTTON_GROUP,
        /** a QCheckBox */
        CHECK_BOX,
        /** a QComboBox */
        COMBO_BOX,
        /** a QDoubleSpinBox */
        DOUBLE_SPIN_BOX,
        /** a QLineEdit */
        LINE_EDIT,
        /** a QListWidget */
        LIST_WIDGET,
        /** a QMenu */
        MENU,
        /** a Mouse Event */
        MOUSE_USER_EVENT,
        /** a QPushButton */
        PUSH_BUTTON,
        /** a QRadioButton */
        RADIO_BUTTON,
        /** a QSlider */
        SLIDER,
        /** a QSpinBox */
        SPIN_BOX,
        /** a QTabBar */
        TAB_BAR,
        /** a QTabWidget */
        TAB_WIDGET,
        /** a QToolButton */
        TOOL_BUTTON
    };

/*
 switch (m_objectType) {
 case WuQMacroClassTypeEnum::ACTION:
 break;
 case WuQMacroClassTypeEnum::CHECK_BOX:
 break;
 case WuQMacroClassTypeEnum::COMBO_BOX:
 break;
 case WuQMacroClassTypeEnum::DOUBLE_SPIN_BOX:
 break;
 case WuQMacroClassTypeEnum::INVALID:
 break;
 case WuQMacroClassTypeEnum::LINE_EDIT:
 break;
 case WuQMacroClassTypeEnum::LIST_WIDGET:
 break;
 case WuQMacroClassTypeEnum::MENU:
 break;
 case WuQMacroClassTypeEnum::MOUSE_USER_EVENT:
 break;
 case WuQMacroClassTypeEnum::PUSH_BUTTON:
 break;
 case WuQMacroClassTypeEnum::RADIO_BUTTON:
 break;
 case WuQMacroClassTypeEnum::SLIDER:
 break;
 case WuQMacroClassTypeEnum::SPIN_BOX:
 break;
 case WuQMacroClassTypeEnum::TAB_BAR:
 break;
 case WuQMacroClassTypeEnum::TAB_WIDGET:
 break;
 case WuQMacroClassTypeEnum::TOOL_BUTTON:
 break;
 }
 */

    ~WuQMacroClassTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static void addWidgetClassNameAlias(const QString& widgetName,
                                        const QString& aliasWidgetName);
    
private:
    WuQMacroClassTypeEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const WuQMacroClassTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<WuQMacroClassTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** Aliases for widget 'first' is QWidget, 'second' is alias */
    static std::vector<std::pair<QString, QString>> s_widgetClassNameAliases;
};

#ifdef __WU_Q_MACRO_CLASS_TYPE_ENUM_DECLARE__
    std::vector<WuQMacroClassTypeEnum> WuQMacroClassTypeEnum::enumData;
    bool WuQMacroClassTypeEnum::initializedFlag = false;
    int32_t WuQMacroClassTypeEnum::integerCodeCounter = 0;
    std::vector<std::pair<QString, QString>> WuQMacroClassTypeEnum::s_widgetClassNameAliases;
#endif // __WU_Q_MACRO_CLASS_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_CLASS_TYPE_ENUM_H__
