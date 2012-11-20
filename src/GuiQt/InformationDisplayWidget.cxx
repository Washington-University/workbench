
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
#include "SceneClass.h"
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
    m_propertiesDialogIdColorComboBox = NULL;
    m_propertiesDialogIdContraColorComboBox = NULL;
    m_propertiesDialogSizeSpinBox = NULL;
    
    m_informationTextBrowser = new HyperLinkTextBrowser();
    m_informationTextBrowser->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    QAction* clearAction = WuQtUtilities::createAction("Clear", 
                                                       "Clear contents of information display", 
                                                       this, 
                                                       this, 
                                                       SLOT(clearInformationText()));

    m_contralateralIdentificationAction = WuQtUtilities::createAction("Contra ID", 
                                                                          "Enable contralateral identification", 
                                                                          this, 
                                                                          this, 
                                                                          SLOT(contralateralIdentificationToggled(bool)));
    m_contralateralIdentificationAction->setCheckable(true);
    
    QAction* copyAction = WuQtUtilities::createAction("Copy", 
                                                      "Copy selection from information display", 
                                                      this, 
                                                      m_informationTextBrowser, 
                                                      SLOT(copy()));
    
    QAction* removeIdSymbolAction = WuQtUtilities::createAction("RID", 
                                                               "Remove ID symbols from ALL surfaces", 
                                                               this, 
                                                               this, 
                                                               SLOT(removeIdSymbols()));
    
    QAction* settingsAction = WuQtUtilities::createAction("Properties",
                                                          "Displays dialog for changing ID symbol colors and size",
                                                          this,
                                                          this,
                                                          SLOT(showPropertiesDialog()));
    
    m_volumeSliceIdentificationAction = WuQtUtilities::createAction("Volume ID", 
                                                                        "Enable volume slice movement to identified brainordinate.", 
                                                                        this, 
                                                                        this, 
                                                                        SLOT(volumeSliceIdentificationToggled(bool)));
    m_volumeSliceIdentificationAction->setCheckable(true);
    
    QObject::connect(m_informationTextBrowser, SIGNAL(copyAvailable(bool)),
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
    idToolBarRight->addAction(m_contralateralIdentificationAction);
    idToolBarRight->addSeparator();
    idToolBarRight->addAction(m_volumeSliceIdentificationAction);
    idToolBarRight->addSeparator();
    idToolBarRight->addAction(settingsAction);
    idToolBarRight->addSeparator();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(idToolBarLeft);
    layout->addWidget(m_informationTextBrowser);
    layout->addWidget(idToolBarRight);
    layout->setStretchFactor(idToolBarLeft, 0);
    layout->setStretchFactor(m_informationTextBrowser, 100);
    layout->setStretchFactor(idToolBarRight, 0);
    
    s_allInformationDisplayWidgets.insert(this);
    updateInformationDisplayWidget();
    clearInformationText();
    
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
    s_allInformationDisplayWidgets.erase(this);
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when contralateral toolbutton is toggled.
 */
void 
InformationDisplayWidget::contralateralIdentificationToggled(bool)
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    info->setContralateralIdentificationEnabled(m_contralateralIdentificationAction->isChecked());
    InformationDisplayWidget::updateAllInformationDisplayWidgets();
}

/**
 * Called when volume identification toolbutton is toggled.
 */
void 
InformationDisplayWidget::volumeSliceIdentificationToggled(bool)
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    info->setVolumeIdentificationEnabled(m_volumeSliceIdentificationAction->isChecked());
    InformationDisplayWidget::updateAllInformationDisplayWidgets();    
}

/**
 * Clear the information text.
 */
void 
InformationDisplayWidget::clearInformationText()
{
    m_informationText = "";
    m_informationText.reserve(32000);
    m_informationTextBrowser->setHtml("");
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
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    m_contralateralIdentificationAction->blockSignals(true);
    m_contralateralIdentificationAction->setChecked(info->isContralateralIdentificationEnabled());
    m_contralateralIdentificationAction->blockSignals(false);
    m_volumeSliceIdentificationAction->blockSignals(true);
    m_volumeSliceIdentificationAction->setChecked(info->isVolumeIdentificationEnabled());
    m_volumeSliceIdentificationAction->blockSignals(false);
}

void 
InformationDisplayWidget::updateAllInformationDisplayWidgets()
{
    for (std::set<InformationDisplayWidget*>::iterator iter = s_allInformationDisplayWidgets.begin();
         iter != s_allInformationDisplayWidgets.end();
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
        m_informationText.append(informationEvent->getText());
        m_informationText.append("<br><br>");
        m_informationTextBrowser->setContentToHtml(m_informationText);
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
            processTextEvent(textEvent);
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
    m_propertiesDialogIdColorComboBox = ded.addCaretColorEnumComboBox("ID Symbol Color:",
                                                                             info->getIdentificationSymbolColor());
    QObject::connect(m_propertiesDialogIdColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(controlInPropertiesDialogChanged()));
    
    m_propertiesDialogIdContraColorComboBox = ded.addCaretColorEnumComboBox("ID Contralateral Symbol Color:",
                           info->getIdentificationContralateralSymbolColor());
    QObject::connect(m_propertiesDialogIdContraColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(controlInPropertiesDialogChanged()));
    
    m_propertiesDialogSizeSpinBox = ded.addDoubleSpinBox("Symbol Size:", 
                                                        info->getIdentificationSymbolSize(),
                                                       0.5,
                                                       100000.0,
                                                       0.5);
    QObject::connect(m_propertiesDialogSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(controlInPropertiesDialogChanged()));
    
    ded.setOkButtonText("Close");
    ded.setCancelButtonText("");
    ded.exec();
    
    m_propertiesDialogIdColorComboBox = NULL;
    m_propertiesDialogIdContraColorComboBox = NULL;
    m_propertiesDialogSizeSpinBox = NULL;
}

/**
 * Gets called when a control in the Properties Dialog is changed.
 * Updates properties and graphics.
 */
void
InformationDisplayWidget::controlInPropertiesDialogChanged()
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesInformation* info = brain->getDisplayPropertiesInformation();
    info->setIdentificationSymbolColor(m_propertiesDialogIdColorComboBox->getSelectedColor());
    info->setIdentificationContralateralSymbolColor(m_propertiesDialogIdContraColorComboBox->getSelectedColor());
    info->setIdentificationSymbolSize(m_propertiesDialogSizeSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
InformationDisplayWidget::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "InformationDisplayWidget",
                                            1);
    sceneClass->addString("m_informationText",
                          m_informationText);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
InformationDisplayWidget::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        m_informationText = "";
    }
    else {
        m_informationText = sceneClass->getStringValue("m_informationText");
    }
    
    m_informationTextBrowser->setContentToHtml(m_informationText);
}



