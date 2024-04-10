#ifndef __CARET_PREFERENCE_DATA_VALUE_H__
#define __CARET_PREFERENCE_DATA_VALUE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <QVariant>

#include "CaretObject.h"

class QSettings;

namespace caret {

    class CaretPreferences;
    
    class CaretPreferenceDataValue : public CaretObject {
        
    public:
        enum class DataType {
            BOOLEAN,
            FLOAT,
            INTEGER,
            STRING
        };
        
        enum class SavedInScene {
            SAVE_NO,
            SAVE_YES
        };
        
        CaretPreferenceDataValue(QSettings* preferenceSettings,
                                 const QString& preferenceName,
                                 const DataType dataType,
                                 const SavedInScene savedInScene,
                                 const QVariant defaultValue);
        
        virtual ~CaretPreferenceDataValue();
        
        CaretPreferenceDataValue(const CaretPreferenceDataValue&) = delete;

        CaretPreferenceDataValue& operator=(const CaretPreferenceDataValue&) = delete;

        QString getName() const;
        
        DataType getDataType() const;
        
        QVariant getPreferenceValue() const;
        
        QVariant getSceneValue() const;
        
        QVariant getValue(/*const ValueType valueType*/) const;
        
        void setValue(const QVariant& value);

        void setSceneValue(const QVariant& value);
        
        void setSceneValueValid(const bool validStatus);

        bool isSavedToScenes() const;
        
        bool isPreferenceValueActive() const;
        
        bool isSceneValueActive() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        QSettings* m_preferenceSettings;
        
        const QString m_preferenceName;
        
        const DataType m_dataType;
        
        const SavedInScene m_savedInScene;
        
        QVariant m_dataValue;
        
        QVariant m_sceneDataValue;
        
        bool m_sceneDataValueValid = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_PREFERENCE_DATA_VALUE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_PREFERENCE_DATA_VALUE_DECLARE__

} // namespace
#endif  //__CARET_PREFERENCE_DATA_VALUE_H__
