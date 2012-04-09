
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

#define __INFORMATION_DISPLAY_WIDGET_DECLARE__
#include "InformationDisplayWidget.h"
#undef __INFORMATION_DISPLAY_WIDGET_DECLARE__

#include <QAction>
#include <QBoxLayout>
#include <QToolBar>

#include "Brain.h"
#include "DisplayPropertiesInformation.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventInformationTextDisplay.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "HyperLinkTextBrowser.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::InformationDisplayWidget 
 * \brief Controls and displays information.
 */

/**
 * Constructor.
 */
InformationDisplayWidget::InformationDisplayWidget(QWidget* parent)
: QWidget(parent)
{
    this->informationTextBrowser = new HyperLinkTextBrowser();
    this->informationTextBrowser->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    QAction* clearAction = WuQtUtilities::createAction("Clear", 
                                                       "Clear contents of information display", 
                                                       this, 
                                                       this, 
                                                       SLOT(clearInformationText()));

    this->contralateralIdentificationAction = WuQtUtilities::createAction("Contra ID", 
                                                                          "Enable contralateral identification", 
                                                                          this, 
                                                                          this, 
                                                                          SLOT(contralateralIdentificationToggled(bool)));
    this->contralateralIdentificationAction->setCheckable(true);
    
    QAction* copyAction = WuQtUtilities::createAction("Copy", 
                                                      "Copy selection from information display", 
                                                      this, 
                                                      this->informationTextBrowser, 
                                                      SLOT(copy()));
    
    QAction* removeIdSymbolAction = WuQtUtilities::createAction("RID", 
                                                               "Remove ID symbols from ALL surfaces.", 
                                                               this, 
                                                               this, 
                                                               SLOT(removeIdSymbols()));
    
    QObject::connect(this->informationTextBrowser, SIGNAL(copyAvailable(bool)),
                     copyAction, SLOT(setEnabled(bool)));
    copyAction->setEnabled(false);
    
    QToolBar* idToolBarLeft = new QToolBar();
    idToolBarLeft->setOrientation(Qt::Vertical);
    idToolBarLeft->setFloatable(false);
    idToolBarLeft->setMovable(false);
    idToolBarLeft->addAction(clearAction);
    idToolBarLeft->addSeparator();
    idToolBarLeft->addAction(copyAction);
    idToolBarLeft->addSeparator();
    
    QToolBar* idToolBarRight = new QToolBar();
    idToolBarRight->setOrientation(Qt::Vertical);
    idToolBarRight->setFloatable(false);
    idToolBarRight->setMovable(false);
    idToolBarRight->addAction(removeIdSymbolAction);
    idToolBarRight->addSeparator();
    idToolBarRight->addAction(this->contralateralIdentificationAction);
    idToolBarRight->addSeparator();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(idToolBarLeft);
    layout->addWidget(this->informationTextBrowser);
    layout->addWidget(idToolBarRight);
    layout->setStretchFactor(idToolBarLeft, 0);
    layout->setStretchFactor(this->informationTextBrowser, 100);
    layout->setStretchFactor(idToolBarRight, 0);
    
    InformationDisplayWidget::allInformationDisplayWidgets.insert(this);
    this->updateInformationDisplayWidget();
}

/**
 * Destructor.
 */
InformationDisplayWidget::~InformationDisplayWidget()
{
    InformationDisplayWidget::allInformationDisplayWidgets.erase(this);
}

/**
 * Called when contralateral toolbutton is toggled.
 * @param
 */
void 
InformationDisplayWidget::contralateralIdentificationToggled(bool)
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    info->setContralateralIdentificationEnabled(this->contralateralIdentificationAction->isChecked());
    InformationDisplayWidget::updateAllInformationDisplayWidgets();
}

/**
 * Clear the information text.
 */
void 
InformationDisplayWidget::clearInformationText()
{
    this->informationTextBrowser->setText("");
}


/**
 * Remove ID symbols from all surfaces.
 */
void 
InformationDisplayWidget::removeIdSymbols()
{
    EventManager::get()->sendEvent(EventIdentificationSymbolRemoval().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update this information widget.
 */
void 
InformationDisplayWidget::updateInformationDisplayWidget()
{
    this->contralateralIdentificationAction->blockSignals(true);
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    this->contralateralIdentificationAction->setChecked(info->isContralateralIdentificationEnabled());
    this->contralateralIdentificationAction->blockSignals(false);
}

void 
InformationDisplayWidget::updateAllInformationDisplayWidgets()
{
    for (std::set<InformationDisplayWidget*>::iterator iter = InformationDisplayWidget::allInformationDisplayWidgets.begin();
         iter != InformationDisplayWidget::allInformationDisplayWidgets.end();
         iter++) {
        InformationDisplayWidget* idw = *iter;
        if (idw != this) {
            idw->updateInformationDisplayWidget();
        }
    }
}

/**
 * Process a information display event directly routed to this instance..
 * 
 * @param informationEvent
 *   The information display event.
 */
void 
InformationDisplayWidget::processTextEvent(EventInformationTextDisplay* informationEvent)
{
    const AString text = informationEvent->getText();
    if (text.isEmpty() == false) {
        switch(informationEvent->getTextType()) {
            case EventInformationTextDisplay::TYPE_PLAIN:
                this->informationTextBrowser->append(informationEvent->getText());
                break;
            case EventInformationTextDisplay::TYPE_HTML:
                this->informationTextBrowser->appendHtml(informationEvent->getText());
                break;
        }
    }
}
