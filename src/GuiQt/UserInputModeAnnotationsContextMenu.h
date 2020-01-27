#ifndef __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_H__
#define __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include <QMenu>

#include "AnnotationGroupingModeEnum.h"
#include "MouseEvent.h"

namespace caret {

    class Annotation;
    class AnnotationFile;
    class AnnotationText;
    class BrainOpenGLWidget;
    class BrowserTabContent;
    class SelectionManager;
    class UserInputModeAnnotations;

    class UserInputModeAnnotationsContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        UserInputModeAnnotationsContextMenu(UserInputModeAnnotations* userInputModeAnnotations,
                                            const MouseEvent& mouseEvent,
                                            SelectionManager* selectionManager,
                                            BrowserTabContent* browserTabContent,
                                            BrainOpenGLWidget* parentOpenGLWidget);
        
        virtual ~UserInputModeAnnotationsContextMenu();
        
        Annotation* getNewAnnotationCreatedByContextMenu();

        // ADD_NEW_METHODS_HERE

    private slots:
        void copyAnnotationToAnnotationClipboard();
        
        void cutAnnnotation();
        
        void deleteAnnotations();
        
        void pasteAnnotationFromAnnotationClipboard();
        
        void pasteSpecialAnnotationFromAnnotationClipboard();
        
        void deselectAllAnnotations();
        
        void selectAllAnnotations();
        
        void setAnnotationText();
        
        void turnOffDisplayInOtherTabs();
        
        void turnOnDisplayInAllTabs();
        
        void turnOnDisplayInAllGroups();
        
        void turnOnDisplayInGroup(QAction*);
        
        void applyGroupingGroup();
        
        void applyGroupingRegroup();
        
        void applyGroupingUngroup();
        
        void duplicateAnnotationSelected(QAction*);
        
    private:
        UserInputModeAnnotationsContextMenu(const UserInputModeAnnotationsContextMenu&);

        UserInputModeAnnotationsContextMenu& operator=(const UserInputModeAnnotationsContextMenu&);
        
        void applyGrouping(const AnnotationGroupingModeEnum::Enum grouping);
        
        QMenu* createTurnOnInDisplayGroupMenu();
        
        QMenu* createDuplicateTabSpaceAnnotationMenu();
        
        UserInputModeAnnotations* m_userInputModeAnnotations;
        
        /*
         * NOT a reference.  Need to COPY as its source may be deleted.
         */
        const MouseEvent m_mouseEvent;
        
        SelectionManager* m_selectionManager;
        
        BrowserTabContent* m_browserTabContent;
        
        BrainOpenGLWidget* m_parentOpenGLWidget;
        
        AnnotationFile* m_annotationFile;
        
        std::vector<Annotation*> m_threeDimCoordAnnotations;
        
        Annotation* m_annotation;
        
        AnnotationText* m_textAnnotation;
        
        Annotation* m_newAnnotationCreatedByContextMenu;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_H__
