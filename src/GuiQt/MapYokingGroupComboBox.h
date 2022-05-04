#ifndef __MAP_YOKING_GROUP_COMBO_BOX_H__
#define __MAP_YOKING_GROUP_COMBO_BOX_H__

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

#include "MapYokingGroupEnum.h"
#include "WuQWidget.h"

namespace caret {

    class AnnotationTextSubstitutionFile;
    class CaretMappableDataFile;
    class ChartTwoOverlay;
    class ChartableMatrixSeriesInterface;
    class EnumComboBoxTemplate;
    class MediaFile;
    class MediaOverlay;
    class Overlay;
    
    class MapYokingGroupComboBox : public WuQWidget {
        Q_OBJECT
        
    public:
        MapYokingGroupComboBox(QObject* parent);
        
        MapYokingGroupComboBox(QObject* parent,
                               const QString& objectName,
                               const QString& descriptiveName);
        
        virtual ~MapYokingGroupComboBox();
        
        virtual QWidget* getWidget();
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        void validateYokingChange(ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface,
                                  const int32_t tabIndex);
        
        void validateYokingChange(Overlay* overlay);
        
        void validateYokingChange(ChartTwoOverlay* chartOverlay);
        
        void validateYokingChange(MediaOverlay* mediaOverlay);
        
        void validateYokingChange(AnnotationTextSubstitutionFile* annTextSubFile);
        
        // ADD_NEW_METHODS_HERE

    signals:
        void itemActivated();

    private slots:
        void comboBoxActivated();
        
    private:
        enum YokeValidationResult {
            YOKE_VALIDATE_RESULT_ACCEPT,
            YOKE_VALIDATE_RESULT_OFF,
            YOKE_VALIDATE_RESULT_PREVIOUS
        };
        MapYokingGroupComboBox(const MapYokingGroupComboBox&);

        MapYokingGroupComboBox& operator=(const MapYokingGroupComboBox&);
        
        YokeValidationResult validateYoking(AnnotationTextSubstitutionFile* selectedAnnTextSubFile,
                                            CaretMappableDataFile* selectedMapFile,
                                            MediaFile* selectedMediaFile,
                                            int32_t& selectedMapIndexInOut,
                                            bool& selectionStatusInOut);
        
        EnumComboBoxTemplate* m_comboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_YOKING_GROUP_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_YOKING_GROUP_COMBO_BOX_DECLARE__

} // namespace
#endif  //__MAP_YOKING_GROUP_COMBO_BOX_H__
