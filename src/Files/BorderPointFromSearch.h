#ifndef __BORDER_POINT_FROM_SEARCH_H__
#define __BORDER_POINT_FROM_SEARCH_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretAssert.h"
#include "CaretObject.h"


/**
 * \class caret::BorderPointFromSearch
 * \brief Contains result of searching for a border point.
 * \ingroup Files
 */

namespace caret {

    class BorderPointFromSearch : public CaretObject {
        
    public:
        /**
         * Constructor.
         */
        BorderPointFromSearch() {
            reset();
        }
        
        /**
         * Copy constructor.
         * 
         * @param rhs
         *    Instance copied to this instance.
         */
        BorderPointFromSearch(const BorderPointFromSearch& rhs)
        : CaretObject(rhs) {
            this->copyHelperBorderPointFromSearch(rhs);
        }
        
        /**
         * Destructor.
         */
        virtual ~BorderPointFromSearch() { }
        
        /**
         * Copy constructor.
         *
         * @param rhs
         *    Instance copied to this instance.
         * @return 
         *    Reference to this instance.
         */
        BorderPointFromSearch& operator=(const BorderPointFromSearch& rhs) {
            if (this != &rhs) {
                CaretObject::operator=(rhs);
                this->copyHelperBorderPointFromSearch(rhs);
            }
            return *this;
        }
        
        /**
         * Comparison operator using distance.
         * 
         * @param rhs
         *    Compared to rhs
         * @param
         *    True if 'this' is less than 'rhs'.
         */
        bool operator<(const BorderPointFromSearch& rhs) const {
            if (isValid()
                && rhs.isValid()) {
                return (m_distance < rhs.m_distance);
            }
            else if (isValid()) {
                return true;
            }
            return false;
        }
        
        /*
         * Reset the data to invalid.
         */
        void reset() {
            m_borderFile  = NULL;
            m_border      = NULL;
            m_borderIndex = -1;
            m_borderPointIndex = -1;
            m_distance    = -1.0;
        }
        
        /**
         * Replace this instance with the given instance if the given
         * instance's distance is less than this instance's distance
         * or if this instance is invalid.
         *
         * @param rhs
         *    The other instance.
         */
        void replaceWithNearerDistance(const BorderPointFromSearch& rhs) {
            if ( ! rhs.isValid()) {
                return;
            }
            
            bool replaceMeFlag = false;
            
            if (isValid()) {
                if (rhs.m_distance < m_distance) {
                    replaceMeFlag = true;
                }
            }
            else {
                replaceMeFlag = true;
            }
            
            if (replaceMeFlag) {
                copyHelperBorderPointFromSearch(rhs);
            }
        }
        
        /**
         * Set the data.
         * 
         * @param borderFile
         *    The border file.
         * @param border
         *    The border
         * @param borderIndex
         *    Index of border in the border file
         * @param borderPointIndex
         *    Index of point in the border
         * @param distance
         *    Distance of border point from the search point.
         */
        void setData(BorderFile* borderFile,
                     Border* border,
                     const int32_t borderIndex,
                     const int32_t borderPointIndex,
                     const float distance) {
            m_borderFile = borderFile;
            m_border = border;
            m_borderIndex = borderIndex;
            m_borderPointIndex = borderPointIndex;
            m_distance = distance;
            
            CaretAssert(m_borderFile);
            CaretAssert(m_border);
            CaretAssert(m_borderIndex >= 0);
            CaretAssert(m_borderPointIndex >= 0);
            CaretAssert(m_distance >= 0.0);
        }
        
        /** @return Is this item valid */
        inline bool isValid() const {
            return ((m_borderFile != NULL)
                    && (m_border != NULL)
                    && (m_borderIndex >= 0)
                    && (m_borderPointIndex >= 0)
                    && (m_distance >= 0.0));
        }
        
        /** @return Border file containing the point */
        inline BorderFile* borderFile() { return m_borderFile; }

        /** @return The border */
        inline Border* border() { return m_border; }
        
        /** @return Index of the border in the border file */
        inline int32_t borderIndex() { return m_borderIndex; }
        
        /** @return Index of the border point */
        inline int32_t borderPointIndex() { return m_borderPointIndex; }
        
        /** @return Distance to the border point from the search point */
        inline float distance() { return m_distance; }
        
    private:
        /**
         * Helps with copying an object of this type.
         * @param rhs
         *    Object that is copied.
         */
        void copyHelperBorderPointFromSearch(const BorderPointFromSearch& rhs) {
            m_borderFile       = rhs.m_borderFile;
            m_border           = rhs.m_border;
            m_borderIndex      = rhs.m_borderIndex;
            m_borderPointIndex = rhs.m_borderPointIndex;
            m_distance         = rhs.m_distance;
        }
        
        BorderFile* m_borderFile;
        
        Border* m_border;
        
        int32_t m_borderIndex;

        int32_t m_borderPointIndex;
        
        float m_distance;

    };
    
#ifdef __BORDER_POINT_FROM_SEARCH_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_POINT_FROM_SEARCH_DECLARE__

} // namespace
#endif  //__BORDER_POINT_FROM_SEARCH_H__
