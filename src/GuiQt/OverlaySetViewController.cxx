
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


#include <QGridLayout>

#define __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
#include "OverlaySetViewController.h"
#undef __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

#include "BrainConstants.h"
#include "OverlaySet.h"
#include "OverlayViewController.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetViewController 
 * \brief View Controller for an overlay set.
 *
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget.
 */
OverlaySetViewController::OverlaySetViewController(QWidget* parent)
: QWidget(parent)
{
    this->overlaySet = NULL;
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 100);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        const bool showTopHorizontalBar = (i > 0);
        
        OverlayViewController* ovc = new OverlayViewController(this,
                                                               gridLayout,
                                                               showTopHorizontalBar);
        this->overlayViewControllers.push_back(ovc);
    }
}

/**
 * Destructor.
 */
OverlaySetViewController::~OverlaySetViewController()
{
    
}

/**
 * Update this overlay set view controller using the given overlay set.
 * @param overlaySet
 *     Overlay set used in this view controller.
 */
void 
OverlaySetViewController::updateViewController(OverlaySet* overlaySet)
{
    this->overlaySet = overlaySet;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        Overlay* overlay = NULL;
        if (this->overlaySet != NULL) {
            overlay = overlaySet->getOverlay(i);
        }
        this->overlayViewControllers[i]->updateViewController(overlay);
    }
}


