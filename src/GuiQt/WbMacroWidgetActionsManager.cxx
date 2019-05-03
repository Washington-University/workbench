
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

#include <limits>

#include <QVariant>

#include "CaretAssert.h"
#include "CaretLogger.h"
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
        m_macroWidgetActions.push_back(getSurfacePropertiesLinkDiameterWidgetAction());
        
        m_macroWidgetActions.push_back(getSurfacePropertiesOpacityWidgetAction());
        
        m_macroWidgetActions.push_back(getSurfacePropertiesVertexDiameterWidgetAction());
        
        m_macroWidgetActions.push_back(getSurfacePropertiesDisplayNormalVectorsWidgetAction());

        m_macroWidgetActions.push_back(getSurfacePropertiesSurfaceDrawingTypeWidgetAction());
    }
    
    return m_macroWidgetActions;
}

/**
 * @return The surface opacity widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesOpacityWidgetAction()
{
    if (m_surfacePropertiesOpacityWidgetAction == NULL) {
        m_surfacePropertiesOpacityWidgetAction = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                 WbMacroWidgetActionNames::getSurfacePropertiesOpacityName(),
                                 "Set the surface opacity",
                                 this);
        m_surfacePropertiesOpacityWidgetAction->setDoubleSpinBoxMinMaxStepDecimals(0.0, 1.0, 0.1, 2);
        
        DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
        
        QObject::connect(m_surfacePropertiesOpacityWidgetAction, &WuQMacroWidgetAction::getModelValue,
                         this, [=](QVariant& value) {
                             value = dsp->getOpacity();
                         });
        
        QObject::connect(m_surfacePropertiesOpacityWidgetAction, &WuQMacroWidgetAction::setModelValue,
                         this, [=](const QVariant& value) {
                             dsp->setOpacity(value.toFloat());
                             GuiManager::updateSurfaceColoring();
                             GuiManager::updateGraphicsAllWindows();
                         });    }
    
    CaretAssert(m_surfacePropertiesOpacityWidgetAction);
    return m_surfacePropertiesOpacityWidgetAction;
}

/**
 * @return The surface link diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesLinkDiameterWidgetAction()
{
    if (m_surfacePropertiesLinkDiameterWidgetAction == NULL) {
        m_surfacePropertiesLinkDiameterWidgetAction = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                                                               WbMacroWidgetActionNames::getSurfacePropertiesLinkDiameterName(),
                                                                               "Set the link (edge) diameter",
                                                                               this);
        m_surfacePropertiesLinkDiameterWidgetAction->setDoubleSpinBoxMinMaxStepDecimals(0.0, std::numeric_limits<float>::max(), 1.0, 1);
        
        DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
        
        QObject::connect(m_surfacePropertiesLinkDiameterWidgetAction, &WuQMacroWidgetAction::getModelValue,
                         this, [=](QVariant& value) {
                             value = dsp->getLinkSize();
                         });
        
        QObject::connect(m_surfacePropertiesLinkDiameterWidgetAction, &WuQMacroWidgetAction::setModelValue,
                         this, [=](const QVariant& value) {
                             dsp->setLinkSize(value.toFloat());
                             GuiManager::updateGraphicsAllWindows();
                         });
    }
    
    return m_surfacePropertiesLinkDiameterWidgetAction;
}

/**
 * @return The surface link diameter widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesVertexDiameterWidgetAction()
{
    if (m_surfacePropertiesVertexDiameterWidgetAction == NULL) {
        m_surfacePropertiesVertexDiameterWidgetAction = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::SPIN_BOX_FLOAT,
                                                                                 WbMacroWidgetActionNames::getSurfacePropertiesVertexDiameterName(),
                                                                                 "Set the vertex diameter",
                                                                                 this);
        m_surfacePropertiesVertexDiameterWidgetAction->setDoubleSpinBoxMinMaxStepDecimals(0.0, std::numeric_limits<float>::max(), 1.0, 1);

        DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
        
        QObject::connect(m_surfacePropertiesVertexDiameterWidgetAction, &WuQMacroWidgetAction::getModelValue,
                         this, [=](QVariant& value) {
                             value = dsp->getNodeSize();
                         });
        
        QObject::connect(m_surfacePropertiesVertexDiameterWidgetAction, &WuQMacroWidgetAction::setModelValue,
                         this, [=](const QVariant& value) {
                             dsp->setNodeSize(value.toFloat());
                             GuiManager::updateGraphicsAllWindows();
                         });
    }
    return m_surfacePropertiesVertexDiameterWidgetAction;
}


/**
 * @return Get (and in needed create) the surface properties display normal vectors widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesDisplayNormalVectorsWidgetAction()
{
    if (m_surfacePropertiesDisplayNormalVectorsWidgetAction == NULL) {
        m_surfacePropertiesDisplayNormalVectorsWidgetAction = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::CHECK_BOX_BOOLEAN,
                                                                WbMacroWidgetActionNames::getSurfacePropertiesDisplayNormalVectorsName(),
                                                                "Display normal vectors on a surface",
                                                                this);
        
        DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
        
        QObject::connect(m_surfacePropertiesDisplayNormalVectorsWidgetAction, &WuQMacroWidgetAction::getModelValue,
                         this, [=](QVariant& value) {
                             value = dsp->isDisplayNormalVectors();
                         });
        
        QObject::connect(m_surfacePropertiesDisplayNormalVectorsWidgetAction, &WuQMacroWidgetAction::setModelValue,
                         this, [=](const QVariant& value) {
                             dsp->setDisplayNormalVectors(value.toBool());
                             GuiManager::updateGraphicsAllWindows();
                         });
    }
    
    return m_surfacePropertiesDisplayNormalVectorsWidgetAction;
}

/**
 * @return Get (and in needed create) the surface properties surface drawing type widget action
 */
WuQMacroWidgetAction*
WbMacroWidgetActionsManager::getSurfacePropertiesSurfaceDrawingTypeWidgetAction()
{
    if (m_surfacePropertiesSurfaceDrawingTypeWidgetAction == NULL) {
        m_surfacePropertiesSurfaceDrawingTypeWidgetAction = new WuQMacroWidgetAction(WuQMacroWidgetAction::WidgetType::COMBO_BOX_STRING_LIST,
                                                                                       WbMacroWidgetActionNames::getSurfacePropertiesDrawingTypeName(),
                                                                                       "Drawing type for surface",
                                                                                       this);
        
        std::vector<AString> drawTypeNames;
        const bool sortNamesFlag(false);
        SurfaceDrawingTypeEnum::getAllGuiNames(drawTypeNames,
                                               sortNamesFlag);
        std::vector<QString> qsDrawTypeNames(drawTypeNames.begin(),
                                             drawTypeNames.end());
        m_surfacePropertiesSurfaceDrawingTypeWidgetAction->setComboBoxStringList(qsDrawTypeNames);
        
        DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
        
        QObject::connect(m_surfacePropertiesSurfaceDrawingTypeWidgetAction, &WuQMacroWidgetAction::getModelValue,
                         this, [=](QVariant& value) {
                             const SurfaceDrawingTypeEnum::Enum sdt = dsp->getSurfaceDrawingType();
                             value = SurfaceDrawingTypeEnum::toGuiName(sdt);
                         });

        QObject::connect(m_surfacePropertiesSurfaceDrawingTypeWidgetAction, &WuQMacroWidgetAction::setModelValue,
                         this, [=](const QVariant& value) {
                             bool validFlag(false);
                             const SurfaceDrawingTypeEnum::Enum std = SurfaceDrawingTypeEnum::fromGuiName(value.toString(), &validFlag);
                             if (validFlag) {
                                 dsp->setSurfaceDrawingType(std);
                             }
                             else {
                                 CaretLogSevere("Failed to convert to surface drawing type: "
                                                + value.toString());
                             }
                             GuiManager::updateGraphicsAllWindows();
                         });
    }
    
    return m_surfacePropertiesSurfaceDrawingTypeWidgetAction;
}

