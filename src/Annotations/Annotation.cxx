
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
#include "AnnotationGroup.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "AnnotationText.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayGroupAndTabItemHelper.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

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
DisplayGroupAndTabItemInterface(),
SceneableInterface(),
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
    delete m_displayGroupAndTabItemHelper;
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Annotation::Annotation(const Annotation& obj)
: CaretObjectTracksModification(obj),
DisplayGroupAndTabItemInterface(obj),
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
    m_annotationGroupKey.reset();
    m_uniqueKey           = -1;
    m_coordinateSpace     = obj.m_coordinateSpace;
    m_tabIndex            = obj.m_tabIndex;
    m_windowIndex         = obj.m_windowIndex;
    m_viewportCoordinateSpaceViewport[0] = obj.m_viewportCoordinateSpaceViewport[0];
    m_viewportCoordinateSpaceViewport[1] = obj.m_viewportCoordinateSpaceViewport[1];
    m_viewportCoordinateSpaceViewport[2] = obj.m_viewportCoordinateSpaceViewport[2];
    m_viewportCoordinateSpaceViewport[3] = obj.m_viewportCoordinateSpaceViewport[3];
    m_lineWidthPixels = obj.m_lineWidthPixels;
    m_lineWidthPercentage = obj.m_lineWidthPercentage;
    m_colorLine           = obj.m_colorLine;
    m_colorBackground     = obj.m_colorBackground;
    m_customColorBackground[0]  = obj.m_customColorBackground[0];
    m_customColorBackground[1]  = obj.m_customColorBackground[1];
    m_customColorBackground[2]  = obj.m_customColorBackground[2];
    m_customColorBackground[3]  = obj.m_customColorBackground[3];
    m_customColorLine[0]  = obj.m_customColorLine[0];
    m_customColorLine[1]  = obj.m_customColorLine[1];
    m_customColorLine[2]  = obj.m_customColorLine[2];
    m_customColorLine[3]  = obj.m_customColorLine[3];

    m_properties = obj.m_properties;
    
    *m_displayGroupAndTabItemHelper = *obj.m_displayGroupAndTabItemHelper;
    
    /*
     * Initializes unique name
     */
    m_uniqueKey = -1;
    m_name = "";
    //setUniqueKey(m_uniqueKey); calling this will cause a crash since we could be in copy constructor and subclass has not been constructed
    
    /*
     * Selected status is NOT copied.
     */
    m_selectedForEditingInWindowFlag.reset();
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
                case AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_WIDTH:
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
 * Set this instance modified.
 */
void
Annotation::setModified()
{
    /*
     * While this method does not need to be overridden, 
     * doing so enables debugging of invalid modification status.
     */
    CaretObjectTracksModification::setModified();
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
 * Apply coloring including line width from annother annotation.
 *
 * @param otherAnnotation
 *     Other annotation from which coloring is copied.
 */
void
Annotation::applyColoringFromOther(const Annotation* otherAnnotation)
{
    CaretAssert(otherAnnotation);

    m_colorBackground     = otherAnnotation->m_colorBackground;
    m_colorLine     = otherAnnotation->m_colorLine;
    m_lineWidthPixels = otherAnnotation->m_lineWidthPixels;
    m_lineWidthPercentage = otherAnnotation->m_lineWidthPercentage;
    
    for (int32_t i = 0; i < 4; i++) {
        m_customColorBackground[i] = otherAnnotation->m_customColorBackground[i];
        m_customColorLine[i] = otherAnnotation->m_customColorLine[i];
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
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    CaretAssertMessage((m_selectedForEditingInWindowFlag.size() == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS),
                       ("m_selectedInWindowFlag (size="
                        + QString::number(m_selectedForEditingInWindowFlag.size())
                        + ") must be the same size as BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS (size="
                        + QString::number(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)
                        + ")"));

    m_selectedForEditingInWindowFlag.reset();
    
    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    m_tabIndex    = -1;
    m_windowIndex = -1;
    m_viewportCoordinateSpaceViewport[0] = 0;
    m_viewportCoordinateSpaceViewport[1] = 0;
    m_viewportCoordinateSpaceViewport[2] = 0;
    m_viewportCoordinateSpaceViewport[3] = 0;
    
    m_displayGroupAndTabItemHelper = new DisplayGroupAndTabItemHelper();
    
    /*
     * Default the unique identifier.
     *
     * NOTE: do not call 'setUniqueIdentifier()'
     * from here as the
     * type of annotation has not been set
     * and may cause a crash.
     */
    m_uniqueKey = -1;

    m_annotationGroupKey.reset();
    
    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_lineWidthPixels = 3.0;
            m_lineWidthPercentage = 1.0;
            
            m_colorBackground = CaretColorEnum::NONE;
            m_colorLine = CaretColorEnum::WHITE;
            
            switch (m_type) {
                case AnnotationTypeEnum::BOX:
                    break;
                case AnnotationTypeEnum::COLOR_BAR:
                    m_colorBackground = CaretColorEnum::BLACK;
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    break;
                case AnnotationTypeEnum::OVAL:
                    break;
                case AnnotationTypeEnum::TEXT:
                    m_colorBackground = CaretColorEnum::NONE;
                    m_colorLine = CaretColorEnum::NONE;
                    break;
            }
            
            m_customColorBackground[0]  = 0.0;
            m_customColorBackground[1]  = 0.0;
            m_customColorBackground[2]  = 0.0;
            m_customColorBackground[3]  = 1.0;
            
            m_customColorLine[0]  = 1.0;
            m_customColorLine[1]  = 1.0;
            m_customColorLine[2]  = 1.0;
            m_customColorLine[3]  = 1.0;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
        {
            m_lineWidthPixels = s_userDefaultLineWidthPixelsObsolete;
            m_lineWidthPercentage = s_userDefaultLineWidthPercentage;
            
            m_colorBackground = s_userDefaultColorBackground;
            m_colorLine = s_userDefaultColorLine;
            
            m_customColorBackground[0]  = s_userDefaultCustomColorBackground[0];
            m_customColorBackground[1]  = s_userDefaultCustomColorBackground[1];
            m_customColorBackground[2]  = s_userDefaultCustomColorBackground[2];
            m_customColorBackground[3]  = s_userDefaultCustomColorBackground[3];
            
            m_customColorLine[0]  = s_userDefaultCustomColorLine[0];
            m_customColorLine[1]  = s_userDefaultCustomColorLine[1];
            m_customColorLine[2]  = s_userDefaultCustomColorLine[2];
            m_customColorLine[3]  = s_userDefaultCustomColorLine[3];
            
            const bool lineBackNoneFlag = ((m_colorLine == CaretColorEnum::NONE)
                                           && (m_colorBackground == CaretColorEnum::NONE));
            const CaretColorEnum::Enum defaultColor = CaretColorEnum::RED;
            switch (m_type) {
                case AnnotationTypeEnum::BOX:
                    if (lineBackNoneFlag) {
                        m_colorBackground = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::COLOR_BAR:
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    if (m_colorLine == CaretColorEnum::NONE) {
                        m_colorLine = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::OVAL:
                    if (lineBackNoneFlag) {
                        m_colorBackground = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::TEXT:
                    m_colorLine          = s_userDefaultForTextColorLine;
                    m_customColorLine[0] = s_userDefaultForTextCustomColorLine[0];
                    m_customColorLine[1] = s_userDefaultForTextCustomColorLine[1];
                    m_customColorLine[2] = s_userDefaultForTextCustomColorLine[2];
                    m_customColorLine[3] = s_userDefaultForTextCustomColorLine[3];
                    break;
            }
        }
            break;
    }
    
    
    /*
     * May need to override colors if both are none BUT NOT for text
     */
    if (m_type != AnnotationTypeEnum::TEXT) {
        if ((m_colorBackground == CaretColorEnum::NONE)
            && (m_colorLine == CaretColorEnum::NONE)) {
            m_colorLine = CaretColorEnum::WHITE;
        }
    }
    
    
    /*
     * Don't allow a line color of NONE for text or line
     */
    bool disallowLineColorNoneFlag = false;
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            disallowLineColorNoneFlag = true;
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            disallowLineColorNoneFlag = true;
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
    if (disallowLineColorNoneFlag) {
        if (m_colorLine == CaretColorEnum::NONE) {
            m_colorLine = CaretColorEnum::WHITE;
            
            if (m_colorBackground == CaretColorEnum::WHITE) {
                m_colorBackground = CaretColorEnum::NONE;
            }
        }
    }
    
    initializeProperties();
    
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::VIEWPORT) {
        setPropertiesForSpecializedUsage(PropertiesSpecializedUsage::VIEWPORT_ANNOTATION);
    }
    
    /*
     * Note: The 'const' members are not saved to the scene as they 
     * are set by constructor.
     *
     * The 'selected' status is not saved to the scene.
     * 
     * Currently this is used for saving color bar attributes
     * to a scene.
     */
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayGroupAndTabItemHelper",
                          "DisplayGroupAndTabItemHelper",
                          m_displayGroupAndTabItemHelper);
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
        m_sceneAssistant->add<AnnotationCoordinateSpaceEnum, AnnotationCoordinateSpaceEnum::Enum>("m_coordinateSpace",
                                                                                                  &m_coordinateSpace);
        m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_colorBackground",
                                                                    &m_colorBackground);
        m_sceneAssistant->addArray("m_customColorBackground", m_customColorBackground, 4, 0.0);
        
        m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_colorLine",
                                                                    &m_colorLine);
        m_sceneAssistant->addArray("m_customColorLine", m_customColorLine, 4, 0.0);
    }
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
 * Get text displayed in the Paste Menu Items
 *
 * @param pasteMenuItemText
 *     Text for paste menu item.
 * @param pasteSpecialMenuItemText
 *     Text for paste special menu item.
 */
void
Annotation::getTextForPasteMenuItems(AString& pasteMenuItemText,
                                AString& pasteSpecialMenuItemText) const
{
    AString typeName = AnnotationTypeEnum::toGuiName(m_type);
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
        {
            const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
            CaretAssertMessage(textAnn,
                               "If this fails, it may be due to this method being called from a constructor "
                               "and the subclass constructor has not yet executed.");
            typeName = ("\""
                        + textAnn->getText()
                        + "\"");
        }
            break;
    }
    
    const AString spaceName = AnnotationCoordinateSpaceEnum::toGuiName(m_coordinateSpace);
    
    pasteMenuItemText = ("Paste "
                         + typeName
                         + " in "
                         + spaceName
                         + " Space");
    
    pasteSpecialMenuItemText = ("Paste "
                                + typeName
                                + " and Change Space...");
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
        CaretAssertMessage((m_coordinateSpace != AnnotationCoordinateSpaceEnum::VIEWPORT),
                           "Annotation coordinate space should never change to VIEWPORT space.");
        setModified();
    }
}

/**
 * @return Is this annotation in surface coordinate space
 * with tangent selected for the surface offset vector?
 */
bool
Annotation::isInSurfaceSpaceWithTangentOffset() const
{
    bool flag = false;
    
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            switch (getSurfaceOffsetVectorType()) {
                case AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX:
                    break;
                case AnnotationSurfaceOffsetVectorTypeEnum::SURFACE_NORMAL:
                    break;
                case AnnotationSurfaceOffsetVectorTypeEnum::TANGENT:
                    flag = true;
                    break;
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    return flag;
}

/**
 * Change a surface space annotation to TANGENT offset and update offset length.
 * If the annotation is already TANGENT space, no changes are made.
 */
void
Annotation::changeSurfaceSpaceToTangentOffset()
{
    std::vector<AnnotationCoordinate*> coords;
    if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE) {
        AnnotationOneDimensionalShape* oneDimAnn = castToOneDimensionalShape();
        if (oneDimAnn != NULL) {
            coords.push_back(oneDimAnn->getStartCoordinate());
            coords.push_back(oneDimAnn->getEndCoordinate());
        }
        else {
            AnnotationTwoDimensionalShape* twoDimAnn = castToTwoDimensionalShape();
            if (twoDimAnn != NULL) {
                coords.push_back(twoDimAnn->getCoordinate());
            }
        }
        
        for (auto c : coords) {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            int32_t surfaceNodeIndex;
            float surfaceOffsetLength;
            AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType;
            c->getSurfaceSpace(structure,
                               surfaceNumberOfNodes,
                               surfaceNodeIndex,
                               surfaceOffsetLength,
                               surfaceOffsetVectorType);
            if (surfaceOffsetVectorType != AnnotationSurfaceOffsetVectorTypeEnum::TANGENT) {
                surfaceOffsetVectorType = AnnotationSurfaceOffsetVectorTypeEnum::TANGENT;
                surfaceOffsetLength     = 1.0;
                c->setSurfaceSpace(structure,
                                   surfaceNumberOfNodes,
                                   surfaceNodeIndex,
                                   surfaceOffsetLength,
                                   surfaceOffsetVectorType);
            }
        }
    }
}

/**
 * Initialize the rotation for an annotation in surface space with tangent
 * offset using the given normal vector from the vertex to which the 
 * annotation is attached.
 *
 * @param structure
 *     The surface structure.
 * @param vertexNormal
 *     Normal vector of surface vertex.
 */
void
Annotation::initializeSurfaceSpaceWithTangentOffsetRotation(const StructureEnum::Enum structure,
                                                            const float vertexNormal[3])
{
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(this);
    if (twoDimAnn != NULL) {
        if (isInSurfaceSpaceWithTangentOffset()) {
            enum class OrientationType {
                LEFT_TO_RIGHT         = 0,
                RIGHT_TO_LEFT         = 1,
                POSTERIOR_TO_ANTERIOR = 2,
                ANTERIOR_TO_POSTERIOR = 3,
                INFERIOR_TO_SUPERIOR  = 4,
                SUPERIOR_TO_INFERIOR  = 5
            };
            
            const OrientationType orientations[6] = {
                OrientationType::LEFT_TO_RIGHT,
                OrientationType::RIGHT_TO_LEFT,
                OrientationType::POSTERIOR_TO_ANTERIOR,
                OrientationType::ANTERIOR_TO_POSTERIOR,
                OrientationType::INFERIOR_TO_SUPERIOR,
                OrientationType::SUPERIOR_TO_INFERIOR
            };
            const float orientationVectors[6][3] {
                {  1.0,  0.0,  0.0 },
                { -1.0,  0.0,  0.0 },
                {  0.0,  1.0,  0.0 },
                {  0.0, -1.0,  0.0 },
                {  0.0,  0.0,  1.0 },
                {  0.0,  0.0, -1.0 }
            };
            
            
            /*
             * Find orientation that aligns with the vertex's normal vector
             */
            OrientationType matchingOrientation = OrientationType::LEFT_TO_RIGHT;
            float matchingAngle = 10000.0f;
            for (int32_t i = 0; i < 6; i++) {
                const float angle = MathFunctions::angleInDegreesBetweenVectors(orientationVectors[i],
                                                                                vertexNormal);
                if (angle < matchingAngle) {
                    matchingAngle = angle;
                    matchingOrientation = orientations[i];
                }
            }
            
            float surfaceUpAxisVector[3] = { 0.0f, 0.0f, 1.0f };
            switch (matchingOrientation) {
                case OrientationType::LEFT_TO_RIGHT:
                    break;
                case OrientationType::RIGHT_TO_LEFT:
                    break;
                case OrientationType::POSTERIOR_TO_ANTERIOR:
                    break;
                case OrientationType::ANTERIOR_TO_POSTERIOR:
                    break;
                case OrientationType::INFERIOR_TO_SUPERIOR:
                    surfaceUpAxisVector[0] = 1.0;
                    surfaceUpAxisVector[0] = 0.0;
                    surfaceUpAxisVector[0] = 0.0;
                    break;
                case OrientationType::SUPERIOR_TO_INFERIOR:
                    surfaceUpAxisVector[0] = -1.0;
                    surfaceUpAxisVector[0] =  0.0;
                    surfaceUpAxisVector[0] =  0.0;
                    break;
            }
            
            /*
             * Vector for annotation's Y (vector from bottom to top of annotation)
             */
            const float annotationUpYVector[3] {
                0.0,
                1.0,
                0.0
            };
            
            /*
             * Initialize the rotation angle so that the annotation's vertical axis
             * is aligned with the screen vertical axis when the surface is in the
             * analogous surface view.  For a text annotation, the text should be
             * flowing left to right across screen.
             */
            Matrix4x4 rotationMatrix;
            rotationMatrix.setMatrixToOpenGLRotationFromVector(vertexNormal);
            Matrix4x4 inverseMatrix(rotationMatrix);
            inverseMatrix.invert();
            inverseMatrix.multiplyPoint3(surfaceUpAxisVector);
            const float alignRotationAngle = MathFunctions::angleInDegreesBetweenVectors(annotationUpYVector,
                                                                                           surfaceUpAxisVector);
            float rotationAngle = alignRotationAngle;
            switch (matchingOrientation) {
                case OrientationType::LEFT_TO_RIGHT:
                    rotationAngle = 360.0 - rotationAngle;
                    break;
                case OrientationType::RIGHT_TO_LEFT:
                    break;
                case OrientationType::POSTERIOR_TO_ANTERIOR:
                    if (StructureEnum::isRight(structure)) {
                        rotationAngle = 360.0 - rotationAngle;
                    }
                    break;
                case OrientationType::ANTERIOR_TO_POSTERIOR:
                    if (StructureEnum::isRight(structure)) {
                        rotationAngle = 360.0 - rotationAngle;
                    }
                    break;
                case OrientationType::INFERIOR_TO_SUPERIOR:
                    if (StructureEnum::isRight(structure)) {
                        rotationAngle += 180.0;
                    }
                    break;
                case OrientationType::SUPERIOR_TO_INFERIOR:
                    rotationAngle += 90.0;
                    break;
            }

            twoDimAnn->setRotationAngle(rotationAngle);
        }
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
        setModified();
    }
}

/**
 * Get the viewport used by an annotation in viewport coordinate space.
 *
 * @param viewportOut
 *     Ouput with the viewport.
 */
void
Annotation::getViewportCoordinateSpaceViewport(int viewportOut[4]) const
{
    viewportOut[0] = m_viewportCoordinateSpaceViewport[0];
    viewportOut[1] = m_viewportCoordinateSpaceViewport[1];
    viewportOut[2] = m_viewportCoordinateSpaceViewport[2];
    viewportOut[3] = m_viewportCoordinateSpaceViewport[3];
}

/**
 * Set the viewport used by an annotation in viewport coordinate space.
 *
 * @param viewport
 *     Input with the viewport.
 */
void
Annotation::setViewportCoordinateSpaceViewport(const int viewport[4])
{
    if ((m_viewportCoordinateSpaceViewport[0] != viewport[0])
        || (m_viewportCoordinateSpaceViewport[1] != viewport[1])
        || (m_viewportCoordinateSpaceViewport[2] != viewport[2])
        || (m_viewportCoordinateSpaceViewport[3] != viewport[3])) {
        m_viewportCoordinateSpaceViewport[0] = viewport[0];
        m_viewportCoordinateSpaceViewport[1] = viewport[1];
        m_viewportCoordinateSpaceViewport[2] = viewport[2];
        m_viewportCoordinateSpaceViewport[3] = viewport[3];
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
 * Convert the annotation's obsolete line width that was in pixels to a percentage of viewport height.
 * Prior to late July, 2017, a line was specified in pixels.
 *
 * First, the annotation's percentage width is examined.  If it is valid (greater than zero), then
 * no conversion is needed.  Otherwise, use the viewport height to convert the pixel width to a
 * percentage and set the annotation's percentage line width.
 *
 * Note that even though this method is 'const' it may cause the modification status to change
 * to modified.
 *
 * @param viewportHeight
 *     The height of the viewport in pixels.
 */
void
Annotation::convertObsoleteLineWidthPixelsToPercentageWidth(const float viewportHeight) const
{
    if (m_lineWidthPercentage > 0.0f) {
        return;
    }
    
    float percentWidth = 1.0f;
    if (viewportHeight > 0.0f) {
        percentWidth = (m_lineWidthPixels / viewportHeight) * 100.0f;
    }
    
    const_cast<Annotation*>(this)->setLineWidthPercentage(percentWidth);
}


/**
 * @return The line width in pixels.
 */
float
Annotation::getLineWidthPixelsObsolete() const
{
    return m_lineWidthPixels;
}

/**
 * Set the line width in pixels.
 *
 * @param lineWidth
 *    New value for line width.
 */
void
Annotation::setLineWidthPixelsObsolete(const float lineWidth)
{
    if (lineWidth != m_lineWidthPixels) {
        m_lineWidthPixels = lineWidth;
        setModified();
    }
}

/**
 * @return The line width percentage.
 */
float
Annotation::getLineWidthPercentage() const
{
    return m_lineWidthPercentage;
}

/**
 * Set the line width percentage.
 *
 * @param lineWidthPercentage
 *    New value for line width percentage.
 */
void
Annotation::setLineWidthPercentage(const float lineWidthPercentage)
{
    if (lineWidthPercentage != m_lineWidthPercentage) {
        m_lineWidthPercentage = lineWidthPercentage;
    }
}

/**
 * @return The line color.
 */
CaretColorEnum::Enum
Annotation::getLineColor() const
{
    return m_colorLine;
}

/**
 * Set the line color.
 *
 * @param color
 *     New value for line color.
 */
void
Annotation::setLineColor(const CaretColorEnum::Enum color)
{
    if (m_colorLine != color) {
        m_colorLine = color;
        setModified();
    }
}

/**
 * Get the line color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
Annotation::getLineColorRGBA(float rgbaOut[4]) const
{
    switch (m_colorLine) {
        case CaretColorEnum::NONE:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            break;
        case CaretColorEnum::CUSTOM:
            getCustomLineColor(rgbaOut);
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
            CaretColorEnum::toRGBAFloat(m_colorLine,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the line color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
Annotation::getLineColorRGBA(uint8_t rgbaOut[4]) const
{
    float rgbaFloat[4] = { 0.0, 0.0, 0.0, 0.0 };
    getLineColorRGBA(rgbaFloat);
    
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
 * Get the background color's RGBA components regardless of
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
            CaretColorEnum::toRGBAFloat(m_colorBackground,
                                       rgbaOut);
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Get the background color's RGBA components regardless of
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
 * Get the custom line color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getCustomLineColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_customColorLine[0];
    rgbaOut[1] = m_customColorLine[1];
    rgbaOut[2] = m_customColorLine[2];
    rgbaOut[3] = m_customColorLine[3];
}

/**
 * Get the custom line color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getCustomLineColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_customColorLine[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_customColorLine[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_customColorLine[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_customColorLine[3] * 255.0);
}

/**
 * Set the custom line color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setCustomLineColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_customColorLine[i]) {
            m_customColorLine[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the custom line color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setCustomLineColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_customColorLine[i]) {
            m_customColorLine[i] = component;
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
 * Initialize properties.
 */
void
Annotation::initializeProperties()
{
    /*
     * Initialize all properties on/supported
     */
    CaretAssert(m_properties.size() >= static_cast<std::underlying_type<Property>::type>(Property::COUNT_FOR_BITSET));
    m_properties.set();
    
    bool colorBarFlag = false;
    bool fillColorFlag = true;
    bool lineArrowsFlag = false;
    bool textFlag = false;
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            colorBarFlag = true;
            break;
        case AnnotationTypeEnum::IMAGE:
            fillColorFlag = false;
            break;
        case AnnotationTypeEnum::LINE:
            fillColorFlag = false;
            lineArrowsFlag = true;
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
            textFlag = true;
            break;
    }
    
    setProperty(Property::FILL_COLOR, fillColorFlag);
    setProperty(Property::LINE_ARROWS, lineArrowsFlag);
    setProperty(Property::TEXT_ALIGNMENT, textFlag);
    setProperty(Property::TEXT_EDIT, textFlag);
    setProperty(Property::TEXT_COLOR, textFlag);
    setProperty(Property::TEXT_FONT_NAME, colorBarFlag | textFlag);
    setProperty(Property::TEXT_FONT_SIZE, colorBarFlag | textFlag);
    setProperty(Property::TEXT_FONT_STYLE, textFlag);
    setProperty(Property::TEXT_ORIENTATION, textFlag);
    
    resetProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    
    /*
     * These are not valid properties 
     */
    resetProperty(Property::INVALID);
    resetProperty(Property::COUNT_FOR_BITSET);
    
    if (colorBarFlag) {
        resetProperty(Property::ARRANGE);
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
        resetProperty(Property::LINE_COLOR);
        resetProperty(Property::LINE_THICKNESS);
        resetProperty(Property::TEXT_COLOR);
        resetProperty(Property::TEXT_EDIT);
        
        setProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    }
}

/**
 * Set the properties for specialized usage of annotations.  This is typically used
 * for annotations that are not stored in an annotation file but instead are a 
 * property of some entity (such as chart labels).
 *
 * @param specializedUsage
 *     The specialized usage.
 */
void
Annotation::setPropertiesForSpecializedUsage(const PropertiesSpecializedUsage specializedUsage)
{
    initializeProperties();

    bool chartLabelTitleFlag = false;
    bool viewportFlag = false;
    switch (specializedUsage) {
        case PropertiesSpecializedUsage::CHART_LABEL:
            chartLabelTitleFlag = true;
            break;
        case PropertiesSpecializedUsage::CHART_TITLE:
            chartLabelTitleFlag = true;
            break;
        case PropertiesSpecializedUsage::VIEWPORT_ANNOTATION:
            viewportFlag = true;
            break;
    }

    if (chartLabelTitleFlag) {
        resetProperty(Property::ARRANGE);
        resetProperty(Property::COORDINATE);
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
        resetProperty(Property::LINE_COLOR);
        resetProperty(Property::LINE_THICKNESS);
        resetProperty(Property::ROTATION);
        resetProperty(Property::TEXT_ALIGNMENT);
        resetProperty(Property::TEXT_COLOR);
        resetProperty(Property::TEXT_ORIENTATION);
        
        setProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    }
    
    if (viewportFlag) {
        resetProperty(Property::ARRANGE);
        resetProperty(Property::COORDINATE);
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
        resetProperty(Property::ROTATION);
        
        setProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    }
}

/**
 * Test a property.
 * 
 * @param property
 *     Property for testing.
 * @return
 *     True if property is on, else false.
 */
bool
Annotation::testProperty(const Property property) const
{
    /*
     * Text connect to brainordiante property is valid only
     * for a a text annotation in surface space
    
     */
    if (property == Property::TEXT_CONNECT_TO_BRAINORDINATE) {
        if (m_type == AnnotationTypeEnum::TEXT) {
            if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::SURFACE) {
                return true;
            }
        }
        return false;
    }
    
    const int32_t propertyIndex = static_cast<std::underlying_type<Property>::type>(property);
    return m_properties.test(propertyIndex);
    
}

/**
 * Test for any of the properties are set.
 *
 * @param propertyOne
 *     First property for testing.
 * @param propertyTwo
 *     Two property for testing.
 * @param propertyThree
 *     Three property for testing.
 * @param propertyFour
 *     Four property for testing.
 * @param propertyFive
 *     Five property for testing.
 * @return
 *     True if property is on, else false.
 */
bool
Annotation::testPropertiesAny(const Property propertyOne,
                       const Property propertyTwo,
                       const Property propertyThree,
                       const Property propertyFour,
                       const Property propertyFive) const
{
    if (testProperty(propertyOne)) {
        return true;
    }
    if (testProperty(propertyTwo)) {
        return true;
    }
    if (propertyThree != Property::INVALID) {
        if (testProperty(propertyThree)) {
            return true;
        }
    }
    if (propertyFour != Property::INVALID) {
        if (testProperty(propertyFour)) {
            return true;
        }
    }
    if (propertyFive != Property::INVALID) {
        if (testProperty(propertyFive)) {
            return true;
        }
    }
    
    return false;
}


/**
 * Set a property.
 *
 * @param property
 *     Property for testing.
 * @param value
 *     New on/off status for property (optional argument defaults to true).
 */
void
Annotation::setProperty(const Property property,
                        const bool value)
{
    const int32_t propertyIndex = static_cast<std::underlying_type<Property>::type>(property);
    m_properties.set(propertyIndex,
                     value);
}

/**
 * Reset (turn off) a property.
 *
 * @param property
 *     Property for testing.
 */
void
Annotation::resetProperty(const Property property)
{
    const int32_t propertyIndex = static_cast<std::underlying_type<Property>::type>(property);
    m_properties.reset(propertyIndex);
}


/**
 * @return The key to the annotation group that owns this annotation.
 */
AnnotationGroupKey
Annotation::getAnnotationGroupKey() const
{
    return m_annotationGroupKey;
}

/**
 * Set the annotation group key.
 *
 * @param annotationGroupKey
 *     The key to the annotation group that contains this annotation.
 */
void
Annotation::setAnnotationGroupKey(const AnnotationGroupKey& annotationGroupKey)
{
    AnnotationGroupKey newGroupKeyForAnnotation = annotationGroupKey;
    
    switch (newGroupKeyForAnnotation.getGroupType()) {
        case AnnotationGroupTypeEnum::INVALID:
            CaretAssertMessage(0,
                               "Do not call this method with invalid key.  "
                               "Instead call invalidateAnnotationGroupKey().");
            break;
        case AnnotationGroupTypeEnum::SPACE:
            CaretAssert(newGroupKeyForAnnotation.getSpaceGroupUniqueKey() > 0);
            
            /*
             * When an annotation is moved to a space group (from a user group),
             * we want to preserve the user group unique key so that it can
             * be used to 'regroup' annotations
             */
            if (m_annotationGroupKey.getGroupType() == AnnotationGroupTypeEnum::USER) {
                newGroupKeyForAnnotation.setUserGroupUniqueKey(m_annotationGroupKey.getUserGroupUniqueKey());
            }            
            break;
        case AnnotationGroupTypeEnum::USER:
            CaretAssert(newGroupKeyForAnnotation.getUserGroupUniqueKey() > 0);
            break;
    }
    
    m_annotationGroupKey = newGroupKeyForAnnotation;
}

/**
 * Set the annotation group key so that it is invalid.
 */
void
Annotation::invalidateAnnotationGroupKey()
{
    m_annotationGroupKey = AnnotationGroupKey();
}

/**
 * Set the unique key for this annotation.  This method is
 * called by the annotation file when the annotation 
 * is added to the file.
 *
 * @param uniqueKey
 *     Unique key displayed in an annotation name.
 */
void
Annotation::setUniqueKey(const int32_t uniqueKey)
{
    m_uniqueKey = uniqueKey;
    
    textAnnotationResetName();
}

/**
 * @return Unique key displayed in annotation name.
 */
int32_t
Annotation::getUniqueKey() const
{
    return m_uniqueKey;
}

/**
 * Invalidate text substitutions.  This method is
 * implemented as a virtual method to avoid
 * dyamic casts since they are slow.
 */
void
Annotation::invalidateTextSubstitution()
{
    /* Nothing, override by AnnotationText */
}


/**
 * @return Name of annotation.
 */
AString
Annotation::getName() const
{
    return m_name;
}


/**
 * Called by text annotation to reset the name
 * displayed in the gui.  DO NOT CALL FROM
 * A CONSTRUCTOR !!!
 */
void
Annotation::textAnnotationResetName()
{
    AString suffixName;
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
        {
            const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(this);
            CaretAssertMessage(textAnn,
                               "If this fails, it may be due to this method being called from a constructor "
                               "and the subclass constructor has not yet executed.");
            suffixName = (": "
                          + textAnn->getText());
        }
            break;
    }
    
    m_name = (AnnotationTypeEnum::toGuiName(m_type)
              + " "
              + AString::number(m_uniqueKey)
              + suffixName);
}

/**
 * Set the drawn status to true for the given window.
 *
 * @param windowIndex
 *     Index of the window.
 *
 * This method is called from the graphics 
 * code when an annotation is drawn.
 *
 * NOTE: To find the annotations drawn in a window,
 * the drawn status is cleared, annotations are drawn
 * in one window, and the graphics system sets the
 * drawn status for annotations that are drawn.
 * A query is then made to find all annotations with
 * the drawn status set.
 */
void
Annotation::setDrawnInWindowStatus(const int32_t windowIndex)
{
    CaretAssertArrayIndex(m_drawnInWindowStatus, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_drawnInWindowStatus[windowIndex] = true;
}

/**
 * Is the drawn status set for the given window.
 *
 * @param windowIndex
 *     Index of the window.
 *
 * NOTE: To find the annotations drawn in a window,
 * the drawn status is cleared, annotations are drawn
 * in one window, and the graphics system sets the
 * drawn status for annotations that are drawn.
 * A query is then made to find all annotations with
 * the drawn status set.
 */
bool
Annotation::isDrawnInWindowStatus(const int32_t windowIndex)
{
    CaretAssertArrayIndex(m_drawnInWindowStatus, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_drawnInWindowStatus[windowIndex];
}

/**
 * Clear the drawn status for all windows.
 *
 * This method is called by the Annotation File
 * containing this annotation to clear the drawn status.
 *
 * NOTE: To find the annotations drawn in a window,
 * the drawn status is cleared, annotations are drawn
 * in one window, and the graphics system sets the
 * drawn status for annotations that are drawn.
 * A query is then made to find all annotations with
 * the drawn status set.
 */
void
Annotation::clearDrawnInWindowStatusForAllWindows()
{
    for (int32_t iWindow = 0; iWindow < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; iWindow++) {
        CaretAssertArrayIndex(m_drawnInWindowStatus, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, iWindow);
        m_drawnInWindowStatus[iWindow] = false;
    }
}


/**
 * @return The annotation's selected for editing status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 *
 * @param windowIndex
 *    Window for annotation selection status.
 */
bool
Annotation::isSelectedForEditing(const int32_t windowIndex) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < static_cast<int32_t>(m_selectedForEditingInWindowFlag.size())));
    return m_selectedForEditingInWindowFlag.test(windowIndex);
}

/**
 * Set the annotation's selected for editing status.
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
Annotation::setSelectedForEditing(const int32_t windowIndex,
                        const bool selectedStatus) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < static_cast<int32_t>(m_selectedForEditingInWindowFlag.size())));
    
    if (selectedStatus) {
        m_selectedForEditingInWindowFlag.set(windowIndex);
    }
    else {
        m_selectedForEditingInWindowFlag.reset(windowIndex);
    }
}

/**
 * Set the annotation's selected for editing status to deselected.
 */
void
Annotation::setDeselectedForEditing()
{
    /*
     * Clear selected status in ALL windows
     */
    m_selectedForEditingInWindowFlag.reset();
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
 * @return Number of children.
 */
int32_t
Annotation::getNumberOfItemChildren() const
{
    /*
     * Annotation has no children.
     */
    return 0;
}

/**
 * Get child at the given index.
 *
 * @param index
 *     Index of the child.
 * @return
 *     Child at the given index.
 */
DisplayGroupAndTabItemInterface*
Annotation::getItemChild(const int32_t /* index */) const
{
    /*
     * Annotation has no children.
     */
    return NULL;
}

/**
 * @return Children of this item.
 */
std::vector<DisplayGroupAndTabItemInterface*>
Annotation::getItemChildren() const
{
    /*
     * Annotation has no children.
     */
    std::vector<DisplayGroupAndTabItemInterface*> children;
    return children;
}


/**
 * @return Parent of this item.
 */
DisplayGroupAndTabItemInterface*
Annotation::getItemParent() const
{
    return m_displayGroupAndTabItemHelper->getParent();
}

/**
 * Set the parent of this item.
 *
 * @param itemParent
 *     Parent of this item.
 */
void
Annotation::setItemParent(DisplayGroupAndTabItemInterface* itemParent)
{
    m_displayGroupAndTabItemHelper->setParent(itemParent);
}

/**
 * @return Name of this item.
 */
AString
Annotation::getItemName() const
{
    return getName();
}

/**
 * Get the icon color for this item.  Icon is filled with background
 * color, outline color is drawn around edges, and text color is small
 * square in center.  For any colors that do not apply, use an alpha
 * value (last element) of zero.
 *
 * @param backgroundRgbaOut
 *     Red, green, blue, alpha components for background ranging [0, 1].
 * @param outlineRgbaOut
 *     Red, green, blue, alpha components for outline ranging [0, 1].
 * @param textRgbaOut
 *     Red, green, blue, alpha components for text ranging [0, 1].
 */
void
Annotation::getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                  float outlineRgbaOut[4],
                                  float textRgbaOut[4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        backgroundRgbaOut[i] = 0.0;
        outlineRgbaOut[i] = 0.0;
        textRgbaOut[i] = 0.0;
    }
    
    getBackgroundColorRGBA(backgroundRgbaOut);

    getLineColorRGBA(outlineRgbaOut);
    
    /*
     * Note: AnnotationText overrides this method to set text color
     */
    textRgbaOut[0] = 0.0;
    textRgbaOut[1] = 0.0;
    textRgbaOut[2] = 0.0;
    textRgbaOut[3] = 0.0;
}

/**
 * @return This item can be expanded.
 */
bool
Annotation::isItemExpandable() const
{
    return false;
}

/**
 * @return Is this item expanded in the given display group/tab?
 *
 * Note: If this annotation is in window space, the display group
 * and tab are ignored and window status is used with the window index
 * of the annotation.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
bool
Annotation::isItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const
{
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
        return m_displayGroupAndTabItemHelper->isExpandedInWindow(m_windowIndex);
    }
    
    return m_displayGroupAndTabItemHelper->isExpanded(displayGroup,
                                                      tabIndex);
}

/**
 * Set this item's expanded status in the given display group/tab.
 *
 * Note: If this annotation is in window space, the display group
 * and tab are ignored and window status is used with the window index
 * of the annotation.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     New expanded status.
 */
void
Annotation::setItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status)
{
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
        m_displayGroupAndTabItemHelper->setExpandedInWindow(m_windowIndex,
                                                            status);
    }
    else {
        m_displayGroupAndTabItemHelper->setExpanded(displayGroup,
                                                    tabIndex,
                                                    status);
    }
    
}

/**
 * Get display selection status in the given display group/tab?
 *
 * Note: If this annotation is in window space, the display group
 * and tab are ignored and window status is used with the window index
 * of the annotation.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
TriStateSelectionStatusEnum::Enum
Annotation::getItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const
{
    if (testProperty(Annotation::Property::DISPLAY_GROUP)) {
        if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
            return m_displayGroupAndTabItemHelper->getSelectedInWindow(m_windowIndex);
        }
        
        return m_displayGroupAndTabItemHelper->getSelected(displayGroup,
                                                           tabIndex);
    }
    
    /*
     * Annotations that do not support a "display group" are always displayed
     */
    return TriStateSelectionStatusEnum::SELECTED;
}

/**
 * Set display this item selected in the given display group/tab.
 *
 * Note: If this annotation is in window space, the display group
 * and tab are ignored and window status is used with the window index
 * of the annotation.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     New selection status.
 */
void
Annotation::setItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const TriStateSelectionStatusEnum::Enum status)
{
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
        m_displayGroupAndTabItemHelper->setSelectedInWindow(m_windowIndex,
                                                            status);
    }
    else {
        m_displayGroupAndTabItemHelper->setSelected(displayGroup,
                                                    tabIndex,
                                                    status);
    }
}

/**
 * Is this item selected for editing in the given window?
 *
 * @param windowIndex
 *     Index of the window.
 * @return
 *     Selection status.
 */
bool
Annotation::isItemSelectedForEditingInWindow(const int32_t windowIndex)
{
    return isSelectedForEditing(windowIndex);
}

/**
 * Set the default value for line color
 *
 * @param color
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultLineColor(const CaretColorEnum::Enum color)
{
    s_userDefaultColorLine = color;
}

/**
 * Set the default value for custom line color
 *
 * @param rgba
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultCustomLineColor(const float rgba[4])
{
    s_userDefaultCustomColorLine[0] = rgba[0];
    s_userDefaultCustomColorLine[1] = rgba[1];
    s_userDefaultCustomColorLine[2] = rgba[2];
    s_userDefaultCustomColorLine[3] = rgba[3];
}

/**
 * Set the default value for line color FOR USE BY TEXT ONLY
 *
 * @param color
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultForTextLineColor(const CaretColorEnum::Enum color)
{
    s_userDefaultForTextColorLine = color;
}

/**
 * Set the default value for custom line color FOR USE BY TEXT ONLY
 *
 * @param rgba
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultForTextCustomLineColor(const float rgba[4])
{
    s_userDefaultForTextCustomColorLine[0] = rgba[0];
    s_userDefaultForTextCustomColorLine[1] = rgba[1];
    s_userDefaultForTextCustomColorLine[2] = rgba[2];
    s_userDefaultForTextCustomColorLine[3] = rgba[3];
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
 * Set the default value for line width percentage
 *
 * @param lineWidthPercentage
 *     Default for newly created annotations.
 */
void
Annotation::setUserDefaultLineWidthPercentage(const float lineWidthPercentage)
{
    s_userDefaultLineWidthPercentage = lineWidthPercentage;
}

