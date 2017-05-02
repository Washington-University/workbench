
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

#define __SURFACE_SELECTION_CONTROL_DECLARE__
#include "SurfaceSelectionViewController.h"
#undef __SURFACE_SELECTION_CONTROL_DECLARE__

#include <QComboBox>

#include "BrainStructure.h"
#include "FileInformation.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "WuQEventBlockingFilter.h"
#include "WuQFactory.h"

using namespace caret;


    
/**
 * \class caret::SurfaceSelectionViewController 
 * \brief Control for selecting surfaces.
 * \ingroup GuiQt
 *
 * Control for selecting surfaces.
 */
/**
 * Constructor.
 * @param parent
 *   The parent.
 * @param surfaceSelection
 *    Surface selection that is controlled through this control.
 */
SurfaceSelectionViewController::SurfaceSelectionViewController(QObject* parent,
                                                 SurfaceSelectionModel* surfaceSelectionModel)
: WuQWidget(parent)
{
    this->initializeControl(MODE_SELECTION_MODEL_STATIC,
                            surfaceSelectionModel);
}

/**
 * Constructor.
 * @param parent
 *   The parent.
 * @param brainStructure
 *   Allows selection of any surface with the specified brain structure.
 */
SurfaceSelectionViewController::SurfaceSelectionViewController(QObject* parent,
                                                 BrainStructure* brainStructure)
: WuQWidget(parent)
{
    std::vector<SurfaceTypeEnum::Enum> allSurfaceTypes;
    SurfaceTypeEnum::getAllEnums(allSurfaceTypes);
    
    SurfaceSelectionModel* ss = new SurfaceSelectionModel(brainStructure->getStructure(),
                                                          allSurfaceTypes);
    this->initializeControl(MODE_BRAIN_STRUCTURE,
                            ss);
}

/**
 * Destructor.
 */
SurfaceSelectionViewController::~SurfaceSelectionViewController()
{
    bool isDeleteSelectionModel = false;
    switch (this->mode) {
        case MODE_BRAIN_STRUCTURE:
            isDeleteSelectionModel = true;
            break;
        case MODE_SELECTION_MODEL_DYNAMIC:
            break;
        case MODE_SELECTION_MODEL_STATIC:
            break;
    }
    
    if (isDeleteSelectionModel) {
        delete this->surfaceSelectionModel;
    }
}

/**
 * Constructor.  Creates a selection control.  User MUST call updateControl(SurfaceSelectionModel*)
 * so that surfaces get loaded.  A instance created this way will NEVER use the selection
 * model this is passed to updateControl() anywhere outside of updateControl().  Thus,
 * slots in the model are NEVER called.
 *
 * @param parent
 *   The parent.
 */
SurfaceSelectionViewController::SurfaceSelectionViewController(QObject* parent)
: WuQWidget(parent)
{
    this->initializeControl(MODE_SELECTION_MODEL_DYNAMIC,
                            NULL);
}

/**
 * Help initialize an instance.
 */
void 
SurfaceSelectionViewController::initializeControl(const Mode mode,
                                                  SurfaceSelectionModel* surfaceSelectionModel)
{
    this->mode = mode;
    this->surfaceComboBox = WuQFactory::newComboBox();
    this->surfaceComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(this->surfaceComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(comboBoxCurrentIndexChanged(int)));

//#ifdef CARET_OS_MACOSX
//    /*
//     * Block the wheel event on Mac since it get issued when a mouse 
//     * is moved over the combo box.
//     */
//    WuQEventBlockingFilter* comboBoxWheelEventBlockingFilter = new WuQEventBlockingFilter(this);
//    comboBoxWheelEventBlockingFilter->setEventBlocked(QEvent::Wheel, 
//                                                            true);
//    surfaceComboBox->installEventFilter(comboBoxWheelEventBlockingFilter);
//#endif // CARET_OS_MACOSX
    
    this->surfaceSelectionModel = surfaceSelectionModel;
}

/**
 * Update the control using the given selection model.  The model is NEVER
 * used outside this method so slots in the model are NEVER called.
 *
 * @param selectionModel
 *    Selection model used to update this control.  If this parameter is NULL
 *    all selections are removed from the control.
 */
void 
SurfaceSelectionViewController::updateControl(SurfaceSelectionModel* selectionModel)
{
    std::vector<Surface*> surfaces;
    Surface* selectedSurface = NULL;
    
    if (selectionModel != NULL) {
        surfaces = selectionModel->getAvailableSurfaces();
        selectedSurface = selectionModel->getSurface();
    }
    
    this->surfaceComboBox->blockSignals(true);
    
    int32_t selectedIndex = 0;
    
    this->surfaceComboBox->clear();
    const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
    for (int32_t i = 0; i < numSurfaces; i++) {
        const int32_t indx = this->surfaceComboBox->count();
        this->surfaceComboBox->addItem(surfaces[i]->getFileNameNoPath());
        this->surfaceComboBox->setItemData(indx, qVariantFromValue((void*)surfaces[i]));
        
        if (surfaces[i] == selectedSurface) {
            selectedIndex = indx;
        }
    }
    
    AString toolTipText;
    if (numSurfaces > 0) {
        this->surfaceComboBox->setCurrentIndex(selectedIndex);
        
        /*
         * ToolTip contains file and path
         */
        CaretAssertVectorIndex(surfaces, selectedIndex);
        const Surface* s = surfaces[selectedIndex];
        if (s != NULL) {
            FileInformation fileInfo(s->getFileName());
            toolTipText = ("Name: "
                           + fileInfo.getFileName()
                           + "\nPath: "
                           + fileInfo.getPathName());
        }
    }
    this->surfaceComboBox->setToolTip(toolTipText);
    
    this->surfaceComboBox->blockSignals(false);
    
}

/**
 * Update the control.
 */
void 
SurfaceSelectionViewController::updateControl()
{
    CaretAssertMessage(this->surfaceSelectionModel,
                       "The surface selection model is NULL, you should have called "
                       "updateControl(SurfaceSelectionModel*)");
    this->updateControl(this->surfaceSelectionModel);
}

/**
 * @return the actual widget.
 */
QWidget* 
SurfaceSelectionViewController::getWidget()
{
    return this->surfaceComboBox;
}

/**
 * @return the selected surface.  NULL if no surface selected.
 */
Surface* 
SurfaceSelectionViewController::getSurface()
{
    //return this->surfaceSelectionModel->getSurface();
    
    Surface* s = NULL;
    
    const int indx = this->surfaceComboBox->currentIndex();
    if ((indx >= 0)
        && (indx < this->surfaceComboBox->count())) {
        void* pointer = this->surfaceComboBox->itemData(indx).value<void*>();
        s = (Surface*)pointer;
    }
    
    return s;
}

/**
 * Set the selected surface.
 * @param surface
 *    The selected surface.
 */
void 
SurfaceSelectionViewController::setSurface(Surface* surface)
{
    if (this->surfaceSelectionModel != NULL) {
        this->surfaceSelectionModel->setSurface(surface);
        this->updateControl();
    }
    else {
        const int32_t numItems = this->surfaceComboBox->count();
        for (int32_t i = 0; i < numItems; i++) {
            void* pointer = this->surfaceComboBox->itemData(i).value<void*>();
            Surface* s = (Surface*)pointer;
            if (surface == s) {
                this->surfaceComboBox->blockSignals(true);
                this->surfaceComboBox->setCurrentIndex(i);
                this->surfaceComboBox->blockSignals(false);
                break;
            }
        }
    }
}

/**
 * Called when the item in the combo box is changed.
 * @param indx
 *    Index of item that was selected.
 */
void 
SurfaceSelectionViewController::comboBoxCurrentIndexChanged(int indx)
{
    void* pointer = this->surfaceComboBox->itemData(indx).value<void*>();
    Surface* s = (Surface*)pointer;
    
    if (this->surfaceSelectionModel != NULL) {
        this->surfaceSelectionModel->setSurface(s);
    }
    
    emit surfaceSelected(s);
}
