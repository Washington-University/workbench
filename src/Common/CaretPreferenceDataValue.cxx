
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

#define __CARET_PREFERENCE_DATA_VALUE_DECLARE__
#include "CaretPreferenceDataValue.h"
#undef __CARET_PREFERENCE_DATA_VALUE_DECLARE__

#include <QSettings>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::CaretPreferenceDataValue 
 * \brief Maintains a single caret preference data valud
 * \ingroup Common
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
 * @param savedInScene
 *     Indicates if item is saved to scene and may override the preference when scene restored.
 * @param defaultValue
 *     Default value for the preference
 */
CaretPreferenceDataValue::CaretPreferenceDataValue(QSettings* preferenceSettings,
                                                   const QString& preferenceName,
                                                   const DataType dataType,
                                                   const SavedInScene savedInScene,
                                                   const QVariant defaultValue)
: CaretObject(),
m_preferenceSettings(preferenceSettings),
m_preferenceName(preferenceName),
m_dataType(dataType),
m_savedInScene(savedInScene)
{
    CaretAssert(m_preferenceSettings);
    CaretAssert( ! m_preferenceName.isEmpty());
    CaretAssert( ! defaultValue.isNull());
    
    m_dataValue = m_preferenceSettings->value(m_preferenceName,
                                              defaultValue);
    m_sceneDataValue = m_dataValue;
}

/**
 * Destructor.
 */
CaretPreferenceDataValue::~CaretPreferenceDataValue()
{
}

/**
 * @return Name of preference
 */
QString
CaretPreferenceDataValue::getName() const
{
    return m_preferenceName;
}

/**
 * @return The data type
 */
CaretPreferenceDataValue::DataType
CaretPreferenceDataValue::getDataType() const
{
    return m_dataType;
}

/**
 * @return The data value.  Some preferences may be overridden by
 * a scene value.  If the scene value is valid, it is returned,
 * otherwise, the preference value is returned.
 *
 * @seealso getPreferenceValue()
 *
 * @param valueType
 *     Type of value returned (active/preference/scene)
 */
QVariant
CaretPreferenceDataValue::getValue(/*const ValueType valueType*/) const
{
    QVariant valueOut;
    if (m_sceneDataValueValid) {
        valueOut = m_sceneDataValue;
    }
    else {
        valueOut = m_dataValue;
    }
    return valueOut;
}

/**
 * @return Always returns the preferences value.
 */
QVariant
CaretPreferenceDataValue::getPreferenceValue() const
{
    return m_dataValue;
}

/**
 * @return Always returns the scene value.
 */
QVariant
CaretPreferenceDataValue::getSceneValue() const
{
    return m_sceneDataValue;
}

/**
 * Set the value.  This does invalidate the scene value.
 *
 * @param value
 *    New value
 */
void
CaretPreferenceDataValue::setValue(const QVariant& value)
{
    /*
     * Setting value invalidates scene value
     */
    m_sceneDataValueValid = false;
    
    if (value != m_dataValue) {
        m_dataValue = value;
        m_preferenceSettings->setValue(m_preferenceName,
                                       m_dataValue);
        m_preferenceSettings->sync();
    }
}

/**
 * Set the scene value.  Also sets the scene value valid.
 *
 * @param value
 *     Value of parameter from the scene.
 */
void
CaretPreferenceDataValue::setSceneValue(const QVariant& value)
{
    m_sceneDataValue      = value;
    m_sceneDataValueValid = true;
    
    switch (m_savedInScene) {
        case SavedInScene::SAVE_NO:
            /* do not allow scene value */
            m_sceneDataValueValid = false;
            break;
        case SavedInScene::SAVE_YES:
            break;
    }
}

/**
 * Set the validity of the scene value.
 *
 * @param validStats
 *     New validity status of the scene value
 */
void
CaretPreferenceDataValue::setSceneValueValid(const bool validStatus)
{
    m_sceneDataValueValid = validStatus;

    switch (m_savedInScene) {
        case SavedInScene::SAVE_NO:
            /* do not allow scene value */
            m_sceneDataValueValid = false;
            break;
        case SavedInScene::SAVE_YES:
            break;
    }
}

/**
 * @return True if preferences is saved to scenes
 */
bool
CaretPreferenceDataValue::isSavedToScenes() const
{
    bool savedFlag(false);
    
    switch (m_savedInScene) {
        case SavedInScene::SAVE_NO:
            break;
        case SavedInScene::SAVE_YES:
            savedFlag = true;
            break;
    }

    return savedFlag;
}

/*
 * @return True if the preference value is currently active.
 * If false, scene value is active.
 */
bool
CaretPreferenceDataValue::isPreferenceValueActive() const
{
    return ( ! m_sceneDataValueValid);
}

/*
 * @return True if the scene value is currently active.
 * If false, preferences value is active.
 */
bool
CaretPreferenceDataValue::isSceneValueActive() const
{
    return m_sceneDataValueValid;
}


