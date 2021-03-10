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

#include "CiftiXMLWriter.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"

using namespace caret;
using namespace std;

void CiftiXMLWriter::writeCiftiXML(QXmlStreamWriter &xml, const CiftiRootElement &rootElement)
{
    m_writingVersion = CiftiVersion(1, 0);//HACK: this writer can't support other versions of XML, the structure is too closely tied to the 1.0 representation
    xml.setAutoFormatting(true);
    xml.writeStartElement("CIFTI");
    xml.writeAttribute("Version", m_writingVersion.toString());
    xml.writeAttribute("NumberOfMatrices",QString::number(rootElement.m_numberOfMatrices));

    for(unsigned int i = 0;i<rootElement.m_numberOfMatrices;i++)
    {
        writeMatrixElement(xml, rootElement.m_matrices[i]);
    }

    xml.writeEndElement(); //CIFTI

}

void CiftiXMLWriter::writeMatrixElement(QXmlStreamWriter &xml, const CiftiMatrixElement &matrixElement)
{ 
    xml.writeStartElement("Matrix");
    map<AString, AString> metadataCopy = matrixElement.m_userMetaData;//since we may be modifying the map, we must make a copy of it
    if (matrixElement.m_palette != NULL)
    {//NULL palette means we didn't mess with palette at all
        if (matrixElement.m_defaultPalette && !(matrixElement.m_palette->isModified()))
        {//it is set to use the default palette instead of a custom palette, so remove the palette item from metadata, if it exists
            metadataCopy.erase("PaletteColorMapping");
        } else {
            metadataCopy["PaletteColorMapping"] = matrixElement.m_palette->encodeInXML();
        }
    }
    if(metadataCopy.size() > 0) writeMetaData(xml,metadataCopy);
    if(matrixElement.m_volume.size() > 0) writeVolume(xml, matrixElement.m_volume[0]);
    if(matrixElement.m_labelTable.size() > 0) writeLabelTable(xml, matrixElement.m_labelTable);

    for(unsigned int i = 0;i<matrixElement.m_matrixIndicesMap.size();i++)
    {
        writeMatrixIndicesMap(xml,matrixElement.m_matrixIndicesMap[i]);
    }

    xml.writeEndElement();//Matrix
}

void CiftiXMLWriter::writeMetaData(QXmlStreamWriter &xml, const map<AString, AString> &metaData)
{     
    xml.writeStartElement("MetaData");

    map<AString, AString>::const_iterator i;

    for (i = metaData.begin(); i != metaData.end(); ++i)
    {
        writeMetaDataElement(xml,i->first,i->second);
    }
    xml.writeEndElement();
}

void CiftiXMLWriter::writeMetaDataElement(QXmlStreamWriter &xml, const AString &name, const AString &value)
{     
    xml.writeStartElement("MD");

    xml.writeStartElement("Name");
    xml.writeCharacters(name);
    xml.writeEndElement();//Name

    xml.writeStartElement("Value");
    xml.writeCharacters(value);
    xml.writeEndElement();//Value

    xml.writeEndElement();//MD
}

void CiftiXMLWriter::writeLabelTable(QXmlStreamWriter &xml, const std::vector <CiftiLabelElement> &labelElement)
{     
    xml.writeStartElement("LabelTable");

    for(unsigned int i=0;i<labelElement.size();i++)
    {
        writeLabel(xml,labelElement[i]);
    }

    xml.writeEndElement();
}

void CiftiXMLWriter::writeLabel(QXmlStreamWriter &xml, const CiftiLabelElement &label)
{     
    xml.writeStartElement("Label");

    xml.writeAttribute("Key", QString::number(label.m_key));
    xml.writeAttribute("Red", QString::number(label.m_red));
    xml.writeAttribute("Green", QString::number(label.m_green));
    xml.writeAttribute("Blue", QString::number(label.m_blue));
    xml.writeAttribute("Alpha", QString::number(label.m_alpha));
    xml.writeAttribute("X", QString::number(label.m_x));
    xml.writeAttribute("Y", QString::number(label.m_y));
    xml.writeAttribute("Z", QString::number(label.m_z));

    xml.writeEndElement();
}

void CiftiXMLWriter::writeMatrixIndicesMap(QXmlStreamWriter &xml, const CiftiMatrixIndicesMapElement &matrixIndicesMap)
{
    xml.writeStartElement("MatrixIndicesMap");
    //TODO
    //xml.writeAttribute("AppliesToMatrixDimension", QString::number(matrixIndicesMap.m_appliesToMatrixDimension));
    QString indicesMapToDataType;
    if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS) indicesMapToDataType = "CIFTI_INDEX_TYPE_BRAIN_MODELS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_FIBERS) indicesMapToDataType = "CIFTI_INDEX_TYPE_FIBERS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) indicesMapToDataType = "CIFTI_INDEX_TYPE_PARCELS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_TIME_POINTS) indicesMapToDataType = "CIFTI_INDEX_TYPE_TIME_POINTS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_SCALARS) indicesMapToDataType = "CIFTI_INDEX_TYPE_SCALARS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_LABELS) indicesMapToDataType = "CIFTI_INDEX_TYPE_LABELS";

    xml.writeAttribute("IndicesMapToDataType",indicesMapToDataType);

    QString timeStepUnits;

    if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_SEC) timeStepUnits = "NIFTI_UNITS_SEC";
    else if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_MSEC) timeStepUnits = "NIFTI_UNITS_MSEC";
    else if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_USEC) timeStepUnits = "NIFTI_UNITS_USEC";

    if(timeStepUnits.length()>0) {
        QString str;
        xml.writeAttribute("TimeStep", QString::number(matrixIndicesMap.m_timeStep, 'f', 10));
        if (matrixIndicesMap.m_hasTimeStart) xml.writeAttribute("TimeStart",QString::number(matrixIndicesMap.m_timeStart,'f',10));
        xml.writeAttribute("TimeStepUnits",timeStepUnits);
    }
    if(matrixIndicesMap.m_appliesToMatrixDimension.size())
    {
        int lastElement = matrixIndicesMap.m_appliesToMatrixDimension.size() -1;
        QString appliesToMatrixDimension, str;
        for(int i = 0;i<lastElement;i++)
        {
            int temp = matrixIndicesMap.m_appliesToMatrixDimension[i];
            if (temp < 2 && m_writingVersion.hasReversedFirstDims()) temp = 1 - temp;//in other words, 0 becomes 1 and 1 becomes 0
            appliesToMatrixDimension.append(QString::number(temp));
        }
        int temp = matrixIndicesMap.m_appliesToMatrixDimension[lastElement];
        if (temp < 2 && m_writingVersion.hasReversedFirstDims()) temp = 1 - temp;//in other words, 0 becomes 1 and 1 becomes 0
        appliesToMatrixDimension.append(QString::number(temp));
        xml.writeAttribute("AppliesToMatrixDimension", appliesToMatrixDimension);
    }

    for(unsigned int i=0;i<matrixIndicesMap.m_brainModels.size();i++)
    {
        writeBrainModel(xml, matrixIndicesMap.m_brainModels[i]);
    }
    for (size_t i = 0; i < matrixIndicesMap.m_namedMaps.size(); ++i)
    {
        writeNamedMap(xml, matrixIndicesMap.m_namedMaps[i]);
    }
    for (size_t i = 0; i < matrixIndicesMap.m_parcelSurfaces.size(); ++i)
    {
        xml.writeStartElement("Surface");
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(matrixIndicesMap.m_parcelSurfaces[i].m_structure));
        xml.writeAttribute("SurfaceNumberOfNodes", AString::number(matrixIndicesMap.m_parcelSurfaces[i].m_numNodes));
        xml.writeEndElement();
    }
    for (size_t i = 0; i < matrixIndicesMap.m_parcels.size(); ++i)
    {
        writeParcel(xml, matrixIndicesMap.m_parcels[i]);
    }
    xml.writeEndElement();
}

void CiftiXMLWriter::writeBrainModel(QXmlStreamWriter &xml, const CiftiBrainModelElement &brainModel)
{     
    xml.writeStartElement("BrainModel");

    xml.writeAttribute("IndexOffset", QString::number(brainModel.m_indexOffset));
    xml.writeAttribute("IndexCount", QString::number(brainModel.m_indexCount));
    QString modelType, brainStructure;
    getModelTypeString(brainModel.m_modelType,modelType);
    xml.writeAttribute("ModelType",modelType);
    xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(brainModel.m_brainStructure));
    if(brainModel.m_modelType == CIFTI_MODEL_TYPE_SURFACE) xml.writeAttribute("SurfaceNumberOfNodes",QString::number(brainModel.m_surfaceNumberOfNodes));


    unsigned long long lastnodeIndex= brainModel.m_nodeIndices.size();

    if(lastnodeIndex)
    {
        xml.writeStartElement("NodeIndices");
        lastnodeIndex--;
        for(unsigned int i = 0;i<lastnodeIndex;i++)
        {
            xml.writeCharacters(QString::number(brainModel.m_nodeIndices[i]));
            xml.writeCharacters(" ");
        }
        xml.writeCharacters(QString::number(brainModel.m_nodeIndices[lastnodeIndex]));
        xml.writeEndElement();//NodeIndices
    }



    const std::vector <voxelIndexType> &ind = brainModel.m_voxelIndicesIJK;
    unsigned long long lastVoxelIndex = ind.size();
    if(lastVoxelIndex)
    {
        xml.writeStartElement("VoxelIndicesIJK");
        QString line( "%1 %2 %3\n");
        if((lastVoxelIndex%3))
        {
            std::cout << "Error writing BrainModel, invalid number of voxel indices:" << lastVoxelIndex << std::endl;
            return;//TODO throw exception
        }
        //else
        //std::cout << "voxel indices ok:" << lastVoxelIndex<< std::endl;

        for(unsigned int i = 0;i < lastVoxelIndex;i+=3)
        {
            xml.writeCharacters(line.arg(QString::number(ind[i]),QString::number(ind[i+1]),QString::number(ind[i+2])));
        }
        xml.writeEndElement();//voxelIndicesIJK
    }

    xml.writeEndElement();
}

void CiftiXMLWriter::writeNamedMap(QXmlStreamWriter& xml, const CiftiNamedMapElement& namedMap)
{
    xml.writeStartElement("NamedMap");
    xml.writeStartElement("MapName");
    xml.writeCharacters(namedMap.m_mapName);
    xml.writeEndElement();
    if (namedMap.m_labelTable != NULL)
    {
        namedMap.m_labelTable->writeAsXML(xml);
    }
    map<AString, AString> metadataCopy = namedMap.m_mapMetaData;//make a copy because we may need to modify it to integrate palette
    if (namedMap.m_palette != NULL)
    {//NULL palette means we didn't mess with palette at all, leave metadata unchanged
        if (namedMap.m_defaultPalette && !(namedMap.m_palette->isModified()))
        {//it is set to use the default palette instead of a custom palette, so remove the palette item from metadata, if it exists
            metadataCopy.erase("PaletteColorMapping");
        } else {
            metadataCopy["PaletteColorMapping"] = namedMap.m_palette->encodeInXML();
        }
    }
    if (metadataCopy.size() != 0)
    {
        writeMetaData(xml, metadataCopy);
    }
    xml.writeEndElement();
}

void CiftiXMLWriter::writeParcel(QXmlStreamWriter& xml, const CiftiParcelElement& parcel)
{
    xml.writeStartElement("Parcel");
    xml.writeAttribute("Name", parcel.m_parcelName);
    int numNodeElements = (int)parcel.m_nodeElements.size();
    for (int i = 0; i < numNodeElements; ++i)
    {
        writeParcelNodes(xml, parcel.m_nodeElements[i]);
    }
    int numVoxInds = (int)parcel.m_voxelIndicesIJK.size();
    if (numVoxInds > 0)
    {
        xml.writeStartElement("VoxelIndicesIJK");
        xml.writeCharacters(AString::number(parcel.m_voxelIndicesIJK[0]));
        int state = 0;
        for (int i = 1; i < numVoxInds; ++i)
        {
            if (state >= 2)
            {
                state = 0;
                xml.writeCharacters("\n");
            } else {
                ++state;
                xml.writeCharacters(" ");
            }
            xml.writeCharacters(AString::number(parcel.m_voxelIndicesIJK[i]));
        }
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void CiftiXMLWriter::writeParcelNodes(QXmlStreamWriter& xml, const CiftiParcelNodesElement& parcelNodes)
{
    int numNodes = (int)parcelNodes.m_nodes.size();
    if (numNodes > 0)//don't write empty elements even if they exist in the tree
    {
        xml.writeStartElement("Nodes");
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(parcelNodes.m_structure));
        xml.writeCharacters(AString::number(parcelNodes.m_nodes[0]));
        for (int i = 1; i < numNodes; ++i)
        {
            xml.writeCharacters(" ");
            xml.writeCharacters(AString::number(parcelNodes.m_nodes[i]));
        }
        xml.writeEndElement();
    }
}

void CiftiXMLWriter::writeVolume(QXmlStreamWriter &xml, const CiftiVolumeElement &volume)
{     
    xml.writeStartElement("Volume");

    QString str("%1,%2,%3");
    xml.writeAttribute("VolumeDimensions", str.arg(QString::number(volume.m_volumeDimensions[0]),QString::number(volume.m_volumeDimensions[1]),QString::number(volume.m_volumeDimensions[2])));
    for(unsigned int i = 0;i<volume.m_transformationMatrixVoxelIndicesIJKtoXYZ.size();i++)
    {
        writeTransformationMatrixVoxelIndicesIJKtoXYZ(xml,volume.m_transformationMatrixVoxelIndicesIJKtoXYZ[i]);
    }

    xml.writeEndElement();
}

void CiftiXMLWriter::writeTransformationMatrixVoxelIndicesIJKtoXYZ(QXmlStreamWriter &xml, const TransformationMatrixVoxelIndicesIJKtoXYZElement &transform)
{     
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");

    QString dataSpaceString, transformedSpaceString, unitsXYZString;
    getDataSpaceString(transform.m_dataSpace, dataSpaceString);
    getDataSpaceString(transform.m_transformedSpace, transformedSpaceString);
    getUnitsXYZString(transform.m_unitsXYZ,unitsXYZString);

    if(dataSpaceString.length() > 0) xml.writeAttribute("DataSpace", dataSpaceString);
    if(transformedSpaceString.length() > 0) xml.writeAttribute("TransformedSpace", transformedSpaceString);
    if(unitsXYZString.length() > 0) xml.writeAttribute("UnitsXYZ", unitsXYZString);

    QString matrixString;
    for(int i = 0;i<12;i++)
    {
        matrixString += QString::number(transform.m_transform[i], 'f', 10) + " ";
    }
    for (int i = 0; i < 3; ++i)//always write 0 0 0 1, ignore the actual last row
    {
        matrixString += QString::number(0.0f, 'f', 10) + " ";
    }
    matrixString += QString::number(1.0f, 'f', 10);
    xml.writeCharacters(matrixString);

    xml.writeEndElement();

}

void CiftiXMLWriter::getModelTypeString(int modelType, QString &modelTypeString)
{
    if(modelType == CIFTI_MODEL_TYPE_SURFACE) modelTypeString = "CIFTI_MODEL_TYPE_SURFACE";
    else if(modelType == CIFTI_MODEL_TYPE_VOXELS) modelTypeString = "CIFTI_MODEL_TYPE_VOXELS";
}

void CiftiXMLWriter::getDataSpaceString(int dataSpace, QString &dataSpaceString)
{
    if(dataSpace == NIFTI_XFORM_UNKNOWN) dataSpaceString = "NIFTI_XFORM_UNKNOWN";
    else if(dataSpace == NIFTI_XFORM_SCANNER_ANAT) dataSpaceString = "NIFTI_XFORM_SCANNER_ANAT";
    else if(dataSpace == NIFTI_XFORM_ALIGNED_ANAT) dataSpaceString = "NIFTI_XFORM_ALIGNED_ANAT";
    else if(dataSpace == NIFTI_XFORM_TALAIRACH) dataSpaceString = "NIFTI_XFORM_TALAIRACH";
    else if(dataSpace == NIFTI_XFORM_MNI_152) dataSpaceString = "NIFTI_XFORM_MNI_152";
}

void CiftiXMLWriter::getUnitsXYZString(int unitsXYZ, QString &unitsXYZString)
{
    if(unitsXYZ == NIFTI_UNITS_MM) unitsXYZString = "NIFTI_UNITS_MM";
    else if(unitsXYZ == NIFTI_UNITS_MICRON) unitsXYZString = "NIFTI_UNITS_MICRON";
}
