
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

#define __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineColorOrTabViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_VIEW_CONTROLLER_DECLARE__

#include <QComboBox>

#include "WuQFactory.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineColorOrTabViewController 
 * \brief View Controller for VolumeSurfaceOutlineColorOrTabModel
 *
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineColorOrTabViewController::VolumeSurfaceOutlineColorOrTabViewController(QObject* parent)
: WuQWidget(parent)
{
    this->colorOrTabModel = NULL;
    this->modelComboBox = WuQFactory::newComboBox();
    QObject::connect(this->modelComboBox, 
                     SIGNAL(activated(int)), 
                     this, 
                     SLOT(itemActivated(int)));
    
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineColorOrTabViewController::~VolumeSurfaceOutlineColorOrTabViewController()
{
    
}

/**
 * Update this view controller.
 */
void 
VolumeSurfaceOutlineColorOrTabViewController::updateViewController(VolumeSurfaceOutlineColorOrTabModel* model)
{
    this->colorOrTabModel = model;
    this->modelComboBox->blockSignals(true);
    this->modelComboBox->clear();
    
    VolumeSurfaceOutlineColorOrTabModel::Item* selectedItem = this->colorOrTabModel->getSelectedItem();
    int32_t selectedItemIndex = -1;
    
    std::vector<VolumeSurfaceOutlineColorOrTabModel::Item*> validItems =
        this->colorOrTabModel->getValidItems();
    const int32_t numItems = static_cast<int32_t>(validItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        VolumeSurfaceOutlineColorOrTabModel::Item* item = validItems[i];
        if (selectedItem == item) {
            selectedItemIndex = i;
        }
        
        this->modelComboBox->addItem(item->getName(),
                                     QVariant::fromValue((void*)item));
    }
    
    if (selectedItemIndex >= 0) {
        this->modelComboBox->setCurrentIndex(selectedItemIndex);
    }
    
    this->modelComboBox->blockSignals(false);
}

/**
 * @return The widget for this view controller.
 */
QWidget* 
VolumeSurfaceOutlineColorOrTabViewController::getWidget()
{
    return this->modelComboBox;
}

/**
 * Called when the user selects an item.
 * @param indx
 *   Index of item selected by the user.
 */
void 
VolumeSurfaceOutlineColorOrTabViewController::itemActivated(int indx)
{
    if (this->colorOrTabModel == NULL) {
        return;
    }
    
    if (indx >= 0) {
        void* pointer = this->modelComboBox->itemData(indx).value<void*>();
        VolumeSurfaceOutlineColorOrTabModel::Item* item = (VolumeSurfaceOutlineColorOrTabModel::Item*)pointer;
        this->colorOrTabModel->setSelectedItem(item);
        
        emit modelSelected(item);
    }
}

