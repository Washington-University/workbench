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
/*LICENSE_END*/
#ifndef __CIFTI_XML_ELEMENTS
#define __CIFTI_XML_ELEMENTS
#include <QtCore>
#include <vector>
#include "nifti2.h"
#include "StructureEnum.h"
/* Cifti Defines */

namespace caret {

/*! ModelType */
enum ModelType {
    CIFTI_MODEL_TYPE_SURFACE=1,/*!< CIFTI_MODEL_TYPE_SURFACE*/
    CIFTI_MODEL_TYPE_VOXELS=2/*!< CIFTI_MODEL_TYPE_VOXELS*/
};

/*! IndicesMapToDataType*/
enum IndicesMapToDataType {
    CIFTI_INDEX_TYPE_INVALID,
    CIFTI_INDEX_TYPE_BRAIN_MODELS=1,/*!< CIFTI_INDEX_TYPE_BRAIN_MODELS*/
    CIFTI_INDEX_TYPE_FIBERS=2,/*!< CIFTI_INDEX_TYPE_FIBERS*/
    CIFTI_INDEX_TYPE_PARCELS=3,/*!< CIFTI_INDEX_TYPE_PARCELS*/
    CIFTI_INDEX_TYPE_TIME_POINTS=4/*!< CIFTI_INDEX_TYPE_TIME_POINTS*/
};

typedef int voxelIndexType;

/// Cifti Brain Model XML Element
class CiftiBrainModelElement {
public:
    //CiftiBrainModelElement();

    unsigned long long m_indexOffset; /*!< Index of first element in dimension of the matrix for this brain structure. The value is the number of elements, NOT the number of bytes. */
    unsigned long long m_indexCount; /*!< Number of elements in this brain model. */
    ModelType m_modelType; /*!< Type of model representing the brain structure. */
    StructureEnum::Enum m_brainStructure; /*!<  Identifies the brain structure. Valid values are contained in nifti2.h */
    unsigned long long m_surfaceNumberOfNodes; /*!< This attribute contains the actual (or true) number of nodes in the surface that is associated with this BrainModel.*/
    //children
    std::vector<unsigned long long> m_nodeIndices; /*!< Contains a list of nodes indices for a BrainModel with ModelType equal to CIFTI_MODEL_TYPE_SURFACE.*/
    std::vector<voxelIndexType> m_voxelIndicesIJK; /*!<  Identifies the voxels that model a brain structure. */
    std::vector<unsigned long long> m_nodeToIndexLookup;//used by CiftiXML to quickly lookup indexes by node number
    void setupLookup();//convenience function to populate lookup
};

/// Cifti Matrix Indices Map XML Element
class CiftiMatrixIndicesMapElement
{
public:
    CiftiMatrixIndicesMapElement()
    {
        m_timeStep = -1.0;
        m_timeStepUnits = -1;
        m_numTimeSteps = -1;
        m_indicesMapToDataType = CIFTI_INDEX_TYPE_INVALID;
    }

    std::vector<int> m_appliesToMatrixDimension; /*!< Lists the dimension(s) of the matrix to which this MatrixIndicesMap applies. */
    IndicesMapToDataType m_indicesMapToDataType; /*!< Type of data to which the MatrixIndicesMap applies.  */
    double m_timeStep; /*!< Indicates amount of time between each timepoint. */
    int m_timeStepUnits;/*!< Indicates units of TimeStep. */
    int m_numTimeSteps;//used by CiftiXML to store the information that is critically lacking in the XML extension
    std::vector<CiftiBrainModelElement> m_brainModels;/*!< A vector array of Brain Models */
};

/// Cifti Label XML Element
class CiftiLabelElement {
public:
    CiftiLabelElement()
    {
        m_red = m_green = m_blue = m_alpha = m_x = m_y = m_z = 0.0;
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
    std::vector<CiftiLabelElement> m_labelTable;/*!< The Matrix's Label Table (optional)*///TODO, this may be better as a hash
    QHash<QString, QString> m_userMetaData;/*!< User Meta Data*/
    std::vector<CiftiMatrixIndicesMapElement> m_matrixIndicesMap;/*!< Vector array of one or more Matrix Indices Map Elements*/
    std::vector<CiftiVolumeElement> m_volume;/*!< Volume Element*/
};

/// Cifti Root XML Element
class CiftiRootElement {
public:
    CiftiRootElement() { m_numberOfMatrices = 0; }
    QString m_version;/*!<  Version String*/
    unsigned long m_numberOfMatrices;/*!< Number of Matrices*/
    std::vector<CiftiMatrixElement> m_matrices; /*!< Matrices, currently there is only matrix, but future versions may allow for more */
};

}


#endif //__CIFTI_ELEMENTS
