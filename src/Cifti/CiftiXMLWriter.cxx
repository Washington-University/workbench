/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "CiftiXMLWriter.h"
#include "iostream"

using namespace caret;

void caret::writeCiftiXML(QXmlStreamWriter &xml, CiftiRootElement &rootElement)
{  
    xml.setAutoFormatting(true);
    xml.writeStartElement("CIFTI");
    if(rootElement.m_version.length() >0) xml.writeAttribute("Version",rootElement.m_version);
    else xml.writeAttribute("Version","1.0");
    xml.writeAttribute("NumberOfMatrices",QString::number(rootElement.m_numberOfMatrices));

    for(unsigned int i = 0;i<rootElement.m_numberOfMatrices;i++)
    {
        writeMatrixElement(xml, rootElement.m_matrices[i]);
    }

    xml.writeEndElement(); //CIFTI

}

void caret::writeMatrixElement(QXmlStreamWriter &xml, CiftiMatrixElement &matrixElement)
{ 
    xml.writeStartElement("Matrix");
    if(matrixElement.m_userMetaData.count() > 0) writeMetaData(xml,matrixElement.m_userMetaData);
    if(matrixElement.m_volume.size() > 0) writeVolume(xml, matrixElement.m_volume[0]);
    if(matrixElement.m_labelTable.size() > 0) writeLabelTable(xml, matrixElement.m_labelTable);

    for(unsigned int i = 0;i<matrixElement.m_matrixIndicesMap.size();i++)
    {
        writeMatrixIndicesMap(xml,matrixElement.m_matrixIndicesMap[i]);
    }

    xml.writeEndElement();//Matrix
}

void caret::writeMetaData(QXmlStreamWriter &xml, QHash<QString, QString> &metaData)
{     
    xml.writeStartElement("MetaData");

    QHash<QString, QString>::Iterator i;

    for (i = metaData.begin(); i != metaData.end(); ++i)
    {
        writeMetaDataElement(xml,i.key(),i.value());
    }
    xml.writeEndElement();
}

void caret::writeMetaDataElement(QXmlStreamWriter &xml, const QString &name, const QString &value)
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

void caret::writeLabelTable(QXmlStreamWriter &xml, std::vector <CiftiLabelElement> &labelElement)
{     
    xml.writeStartElement("LabelTable");

    for(unsigned int i=0;i<labelElement.size();i++)
    {
        writeLabel(xml,labelElement[i]);
    }

    xml.writeEndElement();
}

void caret::writeLabel(QXmlStreamWriter &xml, CiftiLabelElement &label)
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

void caret::writeMatrixIndicesMap(QXmlStreamWriter &xml, CiftiMatrixIndicesMapElement &matrixIndicesMap)
{     
    xml.writeStartElement("MatrixIndicesMap");
    //TODO
    //xml.writeAttribute("AppliesToMatrixDimension", QString::number(matrixIndicesMap.m_appliesToMatrixDimension));
    QString indicesMapToDataType;
    if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS) indicesMapToDataType = "CIFTI_INDEX_TYPE_BRAIN_MODELS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_FIBERS) indicesMapToDataType = "CIFTI_INDEX_TYPE_FIBERS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) indicesMapToDataType = "CIFTI_INDEX_TYPE_PARCELS";
    else if(matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_TIME_POINTS) indicesMapToDataType = "CIFTI_INDEX_TYPE_TIME_POINTS";

    xml.writeAttribute("IndicesMapToDataType",indicesMapToDataType);


    QString timeStepUnits;

    if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_SEC) timeStepUnits = "NIFTI_UNITS_SEC";
    else if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_MSEC) timeStepUnits = "NIFTI_UNITS_MSEC";
    else if(matrixIndicesMap.m_timeStepUnits == NIFTI_UNITS_USEC) timeStepUnits = "NIFTI_UNITS_USEC";

    if(timeStepUnits.length()>0) {
        QString str;
        xml.writeAttribute("TimeStep",str.sprintf("%.1f",matrixIndicesMap.m_timeStep));
        xml.writeAttribute("TimeStepUnits",timeStepUnits);
    }
    if(matrixIndicesMap.m_appliesToMatrixDimension.size())
    {
        int lastElement = matrixIndicesMap.m_appliesToMatrixDimension.size() -1;
        QString appliesToMatrixDimension, str;
        for(int i = 0;i<lastElement;i++)
        {
            appliesToMatrixDimension.append(str.sprintf("%d,",matrixIndicesMap.m_appliesToMatrixDimension[i]));
        }
        appliesToMatrixDimension.append(str.sprintf("%d",matrixIndicesMap.m_appliesToMatrixDimension[lastElement]));
        xml.writeAttribute("AppliesToMatrixDimension", appliesToMatrixDimension);
    }

    for(unsigned int i=0;i<matrixIndicesMap.m_brainModels.size();i++)
    {
        writeBrainModel(xml, matrixIndicesMap.m_brainModels[i]);
    }
    xml.writeEndElement();
}

void caret::writeBrainModel(QXmlStreamWriter &xml, CiftiBrainModelElement &brainModel)
{     
    xml.writeStartElement("BrainModel");

    xml.writeAttribute("IndexOffset", QString::number(brainModel.m_indexOffset));
    xml.writeAttribute("IndexCount", QString::number(brainModel.m_indexCount));
    QString modelType, brainStructure;
    getModelTypeString(brainModel.m_modelType,modelType);
    xml.writeAttribute("ModelType",modelType);
    xml.writeAttribute("BrainStructure", brainModel.m_brainStructure);
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



    std::vector <voxelIndexType> &ind = brainModel.m_voxelIndicesIJK;
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

void caret::writeVolume(QXmlStreamWriter &xml, CiftiVolumeElement &volume)
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

void caret::writeTransformationMatrixVoxelIndicesIJKtoXYZ(QXmlStreamWriter &xml, TransformationMatrixVoxelIndicesIJKtoXYZElement &transform)
{     
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");

    QString dataSpaceString, transformedSpaceString, unitsXYZString;
    getDataSpaceString(transform.m_dataSpace, dataSpaceString);
    getDataSpaceString(transform.m_transformedSpace, transformedSpaceString);
    getUnitsXYZString(transform.m_unitsXYZ,unitsXYZString);

    if(dataSpaceString.length() > 0) xml.writeAttribute("DataSpace", dataSpaceString);
    if(transformedSpaceString.length() > 0) xml.writeAttribute("TransformedSpace", transformedSpaceString);
    if(unitsXYZString.length() > 0) xml.writeAttribute("UnitsXYZ", unitsXYZString);

    QString voxelIndices;
    QString s;
    for(int i = 0;i<15;i++)
    {
        voxelIndices.append(s.sprintf("%.1f ",transform.m_transform[i]));
    }
    voxelIndices.append(s.sprintf("%.1f", transform.m_transform[15]));
    xml.writeCharacters(voxelIndices);

    xml.writeEndElement();

}

void caret::getModelTypeString(int modelType, QString &modelTypeString)
{
    if(modelType == CIFTI_MODEL_TYPE_SURFACE) modelTypeString = "CIFTI_MODEL_TYPE_SURFACE";
    else if(modelType == CIFTI_MODEL_TYPE_VOXELS) modelTypeString = "CIFTI_MODEL_TYPE_VOXELS";
}

void caret::getDataSpaceString(int dataSpace, QString &dataSpaceString)
{
    if(dataSpace == NIFTI_XFORM_UNKNOWN) dataSpaceString = "NIFTI_XFORM_UNKNOWN";
    else if(dataSpace == NIFTI_XFORM_SCANNER_ANAT) dataSpaceString = "NIFTI_XFORM_SCANNER_ANAT";
    else if(dataSpace == NIFTI_XFORM_ALIGNED_ANAT) dataSpaceString = "NIFTI_XFORM_ALIGNED_ANAT";
    else if(dataSpace == NIFTI_XFORM_TALAIRACH) dataSpaceString = "NIFTI_XFORM_TALAIRACH";
    else if(dataSpace == NIFTI_XFORM_MNI_152) dataSpaceString = "NIFTI_XFORM_MNI_152";
}

void caret::getUnitsXYZString(int unitsXYZ, QString &unitsXYZString)
{
    if(unitsXYZ == NIFTI_UNITS_MM) unitsXYZString = "NIFTI_UNITS_MM";
    else if(unitsXYZ == NIFTI_UNITS_MICRON) unitsXYZString = "NIFTI_UNITS_MICRON";
}
