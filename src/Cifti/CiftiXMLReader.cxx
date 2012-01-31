#include <stdio.h>
#include <QtCore>
#include "CiftiXMLElements.h"
#include "iostream"
#include "CiftiXMLReader.h"

using namespace caret;

void caret::parseCiftiXML(QXmlStreamReader &xml, CiftiRootElement &rootElement)
{
    while (!xml.atEnd()  && !xml.hasError()) {
        xml.readNext();
        QString test = xml.name().toString();
        if(xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if(elementName == "CIFTI") {
                QXmlStreamAttributes attributes = xml.attributes();
                if(attributes.hasAttribute("Version")) rootElement.m_version = attributes.value("Version").toString();
                else xml.raiseError("Cifti XML Header missing Version String.");
                if(attributes.hasAttribute("NumberOfMatrices")) rootElement.m_numberOfMatrices = attributes.value("NumberOfMatrices").toString().toULong();
                else xml.raiseError("Cifti XML Header missing number of matrices.");
            }
            else if(elementName == "Matrix") {
                rootElement.m_matrices.push_back(CiftiMatrixElement());
                parseMatrixElement(xml,rootElement.m_matrices.back());
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }

    if(xml.hasError())
    {
        std::cout << "XML error: " << xml.errorString().toAscii().data() << std::endl;
    }
    else if(xml.atEnd())
    {
        ;//std::cout << "Reached end, done" << std::endl;
    }
}

void caret::parseMatrixElement(QXmlStreamReader &xml, CiftiMatrixElement &matrixElement)
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
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }

    QString test2=xml.name().toString();
    //check end element
    if(!xml.hasError()) if(!xml.isEndElement() || (xml.name().toString() != "Matrix"))
        xml.raiseError("Matrix end tag not found.");
}

void caret::parseMetaData(QXmlStreamReader &xml, QHash<QString, QString> &userMetaData)
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
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }
    //check for end element
    if(!xml.isEndElement() || (xml.name().toString() != "MetaData"))
        xml.raiseError("MetaData end tag not found.");
}

void caret::parseMetaDataElement(QXmlStreamReader &xml, QHash<QString,QString> &userMetaData)
{
    QString name;
    QString value;
    QString test;
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
                xml.readNext();
                if(!xml.isEndElement())
                    xml.raiseError("End element for meta data value tag not found.");
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }

    }
    userMetaData.insert(name,value);
    if(!xml.isEndElement() || (xml.name().toString() != "MD"))
        xml.raiseError("End element for MD tag not found");
}

void caret::parseLabelTable(QXmlStreamReader &xml, std::vector<CiftiLabelElement> &labelTable)
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
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }
    //check end element
    if(!xml.isEndElement() || (xml.name().toString() != "LabelTable"))
    {
        xml.raiseError("End element for label table not found.");
    }

}

void caret::parseLabel(QXmlStreamReader &xml, CiftiLabelElement &label)
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

void caret::parseMatrixIndicesMap(QXmlStreamReader &xml, CiftiMatrixIndicesMapElement &matrixIndicesMap)
{
    QXmlStreamAttributes attributes = xml.attributes();

    //get attribute values
    if(attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');

        for(int i = 0;i<values.size();i++)
            matrixIndicesMap.m_appliesToMatrixDimension.push_back(values.at(i).toInt());
    }
    else xml.raiseError("MatrixIndicesMap does not contain AppliesToMatrixDimension value\n");

    if(attributes.hasAttribute("IndicesMapToDataType")) {
        QString indicesMapToDataType = attributes.value("IndicesMapToDataType").toString();
        if(indicesMapToDataType == "CIFTI_INDEX_TYPE_BRAIN_MODELS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
        else if(indicesMapToDataType == "CIFTI_TYPE_FIBERS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_FIBERS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_PARCELS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_PARCELS;
        else if(indicesMapToDataType == "CIFTI_INDEX_TYPE_TIME_POINTS") matrixIndicesMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
        else xml.raiseError("Error, unrecognized value for BrainModel, indicesMapToDataType.");
    }
    else xml.raiseError("MatrixIndicesMap does not contain IndicesMapToDataType value\n");

    if(attributes.hasAttribute("TimeStep")) matrixIndicesMap.m_timeStep = attributes.value("TimeStep").toString().toFloat();
    //else xml.raiseError("MatrixIndicesMap does not contain timeStep Value.");

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
                matrixIndicesMap.m_brainModels.push_back(CiftiBrainModelElement());
                parseBrainModel(xml,matrixIndicesMap.m_brainModels.back());
                xml.readNext();//read next element after brainmodel end element
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }
}

void caret::parseBrainModel(QXmlStreamReader &xml, CiftiBrainModelElement &brainModel)
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

                QStringList list = nodeIndices.split(QRegExp("\\D+"),QString::SkipEmptyParts);
                for(int i = 0;i<list.count();i++)
                {
                    brainModel.m_nodeIndices.push_back(list[i].toULongLong());
                }
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

                QStringList list = voxelIndicesIJK.split(QRegExp("\\D+"),QString::SkipEmptyParts);
                if(list.count()%3) std::cout << voxelIndicesIJK.toAscii().data() << std::endl << list[list.count()-3].toULongLong() << list[list.count()-2].toULongLong() << list[list.count()-1].toULongLong() << std::endl;
                for(int i = 0;i<list.count();i++)
                {
                    brainModel.m_voxelIndicesIJK.push_back(list[i].toULongLong());
                }
                //get end element
                xml.readNext();
                if(!xml.isEndElement()|| (xml.name().toString() != "VoxelIndicesIJK"))
                {
                    xml.raiseError("End element for VoxelIndicesIJK not found.");
                }
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }

    //get end element for BrainModel
    while(!(xml.isEndElement() && (xml.name().toString() == "BrainModel"))&& !xml.hasError()) xml.readNext();
    if(!xml.isEndElement() || (xml.name().toString() != "BrainModel"))
    {
        xml.raiseError("End element for brain Model not found.");
    }
}

void caret::parseVolume(QXmlStreamReader &xml, CiftiVolumeElement &volume)
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
            }
            else std::cout << "unknown element: " << elementName.toAscii().data() << std::endl;
        }
    }

    //check end element for Volume
    if(!xml.isEndElement())
    {
        xml.raiseError("End element for Volume not found.");
    }
}

void caret::parseTransformationMatrixVoxelIndicesIJKtoXYZ(QXmlStreamReader &xml, TransformationMatrixVoxelIndicesIJKtoXYZElement &transform)
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
        else xml.raiseError("Volume contains unknown or unsupported data space.");
    }
    else xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain dataSpace.");

    if(attributes.hasAttribute("TransformedSpace"))
    {
        QString transformedSpace = attributes.value("TransformedSpace").toString();

        if(transformedSpace == "NIFTI_XFORM_UNKNOWN") transform.m_transformedSpace = NIFTI_XFORM_UNKNOWN;
        else if(transformedSpace == "NIFTI_XFORM_SCANNER_ANAT") transform.m_transformedSpace = NIFTI_XFORM_SCANNER_ANAT;
        else if(transformedSpace == "NIFTI_XFORM_ALIGNED_ANAT") transform.m_transformedSpace = NIFTI_XFORM_ALIGNED_ANAT;
        else if(transformedSpace == "NIFTI_XFORM_TALAIRACH") transform.m_transformedSpace = NIFTI_XFORM_TALAIRACH;
        else if(transformedSpace == "NIFTI_XFORM_MNI_152") transform.m_transformedSpace = NIFTI_XFORM_MNI_152;
        else xml.raiseError("Volume contains unknown or unsupported transformed space.");
    }
    else xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain transformedSpace.");

    if(attributes.hasAttribute("UnitsXYZ"))
    {
        QString unitsXYZ = attributes.value("UnitsXYZ").toString();

        if(unitsXYZ == "NIFTI_UNITS_MM") transform.m_unitsXYZ = NIFTI_UNITS_MM;
        else if(unitsXYZ == "NIFTI_UNITS_MICRON") transform.m_unitsXYZ = NIFTI_UNITS_MICRON;
        else xml.raiseError("Volume contains unknown or unsupported spatial XYZ coordinates.");
    }
    else xml.raiseError("TransformationMatrixVoxelIndicesIJKtoXYZ does not contain UnitsXYZ.");

    xml.readNext();
    if(xml.tokenType() != QXmlStreamReader::Characters) {
        xml.raiseError("Error reading Transformation matrix.");
    }
    QString voxelIndicesString = xml.text().toString();
    QStringList voxelIndices = voxelIndicesString.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    for(int i = 0;i<16;i++)
        transform.m_transform[i] = voxelIndices.at(i).toFloat();

    //get end element for TransformationMatrixVoxelIndicesIJKtoXYZ
    while(!xml.isEndElement() && !xml.hasError())  xml.readNext();
    if(!xml.isEndElement())
    {
        xml.raiseError("End element for TransformationMatrixVoxelIndicesIJKtoXYZ not found.");
    }
}
