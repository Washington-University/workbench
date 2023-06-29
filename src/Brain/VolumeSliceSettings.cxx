
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

#include "CaretPreferences.h"
#include "DeveloperFlagsEnum.h"
#include "CaretLogger.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneEnumeratedType.h"
#include "SessionManager.h"
#include "VolumeFile.h"
#include "VolumeMprSettings.h"

using namespace caret;


    
/**
 * \class caret::VolumeSliceSettings 
 * \brief Settings that control the display of volume slices.
 * \ingroup Brain
 */

const static VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum defaultVolumeSliceInterpolationEdgeMaskType =
                                                    VolumeSliceInterpolationEdgeEffectsMaskingEnum::LOOSE;
/**
 * Constructor.
 */
VolumeSliceSettings::VolumeSliceSettings()
: CaretObject()
{
    m_mprSettings.reset(new VolumeMprSettings());
    reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<VolumeSliceViewPlaneEnum,VolumeSliceViewPlaneEnum::Enum>("m_sliceViewPlane",
                                                                                   &m_sliceViewPlane);
    m_sceneAssistant->add<VolumeSliceViewAllPlanesLayoutEnum, VolumeSliceViewAllPlanesLayoutEnum::Enum>("m_slicePlanesAllViewLayout",
                                                                                                        &m_slicePlanesAllViewLayout);
    m_sceneAssistant->add<VolumeSliceDrawingTypeEnum,VolumeSliceDrawingTypeEnum::Enum>("m_sliceDrawingType",
                                                                                   &m_sliceDrawingType);
    m_sceneAssistant->add<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>("m_sliceProjectionType",
                                                                                 &m_sliceProjectionType);
    m_sceneAssistant->add<VolumeSliceInterpolationEdgeEffectsMaskingEnum,VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum>("m_volumeSliceInterpolationEdgeEffectsMaskingType",
                                                                                             &m_volumeSliceInterpolationEdgeEffectsMaskingType);
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
    m_sceneAssistant->add("m_mprSettings", "VolumeMprSettings", m_mprSettings.get());
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
    m_volumeSliceInterpolationEdgeEffectsMaskingType = obj.m_volumeSliceInterpolationEdgeEffectsMaskingType;
    
    m_montageNumberOfColumns = obj.m_montageNumberOfColumns;
    m_montageNumberOfRows    = obj.m_montageNumberOfRows;
    m_montageSliceSpacing    = obj.m_montageSliceSpacing;

    m_sliceCoordinateParasagittal = obj.m_sliceCoordinateParasagittal;
    m_sliceCoordinateCoronal      = obj.m_sliceCoordinateCoronal;
    m_sliceCoordinateAxial        = obj.m_sliceCoordinateAxial;
    m_sliceEnabledParasagittal = obj.m_sliceEnabledParasagittal;
    m_sliceEnabledCoronal      = obj.m_sliceEnabledCoronal;
    m_sliceEnabledAxial        = obj.m_sliceEnabledAxial;
    
    *m_mprSettings = *obj.m_mprSettings;
    
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
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum
VolumeSliceSettings::getVolumeSliceInterpolationEdgeEffectsMaskingType() const
{
    return m_volumeSliceInterpolationEdgeEffectsMaskingType;
}

/**
 * Set the masking used when drawing an oblique volume slice.
 *
 * @param maskingType
 *     Type of masking.
 */
void
VolumeSliceSettings::setVolumeSliceInterpolationEdgeEffectsMaskingType(const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType)
{
    m_volumeSliceInterpolationEdgeEffectsMaskingType = maskingType;
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
    
    switch (m_sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            /*
             * Switch to ALL view for MPR if NOT in MONTAGE
             */
            switch (m_sliceDrawingType) {
                case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                    break;
                case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                    setSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
                    break;
            }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }
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
 * Reset the slices.
 */
void
VolumeSliceSettings::reset()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    m_sliceViewPlane         = VolumeSliceViewPlaneEnum::AXIAL;
    m_slicePlanesAllViewLayout = prefs->getVolumeAllSlicePlanesLayout();
    m_sliceDrawingType       = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
    m_sliceProjectionType    = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL;
    m_volumeSliceInterpolationEdgeEffectsMaskingType = defaultVolumeSliceInterpolationEdgeMaskType;
    
    m_montageNumberOfColumns = 6;
    m_montageNumberOfRows    = 4;
    m_montageSliceSpacing    = 5;
    
    m_sliceCoordinateAxial = 0.0;
    m_sliceCoordinateCoronal = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
    m_sliceEnabledAxial = true;
    m_sliceEnabledCoronal = true;
    m_sliceEnabledParasagittal = true;
    m_initializedFlag = false;
    
    CaretAssert(m_mprSettings);
    m_mprSettings->reset();
    
    m_initializedFlag = false;
}

/**
 * Set the slice indices so that they are at the origin.  However, if the coordinate (0, 0, 0) is outside
 * of the volume, select coordinate at the middle of the volume.
 * @param volumeInterface
 *    The underlay volume (may be NULL)
 */
void
VolumeSliceSettings::selectSlicesAtOrigin(const VolumeMappableInterface* volumeInterface)
{
    m_sliceCoordinateAxial        = 0.0;
    m_sliceCoordinateCoronal      = 0.0;
    m_sliceCoordinateParasagittal = 0.0;
    
    /*
     * If (0, 0, 0) is not within the volume
     * set selected slices to "middle" of the volume.
     */
    if (volumeInterface != NULL) {
        int64_t indexI(0), indexJ(0), indexK(0);
        volumeInterface->enclosingVoxel(0.0, 0.0, 0.0, indexI, indexJ, indexK);
        if ( ! volumeInterface->indexValid(indexI, indexJ, indexK)) {
            int64_t dimI, dimJ, dimK, dimTime, dimCompontents;
            volumeInterface->getDimensions(dimI, dimJ, dimK, dimTime, dimCompontents);
            if ((dimI > 1) && (dimJ > 1) && (dimK > 1)) {
                indexI = dimI / 2;
                indexJ = dimJ / 2;
                indexK = dimK / 2;
                if (volumeInterface->indexValid(indexI, indexJ, indexK)) {
                    volumeInterface->indexToSpace((float)indexI, (float)indexJ, (float)indexK,
                                                  m_sliceCoordinateParasagittal, m_sliceCoordinateCoronal, m_sliceCoordinateAxial);
                }
            }
        }
    }
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
 * Get the slices for a volume in any orientation.
 *
 * @param volumeInterface
 *     Volume interface for file used for slices and orientations.
 * @param parasagittalIndexOut
 *     Output containing parasagittal index.
 * @param coronalIndexOut
 *     Output containing coronal index.
 * @param axialIndexOut
 *     Output containing axial index.
 */
void
VolumeSliceSettings::getSlicesParasagittalCoronalAxial(const VolumeMappableInterface* volumeInterface,
                                                       int64_t& parasagittalIndexOut,
                                                       int64_t& coronalIndexOut,
                                                       int64_t& axialIndexOut) const
{
    CaretAssert(volumeInterface);
    
    std::vector<int64_t> dimensions;
    volumeInterface->getDimensions(dimensions);
    
    if (dimensions[2] >= 0) {
        std::vector<int64_t> dimensions;
        volumeInterface->getDimensions(dimensions);
        
        /*
         * Valid voxel range in space
         */
        float minXYZ[3];
        volumeInterface->indexToSpace(0, 0, 0, minXYZ);
        float maxXYZ[3];
        volumeInterface->indexToSpace(dimensions[0] - 1, dimensions[1] - 1, dimensions[2] - 1, maxXYZ);
        for (int32_t i = 0; i < 3; i++) {
            if (minXYZ[i] > maxXYZ[i]) {
                std::swap(minXYZ[i], maxXYZ[i]);
            }
        }
        
        /*
         * Limit selected slice coordinates to space
         */
        float xyz[3] = {
            m_sliceCoordinateParasagittal,
            m_sliceCoordinateCoronal,
            m_sliceCoordinateAxial
        };
        for (int32_t i = 0; i < 3; i++) {
            if (xyz[i] < minXYZ[i]) {
                xyz[i] = minXYZ[i];
            }
            else if (xyz[i] > maxXYZ[i]) {
                xyz[i] = maxXYZ[i];
            }
        }
        
        int64_t indices[3];
        volumeInterface->enclosingVoxel(xyz[0], xyz[1], xyz[2],
                                        indices[0], indices[1], indices[2]);
        CaretAssert((indices[0] >= 0) && (indices[0] < dimensions[0]));
        CaretAssert((indices[1] >= 0) && (indices[1] < dimensions[1]));
        CaretAssert((indices[2] >= 0) && (indices[2] < dimensions[2]));
        
        parasagittalIndexOut = indices[0];
        coronalIndexOut      = indices[1];
        axialIndexOut        = indices[2];
        
        {
            const VolumeSpace& volumeSpace = volumeInterface->getVolumeSpace();
            if (volumeSpace.isPlumb()) {
                VolumeSpace::OrientTypes orient[3];
                volumeSpace.getOrientation(orient);
                
                for (int32_t i = 0; i < 3; i++) {
                    switch (orient[i]) {
                        case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                        case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                            coronalIndexOut = indices[i];
                            break;
                        case VolumeSpace::INFERIOR_TO_SUPERIOR:
                        case VolumeSpace::SUPERIOR_TO_INFERIOR:
                            axialIndexOut = indices[i];
                            break;
                        case VolumeSpace::LEFT_TO_RIGHT:
                        case VolumeSpace::RIGHT_TO_LEFT:
                            parasagittalIndexOut = indices[i];
                            break;
                    }
                }
            }
        }
    }
    else {
        parasagittalIndexOut = 0;
        coronalIndexOut      = 0;
        axialIndexOut        = 0;
    }
}


/**
 * Set the selected slices for volume with any orientation.  Just one
 * slice can be set by using a negative value for the other slices.
 *
 * @param volumeInterface
 *     Volume interface for file used for slices and orientations.
 * @param newParasagittalIndex
 *     New parasagittal index (ignored if less than zero)
 * @param newCoronalIndex
 *     New coronal index (ignored if less than zero)
 * @param newAxialIndex
 *     New axial index (ignored if less than zero)
 */
void
VolumeSliceSettings::setSlicesParasagittalCoronalAxial(const VolumeMappableInterface* volumeInterface,
                                                       const int64_t newParasagittalIndex,
                                                       const int64_t newCoronalIndex,
                                                       const int64_t newAxialIndex)
{
    int64_t parasagittalIndex, coronalIndex, axialIndex;
    getSlicesParasagittalCoronalAxial(volumeInterface,
                                      parasagittalIndex,
                                      coronalIndex,
                                      axialIndex);
    
    if (newParasagittalIndex >= 0) {
        parasagittalIndex = newParasagittalIndex;
    }
    if (newCoronalIndex >= 0) {
        coronalIndex = newCoronalIndex;
    }
    if (newAxialIndex >= 0) {
        axialIndex = newAxialIndex;
    }
    
    int64_t indices[3] = { parasagittalIndex, coronalIndex, axialIndex };
    
    {
        const VolumeSpace& volumeSpace = volumeInterface->getVolumeSpace();
        if (volumeSpace.isPlumb()) {
            VolumeSpace::OrientTypes orient[3];
            volumeSpace.getOrientation(orient);
            for (int32_t i = 0; i < 3; i++) {
                switch (orient[i]) {
                    case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                    case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                        indices[i] = coronalIndex;
                        break;
                    case VolumeSpace::INFERIOR_TO_SUPERIOR:
                    case VolumeSpace::SUPERIOR_TO_INFERIOR:
                        indices[i] = axialIndex;
                        break;
                    case VolumeSpace::LEFT_TO_RIGHT:
                    case VolumeSpace::RIGHT_TO_LEFT:
                        indices[i] = parasagittalIndex;
                        break;
                }
            }
        }
    }
    
    std::vector<int64_t> dimensions;
    volumeInterface->getDimensions(dimensions);
    for (int32_t i = 0; i < 3; i++) {
        if (indices[i] < 0) {
            indices[i] = 0;
        }
        else if (indices[i] >= dimensions[i]) {
            indices[i] = dimensions[i] - 1;
        }
    }
    
    float xyz[3];
    volumeInterface->indexToSpace(indices,
                                  xyz);
    selectSlicesAtCoordinate(xyz);
}

/**
 * Return the axial slice index.
 * @return
 *   Axial slice index or negative if invalid
 */
int64_t
VolumeSliceSettings::getSliceIndexAxial(const VolumeMappableInterface* volumeFile) const
{
    int64_t paraIndex, coronalIndex, axialIndex;
    getSlicesParasagittalCoronalAxial(volumeFile, paraIndex, coronalIndex, axialIndex);
    return axialIndex;
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
    setSlicesParasagittalCoronalAxial(volumeFile, -1, -1, sliceIndexAxial);
}

/**
 * Return the coronal slice index.
 * @return
 *   Coronal slice index.
 */
int64_t
VolumeSliceSettings::getSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const
{
    int64_t paraIndex, coronalIndex, axialIndex;
    getSlicesParasagittalCoronalAxial(volumeFile, paraIndex, coronalIndex, axialIndex);
    return coronalIndex;
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
    setSlicesParasagittalCoronalAxial(volumeFile, -1, sliceIndexCoronal, -1);
}

/**
 * Return the parasagittal slice index.
 * @return
 *   Parasagittal slice index.
 */
int64_t
VolumeSliceSettings::getSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const
{
    int64_t paraIndex, coronalIndex, axialIndex;
    getSlicesParasagittalCoronalAxial(volumeFile, paraIndex, coronalIndex, axialIndex);
    return paraIndex;
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
    setSlicesParasagittalCoronalAxial(volumeFile, sliceIndexParasagittal, -1, -1);
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
 * @return The MPR settings
 */
VolumeMprSettings*
VolumeSliceSettings::getMprSettings()
{
    return m_mprSettings.get();
}

/**
 * @return the MPR settings (const)
 */
const VolumeMprSettings*
VolumeSliceSettings::getMprSettings() const
{
    return m_mprSettings.get();
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
                                            3);  // 3 -> new MPR modes including average
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
    m_volumeSliceInterpolationEdgeEffectsMaskingType = defaultVolumeSliceInterpolationEdgeMaskType;
    
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

    if (sceneClass->getVersionNumber() < 3) {
        /*
         * For scenes before all view layout, use GRID so that the
         * older scenes display correctly
         */
        if (sceneClass->getObjectWithName("m_slicePlanesAllViewLayout") == NULL) {
            m_slicePlanesAllViewLayout = VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT;
        }
        
        /*
         * Intensity mode was available early in Intensity mode development then
         * moved to volume slice projection type.
         */
        const AString oldMprIntensityMode(sceneClass->getEnumeratedTypeValueAsString("m_mprIntensityProjectionMode"));
        if ( ! oldMprIntensityMode.isEmpty()) {
            if (oldMprIntensityMode == "MAXIMUM") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setViewMode(VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION);
            }
            else if (oldMprIntensityMode == "MINIMUM") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setViewMode(VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION);
            }
        }
        
        /*
         * MPR Neurological and Radiological were combined into VOLUME_SLICE_PROJECTION_MPR
         * and MPR orientation was added
         */
        const AString oldSliceProjectionType(sceneClass->getEnumeratedTypeValueAsString("m_sliceProjectionType"));
        if ( ! oldSliceProjectionType.isEmpty()) {
            if (oldSliceProjectionType == "VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setOrientationMode(VolumeMprOrientationModeEnum::NEUROLOGICAL);
            }
            else if (oldSliceProjectionType == "VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setOrientationMode(VolumeMprOrientationModeEnum::RADIOLOGICAL);
            }
            else if (oldSliceProjectionType == "VOLUME_SLICE_PROJECTION_MPR") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setViewMode(VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION);
            }
            else if (oldSliceProjectionType == "VOLUME_SLICE_PROJECTION_MPR_MAXIMUM_INTENSITY") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setViewMode(VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION);
            }
            else if (oldSliceProjectionType == "VOLUME_SLICE_PROJECTION_MPR_MINIMUM_INTENSITY") {
                m_sliceProjectionType = VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR;
                m_mprSettings->setViewMode(VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION);
            }
        }
        
        /*
         * old mpr orientation moved to VolumeMprSettings
         */
        const AString oldMprOrientation(sceneClass->getEnumeratedTypeValueAsString("m_mprOrientationMode"));
        if ( ! oldMprOrientation.isEmpty()) {
            if (oldMprOrientation == "NEUROLOGICAL") {
                m_mprSettings->setOrientationMode(VolumeMprOrientationModeEnum::NEUROLOGICAL);
            }
            else if (oldMprOrientation == "RADIOLOGICAL") {
                m_mprSettings->setOrientationMode(VolumeMprOrientationModeEnum::RADIOLOGICAL);
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

