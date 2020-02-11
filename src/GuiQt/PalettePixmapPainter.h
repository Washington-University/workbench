#ifndef __PALETTE_PIXMAP_PAINTER_H__
#define __PALETTE_PIXMAP_PAINTER_H__

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



#include <memory>

#include <QPixmap>

#include "CaretObject.h"



namespace caret {

    class Palette;
    
    class PalettePixmapPainter : public CaretObject {
        
    public:
        enum Mode {
            INTERPOLATE_OFF,
            INTERPOLATE_ON
        };
        
        PalettePixmapPainter(const Palette* palette);
        
        virtual ~PalettePixmapPainter();
        
        PalettePixmapPainter(const PalettePixmapPainter&) = delete;

        PalettePixmapPainter& operator=(const PalettePixmapPainter&) = delete;

        QPixmap getPixmap() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void createPixmapInterpolateOff(const Palette* palette,
                                        const qreal pixmapWidth,
                                        const qreal pixmapHeight);
        
        void createPixmapInterpolateOn(const Palette* palette,
                                       const qreal pixmapWidth,
                                       const qreal pixmapHeight);
        
        float nonInterpScalarToPixMap(const float pixmapWidth,
                                      const float scalarIn);
        
        Mode m_mode = Mode::INTERPOLATE_ON;
        
        QPixmap m_pixmap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_PIXMAP_PAINTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_PIXMAP_PAINTER_DECLARE__

} // namespace
#endif  //__PALETTE_PIXMAP_PAINTER_H__
