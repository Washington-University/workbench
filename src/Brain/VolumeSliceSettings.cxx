
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __VOLUME_SLICE_SETTINGS_DECLARE__
#include "VolumeSliceSettings.h"
#undef __VOLUME_SLICE_SETTINGS_DECLARE__

#include "CaretLogger.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneEnumeratedType.h"
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
    m_slicePlanesAllViewLayout = VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT;
    m_sliceDrawingType       = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
    m_sliceProjectionType    = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
    m_obliqueSliceDrawingMaskingType = VolumeSliceObliqueDrawingMaskEnum::ENCLOSING_VOXEL;
    
    m_montageNumberOfColumns = 6; // was 3;
    m_montageNumberOfRows    = 4;
    m_montageSliceSpacing    = 5;
    
    m_sliceCoordinateAxial = 0.0;
    m_sliceCoordinateCoronal = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
    m_sliceEnabledAxial = true;
    m_sliceEnabledCoronal = true;
    m_sliceEnabledParasagittal = true;
    m_initializedFlag = false;
    
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<VolumeSliceViewPlaneEnum,VolumeSliceViewPlaneEnum::Enum>("m_sliceViewPlane",
                                                                                   &m_sliceViewPlane);
    m_sceneAssistant->add<VolumeSliceViewAllPlanesLayoutEnum, VolumeSliceViewAllPlanesLayoutEnum::Enum>("m_slicePlanesAllViewLayout",
                                                                                                        &m_slicePlanesAllViewLayout);
    m_sceneAssistant->add<VolumeSliceDrawingTypeEnum,VolumeSliceDrawingTypeEnum::Enum>("m_sliceDrawingType",
                                                                                   &m_sliceDrawingType);
    m_sceneAssistant->add<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>("m_sliceProjectionType",
                                                                                 &m_sliceProjectionType);
    m_sceneAssistant->add<VolumeSliceObliqueDrawingMaskEnum,VolumeSliceObliqueDrawingMaskEnum::Enum>("m_obliqueSliceDrawingMaskingType",
                                                                                             &m_obliqueSliceDrawingMaskingType);
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
    m_slicePlanesAllViewLayout = obj.m_slicePlanesAllViewLayout;
    m_sliceDrawingType       = obj.m_sliceDrawingType;
    m_sliceProjectionType    = obj.m_sliceProjectionType;
    m_obliqueSliceDrawingMaskingType = obj.m_obliqueSliceDrawingMaskingType;
    
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
 * Get a text description of the instance's content.
 *
 * @param modelType
 *    Type of model.
 * @param descriptionOut
 *    Description of the instance's content.
 */
void
VolumeSliceSettings::getDescriptionOfContent(const ModelTypeEnum::Enum modelType,
                                             PlainTextStringBuilder& descriptionOut) const
{
    bool volumeFlag = false;
    bool wholeBrainFlag = false;
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            volumeFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            wholeBrainFlag = true;
            break;
    }
    
    bool showParasagittalCoordinate = false;
    bool showCoronalCoordinate      = false;
    bool showAxialCoordinate        = false;
    
    descriptionOut.addLine("Volume Slice Settings: ");
    
    descriptionOut.pushIndentation();
    
    if (volumeFlag) {
        descriptionOut.addLine("   View Plane: "
                              + VolumeSliceViewPlaneEnum::toGuiName(m_sliceViewPlane));
        showParasagittalCoordinate = true;
        showCoronalCoordinate      = true;
        showAxialCoordinate        = true;
    }
    
    if (wholeBrainFlag) {
        if (m_sliceEnabledParasagittal) {
            showParasagittalCoordinate = true;
        }
        if (m_sliceEnabledCoronal) {
            showCoronalCoordinate = true;
        }
        if (m_sliceEnabledAxial) {
            showAxialCoordinate = true;
        }
    }
    
    if (showParasagittalCoordinate) {
        descriptionOut.addLine("   Parasagittal (X-axis) Coordinate: "
                              + AString::number(m_sliceCoordinateParasagittal, 'f', 3));
    }
    if (showCoronalCoordinate) {
        descriptionOut.addLine("   Coronal (Y-axis) Coordinate:      "
                              + AString::number(m_sliceCoordinateCoronal, 'f', 3));
    }
    if (showAxialCoordinate) {
        descriptionOut.addLine("   Axial (Z-axis) Coordinate:        "
                              + AString::number(m_sliceCoordinateAxial, 'f', 3));
    }
    
    descriptionOut.popIndentation();
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
 * @return The layout for all slices view (grid, row, column)
 */
VolumeSliceViewAllPlanesLayoutEnum::Enum
VolumeSliceSettings::getSlicePlanesAllViewLayout() const
{
    return m_slicePlanesAllViewLayout;
}

/**
 * Set the layout for all slices view (grid, row, column)
 *
 * @param slicePlanesAllViewLayout
 *     New value for layout.
 */
void
VolumeSliceSettings::setSlicePlanesAllViewLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum slicePlanesAllViewLayout)
{
    m_slicePlanesAllViewLayout = slicePlanesAllViewLayout;
}


/**
 * @return Type of slice drawing (single/montage)
 */
VolumeSliceDrawingTypeEnum::Enum
VolumeSliceSettings::getSliceDrawingType() const
{
    return m_sliceDrawingType;
}

/**
 * Set type of slice drawing (single/montage)
 *
 * @param sliceDrawingType
 *    New value for slice drawing type.
 */
void
VolumeSliceSettings::setSliceDrawingType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType)
{
    m_sliceDrawingType = sliceDrawingType;
}

/**
 * @return The masking used when drawing an oblique volume slice
 */
VolumeSliceObliqueDrawingMaskEnum::Enum
VolumeSliceSettings::getObliqueSliceDrawingMaskingType() const
{
    return m_obliqueSliceDrawingMaskingType;
}

/**
 * Set the masking used when drawing an oblique volume slice.
 *
 * @param maskingType
 *     Type of masking.
 */
void
VolumeSliceSettings::setObliqueSliceDrawingMaskingType(const VolumeSliceObliqueDrawingMaskEnum::Enum maskingType)
{
    m_obliqueSliceDrawingMaskingType = maskingType;
}

/**
 * @return Type of slice projection (oblique/orthogonal)
 */
VolumeSliceProjectionTypeEnum::Enum
VolumeSliceSettings::getSliceProjectionType() const
{
    return m_sliceProjectionType;
}

/**
 * Set type of slice projection (oblique/orthogonal)
 *
 * @param sliceProjectionType
 *    New value for slice projection type.
 */
void
VolumeSliceSettings::setSliceProjectionType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType)
{
    m_sliceProjectionType = sliceProjectionType;
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
VolumeSliceSettings::updateForVolumeFile(const VolumeMappableInterface* volumeFile)
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
VolumeSliceSettings::getSliceIndexAxial(const VolumeMappableInterface* volumeFile) const
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
VolumeSliceSettings::setSliceIndexAxial(const VolumeMappableInterface* volumeFile,
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
VolumeSliceSettings::getSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const
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
VolumeSliceSettings::setSliceIndexCoronal(const VolumeMappableInterface* volumeFile,
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
VolumeSliceSettings::getSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const
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
VolumeSliceSettings::setSliceIndexParasagittal(const VolumeMappableInterface* volumeFile,
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
                                            2);
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
    
    /*
     * Added in scene version 2 or later
     */
    m_sliceDrawingType       = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
    m_sliceProjectionType    = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
    m_obliqueSliceDrawingMaskingType = VolumeSliceObliqueDrawingMaskEnum::ENCLOSING_VOXEL;
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    if (sceneClass->getVersionNumber() < 2) {
        /*
         * Set slice drawing type and projection type using old slice view mode.
         */
        const AString oldViewModeValue = sceneClass->getEnumeratedTypeValueAsString("m_sliceViewMode");
        if (! oldViewModeValue.isEmpty()) {
            if (oldViewModeValue == "MONTAGE") {
                m_sliceDrawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE;
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
            }
            else if (oldViewModeValue == "OBLIQUE") {
                m_sliceDrawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE;
            }
            else if (oldViewModeValue == "ORTHOGONAL") {
                m_sliceDrawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
            }
            else {
                CaretLogWarning("Unrecognized value for old m_sliceViewMode: "
                                + oldViewModeValue);
            }
        }
    }

    /*
     * Restoring scene initialize all members.
     * If this is not done, the slices will be reset
     * in updateForVolumeFile().
     */
    m_initializedFlag = true;
}

