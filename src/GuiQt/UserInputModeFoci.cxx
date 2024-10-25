
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

#define __USER_INPUT_MODE_FOCI_DECLARE__
#include "UserInputModeFoci.h"
#undef __USER_INPUT_MODE_FOCI_DECLARE__

#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventGraphicsPaintSoonOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GuiManager.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocus.h"
#include "SelectionItemHistologyCoordinate.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjector.h"
#include "UserInputModeFociWidget.h"
#include "UserInputModeView.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeFoci 
 * \brief Processes user input for foci.
 */

/**
 * Constructor.
 * @param browserIndexIndex
 *    Index of window
 */
UserInputModeFoci::UserInputModeFoci(const int32_t browserIndexIndex)
: UserInputModeView(browserIndexIndex,
                    UserInputModeEnum::Enum::FOCI)
{
    m_inputModeFociWidget = new UserInputModeFociWidget(this,
                                                        browserIndexIndex);
    m_mode = MODE_CREATE_AT_ID;
    setWidgetForToolBar(m_inputModeFociWidget);
}

/**
 * Destructor.
 */
UserInputModeFoci::~UserInputModeFoci()
{
    
}

/**
 * @return the mode.
 */
UserInputModeFoci::Mode
UserInputModeFoci::getMode() const
{
    return m_mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void
UserInputModeFoci::setMode(const Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());
    }
    this->m_inputModeFociWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeFoci::initialize()
{
    m_focusBeingMovedWithMouse = NULL;
    m_inputModeFociWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeFoci::finish()
{
    m_focusBeingMovedWithMouse = NULL;
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeFoci::update()
{
    
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeFoci::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
            break;
        case MODE_DELETE:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_EDIT:
            cursor = CursorEnum::CURSOR_WHATS_THIS;
            break;
        case MODE_MOVE:
            if (m_focusBeingMovedWithMouse != NULL) {
                cursor = CursorEnum::CURSOR_CROSS;
            }
            else if (m_focusInMoveModeUnderMouse != NULL) {
                cursor = CursorEnum::CURSOR_CROSS;
            }
            break;
    }
    
    return cursor;
}

void
UserInputModeFoci::updateAfterFociChanged()
{
    /*
     * Need to update all graphics windows and all border controllers.
     */
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addFoci().getPointer());
}

/**
 * Determine the structure for the given surface.  Find the primary anatomical
 * surface for the structure and return it.  If no primary anatomical surface
 * is found, the return the surface that was passed in.
 */
Surface*
UserInputModeFoci::getAnatomicalSurfaceForSurface(Surface* surface)
{
    Brain* brain = GuiManager::get()->getBrain();
    const StructureEnum::Enum structure = surface->getStructure();
    BrainStructure* bs = brain->getBrainStructure(structure,
                                                  false);
    Surface* anatSurf = bs->getPrimaryAnatomicalSurface();
    if (anatSurf != NULL) {
        return anatSurf;
    }
    return surface;
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeFoci::mouseLeftClick(const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
        {
            SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
            SelectionItemSurfaceTriangle* idTriangle = idManager->getSurfaceTriangleIdentification();
            SelectionItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
            SelectionItemVoxel* idVoxel = idManager->getVoxelIdentification();
            SelectionItemHistologyCoordinate* idHistology(idManager->getHistologyPlaneCoordinateIdentification());
            idManager->setAllSelectionsEnabled(false);
            idNode->setEnabledForSelection(true);
            idTriangle->setEnabledForSelection(true);
            idVoxel->setEnabledForSelection(true);
            idHistology->setEnabledForSelection(true);
            openGLWidget->performIdentificationSome(mouseEvent.getX(), mouseEvent.getY());
            
            if (idTriangle->isValid()
                && (idTriangle->isBarycentricProjectionValid())) {
                Surface* surfaceViewed = idTriangle->getSurface();
                CaretAssert(surfaceViewed);
                const Surface* anatSurface = getAnatomicalSurfaceForSurface(surfaceViewed);
                noAnatomicalSurfaceWarningDialog(anatSurface);
                int32_t vertices[3];
                idTriangle->getBarycentricVertices(vertices);
                float areas[3];
                idTriangle->getBarycentricAreas(areas);
                float xyz[3];
                if (SurfaceProjectionBarycentric::unprojectToSurface(anatSurface, areas, vertices, xyz)) {
                    const StructureEnum::Enum anatStructure = anatSurface->getStructure();
                    
                    const AString focusName = (StructureEnum::toGuiName(anatStructure)
                                               + " Triangle "
                                               + AString::number(idTriangle->getTriangleNumber()));
                    
                    const AString comment = ("Created from "
                                             + focusName);
                    
                    Focus* focus = new Focus();
                    focus->setName(focusName);
                    focus->getProjection(0)->setStereotaxicXYZ(xyz);
                    focus->setComment(comment);
                    FociPropertiesEditorDialog::createFocus(focus,
                                                            browserTabContent,
                                                            m_inputModeFociWidget);
                }
                else {
                    WuQMessageBox::errorOk(m_inputModeFociWidget, "Failed to unproject focus to anatomial surface");
                }
            }
            else if (idNode->isValid()) {
                Surface* surfaceViewed = idNode->getSurface();
                CaretAssert(surfaceViewed);
                const Surface* anatSurface = getAnatomicalSurfaceForSurface(surfaceViewed);
                noAnatomicalSurfaceWarningDialog(anatSurface);
                const StructureEnum::Enum anatStructure = anatSurface->getStructure();
                const int32_t nodeIndex = idNode->getNodeNumber();
                
                const AString focusName = (StructureEnum::toGuiName(anatStructure)
                                           + " Vertex "
                                           + AString::number(nodeIndex));
                const float* xyz = anatSurface->getCoordinate(nodeIndex);
                
                const AString comment = ("Created from "
                                         + focusName);
                
                Focus* focus = new Focus();
                focus->setName(focusName);
                focus->getProjection(0)->setStereotaxicXYZ(xyz);
                focus->setComment(comment);
                FociPropertiesEditorDialog::createFocus(focus,
                                                        browserTabContent,
                                                        m_inputModeFociWidget);
            }
            else if (idVoxel->isValid()) {
                const VolumeMappableInterface* vf = idVoxel->getVolumeFile();
                const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(vf);
                const Vector3D xyz(idVoxel->getVoxelXYZ());
                const AString focusName = (cmdf->getFileNameNoPath()
                                           + " XYZ ("
                                           + AString::fromNumbers(xyz, 3, ",")
                                           + ")");
                
                const AString comment = ("Created from "
                                         + focusName);
                
                Focus* focus = new Focus();
                focus->setName(focusName);
                focus->getProjection(0)->setStereotaxicXYZ(xyz);
                focus->setComment(comment);
                FociPropertiesEditorDialog::createFocus(focus,
                                                        browserTabContent,
                                                        m_inputModeFociWidget);
            }
            else if (idHistology->isValid()) {
                const HistologyCoordinate histCoord(idHistology->getCoordinate());
                if (histCoord.isStereotaxicXYZValid()) {
                    const Vector3D xyz(histCoord.getStereotaxicXYZ());
                    const AString focusName = (histCoord.getHistologySlicesFileName()
                                               + " Slice ("
                                               + histCoord.getSliceName()
                                               + ")");
                    
                    const AString comment = ("Created from "
                                             + focusName);
                    
                    Focus* focus = new Focus();
                    focus->setName(focusName);
                    focus->getProjection(0)->setStereotaxicXYZ(xyz);
                    focus->setComment(comment);
                    FociPropertiesEditorDialog::createFocus(focus,
                                                            browserTabContent,
                                                            m_inputModeFociWidget);
                }
            }
        }            
            break;
        case MODE_DELETE:
        case MODE_EDIT:
        {
            SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
            SelectionItemFocus* idVolFocus = idManager->getFocusIdentification();
            SelectionItemFocusSurface* idFocusSurface = idManager->getSurfaceFocusIdentification();
            idManager->setAllSelectionsEnabled(false);
            idVolFocus->setEnabledForSelection(true);
            idFocusSurface->setEnabledForSelection(true);
            openGLWidget->performIdentificationSome(mouseEvent.getX(), mouseEvent.getY());

            FociFile* fociFile = NULL;
            Focus*    focus = NULL;
            
            if (idVolFocus->isValid()) {
                fociFile = idVolFocus->getFociFile();
                CaretAssert(fociFile);
                focus    = idVolFocus->getFocus();
                CaretAssert(focus);
            }
            if (idFocusSurface->isValid()) {
                fociFile = idFocusSurface->getFociFile();
                CaretAssert(fociFile);
                focus    = idFocusSurface->getFocus();
                CaretAssert(focus);
                
            }
            
            if ((fociFile != NULL)
                && (focus != NULL)) {
                switch (m_mode) {
                    case MODE_CREATE_AT_ID:
                        break;
                    case MODE_DELETE:
                    {
                        fociFile->removeFocus(focus);
                        updateAfterFociChanged();
                    }
                        break;
                    case MODE_EDIT:
                    {
                        FociPropertiesEditorDialog::editFocus(fociFile,
                                                              focus,
                                                              openGLWidget);
                    }
                        break;
                    case MODE_MOVE:
                        break;
                }
            }
        }
            break;
        case MODE_MOVE:
            CaretAssertToDoWarning();
            break;
    }
}

/**
 *  Warn the user if there is no anatomical surface for projecting the focus.
 *  @param surface
 *     Surface that should be an anatomical surface.
 *  @return True if we should continue creating the focus.
 */
void
UserInputModeFoci::noAnatomicalSurfaceWarningDialog(const Surface* surface) const
{
    if ((surface->getSurfaceType() != SurfaceTypeEnum::RECONSTRUCTION)
        && (surface->getSurfaceType() != SurfaceTypeEnum::ANATOMICAL)) {
        if ((surface->getStructure() == StructureEnum::CORTEX_LEFT)
            || (surface->getStructure() == StructureEnum::CORTEX_RIGHT)) {
            const AString msg("No Anatomical surface was found for this hemisphere.  "
                              "Focus will be created but it may be at an incorrect stereotaxic coordinate.");
            WuQMessageBox::warningOkWithDoNotShowAgain(m_inputModeFociWidget,
                                                       "FocusProjectDialogAnatomicalWarning",
                                                       msg);
        }
    }
}

/**
 * Process a mouse move event
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeFoci::mouseMove(const MouseEvent& mouseEvent)
{
    m_focusInMoveModeUnderMouse = NULL;
    
    bool moveFlag(false);
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
            break;
        case MODE_DELETE:
            break;
        case MODE_EDIT:
            break;
        case MODE_MOVE:
            moveFlag = true;
            break;
    }
    if ( ! moveFlag) {
        UserInputModeView::mouseMove(mouseEvent);
        return;
    }
    if (m_focusBeingMovedWithMouse == NULL) {
        BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
        if (viewportContent == NULL) {
            return;
        }
        
        BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
        SelectionManager* idManager =
        openGLWidget->performIdentificationAll(mouseEvent.getX(),
                                               mouseEvent.getY(),
                                               true);
        
        m_focusBeingMovedWithMouse = NULL;
        SelectionItemFocus* idVolFocus = idManager->getFocusIdentification();
        if (idVolFocus->isValid()) {
            m_focusInMoveModeUnderMouse = idVolFocus->getFocus();
        }
        SelectionItemFocusSurface* idFocusSurface = idManager->getSurfaceFocusIdentification();
        if (idFocusSurface->isValid()) {
            m_focusInMoveModeUnderMouse = idFocusSurface->getFocus();
        }
    }
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeFoci::mouseLeftDrag(const MouseEvent& mouseEvent)
{
    bool moveFlag(false);
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
            break;
        case MODE_DELETE:
            break;
        case MODE_EDIT:
            break;
        case MODE_MOVE:
            moveFlag = true;
            break;
    }
    if ( ! moveFlag) {
        UserInputModeView::mouseLeftDrag(mouseEvent);
        return;
    }
    
    if (m_focusBeingMovedWithMouse == NULL) {
        return;
    }
    
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    SelectionItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
    SelectionItemSurfaceTriangle* idTriangle(idManager->getSurfaceTriangleIdentification());
    SelectionItemVoxel* idVoxel = idManager->getVoxelIdentification();
    SelectionItemHistologyCoordinate* idHistology(idManager->getHistologyPlaneCoordinateIdentification());
    idManager->setAllSelectionsEnabled(false);
    idNode->setEnabledForSelection(true);
    idTriangle->setEnabledForSelection(true);
    idVoxel->setEnabledForSelection(true);
    idHistology->setEnabledForSelection(true);
    openGLWidget->performIdentificationSome(mouseEvent.getX(), mouseEvent.getY());
    if (idTriangle->isValid()
        && idTriangle->isBarycentricProjectionValid()) {
        int32_t vertices[3];
        idTriangle->getBarycentricVertices(vertices);
        float areas[3];
        idTriangle->getBarycentricAreas(areas);
        Surface* surfaceViewed(idTriangle->getSurface());
        CaretAssert(surfaceViewed);
        const Surface* anatSurface = getAnatomicalSurfaceForSurface(surfaceViewed);
        float xyz[3];
        if (SurfaceProjectionBarycentric::unprojectToSurface(anatSurface, areas, vertices, xyz)) {
            m_focusBeingMovedWithMouse->getProjection(0)->setStereotaxicXYZWhileMovingWithMouse(xyz);
            try {
                SurfaceProjector projector(anatSurface);
                projector.projectFocus(0, m_focusBeingMovedWithMouse);
            }
            catch (const SurfaceProjectorException& spe) {
                CaretLogWarning(spe.whatString());
            }
        }
        else {
            CaretLogWarning("Failed to unproject barycentric data to "
                            + anatSurface->getFileName());
        }
    }
    else if (idNode->isValid()) {
        Surface* surfaceViewed = idNode->getSurface();
        CaretAssert(surfaceViewed);
        const Surface* anatSurface = getAnatomicalSurfaceForSurface(surfaceViewed);
        const int32_t nodeIndex = idNode->getNodeNumber();
        const float* xyz = anatSurface->getCoordinate(nodeIndex);
        m_focusBeingMovedWithMouse->getProjection(0)->setStereotaxicXYZWhileMovingWithMouse(xyz);
        try {
            SurfaceProjector projector(anatSurface);
            projector.projectFocus(0, m_focusBeingMovedWithMouse);
        }
        catch (const SurfaceProjectorException& spe) {
            CaretLogWarning(spe.whatString());
        }
    }
    else if (idVoxel->isValid()) {
        const Vector3D xyz(idVoxel->getVoxelXYZ());
        m_focusBeingMovedWithMouse->getProjection(0)->setStereotaxicXYZWhileMovingWithMouse(xyz);
    }
    else if (idHistology->isValid()) {
        const HistologyCoordinate histCoord(idHistology->getCoordinate());
        if (histCoord.isStereotaxicXYZValid()) {
            const Vector3D xyz(histCoord.getStereotaxicXYZ());
            m_focusBeingMovedWithMouse->getProjection(0)->setStereotaxicXYZWhileMovingWithMouse(xyz);
        }
    }
    
    updateAfterFociChanged();
}

/**
 * Process a mouse left press event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void 
UserInputModeFoci::mouseLeftPress(const MouseEvent& mouseEvent)
{
    m_focusInMoveModeUnderMouse = NULL;
    
    bool moveFlag(false);
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
            break;
        case MODE_DELETE:
            break;
        case MODE_EDIT:
            break;
        case MODE_MOVE:
            moveFlag = true;
            break;
    }
    if ( ! moveFlag) {
        UserInputModeView::mouseLeftPress(mouseEvent);
        return;
    }
    
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    SelectionManager* idManager =
    openGLWidget->performIdentificationAll(mouseEvent.getX(),
                                           mouseEvent.getY(),
                                           true);

    m_focusBeingMovedWithMouse = NULL;
    SelectionItemFocus* idVolFocus = idManager->getFocusIdentification();
    if (idVolFocus->isValid()) {
        m_focusBeingMovedWithMouse = idVolFocus->getFocus();
    }
    SelectionItemFocusSurface* idFocusSurface = idManager->getSurfaceFocusIdentification();
    if (idFocusSurface->isValid()) {
        m_focusBeingMovedWithMouse = idFocusSurface->getFocus();
    }
}

/**
 * Process a mouse left release event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void 
UserInputModeFoci::mouseLeftRelease(const MouseEvent& mouseEvent)
{
    bool moveFlag(false);
    switch (m_mode) {
        case MODE_CREATE_AT_ID:
            break;
        case MODE_DELETE:
            break;
        case MODE_EDIT:
            break;
        case MODE_MOVE:
            moveFlag = true;
            break;
    }
    if ( ! moveFlag) {
        m_focusBeingMovedWithMouse = NULL;
        UserInputModeView::mouseLeftRelease(mouseEvent);
        return;
    }
    
    m_focusBeingMovedWithMouse = NULL;
}

/**
 * Show a context menu (pop-up menu at mouse location)
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param menuPosition
 *     Point at which menu is displayed (passed to QMenu::exec())
 * @param openGLWidget
 *     OpenGL widget in which context menu is requested
 */
void
UserInputModeFoci::showContextMenu(const MouseEvent& /*mouseEvent*/,
                                         const QPoint& /*menuPosition*/,
                                         BrainOpenGLWidget* /*openGLWidget*/)
{
    /* no context menu */
}



