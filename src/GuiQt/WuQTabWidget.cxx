
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

#include <QGroupBox>
#include <QHBoxLayout>
#include <QTabBar>
#include <QStackedWidget>
#include <QVBoxLayout>

#define __WU_Q_TAB_WIDGET_DECLARE__
#include "WuQTabWidget.h"
#undef __WU_Q_TAB_WIDGET_DECLARE__

#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::WuQTabWidget 
 * \brief Replaces QTabWidget and allows the tab to be aligned. 
 *
 * On some platforms, the tabs are centered, and when the tab widget
 * is wide an in a scrollable area it is sometimes difficult to find the
 * tabs.  So, this tab widget allows the tab bar placed at a defined
 * alignment.
 */

/**
 * Constructor.
 * @param alignment
 *    Aligment of the tab.
 * @param parent
 *    Parent of this instance.
 */
WuQTabWidget::WuQTabWidget(const TabAlignment alignment,
                           QObject* parent)
: WuQWidget(parent)
{
    m_tabBar = new QTabBar();
    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(tabBarCurrentIndexChanged(int)));
    
    m_stackedWidget = new QStackedWidget();
    
    QHBoxLayout* tabBarLayout = new QHBoxLayout();
    Qt::Alignment tabBarAlignment = Qt::AlignLeft;
    switch (alignment) {
        case TAB_ALIGN_CENTER:
            tabBarAlignment = Qt::AlignHCenter;
            break;
        case TAB_ALIGN_LEFT:
            tabBarAlignment = Qt::AlignLeft;
            break;
        case TAB_ALIGN_RIGHT:
            tabBarAlignment = Qt::AlignRight;
            break;
    }
    tabBarLayout->addWidget(m_tabBar,
                            100,
                            tabBarAlignment);
    tabBarLayout->setMargin(0);
    
    QGroupBox* stackedWidgetGroupBox = new QGroupBox();
    QVBoxLayout* groupBoxLayout = new QVBoxLayout(stackedWidgetGroupBox);
    groupBoxLayout->addWidget(m_stackedWidget);
    
    m_widget = new QWidget();
    QVBoxLayout* verticalLayout = new QVBoxLayout(m_widget);
    QMargins margins = verticalLayout->contentsMargins();
    margins.setLeft(0);
    margins.setRight(0);
    verticalLayout->setContentsMargins(margins);
    verticalLayout->setSpacing(3);
    verticalLayout->addLayout(tabBarLayout,
                              0);
    verticalLayout->addWidget(stackedWidgetGroupBox,
                              0,
                              tabBarAlignment);
    verticalLayout->addStretch();
}

/**
 * Destructor.
 */
WuQTabWidget::~WuQTabWidget()
{
    
}

/**
 * @return The embedded widget.
 */
QWidget*
WuQTabWidget::getWidget()
{
    return m_widget;
}

/**
 * Adds a tab with the given page and label to the tab widget, and returns
 * the index of the tab in the tab bar.  If the tab's label contains an 
 * ampersand, the letter following the ampersand is used as a shortcut
 * for the tab, e.g. if the label is "Bro&wse" then Alt+W becomes a
 * shortcut which will move the focus to this tab.
 * 
 * @param page
 *    New page that is added (must not be NULL).
 * @param label
 *    Label displayed in the page's tab.
 */
void
WuQTabWidget::addTab(QWidget* page,
            const QString& label)
{
    m_tabBar->addTab(label);
    m_stackedWidget->addWidget(page);    
}

/**
 * Called when the tab bar changes the current widget.
 * @param index
 *    Index of selected widget.
 */
void
WuQTabWidget::tabBarCurrentIndexChanged(int index)
{
    setCurrentIndex(index);
    emit currentChanged(index);
}

/**
 * @return Returns the index position of the current tab page.
 * The current index is -1 if there is no current widget.
 */
int
WuQTabWidget::currentIndex() const
{
    return m_tabBar->currentIndex();
}

/**
 * @return Returns a pointer to the page currently being displayed by the 
 * tab dialog. The tab dialog does its best to make sure that this value 
 * is never 0 (but if you try hard enough, it can be).
 */
QWidget*
WuQTabWidget::currentWidget() const
{
    return m_stackedWidget->currentWidget();
}

//signals:
//void currentChanged(int index);

/**
 * Makes widget at the given index the current widget. The widget used must
 * be a page in this tab widget.
 */
void
WuQTabWidget::setCurrentIndex(int index)
{
    m_tabBar->blockSignals(true);
    m_tabBar->setCurrentIndex(index);
    m_tabBar->blockSignals(false);
    m_stackedWidget->setCurrentIndex(index);
}

/**
 * Makes widget the current widget. The widget used must be a page in 
 * this tab widget.
 */
void
WuQTabWidget::setCurrentWidget(QWidget* widget)
{
    const int indx = m_stackedWidget->indexOf(widget);
    if (indx > 0) {
        m_tabBar->blockSignals(true);
        m_tabBar->setCurrentIndex(indx);
        m_tabBar->blockSignals(false);
    }
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
WuQTabWidget::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "WuQTabWidget",
                                            1);

    AString tabName;
    const int32_t selectedTabIndex = currentIndex();
    if (selectedTabIndex >= 0) {
        tabName = m_tabBar->tabText(selectedTabIndex);
    }
    sceneClass->addString("selectedTabName",
                          tabName);
    
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
WuQTabWidget::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const AString tabName = sceneClass->getStringValue("selectedTabName");
    const int32_t numTabs = m_tabBar->count();
    for (int32_t i = 0; i < numTabs; i++) {
        if (m_tabBar->tabText(i) == tabName) {
            setCurrentIndex(i);
            break;
        }
    }
}


