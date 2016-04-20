#ifndef __CIFTI_FILE_H__
#define __CIFTI_FILE_H__

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

#include "CaretPointer.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "CiftiXMLOld.h"

#include <QString>

#include <vector>

namespace caret
{
    
    class CiftiFile : public CiftiInterface
    {
    public:
        enum ENDIAN
        {
            ANY,//so that writeFile() with default endian argument can do nothing after setWritingFile with any endian argument - uses native if there is no rewrite to avoid
            NATIVE,//as long as there are more than two options anyway, provide a convenience option so people don't need to figure out the machine endianness for a common case
            LITTLE,
            BIG
        };

        CiftiFile() { m_endianPref = NATIVE; }
        explicit CiftiFile(const QString &fileName);//calls openFile
        void openFile(const QString& fileName);//starts on-disk reading
        void openURL(const QString& url, const QString& user, const QString& pass);//open from XNAT
        void openURL(const QString& url);//same, without user/pass (or curently, reusing existing auth if the server matches
        void setWritingFile(const QString& fileName, const CiftiVersion& writingVersion = CiftiVersion(), const ENDIAN& endian = NATIVE);//starts on-disk writing
        void writeFile(const QString& fileName, const CiftiVersion& writingVersion = CiftiVersion(), const ENDIAN& endian = ANY);//leaves current state as-is, rewrites if already writing to that filename and version mismatch
        void convertToInMemory();
        QString getFileName() const { return m_fileName; }
        
        bool isInMemory() const;
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead = false) const;//tolerateShortRead is useful for on-disk writing when it is easiest to do RMW multiple times on a new file
        const std::vector<int64_t>& getDimensions() const { return m_dims; }
        void getColumn(float* dataOut, const int64_t& index) const;//for 2D only, will be slow if on disk!
        
        void setCiftiXML(const CiftiXML& xml, const bool useOldMetadata = true);
        void setCiftiXML(const CiftiXMLOld &xml, const bool useOldMetadata = true);//set xml from old implementation
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);//for 2D only, will be slow if on disk!
        
        void getRow(float* dataOut, const int64_t& index, const bool& tolerateShortRead) const;//backwards compatibility for old CiftiFile/CiftiInterface
        void getRow(float* dataOut, const int64_t& index) const;
        int64_t getNumberOfRows() const;
        int64_t getNumberOfColumns() const;
        
        void setRow(const float* dataIn, const int64_t& index);//backwards compatibility for old CiftiFile
        
        class ReadImplInterface
        {
        public:
            virtual void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const = 0;
            virtual void getColumn(float* dataOut, const int64_t& index) const = 0;
            virtual bool isInMemory() const { return false; }
            virtual ~ReadImplInterface();
        };
        //assume if you can write to it, you can also read from it
        class WriteImplInterface : public ReadImplInterface
        {
        public:
            virtual void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect) = 0;
            virtual void setColumn(const float* dataIn, const int64_t& index) = 0;
            virtual ~WriteImplInterface();
        };
    private:
        std::vector<int64_t> m_dims;
        CaretPointer<WriteImplInterface> m_writingImpl;//this will be equal to m_readingImpl when non-null
        CaretPointer<ReadImplInterface> m_readingImpl;
        QString m_writingFile, m_fileName;
        //CiftiXML m_xml;//uncomment when we drop CiftiInterface
        CiftiVersion m_onDiskVersion;
        ENDIAN m_endianPref;
        
        void verifyWriteImpl();
        static void copyImplData(const ReadImplInterface* from, WriteImplInterface* to, const std::vector<int64_t>& dims);
    };
    
}

#endif //__CIFTI_FILE_H__
