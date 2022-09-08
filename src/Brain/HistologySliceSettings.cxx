
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
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_sliceCoordinateXYZ", m_sliceCoordinateXYZ, 3, 0.0);
    m_sceneAssistant->add("m_sliceIndex",
                          &m_sliceIndex);
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
: CaretObject(obj)
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
    m_sliceIndex         = obj.m_sliceIndex;
    m_sliceCoordinateXYZ = obj.m_sliceCoordinateXYZ;
    
    m_initializedFlag = true;
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
HistologySliceSettings::updateForHistologySlicesFile(const HistologySlicesFile* histologySlicesFile)
{
    if (histologySlicesFile == NULL) {
        reset();
        return;
    }
    
    if ( ! m_initializedFlag) {
        m_initializedFlag = true;
        selectSlicesAtOrigin();
    }
    
    /*
     * These calls will make the slices valid
     */
    getSelectedSliceIndex(histologySlicesFile);
}

/**
 * Set the slice indices so that they are at the origin.
 */
void
HistologySliceSettings::selectSlicesAtOrigin()
{
    m_sliceCoordinateXYZ[0] = 0.0;
    m_sliceCoordinateXYZ[1] = 0.0;
    m_sliceCoordinateXYZ[2] = 0.0;
}

/**
 * Reset to defaults
 */
void
HistologySliceSettings::reset()
{
    m_initializedFlag = false;
}

/**
 * @return The selected slice index valid for the given histology slices file
 * @param histologySlicesFile
 *    The histology slices file
 */
int64_t
HistologySliceSettings::getSelectedSliceIndex(const HistologySlicesFile* histologySlicesFile) const
{
    if (histologySlicesFile != NULL) {
        if (m_sliceIndex < 0) {
            m_sliceIndex = 0;
        }
        else if (m_sliceIndex >= histologySlicesFile->getNumberOfHistologySlices()) {
            m_sliceIndex = histologySlicesFile->getNumberOfHistologySlices() - 1;
        }
    }
    else {
        m_sliceIndex = 0;
    }
    return m_sliceIndex;
}

/**
 * Set the slice index for the given histology slices files
 * @param histologySlicesFile
 *    The histology slices file
 * @param sliceIndex
 *    New slice index
 */
void
HistologySliceSettings::setSelectedSliceIndex(const HistologySlicesFile* histologySlicesFile,
                                      const int32_t sliceIndex)
{
    m_sliceIndex = sliceIndex;
    (void)getSelectedSliceIndex(histologySlicesFile); /* makes index valid */
}


/**
 * @return The selected slice number valid for the given histology slices file
 * @param histologySlicesFile
 *    The histology slices file
 */
int64_t
HistologySliceSettings::getSelectedSliceNumber(const HistologySlicesFile* histologySlicesFile) const
{
    int32_t sliceNumberOut(0);
    
    if (histologySlicesFile != NULL) {
        sliceNumberOut = histologySlicesFile->getSliceNumberBySliceIndex(getSelectedSliceIndex(histologySlicesFile));
    }
    
    return sliceNumberOut;
    
}


/**
 * Set the selected slice number valid for the given histology slices file
 * @param histologySlicesFile
 *    The histology slices file
 * @param sliceNumber
 *    New slice number
 */
void
HistologySliceSettings::setSelectedSliceNumber(const HistologySlicesFile* histologySlicesFile,
                                       const int32_t sliceNumber)
{
    if (histologySlicesFile != NULL) {
        const int32_t sliceIndex(histologySlicesFile->getSliceIndexFromSliceNumber(sliceNumber));
        if (sliceIndex >= 0) {
            setSelectedSliceIndex(histologySlicesFile,
                                  sliceIndex);
        }
    }
}

/**
 * @return The sterotaxic XYZ
 */
Vector3D
HistologySliceSettings::getSelectedSliceCoordinateXYZ(const HistologySlicesFile* histologySlicesFile) const
{
    const BoundingBox boundingBox(histologySlicesFile->getStereotaxicXyzBoundingBox());
    if (boundingBox.isValid()) {
        /*
         * Forces slice coordinate to be in the bounding box 
         */
        boundingBox.limitCoordinateToBoundingBox(m_sliceCoordinateXYZ);
    }
    else {
        m_sliceCoordinateXYZ[0] = 0.0;
        m_sliceCoordinateXYZ[1] = 0.0;
        m_sliceCoordinateXYZ[2] = 0.0;
    }
    return m_sliceCoordinateXYZ;
}

/**
 * Set the stereotaxic XYZ.
 * @param xyz
 *    New stereotaxic XYZ.
 */
void
HistologySliceSettings::setSelectedSliceCoordinateXYZ(const Vector3D& xyz)
{
    m_sliceCoordinateXYZ = xyz;
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

