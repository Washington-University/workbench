#ifndef __HISTOLOGY_SPACE_KEY_H__
#define __HISTOLOGY_SPACE_KEY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include "CaretObjectTracksModification.h"

#include "SceneableInterface.h"

class QDir;

namespace caret {
    class SceneClassAssistant;

    class HistologySpaceKey : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        HistologySpaceKey();
        
        HistologySpaceKey(const AString& histologySlicesFileName,
                          const int32_t sliceNumber);
        
        virtual ~HistologySpaceKey();
        
        HistologySpaceKey(const HistologySpaceKey& obj);

        HistologySpaceKey& operator=(const HistologySpaceKey& obj);
        
        bool operator==(const HistologySpaceKey& obj) const;
        
        bool operator!=(const HistologySpaceKey& obj) const;
        
        AString toString() const override;
        
        bool isValid() const;
        
        AString getHistologySlicesFileName() const;
        
        void setHistologySlicesFileName(const AString& histologySlicesFileName);
        
        int32_t getSliceNumber() const;
        
        void setSliceNumber(const int32_t sliceNumber);
        
        AString toEncodedString(const QDir& directory) const;
        
        bool setFromEncodedString(const QDir& directory,
                                  const AString& encodedString);
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperHistologySpaceKey(const HistologySpaceKey& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        AString m_histologySlicesFileName;
        
        int32_t m_sliceNumber = 0;
        
        static const AString s_encodingSeparator;
        
        static const AString s_encodeRelativeToDirectory;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SPACE_KEY_DECLARE__
    const AString HistologySpaceKey::s_encodingSeparator = "::::";
    const AString HistologySpaceKey::s_encodeRelativeToDirectory = "ENCODED_RELATIVE_TO_DIRECTORY";
#endif // __HISTOLOGY_SPACE_KEY_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SPACE_KEY_H__
