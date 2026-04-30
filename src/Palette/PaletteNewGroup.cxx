
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PALETTE_NEW_GROUP_DECLARE__
#include "PaletteNewGroup.h"
#undef __PALETTE_NEW_GROUP_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::PaletteNewGroup 
 * \brief Contains a group of palettes
 * \ingroup Palette
 */

/**
 * Constructor creates an empty, editable palette group.
 *
 * @param groupType
 * Type of group containing palettes
 */
PaletteNewGroup::PaletteNewGroup(const GroupType groupType)
: CaretObject(),
m_groupType(groupType)
{
    switch (groupType) {
        case GroupType::STANDARD:
            m_groupName = "Standard";
            m_editableFlag = false;
            break;
        case GroupType::USER_CUSTOM:
            m_groupName = "User Custom";
            m_editableFlag = true;
            break;
    }
}

/**
 * Destructor.
 */
PaletteNewGroup::~PaletteNewGroup()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PaletteNewGroup::toString() const
{
    return "PaletteNewGroup";
}

/**
 * @return The group type
 */
PaletteNewGroup::GroupType
PaletteNewGroup::getGroupType() const
{
    return m_groupType;
}

/**
 * @return Name of group
 */
AString
PaletteNewGroup::getGroupName() const
{
    return m_groupName;
}

/**
 * @return True if this container is editable
 */
bool
PaletteNewGroup::isEditable() const
{
    return m_editableFlag;
}

/**
 * Set the editable status of this container.  Subclasses that are not editable
 * use this method to disable the editable status.
 *
 * @param status
 *  New editable status
 */
void
PaletteNewGroup::setEditable(const bool status)
{
    m_editableFlag = status;
}

/**
 * Add some example palettes
 */
void
PaletteNewGroup::addExamplePalettes()
{
    {
        std::vector<PaletteNew::ScalarColor> posRange;
        posRange.push_back(PaletteNew::ScalarColor(0.0, 0.4, 0.0, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(0.3, 0.7, 0.0, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(1.0, 0.1, 0.0, 0.0));
        
        std::vector<PaletteNew::ScalarColor> negRange;
        negRange.push_back(PaletteNew::ScalarColor(-1.0, 0.0, 0.4, 0.9));
        negRange.push_back(PaletteNew::ScalarColor(-0.6, 0.7, 0.0, 0.7));
        negRange.push_back(PaletteNew::ScalarColor( 0.0, 0.1, 0.0, 0.3));
        
        float zeroRGB[3] = { 0.0, 1.0, 0.0 };
        PaletteNew* p = new PaletteNew(posRange, zeroRGB, negRange);
        p->setName("Psycho");
        FunctionResult result(addPalette(p));
        if (result.isError()) {
            delete p;
        }
    }

    {
        std::vector<PaletteNew::ScalarColor> posRange;
        posRange.push_back(PaletteNew::ScalarColor(0.0, 0.1, 0.0, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(0.5, 0.7, 0.7, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(1.0, 0.4, 0.7, 0.0));
        
        std::vector<PaletteNew::ScalarColor> negRange;
        negRange.push_back(PaletteNew::ScalarColor(-1.0, 0.0, 0.5, 0.9));
        negRange.push_back(PaletteNew::ScalarColor(-0.2, 0.7, 0.0, 0.7));
        negRange.push_back(PaletteNew::ScalarColor( 0.0, 0.1, 0.0, 0.3));
        
        float zeroRGB[3] = { 0.5, 1.0, 0.0 };
        PaletteNew* p = new PaletteNew(posRange, zeroRGB, negRange);
        p->setName("Roy");
        FunctionResult result(addPalette(p));
        if (result.isError()) {
            delete p;
        }
    }
}

/**
 * Add a new palette to this group with the given name and number of control points
 * @param name
 *    Name for new palette
 * @param numberOfPositiveControlPoints
 *    Number of positive control points for new palette
 * @param numberOfNegativeControlPoints
 *    Number of negative control points for new palette
 */
FunctionResultValue<PaletteNew*>
PaletteNewGroup::addNewPalette(const AString& name,
                               const int32_t numberOfPositiveControlPoints,
                               const int32_t numberOfNegativeControlPoints)
{
    AString errorMessage;
    if (name.isEmpty()) {
        errorMessage.appendWithNewLine("New palette name is empty");
    }
    else if (hasPaletteWithName(name)) {
        errorMessage.appendWithNewLine("A palette with name \""
                                       + name
                                       + "\" already exists");
    }
    if (numberOfPositiveControlPoints < 2) {
        errorMessage.appendWithNewLine("There must be at least two positive control points");
    }
    if (numberOfNegativeControlPoints < 2) {
        errorMessage.appendWithNewLine("There must be at least two negative control points");
    }
    
    PaletteNew* palette(NULL);
    if (errorMessage.isEmpty()) {
        const int32_t numPos(numberOfPositiveControlPoints);
        const int32_t numNeg(numberOfNegativeControlPoints);
        
        const float posStep(1.0 / numPos);
        
        const float redStep(1.0 / (numPos + 3));
        float redCompontent(redStep * 2.0);
        
        std::vector<PaletteNew::ScalarColor> posRange;
        float posScalar(0.0);
        for (int32_t i = 0; i < numPos; i++) {
            if (i == (numPos - 1)) {
                posScalar = 1.0f;
                redCompontent = 1.0f;
            }
            posRange.emplace_back(posScalar, redCompontent, 0.0f, 0.0f);
            
            posScalar     += posStep;
            redCompontent += redStep;
        }
        
        float negStep(1.0 / numNeg);
        
        float blueStep(1.0 / (numNeg + 3));
        float blueComponent(1.0);
        
        std::vector<PaletteNew::ScalarColor> negRange;
        float negScalar(-1.0);
        for (int32_t i = 0; i < numNeg; i++) {
            if (i == (numNeg - 1)) {
                negScalar = 0.0f;
            }
            negRange.emplace_back(negScalar, 0.0, 0.0f, blueComponent);
            
            blueComponent -= blueStep;
            negScalar     += negStep;
        }
        
        float zeroGreen[3] { 0.0, 1.0, 0.0 };
        palette = new PaletteNew(posRange,
                                 zeroGreen,
                                 negRange);
        palette->setName(name);
    }

    return FunctionResultValue<PaletteNew*>(palette,
                                            errorMessage,
                                            errorMessage.isEmpty());
}


/**
 * Add a palette.  Palette must contain a name not used by an existing palette in this group.
 * @param palette
 *     Palette to add.  Ownership will be taken of this pointer
 * @return Function result with success/failure.
 * If success, this group will take possession of pointer and destory it when appropriate.
 * If failure, caller is responsible for destroying the pointer.
 */
FunctionResult
PaletteNewGroup::addPalette(PaletteNew* palette)
{
    if ( ! m_editableFlag) {
        return FunctionResult::error("Adding palettes to " + m_groupName + " group not allowed");
    }
    
    const AString paletteName = palette->getName();
    if (paletteName.trimmed().isEmpty()) {
        return FunctionResult::error("Palette for adding  must have a name");
    }
    
    if (hasPaletteWithName(palette->getName())) {
        return FunctionResult::error("Palette with name \"" + palette->getName() + "\" already exists");
    }
    
    std::unique_ptr<PaletteNew> ptr(palette);
    const auto iterAndBoolStatus(m_palettes.insert(std::move(ptr)));
    if ( ! iterAndBoolStatus.second) {
        CaretLogWarning("Failed to add PaletteNew ="
                        + palette->getName()
                        + " to PaletteNewGroup");
    }
    
    return FunctionResult::ok();
}

/**
 * Get the palettes in this group
 * @palettesOut
 *   Contains palettes (if any) on exit
 */
void
PaletteNewGroup::getPalettes(std::vector<PaletteNew*>& palettesOut) const
{
    palettesOut.clear();
    for (auto& p : m_palettes) {
        palettesOut.push_back(p.get());
    }
    
    std::sort(palettesOut.begin(),
              palettesOut.end(),
              PaletteNewPointerNameSort);
}

/**
 * Get the palettes in this group
 * @palettesOut
 *   Contains palettes (if any) on exit
 */
void
PaletteNewGroup::getPalettes(std::vector<const PaletteNew*>& palettesOut) const
{
    palettesOut.clear();
    for (auto& p : m_palettes) {
        palettesOut.push_back(p.get());
    }
    
    std::sort(palettesOut.begin(),
              palettesOut.end(),
              PaletteNewPointerNameSort);
}

/**
 * Get the palette with the given name
 * @param paletteName
 *    Name of palette
 * @return
 *    Function result with success/failure
 */
FunctionResultValue<PaletteNew*>
PaletteNewGroup::getPaletteWithName(const AString& paletteName) const
{
    PaletteNew* palette(NULL);
    for (auto& p : m_palettes) {
        if (p->getName() == paletteName) {
            palette = p.get();
            CaretAssert(palette);
            break;
        }
    }

    AString errorMessage;
    if (palette == NULL) {
        errorMessage = ("Palette with name \""
                        + paletteName
                        + "\" was not found.");
    }
    
    return FunctionResultValue<PaletteNew*>(palette,
                                            errorMessage,
                                            errorMessage.isEmpty());
}

/**
 * @return True if a palette with the given name exists in this group, else false.
 * @param paletteName
 *    Name of palette
 */
bool
PaletteNewGroup::hasPaletteWithName(const AString& paletteName)
{
    for (auto& p : m_palettes) {
        if (p->getName() == paletteName) {
            return true;
        }
    }
    return false;
}


/**
 * Rename a palette.  Palette must contain a name that matches name of an existing palette
 * @param palette
 *     Palette that is renamed
 * @param newPaletteName
 *     New name for palette
 * @return FunctionResult with success/failure
 */
FunctionResult
PaletteNewGroup::renamePalette(const PaletteNew* palette,
                               const AString& newPaletteName)
{
    if (newPaletteName.trimmed().isEmpty()) {
        return FunctionResult::error("New palette name is empty");
    }
    
    if (hasPaletteWithName(newPaletteName)) {
        return FunctionResult::error("Palette with name \"" + newPaletteName + "\" already exists");
    }

    for (auto& p : m_palettes) {
        if (p.get() == palette) {
            p->setName(newPaletteName);
            return FunctionResult::ok();
        }
    }
    
    return FunctionResult::error("Palette with name \""
                                 + palette->getName()
                                 + "\" was not found for renaming to \""
                                 + newPaletteName
                                 + "\"");
}

/**
 * Remove a palette.
 * @param palette
 *     Palette that is removed
 * @return FunctionResult with success/failure
 */
FunctionResult
PaletteNewGroup::removePalette(const PaletteNew* palette)
{
    if ( ! m_editableFlag) {
        return FunctionResult::error("Removing palettes in " + m_groupName + " group not allowed");
    }

    for (auto iter = m_palettes.begin();
         iter != m_palettes.end();
         iter++) {
        if (iter->get() == palette) {
            m_palettes.erase(iter);
            return FunctionResult::ok();
        }
    }
    
    return FunctionResult::error("Palette with name \""
                                 + palette->getName()
                                 + "\" was not found for removal");
}

/**
 * Update a palette
 * @param palette
 *    Palette that is updated
 * @param positiveMapping
 *    Updated positive mapping
 * @param negativeMapping
 *    Updated negative mapping
 * @param zeroMapping
 *    Updated zero mapping
 */
FunctionResult
PaletteNewGroup::updatePalette(const PaletteNew* palette,
                               const std::vector<PaletteNew::ScalarColor>& positiveMapping,
                               const std::vector<PaletteNew::ScalarColor>& negativeMapping,
                               const PaletteNew::ScalarColor& zeroMapping)
{
    if ( ! m_editableFlag) {
        return FunctionResult::error("Updating palette in " + m_groupName + " group not allowed");
    }
    
    for (auto iter = m_palettes.begin();
         iter != m_palettes.end();
         iter++) {
        if (iter->get() == palette) {
            PaletteNew* palette(iter->get());
            CaretAssert(palette);
            palette->updateRanges(positiveMapping, zeroMapping.color, negativeMapping);
            return FunctionResult::ok();
        }
    }
    
    return FunctionResult::error("Attempting to update mappings for a palette that does not exist.");
}
