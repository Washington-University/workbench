#ifndef __ANNOTATION_FILE_XML_WRITER_H__
#define __ANNOTATION_FILE_XML_WRITER_H__

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
class QXmlStreamWriter;

namespace caret {

    class Annotation;
    class AnnotationBox;
    class AnnotationCoordinate;
    class AnnotationFile;
    class AnnotationGroup;
    class AnnotationImage;
    class AnnotationLine;
    class AnnotationMultiPairedCoordinateShape;
    class AnnotationMultiCoordinateShape;
    class AnnotationPolygon;
    class AnnotationPolyhedron;
    class AnnotationPolyLine;
    class AnnotationTwoCoordinateShape;
    class AnnotationOval;
    class AnnotationText;
    class AnnotationOneCoordinateShape;
    class XmlStreamWriterHelper;

    class AnnotationFileXmlWriter : public AnnotationFileXmlFormatBase {
        
    public:
        AnnotationFileXmlWriter();
        
        virtual ~AnnotationFileXmlWriter();
        
        void writeFile(const AnnotationFile* annotationFile);
        
        void writeFileToString(const AnnotationFile* annotationFile,
                               const AString& fileNameForRelativePaths,
                               QString& fileContentString);
        
        // ADD_NEW_METHODS_HERE

    private:
        AnnotationFileXmlWriter(const AnnotationFileXmlWriter&);

        AnnotationFileXmlWriter& operator=(const AnnotationFileXmlWriter&);
        
        QString realToString(const float value) const;
        
        QString realArrayToString(const float values[],
                                  const int32_t numberOfValues) const;
        
        void getAnnotationPropertiesAsAttributes(const Annotation* annotation,
                                                 QXmlStreamAttributes& attributes);
        
        void getTwoDimAnnotationPropertiesAsAttributes(const AnnotationOneCoordinateShape* shape,
                                                 QXmlStreamAttributes& attributes);
        
        void writeFileContentToXmlStreamWriter(const AnnotationFile* annotationFile,
                                               const QString& filename);
        
        void writeBox(const AnnotationBox* box);
        
        void writeCoordinate(const AnnotationCoordinate* coordinate,
                             const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                             const QString& coordinateXmlElement);
        
        void writeGroup(const AnnotationGroup* group);
        
        void writeImage(const AnnotationImage* image);
        
        void writeLine(const AnnotationLine* line);
        
        void writePolyhedron(const AnnotationPolyhedron* polygonhedron);
        
        void writePolygon(const AnnotationPolygon* polygon);
        
        void writePolyLine(const AnnotationPolyLine* polyLine);
        
        void writeMultiPairedCoordinateShapeAnnotation(const AnnotationMultiPairedCoordinateShape* shape,
                                                       const QString& annotationXmlElement);

        void writeMultiCoordinateShapeAnnotation(const AnnotationMultiCoordinateShape* shape,
                                                 const QString& annotationXmlElement);
        
        void writeTwoCoordinateShapeAnnotation(const AnnotationTwoCoordinateShape* shape,
                                           const QString& annotationXmlElement);
        
        void writeOneCoordinateShapeAnnotation(const AnnotationOneCoordinateShape* shape,
                                           const QString& annotationXmlElement);
        
        void writeOval(const AnnotationOval* oval);
        
        void writeText(const AnnotationText* text);
        
        CaretPointer<QXmlStreamWriter> m_stream;
        
        CaretPointer<XmlStreamWriterHelper> m_streamHelper;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_XML_WRITER_DECLARE__
#endif // __ANNOTATION_FILE_XML_WRITER_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_XML_WRITER_H__
