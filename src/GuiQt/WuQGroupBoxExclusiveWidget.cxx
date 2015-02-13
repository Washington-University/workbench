
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

#define __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_DECLARE__
#include "WuQGroupBoxExclusiveWidget.h"
#undef __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_DECLARE__

#include <QBoxLayout>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQtUtilities.h"


using namespace caret;


    
/**
 * \class caret::WuQGroupBoxExclusiveWidget 
 * \brief A vertical layout with only one active item.
 * \ingroup GuiQt
 *
 * The items are layout out vertically inside an outline box
 * with each item containing a name an radio button for
 * mutual selection.  Sort of a list of group boxes where
 * only one item is enabled at a time.
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget.
 */
WuQGroupBoxExclusiveWidget::WuQGroupBoxExclusiveWidget(QWidget* parent)
: QWidget(parent)
{
    m_currentWidgetIndex = -1;
    m_widgetsGridLayout = new QGridLayout(this);
    m_widgetsGridLayout->setVerticalSpacing(10);
    m_widgetsGridLayout->setColumnMinimumWidth(0, 10);
    m_widgetsGridLayout->setColumnStretch(0,   0);
    m_widgetsGridLayout->setColumnStretch(1, 100);
    
    m_radioButtonGroup = new QButtonGroup(this);
    m_radioButtonGroup->setExclusive(true);
    QObject::connect(m_radioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(radioButtonClicked(QAbstractButton*)));
}

/**
 * Destructor.
 */
WuQGroupBoxExclusiveWidget::~WuQGroupBoxExclusiveWidget()
{
}

/**
 * Called when a group's radio button is clicked.  Will emit the
 * currentChanged() signal.
 *
 * @param button
 *    Button that was clicked.
 */
void
WuQGroupBoxExclusiveWidget::radioButtonClicked(QAbstractButton* button)
{
    int32_t itemIndex = -1;
    QRadioButton* selectedRadioButton = qobject_cast<QRadioButton*>(button);
    
    const int32_t num = count();
    for (int32_t i = 0; i < num; i++) {
        CaretAssertVectorIndex(m_widgetDatas, i);
        if (m_widgetDatas[i].m_radioButton == selectedRadioButton) {
            itemIndex = i;
            m_widgetDatas[i].m_widget->setEnabled(true);
        }
        else {
            m_widgetDatas[i].m_widget->setEnabled(false);
        }
    }
    
    
    if (itemIndex >= 0) {
        m_currentWidgetIndex = itemIndex;
        updateSelectionWithValidWidget();
        emit currentChanged(itemIndex);
    }
    else {
        m_currentWidgetIndex = -1;
    }
}

/**
 * Appends the given widget and returns its index.
 *
 * @param widget
 *    Widget that is added.
 * @param textLabel
 *    Text label displayed next to radio button.
 * @return
 *    Index of the widget.
 */
int32_t 
WuQGroupBoxExclusiveWidget::addWidget(QWidget* widget,
                                      const QString& textLabel)
{
    
    QRadioButton* radioButton = new QRadioButton(textLabel);
    m_radioButtonGroup->addButton(radioButton);
    
    QFrame* frameBox = new QFrame();
    frameBox->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    frameBox->setLineWidth(1);
    frameBox->setMidLineWidth(1);
    QHBoxLayout* frameBoxLayout = new QHBoxLayout(frameBox);
    frameBoxLayout->setContentsMargins(2, 2, 2, 2);
    frameBoxLayout->addWidget(widget, 100, Qt::AlignLeft);
    
    int row = m_widgetsGridLayout->rowCount();
    m_widgetsGridLayout->addWidget(radioButton,
                                   row, 0, 1, 2,
                                   Qt::AlignLeft);
    
    row++;
    m_widgetsGridLayout->addWidget(frameBox,
                                   row, 1);
    
    
    WidgetData widgetData;
    widgetData.m_frameBox    = frameBox;
    widgetData.m_radioButton = radioButton;
    widgetData.m_widget      = widget;
    
    m_widgetDatas.push_back(widgetData);
    
    updateSelectionWithValidWidget();

    return (m_widgetDatas.size() - 1);
}

/**
 * @return Number of widgets in container.
 */
int32_t
WuQGroupBoxExclusiveWidget::count() const
{
    return m_widgetDatas.size();
}

/**
 * @return Index of widget that is selected.  -1 if no widgets have been added.
 */
int32_t
WuQGroupBoxExclusiveWidget::currentIndex() const
{
    if (m_widgetDatas.empty()) {
        return -1;
    }
    
    CaretAssertVectorIndex(m_widgetDatas, m_currentWidgetIndex);
    return m_currentWidgetIndex;
}

void
WuQGroupBoxExclusiveWidget::updateSelectionWithValidWidget()
{
    if (m_widgetDatas.empty()) {
        m_currentWidgetIndex = -1;
        return;
    }
    
    if (m_currentWidgetIndex < 0) {
        m_currentWidgetIndex = 0;
    }
    else if (m_currentWidgetIndex >= count()) {
        m_currentWidgetIndex = count() - 1;
    }
    
    CaretAssertVectorIndex(m_widgetDatas, m_currentWidgetIndex);
    if ( ! isWidgetEnabled(m_currentWidgetIndex)) {
        m_currentWidgetIndex = -1;
        
        const int32_t num = count();
        for (int32_t i = 0; i < num; i++) {
            if (isWidgetEnabled(i)) {
                m_currentWidgetIndex = i;
                break;
            }
        }
    }
    
    if (m_currentWidgetIndex >= 0) {
        CaretAssertVectorIndex(m_widgetDatas, m_currentWidgetIndex);
        m_radioButtonGroup->blockSignals(true);
        m_widgetDatas[m_currentWidgetIndex].m_radioButton->setChecked(true);
        m_radioButtonGroup->blockSignals(false);
    }
    
    const int32_t num = count();
    for (int32_t i = 0; i < num; i++) {
        m_widgetDatas[i].m_widget->setEnabled(m_widgetDatas[i].m_radioButton->isChecked());
    }
}


/**
 * @return The current widget or NULL is no widgets have been added.
 */
QWidget*
WuQGroupBoxExclusiveWidget::currentWidget() const
{
    const int32_t index = currentIndex();
    if (index >= 0) {
        CaretAssertVectorIndex(m_widgetDatas, index);
        return m_widgetDatas[index].m_widget;
    }
    
    return NULL;
}

/**
 * @param widget
 *    Widget for its index.
 * @return 
 *    Index of the given widget or -1 if the widget was never added.
 */
int32_t
WuQGroupBoxExclusiveWidget::indexOf(QWidget* widget) const
{
    const int32_t numWidgets = count();
    for (int32_t i = 0; i < numWidgets; i++) {
        CaretAssertVectorIndex(m_widgetDatas, i);
        if (m_widgetDatas[i].m_widget == widget) {
            return i;
        }
    }
    
    return -1;
}

/**
 * @param index
 *    Index of widget that is returned.
 *
 * @return
 *    The widget at the given index or NULL if index is invalid.
 */
QWidget*
WuQGroupBoxExclusiveWidget::widget(int32_t index) const
{
    if ((index < 0)
        || (index >= count())) {
        const QString msg("Request with invalid widget index="
                          + QString::number(index));
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
        return NULL;
    }

    CaretAssertVectorIndex(m_widgetDatas, index);
    return m_widgetDatas[index].m_widget;
}

/**
 * Sets the current widget to the widget at the given index.
 *
 * @param index
 *     Index of the widget.
 */
void
WuQGroupBoxExclusiveWidget::setCurrentIndex(int32_t index)
{
    if ((index < 0)
        || (index >= count())) {
        const QString msg("Attemp to set invalid current widget index="
                       + QString::number(index));
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
        return;
    }
    
    m_currentWidgetIndex = index;
    
    updateSelectionWithValidWidget();
}

/**
 * Sets the current widget to the widget at the given widget.
 *
 * @param widget
 *     Widget that is selected.
 */
void
WuQGroupBoxExclusiveWidget::setCurrentWidget(QWidget* widget)
{
    if (m_widgetDatas.empty()) {
        return;
    }
    
    const int32_t index = indexOf(widget);
    if (index >= 0) {
        m_currentWidgetIndex = index;
    }
    else {
        const QString msg("Widget was not found");
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
    }

    updateSelectionWithValidWidget();
}

/**
 * Set the widget at given index enabled.
 *
 * @param index
 *     Index of the widget.
 * @param enabled
 *     New enabled status.
 */
void
WuQGroupBoxExclusiveWidget::setWidgetEnabled(const int32_t index,
                                             const bool enabled)
{
    if ((index < 0)
        || (index >= count())) {
        const QString msg("Attemp to set invalid widget enabled index="
                          + QString::number(index));
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
        return;
    }
    
    CaretAssertVectorIndex(m_widgetDatas, index);
    m_widgetDatas[index].m_radioButton->setEnabled(enabled);

    updateSelectionWithValidWidget();
}

/**
 * @return Is the widget at the given index enabled?
 *
 * @param index
 *     Index of the widget.
 */
bool
WuQGroupBoxExclusiveWidget::isWidgetEnabled(const int32_t index) const
{
    if ((index < 0)
        || (index >= count())) {
        const QString msg("Attemp to query invalid widget enabled index="
                          + QString::number(index));
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
        return false;
    }
    
    CaretAssertVectorIndex(m_widgetDatas, index);
    return m_widgetDatas[index].m_radioButton->isEnabled();
}
