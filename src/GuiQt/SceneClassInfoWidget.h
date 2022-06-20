#ifndef __SCENE_CLASS_INFO_WIDGET_H__
#define __SCENE_CLASS_INFO_WIDGET_H__

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

#include <QGroupBox>

class QLabel;

namespace caret {

    class AString;
    class Scene;
    class SceneInfo;
    
    class SceneClassInfoWidget : public QGroupBox {
        
        Q_OBJECT

   public:
        SceneClassInfoWidget();
        
        ~SceneClassInfoWidget();
        
        SceneClassInfoWidget(const SceneClassInfoWidget&) = delete;
        
        SceneClassInfoWidget& operator=(const SceneClassInfoWidget&) = delete;
        
        void updateContent(Scene* scene,
                           const int32_t sceneIndex,
                           const bool activeSceneFlag);
        
        void setBackgroundForSelected(const bool selected);
        
        Scene* getScene();
        
        int32_t getSceneIndex() const;
        
        bool isValid() const;
        
        static void getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                               const int32_t sceneIndex,
                                                               AString& nameTextOut,
                                                               AString& sceneIdTextOut,
                                                               AString& abbreviatedDescriptionTextOut,
                                                               AString& fullDescriptionTextOut,
                                                               const bool scenePreviewDialogFlag);
        
    signals:
        /**
         * Emited when user activates (double clicks) this widget.
         */
        void activated(const int32_t sceneIndex);
        
        /**
         * Emited when user highlights (clicks) this widget.
         */
        void highlighted(const int32_t sceneIndex);
        
    protected:
        virtual void mousePressEvent(QMouseEvent* event);
        
        virtual void mouseDoubleClickEvent(QMouseEvent* event);
        
    private:
        static void limitToNumberOfLines(AString& textLines,
                                         const int32_t maximumNumberOfLines);
        
        QWidget* m_leftSideWidget;
        
        QWidget* m_rightSideWidget;
        
        QLabel* m_previewImageLabel;
        
        QLabel* m_activeSceneLabel;
        
        QLabel* m_nameLabel;
        
        QLabel* m_sceneIdLabel;
        
        QLabel* m_descriptionLabel;
        
        Scene* m_scene;
        
        int32_t m_sceneIndex;
        
        bool m_previewImageValid;
        
        bool m_defaultAutoFillBackgroundStatus;
        
        QPalette::ColorRole m_defaultBackgroundRole;
    };
    
#ifdef __SCENE_CLASS_INFO_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_INFO_WIDGET_DECLARE__

} // namespace
#endif  //__SCENE_CLASS_INFO_WIDGET_H__
