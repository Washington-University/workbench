
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

#define __INFORMATION_DISPLAY_DIALOG_DECLARE__
#include "InformationDisplayDialog.h"
#undef __INFORMATION_DISPLAY_DIALOG_DECLARE__

#include "BrainBrowserWindow.h"
#include "InformationDisplayWidget.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"

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
    
    m_informationWidget = new InformationDisplayWidget(this);
    this->setCentralWidget(m_informationWidget,
                           WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
InformationDisplayDialog::~InformationDisplayDialog()
{
    
}

/**
 * Update the dialog's content.
 */
void 
InformationDisplayDialog::updateDialog()
{
    
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
    sceneClass->addClass(m_informationWidget->saveToScene(sceneAttributes,
                                                          "m_informationWidget"));
    
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
        m_informationWidget->restoreFromScene(sceneAttributes,
                                              NULL);
        return;
    }
    
    m_informationWidget->restoreFromScene(sceneAttributes,
                                          sceneClass->getClass("m_informationWidget"));

    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    swg.restoreFromScene(sceneAttributes, sceneClass->getClass("geometry"));    
}

