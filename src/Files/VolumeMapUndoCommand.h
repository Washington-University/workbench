#ifndef __VOLUME_MAP_UNDO_COMMAND_H__
#define __VOLUME_MAP_UNDO_COMMAND_H__

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


#include "CaretUndoCommand.h"



namespace caret {

    class VolumeFile;
    
    class VolumeMapUndoCommand : public CaretUndoCommand {
        
    public:
        VolumeMapUndoCommand(VolumeFile* volumeFile,
                             const int32_t mapIndex);
        
        virtual ~VolumeMapUndoCommand();
        
        virtual bool redo(AString& errorMessageOut);
        
        virtual bool undo(AString& errorMessageOut);
        
        int32_t count() const;
        
        void addVoxelRedoUndo(const int64_t ijk[3],
                              const float redoValue,
                              const float undoValue);
        
        void addVoxelRedoUndo(const int64_t i,
                              const int64_t j,
                              const int64_t k,
                              const float redoValue,
                              const float undoValue);
        
        // ADD_NEW_METHODS_HERE

    private:
        class VoxelMemento {
        public:
            VoxelMemento(const int64_t ijk[3],
                         const float redoValue,
                         const float undoValue);
            
            ~VoxelMemento();
            
            int64_t m_ijk[3];
            
            float m_redoValue;
            
            float m_undoValue;
        };
        
        VolumeMapUndoCommand(const VolumeMapUndoCommand&);

        VolumeMapUndoCommand& operator=(const VolumeMapUndoCommand&);
        
        VolumeFile* m_volumeFile;
        
        const int32_t m_mapIndex;
        
        std::vector<VoxelMemento*> m_voxelMementos;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_MAP_UNDO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MAP_UNDO_COMMAND_DECLARE__

} // namespace
#endif  //__VOLUME_MAP_UNDO_COMMAND_H__
