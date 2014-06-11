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
#ifndef __CIFTI_XML_OLD__
#define __CIFTI_XML_OLD__
#include "StructureEnum.h"
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "CiftiXMLWriter.h"
#include "VolumeBase.h"
#include "VolumeSpace.h"
#include <QtCore>
#include <vector>

namespace caret {
    class DataFileContentInformation;

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
    
    struct CiftiBrainModelInfo
    {
        ModelType m_type;
        StructureEnum::Enum m_structure;
        CiftiBrainModelInfo()
        {
            m_type = CIFTI_MODEL_TYPE_INVALID;
            m_structure = StructureEnum::INVALID;
        }
    };
    
    class CiftiXMLOld {
    public:
        enum
        {
            ALONG_ROW = 0,
            ALONG_COLUMN = 1,
            ALONG_STACK = 2//better name for this?
        };
        /**
        * Default Constructor
        *
        * Default Constructor
        */
        CiftiXMLOld();
        /**
        * Constructor
        *
        * Constructor, create class using already existing Cifti xml tree
        * @param xml_root
        */
        //CiftiXML(CiftiRootElement &xml_root) { m_root = xml_root; rootChanged(); }
        /**
        * Constructor
        *
        * Constructor, create class using ASCII formatted byte array that
        * containes xml meta data text
        * @param bytes
        */
        CiftiXMLOld(const QByteArray &bytes) { readXML(bytes); }
        /**
        * Constructor
        *
        * Constructor, create class using QString that contains xml
        * meta data text
        * @param xml_string
        */
        CiftiXMLOld(const QString &xml_string) { readXML(xml_string); }
        /**
        * Constructor
        *
        * Constructor, create class using QXmlStreamReader.
        * QXmlStreamReader is assumed to be reading from Cifti XML
        * Text.
        * @param xml_stream
        */
        CiftiXMLOld(QXmlStreamReader &xml_stream) { readXML(xml_stream); }
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param bytes an ASCII formatted byte array that contains Cifti XML data
        */
        void readXML(const QByteArray &bytes) { QString text(bytes);readXML(text); }
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param text QString that contains Cifti XML data
        */
        void readXML(const QString &text) {QXmlStreamReader xml(text); readXML(xml); }
        /**
        * readXML
        *
        * readXML, replacing the currently Cifti XML Root, if it exists
        * @param xml_stream
        */
        void readXML(QXmlStreamReader &xml_stream) { CiftiXMLReader myReader; myReader.parseCiftiXML(xml_stream,m_root); rootChanged(); }
        /**
        * writeXML
        *
        * write the Cifti XML data to the supplied QString
        * @param text
        */
        void writeXML(QString &text) const { text = ""; QXmlStreamWriter xml(&text); CiftiXMLWriter myWriter; myWriter.writeCiftiXML(xml,m_root); }//we don't use the old writer in testing, so it won't recurse
        /**
        * writeXML
        *
        * write the Cifti XML data to the supplied byte array.
        * @param bytes
        */
        void writeXML(QByteArray &bytes) const { bytes.clear(); QXmlStreamWriter xml(&bytes); CiftiXMLWriter myWriter; myWriter.writeCiftiXML(xml,m_root); }
        
        //static void testNewXML(const QString& xmlString);
        //static void testNewXML(const QByteArray& xmlString);

        /**
        * setXMLRoot
        *
        * set the Cifti XML root
        * @param xml_root
        */
        //void setXMLRoot (CiftiRootElement &xml_root) { m_root = xml_root; rootChanged(); }
        
        /**
        * getXMLRoot
        *
        * get a copy of the Cifti XML Root
        * @param xml_root
        */
        //void getXMLRoot (CiftiRootElement &xml_root) const { xml_root = m_root; }
        
        const CiftiVersion& getVersion() const { return m_root.m_version; }
        
        ///get the row index for a node, returns -1 if it doesn't find a matching mapping
        int64_t getRowIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///get the column index for a node, returns -1 if it doesn't find a matching mapping
        int64_t getColumnIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///get the row index for a voxel, returns -1 if it doesn't find a matching mapping
        int64_t getRowIndexForVoxel(const int64_t* ijk) const;
        
        ///get the column index for a voxel, returns -1 if it doesn't find a matching mapping
        int64_t getColumnIndexForVoxel(const int64_t* ijk) const;
        
        ///get the column index for a voxel coordinate, returns -1 if the closest indexes have no cifti data
        int64_t getRowIndexForVoxelCoordinate(const float* xyz) const;
        
        ///get the column index for a voxel coordinate, returns -1 if the closest indexes have no cifti data
        int64_t getColumnIndexForVoxelCoordinate(const float* xyz) const;
        
        ///get row index for a timepoint
        int64_t getRowIndexForTimepoint(const float& seconds) const;
        
        ///get row index for a timepoint
        int64_t getColumnIndexForTimepoint(const float& seconds) const;
        
        ///get the mapping for a surface in rows, returns false and empty vector if not found
        bool getSurfaceMapForRows(std::vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the mapping for a surface in columns, returns false and empty vector if not found
        bool getSurfaceMapForColumns(std::vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const;
            
        ///get the mapping for a surface in columns, returns false and empty vector if not found
        bool getSurfaceMap(const int& direction, std::vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const;
            
        ///get the mapping for a volume in rows, returns false and empty vector if not found
        bool getVolumeMapForRows(std::vector<CiftiVolumeMap>& mappingOut) const;
        
        ///get the mapping for a volume in columns, returns false and empty vector if not found
        bool getVolumeMapForColumns(std::vector<CiftiVolumeMap>& mappingOut) const;
            
        ///get the mapping for a volume, returns false and empty vector if not found
        bool getVolumeMap(const int& direction, std::vector<CiftiVolumeMap>& mappingOut) const;
        
        void getVoxelInfoInDataFileContentInformation(const int& direction,
                                                      DataFileContentInformation& dataFileInformation) const;
        
        ///get the mapping for a volume structure in rows, returns false and empty vector if not found
        bool getVolumeStructureMapForRows(std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the mapping for a volume structure in columns, returns false and empty vector if not found
        bool getVolumeStructureMapForColumns(std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const;
        
        ///get the mapping for a volume structure
        bool getVolumeStructureMap(const int& direction, std::vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const;

        ///get the lists of what structures exist
        bool getStructureListsForRows(std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList) const;

        ///get the lists of what structures exist
        bool getStructureListsForColumns(std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList) const;

        ///get the lists of what structures exist
        bool getStructureLists(const int& direction, std::vector<StructureEnum::Enum>& surfaceList, std::vector<StructureEnum::Enum>& volumeList) const;
        
        ///get the number of structures for a brain model mapping
        int getNumberOfBrainModels(const int& direction) const;
        
        ///get structure info by structure index
        CiftiBrainModelInfo getBrainModelInfo(const int& direction, const int& whichModel) const;

        ///get the list of volume parcels and their maps in rows, returns false and empty vector if not found
        bool getVolumeModelMapsForRows(std::vector<CiftiVolumeStructureMap>& mappingsOut) const;

        ///get the list of volume parcels and their maps in columns, returns false and empty vector if not found
        bool getVolumeModelMapsForColumns(std::vector<CiftiVolumeStructureMap>& mappingsOut) const;

        ///get the original number of nodes of the surfaces used to make this cifti, for rows
        int64_t getRowSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        
        ///get the original number of nodes of the surfaces used to make this cifti, for columns
        int64_t getColumnSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        
        ///get the original number of nodes of the surfaces used to make this cifti along a direction
        int64_t getSurfaceNumberOfNodes(const int& direction, const StructureEnum::Enum& structure) const;
        
        ///get the timestep for rows, returns false if not timeseries
        bool getRowTimestep(float& seconds) const;
        
        ///get the timestep for columns, returns false if not timeseries
        bool getColumnTimestep(float& seconds) const;
        
        ///get the timestart for rows, returns false if not set or not timeseries
        bool getRowTimestart(float& seconds) const;
        
        ///get the timestart for columns, returns false if not set or not timeseries
        bool getColumnTimestart(float& seconds) const;
        
        ///get the number of timepoints for rows, returns false if not timeseries, sets -1 if unknown number of timepoints
        bool getRowNumberOfTimepoints(int& numTimepoints) const;
        
        ///get the number of timepoints for rows, returns false if not timeseries, sets -1 if unknown number of timepoints
        bool getColumnNumberOfTimepoints(int& numTimepoints) const;
        
        ///get the parcels for rows
        bool getParcelsForRows(std::vector<CiftiParcelElement>& parcelsOut) const;
        
        ///get the parcels for columns
        bool getParcelsForColumns(std::vector<CiftiParcelElement>& parcelsOut) const;
        
        ///get the parcels for a dimension
        bool getParcels(const int& direction, std::vector<CiftiParcelElement>& parcelsOut) const;
        
        ///get the parcel surface structures
        bool getParcelSurfaceStructures(const int& direction, std::vector<StructureEnum::Enum>& structuresOut) const;
        
        ///get the row parcel for a node
        int64_t getRowParcelForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///get the column parcel for a node
        int64_t getColumnParcelForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        
        ///get the row parcel for a voxel
        int64_t getRowParcelForVoxel(const int64_t* ijk) const;
        
        ///get the row parcel for a voxel
        int64_t getColumnParcelForVoxel(const int64_t* ijk) const;
        
        ///set the timestep for rows, returns false if not timeseries
        bool setRowTimestep(const float& seconds);
        
        ///set the timestep for columns, returns false if not timeseries
        bool setColumnTimestep(const float& seconds);
        
        ///set the timestart for rows, returns false if not set or not timeseries
        bool setRowTimestart(const float& seconds);
        
        ///set the timestart for columns, returns false if not set or not timeseries
        bool setColumnTimestart(const float& seconds);
        
        ///set the number of timepoints for rows, returns false if not timeseries
        bool setRowNumberOfTimepoints(const int& numTimepoints);
        
        ///set the number of timepoints for rows, returns false if not timeseries
        bool setColumnNumberOfTimepoints(const int& numTimepoints);
        
        ///set rows to be brain models, and clear the list of brain models for rows
        void resetRowsToBrainModels();
        
        ///set columns to be brain models, and clear the list of brain models for columns
        void resetColumnsToBrainModels();
        
        ///set direction to be brain models, and clear it
        void resetDirectionToBrainModels(const int& direction);
        
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
        
        ///add surface or volume model by direction
        bool addSurfaceModel(const int& direction, const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi = NULL);
        bool addSurfaceModel(const int& direction, const int& numberOfNodes, const StructureEnum::Enum& structure, const std::vector<int64_t>& nodeList);
        bool addVolumeModel(const int& direction, const std::vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure);
        
        ///add a surface to the list of parcel surfaces for rows
        bool addParcelSurfaceToRows(const int& numberOfNodes, const StructureEnum::Enum& structure);
        
        ///add a surface to the list of parcel surfaces for columns
        bool addParcelSurfaceToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure);
        
        ///add a surface to the list of parcel surfaces
        bool addParcelSurface(const int& direction, const int& numberOfNodes, const StructureEnum::Enum& structure);
        
        ///add a parcel to rows
        bool addParcelToRows(const CiftiParcelElement& parcel);
        
        ///add a parcel to columns
        bool addParcelToColumns(const CiftiParcelElement& parcel);
        
        ///add a parcel to columns
        bool addParcel(const int& direction, const CiftiParcelElement& parcel);
        
        ///set rows to be of type timepoints
        void resetRowsToTimepoints(const float& timestep, const int& numTimepoints, const float& timestart = 0.0f);
        
        ///set columns to be of type timepoints
        void resetColumnsToTimepoints(const float& timestep, const int& numTimepoints, const float& timestart = 0.0f);
        
        ///set rows to be of type scalars
        void resetRowsToScalars(const int64_t& numMaps);
        
        ///set columns to be of type scalars
        void resetColumnsToScalars(const int64_t& numMaps);
        
        ///set a direction to scalars
        void resetDirectionToScalars(const int& direction, const int64_t& numMaps);
        
        ///set rows to be of type labels
        void resetRowsToLabels(const int64_t& numMaps);
        
        ///set columns to be of type labels
        void resetColumnsToLabels(const int64_t& numMaps);
        
        ///set a direction to labels
        void resetDirectionToLabels(const int& direction, const int64_t& numMaps);
        
        ///set rows to be of type parcels
        void resetRowsToParcels();
        
        ///set columns to be of type parcels
        void resetColumnsToParcels();
        
        ///set dimension to be of type parcels
        void resetDirectionToParcels(const int& direction);
        
        ///get the map name for an index along a column
        AString getMapNameForColumnIndex(const int64_t& index) const;
        
        ///get the map name for an index along a row
        AString getMapNameForRowIndex(const int64_t& index) const;
        
        ///get the map name for an index
        AString getMapName(const int& direction, const int64_t& index) const;
        
        ///get the index for a map number/name - NOTE: returns -1 if mapping type doesn't support names
        int64_t getMapIndexFromNameOrNumber(const int& direction, const AString& numberOrName) const;

        //HACK: const method returns non-const GiftiLabelTable pointer because getCiftiXML MUST return a const CiftiXML&, but we need to be able to change the label table
        ///get the label table for an index along a column
        GiftiLabelTable* getLabelTableForColumnIndex(const int64_t& index) const;
        
        //HACK: const method returns non-const GiftiLabelTable pointer because getCiftiXML MUST return a const CiftiXML&, but we need to be able to change the label table
        ///get the label table for an index along a row
        GiftiLabelTable* getLabelTableForRowIndex(const int64_t& index) const;
        
        //HACK: const method returns non-const GiftiLabelTable pointer because getCiftiXML MUST return a const CiftiXML&, but we need to be able to change the label table
        ///get the label table for an index
        GiftiLabelTable* getMapLabelTable(const int& direction, const int64_t& myMapIndex) const;

        ///set the map name for an index along a column
        bool setMapNameForColumnIndex(const int64_t& index, const AString& name) const;
        
        ///set the map name for an index along a row
        bool setMapNameForRowIndex(const int64_t& index, const AString& name) const;
        
        ///set the map name for an index
        bool setMapNameForIndex(const int& direction, const int64_t& index, const AString& name) const;
        
        ///set the label table for an index along a column
        bool setLabelTableForColumnIndex(const int64_t& index, const GiftiLabelTable& labelTable);
        
        ///set the label table for an index along a row
        bool setLabelTableForRowIndex(const int64_t& index, const GiftiLabelTable& labelTable);
        
        ///set the column map to also apply to rows
        void applyColumnMapToRows();
        
        ///set the row map to also apply to columns
        void applyRowMapToColumns();
        
        ///get the number of rows (column length)
        int64_t getNumberOfRows() const;
        
        ///get the number of columns (row length)
        int64_t getNumberOfColumns() const;
        
        ///get the length of a dimension
        int64_t getDimensionLength(const int& direction) const;
        
        ///get what mapping type the rows use
        IndicesMapToDataType getRowMappingType() const;
        
        ///get what mapping type the columns use
        IndicesMapToDataType getColumnMappingType() const;
        
        ///get what mapping type a dimension uses
        IndicesMapToDataType getMappingType(const int& direction) const;
        
        ///get dimensions, spacing, origin for the volume attribute - returns false if not plumb
        bool getVolumeAttributesForPlumb(VolumeSpace::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const;
        
        ///get dimensions and sform, useful for making a volume
        bool getVolumeDimsAndSForm(int64_t dimsOut[3], std::vector<std::vector<float> >& sformOut) const;
        
        ///set the volume space
        void setVolumeDimsAndSForm(const int64_t dims[3], const std::vector<std::vector<float> >& sform);
        
        ///get volume space object
        bool getVolumeSpace(VolumeSpace& volSpaceOut) const;
        
        ///swap mappings between two directions
        void swapMappings(const int& direction1, const int& direction2);
        
        ///check what types of data it has
        bool hasRowVolumeData() const;
        bool hasColumnVolumeData() const;
        bool hasVolumeData(const int& direction) const;
        bool hasRowSurfaceData(const StructureEnum::Enum& structure) const;
        bool hasColumnSurfaceData(const StructureEnum::Enum& structure) const;
        bool hasSurfaceData(const int& direction, const StructureEnum::Enum& structure) const;
        
        ///comparison
        bool mappingMatches(const int& direction, const CiftiXMLOld& other, const int& otherDirection) const;
        bool matchesForRows(const CiftiXMLOld& rhs) const;
        bool matchesForColumns(const CiftiXMLOld& rhs) const;
        bool matchesVolumeSpace(const CiftiXMLOld& rhs) const;
        bool operator==(const CiftiXMLOld& rhs) const;
        bool operator!=(const CiftiXMLOld& rhs) const { return !((*this) == rhs); }
        
        ///take a mapping from another xml object
        void copyMapping(const int& direction, const CiftiXMLOld& other, const int& otherDirection);
        
        std::map<AString, AString>* getFileMetaData() const;
        
        std::map<AString, AString>* getMapMetadata(const int& direction, const int& index) const;
        
        //HACK: const method returns non-const PaletteColorMapping pointer because getCiftiXML MUST return a const CiftiXML&, but we need to be able to change the palette
        PaletteColorMapping* getFilePalette() const;
        
        PaletteColorMapping* getMapPalette(const int& direction, const int& index) const;
        
    protected:
        CiftiRootElement m_root;
        //int m_rowMapIndex, m_colMapIndex;
        std::vector<int> m_dimToMapLookup;
        
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
        int64_t getParcelForNode(const int64_t& node, const StructureEnum::Enum& structure, const int& myMapIndex) const;
        int64_t getParcelForVoxel(const int64_t* ijk, const int& myMapIndex) const;
        
        ///boilerplate for map information
        bool getTimestep(float& seconds, const int& myMapIndex) const;
        bool getTimestart(float& seconds, const int& myMapIndex) const;
        bool setTimestep(const float& seconds, const int& myMapIndex);
        bool setTimestart(const float& seconds, const int& myMapIndex);
        bool setLabelTable(const int64_t& index, const GiftiLabelTable& labelTable, const int& myMapIndex);
        
        ///some boilerplate to retrieve mappings
        bool getVolumeModelMappings(std::vector<CiftiVolumeStructureMap>& mappingsOut, const int& myMapIndex) const;
        
        ///miscellaneous
        bool checkVolumeIndices(const std::vector<voxelIndexType>& ijkList) const;
        bool checkSurfaceNodes(const std::vector<int64_t>& nodeList, const int& numberOfNodes) const;
        void applyDimensionHelper(const int& from, const int& to);
        int64_t getNewRangeStart(const int& myMapIndex) const;
        void separateMaps();
        int createMap(int dimension);
    };

}
#endif //__CIFTI_XML_OLD__
