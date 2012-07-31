
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __INFORMATION_DISPLAY_DIALOG_DECLARE__
#include "InformationDisplayDialog.h"
#undef __INFORMATION_DISPLAY_DIALOG_DECLARE__

#include "BrainBrowserWindow.h"
#include "InformationDisplayWidget.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::InformationDisplayDialog 
 * \brief Dialog for display of information.
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
    this->setCentralWidget(m_informationWidget);
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
}

