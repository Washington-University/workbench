#ifndef __USER_INPUT_MODE_VIEW__H_
#define __USER_INPUT_MODE_VIEW__H_

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
#include "UserInputModeAbstract.h"

namespace caret {

    class BrainOpenGLViewportContent;
    class ChartTwoOverlay;

    class UserInputModeView : public UserInputModeAbstract {
        
    public:
        UserInputModeView(const int32_t browserIndexIndex);
        
        virtual ~UserInputModeView();
        
        virtual void initialize();
        
        virtual void finish();

        virtual void update();
        
        virtual CursorEnum::Enum getCursor() const;
        
        virtual bool keyPressEvent(const KeyEvent& /*keyEvent*/) override;
        
        virtual void mouseLeftDoubleClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClickWithShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClickWithCtrlShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDrag(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithAlt(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithCtrl(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithShift(const MouseEvent& mouseEvent);

        virtual void mouseLeftPress(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftRelease(const MouseEvent& mouseEvent);
        
        virtual void mouseMove(const MouseEvent& mouseEvent) override;
        
        virtual void gestureEvent(const GestureEvent& gestureEvent);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
        virtual void processEditMenuItemSelection(const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem) override;
        
        virtual void getEnabledEditMenuItems(std::vector<BrainBrowserWindowEditMenuItemEnum::Enum>& enabledEditMenuItemsOut,
                                             AString& redoMenuItemSuffixTextOut,
                                             AString& undoMenuItemSuffixTextOut,
                                             AString& pasteTextOut,
                                             AString& pasteSpecialTextOut) override;
        
    protected:
        UserInputModeView(const int32_t browserIndexIndex,
                          const UserInputModeEnum::Enum inputMode);
        
    private:
        enum class ChartActiveLayerMode {
            DECREMENT,
            DESELECT_ALL,
            INCREMENT,
            SELECT
        };
        
        enum class VOLUME_MPR_CURSOR_MODE {
            INVALID,
            ROTATE_SLICE,
            SCROLL_SLICE,
            SELECT_SLICE
        };
        
        UserInputModeView(const UserInputModeView&);

        UserInputModeView& operator=(const UserInputModeView&);

        void updateGraphics(const MouseEvent& mouseEvent);
        
        void updateGraphics(const BrainOpenGLViewportContent* viewportContent);
        
        VOLUME_MPR_CURSOR_MODE getVolumeMprMouseMode(const MouseEvent& mouseEvent);
        
        void processModelViewIdentification(BrainOpenGLViewportContent* viewportContent,
                                            BrainOpenGLWidget* openGLWidget,
                                            const int32_t mouseClickX,
                                            const int32_t mouseClickY);
        
        void processChartActiveLayerAction(const ChartActiveLayerMode chartActiveMode,
                                           ChartTwoOverlay* chartOverlay,
                                           const int32_t pointIndex);
        
        void applyGraphicsRegionSelectionBox(const MouseEvent& mouseEvent);
        
        void updateGraphicsRegionSelectionBox(const MouseEvent& mouseEvent);
        
        float m_mediaLeftDragWithCtrlModelXYZ[3];
        
        bool m_mediaLeftDragWithCtrlModelXYZValidFlag = false;
        
        float m_histologyLeftDragWithCtrlModelXYZ[3];
        
        bool m_histologyLeftDragWithCtrlModelXYZValidFlag = false;
        
        bool m_lastSliceIncrementMouseYValid = false;
        int32_t m_lastSliceIncrementMouseY = -1;
        
        VOLUME_MPR_CURSOR_MODE m_mprCursorMode = VOLUME_MPR_CURSOR_MODE::INVALID;
        
    public:
        virtual AString toString() const;
        
    };
    
#ifdef __USER_INPUT_MODE_VIEW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_VIEW_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_VIEW__H_
