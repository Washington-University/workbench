#ifndef __IDENTIFICATION_HISTORY_MANAGER_H__
#define __IDENTIFICATION_HISTORY_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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


#include <deque>
#include <memory>

#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class IdentificationHistoryRecord;
    class SceneClassAssistant;

    class IdentificationHistoryManager : public CaretObject, public SceneableInterface {
        
    public:
        IdentificationHistoryManager();
        
        virtual ~IdentificationHistoryManager();
        
        IdentificationHistoryManager(const IdentificationHistoryManager&) = delete;

        IdentificationHistoryManager& operator=(const IdentificationHistoryManager&) = delete;

        AString getText() const;
        
        int32_t getShowLastHistoryCount() const;
        
        void setShowLastHistoryCount(const int32_t historyCount);

        void clearHistory();
        
        void addHistoryRecord(IdentificationHistoryRecord* historyRecord);
        
        int32_t getNumberOfHistoryRecords() const;
        
        const IdentificationHistoryRecord* getHistoryRecord(const int32_t historyIndex) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
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
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::deque<std::unique_ptr<IdentificationHistoryRecord>> m_historyRecords;
        
        int32_t m_showLastHistoryCount = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IDENTIFICATION_HISTORY_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_HISTORY_MANAGER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_HISTORY_MANAGER_H__
