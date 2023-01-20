#ifndef __FOCI_FILE__H_
#define __FOCI_FILE__H_

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

#include "CaretDataFile.h"
#include "GroupAndNameHierarchyUserInterface.h"

namespace caret {

    class GroupAndNameHierarchyModel;
    class Focus;
    class GiftiLabelTable;
    class GiftiMetaData;
    
    class FociFile : public CaretDataFile, public GroupAndNameHierarchyUserInterface {
        
    public:
        FociFile();
        
        virtual ~FociFile();
        
        FociFile(const FociFile& obj);

        FociFile& operator=(const FociFile& obj);
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        void readFile(const AString& filename);
        
        void writeFile(const AString& filename);
        
        void clear();
        
        bool isEmpty() const;
        
        int32_t getNumberOfFoci() const;
        
        void addFocus(Focus* focus);
        
        Focus* getFocus(const int32_t indx);
        
        const Focus* getFocus(const int32_t indx) const;
        
        void removeFocus(const int32_t indx);
        
        void removeFocus(Focus* focus);
        
        GiftiLabelTable* getClassColorTable();
        
        const GiftiLabelTable* getClassColorTable() const;
        
        GiftiLabelTable* getNameColorTable();
        
        const GiftiLabelTable* getNameColorTable() const;
        
        void createNameAndClassColorTables(const GiftiLabelTable* oldColorTable);
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        QStringList getAllFociNamesSorted() const;
        
        void invalidateAllAssignedColors();
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        virtual void groupAndNameHierarchyItemStatusChanged() override;

        static int32_t getFileVersion();
        
        static AString getFileVersionAsString();
        
        /** XML Tag for foci file */
        static const AString XML_TAG_FOCI_FILE;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;

        /** XML Tag for Name Color Table */
        static const AString XML_TAG_NAME_COLOR_TABLE;
        
        /** XML Tag for Class Color Table */
        static const AString XML_TAG_CLASS_COLOR_TABLE;
        
    private:
        void copyHelperFociFile(const FociFile& obj);
        
        void initializeFociFile();
        
        GiftiMetaData* m_metadata;
        
        std::vector<Focus*> m_foci;
        
        /** Holds colors assigned to classes */
        GiftiLabelTable* m_classColorTable;
        
        /** Holds colors assigned to names */
        GiftiLabelTable* m_nameColorTable;
        
        /** Holds class and name hierarchy used for display selection */
        mutable GroupAndNameHierarchyModel* m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        bool m_forceUpdateOfGroupAndNameHierarchy;
        
        /** Version of this FociFile */
        static const int32_t s_fociFileVersion;
        
    };
    
#ifdef __FOCI_FILE_DECLARE__
    const AString FociFile::XML_TAG_FOCI_FILE = "FociFile";
    const AString FociFile::XML_ATTRIBUTE_VERSION = "Version";
    const AString FociFile::XML_TAG_NAME_COLOR_TABLE = "FociNameColorTable";
    const AString FociFile::XML_TAG_CLASS_COLOR_TABLE = "FociClassColorTable";
    const int32_t FociFile::s_fociFileVersion = 2;
#endif // __FOCI_FILE_DECLARE__

} // namespace
#endif  //__FOCI_FILE__H_
