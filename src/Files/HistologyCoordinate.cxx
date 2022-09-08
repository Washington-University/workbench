
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

#define __HISTOLOGY_COORDINATE_DECLARE__
#include "HistologyCoordinate.h"
#undef __HISTOLOGY_COORDINATE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologyCoordinate 
 * \brief Information about histology coordinate
 * \ingroup Files
 */

/**
 * Constructor.
 */
HistologyCoordinate::HistologyCoordinate()
: CaretObject()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_stereotaxicXYZ",
                               m_stereotaxicXYZ,
                               3,
                               0.0);
    m_sceneAssistant->add("m_stereotaxicXYZValid",
                          &m_stereotaxicXYZValid);
    m_sceneAssistant->addArray("m_planeXY",
                               m_planeXY,
                               3,
                               0.0);
    m_sceneAssistant->add("m_planeXYValid",
                          &m_planeXYValid);
    m_sceneAssistant->add("m_histologySlicesFileName",
                          &m_histologySlicesFileName);
    m_sceneAssistant->add("m_histologySlicesFileNameValid",
                          &m_histologySlicesFileNameValid);
    m_sceneAssistant->add("m_histologyMediaFileName",
                          &m_histologyMediaFileName);
    m_sceneAssistant->add("m_histologyMediaFileNameValid",
                          &m_histologyMediaFileNameValid);
    m_sceneAssistant->add("m_sliceIndex",
                          &m_sliceIndex);
    m_sceneAssistant->add("m_sliceIndexValid",
                          &m_sliceIndexValid);
    m_sceneAssistant->add("m_sliceNumber",
                          &m_sliceNumber);
    m_sceneAssistant->add("m_sliceNumberValid",
                          &m_sliceNumberValid);
}


/**
 * Destructor.
 */
HistologyCoordinate::~HistologyCoordinate()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologyCoordinate::HistologyCoordinate(const HistologyCoordinate& obj)
: CaretObject(obj)
{
    this->copyHelperHistologyCoordinate(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologyCoordinate&
HistologyCoordinate::operator=(const HistologyCoordinate& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperHistologyCoordinate(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologyCoordinate::copyHelperHistologyCoordinate(const HistologyCoordinate& obj)
{
    m_stereotaxicXYZ               = obj.m_stereotaxicXYZ;
    m_histologySlicesFileName      = obj.m_histologySlicesFileName;
    m_histologyMediaFileName       = obj.m_histologyMediaFileName;
    m_sliceIndex                   = obj.m_sliceIndex;
    m_planeXY                      = obj.m_planeXY;
    m_stereotaxicXYZValid          = obj.m_stereotaxicXYZValid;
    m_planeXYValid                 = obj.m_planeXYValid;
    m_histologySlicesFileNameValid = obj.m_histologySlicesFileNameValid;
    m_histologyMediaFileNameValid  = obj.m_histologyMediaFileNameValid;
    m_sliceIndexValid              = obj.m_sliceIndexValid;
    m_sliceNumber                  = obj.m_sliceNumber;
    m_sliceNumberValid             = obj.m_sliceNumberValid;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologyCoordinate::toString() const
{
    return "HistologyCoordinate";
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
HistologyCoordinate::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologyCoordinate",
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
HistologyCoordinate::restoreFromScene(const SceneAttributes* sceneAttributes,
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


/**
 * @return stereotaxic coordinate
 */
Vector3D
HistologyCoordinate::getStereotaxicXYZ() const
{
    return m_stereotaxicXYZ;
}

/**
 * Set the stereotaxic coordinate and validity of it
 * @param xyz
 *    New stereotaxic coordinate
 */
void
HistologyCoordinate::setStereotaxicXYZ(const Vector3D& xyz)
{
    m_stereotaxicXYZ = xyz;
    m_stereotaxicXYZValid = true;
}

/**
 * @return name of histology slices file
 */
AString
HistologyCoordinate::getHistologySlicesFileName() const
{
    return m_histologySlicesFileName;
}

/**
 * Set name of histology slices file
 *
 * @param histologySlicesFileName
 *    New value for name of histology slices file
 */
void
HistologyCoordinate::setHistologySlicesFileName(const AString& histologySlicesFileName)
{
    m_histologySlicesFileName = histologySlicesFileName;
    m_histologySlicesFileNameValid = true;
}

/**
 * @return name of histology media file
 */
AString
HistologyCoordinate::getHistologyMediaFileName() const
{
    return m_histologyMediaFileName;
}

/**
 * Set name of histology media file
 *
 * @param histologyMediaFileName
 *    New value for name of histology media file
 */
void
HistologyCoordinate::setHistologyMediaFileName(const AString& histologyMediaFileName)
{
    m_histologyMediaFileName = histologyMediaFileName;
    m_histologyMediaFileNameValid = true;
}

/**
 * @return index of slice
 */
int64_t
HistologyCoordinate::getSliceIndex() const
{
    return m_sliceIndex;
}

/**
 * Set index of slice
 *
 * @param sliceIndex
 *    New value for index of slice
 */
void
HistologyCoordinate::setSliceIndex(const int64_t sliceIndex)
{
    m_sliceIndex = sliceIndex;
    m_sliceIndexValid = true;
}

/**
 * @return plane XY coordinate, Z is slice index
 */
Vector3D
HistologyCoordinate::getPlaneXY() const
{
    return m_planeXY;
}

/**
 * Set the plane coordinate and validity of it
 * @param xyz
 *    New plane coordinate
 */
void
HistologyCoordinate::setPlaneXYZ(const Vector3D& xyz)
{
    m_planeXY = xyz;
    m_planeXYValid = true;
}

/**
 * @return validity of stereotaxic XYZ
 */
bool
HistologyCoordinate::isStereotaxicXYZValid() const
{
    return m_stereotaxicXYZValid;
}

/**
 * @return validity of plane XY
 */
bool
HistologyCoordinate::isPlaneXYValid() const
{
    return m_planeXYValid;
}

/**
 * @return validity of histology slices file name
 */
bool
HistologyCoordinate::isHistologySlicesFileNameValid() const
{
    return m_histologySlicesFileNameValid;
}

/**
 * @return validity of histology media file name
 */
bool
HistologyCoordinate::isHistologyMediaFileNameValid() const
{
    return m_histologyMediaFileNameValid;
}

/**
 * @return validity of slice index
 */
bool
HistologyCoordinate::isSliceIndexValid() const
{
    return m_sliceIndexValid;
}

/**
 * @return number of slice
 */
int64_t
HistologyCoordinate::getSliceNumber() const
{
    return m_sliceNumber;
}

/**
 * Set number of slice
 *
 * @param sliceNumber
 *    New value for number of slice
 */
void
HistologyCoordinate::setSliceNumber(const int64_t sliceNumber)
{
    m_sliceNumber = sliceNumber;
    m_sliceNumberValid = true;
}

/**
 * @return validity of slice number
 */
bool
HistologyCoordinate::isSliceNumberValid() const
{
    return m_sliceNumberValid;
}

