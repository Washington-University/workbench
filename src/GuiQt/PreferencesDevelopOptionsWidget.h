#ifndef __PREFERENCES_DEVELOP_OPTIONS_WIDGET_H__
#define __PREFERENCES_DEVELOP_OPTIONS_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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


#include <map>
#include <memory>

#include <QWidget>

#include "DeveloperFlagsEnum.h"

namespace caret {

    class CaretPreferences;
    class EnumComboBoxTemplate;
    class WuQTrueFalseComboBox;
    
    class PreferencesDevelopOptionsWidget : public QWidget {
        
        Q_OBJECT

    public:
        PreferencesDevelopOptionsWidget(QWidget* parent = 0);
        
        virtual ~PreferencesDevelopOptionsWidget();
        
        PreferencesDevelopOptionsWidget(const PreferencesDevelopOptionsWidget&) = delete;

        PreferencesDevelopOptionsWidget& operator=(const PreferencesDevelopOptionsWidget&) = delete;
        
        void updateContent(CaretPreferences* preferences);

        // ADD_NEW_METHODS_HERE

    private slots:
        void developerFlagSelected(const DeveloperFlagsEnum::Enum flag,
                                   const bool status);
        
        void cropSceneImagesStatusChanged(const bool status);
        
        void graphicsTextureMagnificationFilterEnumComboBoxItemActivated();
        
        void graphicsTextureMinificationFilterEnumComboBoxItemActivated();
        
    private:
        void updateGraphicsAndUserInterface();
        
        /* DO NOT delete */
        CaretPreferences* m_preferences = NULL;
        
        std::map<WuQTrueFalseComboBox*, DeveloperFlagsEnum::Enum> m_developerFlagsMap;
        
        WuQTrueFalseComboBox* m_cropSceneImagesComboBox;
        
        EnumComboBoxTemplate* m_graphicsTextureMagnificationFilterEnumComboBox = NULL;
        
        EnumComboBoxTemplate* m_graphicsTextureMinificationFilterEnumComboBox = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__

} // namespace
#endif  //__PREFERENCES_DEVELOP_OPTIONS_WIDGET_H__
