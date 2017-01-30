
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __MAP_YOKING_GROUP_COMBO_BOX_DECLARE__
#include "MapYokingGroupComboBox.h"
#undef __MAP_YOKING_GROUP_COMBO_BOX_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoOverlay.h"
#include "ChartableMatrixSeriesInterface.h"
#include "CiftiScalarDataSeriesFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventMapYokingValidation.h"
#include "Overlay.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::MapYokingGroupComboBox 
 * \brief Combo box for selection of a map yoking group.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapYokingGroupComboBox::MapYokingGroupComboBox(QObject* parent)
: WuQWidget(parent)
{
    m_comboBox = new EnumComboBoxTemplate(this);
    m_comboBox->setup<MapYokingGroupEnum, MapYokingGroupEnum::Enum>();
    m_comboBox->getWidget()->setStatusTip("Synchronize selected map indices (and selection status for overlays)");
    m_comboBox->getWidget()->setToolTip("Synchronize selected map indices (and selection status for overlays)");
#ifdef CARET_OS_MACOSX
    m_comboBox->getComboBox()->setFixedWidth(m_comboBox->getComboBox()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
    QObject::connect(m_comboBox, SIGNAL(itemActivated()),
                     this, SLOT(comboBoxActivated()));
}

/**
 * Destructor.
 */
MapYokingGroupComboBox::~MapYokingGroupComboBox()
{
}

/**
 * Called when the user selects a yoking group.
 * Verify compatibility before accepting the selection.
 */
void
MapYokingGroupComboBox::comboBoxActivated()
{
    emit itemActivated();
//    MapYokingGroupEnum::Enum mapYokingGroup = getMapYokingGroup();
//    EventMapYokingValidation validateEvent(mapYokingGroup);
}

/**
 * @return The widget.
 */
QWidget*
MapYokingGroupComboBox::getWidget()
{
    return m_comboBox->getWidget();
}

/**
 * @return The selected map yoking group.
 */
MapYokingGroupEnum::Enum
MapYokingGroupComboBox::getMapYokingGroup() const
{
    return m_comboBox->getSelectedItem<MapYokingGroupEnum, MapYokingGroupEnum::Enum>();
}

/**
 * Set the map yoking group.
 *
 * @param mapYokingGroup
 *    The map yoking group.
 */
void
MapYokingGroupComboBox::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_comboBox->setSelectedItem<MapYokingGroupEnum, MapYokingGroupEnum::Enum>(mapYokingGroup);
}

/**
 * Validate a change in yoking for a matrix series file.
 *
 * @param chartableMatrixSeriesInterface
 *     Matrix series file that has yoking changed.
 * @param tabIndex
 *     Index of tab for the file.
 */
void
MapYokingGroupComboBox::validateYokingChange(ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface,
                                             const int32_t tabIndex)
{
    if (chartableMatrixSeriesInterface != NULL) {
        int32_t mapIndex = chartableMatrixSeriesInterface->getSelectedMapIndex(tabIndex);
        const MapYokingGroupEnum::Enum previousMapYokingGroup = chartableMatrixSeriesInterface->getMatrixRowColumnMapYokingGroup(tabIndex);
        const MapYokingGroupEnum::Enum newYokingGroup = getMapYokingGroup();
        CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(chartableMatrixSeriesInterface);
        CaretAssert(mapFile);
        bool selectionStatus = true;
        
        if ((mapFile != NULL)
            && (mapIndex >= 0)) {
            const YokeValidationResult result = validateYoking(mapFile,
                                                         mapIndex,
                                                         selectionStatus);
            
            switch (result) {
                case YOKE_VALIDATE_RESULT_ACCEPT:
                    chartableMatrixSeriesInterface->setMatrixRowColumnMapYokingGroup(tabIndex, newYokingGroup);
                    chartableMatrixSeriesInterface->setSelectedMapIndex(tabIndex, mapIndex);
                    break;
                case YOKE_VALIDATE_RESULT_OFF:
                    chartableMatrixSeriesInterface->setMatrixRowColumnMapYokingGroup(tabIndex,
                                                                   MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
                    break;
                case YOKE_VALIDATE_RESULT_PREVIOUS:
                    chartableMatrixSeriesInterface->setMatrixRowColumnMapYokingGroup(tabIndex,
                                                                   previousMapYokingGroup);
                    break;
            }
            
            setMapYokingGroup(chartableMatrixSeriesInterface->getMatrixRowColumnMapYokingGroup(tabIndex));
        }
    }
}

/**
 * Validate a change in yoking for an overlay.
 *
 * @param overlay
 *    Overlay whose yoking changes.
 */
void
MapYokingGroupComboBox::validateYokingChange(Overlay* overlay)
{
    const MapYokingGroupEnum::Enum previousMapYokingGroup = overlay->getMapYokingGroup();
    const MapYokingGroupEnum::Enum newYokingGroup = getMapYokingGroup();
    CaretMappableDataFile* mapFile = NULL;
    int32_t mapIndex = -1;
    overlay->getSelectionData(mapFile, mapIndex);
    bool selectionStatus = overlay->isEnabled();
    
    if ((mapFile != NULL)
        && (mapIndex >= 0)) {
        const YokeValidationResult result = validateYoking(mapFile,
                                                     mapIndex,
                                                     selectionStatus);
        
        switch (result) {
            case YOKE_VALIDATE_RESULT_ACCEPT:
                overlay->setEnabled(selectionStatus);
                overlay->setSelectionData(mapFile,
                                          mapIndex);
                overlay->setMapYokingGroup(newYokingGroup);
                break;
            case YOKE_VALIDATE_RESULT_OFF:
                overlay->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
                break;
            case YOKE_VALIDATE_RESULT_PREVIOUS:
                overlay->setMapYokingGroup(previousMapYokingGroup);
                break;
        }

        setMapYokingGroup(overlay->getMapYokingGroup());
    }
}

/**
 * Validate a change in yoking for a chart overlay.
 *
 * @param chartOverlay
 *    Chart overlay whose yoking changes.
 */
void
MapYokingGroupComboBox::validateYokingChange(ChartTwoOverlay* chartOverlay)
{
    const MapYokingGroupEnum::Enum previousMapYokingGroup = chartOverlay->getMapYokingGroup();
    const MapYokingGroupEnum::Enum newYokingGroup = getMapYokingGroup();
    CaretMappableDataFile* mapFile = NULL;
    int32_t mapIndex = -1;
    chartOverlay->getSelectionData(mapFile, mapIndex);
    bool selectionStatus = chartOverlay->isEnabled();
    
    if ((mapFile != NULL)
        && (mapIndex >= 0)) {
        const YokeValidationResult result = validateYoking(mapFile,
                                                           mapIndex,
                                                           selectionStatus);
        
        switch (result) {
            case YOKE_VALIDATE_RESULT_ACCEPT:
                chartOverlay->setEnabled(selectionStatus);
                chartOverlay->setSelectionData(mapFile,
                                          mapIndex);
                chartOverlay->setMapYokingGroup(newYokingGroup);
                break;
            case YOKE_VALIDATE_RESULT_OFF:
                chartOverlay->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
                break;
            case YOKE_VALIDATE_RESULT_PREVIOUS:
                chartOverlay->setMapYokingGroup(previousMapYokingGroup);
                break;
        }
        
        setMapYokingGroup(chartOverlay->getMapYokingGroup());
    }
}

/**
 * Validate yoking when a new file is added to a yoking group.
 *
 * @param previousMapYokingGroup
 *     The previous yoking group.
 * @param selectedFile
 *     The file that the user would like to yoke.
 * @param selectedMapIndexInOut
 *     The current map selected for the file.  Its value will be updated
 *     if yoking is selected (turned on or changed).
 * @param selectionStatusInOut
 *     The selection status for an overlay.  Its value will be updated
 *     if yoking is selected (turned on or changed).
 */
MapYokingGroupComboBox::YokeValidationResult
MapYokingGroupComboBox::validateYoking(CaretMappableDataFile* selectedFile,
                                       int32_t& selectedMapIndexInOut,
                                       bool& /* selectionStatusInOut */)
{
    YokeValidationResult yokeResult = YOKE_VALIDATE_RESULT_OFF; //YOKE_VALIDATE_RESULT_PREVIOUS;
    
    MapYokingGroupEnum::Enum newYokingGroup = getMapYokingGroup();
    if (newYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        if ((selectedFile != NULL)
            && (selectedMapIndexInOut >= 0)) {
            /*
             * Get info on yoking selections
             */
            EventMapYokingValidation validateEvent(newYokingGroup);
            EventManager::get()->sendEvent(validateEvent.getPointer());
            
            /*
             * Check compatibility based (number of maps in yoked files)
             * and warn use if there is an incompatibility.
             */
            int32_t numberOfYokedFiles = 0;
            AString message;
            if (validateEvent.validateCompatibility(selectedFile,
                                                    numberOfYokedFiles,
                                                    message)) {
                yokeResult = YOKE_VALIDATE_RESULT_ACCEPT;
            }
            else {
                message.appendWithNewLine("");
                message.appendWithNewLine("Allow yoking?");
                
                message = WuQtUtilities::createWordWrappedToolTipText(message);
                
                WuQMessageBox::YesNoCancelResult result =
                WuQMessageBox::warningYesNoCancel(m_comboBox->getWidget(),
                                                  message,
                                                  "");
                switch (result) {
                    case WuQMessageBox::RESULT_YES:
                        yokeResult = YOKE_VALIDATE_RESULT_ACCEPT;
                        break;
                    case WuQMessageBox::RESULT_NO:
                        yokeResult = YOKE_VALIDATE_RESULT_OFF;
                        break;
                    case WuQMessageBox::RESULT_CANCEL:
                        yokeResult = YOKE_VALIDATE_RESULT_PREVIOUS;
                        break;
                }
            }
            
            if (yokeResult == YOKE_VALIDATE_RESULT_ACCEPT) {
                if (numberOfYokedFiles > 0) {
                    /*
                     * Already have files yoked to this group so use
                     * the map index and status from the yoking group.
                     */
                    selectedMapIndexInOut = MapYokingGroupEnum::getSelectedMapIndex(newYokingGroup);
                    //selectionStatusInOut  = MapYokingGroupEnum::isEnabled(newYokingGroup);
                }
                else {
                    /*
                     * This is the first file added to the yoking group
                     * so set the map index and status in the yoking group
                     * to the file's selections.
                     */
                    MapYokingGroupEnum::setSelectedMapIndex(newYokingGroup,
                                                            selectedMapIndexInOut);
                    //MapYokingGroupEnum::setEnabled(newYokingGroup,
                    //                               selectionStatusInOut);
                }
            }
        }
    }
    
    return yokeResult;
}


//void
//MapYokingGroupComboBox::validateYokingChange(const MapYokingGroupEnum::Enum previousMapYokingGroup,
//                                             CaretMappableDataFile* selectedFile,
//                                             const int32_t selectedMapIndex,
//                                             ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface,
//                                             Overlay* overlay)
//{
//    MapYokingGroupEnum::Enum yokingGroup = getMapYokingGroup();
//    if (yokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        CaretMappableDataFile* selectedFile = NULL;
//        if ((selectedFile != NULL)
//            && (selectedMapIndex >= 0)) {
//            /*
//             * Get info on yoking selections
//             */
//            EventMapYokingValidation validateEvent(yokingGroup);
//            EventManager::get()->sendEvent(validateEvent.getPointer());
//
//            const int32_t numOverlaysYoked = yokedOverlaysEvent.getNumberOfYokedOverlays();
//            
//            /*
//             * Check compatibility based (number of maps in yoked files)
//             * and warn use if there is an incompatibility.
//             */
//            AString message;
//            if ( ! validateEvent.validateCompatibility(selectedFile,
//                                                         message)) {
//                message.appendWithNewLine("");
//                message.appendWithNewLine("Allow yoking?");
//                
//                message = WuQtUtilities::createWordWrappedToolTipText(message);
//                
//                WuQMessageBox::YesNoCancelResult result =
//                WuQMessageBox::warningYesNoCancel(m_comboBox->getWidget(),
//                                                  message,
//                                                  "");
//                switch (result) {
//                    case WuQMessageBox::RESULT_YES:
//                        break;
//                    case WuQMessageBox::RESULT_NO:
//                        yokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
//                        break;
//                    case WuQMessageBox::RESULT_CANCEL:
//                        yokingGroup = previousMapYokingGroup;
//                        break;
//                }
//            }
//            
//            if (overlay != NULL) {
//                
//            }
//            overlay->setYokingGroup(yokingGroup);
//            if (yokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//                if (numOverlaysYoked <= 0) {
//                    OverlayYokingGroupEnum::setSelectedMapIndex(yokingGroup,
//                                                                selectedMapIndex);
//                    const bool enabledStatus = overlay->isEnabled();
//                    OverlayYokingGroupEnum::setEnabled(yokingGroup,
//                                                       enabledStatus);
//                }
//            }
//        }
//    }
//    else {
//        overlay->setYokingGroup(yokingGroup);
//    }
//    
//    updateViewController(overlay);
//    
//    this->updateUserInterfaceAndGraphicsWindow();
//    
//}
//

