#ifndef __IDENTIFICATION_TEXT_GENERATOR__H_
#define __IDENTIFICATION_TEXT_GENERATOR__H_

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


#include "CaretObject.h"

namespace caret {

    class Brain;
    class BrowserTabContent;
    class CaretMappableDataFile;
    class SelectionItemBorderSurface;
    class SelectionItemFocusSurface;
    class SelectionItemFocusVolume;
    class SelectionItemSurfaceNode;
    class SelectionItemVoxel;
    class SelectionManager;
    class IdentificationStringBuilder;
    
    class IdentificationTextGenerator : public CaretObject {
        
    public:
        IdentificationTextGenerator();
        
        virtual ~IdentificationTextGenerator();
        
        AString createIdentificationText(const SelectionManager* idManager,
                                         const Brain* brain) const;
        
    private:
        IdentificationTextGenerator(const IdentificationTextGenerator&);

        IdentificationTextGenerator& operator=(const IdentificationTextGenerator&);
        
    public:
        virtual AString toString() const;
        
    private:
        void generateSurfaceBorderIdentifcationText(IdentificationStringBuilder& idText,
                                                    const SelectionItemBorderSurface* idSurfaceBorder) const;
        
        void generateSurfaceFociIdentifcationText(IdentificationStringBuilder& idText,
                                                    const SelectionItemFocusSurface* idSurfaceFocus) const;
        
        void generateVolumeFociIdentifcationText(IdentificationStringBuilder& idText,
                                                  const SelectionItemFocusVolume* idVolumeFocus) const;
        
        void generateSurfaceIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemSurfaceNode* idSurfaceNode) const;
        
        void generateVolumeIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemVoxel* idVolumeVoxel) const;
        
        void getMapIndicesOfFileUsedInOverlays(const CaretMappableDataFile* caretMappableDataFile,
                                               std::vector<int32_t>& mapIndicesOut) const;
    };
    
#ifdef __IDENTIFICATION_TEXT_GENERATOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_TEXT_GENERATOR_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_TEXT_GENERATOR__H_
