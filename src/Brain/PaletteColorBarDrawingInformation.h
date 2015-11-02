#ifndef __PALETTE_COLOR_BAR_DRAWING_INFORMATION_H__
#define __PALETTE_COLOR_BAR_DRAWING_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "CaretObject.h"



namespace caret {

    class AnnotationColorBar;
    class FastStatistics;
    class PaletteColorMapping;
    
    class PaletteColorBarDrawingInformation : public CaretObject {
        
    public:
        PaletteColorBarDrawingInformation(const AnnotationColorBar*  annotationColorBar,
                                          const FastStatistics*      statistics,
                                          const PaletteColorMapping* paletteColorMapping);
        
        virtual ~PaletteColorBarDrawingInformation();
        
        bool isValid() const;
        
        const AnnotationColorBar*  getAnnotationColorBar() const;
        
        const FastStatistics*      getStatistics() const;
        
        const PaletteColorMapping* getPaletteColorMapping() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        PaletteColorBarDrawingInformation(const PaletteColorBarDrawingInformation& obj);
        
        PaletteColorBarDrawingInformation& operator=(const PaletteColorBarDrawingInformation& obj);
        
        
        const AnnotationColorBar*  m_annotationColorBar;
        
        const FastStatistics*      m_statistics;
        
        const PaletteColorMapping* m_paletteColorMapping;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_COLOR_BAR_DRAWING_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_COLOR_BAR_DRAWING_INFORMATION_DECLARE__

} // namespace
#endif  //__PALETTE_COLOR_BAR_DRAWING_INFORMATION_H__
