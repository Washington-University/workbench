#ifndef __SELECTION_ITEM_NEUROGLANCER_ANNOTATION__H_
#define __SELECTION_ITEM_NEUROGLANCER_ANNOTATION__H_

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

    class NeuroglancerAnnotation;
    class NeuroglancerAnnotationsFile;
    class HistologySlicesFile;
    class Surface;
    class VolumeMappableInterface;
    
    class SelectionItemNeuroglancerAnnotation : public SelectionItem {
        
    public:
        enum class IdType {
            INVALID,
            HISTOLOGY,
            SURFACE,
            VOLUME,
            WHOLE_BRAIN
        };
        
        void setSurfaceSelection(const Surface* surface,
                                 NeuroglancerAnnotationsFile* neuroAnnFile,
                                 NeuroglancerAnnotation* neuroAnn,
                                 const int32_t neuroAnnIndex);

        void setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                   NeuroglancerAnnotationsFile* neuroAnnFile,
                                   NeuroglancerAnnotation* neuroAnn,
                                   const int32_t neuroAnnIndex);
        
        void setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                NeuroglancerAnnotationsFile* neuroAnnFile,
                                NeuroglancerAnnotation* neuroAnn,
                                const int32_t neuroAnnIndex);

        void setWholeBrainSelection(NeuroglancerAnnotationsFile* neuroAnnFile,
                                    NeuroglancerAnnotation* neuroAnn,
                                    const int32_t neuroAnnIndex);

        SelectionItemNeuroglancerAnnotation();
        
        virtual ~SelectionItemNeuroglancerAnnotation();
        
        virtual bool isValid() const override;
        
        IdType getIdType() const;
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        VolumeMappableInterface* getVolumeFile();
        
        const VolumeMappableInterface* getVolumeFile() const;

        HistologySlicesFile* getHistologySlicesFile();
        
        const HistologySlicesFile* getHistologySlicesFile() const;
        
        NeuroglancerAnnotation* getNeuroglancerAnnotation();
        
        const NeuroglancerAnnotation* getNeuroglancerAnnotation() const;
        
        NeuroglancerAnnotationsFile* getNeuroglancerAnnotationsFile();
        
        const NeuroglancerAnnotationsFile* getNeuroglancerAnnotationsFile() const;
        
        int32_t getNeuroglancerAnnotationIndex() const;
        
        virtual void reset() override;
        
        virtual AString toString() const;
        
    private:
        void resetPrivate();
        
        SelectionItemNeuroglancerAnnotation(const SelectionItemNeuroglancerAnnotation&);

        SelectionItemNeuroglancerAnnotation& operator=(const SelectionItemNeuroglancerAnnotation&);
        
        IdType m_idType = IdType::INVALID;
        NeuroglancerAnnotation* m_neuroAnn = NULL;
        NeuroglancerAnnotationsFile* m_neuroAnnFile = NULL;
        const Surface* m_surface = NULL;
        VolumeMappableInterface* m_volumeFile = NULL;
        HistologySlicesFile* m_histologySlicesFile = NULL;
        int32_t m_neuroAnnIndex = -1;
    };
    
#ifdef __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_NEUROGLANCER_ANNOTATION__H_
