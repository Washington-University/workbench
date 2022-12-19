#ifndef __CARET_PREFERENCE_DATA_VALUE_LIST_H__
#define __CARET_PREFERENCE_DATA_VALUE_LIST_H__

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

#include <QList>
#include <QVariant>

#include "CaretObject.h"

class QSettings;

namespace caret {
    
    class CaretPreferences;
    
    class CaretPreferenceDataValueList : public CaretObject {
        
    public:
        enum class DataType {
            BOOLEAN,
            FLOAT,
            INTEGER,
            STRING,
            QVARIANT
        };
        
        CaretPreferenceDataValueList(QSettings* preferenceSettings,
                                      const QString& preferenceName,
                                      const DataType dataType,
                                      const int32_t maximumNumberOfElements);
        
        virtual ~CaretPreferenceDataValueList();
        
        CaretPreferenceDataValueList(const CaretPreferenceDataValueList&) = delete;
        
        CaretPreferenceDataValueList& operator=(const CaretPreferenceDataValueList&) = delete;
        
        QString getName() const;
        
        DataType getDataType() const;
        
        int32_t getSize() const;
        
        bool removeAt(const int32_t index);
        
        QVariant getValue(const int32_t index) const;
        
        void pushBack(const QVariant& value);
        
        void pushFront(const QVariant& value);
        
        void setValue(const int32_t index,
                      const QVariant& value);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void readFromPreferences();
        
        void writeToPreferences();
        
        void resizeListToMaximumNumberOfElements();
        
        QSettings* m_preferenceSettings;
        
        const QString m_preferenceName;
        
        const DataType m_dataType;
        
        const int32_t m_maximumNumberOfElements;
        
        QList<QVariant> m_dataList;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __CARET_PREFERENCE_DATA_VALUE_LIST_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_PREFERENCE_DATA_VALUE_LIST_DECLARE__
    
} // namespace
#endif  //__CARET_PREFERENCE_DATA_VALUE_LIST_H__

