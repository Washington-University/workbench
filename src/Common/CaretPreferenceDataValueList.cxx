
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

#define __CARET_PREFERENCE_DATA_VALUE_LIST_DECLARE__
#include "CaretPreferenceDataValueList.h"
#undef __CARET_PREFERENCE_DATA_VALUE_LIST_DECLARE__

#include <QSettings>

#include "CaretAssert.h"
using namespace caret;



/**
 * \class caret::CaretPreferenceDataValueList
 * \brief Maintains an array caret preference data value
 * \ingroup Common
 *
 * Makes it easy to add a preference that contains multiple values of the same data type
 */

/**
 * Constructor.
 *
 * @param preferenceSettings
 *     QSettings from caret preferences
 * @param preferenceName
 *     Name of the preference
 * @param dataType
 *     Data type of the preference
 * @param maximumNumberOfElements
 *     Maximum number of elements
 */
CaretPreferenceDataValueList::CaretPreferenceDataValueList(QSettings* preferenceSettings,
                                                             const QString& preferenceName,
                                                             const DataType dataType,
                                                             const int32_t maximumNumberOfElements)
: CaretObject(),
m_preferenceSettings(preferenceSettings),
m_preferenceName(preferenceName),
m_dataType(dataType),
m_maximumNumberOfElements(maximumNumberOfElements)
{
    CaretAssert(m_preferenceSettings);
    CaretAssert( ! m_preferenceName.isEmpty());
    CaretAssert(m_maximumNumberOfElements > 0);
    
    readFromPreferences();
}

/**
 * Destructor.
 */
CaretPreferenceDataValueList::~CaretPreferenceDataValueList()
{
}

/**
 * @return Name of preference
 */
QString
CaretPreferenceDataValueList::getName() const
{
    return m_preferenceName;
}

/**
 * @return The data type
 */
CaretPreferenceDataValueList::DataType
CaretPreferenceDataValueList::getDataType() const
{
    return m_dataType;
}

/**
 * @return Size of the list (number of elements)
 */
int32_t
CaretPreferenceDataValueList::getSize() const
{
    return m_dataList.size();
}

/**
 * Remove the element at the given index
 * @param index
 *    Index of the element
 * @return True if the element was removed, else false.
 */
bool
CaretPreferenceDataValueList::removeAt(const int32_t index)
{
    CaretAssertVectorIndex(m_dataList, index);
    if ((index >= 0)
        && (index < m_dataList.size())) {
        m_dataList.remove(index);
        writeToPreferences();
        return true;
    }
    return false;
}

/**
 * @return The data value.
 * @param index
 *     Index of element
 *
 * @seealso getPreferenceValue()
 */
QVariant
CaretPreferenceDataValueList::getValue(const int32_t index) const
{
    CaretAssertVectorIndex(m_dataList, index);
    if ((index >= 0)
        && (index < m_dataList.size())) {
        return m_dataList[index];
    }

    return QVariant();
}

/**
 * Push the value to the back of the list
 * @param value
 *    Value put into front of list
 */
void
CaretPreferenceDataValueList::pushBack(const QVariant& value)
{
    m_dataList.push_back(value);
    writeToPreferences();
}

/**
 * Push the value to the front of the list
 * @param value
 *    Value put into end of list
 */
void
CaretPreferenceDataValueList::pushFront(const QVariant& value)
{
    m_dataList.push_front(value);
    writeToPreferences();
}

/**
 * Set the value.  This does invalidate the scene value.
 * @param index
 *    Index of element
 * @param value
 *    New value
 */
void
CaretPreferenceDataValueList::setValue(const int32_t index,
                                       const QVariant& value)
{
    CaretAssertVectorIndex(m_dataList, index);
    if ((index >= 0)
        && (index < m_dataList.size())) {
        m_dataList[index] = value;
        writeToPreferences();
    }
}

/**
 * Read the data values from the settings
 * @param dataListOut
 *   Output containing the list
 */
void
CaretPreferenceDataValueList::readFromPreferences()
{
    m_dataList.clear();
    
    const int numElements(m_preferenceSettings->beginReadArray(m_preferenceName));
    for (int32_t i = 0; i < numElements; i++) {
        m_preferenceSettings->setArrayIndex(i);
        const QVariant element(m_preferenceSettings->value(QString::number(i)));
        m_dataList.push_back(element);
    }
    m_preferenceSettings->endArray();
    
    resizeListToMaximumNumberOfElements();
}

/**
 * Write the values to preferences and is called anytime the list is changed.
 * If the number of elements in the list exceeds the maximum number of elements
 * elements are removed from the back (end) of the list.
 */
void
CaretPreferenceDataValueList::writeToPreferences()
{
    resizeListToMaximumNumberOfElements();
    
    const int32_t numElements(m_dataList.size());
    m_preferenceSettings->beginWriteArray(m_preferenceName);
    for (int32_t i = 0; i < numElements; i++) {
        m_preferenceSettings->setArrayIndex(i);
        CaretAssertVectorIndex(m_dataList, i);
        m_preferenceSettings->setValue(QString::number(i),
                                       m_dataList[i]);
    }
    m_preferenceSettings->endArray();
    m_preferenceSettings->sync();
}

/**
 * Limit the list to the maximum number of elements
 */
void
CaretPreferenceDataValueList::resizeListToMaximumNumberOfElements()
{
    const int32_t numElements(m_dataList.size());
    if (numElements > m_maximumNumberOfElements) {
        m_dataList.resize(m_maximumNumberOfElements);
    }
}

