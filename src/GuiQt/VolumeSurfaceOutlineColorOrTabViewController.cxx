
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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
                                     qVariantFromValue((void*)item));
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

