
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PREFERENCES_GENERAL_WIDGET_DECLARE__
#include "PreferencesGeneralWidget.h"
#undef __PREFERENCES_GENERAL_WIDGET_DECLARE__

    
/**
 * \class caret::PreferencesGeneralWidget
 * \brief Widget for recent file properties in preferences
 * \ingroup GuiQt
 */
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#include "BrainOpenGLMediaDrawing.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "ImageFile.h"
#include "PreferencesDialog.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;

//Add GuiDarkLightThemeModeEnum.h and call functions in MacDarkTheme.h
//to test and see if it works
//Create a dark theme class that calls o/s specific functions with an "isSupported" method.
//Linux dark theme???

/**
 * Constructor.
 */
PreferencesGeneralWidget::PreferencesGeneralWidget(QWidget* parent)
: QWidget(parent)
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(2, 100);
    
    /*
     * Image texture minification filter
     */
    m_graphicsTextureMinificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
    m_graphicsTextureMinificationFilterEnumComboBox->setup<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    QObject::connect(m_graphicsTextureMinificationFilterEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesGeneralWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated);
    WuQtUtilities::setWordWrappedToolTip(m_graphicsTextureMinificationFilterEnumComboBox->getWidget(),
                                         GraphicsTextureMinificationFilterEnum::toToolTip());
    QLabel* minFilterLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                  "Image Minification Filter (Zoomed Out)",
                                                                  m_graphicsTextureMinificationFilterEnumComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(minFilterLabel,
                                         GraphicsTextureMinificationFilterEnum::toToolTip());


    QLabel* noteLabel = new QLabel("Note: Image Magnification/Minification Filters are NOT saved in the user's preferences.  "
                                   "Therefore, any desired changes to these selections must be made each time "
                                   "the application is started.");
    noteLabel->setWordWrap(true);
    
    /*
     * Layouts
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addWidget(noteLabel);
    layout->addStretch();
}

/**
 * Destructor.
 */
PreferencesGeneralWidget::~PreferencesGeneralWidget()
{
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesGeneralWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);

    const GraphicsTextureMinificationFilterEnum::Enum minFilter  = BrainOpenGLMediaDrawing::getTextureMinificationFilter();
    m_graphicsTextureMinificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>(minFilter);
}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesGeneralWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated()
{
    const GraphicsTextureMinificationFilterEnum::Enum minFilter = m_graphicsTextureMinificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    BrainOpenGLMediaDrawing::setTextureMinificationFilter(minFilter);
    updateGraphicsAndUserInterface();
}

/**
 * Update the graphics and the user interface
 */
void
PreferencesGeneralWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

