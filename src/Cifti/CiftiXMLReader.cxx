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

#include <stdio.h>
#include <QtCore>
#include "CaretLogger.h"
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "DataFileException.h"
#include "GiftiLabelTable.h"

using namespace caret;
using namespace std;

void CiftiXMLReader::parseCiftiXML(QXmlStreamReader &xml, CiftiRootElement &rootElement)
{
    while (!xml.atEnd()  && !xml.hasError()) {
        xml.readNext();
        QString test = xml.name().toString();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "CIFTI") {
                QXmlStreamAttributes attributes = xml.attributes();
                if(attributes.hasAttribute("Version"))
                {
                    rootElement.m_version = CiftiVersion(attributes.value("Version").toString());
                    m_readingVersion = rootElement.m_version;
                    if (m_readingVersion != CiftiVersion(1, 0)) xml.raiseError("cannot read version " + m_readingVersion.toString() + " with old XML code");
                } else xml.raiseError("Cifti XML Header missing Version String.");
                if(attributes.hasAttribute("NumberOfMatrices")) rootElement.m_numberOfMatrices = attributes.value("NumberOfMatrices").toString().toULong();
                else xml.raiseError("Cifti XML Header missing number of matrices.");
            }
            else if(elementName == "Matrix") {
                rootElement.m_matrices.push_back(CiftiMatrixElement());
                parseMatrixElement(xml,rootElement.m_matrices.back());
            }
            else xml.raiseError("unknown element in Cifti: " + elementName);
        }
    }
    if(xml.hasError())
    {
        throw DataFileException("XML error: " + xml.errorString());
    }
    else if(!xml.atEnd())
    {
        CaretLogWarning("Finished parsing Cifti XML without error, but not at end of XML");
    }
    if (rootElement.m_numberOfMatrices != rootElement.m_matrices.size())
    {
        CaretLogWarning("NumberOfMatrices does not match number of <Matrix> elements");
    }
}

void CiftiXMLReader::parseMatrixElement(QXmlStreamReader &xml, CiftiMatrixElement &matrixElement)
{
    QString test = xml.name().toString();

    while (!(xml.isEndElement() && (xml.name().toString() == "Matrix")) && !xml.hasError()) {// && xml.name() == "Matrix") {
        xml.readNext();
        QString test2 = xml.name().toString();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "MetaData")
            {
                parseMetaData(xml,matrixElement.m_userMetaData);
            }
            else if(elementName == "LabelTable") {
                parseLabelTable(xml,matrixElement.m_labelTable);
            }
            else if(elementName == "MatrixIndicesMap")
            {
                matrixElement.m_matrixIndicesMap.push_back(CiftiMatrixIndicesMapElement());
                parseMatrixIndicesMap(xml,matrixElement.m_matrixIndicesMap.back());
            }
            else if(elementName == "Volume")
            {
                matrixElement.m_volume.push_back(CiftiVolumeElement());
                parseVolume(xml,matrixElement.m_volume.back());
                if (xml.hasError()) return;
            }
            else xml.raiseError("unknown element in Matrix: " + elementName);
        }
    }

    QString test2=xml.name().toString();
    //check end element
    if(!xml.hasError()) if(!xml.isEndElement() || (xml.name().toString() != "Matrix"))
        xml.raiseError("Matrix end tag not found.");
}

void CiftiXMLReader::parseMetaData(QXmlStreamReader &xml, map<AString, AString> &userMetaData)
{
    while (!(xml.isEndElement()  && (xml.name().toString() == "MetaData")) && !xml.hasError()) {// && xml.name() == "MetaData") {
        xml.readNext();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "MD")
            {
                parseMetaDataElement(xml,userMetaData);
            }
            else xml.raiseError("unknown element in MetaData: " + elementName);
        }
    }
    //check for end element
    if(!xml.isEndElement() || (xml.name().toString() != "MetaData"))
        xml.raiseError("MetaData end tag not found.");
}

void CiftiXMLReader::parseMetaDataElement(QXmlStreamReader &xml, map<AString, AString> &userMetaData)
{
    QString name;
    QString value;
    QString test;
    bool haveName = false, haveValue = false;
    while (!(xml.isEndElement() && (xml.name().toString() == "MD")) && !xml.hasError()) {
        test = xml.name().toString();
        xml.readNext();

        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "Name")
            {
                xml.readNext();
                if(xml.tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                name = xml.text().toString();
                haveName = true;
                xml.readNext();
                if(!xml.isEndElement())
                    xml.raiseError("End element for meta data name tag not found.");
            }
            else if(elementName == "Value")
            {
                xml.readNext();
                if(xml.tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                value = xml.text().toString();
                haveValue = true;
                xml.readNext();
                if(!xml.isEndElement())
                    xml.raiseError("End element for meta data value tag not found.");
            }
            else xml.raiseError("unknown element in MD: " + elementName);
        }

    }
    if (!haveName || !haveValue)
        xml.raiseError("MD element is missing name or value");
    userMetaData[name] = value;
    if(!xml.isEndElement() || (xml.name().toString() != "MD"))
        xml.raiseError("End element for MD tag not found");
}

void CiftiXMLReader::parseLabelTable(QXmlStreamReader &xml, std::vector<CiftiLabelElement> &labelTable)
{
    while (!(xml.isEndElement() && (xml.name().toString() == "LabelTable"))&& !xml.hasError()) {// && xml.name() == "Matrix") {
        xml.readNext();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "Label")
            {
                labelTable.push_back(CiftiLabelElement());
                parseLabel(xml,labelTable.back());
            }
            else xml.raiseError("unknown element in LabelTable: " + elementName);
        }
    }
    //check end element
    if(!xml.isEndElement() || (xml.name().toString() != "LabelTable"))
    {
        xml.raiseError("End element for label table not found.");
    }

}

void CiftiXMLReader::parseLabel(QXmlStreamReader &xml, CiftiLabelElement &label)
{
    if(!(xml.name().toString() == "Label")) xml.raiseError("Error parsing Label\n");
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("Key")) label.m_key = attributes.value("Key").toString().toULongLong();
    else xml.raiseError("Label does not contain Key value\n");

    if(attributes.hasAttribute("Red")) label.m_red = attributes.value("Red").toString().toFloat();
    else xml.raiseError("Label does not contain Red value\n");

    if(attributes.hasAttribute("Green")) label.m_green = attributes.value("Green").toString().toFloat();
    else xml.raiseError("Label does not contain Green value\n");

    if(attributes.hasAttribute("Blue")) label.m_blue = attributes.value("Blue").toString().toFloat();
    else xml.raiseError("Label does not contain Blue value\n");

    if(attributes.hasAttribute("Alpha")) label.m_alpha = attributes.value("Alpha").toString().toFloat();
    else xml.raiseError("Label does not contain Alpha value\n");

    if(attributes.hasAttribute("X")) label.m_x = attributes.value("X").toString().toFloat();
    else xml.raiseError("Label does not contain X value\n");

    if(attributes.hasAttribute("Y")) label.m_y = attributes.value("Y").toString().toFloat();
    else xml.raiseError("Label does not contain Y value\n");

    if(attributes.hasAttribute("Z")) label.m_z = attributes.value("Z").toString().toFloat();
    else xml.raiseError("Label does not contain Z value\n");

    //get Label Text
    xml.readNext();
    if(xml.tokenType() != QXmlStreamReader::Characters) {
        return;
    }
    label.m_text = xml.text().toString();

    //get end element
    xml.readNext();
    if(!xml.isEndElement())
    {
        xml.raiseError("End element for label not found.");
    }
}

void CiftiXMLReader::parseMatrixIndicesMap(QXmlStreamReader &xml, CiftiMatrixIndicesMapElement &matrixIndicesMap)
{
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');
        bool ok = false;
        for(int i = 0;i<values.size();i++)
        {
            int parsed = values.at(i).toInt(&ok);
            if (!ok || parsed < 0) xml.raiseError("bad value in AppliesToMatrixDimension list: " + values[i]);
            if (parsed < 2 && m_readingVersion.hasReversedFirstDims()) parsed = 1 - parsed;//in other words, 0 becomes 1 and 1 becomes 0
            matrixIndicesMap.m_appliesToMatrixDimension.push_back(parsed);
        }
    }
    else xml.raiseError("MatrixIndicesMap does not contain AppliesToMatrixDimension value\n");

    if(attributes.hasAttribute("IndicesMapToDataType")) {
        QString indicesMapToDataType = attributes.value("IndicesMapToDataType").toString();
        if(indicesMapToDataType == "CIFTI_INDEX_TYPE_BRAIN_MODELS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
        else if(indicesMapToDataType == "CIFTI_TYPE_FIBERS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_FIBERS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_PARCELS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_PARCELS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_TIME_POINTS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_SCALARS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_SCALARS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_LABELS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_LABELS;
        else xml.raiseError("Error, unrecognized value for BrainModel, indicesMapToDataType.");
    }
    else xml.raiseError("MatrixIndicesMap does not contain IndicesMapToDataType value\n");
    
    bool needLabels = (matrixIndicesMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_LABELS);

    bool ok = false;
    if(attributes.hasAttribute("TimeStep"))
    {
        matrixIndicesMap.m_timeStep = attributes.value("TimeStep").toString().toFloat(&ok);
        if (!ok) xml.raiseError("TimeStep value is not numeric");
    }
    //else xml.raiseError("MatrixIndicesMap does not contain timeStep Value.");
    
    if (attributes.hasAttribute("TimeStart"))
    {
        matrixIndicesMap.m_timeStart = attributes.value("TimeStart").toString().toFloat(&ok);
        matrixIndicesMap.m_hasTimeStart = true;
        if (!ok) xml.raiseError("TimeStart value is not numeric");
    }

    if(attributes.hasAttribute("TimeStepUnits"))
    {
        QString timeStepUnits = attributes.value("TimeStepUnits").toString();
        if(timeStepUnits == "NIFTI_UNITS_SEC")
        {
            matrixIndicesMap.m_timeStepUnits = NIFTI_UNITS_SEC;
        }
        else if(timeStepUnits == "NIFTI_UNITS_MSEC")
        {
            matrixIndicesMap.m_timeStepUnits = NIFTI_UNITS_MSEC;
        }
        else if(timeStepUnits == "NIFTI_UNITS_USEC")
        {
            matrixIndicesMap.m_timeStepUnits = NIFTI_UNITS_USEC;
        }
    }

    while (!xml.isEndElement()  && !xml.hasError()) {
        xml.readNext();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "BrainModel")
            {
                if (matrixIndicesMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
                {
                    xml.raiseError("BrainModel element found in incorrect mapping type");
                    break;
                }
                matrixIndicesMap.m_brainModels.push_back(CiftiBrainModelElement());
                parseBrainModel(xml,matrixIndicesMap.m_brainModels.back());
                xml.readNext();//read next element after brainmodel end element
            } else if (elementName == "NamedMap") {
                if (matrixIndicesMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS &&
                    matrixIndicesMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
                {
                    xml.raiseError("NamedMap element found in incorrect mapping type");
                    break;
                }
                matrixIndicesMap.m_namedMaps.push_back(CiftiNamedMapElement());
                parseNamedMap(xml, matrixIndicesMap.m_namedMaps.back(), needLabels);
                xml.readNext();
            } else if (elementName == "Surface") {
                if (matrixIndicesMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS)
                {
                    xml.raiseError("Surface element found in incorrect maping type");
                    break;
                }
                CiftiParcelSurfaceElement tempParcelSurf;
                QXmlStreamAttributes attributes = xml.attributes();
                bool ok = false;
                if (attributes.hasAttribute("BrainStructure"))
                {
                    tempParcelSurf.m_structure = StructureEnum::fromCiftiName(attributes.value("BrainStructure").toString(), &ok);
                    if (!ok)
                    {
                        xml.raiseError("Unrecognized structure name: " + attributes.value("BrainStructure").toString());
                        break;
                    }
                } else {
                    xml.raiseError("Surface element missing required attribute 'BrainStructure'");
                }
                if (attributes.hasAttribute("SurfaceNumberOfNodes"))
                {
                    tempParcelSurf.m_numNodes = attributes.value("SurfaceNumberOfNodes").toString().toLongLong(&ok);
                    if (!ok)
                    {
                        xml.raiseError("Noninteger in 'SurfaceNumberOfNodes': " + attributes.value("SurfaceNumberOfNodes").toString());
                        break;
                    }
                } else {
                    xml.raiseError("Surface element missing required attribute 'SurfaceNumberOfNodes'");
                }
                matrixIndicesMap.m_parcelSurfaces.push_back(tempParcelSurf);
                while (!xml.atEnd() && xml.readNext() != QXmlStreamReader::EndElement);
                if (xml.atEnd())
                {
                    xml.raiseError("End element for Surface not found");
                }
                if (xml.name() != "Surface")
                {
                    xml.raiseError("Found incorrect end element name: " + xml.name().toString());
                }
                xml.readNext();
            } else if (elementName == "Parcel") {
                if (matrixIndicesMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS)
                {
                    xml.raiseError("Parcel element found in incorrect maping type");
                    break;
                }
                matrixIndicesMap.m_parcels.push_back(CiftiParcelElement());
                parseParcel(xml, matrixIndicesMap.m_parcels.back());
                xml.readNext();
            } else xml.raiseError("unknown element in MatrixIndicesMap: " + elementName);
        }
    }
}

void CiftiXMLReader::parseBrainModel(QXmlStreamReader &xml, CiftiBrainModelElement &brainModel)
{
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("IndexOffset")) brainModel.m_indexOffset = attributes.value("IndexOffset").toString().toULongLong();
    else xml.raiseError("BrainModel does not contain required IndexOffset attribute\n");

    if(attributes.hasAttribute("IndexCount")) brainModel.m_indexCount = attributes.value("IndexCount").toString().toULongLong();
    else xml.raiseError("BrainModel does not contain required IndexCount attribute\n");

    if(attributes.hasAttribute("ModelType"))
    {
        QString modelType = attributes.value("ModelType").toString();
        if(modelType == "CIFTI_MODEL_TYPE_SURFACE")
        {
            brainModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
        }
        else if(modelType == "CIFTI_MODEL_TYPE_VOXELS")
        {
            brainModel.m_modelType = CIFTI_MODEL_TYPE_VOXELS;
        }
        else xml.raiseError("BrainModel contains unrecognized model type");
    }
    else xml.raiseError("BrainModel does not contain required ModelType attribute\n");

    if(attributes.hasAttribute("BrainStructure"))
    {
        QString brainStructure = attributes.value("BrainStructure").toString();
        bool ok = false;
        brainModel.m_brainStructure = StructureEnum::fromCiftiName(attributes.value("BrainStructure").toString(), &ok);
        if (!ok)
        {
            xml.raiseError("BrainStructure contains unrecognized value \"" + attributes.value("BrainStructure").toString() + "\"");
        }
    }

    if(attributes.hasAttribute("SurfaceNumberOfNodes")) brainModel.m_surfaceNumberOfNodes = attributes.value("SurfaceNumberOfNodes").toString().toULongLong();
    else brainModel.m_surfaceNumberOfNodes = 0;
    //else xml.raiseError("BrainModel does not contain require SurfaceNumberOfNodes attribute\n");

    while (!(xml.isEndElement() && (xml.name().toString() == "BrainModel")) && !xml.hasError()) {
        xml.readNext();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "NodeIndices")
            {
                //get node Indices
                xml.readNext();
                if(xml.tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                QString nodeIndices = xml.text().toString();

                QStringList list = nodeIndices.split(QRegExp("\\D+"),Qt::SkipEmptyParts);
                bool ok = true;
                for(int i = 0;i<list.count();i++)
                {
                    brainModel.m_nodeIndices.push_back(list[i].toULongLong(&ok));
                    if (!ok)
                    {
                        xml.raiseError("count not parse '" + list[i] + "' as node index");
                        break;
                    }
                }
                if (!ok) break;
                //get end element
                xml.readNext();
                if(!xml.isEndElement())
                {
                    xml.raiseError("End element for NodeIndices not found.");
                }
            }
            else if(elementName == "VoxelIndicesIJK")
            {
                //get voxel Indices IJK
                xml.readNext();
                if(xml.tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                QString voxelIndicesIJK = xml.text().toString();

                QStringList list = voxelIndicesIJK.split(QRegExp("\\D+"),Qt::SkipEmptyParts);
                if(list.count()%3) xml.raiseError("VoxelIndicesIJK has an incomplete triplet");
                bool ok = true;
                for(int i = 0;i<list.count();i++)
                {
                    brainModel.m_voxelIndicesIJK.push_back(list[i].toULongLong(&ok));
                    if (!ok)
                    {
                        xml.raiseError("count not parse '" + list[i] + "' as voxel index");
                        break;
                    }
                }
                if (!ok) break;
                //get end element
                xml.readNext();
                if(!xml.isEndElement()|| (xml.name().toString() != "VoxelIndicesIJK"))
                {
                    xml.raiseError("End element for VoxelIndicesIJK not found.");
                }
            }
            else xml.raiseError("unknown element in BrainModel: " + elementName);
        }
    }

    //get end element for BrainModel
    while(!(xml.isEndElement() && (xml.name().toString() == "BrainModel"))&& !xml.hasError()) xml.readNext();
    if(!xml.isEndElement() || (xml.name().toString() != "BrainModel"))
    {
        xml.raiseError("End element for brain Model not found.");
    }
}

void CiftiXMLReader::parseNamedMap(QXmlStreamReader& xml, CiftiNamedMapElement& namedMap, const bool needLabels)
{
    bool haveName = false, haveLabelTable = false, haveMetaData = false;;
    xml.readNext();
    while (!xml.hasError() && (!xml.isEndElement() || xml.name() != "NamedMap"))
    {
        if (xml.isStartElement())
        {
            if (xml.name() == "MapName")
            {
                if (haveName)
                {
                    xml.raiseError("MapName specified more than once in NamedMap");
                    break;
                }
                namedMap.m_mapName = xml.readElementText();
                haveName = true;
            } else if (xml.name() == "LabelTable") {
                if (haveLabelTable)
                {
                    xml.raiseError("LabelTable specified more than once in NamedMap");
                    break;
                }
                namedMap.m_labelTable.grabNew(new GiftiLabelTable());
                namedMap.m_labelTable->readFromQXmlStreamReader(xml);//we need to do something to read through the label table, so give it to the parser always
                if (!needLabels)//if it shouldn't exist, drop it
                {
                    CaretLogWarning("found label table in a scalar map, discarding");
                    namedMap.m_labelTable.grabNew(NULL);
                }
                haveLabelTable = true;
            } else if (xml.name() == "MetaData") {
                if (haveMetaData)
                {
                    xml.raiseError("MetaData specified more than once in NamedMap");
                    break;
                }
                parseMetaData(xml, namedMap.m_mapMetaData);
                haveMetaData = true;
            } else {
                xml.raiseError("unknown element in NamedMap: " + xml.name().toString());
                break;
            }
        }
        xml.readNext();
    }
    if (!xml.hasError() && (!haveName))
    {
        xml.raiseError("NamedMap element is missing MapName element");
    }
    if (!xml.hasError() && !haveLabelTable && needLabels)
    {
        xml.raiseError("NamedMap element is missing LabelTable element while type is CIFTI_INDEX_TYPE_LABELS");
    }
    if (!xml.hasError() && (!xml.isEndElement() || xml.name() != "NamedMap"))
    {
        xml.raiseError("unexpected element in NamedMap: " + xml.name().toString());
    }
}

void CiftiXMLReader::parseParcel(QXmlStreamReader& xml, CiftiParcelElement& parcel)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("Name"))
    {
        parcel.m_parcelName = attributes.value("Name").toString();
    } else {
        xml.raiseError("Required attribute 'Name' missing from Parcel");
    }
    xml.readNext();
    while (!xml.hasError() && !(xml.isEndElement() && xml.name() == "Parcel"))
    {
        if (xml.isStartElement())
        {
            if (xml.name() == "Nodes")
            {
                parcel.m_nodeElements.push_back(CiftiParcelNodesElement());
                parseParcelNodes(xml, parcel.m_nodeElements.back());
            } else if (xml.name() == "VoxelIndicesIJK") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters)
                {
                    QString voxelIndicesIJK = xml.text().toString();
                    QStringList list = voxelIndicesIJK.split(QRegExp("\\D+"),Qt::SkipEmptyParts);
                    if(list.count()%3) xml.raiseError("VoxelIndicesIJK has an incomplete triplet");
                    bool ok = true;
                    for(int i = 0;i<list.count();i++)
                    {
                        parcel.m_voxelIndicesIJK.push_back(list[i].toULongLong(&ok));
                        if (!ok)
                        {
                            xml.raiseError("count not parse '" + list[i] + "' as voxel index");
                            break;
                        }
                    }
                    xml.readNext();
                    if (!xml.isEndElement() || xml.name() != "VoxelIndicesIJK")
                    {
                        xml.raiseError("found something other than characters inside VoxelIndicesIJK");
                    } else {
                        xml.readNext();
                    }
                } else {
                    xml.raiseError("Error parsing VoxelIndicesIJK element");
                }
            } else {
                xml.raiseError("unknown element in Parcel: " + xml.name().toString());
            }
        }
        xml.readNext();
    }
}

void CiftiXMLReader::parseParcelNodes(QXmlStreamReader& xml, CiftiParcelNodesElement& parcelNodes)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("BrainStructure"))
    {
        bool ok = false;
        parcelNodes.m_structure = StructureEnum::fromCiftiName(attributes.value("BrainStructure").toString(), &ok);
        if (!ok)
        {
            xml.raiseError("Unrecognized BrainStructure in Nodes element: " + attributes.value("BrainStructure").toString());
        }
    } else {
        xml.raiseError("Required attribute 'BrainStructure' missing from Nodes");
    }
    xml.readNext();
    if (xml.isCharacters())
    {
        QString nodeIndices = xml.text().toString();
        QStringList list = nodeIndices.split(QRegExp("\\D+"),Qt::SkipEmptyParts);
        bool ok = true;
        for(int i = 0;i<list.count();i++)
        {
            parcelNodes.m_nodes.push_back(list[i].toULongLong(&ok));
            if (!ok)
            {
                xml.raiseError("count not parse '" + list[i] + "' as node index");
                break;
            }
        }
        xml.readNext();
    } else {
        xml.raiseError("Error parsing Nodes element");
    }
}

void CiftiXMLReader::parseVolume(QXmlStreamReader &xml, CiftiVolumeElement &volume)
{
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("VolumeDimensions"))
    {
        QStringList list = attributes.value("VolumeDimensions").toString().split(',');
        for(int i=0;i<3;i++)
        {
            volume.m_volumeDimensions[i]=list[i].toUInt();
        }
    }
    else xml.raiseError("Volume does not contain required VolumeDimensions attribute\n");

    while (!(xml.isEndElement() && (xml.name().toString() == "Volume"))  && !xml.hasError()) {
        xml.readNext();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "TransformationMatrixVoxelIndicesIJKtoXYZ")
            {
                volume.m_transformationMatrixVoxelIndicesIJKtoXYZ.push_back(TransformationMatrixVoxelIndicesIJKtoXYZElement());
                parseTransformationMatrixVoxelIndicesIJKtoXYZ(xml,volume.m_transformationMatrixVoxelIndicesIJKtoXYZ.back());
                if (xml.hasError()) return;
            }
            else xml.raiseError("unknown element in Volume: " + elementName);
        }
    }

    //check end element for Volume
    if(!xml.isEndElement())
    {
        xml.raiseError("End element for Volume not found.");
    }
}

void CiftiXMLReader::parseTransformationMatrixVoxelIndicesIJKtoXYZ(QXmlStreamReader &xml, TransformationMatrixVoxelIndicesIJKtoXYZElement &transform)
{
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("DataSpace"))
    {
        QString dataSpace = attributes.value("DataSpace").toString();

        if(dataSpace == "NIFTI_XFORM_UNKNOWN") transform.m_dataSpace = NIFTI_XFORM_UNKNOWN;
        else if(dataSpace == "NIFTI_XFORM_SCANNER_ANAT") transform.m_dataSpace = NIFTI_XFORM_SCANNER_ANAT;
        else if(dataSpace == "NIFTI_XFORM_ALIGNED_ANAT") transform.m_dataSpace = NIFTI_XFORM_ALIGNED_ANAT;
        else if(dataSpace == "NIFTI_XFORM_TALAIRACH") transform.m_dataSpace = NIFTI_XFORM_TALAIRACH;
        else if(dataSpace == "NIFTI_XFORM_MNI_152") transform.m_dataSpace = NIFTI_XFORM_MNI_152;
        else { xml.raiseError("Volume contains unknown or unsupported data space."); return; }
    }
    else { xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain dataSpace."); return; }

    if(attributes.hasAttribute("TransformedSpace"))
    {
        QString transformedSpace = attributes.value("TransformedSpace").toString();

        if(transformedSpace == "NIFTI_XFORM_UNKNOWN") transform.m_transformedSpace = NIFTI_XFORM_UNKNOWN;
        else if(transformedSpace == "NIFTI_XFORM_SCANNER_ANAT") transform.m_transformedSpace = NIFTI_XFORM_SCANNER_ANAT;
        else if(transformedSpace == "NIFTI_XFORM_ALIGNED_ANAT") transform.m_transformedSpace = NIFTI_XFORM_ALIGNED_ANAT;
        else if(transformedSpace == "NIFTI_XFORM_TALAIRACH") transform.m_transformedSpace = NIFTI_XFORM_TALAIRACH;
        else if(transformedSpace == "NIFTI_XFORM_MNI_152") transform.m_transformedSpace = NIFTI_XFORM_MNI_152;
        else { xml.raiseError("Volume contains unknown or unsupported transformed space."); return; }
    }
    else { xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain transformedSpace."); return; }

    if(attributes.hasAttribute("UnitsXYZ"))
    {
        QString unitsXYZ = attributes.value("UnitsXYZ").toString();

        if(unitsXYZ == "NIFTI_UNITS_MM") transform.m_unitsXYZ = NIFTI_UNITS_MM;
        else if(unitsXYZ == "NIFTI_UNITS_MICRON") transform.m_unitsXYZ = NIFTI_UNITS_MICRON;
        else { xml.raiseError("Volume contains unknown or unsupported spatial XYZ coordinates."); return; }
    }
    else { xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain UnitsXYZ."); return; }

    xml.readNext();
    if(xml.tokenType() != QXmlStreamReader::Characters) {
        xml.raiseError("Error reading Transformation matrix.");
        return;
    }
    QString voxelIndicesString = xml.text().toString();
    QStringList voxelIndices = voxelIndicesString.split(QRegExp("\\s+"),Qt::SkipEmptyParts);
    if (voxelIndices.size() == 16)
    {
        for(int i = 0;i<16;i++)
            transform.m_transform[i] = voxelIndices.at(i).toFloat();
    } else {
        xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain 16 values");
        return;
    }

    //get end element for TransformationMatrixVoxelIndicesIJKtoXYZ
    while(!xml.isEndElement() && !xml.hasError())  xml.readNext();
    if(!xml.isEndElement())
    {
        xml.raiseError("End element for TransformationMatrixVoxelIndicesIJKtoXYZ not found.");
        return;
    }
}
