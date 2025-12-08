#ifndef __CZI_IMAGE_FILE_META_DATA_XML_READER_H__
#define __CZI_IMAGE_FILE_META_DATA_XML_READER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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


#include <array>
#include <memory>
#include <set>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "Vector3D.h"

class QXmlStreamReader;

namespace caret {
    class Annotation;
    class AnnotationFile;
    class AnnotationPolyLine;
    class CziImageFile;
    class XmlStreamReaderHelper;

    class CziImageFileMetaDataXmlReader : public CaretObject {
        
    public:
        CziImageFileMetaDataXmlReader();
        
        virtual ~CziImageFileMetaDataXmlReader();
        
        CziImageFileMetaDataXmlReader(const CziImageFileMetaDataXmlReader&) = delete;

        CziImageFileMetaDataXmlReader& operator=(const CziImageFileMetaDataXmlReader&) = delete;
        
        FunctionResultValue<AnnotationFile*> readXmlFromString(CziImageFile* cziImageFile,
                                                               const AString& xmlText);
        
        // ADD_NEW_METHODS_HERE

    private:
        void parseXML();
        
        void addAnnotation(Annotation* annotation);
        
        void readShape(const AString& shapeTagName,
                       const AString& idName);
        
        void readShapeArrow(const AString& shapeTagName,
                            const AString& idName);
        
        void readShapeCircle(const AString& shapeTagName,
                             const AString& idName);
        
        void readShapeEllipse(const AString& shapeTagName,
                             const AString& idName);
        
        void readShapeLine(const AString& shapeTagName,
                           const AString& idName);
        
        void readShapeMarker(const AString& shapeTagName,
                             const AString& idName);
        
        void readShapeRectangle(const AString& shapeTagName,
                                const AString& idName);
        
        void readShapeScaleBar(const AString& shapeTagName,
                               const AString& idName);
        
        void readShapeTextBox(const AString& shapeTagName,
                              const AString& idName);
        
        void addPolyLineCoordinate(AnnotationPolyLine* polyLine,
                                   const Vector3D& xyz);
        
        static std::array<uint8_t, 4> colorToRgba(const AString& colorText);
        
        static Vector3D xyPositionToXYZ(const AString& xyPositionText);
        
        std::unique_ptr<QXmlStreamReader> m_reader;
        
        std::unique_ptr<XmlStreamReaderHelper> m_helper;
        
        std::unique_ptr<AnnotationFile> m_annotationFile;
        
        std::set<AString> m_unrecognizedShapeNames;
        
        int32_t m_imageWidth = 0;
        
        int32_t m_imageHeight = 0;
        
        int32_t m_imageLogicalX = 0;
        
        int32_t m_imageLogicalY = 0;
        
        AString m_cziImageFileNameNoPath;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_FILE_META_DATA_XML_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_FILE_META_DATA_XML_READER_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_FILE_META_DATA_XML_READER_H__
