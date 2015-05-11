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


#include "CaretObject.h"



namespace caret {

    class AnnotationFileXmlFormatBase : public CaretObject {

    protected:
        AnnotationFileXmlFormatBase();
        
        
    public:
        virtual ~AnnotationFileXmlFormatBase();
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        AnnotationFileXmlFormatBase(const AnnotationFileXmlFormatBase&);

        AnnotationFileXmlFormatBase& operator=(const AnnotationFileXmlFormatBase&);
        
    protected:
        static const QString ATTRIBUTE_BACKGROUND_CARET_COLOR;
        
        static const QString ATTRIBUTE_BACKGROUND_CUSTOM_RGBA;
        
        static const QString ATTRIBUTE_COORDINATE_SPACE;
        
        static const QString ATTRIBUTE_COORD_X;
        
        static const QString ATTRIBUTE_COORD_Y;
        
        static const QString ATTRIBUTE_COORD_Z;
        
        static const QString ATTRIBUTE_COORD_SURFACE_STRUCTURE;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES;
        
        static const QString ATTRIBUTE_COORD_SURFACE_NODE_INDEX;
        
        static const QString ATTRIBUTE_FOREGROUND_CARET_COLOR;
        
        static const QString ATTRIBUTE_FOREGROUND_CUSTOM_RGBA;
        
        static const QString ATTRIBUTE_FOREGROUND_LINE_WIDTH;
        
        static const QString ATTRIBUTE_HEIGHT;
        
        static const QString ATTRIBUTE_ROTATION_ANGLE;
        
        static const QString ATTRIBUTE_TAB_INDEX;
        
        static const QString ATTRIBUTE_TEXT_FONT_BOLD;
        
        static const QString ATTRIBUTE_TEXT_FONT_ITALIC;
        
        static const QString ATTRIBUTE_TEXT_FONT_NAME;
        
        static const QString ATTRIBUTE_TEXT_FONT_SIZE;
        
        static const QString ATTRIBUTE_TEXT_FONT_UNDERLINE;
        
        static const QString ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT;
        
        static const QString ATTRIBUTE_TEXT_ORIENTATION;
        
        static const QString ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT;
        
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ATTRIBUTE_WIDTH;
        
        static const QString ATTRIBUTE_WINDOW_INDEX;
        
        static const QString ELEMENT_ANNOTATION_FILE;
        
        static const QString ELEMENT_ARROW;
        
        static const QString ELEMENT_BOX;
        
        static const QString ELEMENT_COORDINATE_ONE;
        
        static const QString ELEMENT_COORDINATE_TWO;
        
        static const QString ELEMENT_IMAGE;
        
        static const QString ELEMENT_LINE;
        
        static const QString ELEMENT_OVAL;
        
        static const QString ELEMENT_TEXT;
        
        static const QString ELEMENT_TEXT_DATA;
        
        static const int32_t XML_VERSION_ONE;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_BACKGROUND_CARET_COLOR = "BackgroundCaretColor";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_BACKGROUND_CUSTOM_RGBA = "BackgroundCustomRGBA";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORDINATE_SPACE = "CoordinateSpace";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_X = "X";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_Y = "Y";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_Z = "Z";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_STRUCTURE = "Structure";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES = "NumberOfNodes";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_COORD_SURFACE_NODE_INDEX = "NodeIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_CARET_COLOR = "ForegroundCaretColor";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_CUSTOM_RGBA = "ForegroundCustomRGBA";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_FOREGROUND_LINE_WIDTH = "ForegroundLineWidth";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_HEIGHT = "Height";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_ROTATION_ANGLE = "RotationAngle";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TAB_INDEX = "TabIndex";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_BOLD = "FontBold";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_ITALIC = "FontItalic";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_NAME = "FontName";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_SIZE = "FontSize";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_FONT_UNDERLINE = "FontUnderline";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT = "HorizontalAlignment";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_ORIENTATION = "Orientation";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT = "VerticalAlignment";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_VERSION = "Version";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_WIDTH = "Width";
    
    const QString AnnotationFileXmlFormatBase::ATTRIBUTE_WINDOW_INDEX = "WindowIndex";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_ANNOTATION_FILE = "AnnotationFile";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_ARROW = "Arrow";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_BOX = "Box";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_ONE = "CoordOne";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_COORDINATE_TWO = "CoordTwo";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_IMAGE = "Image";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_LINE = "Line";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_OVAL = "Oval";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_TEXT = "Text";
    
    const QString AnnotationFileXmlFormatBase::ELEMENT_TEXT_DATA = "TextData";
    
    const int32_t AnnotationFileXmlFormatBase::XML_VERSION_ONE = 1;
    
#endif // __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_XML_FORMAT_BASE_H__
