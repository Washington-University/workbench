
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

#define __SCENE_SELECTION_WIDGET_DECLARE__
#include "SceneSelectionWidget.h"
#undef __SCENE_SELECTION_WIDGET_DECLARE__

#include <QVBoxLayout>
#include <QScrollBar>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventSceneActive.h"
#include "Scene.h"
#include "SceneClassInfoWidget.h"
#include "SceneFile.h"
#include "SceneInfo.h"

using namespace caret;


    
/**
 * \class caret::SceneSelectionWidget 
 * \brief Widget used for selection of scenes
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget
 */
SceneSelectionWidget::SceneSelectionWidget(QWidget* parent)
: QScrollArea(parent)
{
    /*
     * Widget and layout containing the scene class info.
     */
    QWidget* sceneSelectionWidget = new QWidget();
    QVBoxLayout* sceneSelectionWidgetLayout = new QVBoxLayout(sceneSelectionWidget);
    m_sceneSelectionLayout = new QVBoxLayout();
    sceneSelectionWidgetLayout->addLayout(m_sceneSelectionLayout);
    sceneSelectionWidgetLayout->addStretch();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(sceneSelectionWidget);
    setWidgetResizable(true);
}

/**
 * Destructor.
 */
SceneSelectionWidget::~SceneSelectionWidget()
{
}

/**
 * Update the content of the widget with the selected scene file and scene
 * @param selectedSceneFile
 *    The selected scene file
 * @param selectedScene
 *    The selected scene
 */
void
SceneSelectionWidget::updateContent(SceneFile* selectedSceneFile,
                                    Scene* selectedScene)
{
    EventSceneActive activeSceneEvent(EventSceneActive::MODE_GET);
    EventManager::get()->sendEvent(activeSceneEvent.getPointer());
    const Scene* activeScene = activeSceneEvent.getScene();
    
    for (std::vector<SceneClassInfoWidget*>::iterator iter = m_sceneClassInfoWidgets.begin();
         iter != m_sceneClassInfoWidgets.end();
         iter++) {
        SceneClassInfoWidget* sciw = *iter;
        sciw->blockSignals(true);
        sciw->updateContent(NULL,
                            -1,
                            false);
    }
    
    int32_t numberOfValidSceneInfoWidgets = 0;
    
    int32_t defaultIndex = -1;
    
    if (selectedSceneFile != NULL) {
        const int32_t numScenes = selectedSceneFile->getNumberOfScenes();
        for (int32_t i = 0; i < numScenes; i++) {
            Scene* scene = selectedSceneFile->getSceneAtIndex(i);
            
            QByteArray imageByteArray;
            AString imageBytesFormat;
            scene->getSceneInfo()->getImageBytes(imageByteArray,
                                                 imageBytesFormat);
            
            SceneClassInfoWidget* sciw = NULL;
            
            if (i >= static_cast<int32_t>(m_sceneClassInfoWidgets.size())) {
                sciw = new SceneClassInfoWidget();
                QObject::connect(sciw, SIGNAL(activated(int32_t)),
                                 this, SLOT(sceneActivated(int32_t)));
                QObject::connect(sciw, SIGNAL(highlighted(int32_t)),
                                 this, SLOT(sceneHighlighted(int32_t)));
                m_sceneClassInfoWidgets.push_back(sciw);
                m_sceneSelectionLayout->addWidget(sciw, 1);
            }
            else {
                sciw = m_sceneClassInfoWidgets[i];
            }
            
            const bool activeSceneFlag(scene == activeScene);
            sciw->updateContent(scene,
                                i,
                                activeSceneFlag);
            
            sciw->setBackgroundForSelected(i == 1);
            
            if (scene == selectedScene) {
                defaultIndex = i;
            }
            else if (defaultIndex < 0) {
                defaultIndex = i;
            }
        }
        numberOfValidSceneInfoWidgets = numScenes;
    }
    
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        bool visibilityStatus = false;
        if (i < numberOfValidSceneInfoWidgets) {
            visibilityStatus = true;
        }
        m_sceneClassInfoWidgets[i]->setVisible(visibilityStatus);
        m_sceneClassInfoWidgets[i]->blockSignals(false);
    }
    
    if (defaultIndex >= 0) {
        highlightSceneAtIndex(defaultIndex);
    }

}

/**
 * Highlight the scene at the given index.
 *
 * @param sceneIndex
 *     Index of scene to highlight.
 */
void
SceneSelectionWidget::highlightSceneAtIndex(const int32_t sceneIndex)
{
    bool sceneIndexValid = false;
    
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        SceneClassInfoWidget* sciw = m_sceneClassInfoWidgets[i];
        if (sciw->isValid()) {
            if (sciw->getSceneIndex() == sceneIndex) {
                sciw->setBackgroundForSelected(true);
                sceneIndexValid = true;
                m_selectedSceneClassInfoIndex = i;
                
                /*
                 * Ensure that the selected scene remains visible
                 * and do not alter value of horiztonal scroll bar
                 */
                const int horizValue = horizontalScrollBar()->value();
                const int xMargin = 0;
                const int yMargin = 50;
                ensureWidgetVisible(sciw,
                                                                xMargin,
                                                                yMargin);
               horizontalScrollBar()->setSliderPosition(horizValue);
            }
            else {
                sciw->setBackgroundForSelected(false);
            }
        }
    }
    
    if ( ! sceneIndexValid) {
        m_selectedSceneClassInfoIndex = -1;
    }
}

/**
 * Highlight the given scene.
 *
 * @param scene
 *     Scene to highlight.
 */
void
SceneSelectionWidget::highlightScene(const Scene* scene)
{
    const int32_t numberOfSceneInfoWidgets = static_cast<int32_t>(m_sceneClassInfoWidgets.size());
    for (int32_t i = 0; i < numberOfSceneInfoWidgets; i++) {
        SceneClassInfoWidget* sciw = m_sceneClassInfoWidgets[i];
        if (sciw->isValid()) {
            if (sciw->getScene() == scene) {
                highlightSceneAtIndex(sciw->getSceneIndex());
                return;
            }
        }
    }
}
