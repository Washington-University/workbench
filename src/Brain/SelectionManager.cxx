
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "CaretOpenGLInclude.h"
#include <cmath>
#include <limits>

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

#define __SELECTION_MANAGER_DECLARE__
#include "SelectionManager.h"
#undef __SELECTION_MANAGER_DECLARE__

#include "IdentificationFormattedTextGenerator.h"
#include "IdentificationSimpleTextGenerator.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemChartDataSeries.h"
#include "SelectionItemChartFrequencySeries.h"
#include "SelectionItemChartMatrix.h"
#include "SelectionItemChartTimeSeries.h"
#include "SelectionItemChartTwoHistogram.h"
#include "SelectionItemChartTwoLabel.h"
#include "SelectionItemChartTwoLineLayer.h"
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#include "SelectionItemChartTwoLineSeries.h"
#include "SelectionItemChartTwoMatrix.h"
#include "SelectionItemCiftiConnectivityMatrixRowColumn.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocus.h"
#include "SelectionItemHistologyCoordinate.h"
#include "SelectionItemImageControlPoint.h"
#include "SelectionItemMediaLogicalCoordinate.h"
#include "SelectionItemMediaPlaneCoordinate.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemUniversalIdentificationSymbol.h"
#include "SelectionItemVolumeMprCrosshair.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "Surface.h"

using namespace caret;

/**
 * \class SelectionManager
 * \brief Manages identification.
 *
 * Manages identification.
 */


/**
 * Constructor.
 */
SelectionManager::SelectionManager()
: CaretObject()
{
    m_annotationIdentification = new SelectionItemAnnotation();
    m_surfaceBorderIdentification = new SelectionItemBorderSurface();
    m_chartDataSeriesIdentification = new SelectionItemChartDataSeries();
    m_chartDataFrequencyIdentification = new SelectionItemChartFrequencySeries();
    m_chartMatrixIdentification     = new SelectionItemChartMatrix();
    m_chartTwoHistogramIdentification = std::unique_ptr<SelectionItemChartTwoHistogram>(new SelectionItemChartTwoHistogram());
    m_chartTwoLabelIdentification = std::unique_ptr<SelectionItemChartTwoLabel>(new SelectionItemChartTwoLabel());
    m_chartTwoLineLayerIdentification = std::unique_ptr<SelectionItemChartTwoLineLayer>(new SelectionItemChartTwoLineLayer());
    m_chartTwoLineLayerVerticalNearestIdentification = std::unique_ptr<SelectionItemChartTwoLineLayerVerticalNearest>(new SelectionItemChartTwoLineLayerVerticalNearest());
    m_chartTwoLineSeriesIdentification = std::unique_ptr<SelectionItemChartTwoLineSeries>(new SelectionItemChartTwoLineSeries());
    m_chartTwoMatrixIdentification = std::unique_ptr<SelectionItemChartTwoMatrix>(new SelectionItemChartTwoMatrix());
    
    m_ciftiConnectivityMatrixRowColumnIdentfication = new SelectionItemCiftiConnectivityMatrixRowColumn();
    m_chartTimeSeriesIdentification = new SelectionItemChartTimeSeries();
    m_surfaceFocusIdentification = new SelectionItemFocusSurface();
    m_focusIdentification = new SelectionItemFocus();
    m_histologyPlaneCoordinateIdentification.reset(new SelectionItemHistologyCoordinate());
    m_imageControlPointIdentification = new SelectionItemImageControlPoint();
    m_mediaLogicalCoordinateIdentification.reset(new SelectionItemMediaLogicalCoordinate());
    m_mediaPlaneCoordinateIdentification.reset(new SelectionItemMediaPlaneCoordinate());
    
    m_samplesIdentification.reset(new SelectionItemAnnotation());
    m_surfaceNodeIdentification = new SelectionItemSurfaceNode();
    m_universalIdentificationSymbol.reset(new SelectionItemUniversalIdentificationSymbol());
    m_surfaceTriangleIdentification = new SelectionItemSurfaceTriangle();
    m_voxelIdentification = new SelectionItemVoxel();
    m_voxelEditingIdentification = new SelectionItemVoxelEditing();
    
    m_volumeMprCrosshairIdentification.reset(new SelectionItemVolumeMprCrosshair());
    
    m_allSelectionItems.push_back(m_annotationIdentification);
    m_allSelectionItems.push_back(m_surfaceBorderIdentification);
    m_allSelectionItems.push_back(m_chartDataSeriesIdentification);
    m_allSelectionItems.push_back(m_chartDataFrequencyIdentification);
    m_allSelectionItems.push_back(m_chartMatrixIdentification);
    m_allSelectionItems.push_back(m_chartTimeSeriesIdentification);
    m_allSelectionItems.push_back(m_chartTwoHistogramIdentification.get());
    m_allSelectionItems.push_back(m_chartTwoLabelIdentification.get());
    m_allSelectionItems.push_back(m_chartTwoLineLayerIdentification.get());
    m_allSelectionItems.push_back(m_chartTwoLineLayerVerticalNearestIdentification.get());
    m_allSelectionItems.push_back(m_chartTwoLineSeriesIdentification.get());
    m_allSelectionItems.push_back(m_chartTwoMatrixIdentification.get());
    m_allSelectionItems.push_back(m_ciftiConnectivityMatrixRowColumnIdentfication);
    m_allSelectionItems.push_back(m_samplesIdentification.get());
    m_allSelectionItems.push_back(m_surfaceFocusIdentification);
    m_allSelectionItems.push_back(m_surfaceNodeIdentification);
    m_allSelectionItems.push_back(m_surfaceTriangleIdentification);
    m_allSelectionItems.push_back(m_histologyPlaneCoordinateIdentification.get());
    m_allSelectionItems.push_back(m_imageControlPointIdentification);
    m_allSelectionItems.push_back(m_mediaLogicalCoordinateIdentification.get());
    m_allSelectionItems.push_back(m_mediaPlaneCoordinateIdentification.get());
    m_allSelectionItems.push_back(m_universalIdentificationSymbol.get());
    m_allSelectionItems.push_back(m_volumeMprCrosshairIdentification.get());
    m_allSelectionItems.push_back(m_voxelIdentification);
    m_allSelectionItems.push_back(m_voxelEditingIdentification);
    m_allSelectionItems.push_back(m_focusIdentification);
    
    m_surfaceSelectedItems.push_back(m_surfaceNodeIdentification);
    m_surfaceSelectedItems.push_back(m_surfaceTriangleIdentification);
    
    m_layeredSelectedItems.push_back(m_surfaceBorderIdentification);
    m_layeredSelectedItems.push_back(m_surfaceFocusIdentification);
    m_layeredSelectedItems.push_back(m_focusIdentification);

    m_volumeSelectedItems.push_back(m_voxelIdentification);
    m_volumeSelectedItems.push_back(m_voxelEditingIdentification);
    
    m_idTextGenerator = new IdentificationSimpleTextGenerator();
    m_idFormattedTextGenerator.reset(new IdentificationFormattedTextGenerator());
    
    m_lastSelectedItem = NULL;
    
    reset();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
}

/**
 * Destructor.
 */
SelectionManager::~SelectionManager()
{
    reset();
    delete m_annotationIdentification;
    m_annotationIdentification = NULL;
    delete m_surfaceBorderIdentification;
    m_surfaceBorderIdentification = NULL;
    delete m_chartDataSeriesIdentification;
    m_chartDataSeriesIdentification = NULL;
    delete m_chartDataFrequencyIdentification;
    m_chartDataFrequencyIdentification = NULL;
    delete m_chartMatrixIdentification;
    m_chartMatrixIdentification = NULL;
    delete m_chartTimeSeriesIdentification;
    m_chartTimeSeriesIdentification = NULL;
    delete m_ciftiConnectivityMatrixRowColumnIdentfication;
    m_ciftiConnectivityMatrixRowColumnIdentfication = NULL;
    delete m_surfaceFocusIdentification;
    m_surfaceFocusIdentification = NULL;
    delete m_imageControlPointIdentification;
    m_imageControlPointIdentification = NULL;
    delete m_surfaceNodeIdentification;
    m_surfaceNodeIdentification = NULL;
    delete m_surfaceTriangleIdentification;
    m_surfaceTriangleIdentification = NULL;
    delete m_voxelIdentification;
    m_voxelIdentification = NULL;
    delete m_voxelEditingIdentification;
    m_voxelEditingIdentification = NULL;
    delete m_focusIdentification;
    m_focusIdentification = NULL;
    delete m_idTextGenerator;
    m_idTextGenerator = NULL;
    
    if (m_lastSelectedItem != NULL) {
        delete m_lastSelectedItem;
    }

    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   The event.
 */
void
SelectionManager::receiveEvent(Event* /*event*/)
{
}

/**
 * Filter selections to arbitrate between triangle/node
 * and to remove any selections behind another selection.
 *
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be
 *    selected.
 */
void 
SelectionManager::filterSelections(const bool applySelectionBackgroundFiltering)
{
    const bool debugFlag(false);
    if (debugFlag) {
        AString logText;
        for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
             iter != m_allSelectionItems.end();
             iter++) {
            SelectionItem* item = *iter;
            if (item->isValid()) {
                logText += ("\n" + item->toString() + "\n");
            }
        }
        std::cout << "Selected Items BEFORE filtering: " << logText << std::endl;
    }

    SelectionItemSurfaceTriangle* triangleID = m_surfaceTriangleIdentification;
    SelectionItemSurfaceNode* nodeID = m_surfaceNodeIdentification;
    
    //
    // If both a node and triangle are found
    //
    if ((nodeID->getNodeNumber() >= 0) &&
        (triangleID->getTriangleNumber() >= 0)) {
        //
        // Is node further from user than triangle?
        //
        double depthDiff = m_surfaceNodeIdentification->getScreenDepth()
        - triangleID->getScreenDepth();
        if (depthDiff > 0.00001) {
            //
            // Do not use node
            //
            m_surfaceNodeIdentification->reset();
        }
    }
    
    //
    // Have a triangle ?
    //
    const int32_t triangleNumber = triangleID->getTriangleNumber();
    if (triangleNumber >= 0) {
        //
        // If no node, use node in nearest triangle
        //
        if (m_surfaceNodeIdentification->getNodeNumber() < 0) {
            const int32_t nearestNode = triangleID->getNearestNodeNumber();
            if (nearestNode >= 0) {
                CaretLogFine("Switched vertex to triangle nearest vertex ."
                             + AString::number(nearestNode));
                nodeID->setNodeNumber(nearestNode);
                nodeID->setScreenDepth(triangleID->getScreenDepth());
                nodeID->setSurface(triangleID->getSurface());
                double xyz[3];
                triangleID->getNearestNodeScreenXYZ(xyz);
                nodeID->setScreenXYZ(xyz);
                triangleID->getNearestNodeModelXYZ(xyz);
                nodeID->setModelXYZ(xyz);
                nodeID->setBrain(triangleID->getBrain());
            }
        }
    }
    
    if (m_universalIdentificationSymbol->isValid()) {
        if (m_surfaceNodeIdentification->isValid()) {
            /*
             * identification symbol may be on side of surface
             * facing away from user (backside)
             */
            const double depthDiff = (m_universalIdentificationSymbol->getScreenDepth()
                                      - m_surfaceNodeIdentification->getScreenDepth());
            if (depthDiff > 0.01) {
                m_universalIdentificationSymbol->reset();
            }
            else {
                m_surfaceNodeIdentification->reset();
            }
        }
        if (m_voxelIdentification->isValid()) {
            const double depthDiff = (m_universalIdentificationSymbol->getScreenDepth()
                                      - m_voxelIdentification->getScreenDepth());
            if (depthDiff > 0.01) {
                m_universalIdentificationSymbol->reset();
            }
            else {
                m_voxelIdentification->reset();
            }
        }
        if (m_histologyPlaneCoordinateIdentification->isValid()) {
            m_histologyPlaneCoordinateIdentification->reset();
        }
        if (m_mediaLogicalCoordinateIdentification->isValid()) {
            /*
             * Media is "flat" so always give priority to identification symbol
             */
            m_mediaLogicalCoordinateIdentification->reset();
        }
        if (m_mediaPlaneCoordinateIdentification->isValid()) {
            m_mediaPlaneCoordinateIdentification->reset();
        }
    }
    
    if (applySelectionBackgroundFiltering) {
         clearDistantSelections();
    }
    
    if (debugFlag) {
        AString logText;
        for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
             iter != m_allSelectionItems.end();
             iter++) {
            SelectionItem* item = *iter;
            if (item->isValid()) {
                logText += ("\n" + item->toString() + "\n");
            }
        }
        std::cout << "Selected Items AFTER filtering: " << logText << std::endl;
    }
}

/**
 * Examine the selection groups and manipulate them
 * so that there are not items selected in more
 * than one group.
 */
void 
SelectionManager::clearDistantSelections()
{
    std::vector<std::vector<SelectionItem*>* > itemGroups;
    /*
     * Make layers items slightly closer since they are 
     * often pasted onto the surface.
     */
    for (std::vector<SelectionItem*>::iterator iter = m_layeredSelectedItems.begin();
         iter != m_layeredSelectedItems.end();
         iter++) {
        SelectionItem* item = *iter;
        item->setScreenDepth(item->getScreenDepth()* 0.99);
    }

    
    itemGroups.push_back(&m_layeredSelectedItems);
    itemGroups.push_back(&m_surfaceSelectedItems);
    itemGroups.push_back(&m_volumeSelectedItems);
    
    std::vector<SelectionItem*>* minDepthGroup = NULL;
    double minDepth = std::numeric_limits<double>::max();
    for (std::vector<std::vector<SelectionItem*>* >::iterator iter = itemGroups.begin();
         iter != itemGroups.end();
         iter++) {
        std::vector<SelectionItem*>* group = *iter;
        SelectionItem* minDepthItem =
        getMinimumDepthFromMultipleSelections(*group);
        if (minDepthItem != NULL) {
            double md = minDepthItem->getScreenDepth();
            if (md < minDepth) {
                minDepthGroup = group;
                minDepth = md;
            }
        }
    }
    
    if (minDepthGroup != NULL) {
        for (std::vector<std::vector<SelectionItem*>* >::iterator iter = itemGroups.begin();
             iter != itemGroups.end();
             iter++) {
            std::vector<SelectionItem*>* group = *iter;
            if (group != minDepthGroup) {
                for (std::vector<SelectionItem*>::iterator iter = group->begin();
                     iter != group->end();
                     iter++) {
                    SelectionItem* item = *iter;
                    item->reset();
                }
            }
        }
    }
}

/**
 * Reset all selected items except for the given selected item.
 * @param selectedItem
 *    SelectedItem that is NOT reset.
 */
void 
SelectionManager::clearOtherSelectedItems(SelectionItem* selectedItem)
{
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        if (item != selectedItem) {
            item->reset();
        }
    }
}

/**
 * From the list of selectable items, find the item with the 
 * minimum depth.
 * @param items  List of selectable items.
 * @return  Reference to selectable item with the minimum depth
 * or NULL if no valid selectable items in the list.
 */
SelectionItem* 
SelectionManager::getMinimumDepthFromMultipleSelections(std::vector<SelectionItem*> items) const
{
    double minDepth = std::numeric_limits<double>::max();
    
    SelectionItem* minDepthItem = NULL;
    
    for (std::vector<SelectionItem*>::iterator iter = items.begin();
         iter != items.end();
         iter++) {
        SelectionItem* item = *iter;
        if (item->isValid()) {
            if (item->getScreenDepth() < minDepth) {
                minDepthItem = item;
                minDepth = item->getScreenDepth();
            }
        }
    }
    
    return minDepthItem;
}

/**
 * Set the enabled status for all selection items.
 *
 * @param status
 *    New status for ALL selection items.
 */
void
SelectionManager::setAllSelectionsEnabled(const bool status)
{
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        item->setEnabledForSelection(status);
    }
}

/**
 * Get simple text describing the current identification data.
 * @param brain
 *    Brain containing the data.
 */
AString 
SelectionManager::getSimpleIdentificationText(const Brain* brain) const
{
    CaretAssert(brain);
    const AString text = m_idTextGenerator->createIdentificationText(this, 
                                                                         brain);
    return text;
}

/**
 * Get formatted text describing the current identification data.
 * @param brain
 *    Brain containing the data.
 * @param tabIndex
 *    Index of tab where identication took place
 */
AString
SelectionManager::getFormattedIdentificationText(const Brain* brain,
                                                 const int32_t tabIndex) const
{
    CaretAssert(brain);
    const AString text = m_idFormattedTextGenerator->createIdentificationText(this,
                                                                              brain,
                                                                              tabIndex);
    return text;
}

/**
 * Reset all identification.
 */
void 
SelectionManager::reset()
{
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        item->reset();
    }
}

/**
 * @return Identification for annotations.
 */
SelectionItemAnnotation*
SelectionManager::getAnnotationIdentification()
{
    return m_annotationIdentification;
}

/**
 * @return Identification for annotations.
 */
const SelectionItemAnnotation*
SelectionManager::getAnnotationIdentification() const
{
    return m_annotationIdentification;
}

/**
 * @return Identification for histology
 */
SelectionItemHistologyCoordinate*
SelectionManager::getHistologyPlaneCoordinateIdentification()
{
    return m_histologyPlaneCoordinateIdentification.get();
}

/**
 * @return Identification for histology
 */
const SelectionItemHistologyCoordinate*
SelectionManager::getHistologyPlaneCoordinateIdentification() const
{
    return m_histologyPlaneCoordinateIdentification.get();
}

/**
 * @return Identification for image control point.
 */
SelectionItemImageControlPoint*
SelectionManager::getImageControlPointIdentification()
{
    return m_imageControlPointIdentification;
}

/**
 * @return Identification for image control point.
 */
const SelectionItemImageControlPoint*
SelectionManager::getImageControlPointIdentification() const
{
    return m_imageControlPointIdentification;
}

/**
 * @return Identification for media
 */
SelectionItemMediaLogicalCoordinate*
SelectionManager::getMediaLogicalCoordinateIdentification()
{
    return m_mediaLogicalCoordinateIdentification.get();
}

/**
 * @return Identification for media
 */
const SelectionItemMediaLogicalCoordinate*
SelectionManager::getMediaLogicalCoordinateIdentification() const
{
    return m_mediaLogicalCoordinateIdentification.get();
}

/**
 * @return Identification for media
 */
SelectionItemMediaPlaneCoordinate*
SelectionManager::getMediaPlaneCoordinateIdentification()
{
    return m_mediaPlaneCoordinateIdentification.get();
}

/**
 * @return Identification for media
 */
const SelectionItemMediaPlaneCoordinate*
SelectionManager::getMediaPlaneCoordinateIdentification() const
{
    return m_mediaPlaneCoordinateIdentification.get();
}

/**
 * @return Identification for samples.
 */
SelectionItemAnnotation*
SelectionManager::getSamplesIdentification()
{
    return m_samplesIdentification.get();
}

/**
 * @return Identification for samples.
 */
const SelectionItemAnnotation*
SelectionManager::getSamplesIdentification() const
{
    return m_samplesIdentification.get();
}

/**
 * @return Identification for surface node.
 */
SelectionItemSurfaceNode* 
SelectionManager::getSurfaceNodeIdentification()
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const SelectionItemSurfaceNode* 
SelectionManager::getSurfaceNodeIdentification() const
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface triangle.
 */
SelectionItemSurfaceTriangle* 
SelectionManager::getSurfaceTriangleIdentification()
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for surface triangle.
 */
const SelectionItemSurfaceTriangle* 
SelectionManager::getSurfaceTriangleIdentification() const
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for volume MPR crosshair.
 */
const SelectionItemVolumeMprCrosshair*
SelectionManager::getVolumeMprCrosshairIdentification() const
{
    return m_volumeMprCrosshairIdentification.get();
}

/**
 * @return Identification for volume MPR crosshair.
 */
SelectionItemVolumeMprCrosshair*
SelectionManager::getVolumeMprCrosshairIdentification()
{
    return m_volumeMprCrosshairIdentification.get();
}

/**
 * @return Identification for voxels.
 */
const SelectionItemVoxel* 
SelectionManager::getVoxelIdentification() const
{
    return m_voxelIdentification;
}

/**
 * @return Identification for voxels.
 */
SelectionItemVoxel* 
SelectionManager::getVoxelIdentification()
{
    return m_voxelIdentification;
}

/**
 * @return Identification for voxel editing.
 */
const SelectionItemVoxelEditing*
SelectionManager::getVoxelEditingIdentification() const
{
    return m_voxelEditingIdentification;
}

/**
 * @return Identification for voxel editing.
 */
SelectionItemVoxelEditing*
SelectionManager::getVoxelEditingIdentification()
{
    return m_voxelEditingIdentification;
}

/**
 * @return Universal identification system selection
 */
SelectionItemUniversalIdentificationSymbol*
SelectionManager::getUniversalIdentificationSymbol()
{
    return m_universalIdentificationSymbol.get();
}

/**
 * @return Universal identification system selection (const method)
 */
const SelectionItemUniversalIdentificationSymbol*
SelectionManager::getUniversalIdentificationSymbol() const
{
    return m_universalIdentificationSymbol.get();
}

/**
 * @return Identification for borders.
 */
SelectionItemBorderSurface* 
SelectionManager::getSurfaceBorderIdentification()
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for borders.
 */
const SelectionItemBorderSurface* 
SelectionManager::getSurfaceBorderIdentification() const
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for foci.
 */
SelectionItemFocusSurface* 
SelectionManager::getSurfaceFocusIdentification()
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
const SelectionItemFocusSurface* 
SelectionManager::getSurfaceFocusIdentification() const
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
SelectionItemFocus*
SelectionManager::getFocusIdentification()
{
    return m_focusIdentification;
}

/**
 * @return Identification for foci.
 */
const SelectionItemFocus*
SelectionManager::getFocusIdentification() const
{
    return m_focusIdentification;
}

/**
 * @return Identification for data-series chart.
 */
SelectionItemChartDataSeries*
SelectionManager::getChartDataSeriesIdentification()
{
    return m_chartDataSeriesIdentification;
}

/**
 * @return Identification for data-series chart (const method).
 */
const SelectionItemChartDataSeries*
SelectionManager::getChartDataSeriesIdentification() const
{
    return m_chartDataSeriesIdentification;
}

/**
 * @return Identification for frequency-series chart.
 */
SelectionItemChartFrequencySeries*
SelectionManager::getChartFrequencySeriesIdentification()
{
    return m_chartDataFrequencyIdentification;
}

/**
 * @return Identification for frequency-series chart (const method).
 */
const SelectionItemChartFrequencySeries*
SelectionManager::getChartFrequencySeriesIdentification() const
{
    return m_chartDataFrequencyIdentification;
}

/**
 * @return Identification for matrix chart.
 */
SelectionItemChartMatrix*
SelectionManager::getChartMatrixIdentification()
{
    return m_chartMatrixIdentification;
}

/**
 * @return Identification for matrix chart (const method)
 */
const SelectionItemChartMatrix*
SelectionManager::getChartMatrixIdentification() const
{
    return m_chartMatrixIdentification;
}

/**
 * @return Identification for chart two histogram identification
 */
SelectionItemChartTwoHistogram*
SelectionManager::getChartTwoHistogramIdentification()
{
    return m_chartTwoHistogramIdentification.get();
}

/**
 * @return Identification for chart two histogram identification
 */
const SelectionItemChartTwoHistogram*
SelectionManager::getChartTwoHistogramIdentification() const
{
    return m_chartTwoHistogramIdentification.get();
}

/**
 * @return Identification for chart two line layer identification
 */
SelectionItemChartTwoLineLayer*
SelectionManager::getChartTwoLineLayerIdentification()
{
    return m_chartTwoLineLayerIdentification.get();
}

/**
 * @return Identification for chart two line layer identification
 */
const SelectionItemChartTwoLineLayer*
SelectionManager::getChartTwoLineLayerIdentification() const
{
    return m_chartTwoLineLayerIdentification.get();
}

/**
 * @return Identification for chart two line layer nearest vertical identification
 */
SelectionItemChartTwoLineLayerVerticalNearest*
SelectionManager::getChartTwoLineLayerVerticalNearestIdentification()
{
    return m_chartTwoLineLayerVerticalNearestIdentification.get();
}

/**
 * @return Identification for chart two line layer nearest vertical identification
 */
const SelectionItemChartTwoLineLayerVerticalNearest*
SelectionManager::getChartTwoLineLayerVerticalNearestIdentification() const
{
    return m_chartTwoLineLayerVerticalNearestIdentification.get();
}

/**
 * @return Identification for chart two line series identification
 */
SelectionItemChartTwoLineSeries*
SelectionManager::getChartTwoLineSeriesIdentification()
{
    return m_chartTwoLineSeriesIdentification.get();
}

/**
 * @return Identification for chart two line series identification
 */
const SelectionItemChartTwoLineSeries*
SelectionManager::getChartTwoLineSeriesIdentification() const
{
    return m_chartTwoLineSeriesIdentification.get();
}

/**
 * @return Identification for chart two label identification
 */
SelectionItemChartTwoLabel*
SelectionManager::getChartTwoLabelIdentification()
{
    return m_chartTwoLabelIdentification.get();
}

/**
 * @return Identification for chart two label identification
 */
const SelectionItemChartTwoLabel*
SelectionManager::getChartTwoLabelIdentification() const
{
    return m_chartTwoLabelIdentification.get();
}

/**
 * @return Identification for chart two matrix identification.
 */
SelectionItemChartTwoMatrix*
SelectionManager::getChartTwoMatrixIdentification()
{
    return m_chartTwoMatrixIdentification.get();
}

/**
 * @return Identification for chart two matrix identification.
 */
const SelectionItemChartTwoMatrix*
SelectionManager::getChartTwoMatrixIdentification() const
{
    return m_chartTwoMatrixIdentification.get();
}

/**
 * @return Identification for CIFTI Connectivity Matrix Row/Column.
 */
SelectionItemCiftiConnectivityMatrixRowColumn*
SelectionManager::getCiftiConnectivityMatrixRowColumnIdentification()
{
    return m_ciftiConnectivityMatrixRowColumnIdentfication;
}

/**
 * @return Identification for CIFTI Connectivity Matrix Row/Column.
 */
const SelectionItemCiftiConnectivityMatrixRowColumn*
SelectionManager::getCiftiConnectivityMatrixRowColumnIdentification() const
{
    return m_ciftiConnectivityMatrixRowColumnIdentfication;
}

/**
 * @return Identification for time-series chart.
 */
SelectionItemChartTimeSeries*
SelectionManager::getChartTimeSeriesIdentification()
{
    return m_chartTimeSeriesIdentification;
}

/**
 * @return Identification for time-series chart (const method).
 */
const SelectionItemChartTimeSeries*
SelectionManager::getChartTimeSeriesIdentification() const
{
    return m_chartTimeSeriesIdentification;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionManager::toString() const
{
    return "SelectionManager";
}

/**
 * @return The last selected item (may be NULL).
 */
const SelectionItem*
SelectionManager::getLastSelectedItem() const
{
    return m_lastSelectedItem;
}

/**
 * Set the last selected item to the given item.
 *
 * @param lastItem
 *     The last item that was selected;
 */
void
SelectionManager::setLastSelectedItem(const SelectionItem* lastItem)
{
    if (m_lastSelectedItem != NULL) {
        delete m_lastSelectedItem;
    }
    m_lastSelectedItem = NULL;
    
    if (lastItem != NULL) {
        const SelectionItemSurfaceNode* nodeID = dynamic_cast<const SelectionItemSurfaceNode*>(lastItem);
        const SelectionItemVoxel* voxelID = dynamic_cast<const SelectionItemVoxel*>(lastItem);
        if (nodeID != NULL) {
            m_lastSelectedItem = new SelectionItemSurfaceNode(*nodeID);
        }
        else if (voxelID != NULL) {
            m_lastSelectedItem = new SelectionItemVoxel(*voxelID);
        }
        else {
            CaretAssertMessage(0,
                               ("Unsupported last ID type" + lastItem->toString()));
        }
    }
}


