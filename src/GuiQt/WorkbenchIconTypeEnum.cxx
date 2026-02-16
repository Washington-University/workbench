
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include <algorithm>
#define __WORKBENCH_ICON_TYPE_ENUM_DECLARE__
#include "WorkbenchIconTypeEnum.h"
#undef __WORKBENCH_ICON_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WorkbenchIconTypeEnum 
 * \brief 
 *
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_workbenchIconTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void workbenchIconTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WorkbenchIconTypeEnum.h"
 * 
 *     Instatiate:
 *         m_workbenchIconTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_workbenchIconTypeEnumComboBox->setup<WorkbenchIconTypeEnum,WorkbenchIconTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_workbenchIconTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(workbenchIconTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_workbenchIconTypeEnumComboBox->setSelectedItem<WorkbenchIconTypeEnum,WorkbenchIconTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WorkbenchIconTypeEnum::Enum VARIABLE = m_workbenchIconTypeEnumComboBox->getSelectedItem<WorkbenchIconTypeEnum,WorkbenchIconTypeEnum::Enum>();
 * 
 */

/*
switch (value) {
    case WorkbenchIconTypeEnum::NO_ICON:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_DELETE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_ARROW:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_BOX:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_IMAGE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_LINE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_MARKER:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_OVAL:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYHEDRON:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYGON:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYLINE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_TEXT:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_CHART:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_HISTOLOGY:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_STEREOTAXIC:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_STEREOTAXIC:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_SURFACE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_TAB:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_WINDOW:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_HORIZONTAL:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_VERTICAL:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE:
        break;
    case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_ANTERIOR:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_DORSAL:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_LEFT:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_LEFT_LATERAL:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_LEFT_MEDIAL:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_POSTERIOR:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_REDO:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_REGION:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_RIGHT:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_LATERAL:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_MEDIAL:
         break;
    case WorkbenchIconTypeEnum::ORIENTATION_UNDO:
        break;
    case WorkbenchIconTypeEnum::ORIENTATION_VENTRAL:
        break;
    case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_COLOR_BAR:
        break;
    case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_CONSTRUCT:
        break;
    case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_WRENCH:
        break;
    case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_DELETE:
        break;
    case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_LOAD:
        break;
    case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_OPTIONS:
        break;
    case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_RELOAD:
        break;
    case WorkbenchIconTypeEnum::TABBAR_DATA_TOOLTIPS:
        break;
    case WorkbenchIconTypeEnum::TABBAR_FEATURES:
        break;
    case WorkbenchIconTypeEnum::TABBAR_HELP:
        break;
    case WorkbenchIconTypeEnum::TABBAR_IDENTIFY_BRAINORDINATE:
        break;
    case WorkbenchIconTypeEnum::TABBAR_INFORMATION:
        break;
    case WorkbenchIconTypeEnum::TABBAR_MACROS_SCROLL:
        break;
    case WorkbenchIconTypeEnum::TABBAR_MOVIE:
        break;
    case WorkbenchIconTypeEnum::TABBAR_OVERLAYS:
        break;
    case WorkbenchIconTypeEnum::TABBAR_SCENES_CLAP_BOARD:
        break;
    case WorkbenchIconTypeEnum::TABBAR_TOOLBAR:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_MISC_LIGHT_BULB:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_RELOCK_ARROW:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_MISC_RULER:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_MISC_SCISSORS:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_AXIAL:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL:
        break;
    case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL:
        break;
}
*/

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 *
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
WorkbenchIconTypeEnum::WorkbenchIconTypeEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
WorkbenchIconTypeEnum::~WorkbenchIconTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WorkbenchIconTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WorkbenchIconTypeEnum(NO_ICON, 
                                    "NO_ICON", 
                                    "No Icon"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_DELETE, 
                                    "ANNOTATION_DELETE", 
                                    "Annotation Delete"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_LINE_ARROW_DOWN, 
                                    "ANNOTATION_LINE_ARROW_DOWN", 
                                    "Annotation Line Arrow Down"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_LINE_ARROW_UP, 
                                    "ANNOTATION_LINE_ARROW_UP", 
                                    "Annotation Line Arrow Up"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_ARROW, 
                                    "ANNOTATION_NEW_SHAPE_ARROW", 
                                    "Annotation New Shape Arrow"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_BOX, 
                                    "ANNOTATION_NEW_SHAPE_BOX", 
                                    "Annotation New Shape Box"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_IMAGE, 
                                    "ANNOTATION_NEW_SHAPE_IMAGE", 
                                    "Annotation New Shape Image"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_LINE, 
                                    "ANNOTATION_NEW_SHAPE_LINE", 
                                    "Annotation New Shape Line"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_MARKER, 
                                    "ANNOTATION_NEW_SHAPE_MARKER", 
                                    "Annotation New Shape Marker"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_OVAL, 
                                    "ANNOTATION_NEW_SHAPE_OVAL", 
                                    "Annotation New Shape Oval"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_POLYHEDRON, 
                                    "ANNOTATION_NEW_SHAPE_POLYHEDRON", 
                                    "Annotation New Shape Polyhedron"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_POLYGON, 
                                    "ANNOTATION_NEW_SHAPE_POLYGON", 
                                    "Annotation New Shape Polygon"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_POLYLINE, 
                                    "ANNOTATION_NEW_SHAPE_POLYLINE", 
                                    "Annotation New Shape Polyline"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SHAPE_TEXT, 
                                    "ANNOTATION_NEW_SHAPE_TEXT", 
                                    "Annotation New Shape Text"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_CHART, 
                                    "ANNOTATION_NEW_SPACE_CHART", 
                                    "Annotation New Space Chart"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_HISTOLOGY, 
                                    "ANNOTATION_NEW_SPACE_HISTOLOGY", 
                                    "Annotation New Space Histology"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL, 
                                    "ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL", 
                                    "Annotation New Space Media File Name And Pixel"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_STEREOTAXIC, 
                                    "ANNOTATION_NEW_SPACE_STEREOTAXIC", 
                                    "Annotation New Space Stereotaxic"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_SURFACE, 
                                    "ANNOTATION_NEW_SPACE_SURFACE", 
                                    "Annotation New Space Surface"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_TAB, 
                                    "ANNOTATION_NEW_SPACE_TAB", 
                                    "Annotation New Space Tab"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_NEW_SPACE_WINDOW, 
                                    "ANNOTATION_NEW_SPACE_WINDOW", 
                                    "Annotation New Space Window"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ORIENTATION_HORIZONTAL,
                                    "ANNOTATION_TEXT_ORIENTATION_HORIZONTAL",
                                    "Annotation Text Orientation Horizontal"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ORIENTATION_VERTICAL,
                                    "ANNOTATION_TEXT_ORIENTATION_VERTICAL",
                                    "Annotation Text Orientation Vertical"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_HORIZ_CENTER, 
                                    "ANNOTATION_TEXT_ALIGN_HORIZ_CENTER", 
                                    "Annotation Text Align Horiz Center"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_HORIZ_LEFT, 
                                    "ANNOTATION_TEXT_ALIGN_HORIZ_LEFT", 
                                    "Annotation Text Align Horiz Left"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT, 
                                    "ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT", 
                                    "Annotation Text Align Horiz Right"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_VERT_BOTTOM, 
                                    "ANNOTATION_TEXT_ALIGN_VERT_BOTTOM", 
                                    "Annotation Text Align Vert Bottom"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_VERT_MIDDLE, 
                                    "ANNOTATION_TEXT_ALIGN_VERT_MIDDLE", 
                                    "Annotation Text Align Vert Middle"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ANNOTATION_TEXT_ALIGN_VERT_TOP, 
                                    "ANNOTATION_TEXT_ALIGN_VERT_TOP", 
                                    "Annotation Text Align Vert Top"));
        
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_ANTERIOR, 
                                    "ORIENTATION_ANTERIOR", 
                                    "Orientation Anterior"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_DORSAL, 
                                    "ORIENTATION_DORSAL", 
                                    "Orientation Dorsal"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_LEFT, 
                                    "ORIENTATION_LEFT", 
                                    "Orientation Left"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_LEFT_LATERAL,
                                             "ORIENTATION_LEFT_LATERAL",
                                             "Orientation Left Lateral"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_LEFT_MEDIAL,
                                             "ORIENTATION_LEFT_MEDIAL",
                                             "Orientation Left Medial"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_POSTERIOR, 
                                    "ORIENTATION_POSTERIOR", 
                                    "Orientation Posterior"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_REDO, 
                                    "ORIENTATION_REDO", 
                                    "Orientation Redo"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_RIGHT,
                                             "ORIENTATION_RIGHT",
                                             "Orientation Right"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_RIGHT_LATERAL,
                                             "ORIENTATION_RIGHT_LATERAL",
                                             "Orientation Right Lateral"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_RIGHT_MEDIAL,
                                             "ORIENTATION_RIGHT_MEDIAL",
                                             "Orientation Right Medial"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_REGION,
                                    "ORIENTATION_REGION", 
                                    "Orientation Region"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_UNDO, 
                                    "ORIENTATION_UNDO", 
                                    "Orientation Undo"));
    
    enumData.push_back(WorkbenchIconTypeEnum(ORIENTATION_VENTRAL, 
                                    "ORIENTATION_VENTRAL", 
                                    "Orientation Ventral"));
    
    enumData.push_back(WorkbenchIconTypeEnum(OVERLAY_TOOLBOX_COLOR_BAR, 
                                    "OVERLAY_TOOLBOX_COLOR_BAR", 
                                    "Overlay Toolbox Color Bar"));
    
    enumData.push_back(WorkbenchIconTypeEnum(OVERLAY_TOOLBOX_CONSTRUCT, 
                                    "OVERLAY_TOOLBOX_CONSTRUCT", 
                                    "Overlay Toolbox Construct"));
    
    enumData.push_back(WorkbenchIconTypeEnum(OVERLAY_TOOLBOX_WRENCH, 
                                    "OVERLAY_TOOLBOX_WRENCH", 
                                    "Overlay Toolbox Wrench"));
    
    enumData.push_back(WorkbenchIconTypeEnum(SPEC_FILE_DIALOG_DELETE,
                                             "SPEC_FILE_DIALOG_DELETE",
                                             "Spec File Dialog Delete"));
    
    enumData.push_back(WorkbenchIconTypeEnum(SPEC_FILE_DIALOG_LOAD,
                                             "SPEC_FILE_DIALOG_LOAD",
                                             "Spec File Dialog Load"));
    
    enumData.push_back(WorkbenchIconTypeEnum(SPEC_FILE_DIALOG_OPTIONS,
                                             "SPEC_FILE_DIALOG_OPTIONS",
                                             "Spec File Dialog Options"));
    
    enumData.push_back(WorkbenchIconTypeEnum(SPEC_FILE_DIALOG_RELOAD,
                                             "SPEC_FILE_DIALOG_RELOAD",
                                             "Spec File Dialog Reload"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_DATA_TOOLTIPS,
                                    "TABBAR_DATA_TOOLTIPS", 
                                    "Tabbar Data Tooltips"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_FEATURES, 
                                    "TABBAR_FEATURES", 
                                    "Tabbar Features"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_IDENTIFY_BRAINORDINATE,
                                             "TABBAR_IDENTIFY_BRAINORDINATE",
                                             "Tabbar Identify Brainordinate"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_INFORMATION, 
                                    "TABBAR_INFORMATION", 
                                    "Tabbar Information"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_MACROS_SCROLL, 
                                    "TABBAR_MACROS_SCROLL", 
                                    "Tabbar Macros Scroll"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_MOVIE, 
                                    "TABBAR_MOVIE", 
                                    "Tabbar Movie"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_OVERLAYS, 
                                    "TABBAR_OVERLAYS", 
                                    "Tabbar Overlays"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_HELP,
                                    "TABBAR_HELP",
                                    "Tabbar Help"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_SCENES_CLAP_BOARD, 
                                    "TABBAR_SCENES_CLAP_BOARD", 
                                    "Tabbar Scenes Clap Board"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TABBAR_TOOLBAR, 
                                    "TABBAR_TOOLBAR", 
                                    "Tabbar Toolbar"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_MISC_LIGHT_BULB, 
                                    "TOOLBAR_MISC_LIGHT_BULB", 
                                    "Toolbar Misc Light Bulb"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_RELOCK_ARROW, 
                                    "TOOLBAR_RELOCK_ARROW", 
                                    "Toolbar Relock Arrow"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_MISC_RULER, 
                                    "TOOLBAR_MISC_RULER", 
                                    "Toolbar Misc Ruler"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_MISC_SCISSORS, 
                                    "TOOLBAR_MISC_SCISSORS", 
                                    "Toolbar Misc Scissors"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS, 
                                    "TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS", 
                                    "Toolbar Slice Indices Move Crosshairs"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_PLANE_AXIAL,
                                    "TOOLBAR_VOLUME_SLICE_PLANE_AXIAL",
                                    "Toolbar Volume Slice Plane Axial"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_CROSSHAIRS, 
                                    "TOOLBAR_VOLUME_SLICE_CROSSHAIRS", 
                                    "Toolbar Volume Slice Crosshairs"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS, 
                                    "TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS", 
                                    "Toolbar Volume Slice Crosshair Labels"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_PLANE_ALL, 
                                    "TOOLBAR_VOLUME_SLICE_PLANE_ALL", 
                                    "Toolbar Volume Slice Plane All"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_PLANE_CORONAL, 
                                    "TOOLBAR_VOLUME_SLICE_PLANE_CORONAL", 
                                    "Toolbar Volume Slice Plane Coronal"));
    
    enumData.push_back(WorkbenchIconTypeEnum(TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL, 
                                    "TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL", 
                                    "Toolbar Volume Slice Plane Parasagittal"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WorkbenchIconTypeEnum*
WorkbenchIconTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WorkbenchIconTypeEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
WorkbenchIconTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WorkbenchIconTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param name 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
WorkbenchIconTypeEnum::Enum 
WorkbenchIconTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WorkbenchIconTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WorkbenchIconTypeEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type WorkbenchIconTypeEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
WorkbenchIconTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WorkbenchIconTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
WorkbenchIconTypeEnum::Enum 
WorkbenchIconTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WorkbenchIconTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WorkbenchIconTypeEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type WorkbenchIconTypeEnum"));
    }
    return enumValue;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
WorkbenchIconTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WorkbenchIconTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
WorkbenchIconTypeEnum::Enum
WorkbenchIconTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WorkbenchIconTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WorkbenchIconTypeEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type WorkbenchIconTypeEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
WorkbenchIconTypeEnum::getAllEnums(std::vector<WorkbenchIconTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Get all of the names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
WorkbenchIconTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WorkbenchIconTypeEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the GUI names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
WorkbenchIconTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WorkbenchIconTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WorkbenchIconTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

