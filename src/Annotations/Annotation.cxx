
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
#include "AnnotationBrowserTab.h"
#include "AnnotationColorBar.h"
#include "AnnotationCoordinate.h"
#include "AnnotationGroup.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "AnnotationPolygon.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationPolyLine.h"
#include "AnnotationSampleMetaData.h"
#include "AnnotationScaleBar.h"
#include "AnnotationText.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DisplayGroupAndTabItemHelper.h"
#include "EventAnnotationPolyhedronNameComponentSettings.h"
#include "EventManager.h"
#include "GiftiMetaData.h"
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

    if (m_type == AnnotationTypeEnum::POLYHEDRON) {
        for (uint32_t i = 0; i < s_selectionLockedPolyhedronInWindow.size(); i++) {
            if (this == s_selectionLockedPolyhedronInWindow[i]) {
                s_selectionLockedPolyhedronInWindow[i] = NULL;
            }
        }
    }
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
    m_spacerTabIndex      = obj.m_spacerTabIndex;
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

    m_drawingNewAnnotationStatusFlag = obj.m_drawingNewAnnotationStatusFlag;

    *m_metaData = *obj.m_metaData;
    
    m_stackingOrder = obj.m_stackingOrder;
    
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
        case AnnotationTypeEnum::BROWSER_TAB:
        {
            const AnnotationBrowserTab* browserTab = dynamic_cast<const AnnotationBrowserTab*>(this);
            CaretAssert(browserTab);
            myClone = new AnnotationBrowserTab(*browserTab);
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
        case AnnotationTypeEnum::POLYHEDRON:
        {
            const AnnotationPolyhedron* polyhedron(castToPolyhedron());
            CaretAssert(polyhedron);
            myClone = new AnnotationPolyhedron(*polyhedron);
        }
            break;
        case AnnotationTypeEnum::POLYGON:
        {
            const AnnotationPolygon* polygon = dynamic_cast<const AnnotationPolygon*>(this);
            CaretAssert(polygon);
            myClone = new AnnotationPolygon(*polygon);
        }
            break;
        case AnnotationTypeEnum::POLYLINE:
        {
            const AnnotationPolyLine* polyLine = dynamic_cast<const AnnotationPolyLine*>(this);
            CaretAssert(polyLine);
            myClone = new AnnotationPolyLine(*polyLine);
        }
            break;
        case AnnotationTypeEnum::SCALE_BAR:
        {
            const AnnotationScaleBar* scaleBar = dynamic_cast<const AnnotationScaleBar*>(this);
            CaretAssert(scaleBar);
            myClone = new AnnotationScaleBar(*scaleBar);
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
 * Clear modification status of this instance
 */
void
Annotation::clearModified()
{
    CaretObjectTracksModification::clearModified();
    m_metaData->clearModified();
}

/**
 * @return True if this instance is modified
 */
bool
Annotation::isModified() const
{
    if (m_metaData->isModified()) {
        return true;
    }
    return CaretObjectTracksModification::isModified();
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
        case AnnotationTypeEnum::BROWSER_TAB:
            annotation = new AnnotationBrowserTab(attributeDefaultType);
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
        case AnnotationTypeEnum::POLYHEDRON:
            annotation = new AnnotationPolyhedron(attributeDefaultType);
            break;
        case AnnotationTypeEnum::POLYGON:
            annotation = new AnnotationPolygon(attributeDefaultType);
            break;
        case AnnotationTypeEnum::POLYLINE:
            annotation = new AnnotationPolyLine(attributeDefaultType);
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            annotation = new AnnotationScaleBar(attributeDefaultType);
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
    m_spacerTabIndex = SpacerTabIndex();
    m_windowIndex = -1;
    m_viewportCoordinateSpaceViewport[0] = 0;
    m_viewportCoordinateSpaceViewport[1] = 0;
    m_viewportCoordinateSpaceViewport[2] = 0;
    m_viewportCoordinateSpaceViewport[3] = 0;
    
    m_drawingNewAnnotationStatusFlag = false;

    m_displayGroupAndTabItemHelper = new DisplayGroupAndTabItemHelper();
    
    m_metaData.reset(new GiftiMetaData());
    
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
                case AnnotationTypeEnum::BROWSER_TAB:
                    m_colorBackground = CaretColorEnum::NONE;
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
                case AnnotationTypeEnum::POLYGON:
                    break;
                case AnnotationTypeEnum::POLYHEDRON:
                    break;
                case AnnotationTypeEnum::POLYLINE:
                    break;
                case AnnotationTypeEnum::SCALE_BAR:
                    m_colorBackground = CaretColorEnum::BLACK;
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
                case AnnotationTypeEnum::BROWSER_TAB:
                    m_colorBackground = CaretColorEnum::NONE;
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
                case AnnotationTypeEnum::POLYHEDRON:
                    if (m_colorLine == CaretColorEnum::NONE) {
                        m_colorLine = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::POLYGON:
                    if (m_colorLine == CaretColorEnum::NONE) {
                        m_colorLine = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::POLYLINE:
                    if (m_colorLine == CaretColorEnum::NONE) {
                        m_colorLine = defaultColor;
                    }
                    break;
                case AnnotationTypeEnum::SCALE_BAR:
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
        case AnnotationTypeEnum::BROWSER_TAB:
            disallowLineColorNoneFlag = true;
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
        case AnnotationTypeEnum::POLYHEDRON:
            disallowLineColorNoneFlag = true;
            break;
        case AnnotationTypeEnum::POLYGON:
            disallowLineColorNoneFlag = true;
            break;
        case AnnotationTypeEnum::POLYLINE:
            disallowLineColorNoneFlag = true;
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            disallowLineColorNoneFlag = true;
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
    
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            setPropertiesForSpecializedUsage(PropertiesSpecializedUsage::VIEWPORT_ANNOTATION);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
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
        
        m_sceneAssistant->add("m_lineWidthPercentage",
                              &m_lineWidthPercentage);
        
        m_sceneAssistant->add("m_stackingOrder",
                              &m_stackingOrder);
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
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
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
 * @return All coordinates in the annotation (const method)
 */
std::vector<const AnnotationCoordinate*>
Annotation::getAllCoordinates() const
{
    std::vector<const AnnotationCoordinate*> allCoords;
    
    const int32_t numCoords(getNumberOfCoordinates());
    for (int32_t i = 0; i < numCoords; i++) {
        allCoords.push_back(getCoordinate(i));
    }
    
    return allCoords;
}

/**
 * Replace all coordinates in this annotation with copies of the given coordinates
 * @param coordinates
 *    Coordinates (const) that are copied into this annotation
 */
void
Annotation::replaceAllCoordinatesNotConst(const std::vector<std::unique_ptr<AnnotationCoordinate>>& coordinates)
{
    std::vector<std::unique_ptr<const AnnotationCoordinate>> constCoords;
    for (const auto& ac : coordinates) {
        std::unique_ptr<const AnnotationCoordinate> cc(new AnnotationCoordinate(*ac.get()));
        constCoords.push_back(std::move(cc));
    }
    replaceAllCoordinates(constCoords);
}

/**
 * Get a copy of all coordinates in the annotation
 * @param allCoordsOut
 *    Output containing copy of all coordinates
 */
std::vector<std::unique_ptr<AnnotationCoordinate>>
Annotation::getCopyOfAllCoordinates() const
{
    std::vector<std::unique_ptr<AnnotationCoordinate>> allCoords;
    
    const int32_t numCoords(getNumberOfCoordinates());
    for (int32_t i = 0; i < numCoords; i++) {
        std::unique_ptr<AnnotationCoordinate> ac(new AnnotationCoordinate(*getCoordinate(i)));
        allCoords.push_back(std::move(ac));
    }
    
    return allCoords;
}

/**
 * @return True if the given annotation is in the same coordinate space as this
 * annotation.  For spacer, tab, and window they must have the same indices. For
 * surface space structure and number of vertices must match.
 * @param annotation
 * Annotation for comparison.
 */
bool
Annotation::isInSameCoordinateSpace(const Annotation* annotation) const
{
    CaretAssert(annotation);
    
    if (getCoordinateSpace() != annotation->getCoordinateSpace()) {
        return false;
    }
    
    bool sameSpaceFlag(false);
        
    switch (annotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            sameSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            if (getCoordinate(0)->getHistologySpaceKey() == annotation->getCoordinate(0)->getHistologySpaceKey()) {
                sameSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            if (getCoordinate(0)->getMediaFileName() == annotation->getCoordinate(0)->getMediaFileName()) {
                sameSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            if (getSpacerTabIndex() == annotation->getSpacerTabIndex()) {
                sameSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            sameSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
        {
            StructureEnum::Enum myStructure = StructureEnum::INVALID;
            int32_t myNumberOfVertices(-1);
            const AnnotationTwoCoordinateShape* oneDimAnn = castToTwoCoordinateShape();
            if (oneDimAnn != NULL) {
                int32_t vertexIndex(-1);
                oneDimAnn->getStartCoordinate()->getSurfaceSpace(myStructure,
                                                                 myNumberOfVertices,
                                                                 vertexIndex);
            }
            else {
                const AnnotationOneCoordinateShape* twoDimAnn = castToOneCoordinateShape();
                if (twoDimAnn != NULL) {
                    int32_t vertexIndex(-1);
                    twoDimAnn->getCoordinate()->getSurfaceSpace(myStructure,
                                                                myNumberOfVertices,
                                                                vertexIndex);
                }
            }
            
            StructureEnum::Enum otherStructure = StructureEnum::INVALID;
            int32_t otherSurfaceNumberOfVertices(-1);
            const AnnotationTwoCoordinateShape* otherOneDimAnn = annotation->castToTwoCoordinateShape();
            if (otherOneDimAnn != NULL) {
                int32_t vertexIndex(-1);
                otherOneDimAnn->getStartCoordinate()->getSurfaceSpace(otherStructure,
                                                                 otherSurfaceNumberOfVertices,
                                                                 vertexIndex);
            }
            else {
                const AnnotationOneCoordinateShape* otherTwoDimAnn = annotation->castToOneCoordinateShape();
                if (otherTwoDimAnn != NULL) {
                    int32_t vertexIndex(-1);
                    otherTwoDimAnn->getCoordinate()->getSurfaceSpace(otherStructure,
                                                                otherSurfaceNumberOfVertices,
                                                                vertexIndex);
                }
            }
            
            if (myNumberOfVertices > 0) {
                if ((myStructure == otherStructure)
                    && (myNumberOfVertices == otherSurfaceNumberOfVertices)) {
                    sameSpaceFlag = true;
                }
            }
        }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (getTabIndex() == annotation->getTabIndex()) {
                sameSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            sameSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (getWindowIndex() == annotation->getWindowIndex()) {
                sameSpaceFlag = true;
            }
            break;
    }
    
    return sameSpaceFlag;
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
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
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
        AnnotationTwoCoordinateShape* oneDimAnn = castToTwoCoordinateShape();
        if (oneDimAnn != NULL) {
            coords.push_back(oneDimAnn->getStartCoordinate());
            coords.push_back(oneDimAnn->getEndCoordinate());
        }
        else {
            AnnotationOneCoordinateShape* twoDimAnn = castToOneCoordinateShape();
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
 * Get the rotation for an annotation in surface space with tangent
 * offset using the given normal vector from the vertex to which the
 * annotation is attached.
 *
 * @param structure
 *     The surface structure.
 * @param vertexNormal
 *     Normal vector of surface vertex.
 * @return
 *     Rotation angle so text is oriented up with 'best axis'
 */
float
Annotation::getSurfaceSpaceWithTangentOffsetRotation(const StructureEnum::Enum structure,
                                                     const float vertexNormal[3]) const
{
    float angleOut(0.0);
    
    const AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<const AnnotationOneCoordinateShape*>(this);
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
            
            angleOut = rotationAngle;
        }
    }
    
    return angleOut;
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
    return;
    
    
    AnnotationOneCoordinateShape* twoDimAnn = castToOneCoordinateShape();
    if (twoDimAnn != NULL) {
        if (isInSurfaceSpaceWithTangentOffset()) {
            const float angle = getSurfaceSpaceWithTangentOffsetRotation(structure,
                                                                         vertexNormal);
            twoDimAnn->setRotationAngle(angle);
        }
    }
    
    return;
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
 * @return Index of the spacer tab.
 */
SpacerTabIndex
Annotation::getSpacerTabIndex() const
{
    return m_spacerTabIndex;
}

/**
 * Set index of the spacer tab.
 *
 * @param spacerTabIndex
 *     Index of the spacer tab.
 */
void
Annotation::setSpacerTabIndex(const SpacerTabIndex& spacerTabIndex)
{
    if (spacerTabIndex != m_spacerTabIndex) {
        m_spacerTabIndex = spacerTabIndex;
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
 * @return Pointer to the metadata
 */
GiftiMetaData*
Annotation::getMetaData()
{
    return m_metaData.get();
}

/**
 * @return Pointer to the metadata (const method)
 */
const GiftiMetaData*
Annotation::getMetaData() const
{
    return m_metaData.get();
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
    
    bool browserTabFlag(false);
    bool colorBarFlag = false;
    bool fillColorFlag = true;
    bool lineArrowsFlag = false;
    bool scaleBarFlag = false;
    bool textAttributesFlag = false;
    bool textFlag = false;
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            browserTabFlag = true;
            fillColorFlag = false;
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
        case AnnotationTypeEnum::POLYHEDRON:
            fillColorFlag = false;
            textAttributesFlag = true;
            break;
        case AnnotationTypeEnum::POLYGON:
            fillColorFlag = false;
            break;
        case AnnotationTypeEnum::POLYLINE:
            fillColorFlag = false;
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            scaleBarFlag = true;
            break;
        case AnnotationTypeEnum::TEXT:
            textFlag = true;
            break;
    }
    
    setProperty(Property::FILL_COLOR, fillColorFlag);
    setProperty(Property::LINE_ARROWS, lineArrowsFlag);
    setProperty(Property::TEXT_ALIGNMENT, textFlag);
    setProperty(Property::TEXT_EDIT, textFlag);
    setProperty(Property::TEXT_COLOR, colorBarFlag | scaleBarFlag | textFlag | textAttributesFlag);
    setProperty(Property::TEXT_FONT_NAME, colorBarFlag | scaleBarFlag | textFlag | textAttributesFlag);
    setProperty(Property::TEXT_FONT_SIZE, colorBarFlag | scaleBarFlag | textFlag | textAttributesFlag);
    setProperty(Property::TEXT_FONT_STYLE, textFlag | textAttributesFlag);
    setProperty(Property::TEXT_ORIENTATION, textFlag);
    
    resetProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    
    /*
     * These are not valid properties 
     */
    resetProperty(Property::INVALID);
    resetProperty(Property::COUNT_FOR_BITSET);
    
    if (browserTabFlag) {
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
        resetProperty(Property::LINE_COLOR);
        resetProperty(Property::LINE_THICKNESS);
        resetProperty(Property::TEXT_COLOR);
        resetProperty(Property::TEXT_EDIT);
    }
    
    if (colorBarFlag) {
        resetProperty(Property::ARRANGE);
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
        resetProperty(Property::LINE_COLOR);
        resetProperty(Property::LINE_THICKNESS);
        resetProperty(Property::TEXT_EDIT);
        
        setProperty(Property::SCENE_CONTAINS_ATTRIBUTES);
    }
    
    if (scaleBarFlag) {
        resetProperty(Property::ARRANGE);
        resetProperty(Property::COPY_CUT_PASTE);
        resetProperty(Property::DELETION);
        resetProperty(Property::DISPLAY_GROUP);
        resetProperty(Property::GROUP);
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
        case AnnotationGroupTypeEnum::SAMPLES_ACTUAL:
            CaretAssert(newGroupKeyForAnnotation.getSamplesActualUniqueKey() > 0);
            break;
        case AnnotationGroupTypeEnum::SAMPLES_DESIRED:
            CaretAssert(newGroupKeyForAnnotation.getSamplesDesiredUniqueKey() > 0);
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
 * Add information about the content of this instance.
 *
 * @param dataFileInformation
 *     Will contain information about this instance.
 */
void
Annotation::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    dataFileInformation.addNameAndValue("Annotation Name", getName());
    dataFileInformation.addNameAndValue("Type", AnnotationTypeEnum::toName(m_type));
    dataFileInformation.addNameAndValue("Space", AnnotationCoordinateSpaceEnum::toGuiName(getCoordinateSpace()));
    
    const AnnotationText* textAnn = castToTextAnnotation();
    if (textAnn != NULL) {
        dataFileInformation.addNameAndValue("Text", textAnn->getText());
    }
}

/**
 * @return Name of annotation.
 */
AString
Annotation::getName() const
{
    AString nameOut(m_name);
    
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
        {
            /*
             * Since it is difficult to detect a change in metadata,
             * we just add the Ding Abbreviation to the annotation name here.
             */
            const AnnotationPolyhedron* polyhedron(castToPolyhedron());
            CaretAssert(polyhedron);

            std::vector<AString> textComponents;
            if ( ! polyhedron->getSampleMetaData()->getSampleName().isEmpty()) {
                textComponents.push_back(polyhedron->getSampleMetaData()->getSampleName());
            }
            if ( ! polyhedron->getSampleMetaData()->getSampleNumber().isEmpty()) {
                textComponents.push_back(polyhedron->getSampleMetaData()->getSampleNumber());
            }
            switch (polyhedron->getPolyhedronType()) {
                case AnnotationPolyhedronTypeEnum::INVALID:
                    break;
                case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
                    textComponents.push_back("A");
                    break;
                case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
                    textComponents.push_back("D");
                    break;
            }

            nameOut = AString::join(textComponents, ".");
        }
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }

    return nameOut;
}

/**
 * @return name for drawing in graphics window
 */
AString
Annotation::getNameForGraphicsDrawing() const
{
    AString nameOut(m_name);
    
    switch (m_type) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
        {
            /*
             * Since it is difficult to detect a change in metadata,
             * we just add the Ding Abbreviation to the annotation name here.
             */
            const AnnotationPolyhedron* polyhedron(castToPolyhedron());
            CaretAssert(polyhedron);
            
            EventAnnotationPolyhedronNameComponentSettings nameCompEvent;
            EventManager::get()->sendEvent(nameCompEvent.getPointer());
            
            std::vector<AString> textComponents;
            if (nameCompEvent.isShowName()) {
                if ( ! polyhedron->getSampleMetaData()->getSampleName().isEmpty()) {
                    textComponents.push_back(polyhedron->getSampleMetaData()->getSampleName());
                }
            }
            if (nameCompEvent.isShowNumber()) {
                if ( ! polyhedron->getSampleMetaData()->getSampleNumber().isEmpty()) {
                    textComponents.push_back(polyhedron->getSampleMetaData()->getSampleNumber());
                }
            }
            if (nameCompEvent.isShowActualDesiredSuffix()) {
                switch (polyhedron->getPolyhedronType()) {
                    case AnnotationPolyhedronTypeEnum::INVALID:
                        break;
                    case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
                        textComponents.push_back("A");
                        break;
                    case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
                        textComponents.push_back("D");
                        break;
                }
            }
            
            
            nameOut = AString::join(textComponents, ".");
        }
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
    
    return nameOut;
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
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
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
Annotation::isDrawnInWindowStatus(const int32_t windowIndex) const
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
 * @return Stacking order (depth in screen) of tab, greater value is 'in front'
 */
int32_t
Annotation::getStackingOrder() const
{
    return m_stackingOrder;
}

/**
 * Set Stacking order (depth in screen) of tab, greater value is 'in front'
 *
 * @param stackingOrder
 *    New value for Stacking order (depth in screen) of tab, greater value is 'in front'
 */
void
Annotation::setStackingOrder(const int32_t stackingOrder)
{
    m_stackingOrder = stackingOrder;
    setModified();
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
    const AnnotationPolyhedron* thisPolyhedron(castToPolyhedron());

    if (thisPolyhedron != NULL) {
        const AnnotationPolyhedron* lockedPolyhedron(getSelectionLockedPolyhedronInWindow(windowIndex));
        if (lockedPolyhedron != NULL) {
            if (thisPolyhedron == lockedPolyhedron) {
                m_selectedForEditingInWindowFlag.set(windowIndex);
            }
            else {
                m_selectedForEditingInWindowFlag.reset(windowIndex);
            }
        }
    }

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
    
    const AnnotationPolyhedron* thisPolyhedron(castToPolyhedron());
    if (thisPolyhedron != NULL) {
        const AnnotationPolyhedron* lockedPolyhedron(getSelectionLockedPolyhedronInWindow(windowIndex));
        if (lockedPolyhedron != NULL) {
            if (thisPolyhedron == lockedPolyhedron) {
                m_selectedForEditingInWindowFlag.set(windowIndex);
            }
            else {
                m_selectedForEditingInWindowFlag.reset(windowIndex);
            }
        }
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
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            return m_displayGroupAndTabItemHelper->isExpandedInWindow(m_windowIndex);
            break;
    }
    
    const int32_t itemTabIndex = updateDisplayGroupTabIndex(displayGroup,
                                                            tabIndex);
    
    return m_displayGroupAndTabItemHelper->isExpanded(displayGroup,
                                                      itemTabIndex);
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
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            m_displayGroupAndTabItemHelper->setExpandedInWindow(m_windowIndex,
                                                                status);
            return;
            break;
    }

    const int32_t itemTabIndex = updateDisplayGroupTabIndex(displayGroup,
                                                                tabIndex);
        
    m_displayGroupAndTabItemHelper->setExpanded(displayGroup,
                                                itemTabIndex,
                                                status);
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
        switch (m_coordinateSpace) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                return m_displayGroupAndTabItemHelper->getSelectedInSpacerTab();
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                return m_displayGroupAndTabItemHelper->getSelectedInWindow(m_windowIndex);
                break;
        }
        
        
        const int32_t itemTabIndex = updateDisplayGroupTabIndex(displayGroup,
                                                                tabIndex);
        
        return m_displayGroupAndTabItemHelper->getSelected(displayGroup,
                                                           itemTabIndex);
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
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            m_displayGroupAndTabItemHelper->setSelectedInSpacerTab(status);
            return;
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            m_displayGroupAndTabItemHelper->setSelectedInWindow(m_windowIndex,
                                                                status);
            return;
            break;
    }
    
    const int32_t itemTabIndex = updateDisplayGroupTabIndex(displayGroup,
                                                            tabIndex);
    
    m_displayGroupAndTabItemHelper->setSelected(displayGroup,
                                                itemTabIndex,
                                                status);
}

/**
 * Update the tab index to correspond to the tab index used for this
 * annotation if it is in tab annotation space.  This functionality
 * was added to resolve WB-831.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
int32_t
Annotation::updateDisplayGroupTabIndex(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    int32_t tabIndexOut(tabIndex);
    if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
        if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            tabIndexOut = getTabIndex();
        }
    }
    return tabIndexOut;
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

/**
 * @return true if this and the given annotation intersect using bounding box from when drawn in the given window
 *         NOTE: if 'other' is 'this' true is returned (overlaps self) but this
 *         could change so it is best to avoid testing overlap of self.
 *         NOTE: Display status is ignored
 *
 * @param other
 *     Other annotation for intersection test
 * @param windowIndex
 *     Index of window
 */
bool
Annotation::intersectionTest(const Annotation* other,
                             const int32_t windowIndex) const
{
    if ( ! isInSameCoordinateSpace(other)) {
        return false;
    }

    if (isDrawnInWindowStatus(windowIndex)
         && other->isDrawnInWindowStatus(windowIndex)) {
        if (m_boundsFromDrawing[windowIndex].intersectsXY(other->m_boundsFromDrawing[windowIndex])) {
            return true;
        }
    }
    
    return false;
}

/**
 * Resize the annotation so that it is the same size, in pixels, in the new viewport.
 * @param oldViewport
 *    Current viewport of annotation
 * @param newViewport
 *    New viewport for annotation
 * @param matchPositionFlag
 *     If true, try to match position, but may require moving the annotion
 * @param matchSizeFlag
 *     If true, match the size of the annotaiton
 */
void
Annotation::matchPixelPositionAndSizeInNewViewport(const int32_t oldViewport[4],
                                                   const int32_t newViewport[4],
                                                   const bool matchPositionFlag,
                                                   const bool matchSizeFlag)
{
    const float newViewportWidth(newViewport[2]);
    if (newViewportWidth <= 0.0) {
        return;
    }
    const float newViewportHeight(newViewport[3]);
    if (newViewportHeight <= 0.0) {
        return;
    }

    bool tabOrWindowFlag(false);
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            tabOrWindowFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            tabOrWindowFlag = true;
            break;
    }
    
    if ( ! tabOrWindowFlag) {
        return;
    }
    
    AnnotationTwoCoordinateShape* oneDimAnn = castToTwoCoordinateShape();
    if (oneDimAnn != NULL) {
        const AnnotationCoordinate* coordOne = oneDimAnn->getStartCoordinate();
        CaretAssert(coordOne);
        const AnnotationCoordinate* coordTwo = oneDimAnn->getEndCoordinate();
        CaretAssert(coordTwo);
        const float length = MathFunctions::distance3D(coordOne->getXYZ(),
                                                       coordTwo->getXYZ());
        if (length > 0.0) {
        }
        CaretLogWarning("Matching size not supported for one-dimensional annotations");
    }
    else {
        AnnotationOneCoordinateShape* twoDimAnn = castToOneCoordinateShape();
        if (twoDimAnn) {
            if (twoDimAnn->isFixedAspectRatio()) {
                CaretLogWarning("Matching size not supported for fixed aspect ratio two-dimensional annotations");
            }
            else {
                const float oldViewportWidth(oldViewport[2]);
                const float oldViewportHeight(oldViewport[3]);
                const float widthPixels((twoDimAnn->getWidth() / 100.0) * oldViewportWidth);
                const float heightPixels((twoDimAnn->getHeight() / 100.0) * oldViewportHeight);
                const float newWidthPct((widthPixels / newViewportWidth) * 100.0);
                const float newHeightPct((heightPixels / newViewportHeight) * 100.0);
                if (matchSizeFlag) {
                    twoDimAnn->setWidth(newWidthPct);
                    twoDimAnn->setHeight(newHeightPct);
                }
                
                if (matchPositionFlag) {
                    float oldXYZ[3];
                    twoDimAnn->getCoordinate()->getXYZ(oldXYZ);
                    const float oldPixelX(((oldXYZ[0] / 100.0) * oldViewportWidth)  + oldViewport[0]);
                    const float oldPixelY(((oldXYZ[1] / 100.0) * oldViewportHeight) + oldViewport[1]);
                    const float newPixelX(oldPixelX - newViewport[0]);
                    const float newPixelY(oldPixelY - newViewport[1]);
                    float newPctX((newPixelX / newViewportWidth) * 100.0);
                    if (newPctX <= 0.0) {
                        newPctX = std::min((twoDimAnn->getWidth() / 2.0), 99.0);
                    }
                    else if (newPctX >= 100.0) {
                        newPctX = std::max((100.0 - (twoDimAnn->getWidth() / 2.0)), 1.0);
                    }
                    float newPctY((newPixelY / newViewportHeight) * 100.0);
                    if (newPctY <= 0.0) {
                        newPctY = std::min((twoDimAnn->getHeight() / 2.0), 99.0);
                    }
                    else if (newPctY >= 100.0) {
                        newPctY = std::max((100.0 - (twoDimAnn->getHeight() / 2.0)), 1.0);
                    }
                    twoDimAnn->getCoordinate()->setXYZ(newPctX,
                                                       newPctY,
                                                       oldXYZ[2]);
                }
                
                AnnotationColorBar* colorBar = dynamic_cast<AnnotationColorBar*>(twoDimAnn);
                if (colorBar != NULL) {
                    const float oldFontHeightPixels((colorBar->getFontPercentViewportSize() / 100.0) * oldViewport[3]);
                    const float newFontHeightPct((oldFontHeightPixels / newViewportHeight) * 100.0);
                    if (matchSizeFlag) {
                        colorBar->setFontPercentViewportSize(newFontHeightPct);
                    }
                }
            }
        }
        else {
            CaretAssertMessage(0, "Annotation is neither one- nor two-dimensional");
        }
    }
}

/**
 * Set the bounds from last time annotation was drawn
 * @param windowIndex
 * Index of window
 * @param bounds
 * The bounds parallel to screen axes (elements are min-x, max-x, min-y, max-y, min-z, max-z)
 */
void
Annotation::setDrawnInWindowBounds(const int32_t windowIndex,
                                   const BoundingBox& bounds) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    m_boundsFromDrawing[windowIndex] = bounds;
}

/**
 * Get the bounds from last time annotation was drawn
 * @param windowIndex
 * Index of window
 * @@return
 * The bounds parallel to screen axes (elements are min-x, max-x, min-y, max-y, average Z)
 */
BoundingBox
Annotation::getDrawnInWindowBounds(const int32_t windowIndex) const
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    return m_boundsFromDrawing[windowIndex];
}

/**
 * @param True if the annotation passes validation, else false.
 * @param messageOut
 *     Output describing failure of validation.
 */
bool
Annotation::validate(AString& messageOut) const
{
    messageOut.clear();
    bool validFlag(true);
    
    const std::vector<const AnnotationCoordinate*> coords(getAllCoordinates());
    const int32_t numCoords(coords.size());
    
    const AnnotationMultiCoordinateShape* multiCoordAnn = castToMultiCoordinateShape();
    if (numCoords == 0) {
        messageOut.appendWithNewLine("Has no coordinates");
    }
    else if (numCoords == 1) {
        if (multiCoordAnn != NULL) {
            messageOut.appendWithNewLine("Has only one coordinate, must have two");
        }
    }
    
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (numCoords > 0) {
                bool structuresMatchFlag(true);
                bool structureInvalidFlag(false);
                bool surfaceVertexCountsMatchFlag(true);
                int32_t invalidVertexIndexFlag(false);
                
                StructureEnum::Enum firstStructure(StructureEnum::INVALID);
                int32_t firstSurfaceVertexCount(-1);
                int32_t firstVertexIndex(-1);
                coords[0]->getSurfaceSpace(firstStructure,
                                           firstSurfaceVertexCount,
                                           firstVertexIndex);
                
                for (int32_t i = 0; i < numCoords; i++) {
                    StructureEnum::Enum structure(StructureEnum::INVALID);
                    int32_t surfaceVertexCount(-1);
                    int32_t vertexIndex(-1);
                    coords[i]->getSurfaceSpace(structure,
                                               surfaceVertexCount,
                                               vertexIndex);
                    if (structure == StructureEnum::INVALID) {
                        structureInvalidFlag = true;
                    }
                    if (structure != firstStructure) {
                        structuresMatchFlag = false;
                    }
                    if (surfaceVertexCount != firstSurfaceVertexCount) {
                        surfaceVertexCountsMatchFlag = false;
                    }
                    if (vertexIndex < 0) {
                        invalidVertexIndexFlag = true;
                    }
                }
                
                if (structureInvalidFlag) {
                    messageOut.appendWithNewLine("Structure is invalid for surface space.");
                }
                if ( ! structuresMatchFlag) {
                    messageOut.appendWithNewLine("Structures do not match for surface space.");
                }
                if ( ! surfaceVertexCountsMatchFlag) {
                    messageOut.appendWithNewLine("Surface vertex counts do not match for surface space.");
                }
                if (invalidVertexIndexFlag) {
                    messageOut.appendWithNewLine("Surface vertex index less than zero for surface space.");
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if ((m_tabIndex < 0)
                || (m_tabIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                messageOut.appendWithNewLine("Tab index is invalid: "
                                             + AString::number(m_tabIndex));
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if ((m_windowIndex < 0)
                || (m_windowIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
                messageOut.appendWithNewLine("Window index is invalid: "
                                             + AString::number(m_tabIndex));
            }
            break;
    }
    
    if ( ! messageOut.isEmpty()) {
        AString infoMsg("Annotation is invalid: "
                        + this->toString());
        for (int32_t i = 0; i < numCoords; i++) {
            infoMsg.appendWithNewLine("Coord "
                                      + AString::number(i)
                                      + ": "
                                      + coords[i]->toStringForCoordinateSpace(m_coordinateSpace));
        }
        if (numCoords > 0) {
            infoMsg.append("\n");
        }
        messageOut.insert(0, infoMsg);
        messageOut.replace("\n", "\n   ");
        validFlag = false;
    }
    
    return validFlag;
}

/**
 * Set drawing new annotation status
 * @param status
 *    The new status
 */
void
Annotation::setDrawingNewAnnotationStatus(const bool status)
{
    m_drawingNewAnnotationStatusFlag = status;
}

/**
 * Is a new annotation being drawn?
 */
bool
Annotation::isDrawingNewAnnotation() const
{
    return m_drawingNewAnnotationStatusFlag;
}

/**
 * @return The polyhedron locked for selection in the given window
 * @param windowIndex
 *    Index of window
 */
AnnotationPolyhedron*
Annotation::getSelectionLockedPolyhedronInWindow(const int32_t windowIndex)
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    return s_selectionLockedPolyhedronInWindow[windowIndex];
}

/**
 * @return Set the polyhedron locked for selection in the given window
 * @param windowIndex
 *    Index of window
 * @param annotationPolyhedron
 *    The polyhedron
 */
void
Annotation::setSelectionLockedPolyhedronInWindow(const int32_t windowIndex,
                                                 AnnotationPolyhedron* annotationPolyhedron)
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    s_selectionLockedPolyhedronInWindow[windowIndex] = annotationPolyhedron;
}

/**
 * Unlock this polyhedron in any windows such as when polyedron is deleted
 * @param annotationPolyhedron
 *    The polyhedron
 */
void
Annotation::unlockPolyhedronInAnyWindow(AnnotationPolyhedron* annotationPolyhedron)
{
    for (uint32_t i = 0; i < s_selectionLockedPolyhedronInWindow.size(); i++) {
        if (annotationPolyhedron == s_selectionLockedPolyhedronInWindow[i]) {
            s_selectionLockedPolyhedronInWindow[i] = NULL;
        }
    }
}

/**
 * Unlock all polyhedrons in any windows, such as when new scene/spec is loaded
 */
void
Annotation::unlockAllPolyhedronsInAllWindows()
{
    s_selectionLockedPolyhedronInWindow.fill(NULL);
}

/**
 * Unlock the the locked polyhedron in the given window
 * @param windowIndex
 *    Index of window
 */
void
Annotation::unlockPolyhedronInWindow(const int32_t windowIndex)
{
    setSelectionLockedPolyhedronInWindow(windowIndex,
                                         NULL);
}

