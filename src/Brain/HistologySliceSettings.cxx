
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

#define __HISTOLOGY_SLICE_SETTINGS_DECLARE__
#include "HistologySliceSettings.h"
#undef __HISTOLOGY_SLICE_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologySliceSettings 
 * \brief Settings for viewing histology slices
 * \ingroup Brain
 */

/**
 * Constructor.
 */
HistologySliceSettings::HistologySliceSettings()
: CaretObject()
{
    m_initializedFlag = false;
    m_histologyCoordinate = HistologyCoordinate();
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_histologyCoordinate",
                          "HistologyCoordinate",
                          &m_histologyCoordinate);
}

/**
 * Destructor.
 */
HistologySliceSettings::~HistologySliceSettings()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologySliceSettings::HistologySliceSettings(const HistologySliceSettings& obj)
: CaretObject(obj),
SceneableInterface()
{
    this->copyHelperHistologySliceSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologySliceSettings&
HistologySliceSettings::operator=(const HistologySliceSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperHistologySliceSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
HistologySliceSettings::copyHelperHistologySliceSettings(const HistologySliceSettings& obj)
{
    m_histologyCoordinate = obj.m_histologyCoordinate;
    
    m_initializedFlag = true;
}

/**
 * Copy the yoked settings
 * @param histologySlicesFile
 *    The histology slices file
 * @param settings
 *    Settings that are copied.
 */
void
HistologySliceSettings::copyYokedSettings(const HistologySlicesFile* histologySlicesFile,
                                          const HistologySliceSettings& settings)
{
    m_histologyCoordinate.copyYokedSettings(histologySlicesFile,
                                            settings.m_histologyCoordinate);
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
HistologySliceSettings::toString() const
{
    return "HistologySliceSettings";
}

/**
 * Update selected slice for the given histology slices file
 * @param histologySlicesFile
 *    The histology slices file
 */
void
HistologySliceSettings::updateForHistologySlicesFile(const HistologySlicesFile* histologySlicesFileIn)
{
    if (histologySlicesFileIn == NULL) {
        reset();
        return;
    }
    HistologySlicesFile* histologySlicesFile(const_cast<HistologySlicesFile*>(histologySlicesFileIn));
    
    bool updateCoordinateFlag(false);
    if ( ! m_histologyCoordinate.isValid()) {
        updateCoordinateFlag = true;
    }
    else if (histologySlicesFile != NULL) {
        if (histologySlicesFile->getFileName() != m_histologyCoordinate.getHistologySlicesFileName()) {
            updateCoordinateFlag = true;
        }
    }
//    else if (histologySlicesFile != m_histologyCoordinate.getHistologySlicesFile()) {
//        updateCoordinateFlag = true;
//    }

    if ( ! updateCoordinateFlag) {
        return;
    }
    
    std::cout << "Updating selected histology coordinate in HistologySliceSettings::updateForHistologySlicesFile" << std::endl;
    m_histologyCoordinate = HistologyCoordinate::newInstanceDefaultSlices(histologySlicesFile);
}

/**
 * @return The histology slice coordinate
 * @param The histology slices file
 */
HistologyCoordinate
HistologySliceSettings::getHistologyCoordinate(const HistologySlicesFile* histologySlicesFile) const
{
    bool updateFlag(false);
    if ( ! m_histologyCoordinate.isValid()) {
        updateFlag = true;
    }
    else if (histologySlicesFile != NULL) {
        if (m_histologyCoordinate.getHistologySlicesFileName() != histologySlicesFile->getFileName()) {
            /*
             * Disabled on 26mar2024
             * If the histology coordinate filename is for a file not in the tab
             * that uses these settings it will cause the updateFlag to go true
             * and yoking will not work.
             */
            /*updateFlag = true;*/
        }
    }
//    else if (m_histologyCoordinate.getHistologySlicesFile() != histologySlicesFile) {
//        updateFlag = true;
//    }
    
    /*
     * If both file and coordinate are invalid, no update needed
     */
    if (histologySlicesFile == NULL) {
        if ( ! m_histologyCoordinate.isValid()) {
            updateFlag = false;
        }
    }
    
    if (updateFlag) {
        m_histologyCoordinate = HistologyCoordinate::newInstanceDefaultSlices(const_cast<HistologySlicesFile*>(histologySlicesFile));
    }

    return m_histologyCoordinate;
}

/**
 * Set the histology coordinate
 * @param histologyCoordinate
 *    New value for coordinate
 */
void
HistologySliceSettings::setHistologyCoordinate(const HistologyCoordinate& histologyCoordinate)
{
    m_histologyCoordinate = histologyCoordinate;
}

/**
 * Set the slice indices so that they are at the center.
 */
void
HistologySliceSettings::selectSlicesAtCenter(const HistologySlicesFile* histologySlicesFile)
{
    m_histologyCoordinate = HistologyCoordinate::newInstanceDefaultSlices(const_cast<HistologySlicesFile*>(histologySlicesFile));
}

/**
 * Reset to defaults
 */
void
HistologySliceSettings::reset()
{
    m_histologyCoordinate = HistologyCoordinate();
    m_initializedFlag = false;
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
HistologySliceSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologySliceSettings",
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
HistologySliceSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
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
 
    m_initializedFlag = false;
}

