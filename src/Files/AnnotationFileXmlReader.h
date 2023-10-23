#ifndef __ANNOTATION_FILE_XML_READER_H__
#define __ANNOTATION_FILE_XML_READER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationFileXmlFormatBase.h"
#include "CaretPointer.h"

class QXmlStreamAttributes;
class QXmlStreamReader;

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class AnnotationFile;
    class AnnotationFontAttributesInterface;
    class AnnotationGroup;
    class AnnotationImage;
    class AnnotationMultiCoordinateShape;
    class AnnotationMultiPairedCoordinateShape;
    class AnnotationTwoCoordinateShape;
    class AnnotationText;
    class AnnotationOneCoordinateShape;
    class XmlStreamReaderHelper;
    
    class AnnotationFileXmlReader : public AnnotationFileXmlFormatBase {
        
    public:
        AnnotationFileXmlReader();
        
        virtual ~AnnotationFileXmlReader();

        void readFile(const QString& filename,
                      AnnotationFile* annotationFile);
        
        void readFileFromString(const QString& fileInString,
                                const AString& fileNameForRelativePaths,
                                AnnotationFile* annotationFile);

        // ADD_NEW_METHODS_HERE

    private:
        AnnotationFileXmlReader(const AnnotationFileXmlReader&);

        AnnotationFileXmlReader& operator=(const AnnotationFileXmlReader&);
        
        void readFileContentFromXmlStreamReader(const QString& filename,
                                                AnnotationFile* annotationFile);
        
        void readVersionOne(AnnotationFile* annotationFile);
        
        void readVersionTwo(AnnotationFile* annotationFile);
        
        void readGroup(AnnotationFile* annotationFile);
        
        void readTwoCoordinateAnnotation(AnnotationFile* annotationFile,
                                         const QString& annotationElementName,
                                         AnnotationTwoCoordinateShape* annotation);

        void readOneCoordinateAnnotation(AnnotationFile* annotationFile,
                                         const QString& annotationElementName,
                                          AnnotationOneCoordinateShape* annotation);
        
        void readMultiCoordinateAnnotation(AnnotationFile* annotationFile,
                                           const QString& annotationElementName,
                                           AnnotationMultiCoordinateShape* annotation);
        
        void readMultiPairedCoordinateAnnotation(AnnotationFile* annotationFile,
                                                 const QString& annotationElementName,
                                                 AnnotationMultiPairedCoordinateShape* annotation);
        
        void readCoordinate(const QString& coordinateElementName,
                            AnnotationCoordinate* coordinate,
                            const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                            const bool readStartElementFlag);
        
        void readAnnotationAttributes(Annotation* annotation,
                                      const QString& annotationElementName,
                                      const QXmlStreamAttributes& attributes);
        
        void readImageDataElement(AnnotationImage* imageAnnotation);
        
        void readTextDataElement(AnnotationText* textAnnotation,
                                 const QString& annotationTextElementName);
        
        void readFontAttibutes(AnnotationFontAttributesInterface* fontAttributes,
                               const AString& elementName,
                               const QXmlStreamAttributes& attributes);
        
        CaretPointer<QXmlStreamReader> m_stream;
        
        CaretPointer<XmlStreamReaderHelper> m_streamHelper;
        
        QString m_filename;
        
        int32_t m_fileVersionNumber;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_XML_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FILE_XML_READER_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_XML_READER_H__
