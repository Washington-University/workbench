#ifndef __ANNOTATION_FILE_XML_FORMAT_BASE_H__
#define __ANNOTATION_FILE_XML_FORMAT_BASE_H__

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


#include <QDir>

#include "CaretObject.h"



namespace caret {

    class AnnotationFile;
    
    class AnnotationFileXmlFormatBase : public CaretObject {

    protected:
        AnnotationFileXmlFormatBase();
        
        void setAnnotationFileDirectory(const AString& annotationFileName);
        
        const QDir& getAnnotationFileDirectory() const;

    public:
        virtual ~AnnotationFileXmlFormatBase();
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        AnnotationFileXmlFormatBase(const AnnotationFileXmlFormatBase&);

        AnnotationFileXmlFormatBase& operator=(const AnnotationFileXmlFormatBase&);
        
        QDir m_annotationFileDirectory;
        
    protected:
        static const QString ATTRIBUTE_BACKGROUND_CARET_COLOR;
        
        static const QString ATTRIBUTE_BACKGROUND_CUSTOM_RGBA;
        
        static const QString ATTRIBUTE_COORDINATE_LIST_COUNT;
        
        static const QString ATTRIBUTE_COORDINATE_SPACE;
        
        static const QString ATTRIBUTE_COORD_X;
        
        static const QString ATTRIBUTE_COORD_Y;
        
        static const QString ATTRIBUTE_COORD_Z;
        
        static const QString ATTRIBUTE_COORD_SURFACE_STRUCTURE;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NODE_INDEX;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NODE_OFFSET;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NODE_OFFSET_VECTOR_TYPE;
        
        static const QString ATTRIBUTE_FOREGROUND_CARET_COLOR;
        
        static const QString ATTRIBUTE_FOREGROUND_CUSTOM_RGBA;
        
        static const QString ATTRIBUTE_FOREGROUND_LINE_WIDTH_PIXELS;
        
        static const QString ATTRIBUTE_FOREGROUND_LINE_WIDTH_PERCENTAGE;
        
        static const QString ATTRIBUTE_GROUP_TYPE;
        
        static const QString ATTRIBUTE_HEIGHT;
        
        static const QString ATTRIBUTE_IMAGE_WIDTH;
        
        static const QString ATTRIBUTE_IMAGE_HEIGHT;
        
        static const QString ATTRIBUTE_LINE_END_ARROW;
        
        static const QString ATTRIBUTE_LINE_START_ARROW;
        
        static const QString ATTRIBUTE_PLANE;
        
        static const QString ATTRIBUTE_ROTATION_ANGLE;
        
        static const QString ATTRIBUTE_SPACER_TAB_INDEX;
        
        static const QString ATTRIBUTE_TAB_INDEX;
        
        static const QString ATTRIBUTE_TAB_OR_WINDOW_INDEX;
        
        static const QString ATTRIBUTE_TEXT_CARET_COLOR;
        
        static const QString ATTRIBUTE_TEXT_CUSTOM_RGBA;
        
        static const QString ATTRIBUTE_TEXT_CONNECT_BRAINORDINATE;
        
        static const QString ATTRIBUTE_TEXT_FONT_BOLD;
        
        static const QString ATTRIBUTE_TEXT_FONT_ITALIC;
        
        static const QString ATTRIBUTE_TEXT_FONT_NAME;
        
        static const QString ATTRIBUTE_TEXT_FONT_POINT_SIZE;
        
        static const QString ATTRIBUTE_TEXT_FONT_PERCENT_VIEWPORT_SIZE;
        
        static const QString ATTRIBUTE_TEXT_FONT_UNDERLINE;
        
        static const QString ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT;
        
        static const QString ATTRIBUTE_TEXT_ORIENTATION;
        
        static const QString ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT;
        
        static const QString ATTRIBUTE_UNIQUE_KEY;
        
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ATTRIBUTE_WIDTH;
        
        static const QString ATTRIBUTE_WINDOW_INDEX;
        
        static const QString ELEMENT_ANNOTATION_FILE;
        
        static const QString ELEMENT_ARROW;
        
        static const QString ELEMENT_BOX;
        
        static const QString ELEMENT_COORDINATE;
        
        static const QString ELEMENT_COORDINATE_LIST;
        
        static const QString ELEMENT_COORDINATE_ONE;
        
        static const QString ELEMENT_COORDINATE_TWO;
        
        static const QString ELEMENT_COORDINATE_HISTOLOGY_SPACE_KEY;
        
        static const QString ELEMENT_COORDINATE_MEDIA_FILE_NAME;
        
        static const QString ELEMENT_FONT_ATTRIBUTES;
        
        static const QString ELEMENT_GROUP;
        
        static const QString ELEMENT_IMAGE;
        
        static const QString ELEMENT_IMAGE_RGBA_BYTES_IN_BASE64;
        
        static const QString ELEMENT_LINE;
        
        static const QString ELEMENT_OVAL;
        
        static const QString ELEMENT_PERCENT_SIZE_TEXT;
        
        static const QString ELEMENT_PERCENT_WIDTH_SIZE_TEXT;
        
        static const QString ELEMENT_POINT_SIZE_TEXT;
        
        static const QString ELEMENT_POLYHEDRON;

        static const QString ELEMENT_POLYHEDRON_DATA;
        
        static const QString ELEMENT_POLYGON;
        
        static const QString ELEMENT_POLY_LINE;
        
        static const QString ELEMENT_TEXT_OBSOLETE;
        
        static const QString ELEMENT_TEXT_DATA;
        
        static const int32_t XML_VERSION_ONE;
        
        static const int32_t XML_VERSION_TWO;
        
        static const int32_t XML_VERSION_THREE;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_BACKGROUND_CARET_COLOR = "backgroundCaretColor";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_BACKGROUND_CUSTOM_RGBA = "backgroundCustomRGBA";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORDINATE_SPACE = "coordinateSpace";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORDINATE_LIST_COUNT = "count";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_X = "x";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_Y = "y";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_Z = "z";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_STRUCTURE = "structure";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES = "numberOfNodes";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NODE_INDEX = "nodeIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NODE_OFFSET = "nodeOffset";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NODE_OFFSET_VECTOR_TYPE = "nodeOffsetVectorType";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_CARET_COLOR = "foregroundCaretColor";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_CUSTOM_RGBA = "foregroundCustomRGBA";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_LINE_WIDTH_PIXELS = "foregroundLineWidth";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_LINE_WIDTH_PERCENTAGE = "foregroundLineWidthPercentage";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_GROUP_TYPE = "groupType";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_HEIGHT = "height";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_IMAGE_WIDTH = "imageWidth";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_IMAGE_HEIGHT = "imageHeight";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_LINE_END_ARROW = "endArrow";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_LINE_START_ARROW = "startArrow";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_PLANE = "plane";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_ROTATION_ANGLE = "rotationAngle";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_SPACER_TAB_INDEX = "spacerTabIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TAB_INDEX = "tabIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TAB_OR_WINDOW_INDEX = "tabOrWindowIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_CARET_COLOR = "textCaretColor";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_CUSTOM_RGBA = "textCustomRGBA";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_CONNECT_BRAINORDINATE = "connectBrainordinate";

    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_BOLD = "fontBold";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_ITALIC = "fontItalic";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_NAME = "fontName";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_POINT_SIZE = "fontPointSize";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_PERCENT_VIEWPORT_SIZE = "fontPercentViewportSize";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_UNDERLINE = "fontUnderline";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT = "horizontalAlignment";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_ORIENTATION = "orientation";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT = "verticalAlignment";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_UNIQUE_KEY = "uniqueKey";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_VERSION = "version";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_WIDTH = "width";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_WINDOW_INDEX = "windowIndex";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_ANNOTATION_FILE = "AnnotationFile";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_ARROW = "arrow";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_BOX = "box";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE = "coord";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_LIST = "coordList";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_ONE = "coordOne";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_TWO = "coordTwo";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_HISTOLOGY_SPACE_KEY = "histologySpaceKey";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_MEDIA_FILE_NAME = "mediaFileName";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_FONT_ATTRIBUTES = "fontAttributes";

    const QString AnnotationFileXmlFormatBase::ELEMENT_GROUP = "group";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_IMAGE = "image";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_IMAGE_RGBA_BYTES_IN_BASE64 = "imageRgbaBytesInBase64";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_LINE = "line";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_OVAL = "oval";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_PERCENT_SIZE_TEXT = "percentSizeText";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_PERCENT_WIDTH_SIZE_TEXT = "percentWidthSizeText";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_POINT_SIZE_TEXT = "pointSizeText";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_POLYHEDRON = "polyhedron";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_POLYHEDRON_DATA = "polyhedronData";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_POLYGON = "polygon";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_POLY_LINE = "polyLine";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_TEXT_OBSOLETE = "text";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_TEXT_DATA = "textData";
    
    const int32_t AnnotationFileXmlFormatBase::XML_VERSION_ONE = 1;
    
    const int32_t AnnotationFileXmlFormatBase::XML_VERSION_TWO = 2;
    
    const int32_t AnnotationFileXmlFormatBase::XML_VERSION_THREE = 3;
    
#endif // __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_XML_FORMAT_BASE_H__
