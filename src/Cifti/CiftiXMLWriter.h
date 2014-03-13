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

#ifndef __CIFTI_XML_WRITER_H__
#define __CIFTI_XML_WRITER_H__
#include <QtCore>
#include "CiftiXMLElements.h"

namespace caret {

    class CiftiXMLWriter
    {
        CiftiVersion m_writingVersion;
        void writeMatrixElement(QXmlStreamWriter &xml, const CiftiMatrixElement &matrixElement);
        void writeMetaData(QXmlStreamWriter &xml, const std::map<AString, AString> &metaData);
        void writeMetaDataElement(QXmlStreamWriter &xml, const AString &name, const AString &value);
        void writeLabelTable(QXmlStreamWriter &xml, const std::vector <CiftiLabelElement> &labelElement);
        void writeLabel(QXmlStreamWriter &xml, const CiftiLabelElement &label);
        void writeMatrixIndicesMap(QXmlStreamWriter &xml, const CiftiMatrixIndicesMapElement &matrixIndicesMap);
        void writeBrainModel(QXmlStreamWriter &xml, const CiftiBrainModelElement &brainModel);
        void writeNamedMap(QXmlStreamWriter& xml, const CiftiNamedMapElement& namedMap);
        void writeParcel(QXmlStreamWriter& xml, const CiftiParcelElement& parcel);
        void writeParcelNodes(QXmlStreamWriter& xml, const CiftiParcelNodesElement& parcelNodes);
        void writeVolume(QXmlStreamWriter &xml, const CiftiVolumeElement &volume);
        void writeTransformationMatrixVoxelIndicesIJKtoXYZ(QXmlStreamWriter &xml, const TransformationMatrixVoxelIndicesIJKtoXYZElement &transform);
        void getModelTypeString(int modelType, QString &modelTypeString);
        void getDataSpaceString(int dataSpace, QString &dataSpaceString);
        void getUnitsXYZString(int UnitsXYZ, QString &unitsXYZString);
    public:
        void writeCiftiXML(QXmlStreamWriter &xml, const CiftiRootElement &rootElement);
    };

}
#endif //__CIFTI_XML_WRITER_H__
