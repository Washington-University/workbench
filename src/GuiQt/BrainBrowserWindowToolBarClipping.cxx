
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
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarClipping 
 * \brief Clipping group for toolbar.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of window
 * @param parentToolBar
 *     The parent toolbar
 * @param parentObjectNamePrefix
 *     Name of parent object
 */
BrainBrowserWindowToolBarClipping::BrainBrowserWindowToolBarClipping(const int32_t browserWindowIndex,
                                                                     BrainBrowserWindowToolBar* parentToolBar,
                                                                     const QString& parentObjectNamePrefix)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    const QString objectNamePrefix(parentObjectNamePrefix
                                   + ":Clipping");
    
    m_xClippingEnabledCheckBox = new QCheckBox("X");
    QObject::connect(m_xClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_xClippingEnabledCheckBox->setToolTip("Enable X clipping plane");
    m_xClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                              + ":EnableX");
    macroManager->addMacroSupportToObject(m_xClippingEnabledCheckBox);

    m_yClippingEnabledCheckBox = new QCheckBox("Y");
    QObject::connect(m_yClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_yClippingEnabledCheckBox->setToolTip("Enable Y clipping plane");
    m_yClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                              + ":EnableY");
    macroManager->addMacroSupportToObject(m_yClippingEnabledCheckBox);
    
    m_zClippingEnabledCheckBox = new QCheckBox("Z");
    QObject::connect(m_zClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_zClippingEnabledCheckBox->setToolTip("Enable Z clipping plane");
    m_zClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                              + ":EnableZ");
    macroManager->addMacroSupportToObject(m_zClippingEnabledCheckBox);
    
    m_surfaceClippingEnabledCheckBox = new QCheckBox("Surface");
    QObject::connect(m_surfaceClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_surfaceClippingEnabledCheckBox->setToolTip("Enable Clipping of Surface");
    m_surfaceClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                                    + ":EnableSurface");
    macroManager->addMacroSupportToObject(m_surfaceClippingEnabledCheckBox);
    
    m_volumeClippingEnabledCheckBox = new QCheckBox("Volume");
    QObject::connect(m_volumeClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_volumeClippingEnabledCheckBox->setToolTip("Enable Clipping of Volume Slices");
    m_volumeClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                              + ":EnableVolume");
    macroManager->addMacroSupportToObject(m_volumeClippingEnabledCheckBox);
    
    m_featuresClippingEnabledCheckBox = new QCheckBox("Features");
    QObject::connect(m_featuresClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingCheckBoxCheckStatusChanged()));
    m_featuresClippingEnabledCheckBox->setToolTip("Enable Clipping of Features");
    m_featuresClippingEnabledCheckBox->setObjectName(objectNamePrefix
                                              + ":EnableFeatures");
    macroManager->addMacroSupportToObject(m_featuresClippingEnabledCheckBox);
    
    QToolButton* setupToolButton = new QToolButton();
    setupToolButton->setText("Setup");
    QObject::connect(setupToolButton, SIGNAL(clicked()),
                     this, SLOT(setupClippingPushButtonClicked()));
    WuQtUtilities::setToolButtonStyleForQt5Mac(setupToolButton);
    setupToolButton->setToolTip("Display Clipping Planes Setup Dialog");
    setupToolButton->setObjectName(objectNamePrefix
                                   + ":ShowSetupDialog");
    macroManager->addMacroSupportToObject(setupToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setHorizontalSpacing(6);
    gridLayout->setVerticalSpacing(4);
    gridLayout->setContentsMargins(1, 1, 1, 1);
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
        
        this->updateGraphicsWindowAndYokedWindows();
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


