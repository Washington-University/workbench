
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ANNOTATION_DEPTH_WIDGET_DECLARE__
#include "AnnotationDepthWidget.h"
#undef __ANNOTATION_DEPTH_WIDGET_DECLARE__

#include <QAction>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationPolyhedron.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "BrowserTabContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "NumericTextFormatting.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "VolumeMappableInterface.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::AnnotationDepthWidget
 * \brief Widget for deleting annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The input mode
 * @param browserWindowIndex
 *    Index of browser window
 * @param parent
 *    The parent widget
 */
AnnotationDepthWidget::AnnotationDepthWidget(const UserInputModeEnum::Enum userInputMode,
                                             const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    const AString toolTipText("<html>"
                              "Depth, in millimeters, of polyhedron (controls number of slices polyhedron spans).  "
                              "Value may be negative or positive."
                              "</html>");
    QLabel* depthLabel = new QLabel("Depth");
    QLabel* mmLabel = new QLabel("(mm)");
    m_millimetersSpinBox = new QDoubleSpinBox();
    m_millimetersSpinBox->setMinimum(-500.0);
    m_millimetersSpinBox->setMaximum(500.0);
    m_millimetersSpinBox->setDecimals(2);
    m_millimetersSpinBox->setValue(3.0);
    m_millimetersSpinBox->setToolTip(toolTipText);
    QObject::connect(m_millimetersSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationDepthWidget::millimeterValueChanged);

    m_slicesLabel = new QLabel("Slices=x.x");

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(depthLabel, 0, Qt::AlignHCenter);
    layout->addWidget(mmLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_millimetersSpinBox, 0, Qt::AlignHCenter);
    layout->addWidget(m_slicesLabel, 0, Qt::AlignLeft);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationDepthWidget::~AnnotationDepthWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
AnnotationDepthWidget::receiveEvent(Event* /*event*/)
{
}

/**
 * Update the content.
 */
void
AnnotationDepthWidget::updateContent(std::vector<Annotation*>& annotationsIn)
{
    m_annotations.clear();

    /*
     * This depth DOES NOT affect the annotation being drawn
     */
    AnnotationPolyhedron* firstPolyhedron(NULL);
    for (auto& ann : annotationsIn) {
        CaretAssert(ann);
        AnnotationPolyhedron* ap(ann->castToPolyhedron());
        if (ap != NULL) {
            if (firstPolyhedron == NULL) {
                firstPolyhedron = ap;
            }
            m_annotations.push_back(ap);
        }
    }
    
    if (firstPolyhedron != NULL) {
        const float sliceThickness(getSliceThickness());
        m_millimetersSpinBox->setSingleStep(sliceThickness / 10.0);
        QSignalBlocker millimetersBlocker(m_millimetersSpinBox);
        const float depthValue(firstPolyhedron->getDepthMillimeters());
        m_millimetersSpinBox->setValue(depthValue);
    }
    
    setEnabled(firstPolyhedron != NULL);
    
    updateSlicesLabel();
}

/**
 * @return First polyhedron of all polyhedrons selected
 */
AnnotationPolyhedron*
AnnotationDepthWidget::getFirstPolyhedron() const
{
    AnnotationPolyhedron* polyhedron(NULL);
    
    if ( ! m_annotations.empty()) {
        CaretAssertVectorIndex(m_annotations, 0);
        polyhedron = m_annotations[0];
    }
    
    return polyhedron;
}

/**
 * @return Slice thickness of volume in selected tab.  Returns 1.0 if no underlay volume in tab.
 */
float
AnnotationDepthWidget::getSliceThickness() const
{
    float sliceThicknessOut(1.0);
    const BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    if (bbw != NULL) {
        const BrowserTabContent* tabContent(bbw->getBrowserTabContent());
        if (tabContent != NULL) {
            const OverlaySet* overlaySet(tabContent->getOverlaySet());
            if (overlaySet != NULL) {
                const VolumeMappableInterface* volumeInterface(tabContent->getOverlaySet()->getUnderlayVolume());
                if (volumeInterface != NULL) {
                    sliceThicknessOut = volumeInterface->getMaximumVoxelSpacing();
                }
            }
        }
    }
    return sliceThicknessOut;
}


/**
 * Update content of slice label.
 */
void
AnnotationDepthWidget::updateSlicesLabel()
{
    AnnotationPolyhedron* firstPolyhedron(getFirstPolyhedron());
    if (firstPolyhedron != NULL) {
        const float numberOfSlices(firstPolyhedron->getDepthSlices(getSliceThickness()));
        const AString numSlicesText(AString::number(numberOfSlices, 'f', 1));
        m_slicesLabel->setText(numSlicesText
                               + " Slices");
    }
    else {
        m_slicesLabel->setText("");
    }
}

/**
 * Called when new sample depth value is changed
 * @param value
 *    New value
 */
void
AnnotationDepthWidget::millimeterValueChanged(double value)
{
    QSignalBlocker blocker(m_millimetersSpinBox);
    
    if ( ! m_annotations.empty()) {
        std::vector<Annotation*> anns(m_annotations.begin(),
                                      m_annotations.end());
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModePolyhedronDepth(value,
                                            anns);
        
        AString errorMessage;
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
        if ( ! annotationManager->applyCommand(undoCommand,
                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
    }
    
    updateSlicesLabel();

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
