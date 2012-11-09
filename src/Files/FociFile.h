#ifndef __FOCI_FILE__H_
#define __FOCI_FILE__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretDataFile.h"

namespace caret {

    class GroupAndNameHierarchyModel;
    class Focus;
    class GiftiLabelTable;
    class GiftiMetaData;
    
    class FociFile : public CaretDataFile {
        
    public:
        FociFile();
        
        virtual ~FociFile();
        
        FociFile(const FociFile& obj);

        FociFile& operator=(const FociFile& obj);
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        void readFile(const AString& filename) throw (DataFileException);
        
        void writeFile(const AString& filename) throw (DataFileException);
        
        void clear();
        
        bool isEmpty() const;
        
        int32_t getNumberOfFoci() const;
        
        void addFocus(Focus* focus);
        
        Focus* getFocus(const int32_t indx);
        
        const Focus* getFocus(const int32_t indx) const;
        
        void removeFocus(const int32_t indx);
        
        void removeFocus(Focus* focus);
        
        GiftiLabelTable* getColorTable();
        
        const GiftiLabelTable* getColorTable() const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        void invalidateAllAssignedColors();
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        static float getFileVersion();
        
        static AString getFileVersionAsString();
        
        /** XML Tag for foci file */
        static const AString XML_TAG_FOCI_FILE;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;

    private:
        void copyHelperFociFile(const FociFile& obj);
        
        void initializeFociFile();
        
        GiftiMetaData* m_metadata;
        
        std::vector<Focus*> m_foci;
        
        /** Holds colors assigned to classes and names */
        GiftiLabelTable* m_colorTable;
        
        /** Holds class and name hierarchy used for display selection */
        mutable GroupAndNameHierarchyModel* m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        bool m_forceUpdateOfGroupAndNameHierarchy;
        
        /** Version of this FociFile */
        static const float fociFileVersion;
    };
    
#ifdef __FOCI_FILE_DECLARE__
    const AString FociFile::XML_TAG_FOCI_FILE = "FociFile";
    const AString FociFile::XML_ATTRIBUTE_VERSION = "Version";
    const float FociFile::fociFileVersion = 1.0;
#endif // __FOCI_FILE_DECLARE__

} // namespace
#endif  //__FOCI_FILE__H_
