
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

#define __VTK_FILE_EXPORTER_DECLARE__
#include "VtkFileExporter.h"
#undef __VTK_FILE_EXPORTER_DECLARE__

#include "AString.h"
#include "ByteOrderEnum.h"
#include "CaretAssert.h"
#include "DataFileException.h"
#include "FileAdapter.h"
#include "SurfaceFile.h"
#include "XmlWriter.h"

#include <limits>

using namespace caret;


    
/**
 * \class caret::VtkFileExporter 
 * \brief Exports data into VTK file formats.
 * \ingroup Files
 */

/**
 * Write the given surface files using the coloring from the given browser tab.
 *
 * File format documented at: http://www.vtk.org/VTK/img/file-formats.pdf
 *
 * @param surfaceFile
 *    The surface file.
 * @param surfaceFilesColoring
 *    RGBA coloring for each of the surfaces.
 * @param vtkFileName
 *    Name of the VTK file.
 */
void
VtkFileExporter::writeSurfaces(const std::vector<SurfaceFile*>& surfaceFiles,
                               const std::vector<const float*>& surfaceFilesColoring,
                               const AString& vtkFileName)
{
    try {
        const int32_t numberOfSurfaceFiles = static_cast<int32_t>(surfaceFiles.size());
        if (numberOfSurfaceFiles <= 0) {
            throw DataFileException(vtkFileName,
                                    "No surfaces provided for export to VTK.");
        }
        
        CaretAssert(surfaceFiles.size() == surfaceFilesColoring.size());
        
        int32_t totalNodes = 0;
        int32_t totalTriangles = 0;
        float coordinateMinimum = std::numeric_limits<float>::max();
        float coordinateMaximum = -std::numeric_limits<float>::max();
        
        for (int32_t iSurface = 0; iSurface < numberOfSurfaceFiles; iSurface++) {
            SurfaceFile* surfaceFile = surfaceFiles[iSurface];
            
            const int32_t numberOfNodes = surfaceFile->getNumberOfNodes();
            totalNodes += numberOfNodes;
            
            const int32_t numberOfTriangles = surfaceFile->getNumberOfTriangles();
            totalTriangles += numberOfTriangles;
            
            for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
                const float* xyz = surfaceFile->getCoordinate(iNode);
                for (int32_t i = 0; i < 3; i++) {
                    if (xyz[i] < coordinateMinimum) coordinateMinimum = xyz[i];
                    if (xyz[i] > coordinateMaximum) coordinateMaximum = xyz[i];
                }
            }
        }
        
        if (totalNodes <= 0) {
            throw DataFileException(vtkFileName,
                                    "Surfaces contain no nodes");
        }
        if (totalNodes <= 0) {
            throw DataFileException(vtkFileName,
                                    "Surfaces contain no triangles");
        }
        
        /*
         * Open a text stream
         */
        FileAdapter fileAdapter;
        AString errorMessage;
        QTextStream* textStream = fileAdapter.openQTextStreamForWritingFile(vtkFileName,
                                                                            errorMessage);
        if (textStream == NULL) {
            throw DataFileException(vtkFileName,
                                    errorMessage);
        }
        
        /*
         * Create the XML Writer
         */
        XmlWriter xmlWriter(*textStream);
        xmlWriter.writeStartDocument("1.0");
        
        /*
         * Write the root element
         */
        XmlAttributes rootAttributes;
        rootAttributes.addAttribute("type", "PolyData");
        rootAttributes.addAttribute("version", "0.1");
        switch (ByteOrderEnum::getSystemEndian()) {
            case ByteOrderEnum::ENDIAN_BIG:
                rootAttributes.addAttribute("byte_order", "BigEndian");
                break;
            case ByteOrderEnum::ENDIAN_LITTLE:
                rootAttributes.addAttribute("byte_order", "LittleEndian");
                break;
        }
        rootAttributes.addAttribute("compressor", "vtkZLibDataCompressor");
        xmlWriter.writeStartElement("VTKFile",
                                    rootAttributes);

        /*
         * Start PolyData element
         */
        xmlWriter.writeStartElement("PolyData");
        
        /*
         * Start Piece element
         */
        XmlAttributes pieceAttributes;
        pieceAttributes.addAttribute("NumberOfPoints", totalNodes);
        pieceAttributes.addAttribute("NumberOfVerts", 0);
        pieceAttributes.addAttribute("NumberOfLines", 0);
        pieceAttributes.addAttribute("NumberOfStrips", 0);
        pieceAttributes.addAttribute("NumberOfPolys", totalTriangles);
        xmlWriter.writeStartElement("Piece",
                                    pieceAttributes);
        
        /*
         * Start PointData
         */
        XmlAttributes pointDataAttributes;
        pointDataAttributes.addAttribute("Scalars", "Scalars_");
        pointDataAttributes.addAttribute("Normals", "Normals");
        xmlWriter.writeStartElement("PointData",
                                    pointDataAttributes);
        
        /*
         * Start Normal Vectors DataArray element
         */
        XmlAttributes dataArrayNormalsAttributes;
        dataArrayNormalsAttributes.addAttribute("type", "Float32");
        dataArrayNormalsAttributes.addAttribute("Name", "Normals");
        dataArrayNormalsAttributes.addAttribute("NumberOfComponents", 3);
        dataArrayNormalsAttributes.addAttribute("format", "ascii");
        dataArrayNormalsAttributes.addAttribute("RangeMin", -1.0);
        dataArrayNormalsAttributes.addAttribute("RangeMax", 1.0);
        xmlWriter.writeStartElement("DataArray",
                                    dataArrayNormalsAttributes);
        
        for (int32_t iSurface = 0; iSurface < numberOfSurfaceFiles; iSurface++) {
            const SurfaceFile* sf = surfaceFiles[iSurface];
            const int32_t numberOfNodes = sf->getNumberOfNodes();
            for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
                const float* normalVector = sf->getNormalVector(iNode);
                xmlWriter.writeCharactersWithIndent(AString::number(normalVector[0]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(normalVector[1]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(normalVector[2]));
                xmlWriter.writeCharacters("\n");
            }
        }
        
        /*
         * End DataArray Normals element
         */
        xmlWriter.writeEndElement();

        /*
         * Start Scalars DataArray element
         * that contains the RGB colors
         */
        XmlAttributes dataArrayScalarsAttributes;
        dataArrayScalarsAttributes.addAttribute("type", "UInt8");
        dataArrayScalarsAttributes.addAttribute("Name", "Scalars_");
        dataArrayScalarsAttributes.addAttribute("NumberOfComponents", 3);
        dataArrayScalarsAttributes.addAttribute("format", "ascii");
        dataArrayScalarsAttributes.addAttribute("RangeMin", 0);
        dataArrayScalarsAttributes.addAttribute("RangeMax", 255);
        xmlWriter.writeStartElement("DataArray",
                                    dataArrayScalarsAttributes);
        
        for (int32_t iSurface = 0; iSurface < numberOfSurfaceFiles; iSurface++) {
            SurfaceFile* sf = surfaceFiles[iSurface];
            const float* surfaceRGBA = surfaceFilesColoring[iSurface];
            const int32_t numberOfNodes = sf->getNumberOfNodes();
            for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
                const float* rgbaFloat = &surfaceRGBA[iNode * 4];
                uint8_t rgb[3];
                for (int32_t k = 0; k < 3; k++) {
                    float value = rgbaFloat[k] * 255.0;
                    if (value > 255.0) value = 255.0;
                    if (value < 0.0) value = 0.0;
                    const uint8_t byteValue = static_cast<uint8_t>(value);
                    rgb[k] = byteValue;
                }
                xmlWriter.writeCharactersWithIndent(AString::number(rgb[0]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(rgb[1]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(rgb[2]));
                xmlWriter.writeCharacters("\n");
            }
        }
        
        /*
         * End DataArray Scalars element
         */
        xmlWriter.writeEndElement();
        
        /*
         * End PointData
         */
        xmlWriter.writeEndElement();
        
        /*
         * No Cell Data
         */
        xmlWriter.writeStartElement("CellData");
        xmlWriter.writeEndElement();
        
        /*
         * Start Points element
         */
        xmlWriter.writeStartElement("Points");
        
        /*
         * Start Coordinates DataArray element
         */
        XmlAttributes dataArrayPointsAttributes;
        dataArrayPointsAttributes.addAttribute("type", "Float32");
        dataArrayPointsAttributes.addAttribute("Name", "Points");
        dataArrayPointsAttributes.addAttribute("NumberOfComponents", 3);
        dataArrayPointsAttributes.addAttribute("format", "ascii");
        dataArrayPointsAttributes.addAttribute("RangeMin", coordinateMinimum);
        dataArrayPointsAttributes.addAttribute("RangeMax", coordinateMaximum);
        xmlWriter.writeStartElement("DataArray",
                                    dataArrayPointsAttributes);
        
        for (int32_t iSurface = 0; iSurface < numberOfSurfaceFiles; iSurface++) {
            const SurfaceFile* sf = surfaceFiles[iSurface];
            const int32_t numberOfNodes = sf->getNumberOfNodes();
            for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
                const float* normalVector = sf->getCoordinate(iNode);
                xmlWriter.writeCharactersWithIndent(AString::number(normalVector[0]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(normalVector[1]));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(normalVector[2]));
                xmlWriter.writeCharacters("\n");
            }
        }
        
        /*
         * End DataArray Coordinates
         */
        xmlWriter.writeEndElement();
        
        /*
         * End Points element
         */
        xmlWriter.writeEndElement();
        
        /*
         * Empty Verts Element
         */
        xmlWriter.writeStartElement("Verts");
        xmlWriter.writeEndElement();
        
        /*
         * Empty Lines Element
         */
        xmlWriter.writeStartElement("Lines");
        xmlWriter.writeEndElement();
        
        /*
         * Empty Strips Element
         */
        xmlWriter.writeStartElement("Strips");
        xmlWriter.writeEndElement();
        
        /*
         * Start Polys Element
         */
        xmlWriter.writeStartElement("Polys");
        
        /*
         * Start DataArray for nodes in every triangle
         */
        XmlAttributes dataArrayTrianglesAttributes;
        dataArrayTrianglesAttributes.addAttribute("type", "Int64");
        dataArrayTrianglesAttributes.addAttribute("Name", "connectivity");
        dataArrayTrianglesAttributes.addAttribute("format", "ascii");
        dataArrayTrianglesAttributes.addAttribute("RangeMin", 0);
        dataArrayTrianglesAttributes.addAttribute("RangeMax", totalNodes - 1);
        xmlWriter.writeStartElement("DataArray",
                                    dataArrayTrianglesAttributes);
        
        /*
         * Write the nodes in every triangle
         */
        int32_t triangleNodeOffset = 0;
        for (int32_t iSurface = 0; iSurface < numberOfSurfaceFiles; iSurface++) {
            const SurfaceFile* sf = surfaceFiles[iSurface];
            const int32_t numberOfTriangles = sf->getNumberOfTriangles();
            for (int32_t iTriangle = 0; iTriangle < numberOfTriangles; iTriangle++) {
                const int32_t* triangleNodes = sf->getTriangle(iTriangle);
                xmlWriter.writeCharactersWithIndent(AString::number(triangleNodes[0] + triangleNodeOffset));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(triangleNodes[1] + triangleNodeOffset));
                xmlWriter.writeCharacters(" ");
                xmlWriter.writeCharacters(AString::number(triangleNodes[2] + triangleNodeOffset));
                xmlWriter.writeCharacters("\n");
            }
            
            /*
             * All surface nodes are in one list so offset for additional surfaces
             */
            triangleNodeOffset += sf->getNumberOfNodes();
        }
        
        /*
         * End DataArray for nodes in every triangle
         */
        xmlWriter.writeEndElement();
        
        /*
         * Start DataArray for offset of each triangle
         */
        XmlAttributes dataArrayTriangleOffsetAttributes;
        dataArrayTriangleOffsetAttributes.addAttribute("type", "Int64");
        dataArrayTriangleOffsetAttributes.addAttribute("Name", "offsets");
        dataArrayTriangleOffsetAttributes.addAttribute("format", "ascii");
        dataArrayTriangleOffsetAttributes.addAttribute("RangeMin", 0);
        dataArrayTriangleOffsetAttributes.addAttribute("RangeMax", totalTriangles - 1);
        xmlWriter.writeStartElement("DataArray",
                                    dataArrayTriangleOffsetAttributes);
        
        /*
         * Write offset of each triangle
         */
        for (int32_t i = 1; i <= totalTriangles; i++) {
            if ((i % 6) == 0) {
                if (i > 0) {
                    xmlWriter.writeCharacters("\n");
                }
                xmlWriter.writeCharactersWithIndent(AString::number(i * 3));
            }
            else {
                xmlWriter.writeCharacters(AString::number(i * 3));
            }
            xmlWriter.writeCharacters(" ");
        }
        xmlWriter.writeCharacters("\n");
         

        /*
         * End DataArray for offset of each triangle
         */
        xmlWriter.writeEndElement();
        
        
        /*
         * End Polys Element
         */
        xmlWriter.writeEndElement();
        
        /*
         * End Piece element
         */
        xmlWriter.writeEndElement();
        
        /*
         * End PolyData element
         */
        xmlWriter.writeEndElement();
        
        /*
         * End root element
         */
        xmlWriter.writeEndElement();
        /*
         * Finish XML and close file
         */
        xmlWriter.writeEndDocument();
        fileAdapter.close();
    }
    catch (const XmlException& e) {
        throw DataFileException(vtkFileName,
                                e.whatString());
    }
}
