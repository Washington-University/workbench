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
#ifndef __CIFTI_XML
#define __CIFTI_XML
#include "StructureEnum.h"
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "CiftiXMLWriter.h"
#include "VolumeFile.h"
#include <QtCore>
#include <vector>

namespace caret {
/// Simple Container class for storing Cifti XML meta data
    struct CiftiSurfaceMap
    {
        int64_t m_ciftiIndex;
        int64_t m_surfaceNode;
    };

    struct CiftiVolumeMap
    {
        int64_t m_ciftiIndex;
        int64_t m_ijk[3];
    };

    struct CiftiVolumeStructureMap
    {
        std::vector<CiftiVolumeMap> m_map;
        StructureEnum::Enum m_structure;
    };

    class CiftiXML {
    public:
        //TODO create initializers for various types of XML meta data (Dense Connectivity, Dense Time Series, etc)
        /**
        * Default Constructor
        *
        * Default Constructor
        */
        CiftiXML();
        /**
        * Constructor
        *
        * Constructor, create class using already existing Cifti xml tree
        * @param xml_root
        */
        CiftiXML(CiftiRootElement &xml_root) { m_root = xml_root; rootChanged(); }
        /**
        * Constructor
        *
        * Constructor, create class using ASCII formatted byte array that
        * containes xml meta data text
        * @param bytes
        */
        CiftiXML(const QByteArray &bytes) { readXML(bytes); }
        /**
        * Constructor
        *
        * Constructor, create class using QString that contains xml
        * meta data text
        * @param xml_string
        */
        CiftiXML(const QString &xml_string) { readXML(xml_string); }
        /**
        * Constructor
        *
        * Constructor, create class using QXmlStreamReader.
        * QXmlStreamReader is assumed to be reading from Cifti XML
        * Text.
        * @param xml_stream
        */
        CiftiXML(QXmlStreamReader &xml_stream) { readXML(xml_stream); }
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param bytes an ASCII formatted byte array that contains Cifti XML data
        */
        void readXML(const QByteArray &bytes) { QString text(bytes);readXML(text);}
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param text QString that contains Cifti XML data
        */
        void readXML(const QString &text) {QXmlStreamReader xml(text); readXML(xml);}
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param xml_stream
        */
        void readXML(QXmlStreamReader &xml_stream) { parseCiftiXML(xml_stream,m_root); rootChanged(); }
        /**
        * writeXML
        *
        * write the Cifti XML data to the supplied QString
        * @param text
        */
        void writeXML(QString &text) { QXmlStreamWriter xml(&text); writeCiftiXML(xml,m_root);}
        /**
        * writeXML
        *
        * write the Cifti XML data to the supplied byte array.
        * @param bytes
        */
        void writeXML(QByteArray &bytes) { QXmlStreamWriter xml(&bytes); writeCiftiXML(xml,m_root);}

        /**
        * setXMLRoot
        *
        * set the Cifti XML root
        * @param xml_root
        */
        void setXMLRoot (CiftiRootElement &xml_root) { m_root = xml_root; rootChanged(); }
        /**
        * getXMLRoot
        *
        * get a copy of the Cifti XML Root
        * @param xml_root
        */
        void getXMLRoot (CiftiRootElement &xml_root) const { xml_root = m_root; }
        
        ///get the row index for a node, returns -1 if it doesn't find a matching mapping
        int64_t getRowIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///get the column index for a node, returns -1 if it doesn't find a matching mapping
        int64_t getColumnIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///SLOW! - get the row index for a voxel, returns -1 if it doesn't find a matching mapping
        int64_t getRowIndexForVoxel(const int64_t* ijk) const;
        
        ///SLOW! - get the column index for a voxel, returns -1 if it doesn't find a matching mapping
        int64_t getColumnIndexForVoxel(const int64_t* ijk) const;
        
        ///SLOW! - get the column index for a voxel coordinate, returns -1 if the closest indexes have no cifti data
        int64_t getRowIndexForVoxelCoordinate(const float* xyz) const;
        
        ///SLOW! - get the column index for a voxel coordinate, returns -1 if the closest indexes have no cifti data
        int64_t getColumnIndexForVoxelCoordinate(const float* xyz) const;
        
        ///get row index for a timepoint
        int64_t getRowIndexForTimepoint(const float& seconds) const;
        
        ///get row index for a timepoint
        int64_t getColumnIndexForTimepoint(const float& seconds) const;
        
        ///get the mapping for a surface in rows, returns false and empty vector if not found
        bool getSurfaceMapForRows(std::vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the mapping for a surface in columns, returns false and empty vector if not found
        bool getSurfaceMapForColumns(std::vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const;
            
        ///get the mapping for a volume in rows, returns false and empty vector if not found
        bool getVolumeMapForRows(std::vector<CiftiVolumeMap>& mappingOut) const;
        
        ///get the mapping for a volume in columns, returns false and empty vector if not found
        bool getVolumeMapForColumns(std::vector<CiftiVolumeMap>& mappingOut) const;
            
        ///get the mapping for a volume in rows, returns false and empty vector if not found
        bool getVolumeStructureMapForRows(std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the mapping for a volume in columns, returns false and empty vector if not found
        bool getVolumeStructureMapForColumns(std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the lists of what structures exist
        bool getStructureListsForRows(std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList) const;

        ///get the lists of what structures exist
        bool getStructureListsForColumns(std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList) const;

        ///get the list of volume parcels and their maps in rows, returns false and empty vector if not found
        bool getVolumeParcelMapsForRows(std::vector<CiftiVolumeStructureMap>& mappingsOut) const;

        ///get the list of volume parcels and their maps in columns, returns false and empty vector if not found
        bool getVolumeParcelMapsForColumns(std::vector<CiftiVolumeStructureMap>& mappingsOut) const;

        ///get the original number of nodes of the surfaces used to make this cifti, for rows
        int64_t getRowSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        
        ///get the original number of nodes of the surfaces used to make this cifti, for columns
        int64_t getColumnSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        
        ///get the timestep for rows, returns false if not timeseries
        bool getRowTimestep(float& seconds) const;
        
        ///get the timestep for columns, returns false if not timeseries
        bool getColumnTimestep(float& seconds) const;
        
        ///get the number of timepoints for rows, returns false if not timeseries, sets -1 if unknown number of timepoints
        bool getRowNumberOfTimepoints(int& numTimepoints) const;
        
        ///get the number of timepoints for rows, returns false if not timeseries, sets -1 if unknown number of timepoints
        bool getColumnNumberOfTimepoints(int& numTimepoints) const;
        
        ///set the timestep for rows, returns false if not timeseries
        bool setRowTimestep(const float& seconds);
        
        ///set the timestep for columns, returns false if not timeseries
        bool setColumnTimestep(const float& seconds);
        
        ///set the number of timepoints for rows, returns false if not timeseries
        bool setRowNumberOfTimepoints(const int& numTimepoints);
        
        ///set the number of timepoints for rows, returns false if not timeseries
        bool setColumnNumberOfTimepoints(const int& numTimepoints);
        
        ///set rows to be brain models, and clear the list of brain models for rows
        void resetRowsToBrainModels();
        
        ///set columns to be brain models, and clear the list of brain models for columns
        void resetColumnsToBrainModels();
        
        ///add a surface brain model to the list of brain models for rows
        bool addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi = NULL);
        
        ///add a surface brain model to the list of brain models for columns
        bool addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi = NULL);
        
        ///add a surface brain model to the list of brain models for rows
        bool addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const std::vector<int64_t>& nodeList);
        
        ///add a surface brain model to the list of brain models for columns
        bool addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const std::vector<int64_t>& nodeList);
        
        ///add a volume brain model to the list of brain models for rows
        bool addVolumeModelToRows(const std::vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure);
        
        ///add a volume brain model to the list of brain models for columns
        bool addVolumeModelToColumns(const std::vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure);
        
        ///set rows to be of type timepoints
        void resetRowsToTimepoints(const float& timestep, const int& timepoints);
        
        ///set columns to be of type timepoints
        void resetColumnsToTimepoints(const float& timestep, const int& timepoints);
        
        ///set the column map to also apply to rows
        void applyColumnMapToRows();
        
        ///set the row map to also apply to columns
        void applyRowMapToColumns();
        
        ///get the number of rows (column length)
        int getNumberOfRows() const;
        
        ///get the number of columns (row length)
        int getNumberOfColumns() const;
        
        ///get what mapping type the rows use
        IndicesMapToDataType getRowMappingType() const;
        
        ///get what mapping type the columns use
        IndicesMapToDataType getColumnMappingType() const;
        
        ///get dimensions, spacing, origin for the volume attribute - returns false if not plumb
        bool getVolumeAttributesForPlumb(VolumeFile::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const;
        
        ///get dimensions and sform, useful for making a volume
        bool getVolumeDimsAndSForm(int64_t dimsOut[3], std::vector<std::vector<float> >& sformOut) const;
        
        ///set the volume space
        void setVolumeDimsAndSForm(const int64_t dims[3], const std::vector<std::vector<float> >& sform);
        
        ///check what types of data it has
        bool hasRowVolumeData() const;
        bool hasColumnVolumeData() const;
        bool hasRowSurfaceData(const StructureEnum::Enum& structure) const;
        bool hasColumnSurfaceData(const StructureEnum::Enum& structure) const;
        
    protected:
        CiftiRootElement m_root;
        int m_rowMapIndex, m_colMapIndex;
        
        ///updates the member variables associated with our root, should only be needed after reading from XML
        void rootChanged();
        
        ///convenience functions to grab the correct model out of the tree, to replace the rowLeft/rowRight stuff (since we might have other surfaces too)
        const CiftiBrainModelElement* findSurfaceModel(const int& myMapIndex, const StructureEnum::Enum& structure) const;
        const CiftiBrainModelElement* findVolumeModel(const int& myMapIndex, const StructureEnum::Enum& structure) const;
        
        ///some boilerplate to get the correct index in a particular mapping
        int64_t getSurfaceIndex(const int64_t& node, const CiftiBrainModelElement* myModel) const;
        int64_t getVolumeIndex(const int64_t* ijk, const int& myMapIndex) const;
        int64_t getVolumeIndex(const float* xyz, const int& myMapIndex) const;
        int64_t getTimestepIndex(const float& seconds, const int& myMapIndex) const;
        bool getTimestep(float& seconds, const int& myMapIndex) const;
        bool setTimestep(const float& seconds, const int& myMapIndex);
        
        ///some boilerplate to build mappings
        bool getSurfaceMapping(std::vector<CiftiSurfaceMap>& mappingOut, const CiftiBrainModelElement* myModel) const;
        bool getVolumeMapping(std::vector<CiftiVolumeMap>& mappingOut, const int& myMapIndex) const;
        bool getVolumeStructureMapping(std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure, const int& myMapIndex) const;
        bool getVolumeParcelMappings(std::vector<CiftiVolumeStructureMap>& mappingsOut, const int& myMapIndex) const;
        bool getStructureLists(std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList, const int& myMapIndex) const;
        
        ///boilerplate for has data
        bool hasVolumeData(const int& myMapIndex) const;
        
        bool addSurfaceModel(const int& myMapIndex, const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi);
        bool addSurfaceModel(const int& myMapIndex, const int& numberOfNodes, const StructureEnum::Enum& structure, const std::vector<int64_t>& nodeList);
        bool addVolumeModel(const int& myMapIndex, const std::vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure);
        bool checkVolumeIndices(const std::vector<voxelIndexType>& ijkList) const;
        bool checkSurfaceNodes(const std::vector<int64_t>& nodeList, const int& numberOfNodes) const;
        void applyDimensionHelper(const int& from, const int& to);
        int getNewRangeStart(const int& myMapIndex) const;
        void separateMaps();
        int createMap(int dimension);
    };

}
#endif//__CIFTI_XML
