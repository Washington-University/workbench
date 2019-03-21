
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __MOVIE_RECORDING_DIALOG_DECLARE__
#include "MovieRecordingDialog.h"
#undef __MOVIE_RECORDING_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrainBrowserWindowComboBox.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CursorDisplayScoped.h"
#include "Event.h"
#include "EventManager.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventMovieManualModeRecording.h"
#include "EventUserInterfaceUpdate.h"
#include "EnumComboBoxTemplate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "MovieRecorder.h"
#include "MovieRecorderVideoFormatTypeEnum.h"
#include "SessionManager.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::MovieRecordingDialog 
 * \brief Dialog for control of movie recording and creation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MovieRecordingDialog::MovieRecordingDialog(QWidget* parent)
: WuQDialogNonModal("Movie Recording",
                    parent)
{
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createMainWidget(), "Main");
    tabWidget->addTab(createSettingsWidget(), "Settings");
    
    setCentralWidget(tabWidget,
                     SCROLL_AREA_NEVER);

    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_MOVIE_RECORDING_DIALOG_UPDATE);
    
    setApplyButtonText("");
    disableAutoDefaultForAllPushButtons();

    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    CaretAssert(movieRecorder);
}

/**
 * Destructor.
 */
MovieRecordingDialog::~MovieRecordingDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when close event is issuedf
 *
 * @param event
 *    The close event
 */
void
MovieRecordingDialog::closeEvent(QCloseEvent* event)
{
    s_previousDialogGeometry = saveGeometry();
    
    WuQDialogNonModal::closeEvent(event);
}

void
MovieRecordingDialog::restorePositionAndSize()
{
    if ( ! s_previousDialogGeometry.isEmpty()) {
        restoreGeometry(s_previousDialogGeometry);
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
MovieRecordingDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MOVIE_RECORDING_DIALOG_UPDATE) {
        updateDialog();
        event->setEventProcessed();
    }
}

/**
 * May be called to update the dialog's content.
 */
void
MovieRecordingDialog::updateDialog()
{
    const MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    CaretAssert(movieRecorder);
    
    m_windowComboBox->updateComboBox();
    m_windowComboBox->setBrowserWindowByIndex(movieRecorder->getRecordingWindowIndex());
    
    bool manualRecordingEnabledFlag(false);
    switch (movieRecorder->getRecordingMode()) {
        case MovieRecorderModeEnum::MANUAL:
            m_recordingManualRadioButton->setChecked(true);
            manualRecordingEnabledFlag = true;
            break;
        case MovieRecorderModeEnum::AUTOMATIC:
            m_recordingAutomaticRadioButton->setChecked(true);
            break;
    }
    
    const MovieRecorderVideoResolutionTypeEnum::Enum resType = movieRecorder->getVideoResolutionType();
    m_movieRecorderVideoResolutionTypeEnumComboBox->setSelectedItem<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>(resType);

    const MovieRecorderCaptureRegionTypeEnum::Enum captureType = movieRecorder->getCaptureRegionType();
    m_movieRecorderCaptureRegionTypeComboBox->setSelectedItem<MovieRecorderCaptureRegionTypeEnum, MovieRecorderCaptureRegionTypeEnum::Enum>(captureType);

    updateManualRecordingOptions();
    updateCustomWidthHeightSpinBoxes();
    updateFrameCountLabel();
}

/**
 * Update the manual recording options
 */
void
MovieRecordingDialog::updateManualRecordingOptions()
{
    const MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    CaretAssert(movieRecorder);
    
    bool manualRecordingEnabledFlag(false);
    switch (movieRecorder->getRecordingMode()) {
        case MovieRecorderModeEnum::MANUAL:
            manualRecordingEnabledFlag = true;
            break;
        case MovieRecorderModeEnum::AUTOMATIC:
            break;
    }
    
    m_manualCaptureToolButton->setEnabled(manualRecordingEnabledFlag);
}

/**
 * Update the custom width/height spin boxes
 */
void
MovieRecordingDialog::updateCustomWidthHeightSpinBoxes()
{
    const MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    CaretAssert(movieRecorder);

    int32_t customWidth(0);
    int32_t customHeight(0);
    movieRecorder->getCustomWidthAndHeight(customWidth,
                                           customHeight);
    QSignalBlocker widthBlocker(m_customWidthSpinBox);
    m_customWidthSpinBox->setValue(customWidth);
    QSignalBlocker heightBlocker(m_customHeightSpinBox);
    m_customHeightSpinBox->setValue(customHeight);
    QSignalBlocker frameRateBlocker(m_frameRateSpinBox);
    m_frameRateSpinBox->setValue(movieRecorder->getFramesRate());
    m_removeTemporaryImagesAfterMovieCreationCheckBox->setChecked(movieRecorder->isRemoveTemporaryImagesAfterMovieCreation());
    
    const bool customSpinBoxesEnabled(movieRecorder->getVideoResolutionType() == MovieRecorderVideoResolutionTypeEnum::CUSTOM);
    m_customWidthSpinBox->setEnabled(customSpinBoxesEnabled);
    m_customHeightSpinBox->setEnabled(customSpinBoxesEnabled);
}

/**
 * Update the frame count label
 */
void
MovieRecordingDialog::updateFrameCountLabel()
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    const int32_t numberOfFrames = movieRecorder->getNumberOfFrames();
    m_frameCountNumberLabel->setNum(numberOfFrames);
    
    const int32_t timeSeconds = (movieRecorder->getNumberOfFrames()
                                 / movieRecorder->getFramesRate());
    QTime qtime(0, 0, 0, 0);
    QTime timeForString = qtime.addSecs(timeSeconds);
    m_lengthLabel->setText(timeForString.toString("h:mm:ss"));
    
    /*
     * Do not allow user to change image size once an image has been captured
     */
    m_movieRecorderVideoResolutionTypeEnumComboBox->getWidget()->setEnabled(numberOfFrames <= 0);
}

/**
 * Called when window index is changed
 *
 * @param windowIndex
 *     Index of window for recording
 */
void
MovieRecordingDialog::windowIndexSelected(const int32_t windowIndex)
{
    SessionManager::get()->getMovieRecorder()->setRecordingWindowIndex(windowIndex);
}

/**
 * Called when video resolution type is changed
 */
void
MovieRecordingDialog::movieRecorderVideoResolutionTypeEnumComboBoxItemActivated()
{
    const MovieRecorderVideoResolutionTypeEnum::Enum dimType = m_movieRecorderVideoResolutionTypeEnumComboBox->getSelectedItem<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>();
    SessionManager::get()->getMovieRecorder()->setVideoResolutionType(dimType);
    updateCustomWidthHeightSpinBoxes();
}

/**
 * Called when capture region type is changed
 */
void
MovieRecordingDialog::movieRecorderCaptureRegionTypeComboBoxActivated()
{
    const MovieRecorderCaptureRegionTypeEnum::Enum regionType = m_movieRecorderCaptureRegionTypeComboBox->getSelectedItem<MovieRecorderCaptureRegionTypeEnum, MovieRecorderCaptureRegionTypeEnum::Enum>();
    SessionManager::get()->getMovieRecorder()->setCaptureRegionType(regionType);
}

/**
* Set the selected browser window to the browser window with the
* given index.
* @param browserWindowIndex
*    Index of browser window.
*/
void
MovieRecordingDialog::setBrowserWindowIndex(const int32_t browserWindowIndex)
{
    m_windowComboBox->setBrowserWindowByIndex(browserWindowIndex);
    windowIndexSelected(browserWindowIndex);
}

/**
 * @param Called when custom width spin box value changed
 *
 * @param width
 *     New custom width
 */
void
MovieRecordingDialog::customWidthSpinBoxValueChanged(int width)
{
    SessionManager::get()->getMovieRecorder()->setCustomWidthAndHeight(width,
                                                                       m_customHeightSpinBox->value());
}

/**
 * @param Called when custom height spin box value changed
 *
 * @param height
 *     New custom height
 */
void
MovieRecordingDialog::customHeightSpinBoxValueChanged(int height)
{
    SessionManager::get()->getMovieRecorder()->setCustomWidthAndHeight(m_customWidthSpinBox->value(),
                                                                       height);
}

/**
 * @param Called when frame rate spin box value changed
 *
 * @param frameRate
 *     New frame rate
 */
void
MovieRecordingDialog::frameRateSpinBoxValueChanged(int frameRate)
{
    SessionManager::get()->getMovieRecorder()->setFramesRate(frameRate);
}

/**
 * @param Called when remove temporary images checkbox is clicked
 *
 * @param checked
 *     New checked status
 */
void
MovieRecordingDialog::removeTemporaryImagesCheckBoxClicked(bool checked)
{
    if ( ! checked) {
        const QString text("If this is deselected, additional movies may contain images from previous movies.");
        if ( ! WuQMessageBox::warningOkCancel(m_removeTemporaryImagesAfterMovieCreationCheckBox,
                                              text)) {
            checked = true;
        }
    }
    
    SessionManager::get()->getMovieRecorder()->setRemoveTemporaryImagesAfterMovieCreation(checked);
    m_removeTemporaryImagesAfterMovieCreationCheckBox->setChecked(checked);
}

/**
 * Called when recording mode button is clicked
 *
 * @param button
 *     Button that was clicked
 */
void
MovieRecordingDialog::recordingModeRadioButtonClicked(QAbstractButton* button)
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    CaretAssert(movieRecorder);
    
    if (button == m_recordingAutomaticRadioButton) {
        movieRecorder->setRecordingMode(MovieRecorderModeEnum::AUTOMATIC);
    }
    else if (button == m_recordingManualRadioButton) {
        movieRecorder->setRecordingMode(MovieRecorderModeEnum::MANUAL);
    }
    else {
        CaretAssert(0);
    }
    
    updateManualRecordingOptions();
}

/**
 * Called when manual capture tool button is clicked
 */
void
MovieRecordingDialog::manualCaptureToolButtonClicked()
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    EventMovieManualModeRecording movieEvent(m_windowComboBox->getSelectedBrowserWindowIndex(),
                                             m_manualCaptureSecondsSpinBox->value());
    EventManager::get()->sendEvent(movieEvent.getPointer());
    updateFrameCountLabel();
}

/**
 * Called when manual capture seconds spin box value changed
 */
void
MovieRecordingDialog::manualCaptureSecondsSpinBoxValueChanged(int /*seconds*/)
{
}

/**
 * Called when create movie push button is clicked
 */
void
MovieRecordingDialog::createMoviePushButtonClicked()
{
    createMovie(m_createMoviePushButton);
}

/**
 * Create a movie from captured images.
 *
 * @param parent
 *     Parent widget for error message dialog.
 */
void
MovieRecordingDialog::createMovie(QWidget* parent)
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    QString currentFileName = movieRecorder->getMovieFileName();
    MovieRecorderVideoFormatTypeEnum::Enum formatType = MovieRecorderVideoFormatTypeEnum::MPEG;
    
    QString filters;
    QString selectedFilter = MovieRecorderVideoFormatTypeEnum::toFileDialogFilter(formatType);
    std::vector<MovieRecorderVideoFormatTypeEnum::Enum> formatEnums;
    MovieRecorderVideoFormatTypeEnum::getAllEnums(formatEnums);
    for (auto fe : formatEnums) {
        if ( ! filters.isEmpty()) {
            filters.append(";;");
        }
        filters.append(MovieRecorderVideoFormatTypeEnum::toFileDialogFilter(fe));
        
        if (currentFileName.endsWith(MovieRecorderVideoFormatTypeEnum::toFileNameExtensionNoDot(fe))) {
            formatType = fe;
            selectedFilter = MovieRecorderVideoFormatTypeEnum::toFileDialogFilter(fe);
        }
    }
    
    QString filename = CaretFileDialog::getSaveFileNameDialog(parent,
                                                              "Choose Movie File",
                                                              currentFileName,
                                                              filters,
                                                              &selectedFilter,
                                                              CaretFileDialog::DontConfirmOverwrite);
    
    if (filename.isEmpty()) {
        return;
    }

    if (selectedFilter.isEmpty()) {
        for (auto fe : formatEnums) {
            if (selectedFilter == MovieRecorderVideoFormatTypeEnum::toFileDialogFilter(fe)) {
                const QString ext = ("." + MovieRecorderVideoFormatTypeEnum::toFileNameExtensionNoDot(fe));
                if ( ! filename.endsWith(ext)) {
                    filename.append(ext);
                    break;
                }
            }
        }
    }
    
    FileInformation fileInfo(filename);
    const QString name(fileInfo.getCanonicalFilePath());
    if (fileInfo.exists()) {
        if ( ! fileInfo.remove()) {
            AString msg("Unable to remove movie file \""
                        + name
                        + "\"");
            WuQMessageBox::errorOk(parent,
                                   msg);
        }
    }
    
    AString errorMessage;
    const bool successFlag = movieRecorder->createMovie(filename,
                                                        errorMessage);
    if ( ! successFlag) {
        WuQMessageBox::errorOk(parent,
                               errorMessage);
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Called when reset push button is clicked
 */
void
MovieRecordingDialog::resetPushButtonClicked()
{
    if (WuQMessageBox::warningOkCancel(m_resetPushButton,
                                       "Reset (delete) recorded images for new movie")) {
        SessionManager::get()->getMovieRecorder()->removeTemporaryImages();
        updateDialog();
    }
}

/**
 * @return New instance of main widget
 */
QWidget*
MovieRecordingDialog::createMainWidget()
{
    QLabel* windowLabel = new QLabel("Record from Window:");
    m_windowComboBox = new BrainBrowserWindowComboBox(BrainBrowserWindowComboBox::STYLE_NUMBER,
                                                      this);
    m_windowComboBox->setToolTip("Sets window that is recorded");
    QObject::connect(m_windowComboBox, &BrainBrowserWindowComboBox::browserWindowIndexSelected,
                     this, &MovieRecordingDialog::windowIndexSelected);
    
    QLabel* regionLabel = new QLabel("Record Region:");
    m_movieRecorderCaptureRegionTypeComboBox = new EnumComboBoxTemplate(this);
    m_movieRecorderCaptureRegionTypeComboBox->getWidget()->setToolTip("Choose region that is capture for movie");
    m_movieRecorderCaptureRegionTypeComboBox->setup<MovieRecorderCaptureRegionTypeEnum,MovieRecorderCaptureRegionTypeEnum::Enum>();
    QObject::connect(m_movieRecorderCaptureRegionTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(movieRecorderCaptureRegionTypeComboBoxActivated()));

    QGroupBox* sourceGroupBox = new QGroupBox("Source");
    QGridLayout* sourceLayout = new QGridLayout(sourceGroupBox);
    sourceLayout->setColumnStretch(0, 0);
    sourceLayout->setColumnStretch(0, 1);
    sourceLayout->setColumnStretch(2, 100);
    int sourceRow(0);
    sourceLayout->addWidget(windowLabel, sourceRow, 0);
    sourceLayout->addWidget(m_windowComboBox->getWidget(), sourceRow, 1);
    sourceRow++;
    sourceLayout->addWidget(regionLabel, sourceRow, 0);
    sourceLayout->addWidget(m_movieRecorderCaptureRegionTypeComboBox->getWidget(), sourceRow, 1);
    sourceRow++;

    m_recordingAutomaticRadioButton  = new QRadioButton(MovieRecorderModeEnum::toGuiName(MovieRecorderModeEnum::AUTOMATIC));
    m_recordingAutomaticRadioButton->setToolTip("When selected, images are recorded as graphics updated");
    
    const QString recordButtonText("Record");
    m_recordingManualRadioButton = new QRadioButton(MovieRecorderModeEnum::toGuiName(MovieRecorderModeEnum::MANUAL));
    m_recordingManualRadioButton->setToolTip("When selected, images recorded when "
                                             + recordButtonText
                                             + " is clicked");
    QButtonGroup* recordingButtonGroup = new QButtonGroup(this);
    recordingButtonGroup->addButton(m_recordingAutomaticRadioButton);
    recordingButtonGroup->addButton(m_recordingManualRadioButton);
    QObject::connect(recordingButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                     this, &MovieRecordingDialog::recordingModeRadioButtonClicked);

    m_manualCaptureToolButton = new QToolButton();
    m_manualCaptureToolButton->setText(recordButtonText);
    m_manualCaptureToolButton->setToolTip("Duration of image displayed in movie");
    QObject::connect(m_manualCaptureToolButton, &QToolButton::clicked,
                     this, &MovieRecordingDialog::manualCaptureToolButtonClicked);

    m_manualCaptureSecondsSpinBox = new QSpinBox();
    m_manualCaptureSecondsSpinBox->setMinimum(1);
    m_manualCaptureSecondsSpinBox->setMaximum(100);
    m_manualCaptureSecondsSpinBox->setSingleStep(1);
    m_manualCaptureSecondsSpinBox->setSizePolicy(QSizePolicy::Fixed,
                                                 m_manualCaptureSecondsSpinBox->sizePolicy().verticalPolicy());
    QObject::connect(m_manualCaptureSecondsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MovieRecordingDialog::manualCaptureSecondsSpinBoxValueChanged);
    
    QLabel* captureSecondsLabel = new QLabel("seconds");
    
    QGroupBox* modeGroupBox = new QGroupBox("Recording Mode");
    QGridLayout* modeLayout = new QGridLayout(modeGroupBox);
    modeLayout->setColumnStretch(0, 0);
    modeLayout->setColumnStretch(1, 0);
    modeLayout->setColumnStretch(2, 0);
    modeLayout->setColumnStretch(3, 0);
    modeLayout->setColumnStretch(4, 100);
    int32_t modeRow(0);
    modeLayout->addWidget(m_recordingAutomaticRadioButton, modeRow, 0);
    modeRow++;
    modeLayout->addWidget(m_recordingManualRadioButton, modeRow, 0);
    modeLayout->addWidget(m_manualCaptureToolButton, modeRow, 1);
    modeLayout->addWidget(m_manualCaptureSecondsSpinBox, modeRow, 2);
    modeLayout->addWidget(captureSecondsLabel, modeRow, 3);
    modeRow++;
    
    m_createMoviePushButton = new QPushButton("Create Movie");
    m_createMoviePushButton->setToolTip("Create a movie file using images that have been recorded");
    QObject::connect(m_createMoviePushButton, &QPushButton::clicked, this,
                     &MovieRecordingDialog::createMoviePushButtonClicked);
    
    m_resetPushButton = new QPushButton("Reset");
    m_resetPushButton->setToolTip("Remove all recorded images to start a new movie");
    QObject::connect(m_resetPushButton, &QPushButton::clicked, this,
                     &MovieRecordingDialog::resetPushButtonClicked);
    
    QLabel* frameCountLabel = new QLabel("Frames: ");
    m_frameCountNumberLabel = new QLabel("0");
    
    QLabel* lengthLabel = new QLabel("Length: ");
    m_lengthLabel       = new QLabel("0");
    
    QGroupBox* movieFileGroupBox = new QGroupBox("Output Movie");
    QGridLayout* movieLayout = new QGridLayout(movieFileGroupBox);
    movieLayout->setColumnStretch(0, 0);
    movieLayout->setColumnStretch(1, 0);
    movieLayout->setColumnStretch(2, 0);
    movieLayout->setColumnStretch(3, 100);
    int32_t movieRow(0);
    movieLayout->addWidget(m_createMoviePushButton, movieRow, 0);
    movieLayout->addWidget(lengthLabel, movieRow, 1);
    movieLayout->addWidget(m_lengthLabel, movieRow, 2);
    movieRow++;
    movieLayout->addWidget(m_resetPushButton, movieRow, 0);
    movieLayout->addWidget(frameCountLabel, movieRow, 1);
    movieLayout->addWidget(m_frameCountNumberLabel, movieRow, 2);
    movieRow++;
    

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(sourceGroupBox);
    layout->addWidget(modeGroupBox);
    layout->addWidget(movieFileGroupBox);
    layout->addStretch();
    
    return widget;
}

/**
 * @return New instance of settings widget
 */
QWidget*
MovieRecordingDialog::createSettingsWidget()
{
    const int spinBoxWidth(100);
    
    QLabel* resolutionLabel = new QLabel("Resolution:");
    m_movieRecorderVideoResolutionTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_movieRecorderVideoResolutionTypeEnumComboBox->getWidget()->setToolTip("Choose width and height of movie");
    m_movieRecorderVideoResolutionTypeEnumComboBox->setup<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>();
    QObject::connect(m_movieRecorderVideoResolutionTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(movieRecorderVideoResolutionTypeEnumComboBoxItemActivated()));
    
    QLabel* customLabel = new QLabel("Custom Resolution:");
    m_customWidthSpinBox = new QSpinBox();
    m_customWidthSpinBox->setMinimum(1);
    m_customWidthSpinBox->setMaximum(500000);
    m_customWidthSpinBox->setSingleStep(1);
    QObject::connect(m_customWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MovieRecordingDialog::customWidthSpinBoxValueChanged);
    m_customWidthSpinBox->setFixedWidth(spinBoxWidth);
    
    m_customHeightSpinBox = new QSpinBox();
    m_customHeightSpinBox->setMinimum(1);
    m_customHeightSpinBox->setMaximum(500000);
    m_customHeightSpinBox->setSingleStep(1);
    m_customHeightSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_customHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MovieRecordingDialog::customHeightSpinBoxValueChanged);

    QLabel* frameRateLabel = new QLabel("Frames Per Second:");
    m_frameRateSpinBox = new QSpinBox();
    m_frameRateSpinBox->setToolTip("20 or 30 recommended");
    m_frameRateSpinBox->setMinimum(1);
    m_frameRateSpinBox->setMaximum(1000);
    m_frameRateSpinBox->setSingleStep(1);
    m_frameRateSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_frameRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MovieRecordingDialog::frameRateSpinBoxValueChanged);

    m_removeTemporaryImagesAfterMovieCreationCheckBox = new QCheckBox("Remove Temporary Images After Movie Creation");
    m_removeTemporaryImagesAfterMovieCreationCheckBox->setToolTip("Temporary images are removed after a movie is created");
    QObject::connect(m_removeTemporaryImagesAfterMovieCreationCheckBox, &QCheckBox::clicked,
                     this, &MovieRecordingDialog::removeTemporaryImagesCheckBoxClicked);
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setRowStretch(100, 100);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 100);
    int32_t row(0);
    gridLayout->addWidget(resolutionLabel, row, 0);
    gridLayout->addWidget(m_movieRecorderVideoResolutionTypeEnumComboBox->getWidget(),
                          row, 1, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(customLabel, row, 0);
    gridLayout->addWidget(m_customWidthSpinBox, row, 1);
    gridLayout->addWidget(m_customHeightSpinBox, row, 2);
    row++;
    gridLayout->addWidget(frameRateLabel, row, 0);
    gridLayout->addWidget(m_frameRateSpinBox,
                          row, 1, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_removeTemporaryImagesAfterMovieCreationCheckBox,
                          row, 0, 1, 3, Qt::AlignLeft);
    row++;

    return widget;
}

