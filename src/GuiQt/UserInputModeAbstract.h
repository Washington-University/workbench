#ifndef __USER_INPUT_MODE_ABSTRACT_H__
#define __USER_INPUT_MODE_ABSTRACT_H__

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

#include "BrainBrowserWindowEditMenuItemEnum.h"
#include "CaretObject.h"
#include "CaretPointer.h"
#include "CursorEnum.h"

class QPoint;
class QWidget;

namespace caret {

    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    class KeyEvent;
    class MouseEvent;
    
    class UserInputModeAbstract : public CaretObject {
        
    public:
        /** Enumerated type for input modes */
        enum UserInputMode {
            /** Invalid */
            INVALID,
            /** Annotation Operations */
            ANNOTATIONS,
            /** Border Operations */
            BORDERS,
            /** Foci Operations */
            FOCI,
            /** Image Operations */
            IMAGE,
            /** Viewing Operations */
            VIEW,
            /** Volume Edit Operations */
            VOLUME_EDIT
        };
        
        UserInputModeAbstract(const UserInputMode inputMode);
        
        virtual ~UserInputModeAbstract();
        
        
        /**
         * @return The input mode enumerated type.
         */
        UserInputMode getUserInputMode() const;
        
        /**
         * Called when 'this' user input receiver is set
         * to receive events.
         */
        virtual void initialize() = 0;
        
        /**
         * Called when 'this' user input receiver is no
         * longer set to receive events.
         */
        virtual void finish() = 0;
        
        /**
         * Called to update the input receiver for various events.
         */
        virtual void update() = 0;
        
        QWidget* getWidgetForToolBar();
        
        /**
         * @return The cursor for display in the OpenGL widget.
         */
        virtual CursorEnum::Enum getCursor() const = 0;
        
        /**
         * Process a key press event
         *
         * @param keyEvent
         *     Key event information.
         * @return
         *     True if the input process recognized the key event
         *     and the key event SHOULD NOT be propagated to parent
         *     widgets
         */
        virtual bool keyPressEvent(const KeyEvent& /*keyEvent*/) { return false; }
        
        /**
         * Process a mouse left double-click event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDoubleClick(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left press event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftPress(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left release event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftRelease(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left click event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClick(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left click with shift key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClickWithShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left click with ctrl and shift keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftClickWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with no keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDrag(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with only the alt key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithAlt(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with ctrl key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithCtrl(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with ctrl and shift keys down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse left drag with shift key down event.
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseLeftDragWithShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse move with no buttons or keys down
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseMove(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Process a mouse move with no buttons and shift key down
         *
         * @param mouseEvent
         *     Mouse event information.
         */
        virtual void mouseMoveWithShift(const MouseEvent& /*mouseEvent*/) { }
        
        /**
         * Show a context menu (pop-up menu at mouse location)
         *
         * @param mouseEvent
         *     Mouse event information.
         * @param menuPosition
         *     Point at which menu is displayed (passed to QMenu::exec())
         * @param openGLWidget
         *     OpenGL widget in which context menu is requested
         */
        virtual void showContextMenu(const MouseEvent& /*mouseEvent*/,
                                     const QPoint& /* menuPosition */,
                                     BrainOpenGLWidget* /* openGLWidget */) { }
        
        virtual void processEditMenuItemSelection(const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem);
        
        virtual void getEnabledEditMenuItems(std::vector<BrainBrowserWindowEditMenuItemEnum::Enum>& enabledEditMenuItemsOut,
                                             AString& redoMenuItemSuffixTextOut,
                                             AString& undoMenuItemSuffixTextOut,
                                             AString& pasteTextOut,
                                             AString& pasteSpecialTextOut);
        
        const MouseEvent* getMousePosition() const;
        
        void setMousePosition(const MouseEvent* mouseEvent,
                              const bool valid);
        
    protected:
        void setWidgetForToolBar(QWidget* widgetForToolBar);
        
    private:
        UserInputModeAbstract(const UserInputModeAbstract&);

        UserInputModeAbstract& operator=(const UserInputModeAbstract&);

        const UserInputMode m_userInputMode;
        
        QWidget* m_widgetForToolBar;
        
        bool    m_mousePositionValid;
        CaretPointer<MouseEvent> m_mousePositionEvent;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_ABSTRACT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ABSTRACT_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ABSTRACT_H__
