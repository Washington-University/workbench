
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#define __MULTI_PAGE_DIALOG_PAGE_DECLARE__
#include "MultiPageDialogPage.h"
#undef __MULTI_PAGE_DIALOG_PAGE_DECLARE__

#include "CaretColorEnumSelectionControl.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "SurfaceSelectionControl.h"
#include "VolumeSurfaceOutlineColorOrTabViewController.h"

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class MultiPageDialogPage 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */
/**
 * Constructor.
 * @param pageName
 *    Name of page.
 */
MultiPageDialogPage::MultiPageDialogPage(const AString& pageName)
: QObject()
{
    this->pageName = pageName;
    this->pageWidget = NULL;
    this->isPageUpdateInProgress = false;
}

/**
 * Destructor.
 */
MultiPageDialogPage::~MultiPageDialogPage()
{
    
}

/**
 * Will be called when Apply button is pressed.
 */
void 
MultiPageDialogPage::applyPage()
{
    if (this->isPageUpdateInProgress) {
        return;
    }

    CaretAssert(this->pageWidget);

    this->applyPageContent();
}

/**
 * Update the contents of the page.
 */
void 
MultiPageDialogPage::updatePage()
{
    this->getPageWidget();
    
    CaretAssert(this->pageWidget);
    
    this->isPageUpdateInProgress = true;
    
    this->updatePageContent();
    
    this->isPageUpdateInProgress = false;
}

/**
 * Get the page's widget.
 * If the page has not been created, it will be created at
 * this time.
 * @return  Widget containing page content.
 */
QWidget* 
MultiPageDialogPage::getPageWidget()
{
    if (this->pageWidget == NULL) {
        this->pageWidget = this->createPageContent();
        CaretAssert(this->pageWidget);
    }
    
    return this->pageWidget;
}

/**
 * @return The name of the page.
 */
AString 
MultiPageDialogPage::getPageName() const
{
    return this->pageName;
}

/**
 * Add a widget to this page.
 * @param widget
 *    The widget.
 * @param isValueChangedSignalConnectedToApplyPage
 *    If true, the controls value changed signal will
 *    be connected to the applyPage() method.
 */
void 
MultiPageDialogPage::addWidget(QObject* widget,
                               const bool isValueChangedSignalConnectedToApplyPage)
{
    if (dynamic_cast<QCheckBox*>(widget) != NULL) {
        QCheckBox* cb = dynamic_cast<QCheckBox*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(cb, SIGNAL(toggled(bool)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<QComboBox*>(widget) != NULL) {
        QComboBox* cb = dynamic_cast<QComboBox*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(cb, SIGNAL(currentIndexChanged(int)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<QDoubleSpinBox*>(widget) != NULL) {
        QDoubleSpinBox* dsb = dynamic_cast<QDoubleSpinBox*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(dsb, SIGNAL(valueChanged(double)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<QSpinBox*>(widget) != NULL) {
        QSpinBox* sb = dynamic_cast<QSpinBox*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(sb, SIGNAL(valueChanged(int)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<CaretColorEnumSelectionControl*>(widget) != NULL) {
        CaretColorEnumSelectionControl* cc = dynamic_cast<CaretColorEnumSelectionControl*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(cc, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<SurfaceSelectionControl*>(widget) != NULL) {
        SurfaceSelectionControl* ss = dynamic_cast<SurfaceSelectionControl*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(ss, SIGNAL(surfaceSelected(Surface*)),
                             this, SLOT(applyPage()));
        }
    }
    else if (dynamic_cast<VolumeSurfaceOutlineColorOrTabViewController*>(widget) != NULL) {
        VolumeSurfaceOutlineColorOrTabViewController* ct = dynamic_cast<VolumeSurfaceOutlineColorOrTabViewController*>(widget);
        if (isValueChangedSignalConnectedToApplyPage) {
            QObject::connect(ct, SIGNAL(modelSelected(VolumeSurfaceOutlineColorOrTabModel::Item*)),
                             this, SLOT(applyPage()));
        }
    }
    else {
        CaretAssertMessage(0, ("Unrecognized widget type \""
                               + widget->objectName()));
    }    
}

/**
 * Update all graphics.
 */
void 
MultiPageDialogPage::updateAllGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

