
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
#include <QDoubleSpinBox>
#include <QToolBar>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "DisplayPropertiesInformation.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventInformationTextDisplay.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "HyperLinkTextBrowser.h"
#include "WuQtUtilities.h"
#include "WuQDataEntryDialog.h"

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
    
    QAction* settingsAction = WuQtUtilities::createAction("Properties",
                                                          "Displays dialog for changing ID symbol colors and size",
                                                          this,
                                                          this,
                                                          SLOT(showPropertiesDialog()));
    
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
    idToolBarRight->addAction(settingsAction);
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
    
    /*
     * Use processed event listener since the text event
     * is first processed by GuiManager which will create
     * this dialog, if needed, and then display it.
     */
    EventManager::get()->addProcessedEventListener(this, 
                                                   EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY);
}

/**
 * Destructor.
 */
InformationDisplayWidget::~InformationDisplayWidget()
{
    InformationDisplayWidget::allInformationDisplayWidgets.erase(this);
    EventManager::get()->removeAllEventsFromListener(this);
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

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
InformationDisplayWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY) {
        EventInformationTextDisplay* textEvent =
        dynamic_cast<EventInformationTextDisplay*>(event);
        CaretAssert(textEvent);
        
        textEvent->setEventProcessed();
        
        const AString text = textEvent->getText();
        if (text.isEmpty() == false) {
            this->processTextEvent(textEvent);
            textEvent->setEventProcessed();
        }
    }
    else {
    }
}

/**
 * Show the symbol properties dialog
 */
void 
InformationDisplayWidget::showPropertiesDialog()
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    
    WuQDataEntryDialog ded("Symbol Properties",
                           this);
    CaretColorEnumComboBox* idColorComboBox = ded.addCaretColorEnumComboBox("ID Symbol Color:",
                                                                             info->getIdentificationSymbolColor());
                                                                             
    CaretColorEnumComboBox* idContraColorComboBox = ded.addCaretColorEnumComboBox("ID Contralateral Symbol Color:",
                                                                             info->getIdentificationContralateralSymbolColor());
    QDoubleSpinBox* sizeSpinBox = ded.addDoubleSpinBox("Symbol Size:", 
                                                        info->getIdentificationSymbolSize(),
                                                       0.5,
                                                       100000.0,
                                                       0.5);
    
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        info->setIdentificationSymbolColor(idColorComboBox->getSelectedColor());
        info->setIdentificationContralateralSymbolColor(idContraColorComboBox->getSelectedColor());
        info->setIdentificationSymbolSize(sizeSpinBox->value());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


