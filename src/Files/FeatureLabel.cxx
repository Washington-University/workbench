
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_LABEL_DECLARE__
#include "FeatureLabel.h"
#undef __FEATURE_LABEL_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::FeatureLabel 
 * \brief Feature index and label
 * \ingroup Files
 */

/**
 * Constructor.
 * @param value
 *    The label value
 * @param text
 *    Text of the label
 */
FeatureLabel::FeatureLabel(const int32_t value,
                                                         const AString& text)
: FeatureBase(FeatureBase::BaseType::FEATURE_LABEL),
m_value(value)
{
    setText(text);
    
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    setCheckable(true);
    setCheckState(Qt::Checked);
}

/**
 * Destructor.
 */
FeatureLabel::~FeatureLabel()
{
}

/**
 * @return Value of the label
 */
int32_t
FeatureLabel::getValue() const
{
    return m_value;
}

/**
 * @return String showing content.
 */
AString
FeatureLabel::toString() const
{
    return ("Value=" + AString::number(m_value)
            + ", Text=" + text());
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
FeatureLabel::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                         const AString& instanceName)
{
    SceneClass* sceneClass(new SceneClass(instanceName,
                                          "FeatureLabel",
                                          1));
    const bool checkedFlag(checkState() == Qt::Checked);
    sceneClass->addBoolean("checkedFlag",
                           checkedFlag);
    return sceneClass;
}



/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
FeatureLabel::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                              const SceneClass* sceneClass)
{
    const bool defaultValue(true);
    const bool checkedFlag(sceneClass->getBooleanValue("checkedFlag",
                                                       defaultValue));
    if (checkedFlag) {
        setCheckState(Qt::Checked);
    }
    else {
        setCheckState(Qt::Unchecked);
    }
}

