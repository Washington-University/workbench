
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 */
BrainBrowserWindowComboBox::BrainBrowserWindowComboBox(QObject* parent)
: WuQWidget(parent)
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
        m_comboBox->addItem(QString::number(i + 1));
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

