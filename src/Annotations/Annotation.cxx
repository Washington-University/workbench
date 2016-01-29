
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

#define __ANNOTATION_DECLARE__
#include "Annotation.h"
#undef __ANNOTATION_DECLARE__

#include "AnnotationBox.h"
#include "AnnotationColorBar.h"
#include "AnnotationCoordinate.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "AnnotationText.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "StructureEnum.h"

using namespace caret;


    
/**
 * \class caret::Annotation 
 * \brief Abstract class for annotations.
 * \ingroup Annotations
 */

/**
 * Constructor for an annotation.
 * 
 * @param drawingType
 *    Type of annotation for drawing.
 * @param attributeDefaultType
 *    Default type for annotation attributes.
 */
Annotation::Annotation(const AnnotationTypeEnum::Enum type,
                       const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: CaretObjectTracksModification(),
m_type(type),
m_attributeDefaultType(attributeDefaultType)
{
    initializeAnnotationMembers();
}

/**
 * Destructor.
 */
Annotation::~Annotation()
{
    //std::cout << "Deleting annotation of type: " << qPrintable(AnnotationTypeEnum::toGuiName(m_type)) << std::endl;
    
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Annotation::Annotation(const Annotation& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj),
m_type(obj.m_type),
m_attributeDefaultType(obj.m_attributeDefaultType)
{
    initializeAnnotationMembers();
    this->copyHelperAnnotation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Annotation&
Annotation::operator=(const Annotation& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotation(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Annotation::copyHelperAnnotation(const Annotation& obj)
{
    m_coordinateSpace     = obj.m_coordinateSpace;
    m_tabIndex            = obj.m_tabIndex;
    m_windowIndex         = obj.m_windowIndex;
    m_foregroundLineWidth = obj.m_foregroundLineWidth;
    m_colorForeground     = obj.m_colorForeground;
    m_colorBackground     = obj.m_colorBackground;
    m_customColorBackground[0]  = obj.m_customColorBackground[0];
    m_customColorBackground[1]  = obj.m_customColorBackground[1];
    m_customColorBackground[2]  = obj.m_customColorBackground[2];
    m_customColorBackground[3]  = obj.m_customColorBackground[3];
    m_customColorForeground[0]  = obj.m_customColorForeground[0];
    m_customColorForeground[1]  = obj.m_customColorForeground[1];
    m_customColorForeground[2]  = obj.m_customColorForeground[2];
    m_customColorForeground[3]  = obj.m_customColorForeground[3];

    resetGroupAndNameHierarchyItem();
    
    /*
     * Selected status is NOT copied.
     */
    m_selectedInWindowFlag.reset();
}

/**
 * @return An identical copy (clone) of "this" annotation.
 *    The type of the annotation is used to ensure the 
 *    correct type of annotation is created and returned.
 */
Annotation*
Annotation::clone() const
{
    Annotation* myClone = NULL;
    
    switch (getType()) {
        case AnnotationTypeEnum::BOX:
        {
            const AnnotationBox* box = dynamic_cast<const AnnotationBox*>(this);
            CaretAssert(box);
            myClone = new AnnotationBox(*box);
        }
            break;
        case AnnotationTypeEnum::COLOR_BAR:
        {
            const AnnotationColorBar* colorBar = dynamic_cast<const AnnotationColorBar*>(this);
            CaretAssert(colorBar);
            myClone = new AnnotationColorBar(*colorBar);
        }
            break;
        case AnnotationTypeEnum::IMAGE:
        {
            const AnnotationImage* image = dynamic_cast<const AnnotationImage*>(this);
            CaretAssert(image);
            myClone = new AnnotationImage(*image);
        }
            break;
        case AnnotationTypeEnum::LINE:
        {
            const AnnotationLine* line = dynamic_cast<const AnnotationLine*>(this);
            CaretAssert(line);
            myClone = new AnnotationLine(*line);
        }
            break;
        case AnnotationTypeEnum::OVAL:
        {
            const AnnotationOval* oval = dynamic_cast<const AnnotationOval*>(this);
            CaretAssert(oval);
            myClone = new AnnotationOval(*oval);
        }
            break;
        case AnnotationTypeEnum::TEXT:
        {
            const AnnotationText* text = dynamic_cast<const AnnotationText*>(this);
            CaretAssert(text);
            
            switch (text->getFontSizeType()) {
                case AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT:
                {
                    const AnnotationPercentSizeText* pctText = dynamic_cast<const AnnotationPercentSizeText*>(text);
                    CaretAssert(pctText);
                    myClone = new AnnotationPercentSizeText(*pctText);
                }
                    break;
                case AnnotationTextFontSizeTypeEnum::POINTS:
                {
                    const AnnotationPointSizeText* pointText = dynamic_cast<const AnnotationPointSizeText*>(text);
                    CaretAssert(pointText);
                    myClone = new AnnotationPointSizeText(*pointText);
                }
            }
        }
            break;
    }
    
    CaretAssert(myClone);
    
    return myClone;
}

/**
 * Replace "this" annotation with content of the given annotation.
 * The annotation must by the same type and class.
 *
 * @param annotation
 *     Annotation whose content is copied to "this" annotation.
 */
void
Annotation::replaceWithCopyOfAnnotation(const Annotation* annotation)
{
    CaretAssert(annotation);
    
    const AnnotationTypeEnum::Enum myType = getType();
    
    if (myType != annotation->getType()) {
        CaretLogSevere("Attempting to replace  "
                       + AnnotationTypeEnum::toGuiName(myType)
                       + " with annotation of different type: "
                       + AnnotationTypeEnum::toGuiName(annotation->getType()));
        return;
    }
    
    *this = *annotation;
}

/**
 * @return Is this annotation deletable?  This method may be overridden
 * by annotations (such as colorbars) that cannot be deleted.
 */
bool
Annotation::isDeletable() const
{
    return true;
}

/**
 * @return Is this annotation requiring that it be kept in a fixed 
 * aspect ratio?  By default, this is false.  This method may be 
 * overridden by annotations that require a fixed aspect ratio
 * (such as an image annotaiton).
 */
bool
Annotation::isFixedAspectRatio() const
{
    return false;
}

/**
 * @return The aspect ratio for annotations that have a fixed aspect ratio.
 * This method may be overridden by annotations that require a fixed aspect ratio
 * (such as an image annotaiton).
 *
 * If the aspect ratio is unknown return 1.  Never return zero.
 */
float
Annotation::getFixedAspectRatio() const
{
    return 1.0;
}

/**
 * Apply coloring including foreground line width from annother annotation.
 *
 * @param otherAnnotation
 *     Other annotation from which coloring is copied.
 */
void
Annotation::applyColoringFromOther(const Annotation* otherAnnotation)
{
    CaretAssert(otherAnnotation);

    m_colorBackground     = otherAnnotation->m_colorBackground;
    m_colorForeground     = otherAnnotation->m_colorForeground;
    m_foregroundLineWidth = otherAnnotation->m_foregroundLineWidth;
    
    for (int32_t i = 0; i < 4; i++) {
        m_customColorBackground[i] = otherAnnotation->m_customColorBackground[i];
        m_customColorForeground[i] = otherAnnotation->m_customColorForeground[i];
    }
}

/**
 * Factory method for creating an annotation of the given type.
 *
 * @param annotationType
 *     Type of annotation that will be created.
 * @param attributeDefaultType
 *    Default type for annotation attributes.
 * @return
 *     New annotation of the given type.
 */
Annotation*
Annotation::newAnnotationOfType(const AnnotationTypeEnum::Enum annotationType,
                                const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
{
    Annotation* annotation = NULL;
    
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            annotation = new AnnotationBox(attributeDefaultType);
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            annotation = new AnnotationColorBar(attributeDefaultType);
            break;
        case AnnotationTypeEnum::IMAGE:
            annotation = new AnnotationImage(attributeDefaultType);
            break;
        case AnnotationTypeEnum::LINE:
            annotation = new AnnotationLine(attributeDefaultType);
            break;
        case AnnotationTypeEnum::OVAL:
            annotation = new AnnotationOval(attributeDefaultType);
            break;
        case AnnotationTypeEnum::TEXT:
            annotation = new AnnotationPercentSizeText(attributeDefaultType);
            break;
    }
    
    return annotation;
}

/**
 * Get the unique identifier for this annotation.
 */
int32_t
Annotation::getUniqueIdentifier() const
{
    return m_uniqueIdentifier;
}

/**
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    CaretAssertMessage((m_selectedInWindowFlag.size() == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS),
                       ("m_selectedInWindowFlag (size="
                        + QString::number(m_selectedInWindowFlag.size())
                        + ") must be the same size as BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS (size="
                        + QString::number(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)
                        + ")"));

    m_selectedInWindowFlag.reset();
    
    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    if (s_uniqueIdentifierGenerator == std::numeric_limits<int32_t>::max()) {
        s_uniqueIdentifierGenerator = 0;
    }
    s_uniqueIdentifierGenerator++;
    
    m_uniqueIdentifier = s_uniqueIdentifierGenerator;
    m_tabIndex    = -1;
    m_windowIndex = -1;
    
    resetGroupAndNameHierarchyItem();
    
    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_foregroundLineWidth = 3.0;
            
            m_colorBackground = CaretColorEnum::NONE;
            m_colorForeground = CaretColorEnum::WHITE;
            
            m_customColorBackground[0]  = 0.0;
            m_customColorBackground[1]  = 0.0;
            m_customColorBackground[2]  = 0.0;
            m_customColorBackground[3]  = 1.0;
            
            m_customColorForeground[0]  = 1.0;
            m_customColorForeground[1]  = 1.0;
            m_customColorForeground[2]  = 1.0;
            m_customColorForeground[3]  = 1.0;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            m_foregroundLineWidth = s_userDefaultForegroundLineWidth;
            
            m_colorBackground = s_userDefaultColorBackground;
            m_colorForeground = s_userDefaultColorForeground;
            
            m_customColorBackground[0]  = s_userDefaultCustomColorBackground[0];
            m_customColorBackground[1]  = s_userDefaultCustomColorBackground[1];
            m_customColorBackground[2]  = s_userDefaultCustomColorBackground[2];
            m_customColorBackground[3]  = s_userDefaultCustomColorBackground[3];
            
            m_customColorForeground[0]  = s_userDefaultCustomColorForeground[0];
            m_customColorForeground[1]  = s_userDefaultCustomColorForeground[1];
            m_customColorForeground[2]  = s_userDefaultCustomColorForeground[2];
            m_customColorForeground[3]  = s_userDefaultCustomColorForeground[3];
            break;
    }
    
    
    /*
     * May need to override colors if both are none
     */
    if ((m_colorBackground == CaretColorEnum::NONE)
        && (m_colorForeground == CaretColorEnum::NONE)) {
        m_colorForeground = CaretColorEnum::WHITE;
    }
    
    /*
     * Don't allow a foregound color of NONE for text or line
     */
    bool disallowForegroundNoneFlag = false;
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            disallowForegroundNoneFlag = true;
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            disallowForegroundNoneFlag = true;
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
            disallowForegroundNoneFlag = true;
            break;
    }
    if (disallowForegroundNoneFlag) {
        if (m_colorForeground == CaretColorEnum::NONE) {
            m_colorForeground = CaretColorEnum::WHITE;
            
            if (m_colorBackground == CaretColorEnum::WHITE) {
                m_colorBackground = CaretColorEnum::NONE;
            }
        }
    }
    
    /*
     * Note: The 'const' members are not saved to the scene as they 
     * are set by constructor.
     *
     * The 'selected' status is not saved to the scene.
     */
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<AnnotationCoordinateSpaceEnum>("m_coordinateSpace",
                                                         &m_coordinateSpace);
    m_sceneAssistant->add("m_tabIndex",
                          &m_tabIndex);
    m_sceneAssistant->add("m_windowIndex",
                          &m_windowIndex);
    m_sceneAssistant->add("m_foregroundLineWidth",
                          &m_foregroundLineWidth);
    m_sceneAssistant->addArray("m_customColorBackground",
                               m_customColorBackground, 4, 0.0);
    m_sceneAssistant->addArray("m_customColorForeground",
                               m_customColorForeground, 4, 1.0);
}


/**
 * @return The annotation drawing type.
 */
AnnotationTypeEnum::Enum
Annotation::getType() const
{
    return m_type;
}

QString
Annotation::getShortDescriptiveString() const
{
    QString s = AnnotationTypeEnum::toGuiName(m_type);
    
    if (m_type == AnnotationTypeEnum::TEXT) {
        const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
        s += (" \"" + textAnn->getText() + "\"");
    }
    
    return s;
}


/**
 * @return The coordinate space.
 */
AnnotationCoordinateSpaceEnum::Enum
Annotation::getCoordinateSpace() const
{
    return m_coordinateSpace;
}

/**
 * Set the coordinate space.
 *
 * @param coordinateSpace
 *     New value for coordinate space.
 */
void
Annotation::setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace)
{
    if (m_coordinateSpace != coordinateSpace) {
        m_coordinateSpace = coordinateSpace;
        resetGroupAndNameHierarchyItem();
        setModified();
    }
}

/**
 * @return The tab index.  Valid only for tab coordinate space annotations.
 */
int32_t
Annotation::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set tab index.  Valid only for tab coordinate space annotations.
 *
 * @param tabIndex
 */
void
Annotation::setTabIndex(const int32_t tabIndex)
{
    if (tabIndex != m_tabIndex) {
        m_tabIndex = tabIndex;
        resetGroupAndNameHierarchyItem();
        setModified();
    }
}

/**
 * @return The window index.  Valid only for window coordinate space annotations.
 */
int32_t
Annotation::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Set window index.  Valid only for window coordinate space annotations.
 *
 * @param tabIndex
 */
void
Annotation::setWindowIndex(const int32_t windowIndex)
{
    if (windowIndex != m_windowIndex) {
        m_windowIndex = windowIndex;
        resetGroupAndNameHierarchyItem();
        setModified();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
Annotation::toString() const
{
    AString msg("Annotation type="
                      + AnnotationTypeEnum::toName(m_type));
    
    msg += (" space="
            + AnnotationCoordinateSpaceEnum::toGuiName(getCoordinateSpace()));
    
    const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
    if (textAnn != NULL) {
        msg += (" text=" + textAnn->getText());
    }
    return msg;
}

/**
 * @return Name of annotation for features toolbox class/name hierarchy
 */
AString
Annotation::getNameForHierarchy() const
{
    AString name;

    /*
     * If in surface space, name begins with vertex number(s)
     */
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::SURFACE) {
        const AnnotationCoordinate* coordOne = NULL;
        const AnnotationCoordinate* coordTwo = NULL;
        
        const AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<const AnnotationTwoDimensionalShape*>(this);
        if (twoDimShape != NULL) {
            coordOne = twoDimShape->getCoordinate();
        }
        else {
            const AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(this);
            if (oneDimShape != NULL) {
                coordOne = oneDimShape->getStartCoordinate();
                coordTwo = oneDimShape->getEndCoordinate();
            }
        }
        
        int32_t firstVertexIndex = -1;
        if (coordOne != NULL) {
            StructureEnum::Enum firstStructure = StructureEnum::INVALID;
            int32_t numVertices = -1;
            coordOne->getSurfaceSpace(firstStructure, numVertices, firstVertexIndex);
        }
        
        int32_t secondVertexIndex = -1;
        if (coordTwo != NULL) {
            StructureEnum::Enum secondStructure = StructureEnum::INVALID;
            int32_t numVertices = -1;
            coordTwo->getSurfaceSpace(secondStructure, numVertices, secondVertexIndex);
        }
        
        if ((firstVertexIndex >= 0)
            && (secondVertexIndex >= 0)) {
            name.append(" Vertices "
                        + AString::number(firstVertexIndex)
                        + ", "
                        + AString::number(secondVertexIndex));
        }
        else if (firstVertexIndex >= 0) {
            name.append(" Vertex "
                        + AString::number(firstVertexIndex));
        }

        name.append(": ");
    }
    
    if (m_type == AnnotationTypeEnum::TEXT) {
        const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
        QString textChars = textAnn->getText().replace("\n", " "); // remove newlines
        if (textChars.isEmpty()) {
            textChars = "Text Missing !";
        }
        name.append(textChars);
    }
    else {
        name.append(AnnotationTypeEnum::toGuiName(m_type));
    }
    return name;
}

/**
 * @return Name of annotation space for features toolbox class/name hierarchy
 */
AString
Annotation::getClassNameForHierarchy() const
{
    AString name = ("SPACE: "
                    + AnnotationCoordinateSpaceEnum::toGuiName(m_coordinateSpace));
    
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
        {
            const AnnotationCoordinate* coordOne = NULL;
            const AnnotationCoordinate* coordTwo = NULL;
            
            const AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<const AnnotationTwoDimensionalShape*>(this);
            if (twoDimShape != NULL) {
                coordOne = twoDimShape->getCoordinate();
            }
            else {
                const AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(this);
                if (oneDimShape != NULL) {
                    coordOne = oneDimShape->getStartCoordinate();
                    coordTwo = oneDimShape->getEndCoordinate();
                }
            }
            
            StructureEnum::Enum firstStructure = StructureEnum::INVALID;
            if (coordOne != NULL) {
                firstStructure = coordOne->getSurfaceStructure();
            }
            
            StructureEnum::Enum secondStructure = StructureEnum::INVALID;
            if (coordTwo != NULL) {
                secondStructure = coordTwo->getSurfaceStructure();
            }
            
            name += (" "
                    + StructureEnum::toGuiName(firstStructure));
            if (secondStructure != StructureEnum::INVALID) {
                if (secondStructure != firstStructure) {
                    name += (" " + StructureEnum::toGuiName(secondStructure));
                }
            }
        }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            name += (" " + QString::number(m_tabIndex + 1));
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            name += (" " + QString::number(m_windowIndex + 1));
            break;
    }
    
    return name;
}

/**
 * @return The foreground line width.
 */
float
Annotation::getForegroundLineWidth() const
{
    return m_foregroundLineWidth;
}

/**
 * Set the foreground line width.
 *
 * @param lineWidth
 *    New value for foreground line width.
 */
void
Annotation::setForegroundLineWidth(const float lineWidth)
{
    if (lineWidth != m_foregroundLineWidth) {
        m_foregroundLineWidth = lineWidth;
        setModified();
    }
}

/**
 * @return Is foreground line width supported?
 * Most annotations support a foreground line width.
 * Annotations that do not support a foreground line width
 * must override this method and return a value of false.
 */
bool
Annotation::isForegroundLineWidthSupported() const
{
    return true;
}

/**
 * @return Is background color supported?
 * Most annotations support a background color.
 * Annotations that do not support a background color
 * must override this method and return a value of false.
 */
bool
Annotation::isBackgroundColorSupported() const
{
    return true;
}


/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
Annotation::getForegroundColor() const
{
    return m_colorForeground;
}

/**
 * Set the foreground color.
 *
 * @param color
 *     New value for foreground color.
 */
void
Annotation::setForegroundColor(const CaretColorEnum::Enum color)
{
    if (m_colorForeground != color) {
        m_colorForeground = color;
        setModified();
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
Annotation::getForegroundColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorForeground) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomForegroundColor(rgbaOut);
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBFloat(m_colorForeground,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
Annotation::getForegroundColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getForegroundColorRGBA(rgbaFloat);
    
    rgbaOut[0] = static_cast<uint8_t>(rgbaFloat[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(rgbaFloat[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(rgbaFloat[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(rgbaFloat[3] * 255.0);
}

/**
 * @return The background color.
 */
CaretColorEnum::Enum
Annotation::getBackgroundColor() const
{
    return m_colorBackground;
}

/**
 * Set the background color.
 *
 * @param color
 *     New value for background color.
 */
void
Annotation::setBackgroundColor(const CaretColorEnum::Enum color)
{
    if (m_colorBackground != color) {
        m_colorBackground = color;
        setModified();
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
Annotation::getBackgroundColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorBackground) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomBackgroundColor(rgbaOut);
            break;
        case CaretColorEnum::AQUA:
        case CaretColorEnum::BLACK:
        case CaretColorEnum::BLUE:
        case CaretColorEnum::FUCHSIA:
        case CaretColorEnum::GRAY:
        case CaretColorEnum::GREEN:
        case CaretColorEnum::LIME:
        case CaretColorEnum::MAROON:
        case CaretColorEnum::NAVY:
        case CaretColorEnum::OLIVE:
        case CaretColorEnum::PURPLE:
        case CaretColorEnum::RED:
        case CaretColorEnum::SILVER:
        case CaretColorEnum::TEAL:
        case CaretColorEnum::WHITE:
        case CaretColorEnum::YELLOW:
            CaretColorEnum::toRGBFloat(m_colorBackground,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
Annotation::getBackgroundColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getBackgroundColorRGBA(rgbaFloat);
    
    rgbaOut[0] = static_cast<uint8_t>(rgbaFloat[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(rgbaFloat[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(rgbaFloat[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(rgbaFloat[3] * 255.0);
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getCustomForegroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorForeground[0];
    rgbaOut[1] = m_customColorForeground[1];
    rgbaOut[2] = m_customColorForeground[2];
    rgbaOut[3] = m_customColorForeground[3];
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getCustomForegroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorForeground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorForeground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorForeground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorForeground[3] * 255.0);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setCustomForegroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorForeground[i]) {
            m_customColorForeground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setCustomForegroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorForeground[i]) {
            m_customColorForeground[i] = component;
            setModified();
        }
    }
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getCustomBackgroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorBackground[0];
    rgbaOut[1] = m_customColorBackground[1];
    rgbaOut[2] = m_customColorBackground[2];
    rgbaOut[3] = m_customColorBackground[3];
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getCustomBackgroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorBackground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorBackground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorBackground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorBackground[3] * 255.0);
}

/**
 * Set the background color with floats.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setCustomBackgroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorBackground[i]) {
            m_customColorBackground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the background color with bytes.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setCustomBackgroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorBackground[i]) {
            m_customColorBackground[i] = component;
            setModified();
        }
    }
}

/**
 * @return The annotation's selected status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 *
 * @param windowIndex
 *    Window for annotation selection status.
 */
bool
Annotation::isSelected(const int32_t windowIndex) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < static_cast<int32_t>(m_selectedInWindowFlag.size())));
    return m_selectedInWindowFlag.test(windowIndex);
}

/**
 * Set the annotation's selected status.
 *
 * This method is private - AnnotationManager handles selection and allowing
 * public access to this method could cause improper selection status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 *
 * @param windowIndex
 *    Window for annotation selection.
 * @param selectedStatus
 *    New selection status.
 */
void
Annotation::setSelected(const int32_t windowIndex,
                        const bool selectedStatus) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < static_cast<int32_t>(m_selectedInWindowFlag.size())));
    
    if (selectedStatus) {
        m_selectedInWindowFlag.set(windowIndex);
    }
    else {
        m_selectedInWindowFlag.reset(windowIndex);
    }
}

/**
 * Set the annotation's selected status to deselected.
 */
void
Annotation::setDeselected()
{
    /*
     * Clear selected status in ALL windows
     */
    m_selectedInWindowFlag.reset();
}

/**
 * Convert a relative (zero to one) XYZ coordinate to
 * a viewport coordinate.
 *
 * @param relativeXYZ
 *     The relative (zero to one) XYZ.
 * @param viewportWidth
 *     Width of the viewport.
 * @param viewportHeight
 *     Height of the viewport.
 * @param viewportXYZOut
 *     Output viewport coordinate.
 */
void
Annotation::relativeXYZToViewportXYZ(const float relativeXYZ[3],
                                     const float viewportWidth,
                                     const float viewportHeight,
                                     float viewportXYZOut[3])
{
    viewportXYZOut[0] = (relativeXYZ[0] / 100.0) * viewportWidth;
    viewportXYZOut[1] = (relativeXYZ[1] / 100.0) * viewportHeight;
    viewportXYZOut[2] = relativeXYZ[2];
}

/**
 * Convert a viewport XYZ coordinate to
 * a relative coordinate.  The output relative
 * coordinates are may be outside of the range
 * zero to one if viewport coordinate is outside
 * of the viewport bounds.
 *
 * @param viewportXYZ
 *     The relative (zero to one) XYZ.
 * @param viewportWidth
 *     Width of the viewport.
 * @param viewportHeight
 *     Height of the viewport.
 * @param relativeXYZOut
 *     Output relative coordinate.
 */
void
Annotation::viewportXYZToRelativeXYZ(const float viewportXYZ[3],
                                     const float viewportWidth,
                                     const float viewportHeight,
                                     float relativeXYZOut[3])
{
    relativeXYZOut[0] = 100.0 * (viewportXYZ[0] / viewportWidth);
    relativeXYZOut[1] = 100.0 * (viewportXYZ[1] / viewportHeight);
    relativeXYZOut[2] = viewportXYZ[2];
}

/**
 * Convert a viewport XYZ coordinate to
 * a relative coordinate.  The output relative coordinate
 * will be limited to the range zero to one even if the
 * viewport coordinate is outside of the viewport bounds.
 *
 * @param viewportXYZ
 *     The relative (zero to one) XYZ.
 * @param viewportWidth
 *     Width of the viewport.
 * @param viewportHeight
 *     Height of the viewport.
 * @param relativeXYZOut
 *     Output relative coordinate.
 */
void
Annotation::viewportXYZToLimitedRelativeXYZ(const float viewportXYZ[3],
                                     const float viewportWidth,
                                     const float viewportHeight,
                                     float relativeXYZOut[3])
{
    viewportXYZToRelativeXYZ(viewportXYZ,
                             viewportWidth,
                             viewportHeight,
                             relativeXYZOut);
    
    relativeXYZOut[0] = MathFunctions::limitRange(relativeXYZOut[0], 0.0f, 1.0f);
    relativeXYZOut[1] = MathFunctions::limitRange(relativeXYZOut[1], 0.0f, 1.0f);
    relativeXYZOut[2] = MathFunctions::limitRange(relativeXYZOut[2], 0.0f, 1.0f);
}


/**
 * Set the selection item for the group/name hierarchy.
 *
 * @param item
 *     The selection item from the group/name hierarchy.
 */
void
Annotation::setGroupNameSelectionItem(GroupAndNameHierarchyItem* item)
{
    m_groupNameSelectionItem = item;
}

/**
 * @return The selection item for the Group/Name selection hierarchy.
 *      May be NULL in some circumstances.
 */
const GroupAndNameHierarchyItem*
Annotation::getGroupNameSelectionItem() const
{
    return m_groupNameSelectionItem;
}

/**
 * Reset the group and name hierarchy so that
 * it is recreated (when needed).  The hierarchy
 * group is the stereotaxic space so any time the
 * space (type, window, tab) changes, this must
 * be reset.
 */
void
Annotation::resetGroupAndNameHierarchyItem()
{
    m_groupNameSelectionItem = NULL;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
Annotation::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Annotation",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
Annotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
    
}

/**
 * Set the default value for foreground color
 *
 * @param color
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultForegroundColor(const CaretColorEnum::Enum color)
{
    s_userDefaultColorForeground = color;
}

/**
 * Set the default value for background color
 *
 * @param color
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultBackgroundColor(const CaretColorEnum::Enum color)
{
    s_userDefaultColorBackground = color;
}

/**
 * Set the default value for custom foreground color
 *
 * @param rgba
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultCustomForegroundColor(const float rgba[4])
{
    s_userDefaultCustomColorForeground[0] = rgba[0];
    s_userDefaultCustomColorForeground[1] = rgba[1];
    s_userDefaultCustomColorForeground[2] = rgba[2];
    s_userDefaultCustomColorForeground[3] = rgba[3];
}

/**
 * Set the default value for custom background color
 *
 * @param rgba
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultCustomBackgroundColor(const float rgba[4])
{
    s_userDefaultCustomColorBackground[0] = rgba[0];
    s_userDefaultCustomColorBackground[1] = rgba[1];
    s_userDefaultCustomColorBackground[2] = rgba[2];
    s_userDefaultCustomColorBackground[3] = rgba[3];
}

/**
 * Set the default value for foreground line width
 *
 * @param lineWidth
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultForegroundLineWidth(const float lineWidth)
{
    s_userDefaultForegroundLineWidth = lineWidth;
}
