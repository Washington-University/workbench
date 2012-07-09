
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __VOLUME_SLICE_COORDINATE_SELECTION_DECLARE__
#include "VolumeSliceCoordinateSelection.h"
#undef __VOLUME_SLICE_COORDINATE_SELECTION_DECLARE__

#include "CaretAssert.h"
#include "SceneClassAssistant.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class VolumeSliceIndicesSelection 
 * \brief Provides a mechanism for volume slice selection.
 *
 * This class provides a mechanism for selection of volume
 * slices indices and is used for display of volume slices
 * in the various viewers.
 */

/**
 * Constructor.
 */
VolumeSliceCoordinateSelection::VolumeSliceCoordinateSelection()
: CaretObject()
{
    reset();
    m_sceneAssistant = new SceneClassAssistant();
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
VolumeSliceCoordinateSelection::~VolumeSliceCoordinateSelection()
{
    delete m_sceneAssistant;
}

/**
 * Reset the slices.
 */
void 
VolumeSliceCoordinateSelection::reset()
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
VolumeSliceCoordinateSelection::updateForVolumeFile(const VolumeFile* volumeFile)
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
VolumeSliceCoordinateSelection::selectSlicesAtOrigin()
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
VolumeSliceCoordinateSelection::selectSlicesAtCoordinate(const float xyz[3])
{
    m_sliceCoordinateParasagittal = xyz[0];
    m_sliceCoordinateCoronal      = xyz[1];
    m_sliceCoordinateAxial        = xyz[2];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSliceCoordinateSelection::toString() const
{
    const AString msg = 
    "VolumeSliceCoordinateSelection=(" 
    + AString::number(m_sliceCoordinateParasagittal)
    + ", "
    + AString::number(m_sliceCoordinateCoronal)
    + ", "
    + AString::number(m_sliceCoordinateAxial)
    + ")";
    
    return msg;
}

/**
 * Return the axial slice index.
 * @return
 *   Axial slice index or negative if invalid
 */
int64_t 
VolumeSliceCoordinateSelection::getSliceIndexAxial(const VolumeFile* volumeFile) const
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
VolumeSliceCoordinateSelection::setSliceIndexAxial(const VolumeFile* volumeFile,
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
VolumeSliceCoordinateSelection::getSliceIndexCoronal(const VolumeFile* volumeFile) const
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
VolumeSliceCoordinateSelection::setSliceIndexCoronal(const VolumeFile* volumeFile,
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
VolumeSliceCoordinateSelection::getSliceIndexParasagittal(const VolumeFile* volumeFile) const
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
VolumeSliceCoordinateSelection::setSliceIndexParasagittal(const VolumeFile* volumeFile,
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
VolumeSliceCoordinateSelection::getSliceCoordinateAxial() const
{
    return m_sliceCoordinateAxial;
}

/**
 * Set the coordinate for the axial slice.
 * @param z
 *    Z-coordinate for axial slice.
 */
void 
VolumeSliceCoordinateSelection::setSliceCoordinateAxial(const float z)
{
    m_sliceCoordinateAxial = z;
}

/**
 * @return Coordinate of coronal slice.
 */
float 
VolumeSliceCoordinateSelection::getSliceCoordinateCoronal() const
{
    return m_sliceCoordinateCoronal;
}

/**
 * Set the coordinate for the coronal slice.
 * @param y
 *    Y-coordinate for coronal slice.
 */
void 
VolumeSliceCoordinateSelection::setSliceCoordinateCoronal(const float y)
{
    m_sliceCoordinateCoronal = y;
}

/**
 * @return Coordinate of parasagittal slice.
 */
float 
VolumeSliceCoordinateSelection::getSliceCoordinateParasagittal() const
{
    return m_sliceCoordinateParasagittal;
}

/**
 * Set the coordinate for the parasagittal slice.
 * @param x
 *    X-coordinate for parasagittal slice.
 */
void 
VolumeSliceCoordinateSelection::setSliceCoordinateParasagittal(const float x)
{
    m_sliceCoordinateParasagittal = x;
}

/**
 * Is the parasagittal slice enabled?
 * @return
 *    Enabled status of parasagittal slice.
 */
bool 
VolumeSliceCoordinateSelection::isSliceParasagittalEnabled() const
{
    return m_sliceEnabledParasagittal;
}

/**
 * Set the enabled status of the parasagittal slice.
 * @param sliceEnabledParasagittal 
 *    New enabled status.
 */ 
void 
VolumeSliceCoordinateSelection::setSliceParasagittalEnabled(const bool sliceEnabledParasagittal)
{
    m_sliceEnabledParasagittal = sliceEnabledParasagittal;
}

/**
 * Is the coronal slice enabled?
 * @return
 *    Enabled status of coronal slice.
 */
bool 
VolumeSliceCoordinateSelection::isSliceCoronalEnabled() const
{
    return m_sliceEnabledCoronal;
}

/**
 * Set the enabled status of the coronal slice.
 * @param sliceEnabledCoronal 
 *    New enabled status.
 */ 
void 
VolumeSliceCoordinateSelection::setSliceCoronalEnabled(const bool sliceEnabledCoronal)
{
    m_sliceEnabledCoronal = sliceEnabledCoronal;
}

/**
 * Is the axial slice enabled?
 * @return
 *    Enabled status of axial slice.
 */
bool 
VolumeSliceCoordinateSelection::isSliceAxialEnabled() const
{
    return m_sliceEnabledAxial;
}

/**
 * Set the enabled status of the axial slice.
 * @param sliceEnabledAxial 
 *    New enabled status.
 */ 
void 
VolumeSliceCoordinateSelection::setSliceAxialEnabled(const bool sliceEnabledAxial)
{
    m_sliceEnabledAxial = sliceEnabledAxial;
}

/**
 * Copy selections from another volume slice selections.
 * @param vscs
 *   Other selections that are copied to 'this' instance.
 */
void 
VolumeSliceCoordinateSelection::copySelections(const VolumeSliceCoordinateSelection& vscs)
{
    m_sliceCoordinateParasagittal = vscs.m_sliceCoordinateParasagittal;
    m_sliceCoordinateCoronal      = vscs.m_sliceCoordinateCoronal;
    m_sliceCoordinateAxial        = vscs.m_sliceCoordinateAxial;
    m_sliceEnabledParasagittal = vscs.m_sliceEnabledParasagittal;
    m_sliceEnabledCoronal      = vscs.m_sliceEnabledCoronal;
    m_sliceEnabledAxial        = vscs.m_sliceEnabledAxial;
    m_initializedFlag = true;
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
VolumeSliceCoordinateSelection::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSliceCoordinateSelection",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
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
VolumeSliceCoordinateSelection::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    /*
     * Will prevent slices, that were applied using data from the scene
     * from being overwritten.
     */
    m_initializedFlag = true;
}


