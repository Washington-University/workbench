
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __RECENT_SCENE_INFO_CONTAINER_DECLARE__
#include "RecentSceneInfoContainer.h"
#undef __RECENT_SCENE_INFO_CONTAINER_DECLARE__

#include <QVariant>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::RecentSceneInfoContainer 
 * \brief Container for info about a recent scene
 * \ingroup Common
 */

/**
 * Constructor for scene file name and scene name
 * @param sceneFileName
 *    Name of scene file
 * @param sceneName
 *    Name of scene
 */
RecentSceneInfoContainer::RecentSceneInfoContainer(const AString& sceneFileName,
                                                   const AString& sceneName)
: CaretObject(),
m_sceneFileName(sceneFileName),
m_sceneName(sceneName)
{
    
}

/**
 * Constructor for a QVariant that is a QStringList with the first
 * element being the scene file name and the second
 * element being the scene name.
 * @param variant
 *    The QVariant
 * @param validFlagOut
 *    True if the variant contained a QStringList with two elements
 */
RecentSceneInfoContainer::RecentSceneInfoContainer(const QVariant& variant,
                                                   bool& validFlagOut)
{
    validFlagOut = false;
    
    const QStringList sl(variant.toStringList());
    if (sl.size() == 2) {
        m_sceneFileName = sl[0];
        m_sceneName     = sl[1];
        validFlagOut = true;
    }
}


/**
 * Destructor.
 */
RecentSceneInfoContainer::~RecentSceneInfoContainer()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
RecentSceneInfoContainer::RecentSceneInfoContainer(const RecentSceneInfoContainer& obj)
: CaretObject(obj)
{
    this->copyHelperRecentSceneInfoContainer(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
RecentSceneInfoContainer&
RecentSceneInfoContainer::operator=(const RecentSceneInfoContainer& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperRecentSceneInfoContainer(obj);
    }
    return *this;    
}

/**
 * Equality operator
 * @param obj
 *    Compare 'this' to 'obj'
 * @return
 *    True if 'this' and 'obj' are the same scene file name and scene name
 */
bool
RecentSceneInfoContainer::operator==(const RecentSceneInfoContainer& obj) const
{
    if (&obj != this) {
        if ((m_sceneFileName == obj.m_sceneFileName)
            && (m_sceneName  == obj.m_sceneName)) {
            return true;
        }
    }
    return false;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
RecentSceneInfoContainer::copyHelperRecentSceneInfoContainer(const RecentSceneInfoContainer& obj)
{
    m_sceneFileName = obj.m_sceneFileName;
    m_sceneName     = obj.m_sceneName;
}

/**
 * @return Name of scene file
 */
AString 
RecentSceneInfoContainer::getSceneFileName() const
{
    return m_sceneFileName;
}

/**
 * @return Name of scene
 */
AString
RecentSceneInfoContainer::getSceneName() const
{
    return m_sceneName;
}

/**
 * @return A QVariant that is a QStringList with the first
 * element being the scene file name and the second
 * element being the scene name
 */
QVariant
RecentSceneInfoContainer::toQVariant() const
{
    QStringList sl;
    sl.push_back(m_sceneFileName);
    sl.push_back(m_sceneName);
    return QVariant(sl);
}

