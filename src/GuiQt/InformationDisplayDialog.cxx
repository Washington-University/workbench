
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

#include <QAction>
#include <QBoxLayout>
#include <QSpinBox>
#include <QToolBar>

#define __INFORMATION_DISPLAY_DIALOG_DECLARE__
#include "InformationDisplayDialog.h"
#undef __INFORMATION_DISPLAY_DIALOG_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventUpdateInformationWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "HyperLinkTextBrowser.h"
#include "IdentificationManager.h"
#include "InformationDisplayPropertiesDialog.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionManager.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::InformationDisplayDialog 
 * \brief Dialog for display of information.
 * \ingroup GuiQt
 *
 */

/**
 * Constructor.
 */
InformationDisplayDialog::InformationDisplayDialog(BrainBrowserWindow* parent)
: WuQDialogNonModal("Information",
                    parent)
{
    this->setDeleteWhenClosed(false);
    
    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    m_propertiesDialog = NULL;
    
    m_informationTextBrowser = new HyperLinkTextBrowser();
    m_informationTextBrowser->setLineWrapMode(QTextEdit::NoWrap);
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
                                                                "Remove All ID symbols",
                                                                this,
                                                                this,
                                                                SLOT(removeIdSymbols()));
    
    QAction* settingsAction = WuQtUtilities::createAction("Properties",
                                                          "Displays dialog for changing ID symbol colors and size",
                                                          this,
                                                          this,
                                                          SLOT(showPropertiesDialog()));
        
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
    idToolBarRight->addAction(settingsAction);
    idToolBarRight->addSeparator();
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(idToolBarLeft);
    layout->addWidget(m_informationTextBrowser);
    layout->addWidget(idToolBarRight);
    layout->setStretchFactor(idToolBarLeft, 0);
    layout->setStretchFactor(m_informationTextBrowser, 100);
    layout->setStretchFactor(idToolBarRight, 0);
    
    /*
     * Use processed event listener since the text event
     * is first processed by GuiManager which will create
     * this dialog, if needed, and then display it.
     */
    EventManager::get()->addProcessedEventListener(this,
                                                   EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS);
    this->setCentralWidget(widget,
                           WuQDialog::SCROLL_AREA_NEVER);
    
    /*
     * There may already be identification text, so try to display it.
     */
    updateDialog();
    
}

/**
 * Destructor.
 */
InformationDisplayDialog::~InformationDisplayDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the dialog's content.
 */
void 
InformationDisplayDialog::updateDialog()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    const AString text = idManager->getIdentificationText();
    m_informationTextBrowser->setContentToHtml(text);
    
    if (m_propertiesDialog != NULL) {
        m_propertiesDialog->updateDialog();
    }
    
    m_contralateralIdentificationAction->blockSignals(true);
    m_contralateralIdentificationAction->setChecked(idManager->isContralateralIdentificationEnabled());
    m_contralateralIdentificationAction->blockSignals(false);
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
InformationDisplayDialog::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "InformationDisplayDialog",
                                            1);
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
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
InformationDisplayDialog::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }

    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    swg.restoreFromScene(sceneAttributes, sceneClass->getClass("geometry"));    
}

/**
 * Called when contralateral toolbutton is toggled.
 */
void
InformationDisplayDialog::contralateralIdentificationToggled(bool)
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->setContralateralIdentificationEnabled(m_contralateralIdentificationAction->isChecked());
}

/**
 * Clear the information text.
 */
void
InformationDisplayDialog::clearInformationText()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->removeIdentificationText();
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


/**
 * Remove ID symbols from all surfaces.
 */
void
InformationDisplayDialog::removeIdSymbols()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->removeAllIdentifiedSymbols();
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
InformationDisplayDialog::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS) {
        EventUpdateInformationWindows* textEvent =
        dynamic_cast<EventUpdateInformationWindows*>(event);
        CaretAssert(textEvent);
        textEvent->setEventProcessed();
        
        doUpdate = true;
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiUpdateEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiUpdateEvent);
        uiUpdateEvent->setEventProcessed();
        
        doUpdate = true;
    }
    
    if (doUpdate) {
        updateDialog();
    }
}

/**
 * Show the symbol properties dialog
 */
void
InformationDisplayDialog::showPropertiesDialog()
{
    if (m_propertiesDialog == NULL) {
        m_propertiesDialog = new InformationDisplayPropertiesDialog(this);
    }
    m_propertiesDialog->showDialog();
}

