#ifndef __WU_Q_SCROLL_AREA_H__
#define __WU_Q_SCROLL_AREA_H__

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

#include <QScrollArea>



namespace caret {

    class WuQScrollArea : public QScrollArea {
        
        Q_OBJECT

    public:
        static WuQScrollArea* newInstance(const int32_t fixedWidth,
                                          const int32_t fixedHeight);
        
        virtual ~WuQScrollArea();
        
        WuQScrollArea(const WuQScrollArea&) = delete;

        WuQScrollArea& operator=(const WuQScrollArea&) = delete;

        virtual QSize sizeHint() const override;

        // ADD_NEW_METHODS_HERE

    private:
        WuQScrollArea(const int32_t fixedWidth,
                      const int32_t fixedHeight);
        
        const int32_t m_fixedWidth;
        
        const int32_t m_fixedHeight;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_SCROLL_AREA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_SCROLL_AREA_DECLARE__

} // namespace
#endif  //__WU_Q_SCROLL_AREA_H__
