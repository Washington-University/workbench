
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WB_MACRO_WIDGET_ACTIONS_MANAGER_DECLARE__
#include "WbMacroWidgetActionsManager.h"
#undef __WB_MACRO_WIDGET_ACTIONS_MANAGER_DECLARE__

#include <QVariant>

#include "CaretAssert.h"
#include "Brain.h"
#include "DisplayPropertiesSurface.h"
#include "GuiManager.h"
#include "WbMacroWidgetActionNames.h"
#include "WuQMacroWidgetAction.h"

using namespace caret;


    
/**
 * \class caret::WbMacroWidgetActionsManager 
 * \brief Manager for macro widget actions used by workbench
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent object
 */
WbMacroWidgetActionsManager::WbMacroWidgetActionsManager(QObject* parent)
: QObject(parent)
{
    
}

/**
 * Destructor.
 */
WbMacroWidgetActionsManager::~WbMacroWidgetActionsManager()
{
}

/**
 * @return All macro widget actions used by workbench
 */
std::vector<WuQMacroWidgetAction*>
WbMacroWidgetActionsManager::getMacroWidgetActions()
{
    if (m_macroWidgetActions.empty()) {
        m_surfacePropertiesLinkDiameterWidgetAction = createSurfacePropertiesLinkDiameterWidgetAction();
        m_macroWidgetActions.push_back(m_surfacePropertiesLinkDiameterWidgetAction);
        
        m_surfacePropertiesOpacityWidgetAction = createSurfacePropertiesOpacityWidgetAction();
        m_macroWidgetActions.push_back(m_surfacePropertiesOpacityWidgetAction);
        
        m_surfacePropertiesVertexDiameterWidgetAction = createSurfacePropertiesVertexDiameterWidgetAction();
        m_macroWidgetActions.push_back(m_surfacePropertiesVertexDiameterWidgetAction);
    }
    
    return m_macroWidgetActions;
}

/**
 * @return New instance of surface opacity widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::createSurfacePropertiesOpacityWidgetAction()
{
    WuQMacroWidgetAction* action = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                                            WbMacroWidgetActionNames::getSurfacePropertiesOpacityName(),
                                                            "Set the surface opacity",
                                                            parent());
    
    DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    
    QObject::connect(action, &WuQMacroWidgetAction::getModelValue,
                     this, [=](QVariant& value) {
                         value = dsp->getOpacity();
                     });
    
    QObject::connect(action, &WuQMacroWidgetAction::setModelValue,
                     this, [=](const QVariant& value) {
                         GuiManager::get()->getBrain()->getDisplayPropertiesSurface()->setOpacity(value.toFloat());
                         GuiManager::updateSurfaceColoring();
                         GuiManager::updateGraphicsAllWindows();
                     });
    
    return action;
}

/**
 * @return The surface opacity widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesOpacityWidgetAction()
{
    CaretAssert(m_surfacePropertiesOpacityWidgetAction);
    return m_surfacePropertiesOpacityWidgetAction;
}

/**
 * @return The surface link diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesLinkDiameterWidgetAction()
{
    return m_surfacePropertiesLinkDiameterWidgetAction;
}

/**
 * @return New instance of surface link diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::createSurfacePropertiesLinkDiameterWidgetAction()
{
    WuQMacroWidgetAction* action = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                                            WbMacroWidgetActionNames::getSurfacePropertiesLinkDiameterName(),
                                                            "Set the link (edge) diameter",
                                                            parent());
    
    DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    
    QObject::connect(action, &WuQMacroWidgetAction::getModelValue,
                     this, [=](QVariant& value) {
                         value = dsp->getLinkSize();
                     });
    
    QObject::connect(action, &WuQMacroWidgetAction::setModelValue,
                     this, [=](const QVariant& value) {
                         GuiManager::get()->getBrain()->getDisplayPropertiesSurface()->setLinkSize(value.toFloat());
                         GuiManager::updateGraphicsAllWindows();
                     });
    
    return action;
}

/**
 * @return New instance of surface vertex diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::createSurfacePropertiesVertexDiameterWidgetAction()
{
    WuQMacroWidgetAction* action = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                                            WbMacroWidgetActionNames::getSurfacePropertiesVertexDiameterName(),
                                                            "Set the vertex diameter",
                                                            parent());
    
    DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    
    QObject::connect(action, &WuQMacroWidgetAction::getModelValue,
                     this, [=](QVariant& value) {
                         value = dsp->getNodeSize();
                     });
    
    QObject::connect(action, &WuQMacroWidgetAction::setModelValue,
                     this, [=](const QVariant& value) {
                         GuiManager::get()->getBrain()->getDisplayPropertiesSurface()->setNodeSize(value.toFloat());
                         GuiManager::updateGraphicsAllWindows();
                     });
    
    return action;
}

/**
 * @return The surface link diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesVertexDiameterWidgetAction()
{
    return m_surfacePropertiesVertexDiameterWidgetAction;
}
