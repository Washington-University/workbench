
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

#define __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "AnnotationSelectionViewController.h"
#undef __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupAndTabItemViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSelectionViewController 
 * \brief View controller for display of annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parentObjectName
 *     Name of parent object
 * @param parent
 *     The parent widget.
 */
AnnotationSelectionViewController::AnnotationSelectionViewController(const int32_t browserWindowIndex,
                                                                     const QString& parentObjectName,
                                                                     QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    const QString objectNamePrefix(parentObjectName
                                   + ":AnnotationSelection");
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    QLabel* groupLabel = new QLabel("Group");
    
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this,
                                                          (objectNamePrefix
                                                           + ":DisplayGroup"),
                                                          "annotations");
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupSelectionLayout = new QHBoxLayout();
    groupSelectionLayout->addWidget(groupLabel);
    groupSelectionLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupSelectionLayout->addStretch();
    QMargins groupLayoutMargins = groupSelectionLayout->contentsMargins();
    groupLayoutMargins.setBottom(0);
    groupLayoutMargins.setTop(0);

    m_displayAnnotationsCheckBox = new QCheckBox("Display Annotations");
    m_displayAnnotationsCheckBox->setToolTip("Disables/enables display of annotations in all windows");
    QObject::connect(m_displayAnnotationsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    m_displayAnnotationsCheckBox->setObjectName(objectNamePrefix
                                                + "DisplayAnnotatons");
    macroManager->addMacroSupportToObject(m_displayAnnotationsCheckBox,
                                          "Enable display of annotations");
    
    
    m_displayTextAnnotationsCheckBox = new QCheckBox("Display Text Annotations");
    m_displayTextAnnotationsCheckBox->setToolTip("Disables/enables display of text annotations in all windows");
    QObject::connect(m_displayTextAnnotationsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    m_displayTextAnnotationsCheckBox->setObjectName(objectNamePrefix
                                                    + "DisplayTextAnnotatons");
    macroManager->addMacroSupportToObject(m_displayTextAnnotationsCheckBox,
                                          "Enable display of text annotations");
    
    m_displayWindowAnnotationInSingleTabViewsCheckBox = new QCheckBox("Show Window "
                                                                      + QString::number(m_browserWindowIndex + 1)
                                                                      + " Annotations in Single Tab View");
    const QString singTT(WuQtUtilities::createWordWrappedToolTipText("When checked, window annotations are always displayed.\n"
                                                                     "When unchecked, window annotations are only displayed when tile tabs is enabled."));
    m_displayWindowAnnotationInSingleTabViewsCheckBox->setToolTip(singTT);
    QObject::connect(m_displayWindowAnnotationInSingleTabViewsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    m_displayWindowAnnotationInSingleTabViewsCheckBox->setObjectName(objectNamePrefix
                                                    + "DisplayWindowAnnotatonsInSingleTabView");
    macroManager->addMacroSupportToObject(m_displayWindowAnnotationInSingleTabViewsCheckBox,
                                          "Enable display window annotations in single tab view");
    
    const QString smallTT(WuQtUtilities::createWordWrappedToolTipText("When surfaces are very small (e.g. Marmoset),"
                                                                      "the minimum size of a text annotation in "
                                                                      "surface space may be too large as the size "
                                                                      "of the drawn text is a function of the surface "
                                                                      "size.  Enabling this item allows text annotations "
                                                                      "to achieve a smaller size.  If enabled "
                                                                      "with previously created annotations, it may result in "
                                                                      "many of them shrinking in size.  Using Edit Menu -> "
                                                                      "Select All and then adjusting the Text Size "
                                                                      "will simplify bringing all of them back to a "
                                                                      "reasonable size."));
    m_smallSurfaceTextSizeCorrectionCheckBox = new QCheckBox("Text size improvement on small surfaces");
    m_smallSurfaceTextSizeCorrectionCheckBox->setToolTip(smallTT);
    QObject::connect(m_smallSurfaceTextSizeCorrectionCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    

    m_sceneAssistant = new SceneClassAssistant();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displayAnnotationsCheckBox);
    layout->addWidget(m_displayTextAnnotationsCheckBox);
    layout->addWidget(m_displayWindowAnnotationInSingleTabViewsCheckBox);
    layout->addWidget(m_smallSurfaceTextSizeCorrectionCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(groupSelectionLayout);
    layout->addWidget(createSelectionWidget(objectNamePrefix), 100);
    
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    s_allAnnotationSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
AnnotationSelectionViewController::~AnnotationSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
    
    s_allAnnotationSelectionViewControllers.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdateFlag = false;
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        doUpdateFlag = true;
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
       EventUserInterfaceUpdate* eventUI = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUI);

        doUpdateFlag = true;
        
        eventUI->setEventProcessed();
    }
    
    if (doUpdateFlag) {
        updateAnnotationSelections();
    }
}

/**
 * Update other selection annotation selector since they may share properties
 */
void
AnnotationSelectionViewController::updateOtherAnnotationViewControllers()
{
    for (std::set<AnnotationSelectionViewController*>::iterator iter = s_allAnnotationSelectionViewControllers.begin();
         iter != s_allAnnotationSelectionViewControllers.end();
         iter++) {
        AnnotationSelectionViewController* avc = *iter;
        if (avc != this) {
            avc->updateAnnotationSelections();
        }
    }
}

/**
 * Update the annotation selections.
 */
void
AnnotationSelectionViewController::updateAnnotationSelections()
{
    const DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    m_displayAnnotationsCheckBox->setChecked(dpa->isDisplayAnnotations());
    m_displayTextAnnotationsCheckBox->setChecked(dpa->isDisplayTextAnnotations());
    m_displayWindowAnnotationInSingleTabViewsCheckBox->setChecked(dpa->isDisplayWindowAnnotationsInSingleTabViews(m_browserWindowIndex));
    m_smallSurfaceTextSizeCorrectionCheckBox->setChecked(dpa->isTextSizeSmallSurfaceCorrectionsEnabled());
    
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<AnnotationFile*> annotationFiles;
    brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    std::vector<DisplayGroupAndTabItemInterface*> fileItems;
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* annFile = *fileIter;
        fileItems.push_back(annFile);
    }
    
    const DisplayGroupEnum::Enum displayGroup = dpa->getDisplayGroupForTab(browserTabIndex);
    m_displayGroupComboBox->setSelectedDisplayGroup(displayGroup);
    
    const bool allowAnnotationSelectionFlag(true);
    
    m_selectionViewController->updateContent(fileItems,
                                             displayGroup,
                                             browserTabIndex,
                                             allowAnnotationSelectionFlag);
}

/**
 * @return New instance of selection widget
 * @param objectNamePrefix
 *    Object name prefix for macros
 */
QWidget*
AnnotationSelectionViewController::createSelectionWidget(const AString& objectNamePrefix)
{
    m_selectionViewController = new DisplayGroupAndTabItemViewController(DataFileTypeEnum::ANNOTATION,
                                                                         m_browserWindowIndex,
                                                                         AString(objectNamePrefix + ":Selection:"),
                                                                         "Annotation Selection");
    return m_selectionViewController;
}


/**
 * Called when one of the checkboxes is clicked.
 */
void
AnnotationSelectionViewController::checkBoxToggled()
{
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }

    dpa->setDisplayAnnotations(m_displayAnnotationsCheckBox->isChecked());
    dpa->setDisplayTextAnnotations(m_displayTextAnnotationsCheckBox->isChecked());
    dpa->setDisplayWindowAnnotationsInSingleTabViews(m_browserWindowIndex,
                                     m_displayWindowAnnotationInSingleTabViewsCheckBox->isChecked());
    dpa->setTextSizeSmallSurfaceCorrectionsEnabled(m_smallSurfaceTextSizeCorrectionCheckBox->isChecked());
    
    updateOtherAnnotationViewControllers();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when the display group combo box is changed.
 */
void
AnnotationSelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    dpa->setDisplayGroupForTab(browserTabIndex, displayGroup);
    
    updateAnnotationSelections();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
AnnotationSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationSelectionViewController",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
AnnotationSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

