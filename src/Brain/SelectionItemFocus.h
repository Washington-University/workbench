#ifndef __SELECTION_ITEM_FOCUS__H_
#define __SELECTION_ITEM_FOCUS__H_

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

    class Focus;
    class FociFile;
    class HistologySlicesFile;
    class Surface;
    class VolumeMappableInterface;
    
    class SelectionItemFocus : public SelectionItem {
        
    public:
        enum class IdType {
            INVALID,
            HISTOLOGY,
            SURFACE,
            VOLUME
        };
        
        void setSurfaceSelection(Surface* surface,
                                 FociFile* fociFile,
                                 Focus* focus,
                                 const int32_t focusIndex,
                                 const int32_t focusProjectionIndex);

        void setHistologySelection(HistologySlicesFile* histologySlicesFile,
                                   FociFile* fociFile,
                                   Focus* focus,
                                   const int32_t focusIndex,
                                   const int32_t focusProjectionIndex);
        
        void setVolumeSelection(VolumeMappableInterface* volumeMappableInterface,
                                FociFile* fociFile,
                                Focus* focus,
                                const int32_t focusIndex,
                                const int32_t focusProjectionIndex);

        SelectionItemFocus();
        
        virtual ~SelectionItemFocus();
        
        virtual bool isValid() const override;
        
        IdType getIdType() const;
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        VolumeMappableInterface* getVolumeFile();
        
        const VolumeMappableInterface* getVolumeFile() const;

        HistologySlicesFile* getHistologySlicesFile();
        
        const HistologySlicesFile* getHistologySlicesFile() const;
        
        Focus* getFocus();
        
        const Focus* getFocus() const;
        
        FociFile* getFociFile();
        
        const FociFile* getFociFile() const;
        
        int32_t getFocusIndex() const;
        
        int32_t getFocusProjectionIndex() const;
        
        virtual void reset() override;
        
        virtual AString toString() const;
        
    private:
        void resetPrivate();
        
        SelectionItemFocus(const SelectionItemFocus&);

        SelectionItemFocus& operator=(const SelectionItemFocus&);
        
        IdType m_idType = IdType::INVALID;
        Focus* m_focus = NULL;
        FociFile* m_fociFile = NULL;
        Surface* m_surface = NULL;
        VolumeMappableInterface* m_volumeFile = NULL;
        HistologySlicesFile* m_histologySlicesFile = NULL;
        int32_t m_focusIndex = -1;
        int32_t m_focusProjectionIndex = -1;
    };
    
#ifdef __SELECTION_ITEM_FOCUS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_FOCUS__H_
