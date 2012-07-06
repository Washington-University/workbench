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

#ifndef __CIFTI_XML_WRITER_H__
#define __CIFTI_XML_WRITER_H__
#include <QtCore>
#include "CiftiXMLElements.h"

namespace caret {

void writeCiftiXML(QXmlStreamWriter &xml, const CiftiRootElement &rootElement);
void writeMatrixElement(QXmlStreamWriter &xml, const CiftiMatrixElement &matrixElement);
void writeMetaData(QXmlStreamWriter &xml, const QHash<QString, QString> &metaData);
void writeMetaDataElement(QXmlStreamWriter &xml, const QString &name, const QString &value);
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

}
#endif //__CIFTI_XML_WRITER_H__
