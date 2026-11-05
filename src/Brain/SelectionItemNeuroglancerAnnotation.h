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

    class HistologySlicesFile;
    class NeuroglancerAnnotationFile;
    class NeuroglancerAnnotation;
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
                                 NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                 const NeuroglancerAnnotation* neuroglancerAnnotation);

        void setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                   NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                   const NeuroglancerAnnotation* neuroglancerAnnotation);

        void setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                const NeuroglancerAnnotation* neuroglancerAnnotation);

        void setWholeBrainSelection(NeuroglancerAnnotationFile* neuroglancerAnnotationFile,
                                    const NeuroglancerAnnotation* neuroglancerAnnotation);

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
        
        const NeuroglancerAnnotation* getNeuroglancerAnnotation() const;
        
        NeuroglancerAnnotationFile* getNeuroglancerAnnotationFile();
        
        const NeuroglancerAnnotationFile* getNeuroglancerAnnotationFile() const;
        
        uint64_t getGroupID() const;
        
        uint64_t getUniqueID() const;
        
        virtual void reset() override;
        
        virtual AString toString() const;
        
    private:
        void resetPrivate();
        
        SelectionItemNeuroglancerAnnotation(const SelectionItemNeuroglancerAnnotation&);

        SelectionItemNeuroglancerAnnotation& operator=(const SelectionItemNeuroglancerAnnotation&);
        
        IdType m_idType = IdType::INVALID;
        const NeuroglancerAnnotation* m_neuroglancerAnnotation = NULL;
        NeuroglancerAnnotationFile* m_neuroglancerAnnotationFile = NULL;
        const Surface* m_surface = NULL;
        VolumeMappableInterface* m_volumeFile = NULL;
        HistologySlicesFile* m_histologySlicesFile = NULL;
    };
    
#ifdef __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_NEUROGLANCER_ANNOTATION_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_NEUROGLANCER_ANNOTATION__H_
