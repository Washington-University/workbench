

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QScrollArea>
#include <QToolBox>
#include <QTabWidget>
#include <QTimer>

#include "BorderSelectionViewController.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretPreferences.h"
#include "ChartViewController.h"
#include "ConnectivityManagerViewController.h"
#include "EventBrowserWindowContentGet.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberOrientationSelectionViewController.h"
#include "FiberTrajectorySelectionViewController.h"
#include "FociSelectionViewController.h"
#include "GuiManager.h"
#include "LabelSelectionViewController.h"
#include "OverlaySetViewController.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "SessionManager.h"
#include "VolumeSurfaceOutlineSetViewController.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Construct the toolbox.
 * 
 * @param browserWindowIndex
 *    Index of browser window that contains this toolbox.
 * @param title
 *    Title for the toolbox.
 * @param location
 *    Locations allowed for this toolbox.
 */
BrainBrowserWindowOrientedToolBox::BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                                                     const QString& title,
                                                                     const ToolBoxType toolBoxType,
                                                                     QWidget* parent)
:   QDockWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    toggleViewAction()->setText("Toolbox");
    
    m_toolBoxTitle = title;
    setWindowTitle(m_toolBoxTitle);
    
    bool isFeaturesToolBox  = false;
    bool isOverlayToolBox = false;
    bool isChartsToolBox = false;
    Qt::Orientation orientation = Qt::Horizontal;
    switch (toolBoxType) {
        case TOOL_BOX_FEATURES:
            orientation = Qt::Vertical;
            isFeaturesToolBox = true;
            toggleViewAction()->setText("Features Toolbox");
            break;
        case TOOL_BOX_OVERLAYS_HORIZONTAL:
            orientation = Qt::Horizontal;
            isOverlayToolBox = true;
            break;
        case TOOL_BOX_OVERLAYS_VERTICAL:
            orientation = Qt::Vertical;
            isOverlayToolBox = true;
            break;
        case TOOL_BOX_CHARTS_HORIZONTAL:
            orientation = Qt::Horizontal;
            isChartsToolBox = true;
            toggleViewAction()->setText("Charts Toolbox");
            break;
        case TOOL_BOX_CHARTS_VERTICAL:
            orientation = Qt::Vertical;
            isChartsToolBox = true;
            toggleViewAction()->setText("Charts Toolbox");
            break;
    }
    
    m_borderSelectionViewController = NULL;
    m_chartViewController = NULL;
    m_connectivityViewController = NULL;
    m_fiberOrientationViewController = NULL;
    m_fiberTrajectorySelectionViewController = NULL;
    m_fociSelectionViewController = NULL;
    m_labelSelectionViewController = NULL;
    m_overlaySetViewController = NULL;
    m_timeSeriesViewController = NULL;
    m_volumeSurfaceOutlineSetViewController = NULL;

    m_tabWidget = new QTabWidget();
    
    m_borderTabIndex = -1;
    m_chartTabIndex = -1;
    m_connectivityTabIndex = -1;
    m_fiberOrientationTabIndex = -1;
    m_fiberTrajectoryTabIndex = -1;
    m_fociTabIndex = -1;
    m_labelTabIndex = -1;
    m_overlayTabIndex = -1;
    m_timeSeriesTabIndex = -1;
    m_volumeSurfaceOutlineTabIndex = -1;
    
    if (isOverlayToolBox) {
        m_overlaySetViewController = new OverlaySetViewController(orientation,
                                                                      browserWindowIndex,
                                                                      this);  
        m_overlayTabIndex = addToTabWidget(m_overlaySetViewController,
                       "Layers");
    }
    if (isOverlayToolBox) {
        m_connectivityViewController = new ConnectivityManagerViewController(orientation,
                                                                                 browserWindowIndex,
                                                                                 DataFileTypeEnum::CONNECTIVITY_DENSE);
        m_connectivityTabIndex = addToTabWidget(m_connectivityViewController,
                             "Connectivity");
    }
    if (isOverlayToolBox) {
        m_timeSeriesViewController = new ConnectivityManagerViewController(orientation,
                                                                               browserWindowIndex,
                                                                               DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
        m_timeSeriesTabIndex = addToTabWidget(m_timeSeriesViewController,
                             "Data Series");
    }
    if (1) {//isChartsToolBox) {
        m_chartViewController = new ChartViewController(orientation,
                                                        browserWindowIndex);
        m_chartTabIndex = addToTabWidget(m_chartViewController,
                                               "Charts");                                                        
    }
    if (isFeaturesToolBox) {
        m_borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                                this);
        m_borderTabIndex = addToTabWidget(m_borderSelectionViewController,
                             "Borders");
    }
    
    if (isFeaturesToolBox) {
        m_fiberOrientationViewController = new FiberOrientationSelectionViewController(browserWindowIndex,
                                                                                       this);
        m_fiberOrientationTabIndex = addToTabWidget(m_fiberOrientationViewController,
                       "Fibers");
    }
    
    if (isFeaturesToolBox) {
        m_fiberTrajectorySelectionViewController = new FiberTrajectorySelectionViewController(browserWindowIndex,
                                                                                              this);
        m_fiberTrajectoryTabIndex = addToTabWidget(m_fiberTrajectorySelectionViewController,
                                                   "Trajectory");
    }
    
    if (isFeaturesToolBox) {
        m_fociSelectionViewController = new FociSelectionViewController(browserWindowIndex,
                                                                                this);
        m_fociTabIndex = addToTabWidget(m_fociSelectionViewController,
                             "Foci");
    }
    
    if (isFeaturesToolBox) {
        m_labelSelectionViewController = new LabelSelectionViewController(browserWindowIndex,
                                                                          this);
        m_labelTabIndex = addToTabWidget(m_labelSelectionViewController,
                       "Labels");
    }
    
    if (isOverlayToolBox) {
        m_volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                                m_browserWindowIndex);
        m_volumeSurfaceOutlineTabIndex = addToTabWidget(m_volumeSurfaceOutlineSetViewController,
                             "Vol/Surf Outline");
    }
        
    setWidget(m_tabWidget);

    if (orientation == Qt::Horizontal) {
        setMinimumHeight(200);
        setMaximumHeight(800);
    }
    else {
        if (isOverlayToolBox) {
            setMinimumWidth(300);
            setMaximumWidth(800);
        }
        else {
            setMinimumWidth(200);
            setMaximumWidth(800);
        }
    }

    QObject::connect(this, SIGNAL(topLevelChanged(bool)),
                     this, SLOT(floatingStatusChanged(bool)));

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Place widget into a scroll area and then into the tab widget.
 * @param page
 *    Widget that is added.
 * @param label
 *    Name corresponding to widget's tab.
 */
int 
BrainBrowserWindowOrientedToolBox::addToTabWidget(QWidget* page,
                                                  const QString& label)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    int indx = m_tabWidget->addTab(scrollArea,
                                       label);
    return indx;
}


/**
 * Called when floating status changes.
 * @param status
 *   New floating status.
 */
void 
BrainBrowserWindowOrientedToolBox::floatingStatusChanged(bool /*status*/)
{
    QString title = m_toolBoxTitle;
    setWindowTitle(title);
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
BrainBrowserWindowOrientedToolBox::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainBrowserWindowOrientedToolBox",
                                            1);
    
    AString tabName;
    const int tabIndex = m_tabWidget->currentIndex();
    if ((tabIndex >= 0) 
        && tabIndex < m_tabWidget->count()) {
        tabName = m_tabWidget->tabText(tabIndex);
    }
    sceneClass->addString("selectedTabName",
                          tabName);
    
    /*
     * Save current widget size
     */
    QWidget* childWidget = m_tabWidget->currentWidget();
    if (childWidget != NULL) {
        SceneWindowGeometry swg(childWidget,
                                this);
        sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                             "childWidget"));
    }
    
    /*
     * Save the toolbox
     */
    SceneWindowGeometry swg(this,
                            GuiManager::get()->getBrowserWindowByWindowIndex(this->m_browserWindowIndex));
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
    /**
     * Save the size when visible BUT NOT floating
     */
    if (isFloating() == false) {
        sceneClass->addInteger("toolboxWidth", width());
        sceneClass->addInteger("toolboxHeight", height());
    }
    
    /*
     * Save controllers in the toolbox
     */
    if (m_borderSelectionViewController != NULL) {
        sceneClass->addClass(m_borderSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_borderSelectionViewController"));
    }
    if (m_fiberOrientationViewController != NULL) {
        sceneClass->addClass(m_fiberOrientationViewController->saveToScene(sceneAttributes,
                                                     "m_fiberOrientationViewController"));
    }
    if (m_fociSelectionViewController != NULL) {
        sceneClass->addClass(m_fociSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_fociSelectionViewController"));
    }
    if (m_labelSelectionViewController != NULL) {
        sceneClass->addClass(m_labelSelectionViewController->saveToScene(sceneAttributes,
                                                     "m_labelSelectionViewController"));
    }
    if (m_fiberTrajectorySelectionViewController != NULL) {
        sceneClass->addClass(m_fiberTrajectorySelectionViewController->saveToScene(sceneAttributes,
                                                     "m_fiberTrajectorySelectionViewController"));
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
BrainBrowserWindowOrientedToolBox::restoreFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const AString tabName = sceneClass->getStringValue("selectedTabName",
                                                       "");
    for (int32_t i = 0; i < m_tabWidget->count(); i++) {
        if (m_tabWidget->tabText(i) == tabName) {
            m_tabWidget->setCurrentIndex(i);
            break;
        }
    }
    
    /*
     * Save controllers in the toolbox
     */
    if (m_borderSelectionViewController != NULL) {
        m_borderSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_borderSelectionViewController"));
    }
    if (m_fiberOrientationViewController != NULL) {
        m_fiberOrientationViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_fiberOrientationViewController"));
    }
    if (m_fociSelectionViewController != NULL) {
        m_fociSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_fociSelectionViewController"));
    }
    if (m_labelSelectionViewController != NULL) {
        m_labelSelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_labelSelectionViewController"));
    }
    if (m_fiberTrajectorySelectionViewController != NULL) {
        m_fiberTrajectorySelectionViewController->restoreFromScene(sceneAttributes,
                                                          sceneClass->getClass("m_fiberTrajectorySelectionViewController"));
    }
    
    /*
     * Restore current widget size
     */
    QWidget* childWidget = m_tabWidget->currentWidget();
    QSize childMinSize;
    QSize childSize;
    if (childWidget != NULL) {
        childMinSize = childWidget->minimumSize();
        SceneWindowGeometry swg(childWidget,
                                this);
        swg.restoreFromScene(sceneAttributes,
                             sceneClass->getClass("childWidget"));
        childSize = childWidget->size();
    }
    
    if (isFloating() && isVisible()) {
        SceneWindowGeometry swg(this,
                                GuiManager::get()->getBrowserWindowByWindowIndex(this->m_browserWindowIndex));
        swg.restoreFromScene(sceneAttributes,
                             sceneClass->getClass("geometry"));
    }
    else {
        /*
         * From http://stackoverflow.com/questions/2722939/c-resize-a-docked-qt-qdockwidget-programmatically
         *
         * Set the minimum and maximum sizes and restore them later.
         * Trying to restore them immediately does not work.  So, as
         * explained in the link above, set the minimum and maximum
         * sizes to that the toolbox is the correct size and then use
         * a timer to restore the correct values for the minimum and
         * maximum sizes after a little delay.
         */
        const int w = sceneClass->getIntegerValue("toolboxWidth", -1);
        const int h = sceneClass->getIntegerValue("toolboxHeight", -1);
        if ((w > 0) && (h > 0)) {
            m_minimumSizeAfterSceneRestored = minimumSize();
            m_maximumSizeAfterSceneRestored = maximumSize();
            
            setMaximumWidth(w);
            setMaximumHeight(h);
            setMinimumWidth(w);
            setMinimumHeight(h);

            QTimer::singleShot(1000,  // 1000 ms => 1 second
                               this,
                               SLOT(restoreMinimumAndMaximumSizesAfterSceneRestored()));
        }
    }
}

/**
 * This slot is called when restoring a scene
 */
void
BrainBrowserWindowOrientedToolBox::restoreMinimumAndMaximumSizesAfterSceneRestored()
{
    setMinimumSize(m_minimumSizeAfterSceneRestored);
    setMaximumSize(m_maximumSizeAfterSceneRestored);
}


/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
BrainBrowserWindowOrientedToolBox::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();

        Brain* brain = GuiManager::get()->getBrain();
        
        /*
         * Determine types of data this is loaded
         */
        bool haveBorders    = false;
        bool haveCiftiMatrix = false;
        bool haveDataSeries = false;
        bool haveFibers     = false;
        bool haveFoci       = false;
        bool haveLabels     = false;
        bool haveSurfaces   = false;
        bool haveTraj       = false;
        bool haveVolumes    = false;
        
        std::vector<CaretDataFile*> allDataFiles;
        brain->getAllDataFiles(allDataFiles);
        for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
             iter != allDataFiles.end();
             iter++) {
            const CaretDataFile* caretDataFile = *iter;
            const DataFileTypeEnum::Enum dataFileType = caretDataFile->getDataFileType();
            switch (dataFileType) {
                case DataFileTypeEnum::BORDER:
                    haveBorders = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                    haveCiftiMatrix = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    haveLabels = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                    haveCiftiMatrix = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                    haveDataSeries = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                    haveFibers = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                    haveTraj = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                    haveCiftiMatrix = true;
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                    haveCiftiMatrix = true;
                    break;
                case DataFileTypeEnum::FOCI:
                    haveFoci = true;
                    break;
                case DataFileTypeEnum::LABEL:
                    haveLabels = true;
                    break;
                case DataFileTypeEnum::METRIC:
                    break;
                case DataFileTypeEnum::PALETTE:
                    break;
                case DataFileTypeEnum::RGBA:
                    break;
                case DataFileTypeEnum::SCENE:
                    break;
                case DataFileTypeEnum::SPECIFICATION:
                    break;
                case DataFileTypeEnum::SURFACE:
                    haveSurfaces = true;
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    break;
                case DataFileTypeEnum::VOLUME:
                    haveVolumes = true;
                    break;
            }
        }
        
        /*
         * Enable surface volume outline only if have both surfaces and volumes
         * loaded and model being viewed is allows drawing of volume surface
         * outline.
         */
        bool enableVolumeSurfaceOutline = false;
        EventBrowserWindowContentGet browserContentEvent(m_browserWindowIndex);
        EventManager::get()->sendEvent(browserContentEvent.getPointer());
        const int32_t numItemsInWindow = browserContentEvent.getNumberOfItemsToDraw();
        if (numItemsInWindow == 1) {
            BrowserTabContent* windowContent = browserContentEvent.getTabContentToDraw(0);
            if (windowContent != NULL) {
                switch (windowContent->getSelectedModelType()) {
                    case ModelTypeEnum::MODEL_TYPE_INVALID:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                        enableVolumeSurfaceOutline = (haveSurfaces
                                                      & haveVolumes);
                        break;
                    case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                        enableVolumeSurfaceOutline = (haveSurfaces
                                                      & haveVolumes);
                        break;
                }
            }
        }
        
        /*
         * Enable/disable Tabs based upon data that is loaded
         * NOTE: Order is important so that overlay tab is 
         * automatically selected.
         */
        if (m_connectivityTabIndex >= 0) m_tabWidget->setTabEnabled(m_connectivityTabIndex, haveCiftiMatrix);
        if (m_timeSeriesTabIndex >= 0) m_tabWidget->setTabEnabled(m_timeSeriesTabIndex, haveDataSeries);
        if (m_volumeSurfaceOutlineTabIndex >= 0) m_tabWidget->setTabEnabled(m_volumeSurfaceOutlineTabIndex, enableVolumeSurfaceOutline);
        
        if (m_borderTabIndex >= 0) m_tabWidget->setTabEnabled(m_borderTabIndex, haveBorders);
        if (m_fiberOrientationTabIndex >= 0) m_tabWidget->setTabEnabled(m_fiberOrientationTabIndex, haveFibers);
        if (m_fiberTrajectoryTabIndex >= 0) m_tabWidget->setTabEnabled(m_fiberTrajectoryTabIndex, haveTraj);
        if (m_fociTabIndex >= 0) m_tabWidget->setTabEnabled(m_fociTabIndex, haveFoci);
        if (m_labelTabIndex >= 0) m_tabWidget->setTabEnabled(m_labelTabIndex, haveLabels);
        
        //ensure overlays is always enabled if it exists, and do it after everything else just in case some uninitialized tab index clobbers it above
        if (m_overlayTabIndex >= 0) m_tabWidget->setTabEnabled(m_overlayTabIndex, true);
        
        int curTab = m_tabWidget->currentIndex();
        if (!m_tabWidget->isTabEnabled(curTab))
        {
            for (int i = 0; i < m_tabWidget->count(); ++i)
            {
                if (m_tabWidget->isTabEnabled(i))
                {
                    m_tabWidget->setTabEnabled(i, true);
                    break;
                }
            }
        }
    }
    else {
    }
}


