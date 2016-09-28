
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

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>


#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_DECLARE__
#include "BrainBrowserWindowToolBarClipping.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CLIPPING_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarClipping 
 * \brief Clipping group for toolbar.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarClipping::BrainBrowserWindowToolBarClipping(const int32_t browserWindowIndex,
                                                                     BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar)
{
    m_xClippingEnabledCheckBox = new QCheckBox("X");
    QObject::connect(m_xClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));

    m_yClippingEnabledCheckBox = new QCheckBox("Y");
    QObject::connect(m_yClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    
    m_zClippingEnabledCheckBox = new QCheckBox("Z");
    QObject::connect(m_zClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    
    m_surfaceClippingEnabledCheckBox = new QCheckBox("Surface");
    QObject::connect(m_surfaceClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    
    m_volumeClippingEnabledCheckBox = new QCheckBox("Volume");
    QObject::connect(m_volumeClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    
    m_featuresClippingEnabledCheckBox = new QCheckBox("Features");
    QObject::connect(m_featuresClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    
    QToolButton* setupToolButton = new QToolButton();
    setupToolButton->setText("Setup");
    QObject::connect(setupToolButton, SIGNAL(clicked()),
                     this, SLOT(setupClippingPushButtonClicked()));
    WuQtUtilities::setToolButtonStyleForQt5Mac(setupToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setHorizontalSpacing(6);
    gridLayout->setVerticalSpacing(8);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(m_xClippingEnabledCheckBox, rowIndex, 0);
    gridLayout->addWidget(m_yClippingEnabledCheckBox, rowIndex, 1);
    gridLayout->addWidget(m_zClippingEnabledCheckBox, rowIndex, 2);
    rowIndex++;
    gridLayout->addWidget(m_featuresClippingEnabledCheckBox, rowIndex, 0, 1, 3);
    rowIndex++;
    gridLayout->addWidget(m_surfaceClippingEnabledCheckBox, rowIndex, 0, 1, 3);
    rowIndex++;
    gridLayout->addWidget(m_volumeClippingEnabledCheckBox, rowIndex, 0, 1, 3);
    rowIndex++;
    gridLayout->addWidget(setupToolButton, rowIndex, 0, 1, 3, Qt::AlignHCenter);
    
//    /*
//     * Layout:
//     *   Column 1: Clipping X, Y, Z
//     *   Column 2: Nothing but used as space
//     *   Column 3: Type of data clipped.
//     */
//    QGridLayout* checkboxGridLayout = new QGridLayout();
//    WuQtUtilities::setLayoutSpacingAndMargins(checkboxGridLayout, 4, 0);
//    checkboxGridLayout->setColumnMinimumWidth(1, 15);
//    checkboxGridLayout->setColumnStretch(0, 0);
//    checkboxGridLayout->setColumnStretch(0, 1);
//    checkboxGridLayout->setColumnStretch(0, 2);
//    checkboxGridLayout->addWidget(m_xClippingEnabledCheckBox, 0, 0);
//    checkboxGridLayout->addWidget(m_yClippingEnabledCheckBox, 1, 0);
//    checkboxGridLayout->addWidget(m_zClippingEnabledCheckBox, 2, 0);
//    checkboxGridLayout->addWidget(m_surfaceClippingEnabledCheckBox, 0, 2);
//    checkboxGridLayout->addWidget(m_volumeClippingEnabledCheckBox, 1, 2);
//    checkboxGridLayout->addWidget(m_featuresClippingEnabledCheckBox, 2, 2);
//    
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
//    layout->addLayout(checkboxGridLayout);
//    layout->addWidget(setupToolButton, 0, Qt::AlignHCenter);
//    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarClipping::~BrainBrowserWindowToolBarClipping()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarClipping::updateContent(BrowserTabContent* browserTabContent)
{
    bool xEnabled;
    bool yEnabled;
    bool zEnabled;
    bool surfaceEnabled;
    bool volumeEnabled;
    bool featuresEnabled;
    browserTabContent->getClippingPlaneEnabled(xEnabled,
                                               yEnabled,
                                               zEnabled,
                                               surfaceEnabled,
                                               volumeEnabled,
                                               featuresEnabled);
    
    m_xClippingEnabledCheckBox->setChecked(xEnabled);
    m_yClippingEnabledCheckBox->setChecked(yEnabled);
    m_zClippingEnabledCheckBox->setChecked(zEnabled);
    
    m_surfaceClippingEnabledCheckBox->setChecked(surfaceEnabled);
    m_volumeClippingEnabledCheckBox->setChecked(volumeEnabled);
    m_featuresClippingEnabledCheckBox->setChecked(featuresEnabled);
}

/**
 * Called when a clipping checkbox status is changed.
 */
void
BrainBrowserWindowToolBarClipping::clippingCheckBoxCheckStatusChanged()
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        browserTabContent->setClippingPlaneEnabled(m_xClippingEnabledCheckBox->isChecked(),
                                                   m_yClippingEnabledCheckBox->isChecked(),
                                                   m_zClippingEnabledCheckBox->isChecked(),
                                                   m_surfaceClippingEnabledCheckBox->isChecked(),
                                                   m_volumeClippingEnabledCheckBox->isChecked(),
                                                   m_featuresClippingEnabledCheckBox->isChecked());
        
        this->updateGraphicsWindow();
        this->updateOtherYokedWindows();
    }
}

/**
 * Called when the setup clipping push button is clicked.
 */
void
BrainBrowserWindowToolBarClipping::setupClippingPushButtonClicked()
{
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    GuiManager::get()->processShowClippingPlanesDialog(browserWindow);
}


