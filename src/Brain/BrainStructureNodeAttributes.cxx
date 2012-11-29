
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

#include <algorithm>

#define __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__
#include "BrainStructureNodeAttributes.h"
#undef __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::BrainStructureNodeAttributes 
 * \brief Contains attributes for all node in a brain structure.
 *
 * Contains attributes for all nodes in a brain structure.
 * If the number of nodes in the brain structure changes,
 * this class' update() method must be called.
 */

/**
 * Constructor.
 */
BrainStructureNodeAttributes::BrainStructureNodeAttributes()
: CaretObject()
{
    this->update(0);
}

/**
 * Destructor.
 */
BrainStructureNodeAttributes::~BrainStructureNodeAttributes()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainStructureNodeAttributes::toString() const
{
    return ("BrainStructureNodeAttributes");
}

void 
BrainStructureNodeAttributes::update(const int32_t numberOfNodes)
{
//    if (numberOfNodes > 0) {
//        m_identificationType.resize(numberOfNodes);
//        this->setAllIdentificationNone();
//    }
//    else {
//        m_identificationType.clear();
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
BrainStructureNodeAttributes::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainStructureNodeAttributes",
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
BrainStructureNodeAttributes::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }    
}

