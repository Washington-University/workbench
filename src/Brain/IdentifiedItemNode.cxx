
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

#define __IDENTIFIED_ITEM_NODE_DECLARE__
#include "IdentifiedItemNode.h"
#undef __IDENTIFIED_ITEM_NODE_DECLARE__

#include "SceneAttributes.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItem
 * \brief Describes an identified item.
 */

/**
 * Constructor.
 *
 * @param text
 *    Text describing the identified item.
 * @param structure
 *    Structure on which identification took place.
 * @param contralateralStructure
 *    Contralateral of identification structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface on which identification took place.
 * @param nodeIndex
 *    Index of node that was identified.
 *
 */
IdentifiedItemNode::IdentifiedItemNode()
: IdentifiedItem()
{
    m_structure = StructureEnum::INVALID;
    m_contralateralStructure = StructureEnum::INVALID;
    m_surfaceNumberOfNodes = -1,
    m_nodeIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<StructureEnum>("m_structure", &m_structure);
    m_sceneAssistant->add<StructureEnum>("m_contralateralStructure", &m_contralateralStructure);
    m_sceneAssistant->add("m_surfaceNumberOfNodes", &m_surfaceNumberOfNodes);
    m_sceneAssistant->add("m_nodeIndex", &m_nodeIndex);
    m_sceneAssistant->addArray("m_symbolRGB", m_symbolRGB, 3, 0);
    m_sceneAssistant->addArray("m_contralateralSymbolRGB", m_contralateralSymbolRGB, 3, 0);
    m_sceneAssistant->add("m_symbolSize", &m_symbolSize);
}

/**
 * Constructor.
 *
 * @param text
 *    Text describing the identified item.
 * @param structure
 *    Structure on which identification took place.
 * @param contralateralStructure
 *    Contralateral of identification structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface on which identification took place.
 * @param nodeIndex
 *    Index of node that was identified.
 *
 */
IdentifiedItemNode::IdentifiedItemNode(const AString& text,
                                       const StructureEnum::Enum structure,
                                       const StructureEnum::Enum contralateralStructure,
                                       const int32_t surfaceNumberOfNodes,
                                       const int32_t nodeIndex)
: IdentifiedItem(text)
{
    m_structure = structure;
    m_contralateralStructure = contralateralStructure;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes,
    m_nodeIndex = nodeIndex;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<StructureEnum>("m_structure", &m_structure);
    m_sceneAssistant->add<StructureEnum>("m_contralateralStructure", &m_contralateralStructure);
    m_sceneAssistant->add("m_surfaceNumberOfNodes", &m_surfaceNumberOfNodes);
    m_sceneAssistant->add("m_nodeIndex", &m_nodeIndex);
    m_sceneAssistant->addArray("m_symbolRGB", m_symbolRGB, 3, 0);
    m_sceneAssistant->addArray("m_contralateralSymbolRGB", m_contralateralSymbolRGB, 3, 0);
    m_sceneAssistant->add("m_symbolSize", &m_symbolSize);
}

/**
 * Destructor.
 */
IdentifiedItemNode::~IdentifiedItemNode()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItemNode::IdentifiedItemNode(const IdentifiedItemNode& obj)
: IdentifiedItem(obj)
{
    this->copyHelperIdentifiedItemNode(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItemNode&
IdentifiedItemNode::operator=(const IdentifiedItemNode& obj)
{
    if (this != &obj) {
        IdentifiedItem::operator=(obj);
        this->copyHelperIdentifiedItemNode(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItemNode::copyHelperIdentifiedItemNode(const IdentifiedItemNode& obj)
{
    m_structure = obj.m_structure;
    m_contralateralStructure = obj.m_contralateralStructure;
    m_surfaceNumberOfNodes = obj.m_surfaceNumberOfNodes;
    m_nodeIndex = obj.m_nodeIndex;
    
    m_symbolRGB[0] = obj.m_symbolRGB[0];
    m_symbolRGB[1] = obj.m_symbolRGB[1];
    m_symbolRGB[2] = obj.m_symbolRGB[2];
    
    m_contralateralSymbolRGB[0] = obj.m_contralateralSymbolRGB[0];
    m_contralateralSymbolRGB[1] = obj.m_contralateralSymbolRGB[1];
    m_contralateralSymbolRGB[2] = obj.m_contralateralSymbolRGB[2];
    
    m_symbolSize = obj.m_symbolSize;
}

/**
 * @return The structure for the identified node.
 */
StructureEnum::Enum
IdentifiedItemNode::getStructure() const
{
    return m_structure;
}

/**
 * @return The contralateral structure of the identified node.
 */
StructureEnum::Enum
IdentifiedItemNode::getContralateralStructure() const
{
    return m_contralateralStructure;
}

/**
 * @return The number of nodes in the surface on which identification took place.
 */
int32_t
IdentifiedItemNode::getSurfaceNumberOfNodes() const
{
    return m_surfaceNumberOfNodes;
}

/**
 * @return The index of the surface node that was identified.
 */
int32_t
IdentifiedItemNode::getNodeIndex() const
{
    return m_nodeIndex;
}

/**
 * @return The color for the symbol's identification symbol.
 */
const float*
IdentifiedItemNode::getSymbolRGB() const
{
    return m_symbolRGB;
}

/**
 * @return The color for the symbol's identification symbol on the
 * contralateral surface.
 */
const float*
IdentifiedItemNode::getContralateralSymbolRGB() const
{
    return m_contralateralSymbolRGB;
}

/**
 * @return  The size of the symbol.
 */
const float
IdentifiedItemNode::getSymbolSize() const
{
    return m_symbolSize;
}

/**
 * Set the color for the identification symbol.
 *
 * @param rgb
 *   Red, green, blue color components for identification system.
 */
void
IdentifiedItemNode::setSymbolRGB(const float* rgb)
{
    m_symbolRGB[0] = rgb[0];
    m_symbolRGB[1] = rgb[1];
    m_symbolRGB[2] = rgb[2];
}

/**
 * Set the color for the contralateral identification symbol.
 *
 * @param rgb
 *   Red, green, blue color components for identification system.
 */
void
IdentifiedItemNode::setContralateralSymbolRGB(const float* rgb)
{
    m_contralateralSymbolRGB[0] = rgb[0];
    m_contralateralSymbolRGB[1] = rgb[1];
    m_contralateralSymbolRGB[2] = rgb[2];
}

/**
 * Set the size of the identification symbol.
 *
 * @param symbolSize
 *    Size of identification symbol.
 */
void
IdentifiedItemNode::setSymbolSize(const float symbolSize)
{
    m_symbolSize = symbolSize;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
IdentifiedItemNode::toString() const
{
    const AString s = (IdentifiedItem::toString()
                       + ", m_structure=" + StructureEnum::toName(m_structure)
                       + ", m_contralateralStructure=" + StructureEnum::toName(m_contralateralStructure)
                       + ", m_surfaceNumberOfNodes=" + AString::number(m_surfaceNumberOfNodes)
                       + ", m_nodeIndex=" + AString::number(m_nodeIndex));
    return s;
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
IdentifiedItemNode::saveToScene(const SceneAttributes* sceneAttributes,
                            const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItemNode",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, sceneClass);
    
    /*
     * Save data in parent class.
     */
    saveMembers(sceneAttributes,
                sceneClass);
    
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
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
IdentifiedItemNode::restoreFromScene(const SceneAttributes* sceneAttributes,
                                 const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Restores data in parent class.
     */
    restoreMembers(sceneAttributes,
                   sceneClass);
}

