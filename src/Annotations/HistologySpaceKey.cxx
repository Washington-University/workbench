
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

#define __HISTOLOGY_SPACE_KEY_DECLARE__
#include "HistologySpaceKey.h"
#undef __HISTOLOGY_SPACE_KEY_DECLARE__

#include <QDir>
#include <QFileInfo>
#include <QStringList>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologySpaceKey
 * \brief Key that relates an item to a histology slices file and slice number
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
HistologySpaceKey::HistologySpaceKey()
: CaretObjectTracksModification()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_histologySlicesFileName",
                          &m_histologySlicesFileName);
    m_sceneAssistant->add("m_sliceNumber", /* Use old name so old scenes work*/
                          &m_sliceName);
}

/**
 * Constructor.
 * @param histologySlicesFileName
 *    Name of histology slices file
 * @param sliceName
 *    Number of slice
 */
HistologySpaceKey::HistologySpaceKey(const AString& histologySlicesFileName,
                                     const AString& sliceName)
: m_histologySlicesFileName(histologySlicesFileName),
m_sliceName(sliceName)
{
    
}

/**
 * Destructor.
 */
HistologySpaceKey::~HistologySpaceKey()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologySpaceKey::HistologySpaceKey(const HistologySpaceKey& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperHistologySpaceKey(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologySpaceKey&
HistologySpaceKey::operator=(const HistologySpaceKey& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperHistologySpaceKey(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologySpaceKey::copyHelperHistologySpaceKey(const HistologySpaceKey& obj)
{
    m_histologySlicesFileName = obj.m_histologySlicesFileName;
    m_sliceName             = obj.m_sliceName;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
HistologySpaceKey::operator==(const HistologySpaceKey& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* perform equality testing HERE and return true if equal ! */
    if ( (m_histologySlicesFileName == obj.m_histologySlicesFileName)
        && (m_sliceName == obj.m_sliceName) ) {
        return true;
    }
    return false;    
}

/**
 * Not equal operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
HistologySpaceKey::operator!=(const HistologySpaceKey& obj) const
{
    return ( ! (*this == obj));
}

/**
 * @return True if this instance is valid (file name not empty and slice number >= 0).
 */
bool
HistologySpaceKey::isValid() const
{
    return ( ( !m_histologySlicesFileName.isEmpty())
            && ( ! m_sliceName.isEmpty()));
}

/**
 * @return Name of histology slices file (full path)
 */
AString
HistologySpaceKey::getHistologySlicesFileName() const
{
    return m_histologySlicesFileName;
}

/**
 * Sets name of histology slices file, should be full path
 */
void
HistologySpaceKey::setHistologySlicesFileName(const AString& histologySlicesFileName)
{
    if (histologySlicesFileName != m_histologySlicesFileName) {
        m_histologySlicesFileName = histologySlicesFileName;
        setModified();
    }
}

/**
 * @return Slice number (NOT index)
 */
AString
HistologySpaceKey::getSliceName() const
{
    return m_sliceName;
}

/**
 * Set the slice number (NOT index)
 * @param sliceName
 *   New value for slice number
 */
void
HistologySpaceKey::setSliceName(const AString& sliceName)
{
    if (sliceName != m_sliceName) {
        m_sliceName = sliceName;
        setModified();
    }
}

/**
 * @return String representation
 */
AString
HistologySpaceKey::toString() const
{
    AString s("Histology Slice File Name: "
              + m_histologySlicesFileName
              + " Slice Number: "
              + m_sliceName);
    
    return s;
}

/**
 * @param filePath
 *    If not empty, path of histology slices file is made relative to this path
 * @return This instance encoded in a string
 */
AString
HistologySpaceKey::toEncodedString(const QDir& directory) const
{
    QString relativeFileName(directory.relativeFilePath(m_histologySlicesFileName));
    
    QStringList sl;
    sl.append(s_encodeRelativeToDirectory);
    sl.append(relativeFileName);
    sl.append(m_sliceName);

    return sl.join(s_encodingSeparator);
}

/**
 * Set this instance from an instance encoded in a string
 * @param encodedString
 *    String containing encoded instance
 * @return
 *    True if successfully decoded or false if error
 */
bool
HistologySpaceKey::setFromEncodedString(const QDir& directory,
                                        const AString& encodedString)
{
#if QT_VERSION >= 0x060000
    const QStringList list(encodedString.split(s_encodingSeparator,
                                               Qt::SkipEmptyParts));
#else
    const QStringList list(encodedString.split(s_encodingSeparator,
                                               QString::SkipEmptyParts));
#endif
    if (list.length() < 3) {
        const AString msg("Encoded HistologySpaceKey of text \""
                          + encodedString
                          + "\"contains fewer than three elements.");
        CaretLogWarning(msg);
    }
    else {
        const AString encodingType(list.at(0));
        if (encodingType == s_encodeRelativeToDirectory) {
            const QFileInfo fileInfo(directory,
                                     list.at(1));
            setHistologySlicesFileName(fileInfo.absoluteFilePath());
            setSliceName(list.at(2));
            return true;
        }
        else {
            const AString msg("Unsupported encoding="
                              + encodingType
                              + " when decoding HistologySpaceKey");
            CaretLogWarning(msg);
        }
    }
    
    return false;
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
HistologySpaceKey::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologySpaceKey",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
HistologySpaceKey::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

