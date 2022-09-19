#ifndef __CZI_META_FILE_XML_STREAM_READER_H__
#define __CZI_META_FILE_XML_STREAM_READER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include <QDir>

#include <memory>
#include <set>

#include "CziMetaFile.h"
#include "CziMetaFileXmlStreamBase.h"

class QXmlStreamReader;

namespace caret {

    class HistologySlice;
    class HistologySliceImage;
    class HistologySlicesFile;
    class Matrix4x4;
    class XmlStreamReaderHelper;
    
    class CziMetaFileXmlStreamReader : public CziMetaFileXmlStreamBase {
        
    public:
        CziMetaFileXmlStreamReader();
        
        virtual ~CziMetaFileXmlStreamReader();
        
        CziMetaFileXmlStreamReader(const CziMetaFileXmlStreamReader&) = delete;

        CziMetaFileXmlStreamReader& operator=(const CziMetaFileXmlStreamReader&) = delete;
        
        void readFile(const AString& filename,
                      CziMetaFile* cziMetaFile);

        void readFile(const AString& filename,
                      HistologySlicesFile* histologySlicesFile);
        
        // ADD_NEW_METHODS_HERE

    private:
        enum class MatrixType {
            THREE_DIM,
            TWO_DIM
        };
        
        void readFileContent(CziMetaFile* cziMetaFile);
        
        void readFileContent(HistologySlicesFile* histologySlicesFile);
        
        CziMetaFile::Slice* readSliceElement(CziMetaFile* cziMetaFile,
                                             const int32_t sliceNumber);

        HistologySlice* readSliceElement(HistologySlicesFile* histologySlicesFile,
                                         const int32_t sliceNumber);
        
        CziMetaFile::Scene* readSceneElement(CziMetaFile* cziMetaFile,
                                             const QString& sceneName);
        
        HistologySliceImage* readSceneElement(HistologySlicesFile* histologySlicesFile,
                                             const QString& sceneName);
        
        void readMatrixFromElementText(const QString& elementName,
                                       const MatrixType matrixType,
                                       Matrix4x4& matrixOut);
        
        AString makeAbsoluteFilePath(const AString& filename) const;
        
        QDir m_directory;
        
        AString m_filename;
        
        std::unique_ptr<QXmlStreamReader> m_xmlReader;
        
        std::unique_ptr<XmlStreamReaderHelper> m_xmlStreamHelper;
        
        int32_t m_fileVersion = -1;
        
        std::set<AString> m_unexpectedXmlElements;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_META_FILE_XML_STREAM_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_META_FILE_XML_STREAM_READER_DECLARE__

} // namespace
#endif  //__CZI_META_FILE_XML_STREAM_READER_H__
