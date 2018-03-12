
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

#define __IDENTIFIED_ITEM_NODE_DECLARE__
#include "IdentifiedItemNode.h"
#undef __IDENTIFIED_ITEM_NODE_DECLARE__

#include "CaretPreferences.h"
#include "SceneAttributes.h"
#include "SceneClassAssistant.h"
#include "SessionManager.h"

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
    initializeMembers();
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
                                       const int32_t surfaceNumberOfNodes,
                                       const int32_t nodeIndex)
: IdentifiedItem(text)
{
    initializeMembers();
    
    m_structure = structure;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes,
    m_nodeIndex = nodeIndex;
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
    initializeMembers();
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
 * Initialize members of this class.
 */
void
IdentifiedItemNode::initializeMembers()
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
 * @return Is this item valid?  Typically only used when restoring
 * from scene.
 */
bool
IdentifiedItemNode::isValid() const
{
    if (m_structure == StructureEnum::INVALID) {
        return false;
    }
    if (m_surfaceNumberOfNodes <= 0) {
        return false;
    }
    if (m_nodeIndex < 0) {
        return false;
    }
    
    return true;
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
 * Set the contralateral structure.
 * @param contralateralStructure
 *   The contralateral structure.
 */
void
IdentifiedItemNode::setContralateralStructure(const StructureEnum::Enum contralateralStructure)
{
    m_contralateralStructure = contralateralStructure;
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
 * Get color for the identification symbol.
 *
 * @param rgbaOut
 *    RGBA ranging 0 to 255.
 */
void
IdentifiedItemNode::getSymbolRGBA(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_symbolRGB[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_symbolRGB[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_symbolRGB[2] * 255.0);
    rgbaOut[3] = 255;
}

/**
 * Get color for the contralateral identification symbol.
 *
 * @param rgbaOut
 *    RGBA ranging 0 to 255.
 */
void
IdentifiedItemNode::getContralateralSymbolRGB(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_contralateralSymbolRGB[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_contralateralSymbolRGB[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_contralateralSymbolRGB[2] * 255.0);
    rgbaOut[3] = 255;
}

/**
 * @return  The size of the symbol.
 */
float
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
     * Save data in parent class.+
     
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

