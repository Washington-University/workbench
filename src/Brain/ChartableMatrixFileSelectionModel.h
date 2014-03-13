#ifndef __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_H__
#define __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_H__

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


#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class Brain;
    class ChartableMatrixInterface;
    class SceneClassAssistant;

    class ChartableMatrixFileSelectionModel : public CaretObject, public SceneableInterface {
        
    public:
        ChartableMatrixFileSelectionModel(Brain* brain);
        
        virtual ~ChartableMatrixFileSelectionModel();
        
        ChartableMatrixInterface* getSelectedFile();
        
        const ChartableMatrixInterface* getSelectedFile() const;
        
        std::vector<ChartableMatrixInterface*> getAvailableFiles() const;
        
        void setSelectedFile(ChartableMatrixInterface* selectedFile);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        ChartableMatrixFileSelectionModel(const ChartableMatrixFileSelectionModel&);

        ChartableMatrixFileSelectionModel& operator=(const ChartableMatrixFileSelectionModel&);
        
        void updateSelection() const;
        
        mutable ChartableMatrixInterface* m_selectedFile;
        
        Brain* m_brain;
        
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_MATRIX_FILE_SELECTION_MODEL_DECLARE__

} // namespace
#endif  //__CHARTABLE_MATRIX_FILE_SELECTION_MODEL_H__
