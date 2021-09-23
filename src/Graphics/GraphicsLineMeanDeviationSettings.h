#ifndef __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_H__
#define __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include "CaretObject.h"



namespace caret {

    class CaretMappableDataFile;
    class ChartTwoDataCartesian;
    
    class GraphicsLineMeanDeviationSettings : public CaretObject {
        
    public:
        GraphicsLineMeanDeviationSettings();
        
        virtual ~GraphicsLineMeanDeviationSettings();
        
        GraphicsLineMeanDeviationSettings(const GraphicsLineMeanDeviationSettings& obj);

        GraphicsLineMeanDeviationSettings& operator=(const GraphicsLineMeanDeviationSettings& obj);
        
        bool operator==(const GraphicsLineMeanDeviationSettings& obj) const;
        
        bool anyModificationEnabled() const;
        
        void reset();

        void updateFileSettings(CaretMappableDataFile* mapFile,
                                int32_t mapIndex,
                                int32_t numberOfMaps,
                                ChartTwoDataCartesian* cartesianLineData,
                                int32_t numberOfCartesianVertices);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        CaretMappableDataFile* m_mapFile = NULL;
        int32_t m_mapIndex = -1;
        int32_t m_numberOfMaps = -1;
        ChartTwoDataCartesian* m_cartesianLineData = NULL;
        int32_t m_numberOfCartesianVertices = -1;
        
        float m_newMeanValue = 0.0;
        float m_newDeviationValue = 1.0;
        bool  m_newMeanEnabled = false;
        bool  m_newDeviationEnabled = false;
        bool  m_absoluteValueEnabled = false;
        bool  m_dataOffsetEnabled = false;
        float m_dataOffsetValue = 0.0;
        bool  m_multiplyDeviationEnabled = false;
        float m_multiplyDeviationValue = 1.0;
        
    private:
        void copyHelperGraphicsLineMeanDeviationSettings(const GraphicsLineMeanDeviationSettings& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_DECLARE__

} // namespace
#endif  //__GRAPHICS_LINE_MEAN_DEVIATION_SETTINGS_H__
