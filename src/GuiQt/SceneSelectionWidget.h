#ifndef __SCENE_SELECTION_WIDGET_H__
#define __SCENE_SELECTION_WIDGET_H__

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

#include <QScrollArea>

class QVBoxLayout;

namespace caret {

    class Scene;
    class SceneFile;
    class SceneClassInfoWidget;
    
    class SceneSelectionWidget : public QScrollArea {
        
        Q_OBJECT

    public:
        SceneSelectionWidget(QWidget* parent = 0);
        
        virtual ~SceneSelectionWidget();
        
        SceneSelectionWidget(const SceneSelectionWidget&) = delete;

        SceneSelectionWidget& operator=(const SceneSelectionWidget&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        void highlightScene(const Scene* scene);
        
        void highlightSceneAtIndex(const int32_t sceneIndex);
        
        void updateContent(SceneFile* selectedSceneFile,
                           Scene* selectedScene);
        
        QVBoxLayout* m_sceneSelectionLayout;
        
        std::vector<SceneClassInfoWidget*> m_sceneClassInfoWidgets;
        
        int32_t m_selectedSceneClassInfoIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_SELECTION_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_SELECTION_WIDGET_DECLARE__

} // namespace
#endif  //__SCENE_SELECTION_WIDGET_H__
