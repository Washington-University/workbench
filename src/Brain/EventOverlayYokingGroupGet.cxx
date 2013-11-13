
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __EVENT_OVERLAY_GET_YOKED_DECLARE__
#include "EventOverlayYokingGroupGet.h"
#undef __EVENT_OVERLAY_GET_YOKED_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventTypeEnum.h"
#include "Overlay.h"

using namespace caret;


    
/**
 * \class caret::EventOverlayYokingGroupGet 
 * \brief Get all overlays yoked to a specific yoking group.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param yokingGroup
 *    Yoking group for matching to overlays.
 */
EventOverlayYokingGroupGet::EventOverlayYokingGroupGet(const OverlayYokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_OVERLAY_GET_YOKED),
m_yokingGroup(yokingGroup)
{
    
}

/**
 * Destructor.
 */
EventOverlayYokingGroupGet::~EventOverlayYokingGroupGet()
{
    
}

/**
 * Get the yoking group for which overlays must match.
 */
OverlayYokingGroupEnum::Enum
EventOverlayYokingGroupGet::getYokingGroup() const
{
    return m_yokingGroup;
}

/**
 * Add an overlay that is yoked to the desired yoking group.
 * If the overlay is not yoked to the desired yoking group,
 * no action is taken.
 *
 * @param browserTabContent
 *    Browser tab containing the overlay.
 * @param overlay
 *    The overlay.
 */
void
EventOverlayYokingGroupGet::addYokedOverlay(const AString& modelName,
                                      const int32_t tabIndex,
                                      Overlay* overlay)
{
    CaretAssert(tabIndex >= 0);
    CaretAssert(overlay);
    
    if (overlay->getYokingGroup() == m_yokingGroup) {
        CaretMappableDataFile* selectedDataFile = NULL;
        int32_t selectedMapIndex;
        overlay->getSelectionData(selectedDataFile,
                                  selectedMapIndex);
        if (selectedDataFile != NULL) {
            YokedOverlayInfo yoi;
            yoi.m_modelName = modelName;
            yoi.m_tabIndex = tabIndex;
            yoi.m_overlay = overlay;
            yoi.m_overlayFile = selectedDataFile;
            yoi.m_overlayFileName = selectedDataFile->getFileNameNoPath();
            yoi.m_overlayFileNumberOfMaps = selectedDataFile->getNumberOfMaps();
            m_yokedOverlays.push_back(yoi);
        }
    }
}

/**
 * @return The number of overlays yoked to the desired yoking group.
 */
int32_t
EventOverlayYokingGroupGet::getNumberOfYokedOverlays() const
{
    return m_yokedOverlays.size();
}

/**
 * Get the overlay at the given index.
 *
 * @param indx
 *    Index of the overlay.
 * @return 
 *    Overlay at the given index.
 */
Overlay*
EventOverlayYokingGroupGet::getYokedOverlay(const int32_t indx)
{
    CaretAssertVectorIndex(m_yokedOverlays, indx);
    const YokedOverlayInfo& yoi = m_yokedOverlays[indx];
    return yoi.m_overlay;
}


/**
 * Get information about an overlay that is yoked to the desired yoking group.
 *
 * @param indx
 *    Index of the item.
 * @param browserTabContentOut
 *    Browser tab containing the overlay.
 * @param overlayOut
 *    The overlay.
 * @param overlayFileOut
 *    File selected in the overlay.
 * @param overlayFileNameOut
 *    Name of the file selected in the overlay.
 * @param overlayFileNumberOfMapsOut
 *    Number of maps in the selected file.
 */
//void
//EventOverlayYokingGroupGet::getYokedOverlay(const int32_t indx,
//                                      AString& modelNameOut,
//                                      int32_t tabIndexOut,
//                                      Overlay*& overlayOut,
//                                      CaretMappableDataFile*& overlayFileOut,
//                                      AString& overlayFileNameOut,
//                                      int32_t& overlayFileNumberOfMapsOut) const
//{
//    CaretAssertVectorIndex(m_yokedOverlays, indx);
//    const YokedOverlayInfo& yoi = m_yokedOverlays[indx];
//    
//    modelNameOut = yoi.m_modelName;
//    tabIndexOut  = yoi.m_tabIndex;
//    overlayOut = yoi.m_overlay;
//    overlayFileOut = yoi.m_overlayFile;
//    overlayFileNameOut = yoi.m_overlayFileName;
//    overlayFileNumberOfMapsOut = yoi.m_overlayFileNumberOfMaps;
//}

///**
// * @return The maximum map index for yoked overlays.  If the files 
// * selected in the overlays contains a different number of maps,
// * the value returned is the minimum number of maps.
// *
// * Note: -1 indicates no overlays are yoked.
// */
//int32_t
//EventOverlayYokingGroupGet::getMaximumMapIndex() const
//{
//    const int32_t numItems = getNumberOfYokedOverlays();
//    if (numItems <= 0) {
//        return -1;
//    }
//    
//    int32_t maximumMapIndex = std::numeric_limits<int32_t>::max();
//    
//    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
//        maximumMapIndex = std::min(maximumMapIndex,
//                                   m_yokedOverlays[itemIndex].m_overlayFileNumberOfMaps);
//    }
//    
//    return maximumMapIndex;
//}
//
///**
// * Set the map index for all yoked files.
// *
// * @param mapIndex
// *    Index of the map for selection.
// */
//void
//EventOverlayYokingGroupGet::setMapIndex(const int32_t mapIndex)
//{
//    const int32_t newMapIndex = std::min(mapIndex,
//                                         getMaximumMapIndex());
//    
//    const int32_t numItems = getNumberOfYokedOverlays();
//    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
//        const YokedOverlayInfo& yoi = m_yokedOverlays[itemIndex];
//        yoi.m_overlay->setSelectionData(yoi.m_overlayFile,
//                                        newMapIndex);
//    }
//}

///**
// * Synchronize the selected maps in all yoked overlays.
// *
// * @param overlay
// *    If there is more than one overlay, do not use this
// *    overlays map index.
// */
//void
//EventOverlayYokingGroupGet::synchronizeSelectedMaps(Overlay* overlay)
//{
//    const int32_t numItems = getNumberOfYokedOverlays();
//    if (numItems == 1) {
//        CaretMappableDataFile* cmdf = NULL;
//        int32_t mapIndex;
//        overlay->getSelectionData(cmdf, mapIndex);
//        setMapIndex(mapIndex);
//    }
//    
//    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
//        const YokedOverlayInfo& yoi = m_yokedOverlays[itemIndex];
//        if (yoi.m_overlay != overlay) {
//            CaretMappableDataFile* cmdf = NULL;
//            int32_t mapIndex;
//            yoi.m_overlay->getSelectionData(cmdf, mapIndex);
//            setMapIndex(mapIndex);
//            break;
//        }
//    }
//}

/**
 * Validate compatibility of the given file with any overlays that
 * are currently yoked.  This must be called after the event 
 * returns.  A file is compatible if it contains the same number of 
 * maps.
 *
 * @param overlayFile
 *    File for testing compatibility.
 * @param messageOut
 *    If there are incompatibilities, this message will describe the issues.
 * @return true if compatable, false otherwise.
 */
bool
EventOverlayYokingGroupGet::validateCompatibility(CaretMappableDataFile* overlayFile,
                                            AString& messageOut) const
{
    messageOut.clear();
    
    CaretAssert(overlayFile);
    
    const int32_t numItems = getNumberOfYokedOverlays();
    if (numItems == 0) {
        return true;
    }
    
    /*
     * Get the yoked files and get counts of maps in the files.  
     * Use a set since a file may be in more than one overlay.
     */
    std::set<CaretMappableDataFile*> yokedFiles;
    std::set<int32_t> mapCounts;
    yokedFiles.insert(overlayFile);
    mapCounts.insert(overlayFile->getNumberOfMaps());
    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
        const YokedOverlayInfo& yoi = m_yokedOverlays[itemIndex];
        yokedFiles.insert(yoi.m_overlayFile);
        mapCounts.insert(yoi.m_overlayFile->getNumberOfMaps());
    }
    
    /*
     * All files contain the same number of maps?
     */
    if (mapCounts.size() <= 1) {
        return true;
    }

    /*
     * Generate message about incompatible number of maps.
     */
    messageOut = "Incompatible number of maps for yoking:\n";
//    messageOut.appendWithNewLine(QString::number(overlayFile->getNumberOfMaps())
//                                 + " maps in "
//                                 + overlayFile->getFileNameNoPath());
    for (int32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
        const YokedOverlayInfo& yoi = m_yokedOverlays[itemIndex];
        messageOut.appendWithNewLine(AString::number(yoi.m_overlayFileNumberOfMaps)
                                     + " maps in "
                                     + yoi.m_overlayFileName
                                     + " in tab "
                                     + QString::number(yoi.m_tabIndex + 1)
                                     + " "
                                     + yoi.m_modelName);
    }
    
    return false;
}

