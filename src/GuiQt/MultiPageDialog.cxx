
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

#define __MULTI_PAGE_DIALOG_DECLARE__
#include "MultiPageDialog.h"
#undef __MULTI_PAGE_DIALOG_DECLARE__

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>

#include "CaretAssert.h"
#include "MultiPageDialogPage.h"

using namespace caret;


    
/**
 * \class MultiPageDialog 
 * \brief Dialog that contains multiple pages.
 *
 * Dialog that contains multiple pages.
 */
/**
 * Constructor.
 */
MultiPageDialog::MultiPageDialog(const AString& dialogTitle,
                                 QWidget* parent,
                                 Qt::WindowFlags flags)
: WuQDialogNonModal(dialogTitle,
                    parent,
                    flags)
{
    QLabel* pageSelectionLabel = new QLabel("Page: ");
    this->pageSelectionComboBox = new QComboBox();
    QObject::connect(this->pageSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(pageSelectionComboBoxChanged(int)));
    
    QHBoxLayout* pageSelectionLayout = new QHBoxLayout();
    pageSelectionLayout->addWidget(pageSelectionLabel, 0);
    pageSelectionLayout->addWidget(this->pageSelectionComboBox, 100);
    
    this->stackedWidget = new QStackedWidget();
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addLayout(pageSelectionLayout, 0);
    layout->addWidget(this->stackedWidget, 100);
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
MultiPageDialog::~MultiPageDialog()
{
    const int32_t numPages = this->pageSelectionComboBox->count();
    for (int32_t i = 0; i < numPages; i++) {
        void* pointer = this->pageSelectionComboBox->itemData(i).value<void*>();
        MultiPageDialogPage* p = (MultiPageDialogPage*)pointer;
        delete p;
    }
    
    this->pageSelectionComboBox->clear();
}

/** 
 * May be called requesting the dialog to update its content 
 */
void 
MultiPageDialog::updateDialog()
{
    MultiPageDialogPage* page = this->getDisplayedPage();
    this->showPage(page);
}

/**
 * Add a page to the dialog.
 * @param page
 *    Page this is added.
 * @param isCreatedImmediately
 *    If true, the page's createPageContent() method is called
 *    to immediately create the page's widgets.  Otherwise,
 *    the page's widgets are created when it is first displayed.
 */
void 
MultiPageDialog::addPage(MultiPageDialogPage* page,
                         const bool isCreatedImmediately)
{
    CaretAssert(page);
    
    if (isCreatedImmediately) {
        page->getPageWidget();
    }
    
    this->pageSelectionComboBox->blockSignals(true);
    
    const int32_t comboBoxIndex = this->pageSelectionComboBox->count();
    this->pageSelectionComboBox->addItem(page->getPageName());
    this->pageSelectionComboBox->setItemData(comboBoxIndex, 
                                             qVariantFromValue((void*)page));
    
    this->pageSelectionComboBox->blockSignals(false);
}

/**
 * Show the page.
 * @param page
 *    Page this is displayed.
 */
void 
MultiPageDialog::showPage(MultiPageDialogPage* page)
{
    const int32_t numPages = this->pageSelectionComboBox->count();
    for (int32_t i = 0; i < numPages; i++) {
        void* pointer = this->pageSelectionComboBox->itemData(i).value<void*>();
        MultiPageDialogPage* p = (MultiPageDialogPage*)pointer;
        
        if (page == p) {
            p->updatePage();
            
            QWidget* pageWidget = p->getPageWidget();
            if (this->stackedWidget->indexOf(pageWidget) < 0) {
                this->stackedWidget->addWidget(pageWidget);
            }
            this->stackedWidget->setCurrentWidget(pageWidget);
            break;
        }
    }
}

/**
 * Called when the page selection combo box is changed.
 * @param indx
 *    Index of the page.
 */
void MultiPageDialog::pageSelectionComboBoxChanged(int indx)
{
    if (indx >= 0) {
        void* pointer = this->pageSelectionComboBox->itemData(indx).value<void*>();
        MultiPageDialogPage* p = (MultiPageDialogPage*)pointer;
        this->showPage(p);
    }
}

/**
 * Called when the apply button is pressed.
 */
void 
MultiPageDialog::applyButtonPressed()
{
    MultiPageDialogPage* page = this->getDisplayedPage();
    if (page != NULL) {
        page->applyPage();
    }
}

/**
 * @return The displayed page.
 */ 
MultiPageDialogPage* 
MultiPageDialog::getDisplayedPage()
{
    const int32_t indx = this->pageSelectionComboBox->currentIndex();
    if (indx >= 0) {
        void* pointer = this->pageSelectionComboBox->itemData(indx).value<void*>();
        MultiPageDialogPage* p = (MultiPageDialogPage*)pointer;
        return p;
    }
    
    return NULL;
}
