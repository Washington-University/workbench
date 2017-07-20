#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

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

#include <bitset>

#include "AnnotationAttributesDefaultTypeEnum.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationGroupKey.h"
#include "AnnotationSizingHandleTypeEnum.h"
#include "AnnotationTypeEnum.h"
#include "CaretColorEnum.h"
#include "CaretObjectTracksModification.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationSpatialModification;
    class DisplayGroupAndTabItemHelper;
    class SceneClassAssistant;

    class Annotation : public CaretObjectTracksModification, public DisplayGroupAndTabItemInterface, public SceneableInterface {
        
    public:
        /**
         * Properties supported by an annotation
         */
        enum class Property : int32_t {
            /** Invalid (for internal use only) */
            INVALID,
            /** Annotation can be arranged by user */
            ARRANGE,
            /** Annotation can be moved by user */
            COORDINATE,
            /** Annotation allows cut, copy, and paste by user*/
            COPY_CUT_PASTE,
            /** Annotation may be deleted by user */
            DELETE,
            /** Annotation display controlled by Display Group and Tab */
            DISPLAY_GROUP,
            /** Annotation has fill (background) color */
            FILL_COLOR,
            /** Annotation can be grouped by user */
            GROUP,
            /** Annotation has arrows at its line endpoints */
            LINE_ARROWS,
            /** Annotation has a line color */
            LINE_COLOR,
            /** Annotation has line thickness */
            LINE_THICKNESS,
            /** Annotation can be rotated by user */
            ROTATION,
            /** Annotation is selectable */
            SELECT,
            /** Annotation has alignment of text */
            TEXT_ALIGNMENT,
            /** Annotation allows connection to brainordinate */
            TEXT_CONNECT_TO_BRAINORDINATE,
            /** Annotation allows editing of its text by user */
            TEXT_EDIT,
            /** Annotation has text color */
            TEXT_COLOR,
            /** Annotation has font name */
            TEXT_FONT_NAME,
            /** Annotation has font size*/
            TEXT_FONT_SIZE,
            /** Annotation has font style */
            TEXT_FONT_STYLE,
            /** Annotation has orientation of text */
            TEXT_ORIENTATION,
            /** Count of properties MUST BE LAST */
            COUNT_FOR_BITSET
        };
        
        Annotation(const AnnotationTypeEnum::Enum type,
                   const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~Annotation();
        
        Annotation(const Annotation& obj);

        Annotation& operator=(const Annotation& obj);
       
        static Annotation* newAnnotationOfType(const AnnotationTypeEnum::Enum annotationType,
                                               const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        Annotation* clone() const;
        
        bool testProperty(const Property property) const;
        
        bool testPropertiesAny(const Property propertyOne,
                               const Property propertyTwo,
                               const Property propertyThree = Property::INVALID,
                               const Property propertyFour  = Property::INVALID,
                               const Property propertyFive  = Property::INVALID) const;
        
        void setProperty(const Property property,
                         const bool value = true);
        
        void resetProperty(const Property property);
        
        void setPropertiesForChartAxisTitle();
        
        AnnotationGroupKey getAnnotationGroupKey() const;
        
        int32_t getUniqueKey() const;
        
        AString getName() const;
        
        void replaceWithCopyOfAnnotation(const Annotation* annotation);
        
        QString getShortDescriptiveString() const;
        
        void getTextForPasteMenuItems(AString& pasteMenuItemText,
                                      AString& pasteSpecialMenuItemText) const;
        
        AnnotationTypeEnum::Enum getType() const ;
        
        AnnotationCoordinateSpaceEnum::Enum getCoordinateSpace() const;
        
        void setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace);
        
        int32_t getTabIndex() const;
        
        void setTabIndex(const int32_t tabIndex);
        
        int32_t getWindowIndex() const;
        
        void setWindowIndex(const int32_t windowIndex);
        
        void getViewportCoordinateSpaceViewport(int viewportOut[4]) const;
        
        void setViewportCoordinateSpaceViewport(const int viewport[4]);
        
        CaretColorEnum::Enum getLineColor() const;

        void setLineColor(const CaretColorEnum::Enum color);

        void getLineColorRGBA(float rgbaOut[4]) const;
        
        void getLineColorRGBA(uint8_t rgbaOut[4]) const;
        
        CaretColorEnum::Enum getBackgroundColor() const;
        
        void setBackgroundColor(const CaretColorEnum::Enum color);
        
        void getBackgroundColorRGBA(float rgbaOut[4]) const;
        
        void getBackgroundColorRGBA(uint8_t rgbaOut[4]) const;
        
        void getCustomLineColor(float rgbaOut[4]) const;
        
        void getCustomLineColor(uint8_t rgbaOut[4]) const;
        
        void setCustomLineColor(const float rgba[4]);
        
        void setCustomLineColor(const uint8_t rgba[4]);
        
        void getCustomBackgroundColor(float rgbaOut[4]) const;
        
        void getCustomBackgroundColor(uint8_t rgbaOut[4]) const;
        
        void setCustomBackgroundColor(const float rgba[4]);
        
        void setCustomBackgroundColor(const uint8_t rgba[4]);
        
        float getLineWidth() const;
        
        void setLineWidth(const float lineWidth);
        
        bool isSelectedForEditing(const int32_t windowIndex) const;
        
        static void setUserDefaultLineColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultCustomLineColor(const float rgba[4]);
        
        static void setUserDefaultForTextLineColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultForTextCustomLineColor(const float rgba[4]);
        
        static void setUserDefaultBackgroundColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultCustomBackgroundColor(const float rgba[4]);
        
        static void setUserDefaultLineWidth(const float lineWidth);
        
        virtual bool isFixedAspectRatio() const;
        
        virtual float getFixedAspectRatio() const;
        
        /**
         * Apply a spatial modification to an annotation.
         *
         * @param spatialModification
         *     Contains information about the spatial modification.
         * @return
         *     True if the annotation was modified, else false.
         */
        virtual bool applySpatialModification(const AnnotationSpatialModification& spatialModification) = 0;
        
        /**
         * Is the given sizing handle valid for this annotation?
         * 
         * @sizingHandle
         *    The sizing handle.
         * @return
         *    True if sizing handle valid, else false.
         */
        virtual bool isSizeHandleValid(const AnnotationSizingHandleTypeEnum::Enum sizingHandle) const = 0;
        
        virtual void applyColoringFromOther(const Annotation* otherAnnotation);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation) = 0;
        
        static void relativeXYZToViewportXYZ(const float relativeXYZ[3],
                                             const float viewportWidth,
                                             const float viewportHeight,
                                             float viewportXYZOut[3]);
        
        static void viewportXYZToRelativeXYZ(const float viewportXYZ[3],
                                             const float viewportWidth,
                                             const float viewportHeight,
                                             float relativeXYZOut[3]);
        
        static void viewportXYZToLimitedRelativeXYZ(const float viewportXYZ[3],
                                             const float viewportWidth,
                                             const float viewportHeight,
                                             float relativeXYZOut[3]);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        
        
        virtual int32_t getNumberOfItemChildren() const;
        
        virtual DisplayGroupAndTabItemInterface* getItemChild(const int32_t index) const;
        
        virtual std::vector<DisplayGroupAndTabItemInterface*> getItemChildren() const;
        
        virtual DisplayGroupAndTabItemInterface* getItemParent() const;
        
        virtual void setItemParent(DisplayGroupAndTabItemInterface* itemParent);

        virtual AString getItemName() const;
        
        virtual void getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                           float outlineRgbaOut[4],
                                           float textRgbaOut[4]) const;
        
        virtual bool isItemExpandable() const;
        
        virtual bool isItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex) const;
        
        virtual void setItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex,
                                     const bool status);
        
        virtual TriStateSelectionStatusEnum::Enum getItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex) const;
        
        virtual void setItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex,
                                     const TriStateSelectionStatusEnum::Enum status);
        
        virtual bool isItemSelectedForEditingInWindow(const int32_t windowIndex);
        
        void setDrawnInWindowStatus(const int32_t windowIndex);
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) = 0;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotation(const Annotation& obj);

        void initializeAnnotationMembers();
        
        void initializeProperties();
        
        void initializePropertiesForViewportSpaceAnnotation();
        
        // private - AnnotationManager handles selection and allowing
        // public access to this method could cause improper selection status
        void setSelectedForEditing(const int32_t windowIndex,
                         const bool selectedStatus) const;
        
        const AnnotationTypeEnum::Enum m_type;
        
    protected:
        void setDeselectedForEditing();
        
        const AnnotationAttributesDefaultTypeEnum::Enum m_attributeDefaultType;
        
        void textAnnotationResetName();
        
        bool isDrawnInWindowStatus(const int32_t windowIndex);
        
        void clearDrawnInWindowStatusForAllWindows();
        
    private:
        void setAnnotationGroupKey(const AnnotationGroupKey& annotationGroupKey);

        void invalidateAnnotationGroupKey();
        
        void setUniqueKey(const int32_t uniqueKey);
        
        SceneClassAssistant* m_sceneAssistant;
        
        DisplayGroupAndTabItemHelper* m_displayGroupAndTabItemHelper;
        
        AnnotationCoordinateSpaceEnum::Enum  m_coordinateSpace;
        
        int32_t m_tabIndex;
        
        int32_t m_windowIndex;
        
        int32_t m_viewportCoordinateSpaceViewport[4];
        
        CaretColorEnum::Enum m_colorLine;
        
        CaretColorEnum::Enum m_colorBackground;
        
        float m_customColorLine[4];
        
        float m_customColorBackground[4];
        
        float m_lineWidth;

        AString m_name;
        
        int32_t m_uniqueKey;
        
        AnnotationGroupKey m_annotationGroupKey;
        
        bool m_drawnInWindowStatus[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        /**
         * Selection (NOT DISPLAY) status in each window.
         *
         * Number of elements must be same as Constants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS
         * An assertion will fail in cxx file the if number of elements differs.
         */
        mutable std::bitset<10> m_selectedForEditingInWindowFlag;
        
        /**
         * Contains annotation properties.  In the cxx file, an assertion will fail
         * if the number of elements is insufficient.
         */
        std::bitset<32> m_properties;
        
        static CaretColorEnum::Enum s_userDefaultColorLine;
        
        static float s_userDefaultCustomColorLine[4];
        
        static CaretColorEnum::Enum s_userDefaultForTextColorLine;
        
        static float s_userDefaultForTextCustomColorLine[4];
        
        static CaretColorEnum::Enum s_userDefaultColorBackground;
        
        static float s_userDefaultCustomColorBackground[4];
        
        static float s_userDefaultLineWidth;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFile;
        friend class AnnotationFileXmlReader;
        friend class AnnotationGroup;
        friend class AnnotationManager;
    };
    
#ifdef __ANNOTATION_DECLARE__
    CaretColorEnum::Enum Annotation::s_userDefaultColorLine = CaretColorEnum::NONE;
    
    float Annotation::s_userDefaultCustomColorLine[4] = { 1.0, 1.0, 1.0, 1.0 };
    
    CaretColorEnum::Enum Annotation::s_userDefaultForTextColorLine = CaretColorEnum::NONE;
    
    float Annotation::s_userDefaultForTextCustomColorLine[4] = { 1.0, 1.0, 1.0, 1.0 };
    
    CaretColorEnum::Enum Annotation::s_userDefaultColorBackground = CaretColorEnum::NONE;
    
    float Annotation::s_userDefaultCustomColorBackground[4] = { 0.0, 0.0, 0.0, 1.0 };
    
    float Annotation::s_userDefaultLineWidth = 3.0;
#endif // __ANNOTATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_H__
