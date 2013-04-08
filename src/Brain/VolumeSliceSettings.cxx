
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __VOLUME_SLICE_SETTINGS_DECLARE__
#include "VolumeSliceSettings.h"
#undef __VOLUME_SLICE_SETTINGS_DECLARE__

#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::VolumeSliceSettings 
 * \brief Settings that control the display of volume slices.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
VolumeSliceSettings::VolumeSliceSettings()
: CaretObject()
{
    m_sliceViewPlane         = VolumeSliceViewPlaneEnum::AXIAL;
    m_sliceViewMode          = VolumeSliceViewModeEnum::ORTHOGONAL;
    m_montageNumberOfColumns = 3;
    m_montageNumberOfRows    = 4;
    m_montageSliceSpacing    = 5;
    
    m_sliceCoordinateAxial = 0.0;
    m_sliceCoordinateCoronal = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
    m_sliceEnabledAxial = true;
    m_sliceEnabledCoronal = true;
    m_sliceEnabledParasagittal = true;
    m_initializedFlag = false;
    
    //m_lastVolumeFile = NULL;

    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<VolumeSliceViewPlaneEnum,VolumeSliceViewPlaneEnum::Enum>("m_sliceViewPlane",
                                                                                   &m_sliceViewPlane);
    m_sceneAssistant->add<VolumeSliceViewModeEnum,VolumeSliceViewModeEnum::Enum>("m_sliceViewMode",
                                                                                   &m_sliceViewMode);
    m_sceneAssistant->add("m_montageNumberOfColumns",
                          &m_montageNumberOfColumns);
    m_sceneAssistant->add("m_montageNumberOfRows",
                          &m_montageNumberOfRows);
    m_sceneAssistant->add("m_montageSliceSpacing",
                          &m_montageSliceSpacing);
    
    m_sceneAssistant->add("m_sliceCoordinateAxial", &m_sliceCoordinateAxial);
    m_sceneAssistant->add("m_sliceCoordinateCoronal", &m_sliceCoordinateCoronal);
    m_sceneAssistant->add("m_sliceCoordinateParasagittal", &m_sliceCoordinateParasagittal);
    m_sceneAssistant->add("m_sliceEnabledAxial", &m_sliceEnabledAxial);
    m_sceneAssistant->add("m_sliceEnabledCoronal", &m_sliceEnabledCoronal);
    m_sceneAssistant->add("m_sliceEnabledParasagittal", &m_sliceEnabledParasagittal);
}

/**
 * Destructor.
 */
VolumeSliceSettings::~VolumeSliceSettings()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VolumeSliceSettings::VolumeSliceSettings(const VolumeSliceSettings& obj)
: CaretObject(obj), SceneableInterface(obj)
{
    this->copyHelperVolumeSliceSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VolumeSliceSettings&
VolumeSliceSettings::operator=(const VolumeSliceSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperVolumeSliceSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VolumeSliceSettings::copyHelperVolumeSliceSettings(const VolumeSliceSettings& obj)
{
    m_sliceViewPlane         = obj.m_sliceViewPlane;
    m_sliceViewMode          = obj.m_sliceViewMode;
    m_montageNumberOfColumns = obj.m_montageNumberOfColumns;
    m_montageNumberOfRows    = obj.m_montageNumberOfRows;
    m_montageSliceSpacing    = obj.m_montageSliceSpacing;

    m_sliceCoordinateParasagittal = obj.m_sliceCoordinateParasagittal;
    m_sliceCoordinateCoronal      = obj.m_sliceCoordinateCoronal;
    m_sliceCoordinateAxial        = obj.m_sliceCoordinateAxial;
    m_sliceEnabledParasagittal = obj.m_sliceEnabledParasagittal;
    m_sliceEnabledCoronal      = obj.m_sliceEnabledCoronal;
    m_sliceEnabledAxial        = obj.m_sliceEnabledAxial;
    m_initializedFlag = true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSliceSettings::toString() const
{
    return "VolumeSliceSettings";
}

/**
 * @return The slice view plane.
 *
 */
VolumeSliceViewPlaneEnum::Enum
VolumeSliceSettings::getSliceViewPlane() const
{
    return m_sliceViewPlane;
}

/**
 * Set the slice view plane.
 * @param windowTabNumber
 *    New value for slice plane.
 */
void
VolumeSliceSettings::setSliceViewPlane(const VolumeSliceViewPlaneEnum::Enum slicePlane)
{
    m_sliceViewPlane = slicePlane;
}

/**
 * @return the slice viewing mode.
 */
VolumeSliceViewModeEnum::Enum
VolumeSliceSettings::getSliceViewMode() const
{
    return m_sliceViewMode;
}

/**
 * Set the slice viewing mode.
 * @param sliceViewMode
 *    New value for view mode
 */
void
VolumeSliceSettings::setSliceViewMode(const VolumeSliceViewModeEnum::Enum sliceViewMode)
{
    m_sliceViewMode = sliceViewMode;
}

/**
 * @return the montage number of columns for the given window tab.
 */
int32_t
VolumeSliceSettings::getMontageNumberOfColumns() const
{
    return m_montageNumberOfColumns;
}


/**
 * Set the montage number of columns in the given window tab.
 * @param montageNumberOfColumns
 *    New value for montage number of columns
 */
void
VolumeSliceSettings::setMontageNumberOfColumns(const int32_t montageNumberOfColumns)
{
    m_montageNumberOfColumns = montageNumberOfColumns;
}

/**
 * @return the montage number of rows for the given window tab.
 */
int32_t
VolumeSliceSettings::getMontageNumberOfRows() const
{
    return m_montageNumberOfRows;
}

/**
 * Set the montage number of rows.
 * @param montageNumberOfRows
 *    New value for montage number of rows
 */
void
VolumeSliceSettings::setMontageNumberOfRows(const int32_t montageNumberOfRows)
{
    m_montageNumberOfRows = montageNumberOfRows;
}

/**
 * @return the montage slice spacing.
 */
int32_t
VolumeSliceSettings::getMontageSliceSpacing() const
{
    return m_montageSliceSpacing;
}

/**
 * Set the montage slice spacing.
 * @param montageSliceSpacing
 *    New value for montage slice spacing
 */
void
VolumeSliceSettings::setMontageSliceSpacing(const int32_t montageSliceSpacing)
{
    m_montageSliceSpacing = montageSliceSpacing;
}

/**
 * Set the selected slices to the origin.
 */
void
VolumeSliceSettings::setSlicesToOrigin()
{
    selectSlicesAtOrigin();
}

/**
 * Reset the slices.
 */
void
VolumeSliceSettings::reset()
{
    m_sliceCoordinateAxial        = 0.0;
    m_sliceCoordinateCoronal      = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
    
    m_sliceEnabledAxial        = true;
    m_sliceEnabledCoronal      = true;
    m_sliceEnabledParasagittal = true;
    
    m_initializedFlag = false;
}

/**
 * Update the slices coordinates so that they are valid for
 * the given VolumeFile.
 * @param volumeFile
 *   File for which slice coordinates are made valid.
 */
void
VolumeSliceSettings::updateForVolumeFile(const VolumeFile* volumeFile)
{
    if (volumeFile == NULL) {
        reset();
        return;
    }
    
    if (m_initializedFlag == false) {
        m_initializedFlag = true;
        selectSlicesAtOrigin();
    }
    
    /*
     * These calls will make the slices valid
     */
    getSliceIndexParasagittal(volumeFile);
    getSliceIndexCoronal(volumeFile);
    getSliceIndexAxial(volumeFile);
}

/**
 * Set the slice indices so that they are at the origin.
 */
void
VolumeSliceSettings::selectSlicesAtOrigin()
{
    m_sliceCoordinateAxial        = 0.0;
    m_sliceCoordinateCoronal      = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
}

/**
 * Set the selected slices to the given coordinate.
 * @param xyz
 *    Coordinate for selected slices.
 */
void
VolumeSliceSettings::selectSlicesAtCoordinate(const float xyz[3])
{
    m_sliceCoordinateParasagittal = xyz[0];
    m_sliceCoordinateCoronal      = xyz[1];
    m_sliceCoordinateAxial        = xyz[2];
}

/**
 * Return the axial slice index.
 * @return
 *   Axial slice index or negative if invalid
 */
int64_t
VolumeSliceSettings::getSliceIndexAxial(const VolumeFile* volumeFile) const
{
    CaretAssert(volumeFile);
    
    std::vector<int64_t> dimensions;
    volumeFile->getDimensions(dimensions);
    
    int64_t axialSliceOut = -1;
    
    if (dimensions[2] >= 0) {
        int64_t paragittalSlice, coronalSlice;
        volumeFile->enclosingVoxel(m_sliceCoordinateParasagittal,
                                   m_sliceCoordinateCoronal,
                                   m_sliceCoordinateAxial,
                                   paragittalSlice,
                                   coronalSlice,
                                   axialSliceOut);
        if (axialSliceOut < 0) {
            axialSliceOut = 0;
            
            float xyz[3];
            volumeFile->indexToSpace(0, 0, axialSliceOut, xyz);
            m_sliceCoordinateAxial = xyz[2];
        }
        else if (axialSliceOut >= dimensions[2]) {
            axialSliceOut = dimensions[2] - 1;
            
            float xyz[3];
            volumeFile->indexToSpace(0, 0, axialSliceOut, xyz);
            m_sliceCoordinateAxial = xyz[2];
        }
    }
    return axialSliceOut;
}

/**
 * Set the axial slice index.
 * @param
 *    New value for axial slice index.
 */
void
VolumeSliceSettings::setSliceIndexAxial(const VolumeFile* volumeFile,
                                                   const int64_t sliceIndexAxial)
{
    CaretAssert(volumeFile);
    float xyz[3];
    volumeFile->indexToSpace(0, 0, sliceIndexAxial, xyz);
    
    m_sliceCoordinateAxial = xyz[2];
}

/**
 * Return the coronal slice index.
 * @return
 *   Coronal slice index.
 */
int64_t
VolumeSliceSettings::getSliceIndexCoronal(const VolumeFile* volumeFile) const
{
    CaretAssert(volumeFile);
    
    std::vector<int64_t> dimensions;
    volumeFile->getDimensions(dimensions);
    
    int64_t coronalSliceOut = -1;
    
    if (dimensions[1] >= 0) {
        int64_t paragittalSlice, axialSlice;
        volumeFile->enclosingVoxel(m_sliceCoordinateParasagittal,
                                   m_sliceCoordinateCoronal,
                                   m_sliceCoordinateAxial,
                                   paragittalSlice,
                                   coronalSliceOut,
                                   axialSlice);
        if (coronalSliceOut < 0) {
            coronalSliceOut = 0;
            
            float xyz[3];
            volumeFile->indexToSpace(0, coronalSliceOut, 0, xyz);
            m_sliceCoordinateCoronal = xyz[1];
        }
        else if (coronalSliceOut >= dimensions[1]) {
            coronalSliceOut = dimensions[1] - 1;
            
            float xyz[3];
            volumeFile->indexToSpace(0, coronalSliceOut, 0, xyz);
            m_sliceCoordinateCoronal = xyz[1];
        }
    }
    return coronalSliceOut;
}


/**
 * Set the coronal slice index.
 * @param sliceIndexCoronal
 *    New value for coronal slice index.
 */
void
VolumeSliceSettings::setSliceIndexCoronal(const VolumeFile* volumeFile,
                                                     const int64_t sliceIndexCoronal)
{
    CaretAssert(volumeFile);
    float xyz[3];
    volumeFile->indexToSpace(0, sliceIndexCoronal, 0, xyz);
    
    m_sliceCoordinateCoronal = xyz[1];
}

/**
 * Return the parasagittal slice index.
 * @return
 *   Parasagittal slice index.
 */
int64_t
VolumeSliceSettings::getSliceIndexParasagittal(const VolumeFile* volumeFile) const
{
    std::vector<int64_t> dimensions;
    volumeFile->getDimensions(dimensions);
    
    int64_t parasagittalSliceOut = -1;
    
    if (dimensions[0] >= 0) {
        int64_t coronalSlice, axialSlice;
        volumeFile->enclosingVoxel(m_sliceCoordinateParasagittal,
                                   m_sliceCoordinateCoronal,
                                   m_sliceCoordinateAxial,
                                   parasagittalSliceOut,
                                   coronalSlice,
                                   axialSlice);
        if (parasagittalSliceOut < 0) {
            parasagittalSliceOut = 0;
            
            float xyz[3];
            volumeFile->indexToSpace(parasagittalSliceOut, 0, 0, xyz);
            m_sliceCoordinateParasagittal = xyz[0];
        }
        else if (parasagittalSliceOut >= dimensions[0]) {
            parasagittalSliceOut = dimensions[0] - 1;
            
            float xyz[3];
            volumeFile->indexToSpace(parasagittalSliceOut, 0, 0, xyz);
            m_sliceCoordinateParasagittal = xyz[0];
        }
    }
    return parasagittalSliceOut;
}

/**
 * Set the parasagittal slice index.
 * @param sliceIndexParasagittal
 *    New value for parasagittal slice index.
 */
void
VolumeSliceSettings::setSliceIndexParasagittal(const VolumeFile* volumeFile,
                                                          const int64_t sliceIndexParasagittal)
{
    CaretAssert(volumeFile);
    float xyz[3];
    volumeFile->indexToSpace(sliceIndexParasagittal, 0, 0, xyz);
    
    m_sliceCoordinateParasagittal = xyz[0];
}

/**
 * @return Coordinate of axial slice.
 */
float
VolumeSliceSettings::getSliceCoordinateAxial() const
{
    return m_sliceCoordinateAxial;
}

/**
 * Set the coordinate for the axial slice.
 * @param z
 *    Z-coordinate for axial slice.
 */
void
VolumeSliceSettings::setSliceCoordinateAxial(const float z)
{
    m_sliceCoordinateAxial = z;
}

/**
 * @return Coordinate of coronal slice.
 */
float
VolumeSliceSettings::getSliceCoordinateCoronal() const
{
    return m_sliceCoordinateCoronal;
}

/**
 * Set the coordinate for the coronal slice.
 * @param y
 *    Y-coordinate for coronal slice.
 */
void
VolumeSliceSettings::setSliceCoordinateCoronal(const float y)
{
    m_sliceCoordinateCoronal = y;
}

/**
 * @return Coordinate of parasagittal slice.
 */
float
VolumeSliceSettings::getSliceCoordinateParasagittal() const
{
    return m_sliceCoordinateParasagittal;
}

/**
 * Set the coordinate for the parasagittal slice.
 * @param x
 *    X-coordinate for parasagittal slice.
 */
void
VolumeSliceSettings::setSliceCoordinateParasagittal(const float x)
{
    m_sliceCoordinateParasagittal = x;
}

/**
 * Is the parasagittal slice enabled?
 * @return
 *    Enabled status of parasagittal slice.
 */
bool
VolumeSliceSettings::isSliceParasagittalEnabled() const
{
    return m_sliceEnabledParasagittal;
}

/**
 * Set the enabled status of the parasagittal slice.
 * @param sliceEnabledParasagittal
 *    New enabled status.
 */
void
VolumeSliceSettings::setSliceParasagittalEnabled(const bool sliceEnabledParasagittal)
{
    m_sliceEnabledParasagittal = sliceEnabledParasagittal;
}

/**
 * Is the coronal slice enabled?
 * @return
 *    Enabled status of coronal slice.
 */
bool
VolumeSliceSettings::isSliceCoronalEnabled() const
{
    return m_sliceEnabledCoronal;
}

/**
 * Set the enabled status of the coronal slice.
 * @param sliceEnabledCoronal
 *    New enabled status.
 */
void
VolumeSliceSettings::setSliceCoronalEnabled(const bool sliceEnabledCoronal)
{
    m_sliceEnabledCoronal = sliceEnabledCoronal;
}

/**
 * Is the axial slice enabled?
 * @return
 *    Enabled status of axial slice.
 */
bool
VolumeSliceSettings::isSliceAxialEnabled() const
{
    return m_sliceEnabledAxial;
}

/**
 * Set the enabled status of the axial slice.
 * @param sliceEnabledAxial
 *    New enabled status.
 */
void
VolumeSliceSettings::setSliceAxialEnabled(const bool sliceEnabledAxial)
{
    m_sliceEnabledAxial = sliceEnabledAxial;
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
VolumeSliceSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSliceSettings",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
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
VolumeSliceSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);        
}

