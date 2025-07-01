#ifndef __VIEWING_TRANSFORMATIONS_HISTOLOGY_H__
#define __VIEWING_TRANSFORMATIONS_HISTOLOGY_H__

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


#include "ViewingTransformations.h"



namespace caret {
    
    class BrowserTabContent;
    class GraphicsRegionSelectionBox;
    class HistologySlice;
    class MouseEvent;
    
    class ViewingTransformationsHistology : public ViewingTransformations {
        
    public:
        ViewingTransformationsHistology();
        
        virtual ~ViewingTransformationsHistology();
        
        ViewingTransformationsHistology(const ViewingTransformationsHistology& obj);

        ViewingTransformationsHistology& operator=(const ViewingTransformationsHistology& obj);
        
        virtual void copyTransformsForYoking(const ViewingTransformationsHistology& otherTransformation,
                                             const float otherImageWidthHeight[2],
                                             const float myImageWidthHeight[2]);
        
        virtual bool setHistologyViewToBounds(const MouseEvent* mouseEvent,
                                              const GraphicsRegionSelectionBox* selectionRegion,
                                              const GraphicsObjectToWindowTransform* transform,
                                              const HistologySlice* histologySlice);
        virtual void resetView();
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperViewingTransformationsHistology(const ViewingTransformationsHistology& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VIEWING_TRANSFORMATIONS_HISTOLOGY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VIEWING_TRANSFORMATIONS_HISTOLOGY_DECLARE__

} // namespace
#endif  //__VIEWING_TRANSFORMATIONS_HISTOLOGY_H__
