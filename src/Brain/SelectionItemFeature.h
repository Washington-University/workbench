#ifndef __SELECTION_ITEM_FEATURE__H_
#define __SELECTION_ITEM_FEATURE__H_

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


#include "SelectionItem.h"

namespace caret {

    class FeatureFile;
    class FeatureItem;
    class HistologySlicesFile;
    class Surface;
    class VolumeMappableInterface;
    
    class SelectionItemFeature : public SelectionItem {
        
    public:
        enum class IdType {
            INVALID,
            HISTOLOGY,
            SURFACE,
            VOLUME,
            WHOLE_BRAIN
        };
        
        void setSurfaceSelection(const Surface* surface,
                                 FeatureFile* featureFile,
                                 FeatureItem* featureItem,
                                 const int32_t featureIndex);

        void setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                   FeatureFile* featureFile,
                                   FeatureItem* featureItem,
                                   const int32_t featureIndex);
        
        void setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                FeatureFile* featureFile,
                                FeatureItem* featureItem,
                                const int32_t featureIndex);

        void setWholeBrainSelection(FeatureFile* featureFile,
                                    FeatureItem* featureItem,
                                    const int32_t featureIndex);

        SelectionItemFeature();
        
        virtual ~SelectionItemFeature();
        
        virtual bool isValid() const override;
        
        IdType getIdType() const;
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        VolumeMappableInterface* getVolumeFile();
        
        const VolumeMappableInterface* getVolumeFile() const;

        HistologySlicesFile* getHistologySlicesFile();
        
        const HistologySlicesFile* getHistologySlicesFile() const;
        
        FeatureItem* getFeatureItem();
        
        const FeatureItem* getFeatureItem() const;
        
        FeatureFile* getFeatureFile();
        
        const FeatureFile* getFeatureFile() const;
        
        int32_t getFeatureItemIndex() const;
        
        virtual void reset() override;
        
        virtual AString toString() const;
        
    private:
        void resetPrivate();
        
        SelectionItemFeature(const SelectionItemFeature&);

        SelectionItemFeature& operator=(const SelectionItemFeature&);
        
        IdType m_idType = IdType::INVALID;
        FeatureItem* m_featureItem = NULL;
        FeatureFile* m_featureFile = NULL;
        const Surface* m_surface = NULL;
        VolumeMappableInterface* m_volumeFile = NULL;
        HistologySlicesFile* m_histologySlicesFile = NULL;
        int32_t m_featureIndex = -1;
    };
    
#ifdef __SELECTION_ITEM_FEATURE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_FEATURE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_FEATURE__H_
