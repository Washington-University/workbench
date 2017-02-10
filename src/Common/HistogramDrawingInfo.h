#ifndef __HISTOGRAM_DRAWING_INFO_H__
#define __HISTOGRAM_DRAWING_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

    class HistogramDrawingInfo : public CaretObject {
        
    public:
        HistogramDrawingInfo();
        
        virtual ~HistogramDrawingInfo();

        void initialize(const int32_t numberOfDataPoints);
        
        void reset();
        
        bool getBounds(float boundsOut[4]) const;
        
        int32_t getNumberOfDataPoints() const;
        
        bool isValid() const;
        
        /** @return X-coordinates of histogram bars */
        const std::vector<float>& getDataX() const { return m_dataX; }

        /** @return Y-coordinates of histogram bars */
        const std::vector<float>& getDataY() const { return m_dataY; }
        
        /** @return RGBA data for histogram bars */
        const std::vector<float>& getDataRGBA() const { return m_dataRGBA; }
        
        /** @return XYZ coordinates for threshold one bounds, may be empty ! */
        const std::vector<float>& getThresholdOneBounds() const { return m_thresholdOneBoundsXYZ; }
        
        /** @return XYZ coordinates for threshold two bounds, may be empty ! */
        const std::vector<float>& getThresholdTwoBounds() const { return m_thresholdTwoBoundsXYZ; }
        
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        HistogramDrawingInfo(const HistogramDrawingInfo& obj);

        HistogramDrawingInfo& operator=(const HistogramDrawingInfo&);
        
        
        mutable float m_bounds[4];
        
        mutable bool m_boundsValid = false;
        
        std::vector<float> m_dataX;
        
        std::vector<float> m_dataY;
        
        std::vector<float> m_dataRGBA;
        
        std::vector<float> m_thresholdOneBoundsXYZ;
        
        bool m_thresholdOneBoundsValid = false;
        
        std::vector<float> m_thresholdTwoBoundsXYZ;
        
        bool m_thresholdTwoBoundsValid = false;
        
        // ADD_NEW_MEMBERS_HERE

        friend class CaretMappableDataFile;
    };
    
#ifdef __HISTOGRAM_DRAWING_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOGRAM_DRAWING_INFO_DECLARE__

} // namespace
#endif  //__HISTOGRAM_DRAWING_INFO_H__
