
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
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GuiManager.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "UserInputModeFociWidget.h"
#include "UserInputModeView.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeFoci 
 * \brief Processes user input for foci.
 */

/**
 * Constructor.
 */
UserInputModeFoci::UserInputModeFoci(const int32_t windowIndex)
: UserInputModeView(UserInputModeAbstract::FOCI),
  m_windowIndex(windowIndex)
{
    m_inputModeFociWidget = new UserInputModeFociWidget(this,
                                                        windowIndex);
    m_mode = MODE_CREATE;
    m_editOperation = EDIT_OPERATION_PROPERTIES;
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
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_windowIndex).getPointer());
    }
    this->m_inputModeFociWidget->updateWidget();
}

/**
 * @return The edit operation.
 */
UserInputModeFoci::EditOperation
UserInputModeFoci::getEditOperation() const
{
    return m_editOperation;
}

/**
 * Set the edit operation.
 * @param editOperation
 *   New edit operation.
 */
void
UserInputModeFoci::setEditOperation(const EditOperation editOperation)
{
    m_editOperation = editOperation;
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeFoci::initialize()
{
    m_inputModeFociWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeFoci::finish()
{
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
        case MODE_CREATE:
            break;
        case MODE_EDIT:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            switch (m_editOperation) {
                case EDIT_OPERATION_DELETE:
                    cursor = CursorEnum::CURSOR_CROSS;
                    break;
                case EDIT_OPERATION_PROPERTIES:
                    cursor = CursorEnum::CURSOR_WHATS_THIS;
                    break;
            }
            break;
        case MODE_OPERATIONS:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
    SelectionManager* idManager =
    openGLWidget->performIdentification(mouseEvent.getX(),
                                        mouseEvent.getY(),
                                        true);
    
    switch (m_mode) {
        case MODE_CREATE:
        {
            SelectionItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
            SelectionItemVoxel* idVoxel = idManager->getVoxelIdentification();
            if (idNode->isValid()) {
                Surface* surfaceViewed = idNode->getSurface();
                CaretAssert(surfaceViewed);
                const Surface* anatSurface = getAnatomicalSurfaceForSurface(surfaceViewed);
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
                int64_t ijk[3];
                idVoxel->getVoxelIJK(ijk);
                float xyz[3];
                vf->indexToSpace(ijk, xyz);
                
                const AString focusName = (cmdf->getFileNameNoPath()
                                           + " IJK ("
                                           + AString::fromNumbers(ijk, 3, ",")
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
        }            break;
        case MODE_EDIT:
        {
            FociFile* fociFile = NULL;
            Focus*    focus = NULL;
            
            SelectionItemFocusVolume* idVolFocus = idManager->getVolumeFocusIdentification();
            if (idVolFocus->isValid()) {
                fociFile = idVolFocus->getFociFile();
                CaretAssert(fociFile);
                focus    = idVolFocus->getFocus();
                CaretAssert(focus);
            }
            SelectionItemFocusSurface* idFocus = idManager->getSurfaceFocusIdentification();
            if (idFocus->isValid()) {
                fociFile = idFocus->getFociFile();
                CaretAssert(fociFile);
                focus    = idFocus->getFocus();
                CaretAssert(focus);
                
            }
            
            if ((fociFile != NULL)
                && (focus != NULL)) {
                switch (m_editOperation) {
                    case EDIT_OPERATION_DELETE:
                        fociFile->removeFocus(focus);
                        updateAfterFociChanged();
                        break;
                    case EDIT_OPERATION_PROPERTIES:
                    {
                        FociPropertiesEditorDialog::editFocus(fociFile,
                                                              focus,
                                                              openGLWidget);
                    }
                }
            }
        }
            break;
        case MODE_OPERATIONS:
            break;
    }
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



