
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

#define __BRAIN_BROWSER_WINDOW_COMBO_BOX_DECLARE__
#include "BrainBrowserWindowComboBox.h"
#undef __BRAIN_BROWSER_WINDOW_COMBO_BOX_DECLARE__

#include <QComboBox>

#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "GuiManager.h"

using namespace caret;
    
/**
 * \class caret::BrainBrowserWindowComboBox 
 * \brief Combo box for selection of browser windows
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param style
 *     Style of combo box content
 * @param parent
 *     Parent of this combo box
 */
BrainBrowserWindowComboBox::BrainBrowserWindowComboBox(const Style style,
                                                       QObject* parent)
: WuQWidget(parent),
m_style(style)
{
    m_comboBox = new QComboBox();
    QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(comboBoxIndexSelected(int)));
}

/**
 * Destructor.
 */
BrainBrowserWindowComboBox::~BrainBrowserWindowComboBox()
{
    
}

/**
 * Called when the user selects an item from the combo box.
 * @param indx
 *    Index of item selected.
 */
void
BrainBrowserWindowComboBox::comboBoxIndexSelected(int /*indx*/)
{
    BrainBrowserWindow* bbw = getSelectedBrowserWindow();
    if (bbw != NULL) {
        emit browserWindowIndexSelected(bbw->getBrowserWindowIndex());
        emit browserWindowSelected(bbw);
    }
}

/**
 * Update the combo box.
 */
void
BrainBrowserWindowComboBox::updateComboBox()
{
    m_comboBox->blockSignals(true);
    
    std::vector<BrainBrowserWindow*> browserWindows =
       GuiManager::get()->getAllOpenBrainBrowserWindows();
    
    BrainBrowserWindow* selectedWindow = getSelectedBrowserWindow();
    
    m_comboBox->clear();
    
    int32_t defaultIndex = 0;
    
    const int32_t numWindows = static_cast<int32_t>(browserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        BrainBrowserWindow* bbw = browserWindows[i];
        const int32_t browserWindowIndex = bbw->getBrowserWindowIndex();
        switch (m_style) {
            case STYLE_NAME_AND_NUMBER:
                m_comboBox->addItem("Window " + QString::number(browserWindowIndex + 1));
                break;
            case STYLE_NUMBER:
                m_comboBox->addItem(QString::number(browserWindowIndex + 1));
                break;
        }
        m_comboBox->setItemData(i,
                                qVariantFromValue((void*)bbw));
        
        if (bbw == selectedWindow) {
            defaultIndex = i;
        }
    }
    
    if (defaultIndex < m_comboBox->count()) {
        m_comboBox->setCurrentIndex(defaultIndex);
    }
    
    m_comboBox->blockSignals(false);
}

/**
 * @return The widget inside this control. 
 */
QWidget*
BrainBrowserWindowComboBox::getWidget()
{
    return m_comboBox;
}

/**
 * Set the selected browser window using the browser window index.
 * If the index is invalid selection will not change.
 * 
 * @param browserWindowIndex
 *     Index of window.
 */
void
BrainBrowserWindowComboBox::setBrowserWindowByIndex(const int32_t browserWindowIndex)
{
    for (int32_t i = 0; i < m_comboBox->count(); i++) {
        void* pointer = m_comboBox->itemData(i).value<void*>();
        BrainBrowserWindow* bbw = (BrainBrowserWindow*)pointer;
        if (bbw->getBrowserWindowIndex() == browserWindowIndex) {
            m_comboBox->blockSignals(true);
            m_comboBox->setCurrentIndex(i);
            m_comboBox->blockSignals(false);
            return;
        }
    }
}

/**
 * Set the selected browser window.
 * If the window is invalid selection will not change.
 *
 * @param browserWindow
 *     The window.
 */
void
BrainBrowserWindowComboBox::setBrowserWindow(BrainBrowserWindow* browserWindow)
{
    CaretAssert(browserWindow);
    setBrowserWindowByIndex(browserWindow->getBrowserWindowIndex());
}

/**
 * @return Index of selected browser window.  A negative value
 * is returned if invalid.
 */
int32_t
BrainBrowserWindowComboBox::getSelectedBrowserWindowIndex() const
{
    int32_t indx = -1;
    
    BrainBrowserWindow* bbw = getSelectedBrowserWindow();
    if (bbw != NULL) {
        indx = bbw->getBrowserWindowIndex();
    }
    
    return indx;
}

/**
 * @return Selected browser window.  NULL is returned if invalid.
 */
BrainBrowserWindow*
BrainBrowserWindowComboBox::getSelectedBrowserWindow() const
{
    BrainBrowserWindow* bbw = NULL;
    
    const int32_t indx = m_comboBox->currentIndex();
    if ((indx >= 0)
        && (indx < m_comboBox->count())) {
        void* pointer = m_comboBox->itemData(indx).value<void*>();
        bbw = (BrainBrowserWindow*)pointer;
    }
    
    return bbw;
}

