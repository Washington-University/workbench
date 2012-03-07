#ifndef __BORDER_FILE__H_
#define __BORDER_FILE__H_

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


#include "CaretDataFile.h"

namespace caret {

    class Border;
    class ClassAndNameHierarchyModel;
    class GiftiLabelTable;
    class SurfaceFile;
    class SurfaceProjectedItem;
    
    class BorderFile : public CaretDataFile {
        
    public:
        BorderFile();
        
        virtual ~BorderFile();
        
        BorderFile(const BorderFile& obj);

        BorderFile& operator=(const BorderFile& obj);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        
        void readFile(const AString& filename) throw (DataFileException);
        
        void writeFile(const AString& filename) throw (DataFileException);
        
        void clear();
        
        bool isEmpty() const;
        
        int32_t getNumberOfBorders() const;
        
        Border* getBorder(const int32_t indx);
        
        const Border* getBorder(const int32_t indx) const;
        
        bool findBorderNearestXYZ(const SurfaceFile* surfaceFile,
                                 const float xyz[3],
                                 const float maximumDistance,
                                 Border*& borderOut,
                                 int32_t& borderIndexOut,
                                 SurfaceProjectedItem*& borderPointOut,
                                 int32_t& borderPointIndexOut,
                                 float& distanceToNearestPointOut) const;
        
        void addBorder(Border* border);
        
        void removeBorder(const int32_t indx);
        
        void removeBorder(Border* border);
        
        GiftiLabelTable* getClassColorTable();
        
        const GiftiLabelTable* getClassColorTable() const;
        
        ClassAndNameHierarchyModel* getClassAndNameHierarchyModel();
        
        static float getFileVersion();
        
        static AString getFileVersionAsString();
        
        /** XML Tag for BorderFile element */
        static const AString XML_TAG_BORDER_FILE;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
    private:
        void copyHelperBorderFile(const BorderFile& obj);
        
        void initializeBorderFile();
        
        GiftiMetaData* metadata;
        
        std::vector<Border*> borders;
        
        /** Holds colors assigned to classes */
        GiftiLabelTable* classColorTable;
        
        mutable ClassAndNameHierarchyModel* classNameHierarchy;
        
        /** Version of this BorderFile */
        static const float borderFileVersion;
    };
    
#ifdef __BORDER_FILE_DECLARE__
    const AString BorderFile::XML_TAG_BORDER_FILE = "BorderFile";
    const AString BorderFile::XML_ATTRIBUTE_VERSION = "Version";
    
    const float BorderFile::borderFileVersion = 1.0;
#endif // __BORDER_FILE_DECLARE__

} // namespace
#endif  //__BORDER_FILE__H_
