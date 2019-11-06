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
#ifndef __CIFTI_XML_ELEMENTS
#define __CIFTI_XML_ELEMENTS
#include <QtCore>
#include <vector>
#include <map>
#include "AString.h"
#include "CaretPointer.h"
#include "CaretCompact3DLookup.h"
#include "CiftiVersion.h"
#include "nifti2.h"
#include "StructureEnum.h"
/* Cifti Defines */

namespace caret {

class PaletteColorMapping;

class GiftiLabelTable;
/*! ModelType */
enum ModelType {
    CIFTI_MODEL_TYPE_INVALID,
    CIFTI_MODEL_TYPE_SURFACE=1,/*!< CIFTI_MODEL_TYPE_SURFACE*/
    CIFTI_MODEL_TYPE_VOXELS=2/*!< CIFTI_MODEL_TYPE_VOXELS*/
};

/*! IndicesMapToDataType*/
enum IndicesMapToDataType {
    CIFTI_INDEX_TYPE_INVALID,
    CIFTI_INDEX_TYPE_BRAIN_MODELS=1,/*!< CIFTI_INDEX_TYPE_BRAIN_MODELS*/
    CIFTI_INDEX_TYPE_FIBERS=2,/*!< CIFTI_INDEX_TYPE_FIBERS*/
    CIFTI_INDEX_TYPE_PARCELS=3,/*!< CIFTI_INDEX_TYPE_PARCELS*/
    CIFTI_INDEX_TYPE_TIME_POINTS=4,/*!< CIFTI_INDEX_TYPE_TIME_POINTS*/
    CIFTI_INDEX_TYPE_SCALARS=5,
    CIFTI_INDEX_TYPE_LABELS=6
};

typedef int voxelIndexType;

class CiftiMatrixIndicesMapElement;

/// Cifti Brain Model XML Element
class CiftiBrainModelElement {
public:
    //CiftiBrainModelElement();

    int64_t m_indexOffset; /*!< Index of first element in dimension of the matrix for this brain structure. The value is the number of elements, NOT the number of bytes. */
    int64_t m_indexCount; /*!< Number of elements in this brain model. */
    ModelType m_modelType; /*!< Type of model representing the brain structure. */
    StructureEnum::Enum m_brainStructure; /*!<  Identifies the brain structure. Valid values are contained in nifti2.h */
    int64_t m_surfaceNumberOfNodes; /*!< This attribute contains the actual (or true) number of nodes in the surface that is associated with this BrainModel.*/
    //children
    std::vector<int64_t> m_nodeIndices; /*!< Contains a list of nodes indices for a BrainModel with ModelType equal to CIFTI_MODEL_TYPE_SURFACE.*/
    std::vector<voxelIndexType> m_voxelIndicesIJK; /*!<  Identifies the voxels that model a brain structure. */
    std::vector<int64_t> m_nodeToIndexLookup;//used by CiftiXML to quickly lookup indexes by node number
    void setupLookup(CiftiMatrixIndicesMapElement& myMap);//convenience function to populate lookup
    bool operator==(const CiftiBrainModelElement& rhs) const;
    bool operator<(const CiftiBrainModelElement& rhs) const;//for sorting by starting index
};

struct CiftiNamedMapElement
{
    mutable AString m_mapName;
    mutable CaretPointer<GiftiLabelTable> m_labelTable;
    mutable std::map<AString, AString> m_mapMetaData;/*!< User Meta Data*/
    mutable CaretPointer<PaletteColorMapping> m_palette;//palette settings storage
    mutable bool m_defaultPalette;//secondary variable to enable resetting the palette to use defaults, which will make it not write the palette to file
    CiftiNamedMapElement();
    CiftiNamedMapElement(const CiftiNamedMapElement& rhs);//to turn copy and assignment into a deep copy of the label table
    ~CiftiNamedMapElement();
    CiftiNamedMapElement& operator=(const CiftiNamedMapElement& rhs);
    bool operator==(const CiftiNamedMapElement& rhs) const;
    bool operator!=(const CiftiNamedMapElement& rhs) const { return !(*this == rhs); }
};

struct CiftiParcelNodesElement
{
    StructureEnum::Enum m_structure;
    std::vector<int64_t> m_nodes;
    bool operator==(const CiftiParcelNodesElement& rhs) const;
    bool operator!=(const CiftiParcelNodesElement& rhs) const { return !(*this == rhs); }
};

struct CiftiParcelElement
{
    AString m_parcelName;
    std::vector<CiftiParcelNodesElement> m_nodeElements;
    std::vector<voxelIndexType> m_voxelIndicesIJK;
    bool operator==(const CiftiParcelElement& rhs) const;
    bool operator!=(const CiftiParcelElement& rhs) const { return !(*this == rhs); }
};

struct CiftiParcelSurfaceElement
{
    StructureEnum::Enum m_structure;
    int64_t m_numNodes;
    std::vector<int64_t> m_lookup;
    bool operator==(const CiftiParcelSurfaceElement& rhs) const;
    bool operator!=(const CiftiParcelSurfaceElement& rhs) const { return !(*this == rhs); }
};

/// Cifti Matrix Indices Map XML Element
class CiftiMatrixIndicesMapElement
{
public:
    CiftiMatrixIndicesMapElement()
    {
        m_timeStep = -1.0;
        m_timeStart = -1.0;
        m_hasTimeStart = false;
        m_timeStepUnits = -1;
        m_numTimeSteps = -1;
        m_indicesMapToDataType = CIFTI_INDEX_TYPE_INVALID;
    }

    std::vector<int> m_appliesToMatrixDimension; /*!< Lists the dimension(s) of the matrix to which this MatrixIndicesMap applies. */
    IndicesMapToDataType m_indicesMapToDataType; /*!< Type of data to which the MatrixIndicesMap applies.  */
    double m_timeStep; /*!< Indicates amount of time between each timepoint. */
    double m_timeStart;
    bool m_hasTimeStart;
    int m_timeStepUnits;/*!< Indicates units of TimeStep. */
    int m_numTimeSteps;//used by CiftiXML to store the information that is critically lacking in the XML extension
    std::vector<CiftiBrainModelElement> m_brainModels;/*!< A vector array of Brain Models */
    CaretCompact3DLookup<int64_t> m_voxelToIndexLookup;//make one unified lookup rather than separate lookups per volume structure
    std::vector<CiftiNamedMapElement> m_namedMaps;
    std::vector<CiftiParcelElement> m_parcels;
    std::vector<CiftiParcelSurfaceElement> m_parcelSurfaces;
    void setupLookup();
    bool operator==(const CiftiMatrixIndicesMapElement& rhs) const;
};

/// Cifti Label XML Element
class CiftiLabelElement {
public:
    CiftiLabelElement()
    {
        m_red = m_green = m_blue = m_alpha = m_x = m_y = m_z = 0.0;
        m_key = 0;
    }
    unsigned long long m_key;/*!< Corresponding index, starting at zero, of a row and/or column of the connectivity matrix.*/
    float m_red;/*!<  Red color component for label. Value is floating point with range 0.0 to 1.0.*/
    float m_green;/*!< Green color component for label. Value is floating point with range 0.0 to 1.0.*/
    float m_blue;/*!< Blue color component for label. Value is floating point with range 0.0 to 1.0.*/
    float m_alpha;/*!< Alpha color component for label. Value is floating point with range 0.0 to 1.0.*/
    float m_x;/*!< X-coordinate of spatial location associated with the label. Value is floating point.*/
    float m_y;/*!< Y-coordinate of spatial location associated with the label. Value is floating point.*/
    float m_z;/*!< Z-coordinate of spatial location associated with the label. Value is floating point.*/
    QString m_text;/*!< Text of the label.*/
};

/// Transformation Matrix Voxel Indices IJK to XYZ XML Element
class TransformationMatrixVoxelIndicesIJKtoXYZElement
{
public:
    unsigned long m_dataSpace;/*!< Contains the name of the space of the voxels prior to application of the transformation matrix.*/
    unsigned long m_transformedSpace;/*!< Contains the name of the space of the voxels after application of the transformation of voxel IJK indices to spatial XYZ coordinates. */
    unsigned long m_unitsXYZ;/*!< Identifies the units of the spatial XYZ coordinates.*/
    float m_transform[16];/*!< A 16 element matrix in row-major form which contains a  matrix that for conversion of Voxel IJK Indices to spatial XYZ coordinates (+X=>right, +Y=>anterior, +Z=>posterior).*/
};

/// Cifti Volume XML Element
class CiftiVolumeElement {
public:
    std::vector<TransformationMatrixVoxelIndicesIJKtoXYZElement> m_transformationMatrixVoxelIndicesIJKtoXYZ;/*!< Vector array of 0 or more TransformationMatrixVoxelIndicesIJKtoXYZ*/
    unsigned int m_volumeDimensions[3];/*!< Three integer values that indicate original dimensions of the volume from which the voxels were extracted.*/
};

/// Cifti Matrix XML Element
class CiftiMatrixElement {
public:
    CiftiMatrixElement(const CiftiMatrixElement& rhs);
    CiftiMatrixElement& operator=(const CiftiMatrixElement& rhs);
    CiftiMatrixElement();
    ~CiftiMatrixElement();
    std::vector<CiftiLabelElement> m_labelTable;/*!< The Matrix's Label Table (optional)*///TODO: replace this with GiftiLabelTable (or remove? not being used for anything)
    mutable std::map<AString, AString> m_userMetaData;/*!< User Meta Data*/
    mutable CaretPointer<PaletteColorMapping> m_palette;//palette settings storage
    mutable bool m_defaultPalette;//secondary variable to enable resetting the palette to use defaults, which will make it not write the palette to file
    std::vector<CiftiMatrixIndicesMapElement> m_matrixIndicesMap;/*!< Vector array of one or more Matrix Indices Map Elements*/
    std::vector<CiftiVolumeElement> m_volume;/*!< Volume Element*/
};

/// Cifti Root XML Element
class CiftiRootElement {
public:
    CiftiRootElement() { m_numberOfMatrices = 0; }
    CiftiVersion m_version;/*!<  Version String*/
    unsigned long m_numberOfMatrices;/*!< Number of Matrices*/
    std::vector<CiftiMatrixElement> m_matrices; /*!< Matrices, currently there is only matrix, but future versions may allow for more */
};

}


#endif //__CIFTI_ELEMENTS
