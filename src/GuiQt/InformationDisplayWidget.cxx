
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

#include <limits>

#define __INFORMATION_DISPLAY_WIDGET_DECLARE__
#include "InformationDisplayWidget.h"
#undef __INFORMATION_DISPLAY_WIDGET_DECLARE__

#include <QAction>
#include <QBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QToolBar>

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventUpdateInformationWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "HyperLinkTextBrowser.h"
#include "IdentificationManager.h"
#include "IdentifyBrainordinateDialog.h"
#include "SceneClass.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionManager.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "WuQtUtilities.h"
#include "WuQDataEntryDialog.h"

using namespace caret;


    
/**
 * \class caret::InformationDisplayWidget 
 * \brief Controls and displays information.
 * \ingroup GuiQt
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
    m_propertiesDialogMostRecentSizeSpinBox = NULL;
    
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
                                                                        "Enable volume slice movement to selected brainordinate.", 
                                                                        this, 
                                                                        this, 
                                                                        SLOT(volumeSliceIdentificationToggled(bool)));
    m_volumeSliceIdentificationAction->setCheckable(true);
    
    QAction* identifySurfaceAction = WuQtUtilities::createAction("Select\nBrainordinate",
                                                                 "Enter a brainordinate for identification",
                                                                 this,
                                                                 this,
                                                                 SLOT(identifyBrainordinateTriggered()));
    
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
    idToolBarRight->addAction(identifySurfaceAction);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(idToolBarLeft);
    layout->addWidget(m_informationTextBrowser);
    layout->addWidget(idToolBarRight);
    layout->setStretchFactor(idToolBarLeft, 0);
    layout->setStretchFactor(m_informationTextBrowser, 100);
    layout->setStretchFactor(idToolBarRight, 0);
    
    s_allInformationDisplayWidgets.insert(this);
    
    /*
     * There may already be identification text, so try to display it.
     */
    updateInformationDisplayWidget();
    
    /*
     * Initialize for brainordinate entry
     */
    m_brainordinateEntryStructure = StructureEnum::INVALID;
    m_brainordinateEntryNodeIndex = 0;
    m_brainordinateEntryDialogPosition[0] = -1;
    m_brainordinateEntryDialogPosition[1] = -1;
    
    /*
     * Use processed event listener since the text event
     * is first processed by GuiManager which will create
     * this dialog, if needed, and then display it.
     */
    EventManager::get()->addProcessedEventListener(this, 
                                                   EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS);
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
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->setContralateralIdentificationEnabled(m_contralateralIdentificationAction->isChecked());
    updateAllInformationDisplayWidgets();
}

/**
 * Called when volume identification toolbutton is toggled.
 */
void 
InformationDisplayWidget::volumeSliceIdentificationToggled(bool)
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->setVolumeIdentificationEnabled(m_volumeSliceIdentificationAction->isChecked());
    updateAllInformationDisplayWidgets();    
}

/**
 * Clear the information text.
 */
void 
InformationDisplayWidget::clearInformationText()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->removeIdentificationText();
    updateAllInformationDisplayWidgets();
}


/**
 * Remove ID symbols from all surfaces.
 */
void 
InformationDisplayWidget::removeIdSymbols()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->removeAllIdentifiedNodes();
    updateAllInformationDisplayWidgets();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update this information widget.
 */
void 
InformationDisplayWidget::updateInformationDisplayWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    const AString text = idManager->getIdentificationText();
    m_informationTextBrowser->setContentToHtml(text);
    
    m_contralateralIdentificationAction->blockSignals(true);
    m_contralateralIdentificationAction->setChecked(idManager->isContralateralIdentificationEnabled());
    m_contralateralIdentificationAction->blockSignals(false);
    m_volumeSliceIdentificationAction->blockSignals(true);
    m_volumeSliceIdentificationAction->setChecked(idManager->isVolumeIdentificationEnabled());
    m_volumeSliceIdentificationAction->blockSignals(false);
}

void 
InformationDisplayWidget::updateAllInformationDisplayWidgets()
{
    for (std::set<InformationDisplayWidget*>::iterator iter = s_allInformationDisplayWidgets.begin();
         iter != s_allInformationDisplayWidgets.end();
         iter++) {
        InformationDisplayWidget* idw = *iter;
        idw->updateInformationDisplayWidget();
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
        updateInformationDisplayWidget();
    }
}

/**
 * Show the symbol properties dialog
 */
void 
InformationDisplayWidget::showPropertiesDialog()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
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
    m_propertiesDialogSizeSpinBox->setDecimals(1);
    m_propertiesDialogSizeSpinBox->setSuffix("mm");
    QObject::connect(m_propertiesDialogSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(controlInPropertiesDialogChanged()));
    
    m_propertiesDialogMostRecentSizeSpinBox = ded.addDoubleSpinBox("Most Recent ID Symbol Size:",
                                                                   info->getMostRecentIdentificationSymbolSize(),
                                                                   0.5,
                                                                   100000.0,
                                                                   0.5);
    m_propertiesDialogMostRecentSizeSpinBox->setDecimals(1);
    m_propertiesDialogMostRecentSizeSpinBox->setSuffix("mm");
    QObject::connect(m_propertiesDialogMostRecentSizeSpinBox, SIGNAL(valueChanged(double)),
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
    IdentificationManager* info = brain->getIdentificationManager();
    info->setIdentificationSymbolColor(m_propertiesDialogIdColorComboBox->getSelectedColor());
    info->setIdentificationContralateralSymbolColor(m_propertiesDialogIdContraColorComboBox->getSelectedColor());
    info->setIdentificationSymbolSize(m_propertiesDialogSizeSpinBox->value());
    info->setMostRecentIdentificationSymbolSize(m_propertiesDialogMostRecentSizeSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Allow user to identify a brainordinate by structure/node index.
 */
void
InformationDisplayWidget::identifyBrainordinateTriggered()
{
    IdentifyBrainordinateDialog idd(this);
    idd.restoreWindowPositionFromLastDisplayedPosition();
    idd.exec();
    
//    WuQDataEntryDialog ded("Select Brainordinate",
//                           this);
//    
//    StructureEnumComboBox* structureComboBox = ded.addStructureEnumComboBox("Structure");
//    structureComboBox->listOnlyValidStructures();
//    structureComboBox->setSelectedStructure(m_brainordinateEntryStructure);
//    
//    QSpinBox* nodeIndexSpinBox = ded.addSpinBox("Node Index",
//                                                m_brainordinateEntryNodeIndex,
//                                                0,
//                                                std::numeric_limits<int32_t>::max());
//    ded.setDisplayedXY(m_brainordinateEntryDialogPosition);
//    
//    if (ded.exec() == WuQDataEntryDialog::Accepted) {
//        m_brainordinateEntryStructure = structureComboBox->getSelectedStructure();
//        m_brainordinateEntryNodeIndex = nodeIndexSpinBox->value();
//        
//        Brain* brain = GuiManager::get()->getBrain();
//        BrainStructure* bs = brain->getBrainStructure(m_brainordinateEntryStructure,
//                                                      false);
//        if (bs != NULL) {
//            if (m_brainordinateEntryNodeIndex < bs->getNumberOfNodes()) {
//                Surface* surface = bs->getVolumeInteractionSurface();
//                if (surface != NULL) {
//                    SelectionManager* selectionManager = brain->getSelectionManager();
//                    selectionManager->reset();
//                    SelectionItemSurfaceNode* nodeID = selectionManager->getSurfaceNodeIdentification();
//                    nodeID->setBrain(brain);
//                    nodeID->setNodeNumber(m_brainordinateEntryNodeIndex);
//                    nodeID->setSurface(surface);
//                    const float* fxyz = surface->getCoordinate(m_brainordinateEntryNodeIndex);
//                    const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
//                    nodeID->setModelXYZ(xyz);
//                    GuiManager::get()->processIdentification(selectionManager,
//                                                             &ded);
//                    
//                    m_brainordinateEntryDialogPosition[0] = ded.x();
//                    m_brainordinateEntryDialogPosition[1] = ded.y();
//                }
//            }
//        }
//    }
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
        return;
    }
}



